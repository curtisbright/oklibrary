// Oliver Kullmann, 28.1.2022 (Swansea)
/* Copyright 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*
  Tools for creating sampling sequences

    - scoped enum Smode
    - sampling_points(x_t,x_t, index_t, RandGen_t*, Smode) -> vec_t

    - fill_possibilities creates the mesh for scanning;
    - next_combination allows to run through all combinations.


TODOS:

1. Accuracy of sampling_points(l,r,M)

   For example

const auto res = sampling_points(0,10,9);
for (const auto x : res) std::cerr << " " << Wrap(x);
std::cerr << "\n";

   yields

 0 1.1111111111111111111 2.2222222222222222222 3.3333333333333333333 4.4444444444444444445 5.5555555555555555555 6.6666666666666666665 7.777777777777777778 8.888888888888888889 10

which actually seems correct:

assert(res[3] == *--F80it(3.3333333333333333335L));
assert(res[4] == 4.444444444444444444444444444444444L);
assert(res[5] == 5.555555555555555555555555555555555L);
assert(res[6] == 6.666666666666666666666666666666666L);

works out (where the above value "...5" for res[3] is obtained without using
the gcd, and thus indeed we need the gcd).

Is the current computation the best we can do?

*/

#ifndef SAMPLING_Msl8gWybWD
#define SAMPLING_Msl8gWybWD

#include <algorithm>
#include <vector>
#include <set>
#include <numeric>

#include <cassert>

#include <Transformers/Generators/Random/Numbers.hpp>
#include <Transformers/Generators/Random/FPDistributions.hpp>
#include <Transformers/Generators/Random/Algorithms.hpp>

#include "NumTypes.hpp"
#include "NumBasicFunctions.hpp"
#include "NumPrecise.hpp"


#include "OptTypes.hpp"

namespace Sampling {

  namespace FP = FloatingPoint;
  namespace OS = Optimisation;

  // SamplingMode
  enum class Smode {
    eq_un = 0, // equi-distant or uniform random
    boxed = 1, // kind of equi-distant uniform random
    eq = 2     // equi-distant (only)
  };

  /* M+1 Sampling points in the interval [l,r] uniformly:
      - if rg is the null-pointer, then using equi-distant sampling
      - otherwise use random uniform sampling or random boxed sampling,
        (or again equi-distant sampling, when forced).
     The resulting vector is sorted.
  */
  inline OS::vec_t sampling_points(const OS::x_t l, const OS::x_t r,
                                   const OS::index_t M,
                                   RandGen::RandGen_t* const rg = nullptr,
                                   Smode rsm = Smode::eq_un) {
    assert(l < r);
    assert(M < FP::P264m1);
    assert(rsm != Smode::boxed or rg);
    OS::vec_t res; res.reserve(M+1);
    if (not rg or rsm == Smode::eq) {
      if (M == 0) res.push_back(FP::midpoint(l,r));
      else {
        res.push_back(l);
        if (M >= 2) {
          for (OS::index_t i = 1; i < M; ++i) {
            const OS::index_t g = std::gcd(i,M);
            const OS::index_t ip = i/g, Mp = M/g;
            const OS::x_t delta = (r - l) / Mp;
            const OS::x_t x = FP::fma(ip, delta, l);
            /* Remark: this computation of x seems most accurate than e.g.
                 const OS::x_t x = FP::fma(ip * (r - l), 1.0L / Mp, l);
                 const OS::x_t x = FP::lerp(l, r, OS::x_t(ip) / Mp);
               seem to yield worse results.
            */
            assert(x <= r);
            res.push_back(x);
          }
        }
        res.push_back(r);
      }
    }
    else {
      if (rsm == Smode::eq_un) {
        const RandGen::Uniform80RangeI U(*rg, l, r);
        for (OS::index_t i = 0; i <= M; ++i) res.push_back(U());
        std::sort(res.begin(), res.end());
      }
      else {
        assert(rsm == Smode::boxed);
        assert(M < FP::P264m1-1);
        const auto boxes = sampling_points(l,r,M+1);
        assert(boxes.size() == M+2);
        for (OS::index_t i = 0; i <= M; ++i)
          res.push_back(RandGen::Uniform80Range(*rg, boxes[i], boxes[i+1])());
      }
    }
    assert(res.size() == M+1);
    assert(std::is_sorted(res.begin(), res.end()));
    return res;
  }



  /*
    For a vector x of float80 with assertions, of size N,
    and a vector I of intervals (of size N),
    create a vector res of size N of vec_t:

     - if x[i] is not asserted int, then res[i] = {x[i]};
       in the sequel x[i] is asserted int (correctly);
     - if x[i] is negative, then (-x[i]+1) equidistant points
       with first element I[i].l and last element I[i].r are
       put into x[i];
       in the sequel x[i] >= 0;
     - if x[i] has not "+", then in case rg = 0 we also create
       x[i]+1 equidistant points (as above), while otherwise
       x[i]+1 many random points in the closed interval given by
       I[i] are created (uniform distribution);
     - if x[i] has "+", then rg must not be 0, and x[i]+1 "boxed"
       random points are created (in the half-open subintervals).

    The special case x[i]=0 also has the same cases as above (in
    all cases res[i] consists of a single point {p}):
     - If isint=false, then p = 0;
       in the sequel isint=true.
     - If "x[i]==+0", that is, hasplus=true, then p is a random element
       of the half-open interval [l,r).
     - If "x[i]==-0", that is, x[i] has signbit, then p = (l+r)/2.
     - If "x[i]==0", that is, hasplus is false and signbit is false,
       then if rg==0, then p = (l+r)/2, otherwise p is a random element
       of the closed interval [l,r].
  */
  std::vector<OS::vec_t> fill_possibilities(const OS::evec_t& x,
      const OS::list_intervals_t& I, RandGen::RandGen_t* const rg = nullptr) {
    assert(OS::element(x, I));
    const auto N = x.size();
    assert(I.size() == N);
    std::vector<OS::vec_t> res; res.reserve(N);
    std::set<OS::index_t> groups;
    for (OS::index_t i = 0; i < N; ++i) {
      const OS::x_t xi = x[i].x, li = I[i].l, ri = I[i].r;
      if (not x[i].isint) res.push_back({xi});
      else if (li == ri) res.push_back({li});
      else {
        if (xi >= 0) {
          assert(FP::isUInt(xi));
          const FP::UInt_t M = xi;
          if (x[i].hasplus)
            res.push_back(sampling_points(li, ri, M, rg, Smode::boxed));
          else if (FP::signbit(xi))
            res.push_back(sampling_points(li, ri, M, nullptr));
          else
            res.push_back(sampling_points(li, ri, M, rg));
          if (x[i].hase0) {
            if (groups.contains(M))
              RandGen::shuffle(res.back().begin(), res.back().end(), *rg);
            else
              groups.insert(M);
          }
        }
        else {
          const OS::x_t nxi = -xi;
          assert(FP::isUInt(nxi));
          const FP::UInt_t M = nxi;
          res.push_back(sampling_points(li, ri, M));
          if (x[i].hase0) {
            if (groups.contains(M))
              RandGen::shuffle(res.back().begin(), res.back().end(), *rg);
            else
              groups.insert(M);
          }
        }
      }
    }
    assert(res.size() == N);
    return res;
  }


  template <class ITER>
  bool next_combination(
    std::vector<ITER>& current,
    const std::vector<ITER>& begin, const std::vector<ITER>& end) noexcept {
    const auto N = current.size();
    assert(begin.size() == N);
    assert(end.size() == N);
    for (OS::index_t i = 0; i < N; ++i) {
      assert(current[i] != end[i]);
      ++current[i];
      if (current[i] != end[i]) {
        for (OS::index_t j = 0; j < i; ++j) current[j] = begin[j];
        return true;
      }
    }
    return false;
  }


  template <class CON, typename ELEM>
  struct Lockstep {
    typedef CON container;
    typedef ELEM element;

    typedef std::vector<container> vcon_t;
    typedef std::vector<element*> vpelem_t;
    vcon_t content;
    vpelem_t delivery;
    Lockstep(const vcon_t V, const vpelem_t D) noexcept :
        content(V), delivery(D) {
      assert(not content.empty());
      assert(content.size() == delivery.size());
    }

    typedef typename container::const_iterator iterator;
    typedef std::vector<iterator> vit_t;
    struct It {
      vit_t vi;
      void operator ++() noexcept { for (auto& i : vi) ++i; }
      bool operator ==(const It& rhs) const noexcept {
        assert(not vi.empty() and not rhs.vi.empty());
        return vi.front() == rhs.vi.front();
      }
    };

    It begin() const {
      It vi; vi.vi.reserve(content.size());
      for (const container& c : content) vi.vi.push_back(c.begin());
      return vi;
    }
    It end() const noexcept {
      return {{content.front().end()}};
    }

    void update(const It& vi) const noexcept {
      assert(vi.vi.size() == delivery.size());
      for (OS::index_t i = 0; i < delivery.size(); ++i) {
        *delivery[i] = *vi.vi[i];
      }
    }

  };

}

#endif
