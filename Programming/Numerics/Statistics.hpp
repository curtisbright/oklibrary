// Oliver Kullmann, 8.11.2020 (Swansea)
/* Copyright 2020, 2021, 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/* Averages, variance etc.

   One-dimensional sequences (count, min, max, arithmetic mean, standard
   deviation and median):

    - class BStatsR<OUT> (reporting the basic statistics)
    - class StatsR<OUT> (derived class, also reporting the median)

    - class BasicStats<IN, OUT>
    - function median<OUT, V>(V v)
    - class StatsStors<IN, OUT> (keeps the data, provided median)
    - class FreqStats<IN, OUT> (also keeps the data, providng median, in the
      form of a frequency-table)

   Sequences of points (pairs of x/y-values):

    - class StatsPoints<FLOAT>

   Evaluating functions:

    - RandVal<IN, OUT> (statistically evaluating function-values for random
      arguments)

TODOS:

1. Improve documentation
  - For a general overview on statistics see
   https://en.wikipedia.org/wiki/Consistent_estimator
   https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
   https://en.wikipedia.org/wiki/Standard_deviation#Uncorrected_sample_standard_deviation

2. See todos in DifferentialEquations/Ode1.cpp.

*/

#ifndef STATISTICS_n7Ju3x1bcK
#define STATISTICS_n7Ju3x1bcK

#include <ostream>
#include <vector>
#include <algorithm>
#include <ostream>
#include <iomanip>
#include <array>
#include <functional>
#include <limits>
#include <numeric>
#include <ios>
#include <map>
#include <utility>

#include <cassert>
#include <cmath>
#include <cstdint>

#include <Numerics/FloatingPoint.hpp>
#include <Transformers/Generators/Random/Tests.hpp>
#include <Transformers/Generators/Random/FPDistributions.hpp>

namespace GenStats {

  // Basic-stats-report:
  template <typename OUT>
  struct BStatsR {
    typedef OUT output_t;
    typedef std::uint64_t count_t;

    count_t N;
    output_t min, amean, max, sdc;

    BStatsR() noexcept : N(0), min(0), amean(0), max(0), sdc(0) {}
    BStatsR(const count_t N, const output_t min, const output_t amean,
            const output_t max, const output_t sdc) noexcept
      : N(N), min(min), amean(amean), max(max), sdc(sdc) {}

    bool operator ==(const BStatsR&) const = default;
    auto operator <=>(const BStatsR&) const = default;

    friend std::ostream& operator <<(std::ostream& out, const BStatsR& s) {
      return out << s.N << " : " << s.min << " " << s.amean << " " << s.max
                 << "; " << s.sdc;
    }
  };
  template <typename OUT>
  struct StatsR : BStatsR<OUT> {
    typedef OUT output_t;
    typedef std::uint64_t count_t;

    output_t median;

    StatsR() noexcept : median(0) {}
    StatsR(const BStatsR<OUT> s, const output_t median) noexcept
      : BStatsR<OUT>(s), median(median) {}

    bool operator ==(const StatsR&) const = default;
    auto operator <=>(const StatsR&) const = default;

    friend std::ostream& operator <<(std::ostream& out, const StatsR& s) {
      return out << BStatsR(s) << " " << s.median;
    }
  };


  // Averages, variance, standard deviation,
  // simplest functionality, no storing, naive algorithm:
  template <typename IN, typename OUT>
  struct BasicStats {
    typedef IN input_t;
    typedef OUT output_t;
    typedef std::uint64_t count_t;
  private :
    count_t N_ = 0;
    input_t sum_ = 0;
    input_t sum_sq_ = 0;
    input_t min_ = std::numeric_limits<input_t>::max();
    input_t max_ = std::numeric_limits<input_t>::lowest();
  public :

    constexpr BasicStats() noexcept = default;
    constexpr BasicStats(const count_t N0, const input_t sum0,
      const input_t sum_sq0, const input_t min0, const input_t max0) noexcept :
    N_(N0), sum_(sum0), sum_sq_(sum_sq0), min_(min0), max_(max0) {
      assert(N_ == 0 or min_ <= max_);
      assert(sum_sq_ >= 0);
    }

    BasicStats& operator +=(const input_t x) noexcept {
      ++N_;
      sum_ += x;
      sum_sq_ += x*x;
      min_ = std::min(min_, x);
      max_ = std::max(max_, x);
      return *this;
    }
    BasicStats& operator +=(const BasicStats& s) noexcept {
      N_ += s.N_;
      sum_ += s.sum_;
      sum_sq_ += s.sum_sq_;
      min_ = std::min(min_, s.min_);
      max_ = std::max(max_, s.max_);
      return *this;
    }
    friend BasicStats operator +(const BasicStats& s1, const BasicStats& s2) {
      BasicStats res(s1);
      res += s2;
      return res;
    }

    constexpr count_t N() const noexcept { return N_; }
    constexpr input_t min() const noexcept { return min_; }
    constexpr input_t max() const noexcept { return max_; }
    constexpr input_t sum() const noexcept { return sum_; }
    constexpr input_t sum_sq() const noexcept { return sum_sq_; }

    constexpr output_t amean() const noexcept {
      if (N_ == 0) return 0;
      return output_t(sum_) / output_t(N_);
    }
    constexpr output_t var_population() const noexcept {
      if (N_ <= 1) return 0;
      const output_t s = sum_;
      const output_t n = N_;
      return (output_t(sum_sq_) - s*s / n) / n;
    }
    constexpr output_t var_unbiased() const noexcept {
      if (N_ <= 1) return 0;
      const output_t s = sum_;
      return (output_t(sum_sq_) - s*s / output_t(N_)) / output_t(N_-1);
    }
    constexpr output_t sd_population() const noexcept {
      return std::sqrt(var_population());
    }
    constexpr output_t sd_corrected() const noexcept {
      return std::sqrt(var_unbiased());
    }

    void simple_output(std::ostream& out) const {
      out << min() << " " << amean() << " " << max() << "; " <<
        sd_corrected();
    }

    typedef BStatsR<output_t> bstats_t;
    bstats_t extract() const noexcept {
      return {N_, output_t(min_), amean(), output_t(max_), sd_corrected()};
    }
    operator bstats_t() const noexcept {
      return extract();
    }

    friend bool operator ==(const BasicStats&,
                            const BasicStats&) noexcept = default;
    // Different from output-streaming for BStatsR<output_t>, where input_t for
    // min and max is used:
    friend std::ostream& operator <<(std::ostream& out, const BasicStats& s) {
      return out << s.N() << " : " << s.min() << " " << s.amean() << " "
                 << s.max() << "; " << s.sd_corrected();
    }
  };


  template <typename OUT, class V>
  OUT median(const V& v) noexcept {
    assert(std::is_sorted(v.begin(), v.end()));
    const auto N = v.size();
    if (N == 0) return 0;
    else if (N % 2 == 1) return v[(N-1)/2];
    else return std::midpoint<OUT>(v[N/2-1], v[N/2]);
  }

  /* StatsStore<IN, OUT>

     Now storing the data, and using the more precise calculation of
     statistics based on the summation of the square of the differences.

     - So there is an update-mechanism for the computation of the arithmetic
       mean, which is then used for the variance-related statistics.
     - The computation of amean() does not trigger an update, but the
       computation of sum_sqd() or any related function does.
     - If sum_sqd() or any of the functions involving it was called before,
       then that old value is kept (until further data is updated).
     - The median- and the ks-computation sorts the data (in ascending order).
     - If in the same expression there are various expressions involving an
       update (so for example output-streaming), then it seems needed to call
       the update-function upfront.
  */

  template <typename IN, typename OUT>
  struct StatsStore {
    typedef IN input_t;
    typedef OUT output_t;

    typedef std::uint64_t count_t;
    typedef std::vector<input_t> vec_t;

    StatsStore() noexcept {}

    StatsStore& operator +=(const input_t x) noexcept {
      data_.push_back(x); sorted=false; utd=false;
      ++N_;
      sum_ += x;
      if (x < min_) min_ = x;
      if (x > max_) max_ = x;
      return *this;
    }

    input_t min() const noexcept { return min_; }
    input_t max() const noexcept { return max_; }
    count_t N() const noexcept { return N_; }
    input_t sum() const noexcept { return sum_; }
    const vec_t& data() const noexcept { return data_; }

    output_t amean() const noexcept {
      if (utd) return am;
      else return output_t(sum_) / output_t(N_);
    }

    void update() const noexcept {
      if (utd) return;
      assert(N_ >= 1);
      camean(); csqd(); utd = true;
    }
    output_t sum_sqd() const noexcept {
      update(); return sqd;
    }

    output_t var_population() const noexcept {
      if (N_ <= 1) return 0;
      return sum_sqd() / output_t(N_);
    }
    output_t var_unbiased() const noexcept {
      if (N_ <= 1) return 0;
      return sum_sqd() / output_t(N_-1);
    }
    output_t sd_population() const noexcept {
      return std::sqrt(var_population());
    }
    output_t sd_corrected() const noexcept {
      return std::sqrt(var_unbiased());
    }

    output_t median() noexcept {
      if (not sorted) {
        std::sort(data_.begin(), data_.end());
        sorted = true;
      }
      return GenStats::median<output_t, vec_t>(data_);
    }

    typedef StatsR<output_t> stats_t;
    stats_t extract() noexcept {
      update();
      return {{N_, output_t(min_), am, output_t(max_), sd_corrected()},
          median()};
    }
    operator stats_t() noexcept {
      return extract();
    }

    RandGen::report_ks ks() {
      if (not sorted) {
        std::sort(data_.begin(), data_.end());
        sorted = true;
      }
      return RandGen::ks_P(data_);
    }

    int width = 30;
    void simple_output(std::ostream& out, const bool with_ks = false) {
      const auto sd = sd_corrected();
      const auto med = median();
      const auto w = std::setw(width);
      out << "N=" << N_ << "\n"
          << w << min() << w << amean() << w << max() << "\n"
          << w << "median" << w << med << "\n"
          << w << "sd" << w << sd << "\n";
      if (with_ks) {
        const auto K = ks();
        out << "ks:" << w << K.d << w << K.ks << "\n";
      }
    }

  private :

    void camean() const noexcept {
      am = output_t(sum_) / output_t(N_);
    }
    void csqd() const noexcept {
      sqd = 0;
      for (const input_t x : data_) {
        const output_t diff = output_t(x) - am;
        sqd += diff*diff;
      }
    }

    vec_t data_;
    count_t N_ = 0;
    input_t sum_ = 0;
    input_t min_ = std::numeric_limits<input_t>::max();
    input_t max_ = std::numeric_limits<input_t>::lowest();

    mutable bool sorted = true;
    mutable bool utd = true;
    mutable output_t am = 0;
    mutable output_t sqd = 0;
  };


  template <typename IN, typename OUT>
  struct FreqStats {
    typedef IN input_t;
    typedef OUT output_t;
    typedef std::uint64_t count_t;
    typedef std::map<input_t, count_t> cmap_t;

    FreqStats() : num_in(0) {}
    template <class RAN>
    explicit FreqStats(const RAN& R) : num_in(0) { insert(R); }

    // Returns the total number of insertions, and the number of new
    // elements inserted:
    typedef std::pair<count_t, count_t> pcount_t;
    template <class RAN>
    pcount_t insert(const RAN& R) {
      pcount_t res{num_in, cm.size()};
      for (const input_t& x : R) { ++cm[x]; ++num_in; }
      res.first = num_in - res.first;
      res.second = cm.size() - res.second;
      return res;
    }
    pcount_t operator +=(const input_t x) noexcept {
      ++num_in;
      const count_t old = cm.size();
      ++cm[x];
      return {1, cm.size() - old};
    }

    count_t num_inputs() const noexcept { return num_in; }
    count_t num_values() const noexcept { return cm.size(); }
    const cmap_t cmap() const noexcept { return cm; }

    input_t min() const noexcept {
      if (num_in == 0) return std::numeric_limits<input_t>::max();
      else return cm.begin()->first;
    }
    input_t max() const noexcept {
      if (num_in == 0) return std::numeric_limits<input_t>::lowest();
      else return (--cm.end())->first;
    }
    output_t sum() const noexcept {
      output_t res = 0;
      for (const auto& p : cm) res += p.second * p.first;
      return res;
    }
    output_t sum_sq() const noexcept {
      output_t res = 0;
      for (const auto& p : cm) res += p.second * p.first*p.first;
      return res;
    }
    output_t sum_sqd(const output_t am) const noexcept {
      if (num_in <= 1) return 0;
      output_t res = 0;
      for (const auto& p : cm) {
        const output_t diff = output_t(p.first) - am;
        res += p.second * diff*diff;
      }
      return res;
    }
    output_t sum_sqd() const noexcept {
      return sum_sqd(amean());
    }

    output_t amean() const noexcept {
      if (num_in == 0) return 0;
      else return sum() / output_t(num_in);
    }
     output_t var_population(const output_t am) const noexcept {
      if (num_in <= 1) return 0;
      return sum_sqd(am) / output_t(num_in);
    }
    output_t var_population() const noexcept {
      return var_pupulation(amean());
    }
    output_t var_unbiased(const output_t am) const noexcept {
      if (num_in <= 1) return 0;
      return sum_sqd(am) / output_t(num_in-1);
    }
    output_t var_unbiased() const noexcept {
      return var_unbiased(amean());
    }
    output_t sd_population(const output_t am) const noexcept {
      return std::sqrt(var_population(am));
    }
    output_t sd_population() const noexcept {
      return sd_population(amean());
    }
    output_t sd_corrected(const output_t am) const noexcept {
      return std::sqrt(var_unbiased(am));
    }
    output_t sd_corrected() const noexcept {
      return sd_corrected(amean());
    }

    output_t median() const noexcept {
      if (num_in == 0) return 0;
      else if (num_in % 2 == 1) {
        const count_t index = (num_in - 1) / 2;
        auto it = cm.begin();
        for (count_t sum = it->second; index >= sum; ++it, sum += it->second);
        return it->first;
      }
      else {
        const count_t left = num_in / 2 - 1;
        auto it = cm.begin();
        count_t sum = it->second;
        for (; left >= sum; ++it, sum += it->second);
        const output_t val_left = it->first;
        if (left+1 < sum) return val_left;
        else return std::midpoint<output_t>(val_left, (++it)->first);
      }
    }

    // Statistics of "second order" (regarding the counts of values):
    typedef StatsStore<count_t, FloatingPoint::float80> secord_t;
    secord_t second_order_stats() const noexcept {
      secord_t res;
      for (const auto& p : cm) res += p.second;
      res.update();
      return res;
    }

    typedef StatsR<output_t> stats_t;
    stats_t extract1() const noexcept {
      const auto am = amean();
      return {
        {num_in, output_t(min()), am, output_t(max()), sd_corrected(am)},
        median()};
    }
    typedef StatsR<FloatingPoint::float80> stats2_t;
    stats2_t extract2() const noexcept {
      return second_order_stats().extract();
    }

    bool operator ==(const FreqStats&) const noexcept = default;

    typedef typename cmap_t::value_type pair_t;
    void out_pair(std::ostream& out, const pair_t& p) const {
      out << p.first << ":" << p.second;
    }
    void out_map(std::ostream& out) const {
      if (cm.empty()) return;
      auto it = cm.begin(); const auto end = cm.end();
      out_pair(out, *it); ++it;
      for (; it != end; ++it) { out << " "; out_pair(out, *it); }
    }

    friend std::ostream& operator <<(std::ostream& out, const FreqStats& fs) {
      out << "L1 " << fs.extract1() << "\n";
      if (not fs.cmap().empty()) {
        fs.out_map(out);
        out << "\nL2 " << fs.extract2() << "\n";
      }
      return out;
    }

  private :
    cmap_t cm;
    count_t num_in;
  };



  /*  RandVal<IN, OUT>:

       - generate with dimension k and seed-sequence s
       - set the boundaries of the k-dimension input-cube
       - set the number N of iterations
       - create BasicStats-object from evaluating N times a function f
         on random points from the input-cube.

  */

  template<typename IN, typename OUT = IN>
  struct RandVal {
    typedef IN input_t;
    typedef OUT output_t;

    typedef std::uint64_t count_t;
    typedef std::vector<input_t> vec_t;
    typedef std::function<output_t(const vec_t&)> function_t;
    typedef typename vec_t::size_type size_t;
    typedef std::array<input_t, 2> interval_t;
    typedef std::vector<interval_t> bounds_t;
    typedef RandGen::vec_eseed_t seed_t;
    typedef BasicStats<input_t, output_t> stats_t;

    static constexpr count_t default_N = 1000;
    const size_t k;
    count_t N = default_N;
    bool sorted = false;

    RandVal(const size_t k, const seed_t& s) noexcept :
    k(k), rg(s), ib(k,{0,1}) {}

    input_t a(const size_t i) const noexcept {
      assert(i < k);
      return ib[i][0];
    }
    input_t b(const size_t i) const noexcept {
      assert(i < k);
      return ib[i][1];
    }
    void a(const size_t i, const input_t x) noexcept {
      assert(i < k);
      ib[i][0] = x;
    }
    void b(const size_t i, const input_t x) noexcept {
      assert(i < k);
      ib[i][1] = x;
    }

    stats_t run(const function_t& f) noexcept {
      vec_t x(k);
      stats_t res;
      typedef RandGen::Uniform80Range<RandGen::RandGen_t> gen_t;
      std::vector<gen_t> gen; gen.reserve(k);
      for (size_t i = 0; i < k; ++i)
        gen.emplace_back(rg, ib[i][0], ib[i][1]);
      for (count_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < k; ++j) x[j] = gen[j]();
        if (sorted) std::sort(x.begin(), x.end());
        res += f(x);
      }
      return res;
    }

  private :

    RandGen::RandGen_t rg;
    bounds_t ib;
  };


  template <typename FLOAT>
  struct StatsPoints {
    typedef FLOAT float_t;
    typedef std::uint64_t count_t;
    typedef std::array<float_t,2> point_t;

    count_t N;
    point_t xmin, xmax, ymin, ymax;
    float_t xmid, ymid, xspan, yspan, spanq, ymean, ysd, ymed;

    template <class VEC>
    StatsPoints(const VEC& v) noexcept {
      const auto size = v.size();
      assert(size != 0);
      assert(std::is_sorted(v.begin(), v.end(),
                            [](auto a, auto b){return a[0]<b[0];}));
      N = size;
      xmin = v.front(); xmax = v.back();
      xmid = std::midpoint<float_t>(xmin[0], xmax[0]);
      xspan = xmax[0] - xmin[0];
      ymin = {0, std::numeric_limits<float_t>::infinity()};
      ymax = {0, -std::numeric_limits<float_t>::infinity()};
      std::vector<float_t> yv; yv.reserve(size);
      {float_t ysum = 0;
       for (const auto [x,y] : v) {
         yv.push_back(y);
         ysum += y;
         if (y < ymin[1]) { ymin = {x,y}; }
         if (y > ymax[1]) { ymax = {x,y}; }
       }
       ymid = std::midpoint<float_t>(ymin[1], ymax[1]);
       yspan = ymax[1] - ymin[1];
       spanq = yspan / xspan;
       ymean = ysum / size;
      }
      {float_t ssum = 0;
       for (const float_t y : yv) {
         const float_t diff = y - ymean;
         ssum += diff*diff;
       }
       ysd = std::sqrt(ssum / size);
      }
      std::sort(yv.begin(), yv.end());
      ymed = median<float_t>(yv);
    }

    static constexpr int min_width = 20;
    static constexpr int max_width = 32;
    static constexpr std::streamsize min_prec = 5;
    static constexpr const char* def_name = "y";

    struct Format {
      int W = min_width;
      std::streamsize prec = min_prec;
      const char* name = def_name;
      bool sn = false;

      using E = FloatingPoint::WrapE<float_t>;

      Format() noexcept { E::deactivated = true; }
      Format(const char* const n) noexcept : name(n) {}
      Format (const std::streamsize p, const char* const n) noexcept :
        prec(p), name(n) {
        if (prec == -1) W = max_width;
      }
      Format(const std::streamsize p, const char* const n, const bool d)
        noexcept : prec(p), name(n), sn(not d) {
        if (prec == -1) W = max_width;
      }
      Format(const char* const n, const bool rp, const bool sn) noexcept :
        name(n), sn(sn) {
        if (not rp) {prec = -1; W = max_width;}
      }
    };

    void out(std::ostream& out, const Format& f = Format()) const {
      const auto old_prec = out.precision();
      using E = Format::E;
      const auto old_sn = E::deactivated;

      const int W = f.W;
      if (f.prec == -1)
        FloatingPoint::fullprec_floatg<float_t>(std::cout);
      else
        out.precision(f.prec);
      const std::string s = f.name;
      E::deactivated = not f.sn;

      using std::setw;
      const auto w = setw(W);
      const auto L = s.size();

      out <<
        "x" <<setw(W-1)<< xmin[0] <<w<< xmid <<w<< xmax[0] << "\n" <<
        (" "+s) <<setw(W-L-1)<< xmin[1] <<w<< " " <<w<< xmax[1] << "\n" <<
        s <<setw(W-L)<< E(ymin[1]) <<w<< E(ymid) <<w<< E(ymax[1]) << "\n"
        " x" <<setw(W-2)<< ymin[0] <<w<< " " <<w<< ymax[0] << "\n"
        " ads" <<setw(W-4)<< E(ymean) <<w<< E(ymed) <<w<< E(ysd) << "\n"
        "span-q" <<setw(2*W-6)<< spanq << "\n"
        ;

      out.precision(old_prec);
      E::deactivated = old_sn;
    }

  };

}

#endif
