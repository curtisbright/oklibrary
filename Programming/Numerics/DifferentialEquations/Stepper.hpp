// Oliver Kullmann, 10.4.2021 (Swansea)
/* Copyright 2021 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*

  Tools for processing ode's

*/

#ifndef STEPPER_1BFtLs6Zgo
#define STEPPER_1BFtLs6Zgo

#include <array>
#include <vector>
#include <algorithm>
#include <limits>

#include <cmath>

#include <Numerics/FloatingPoint.hpp>
#include <Numerics/Statistics.hpp>


namespace Stepper {

  template <typename FLOAT, template <typename> class ODE>
  struct X0Y0 {
    typedef FLOAT float_t;
    typedef ODE<float_t> ode_t;
    typedef typename ode_t::x_t x_t;
    typedef typename ode_t::y_t y_t;
    typedef typename ode_t::F_t F_t;
    typedef typename ode_t::f_t f_t;
    typedef typename ode_t::count_t count_t;

    static constexpr count_t default_N = ode_t::default_N;
  private :
    ode_t ode;
  public :
    const F_t F;
    const f_t sol;
    const count_t size;

    X0Y0(const x_t x0, const y_t y0, const F_t F, const f_t sol = f_t()) noexcept : ode(x0, y0, F, sol), F(ode.F), sol(ode.sol), size(ode.size) {}

    x_t x() const noexcept { return ode.x(); }
    y_t y() const noexcept { return ode.y(); }
    float_t accuracy() const noexcept { return ode.accuracy(); }

    struct point_t {
      x_t x; y_t y;
      friend constexpr bool operator ==(const point_t lhs, const point_t rhs)
        noexcept {
        return lhs.x == rhs.x and lhs.y == rhs.y;
      }
    };
    typedef std::vector<point_t> points_vt;
    typedef std::array<float_t, 2> spoint_t;
    typedef std::vector<spoint_t> spoints_vt;
    spoints_vt translate(const points_vt& v) const noexcept {
      assert(size == 1);
      spoints_vt res; res.reserve(v.size());
      for (const auto& p : v) res.push_back({p.x, p.y});
      return res;
    }
    spoints_vt translate(const points_vt& v, const count_t i) const noexcept {
      assert(size >= 2);
      assert(i < size);
      spoints_vt res; res.reserve(v.size());
      for (const auto& [x,y] : v) res.push_back({x, y[i]});
      return res;
    }

    x_t a() const noexcept { return a0; }
    x_t b() const noexcept { return b0; }
    bool left_included() const noexcept { return left; }
    bool right_included() const noexcept { return right; }
    x_t orig_x0() const noexcept { return ox0; }
    y_t orig_y0() const noexcept { return oy0; }

    x_t xmin() const noexcept { return xmin0; }
    x_t xmax() const noexcept { return xmax0; }
    float_t ymin() const noexcept { return ymin0; }
    float_t yminx() const noexcept { return yminx0; }
    float_t ymax() const noexcept { return ymax0; }
    float_t ymaxx() const noexcept { return ymaxx0; }
    float_t ymean() const noexcept { return ymean0; }
    float_t ysd() const noexcept { return ysd0; }
    const points_vt& points() const noexcept { return pv; }

    float_t accmin() const noexcept { return accmin0; }
    float_t accmax() const noexcept { return accmax0; }
    float_t accmaxx() const noexcept { return accmaxx0; }
    float_t accmean() const noexcept { return accmean0; }
    float_t accsd() const noexcept { return accsd0; }
    float_t accmed() const noexcept { return accmed0; }
    const points_vt& accuracies() const noexcept { return acc; }

    // Computing {i_middle, x0_middle}:
    inline static std::pair<count_t, float_t> best(
      const x_t a0, const float_t b0,
      const x_t delta, const x_t x0, const bool left, const bool right,
      const count_t N) noexcept {
      assert(N >= 2);
      const x_t i = (x0-a0) / delta;
      const count_t fi = std::floor(i), ci = std::ceil(i);
      const x_t xf = a0 + fi * delta, xc = a0 + ci * delta;
      if (std::abs(xf - x0) < std::abs(xc - x0)) {
        if (left or fi != 0) return {fi, xf};
        else return {1, a0+delta};
      }
      else {
        if (ci == 0) if (left) return {0, a0}; else return {1, a0+delta};
        if (right or ci != N) return {ci, xc};
        else return {N-1, b0-delta};
      }
    }

    void interval(
      // begin, end, and their inclusions:
      const x_t b, const bool bi, const x_t e, const bool ei,
      // big steps, small steps, initial steps:
      const count_t bs, const count_t ss, const count_t is = ode_t::default_N) {
      ox0 = ode.x(); oy0 = ode.y();
      N = bs; ssi = ss; iN = is;
      a0 = b; b0 = e; left = bi; right = ei;
      assert(N >= 1 and ssi >= 1 and iN >= 1);
      assert(a0 < b0);
      pv.clear();
      const count_t size = N + 1 - not left - not right; // number of points
      if (size == 0) {assert(pv.size() == size); return;}
      if (ode.x() < a0) { ode.steps(a0 - ode.x(), iN); ode.precise_x0(a0); }
      else if (ode.x() > b0) {ode.steps(b0 - ode.x(), iN); ode.precise_x0(b0);}
      assert(a0 <= ode.x() and ode.x() <= b0);

      if (size == 1) {
        assert(not left or not right);
        if (left) {
          assert(N == 1);
          if (ode.x() != a0) {ode.steps(a0-ode.x(), ssi); ode.precise_x0(a0);}
        }
        else if (right) {
          assert(N == 1);
          if (ode.x() != b0) {ode.steps(b0-ode.x(), ssi); ode.precise_x0(b0);}
        }
        else {
          assert(N == 2);
          const x_t x = std::midpoint(a0, b0);
          if (ode.x() != x) {ode.steps(x-ode.x(),ssi); ode.precise_x0(x);}
        }
        pv.push_back({ode.x(),ode.y()});
        assert(pv.size() == size); return;
      }
      assert(size >= 2);
      if (ode.x() == a0) {
        const x_t delta = (b0 - a0) / N;
        if (left) pv.push_back({ode.x(),ode.y()});
        for (count_t i = 1; i < N; ++i) {
          ode.steps(delta, ssi);
          ode.precise_x0(a0 + i * delta);
          pv.push_back({ode.x(),ode.y()});
        }
        if (right) {
          ode.steps(delta, ssi);
          ode.precise_x0(b0);
          pv.push_back({ode.x(),ode.y()});
        }
        assert(pv.size() == size); return;
      }
      else if (ode.x() == b0) {
        const x_t delta = (a0 - b0) / N;
        if (right) pv.push_back({ode.x(),ode.y()});
        for (count_t i = 1; i < N; ++i) {
          ode.steps(delta, ssi);
          ode.precise_x0(b0 + i * delta);
          pv.push_back({ode.x(),ode.y()});
        }
        if (left) {
          ode.steps(delta, ssi);
          ode.precise_x0(a0);
          pv.push_back({ode.x(),ode.y()});
        }
        std::reverse(pv.begin(), pv.end());
        assert(pv.size() == size); return;
      }
      else {
        assert(a0 < ode.x() and ode.x() < b0);
        const x_t orig_x0 = ode.x();
        const y_t orig_y0 = ode.y();
        const x_t diffl = a0 - ode.x(), diffr = b0 - ode.x();
        assert(diffl < 0 and diffr > 0);
        if (N == 1) {
          assert(left and right);
          ode.steps(diffl, ssi);
          pv.push_back({a0,ode.y()});
          ode.reset(orig_x0, orig_y0);
          ode.steps(diffr, ssi);
          ode.precise_x0(b0); pv.push_back({b0,ode.y()});
          assert(pv.size() == size); return;
        }

        assert(N >= 2);
        const float_t delta = (b0 - a0) / N;
        const auto [i_middle, x0_middle] =
          best(a0,b0, delta,ode.x(), left,right,N);
        assert(i_middle <= N and x0_middle == a0 + i_middle*delta);
        ode.steps(x0_middle - ode.x(), ssi);
        const y_t y0_middle = ode.y();
        ode.precise_x0(x0_middle);
        if (i_middle == 0) {
          if (left) pv.push_back({ode.x(),ode.y()});
          for (count_t i = 1; i < N; ++i) {
            ode.steps(delta, ssi);
            ode.precise_x0(a0 + i * delta);
            pv.push_back({ode.x(),ode.y()});
          }
          if (right) {
            ode.steps(delta, ssi);
            ode.precise_x0(b0);
            pv.push_back({b0,ode.y()});
          }
          assert(pv.size() == size); return;
        }
        else if (i_middle == N) {
          ode.precise_x0(b0);
          const x_t deltan = -delta;
          if (right) pv.push_back({ode.x(),ode.y()});
          for (count_t i = 1; i < N; ++i) {
            ode.steps(deltan, ssi);
            ode.precise_x0(b0 + i * deltan);
            pv.push_back({ode.x(),ode.y()});
          }
          if (left) {
            ode.steps(deltan, ssi);
            ode.precise_x0(a0);
            pv.push_back({ode.x(),ode.y()});
          }
          std::reverse(pv.begin(), pv.end());
          assert(pv.size() == size); return;
        }
        else {
          pv.push_back({ode.x(),ode.y()});
          {const x_t deltan = -delta;
           for (count_t i = i_middle; i != 1; --i) {
             ode.steps(deltan, ssi);
             ode.precise_x0(a0 + (i-1) * delta);
             pv.push_back({ode.x(),ode.y()});
           }
           if (left) {
             ode.steps(deltan, ssi);
             ode.precise_x0(a0);
             pv.push_back({a0,ode.y()});
           }
           std::reverse(pv.begin(), pv.end());
          }
          ode.reset(x0_middle, y0_middle);
          for (count_t i = i_middle+1; i != N; ++i) {
            ode.steps(delta, ssi);
            ode.precise_x0(a0 + i * delta);
            pv.push_back({ode.x(),ode.y()});
          }
          if (right) {
            ode.steps(delta, ssi);
            ode.precise_x0(b0);
            pv.push_back({b0,ode.y()});
          }
          assert(pv.size() == size); return;
        }
      }
    }

    void update_stats() {
      if (pv.empty()) {
        xmin0 = std::numeric_limits<float_t>::infinity();
        xmax0 = -std::numeric_limits<float_t>::infinity();
        ymin0 = std::numeric_limits<float_t>::infinity();
        ymax0 = -std::numeric_limits<float_t>::infinity();
        ymean0 = 0; ysd0 = 0;
      }
      else {
        assert(std::is_sorted(pv.begin(), pv.end(),
                              [](auto a, auto b){return a.x<b.x;}));
        xmin0 = pv.front().x; xmax0 = pv.back().x;
        ymin0 = pv.front().y; ymax0 = ymin0;
        yminx0 = xmin0; ymaxx0 = yminx0;
        float_t sum = ymin0;
        const size_t size = pv.size();
        for (size_t i = 1; i < size; ++i) {
          const float_t y = pv[i].y;
          sum += y;
          if (y < ymin0) { ymin0 = y; yminx0 = pv[i].x; }
          if (y > ymax0) { ymax0 = y; ymaxx0 = pv[i].x; }
        }
        ymean0 = sum / size;
        sum = 0;
        for (const auto& p : pv) {
          const float_t diff = p.y - ymean0;
          sum += diff*diff;
        }
        ysd0 = std::sqrt(sum / size);
      }
    }

    void update_accuracies() {
      acc.clear();
      accmin0 = std::numeric_limits<float_t>::infinity();
      accmax0 = -std::numeric_limits<float_t>::infinity();
      if (pv.empty()) { accmean0 = 0; accsd0 = 0; accmed0 = 0; }
      else {
        acc.reserve(pv.size());
        float_t sum = 0;
        for (const auto [x,y] : pv) {
          const auto a =
            FloatingPoint::accuracyg<float_t>(ode.sol(x), y,
                                              FloatingPoint::PrecZ::eps);
          acc.push_back({x,a});
          sum += a;
          accmin0 = std::min(accmin0, a);
          if (a > accmax0) { accmax0 = a; accmaxx0 = x; }
        }
        const size_t size = pv.size();
        accmean0 = sum / size;
        sum = 0;
        std::vector<float_t> a; a.reserve(acc.size());
        for (const auto& p : acc) {
          const float_t diff = p.y - accmean0;
          sum += diff*diff;
          a.push_back(p.y);
        }
        accsd0 = std::sqrt(sum / size);
        std::sort(a.begin(), a.end());
        accmed0 = GenStats::median<float_t>(a);
      }
    }

    static constexpr int W = 20;

    friend std::ostream& operator <<(std::ostream& out, const X0Y0& s) {
      const auto prec = out.precision();
      using std::setw;
      const auto w = setw(W);
      out.precision(5);
      namespace FP = FloatingPoint;
      out <<
        "x0" << setw(W-2) << s.orig_x0() << "\n"
        "y0" << setw(W-2) << s.orig_y0() << "\n"
        "a,b" << setw(W-5) << s.a() << "," << s.left_included() <<
        setw(2*W-2) << s.b() << "," << s.right_included() << "\n"
        "N(b,s,i)" << setw(W-8) << s.N << w << s.ssi << w << s.iN << "\n\n"
        ;
      using W = FP::WrapE<float_t>;
      FP::fullprec_floatg<float_t>(std::cout);
      out << "x0,y0 : " << s.orig_x0() << " " << s.orig_y0() << "\n"
        "x  : " << s.xmin() << " " << std::midpoint(s.xmin(), s.xmax())
        << " " << s.xmax() << "\n"
        "y  : (" << s.ymin() << ", " << s.yminx() << ")\n  "
        << std::midpoint(s.ymin(), s.ymax()) << "\n  "
        "("  << s.ymax() << ", " << s.ymaxx() << ")\n"
        "  mu,md,sd : " << s.ymean() << " ? " << s.ysd() << "\n"
        "span-q = " <<
        (s.ymax() - s.ymin()) / (s.xmax() - s.xmin()) << "\n"
        "acc: " << "(" << W(s.accmin()) << ", ?)\n  "
        << std::midpoint(s.accmin(), s.accmax()) << "\n  "
        "(" << W(s.accmax()) << ", " << s.accmaxx() << ")\n"
        "  mu,md,sd : " <<
        W(s.accmean()) << "  " << W(s.accmed()) << "  " << W(s.accsd())
          << "\n"
        "span-q = " <<
        (s.accmax() - s.accmin()) / (s.xmax() - s.xmin()) << "\n";
      out.precision(prec);
      return out;
    }

  private :

    count_t N; // N >= 1 is the number of sub-intervals
    count_t ssi; // steps for sub-intervals
    count_t iN; // steps for initial move
    x_t a0, b0; // left and right border of interval
    bool left, right; // whether the borders are included
    x_t ox0;
    y_t oy0;

    x_t xmin0, xmax0;
    float_t ymin0, yminx0, ymax0, ymaxx0, ymean0, ysd0;
    float_t accmin0, accmax0, accmaxx0, accmean0, accsd0, accmed0;

    points_vt pv; // vector of points (x,f(x))
    points_vt acc; // (x, accuracy)

  };

}

#endif
