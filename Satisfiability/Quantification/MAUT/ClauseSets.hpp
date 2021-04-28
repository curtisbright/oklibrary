// Oliver Kullmann, 29.4.2021 (Swansea)
/* Copyright 2021 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*

  Clauses and clause-sets

*/

#ifndef CLAUSESETS_AyFIp7S1Y2
#define CLAUSESETS_AyFIp7S1Y2

#include <vector>
#include <algorithm>
#include <ostream>

#include "VarLit.hpp"

namespace MAUT {

  typedef std::vector<LIT> CL;
  bool valid(const CL& C) noexcept {
    if (C.empty()) return true;
    const auto b = C.begin(), e = C.end();
    return std::is_sorted(b, e, comp) and
      std::adjacent_find(b, e) == e and
      std::find(b, e, 0) == e and
      std::all_of(b, e, [](const LIT x){return valid(x);}) and
      std::adjacent_find(b, e, [](LIT x, LIT y){return y==-x;}) == e;
  }

  typedef std::vector<CL> CLS;
  bool valid(const CLS& F) noexcept {
    return std::all_of(F.begin(), F.end(), [](const CL& C){return valid(C);});
  }

  typedef CLS::size_type size_t;

  typedef CLS::const_pointer LitOcc;
  typedef std::vector<LitOcc> OccList;

  struct BasicStats {
    VAR n;
    size_t c, l;
  };
  std::ostream& operator <<(std::ostream& out, const BasicStats& S) {
    return out << S.n << " " << S.c << " " << S.l;
  }
  BasicStats count(const CLS& F) noexcept {
    assert(valid(F));
    BasicStats res{0,F.size(),0};
    for (const CL& C : F) {
      const auto s = C.size();
      if (s != 0) {
        res.n = std::max(var(C.back()), res.n);
        res.l += s;
      }
    }
    return res;
  }

  struct Occurrences {
    const VAR n;

    Occurrences(const VAR n) : n(n), occ(2*n+1) { assert(valid(n)); }

    const OccList& operator [](const LIT x) noexcept {
      return occ[tr(x)];
    }

    void enter(const CLS& F) {
      for (const CL& C : F) {
        const LitOcc p = &C;
        for (const LIT x : C) add(x, p);
      }
    }

  private :
    size_t tr(const LIT x) const noexcept {
      assert(valid(x) and var(x) <= n);
      if (x >= 0) return x;
      else return var(x) + n;
    }
    size_t trc(const LIT x) const {
      assert(valid(x));
      if (var(x) > n) {
        throw std::out_of_range("trc");
      }
      if (x >= 0) return x;
      else return var(x) + n;
    }
    void add(const LIT x, const LitOcc p) {
      occ[trc(x)].push_back(p);
    }
    std::vector<OccList> occ;
  };


  struct ClauseSet {
    CLS F;
    Occurrences occ;
    BasicStats s;
    explicit ClauseSet(const VAR n) : occ(n) {}
  };

}

#endif
