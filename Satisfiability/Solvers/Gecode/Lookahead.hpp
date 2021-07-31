// Oleg Zaikin, 5.4.2021 (Irkutsk)
/* Copyright 2021 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*

    An implementation of look-ahead for the Gecode library.

 TODOS:

0. Provide overview on functionality provided.
    - Also each function/class needs at least a short specification.

1. Divide Lookahead.hpp into several files.
    - At least the SearchStat struct should mode into new Statistics.hpp,

2. Statistics are urgently needed.
    - Basic statistics (number of nodes, inner nodes, unsatisfiable leaves,
      and solutions) is now calculated if look-ahead branching is used.
    - More statistics will be added soon.

3. Four levels of LA-reduction:
    - Level 0 :
     - no explicit reduction;
     - for every branching unsatisfiable branches are just removed;
     - if a branching of width 0 is created, the problem is (immediately)
       recognised as unsatisfiable;

       In NaryLookahead::choice(Space&), there one finds "report that the
       current branch is failed": likely this refers to the *branching*?
       And what is "the first failed value"? There should be *no* value here.
     - if a branching of width 1 is created, then this branching, as a single-
       child-branching, is immediately excecuted.
    - Level 1 :
     - still no explicit reduction;
     - additionally to level 1, if in a considered branching a branch is found
       unsatisfiable, then the corresponding restriction is applied to the
       current instance (globally).
    - Level 2 :
     - now there is an explicit reduction, which applies the reductions found
       by falsified branches until a fixed-point is reached;
     - then in the separate branching function no checks for unsatisfiability
       etc. are done (since no such case can occur).
    - Level 3 :
     - additionally to level 2, now in a considered branching also the
       intersection of the branches is considered for a common reduction.

4. Generate examples for which tree sizes when using look-ahead are known.
  - It will allow checking correctness of the look-ahead implementation.
  - By now correctness is checked by hand on several small examples:
      Trivial::Sum; Send-more-money; Send-most-money.

5. Later: we don't want to handle variables, but branchers.
  - We can't restrict to just branching on values.
  - We want access to the given possibilities for branching.

6. Later: general concept of a distance.
  - Now distance is a delta of measures.
  - A general concept of a distance should be properly supported.
  - A distance can be handled as a function of two arguments dist(F,F').

*/

#ifndef LOOKAHEAD_lNFKYYpHQ8
#define LOOKAHEAD_lNFKYYpHQ8

#include <iostream>
#include <limits>
#include <vector>
#include <memory>
#include <functional>
#include <array>
#include <tuple>
#include <string>
#include <iomanip>
#include <algorithm>

#include <cmath>
#include <cassert>
#include <cstdint>

#include <gecode/int.hh>
#include <gecode/search.hh>

#include <Numerics/FloatingPoint.hpp>
#include <Numerics/Tau.hpp>
#include <SystemSpecifics/Timing.hpp>
#include <ProgramOptions/Environment.hpp>

namespace Lookahead {

  namespace FP = FloatingPoint;
  namespace GC = Gecode;

  // size_t is used for sizes of Gecode arrays.
  // For a Gecode array, size() returns int, so the function
  // size_t tr(int size) was introduced to convert int to size_t.
  typedef unsigned size_t;
  inline constexpr size_t tr(const int size, [[maybe_unused]] const size_t bound = 0) noexcept {
    assert(bound <= std::numeric_limits<int>::max());
    assert(size >= int(bound));
    return size;
  }

  typedef FP::float80 float_t;
  typedef std::uint64_t count_t;

  // Array of values of an integer variable:
  typedef GC::Int::IntView IntView;
  // Array of array of values of integer variables:
  typedef GC::ViewArray<IntView> IntViewArray;
  // Value iterator for an integer variable:
  typedef GC::IntVarValues IntVarValues;

  typedef std::vector<int> values_t;
  typedef std::vector<bool> eq_values_t;

  // bt_t is a branching tuple, i.e. a tuple of distances.
  // Distance is how much simpler the problem became.
  typedef std::vector<float_t> bt_t;
  // measure_t is a function for measuring a given formula.
  // A fromula is an array of integer variables and their values.
  typedef std::function<float_t(const GC::IntVarArray)> measure_t;


  // Branching type, i.e. how branching is formed and executed.
  // la: choose a variable via look-ahead.
  // mind: choose a variable with minimal domain size.
  enum class BrTypeO {la=0, mind=1};

  // How branches are formed for a branching.
  // eq: for a variable var and its values val1, ..., valk,
  //  for each value vali the branching is formed by two branches
  //  var=vali, var!=vali.
  // val: for a variable var and its values val1, ..., valk,
  //  one branching is formed by branches var=val1, ..., var=valk.
  enum class BrSourceO {eqval=0, eq=1, val=2};

  // Function to measure a formula represented by an array of
  // integer variables. The function used to calculate distances,
  // which form branching tuples.
  // mu0: summation of size(var)-1 for all variables.
  // mu1: product of log2 of sizes of all variables.
  enum class BrMeasureO {mu0=0, mu1=1};

  // The number of solutions to find.
  // one: find one solution or prove that no solution exists.
  // all: find all solutions or prove that no solution exists.
  enum class BrSolutionO {one=0, all=1};
}
namespace Environment {
  template <>
  struct RegistrationPolicies<Lookahead::BrTypeO> {
    static constexpr int size = int(Lookahead::BrTypeO::mind)+1;
    static constexpr std::array<const char*, size> string
    {"la", "mind"};
  };
  template <>
  struct RegistrationPolicies<Lookahead::BrSourceO> {
    static constexpr int size = int(Lookahead::BrSourceO::val)+1;
    static constexpr std::array<const char*, size> string
    {"eqval", "eq", "val"};
    // ??? always same length ???
  };
  template <>
  struct RegistrationPolicies<Lookahead::BrMeasureO> {
    static constexpr int size = int(Lookahead::BrMeasureO::mu1)+1;
    static constexpr std::array<const char*, size> string
    {"mu0", "mu1"};
  };
  template <>
  struct RegistrationPolicies<Lookahead::BrSolutionO> {
    static constexpr int size = int(Lookahead::BrSolutionO::all)+1;
    static constexpr std::array<const char*, size> string
    {"one", "all"};
  };
}
namespace Lookahead {
  constexpr char sep = ',';
  typedef std::tuple<BrTypeO, BrSourceO, BrMeasureO, BrSolutionO> option_t;

  std::ostream& operator <<(std::ostream& out, const BrTypeO brt) {
    switch (brt) {
    case BrTypeO::mind : return out << "min-domain-size";
    default : return out << "lookahead";}
  }
  std::ostream& operator <<(std::ostream& out, const BrSourceO brsrs) {
    switch (brsrs) {
    case BrSourceO::eq : return out << "equals-only";
    case BrSourceO::val : return out << "values-only";
    default : return out << "equals+values";}
  }
  std::ostream& operator <<(std::ostream& out, const BrMeasureO brm) {
    switch (brm) {
    case BrMeasureO::mu1 : return out << "product-msr";
    default : return out << "sum-msr";}
  }
  std::ostream& operator <<(std::ostream& out, const BrSolutionO brsln) {
    switch (brsln) {
    case BrSolutionO::all : return out << "all-sol";
    default : return out << "one-sol";}
  }


  // XXX Specification XXX
  struct SearchStat {
    count_t nodes;
    count_t inner_nodes;
    count_t unsat_leaves;
    count_t solutions;
    count_t choice_calls;
    count_t tau_calls;
    count_t subproblem_calls;

    // XXX use Statistics::BasicStats<double,double> XXX
    double choice_time;
    double tau_time;
    double subproblem_time;
    double propag_time;

    GC::Search::Statistics engine; // XXX check whether it is a value-object, and rename XXX

    option_t br_options; // XXX not a statistics XXX likely shouldn't be here

    SearchStat() : nodes(0), inner_nodes(0), unsat_leaves(0),
                   solutions(0), choice_calls(0), tau_calls(0),
                   subproblem_calls(0), choice_time(0),
                   tau_time(0), subproblem_time(0), propag_time(0),
                   engine(), br_options() {}

    bool valid() const noexcept {
      return unsat_leaves + solutions + inner_nodes == nodes;
      // XXX seems incomplete XXX
    }

    // XXX ??? remove XXX
    void reset() noexcept {
      assert(valid());
      nodes = inner_nodes = unsat_leaves = solutions = 0;
    }

    // XXX Use a proper class, make all data members private, and make all
    // updating-etc automatic (so this should become private) XXX
    void update_nodes() noexcept {
      const BrTypeO brt = std::get<BrTypeO>(br_options);
      if (brt != BrTypeO::la and unsat_leaves < engine.fail)
        unsat_leaves += engine.fail;
      nodes = inner_nodes + unsat_leaves + solutions;
      assert(valid());
    }

    void update_choice_stat(const double t) noexcept {
      ++choice_calls;
      choice_time += t;
    }
    void update_tau_stat(const double t) noexcept {
      ++tau_calls;
      tau_time += t;
    }
    void update_subproblem_stat(const double t) noexcept {
      ++subproblem_calls;
      subproblem_time += t;
    }

    void print() const noexcept {
      assert(valid());
      const BrTypeO brt = std::get<BrTypeO>(br_options);
      const BrSourceO brsrc = std::get<BrSourceO>(br_options);
      const BrMeasureO brm = std::get<BrMeasureO>(br_options);
      const BrSolutionO brsln = std::get<BrSolutionO>(br_options);
      using std::setw;
      const auto w = setw(10);
      std::cout << nodes << w << inner_nodes << w << unsat_leaves << w << solutions
                << w << int(brt) << w << int(brsrc) << w << int(brm) << w
                << int(brsln) << "\n";
    }
  };

  // XXX no global variables in header-files !!! XXX
  SearchStat global_stat;

  inline float_t mu0(const GC::IntVarArray& V) noexcept {
    float_t s = 0;
    for (const auto& v : V) {
      const auto is = tr(v.size(), 1);
      s += is - 1;
    }
    return s;
  }

  inline float_t mu1(const GC::IntVarArray& V) noexcept {
    float_t s = 0;
    for (const auto& v : V) {
      const auto is = tr(v.size(), 1);
      s += FP::log2(is);
    }
    return s;
  }


  template<class ModSpace>
  std::shared_ptr<ModSpace> subproblem(ModSpace* const m, const int v, const int val,
                                       const bool eq = true) noexcept {
    const Timing::UserTime timing;
    const Timing::Time_point t0 = timing();
    assert(m->valid());
    assert(m->valid(v));
    assert(m->status() == GC::SS_BRANCH);
    // Clone space:
    std::shared_ptr<ModSpace> c(static_cast<ModSpace*>(m->clone()));
    assert(c->valid());
    assert(c->valid(v));
    assert(c->status() == GC::SS_BRANCH);
    // Add an equality constraint for the given variable and its value:
    if (eq) GC::rel(*(c.get()), (c.get())->at(v), GC::IRT_EQ, val);
    else GC::rel(*(c.get()), (c.get())->at(v), GC::IRT_NQ, val);
    const Timing::Time_point t1 = timing();
    global_stat.update_subproblem_stat(t1-t0);
    return c;
  }


  enum class BrStatus { unsat=0, sat=1, branching=2 };

  // Documentation XXX
  struct Branching {
    BrStatus status;
    int var;
    values_t values;
    eq_values_t eq_values;
    bt_t v_tuple;
    bt_t eq_tuple;
    float_t ltau;

    // ???
    bool valid() const noexcept {
      return var >= 0 and eq_values.size() <= 2 and ltau >= 0 and
      (eq_values.empty() or eq_values.size() == 1 or eq_values[0] != eq_values[1]) and
      v_tuple.size() <= values.size() and eq_tuple.size() <= eq_values.size() and
      ((status == BrStatus::unsat and values.empty() and eq_values.empty()) or
       (status != BrStatus::unsat and values.size() == 1 and not eq_values.empty()) or
       (status != BrStatus::unsat and not values.empty() and eq_values.empty()));
    }

    bool operator <(const Branching& a) const noexcept { return ltau < a.ltau; }

    void print() const noexcept {
      std::cout << static_cast<int>(status) << " " << var << " {";
      for (auto& x : values) std::cout << x << ",";
      std::cout << "} {";
      for (auto x : eq_values) std::cout << int(x) << ",";
      std::cout << "} {";
      for (auto& x : v_tuple) std::cout << (int)x << ",";
      std::cout << "} {";
      for (auto& x : eq_tuple) std::cout << (int)x << ",";
      std::cout << "} " << ltau << std::endl;
    }

    bool update_v() noexcept {
      assert(status != BrStatus::unsat);
      bool brk = false;
      // If branching of width 1, immediately execute:
      if (v_tuple.size() == 1) {
        assert(status != BrStatus::unsat);
        assert(not values.empty());
        brk = true;
      }
      // If branching of width 0, the problem is unsat, immediately execute:
      else if (v_tuple.empty() and values.empty()) {
        assert(status != BrStatus::sat);
        status = BrStatus::unsat;
        brk = true;
      }
      // If all subproblems are satisfiable, immediately execute:
      else if (v_tuple.empty() and not values.empty()) {
        assert(status == BrStatus::sat);
        brk = true;
      }
      else {
        assert(v_tuple.size() > 1);
        const Timing::UserTime timing;
        const Timing::Time_point t0 = timing();
        ltau = Tau::ltau(v_tuple);
        const Timing::Time_point t1 = timing();
        global_stat.update_tau_stat(t1-t0);
      }
      return brk;
    }

    bool update_eq() noexcept {
      assert(status != BrStatus::unsat);
      bool brk = false;
      if (eq_tuple.size() == 1) {
        assert(status != BrStatus::unsat);
        brk = true;
      }
      else if (eq_tuple.empty() and eq_values.empty()) {
        assert(status != BrStatus::sat);
        values = {};
        status = BrStatus::unsat;
        brk = true;
      }
      else if (eq_tuple.empty() and not eq_values.empty()) {
        assert(status == BrStatus::sat);
        brk = true;
      }
      else {
        assert(not eq_tuple.empty());
        const Timing::UserTime timing;
        const Timing::Time_point t0 = timing();
        ltau = Tau::ltau(eq_tuple);
        const Timing::Time_point t1 = timing();
        global_stat.update_tau_stat(t1-t0);
      }
      return brk;
    }

    size_t branches_num() const noexcept {
      assert(valid());
      if (eq_values.empty()) return values.empty() ? 1 : values.size();
      else return eq_values.size();
    }

    Branching(const BrStatus st=BrStatus::unsat, const int v=0,
              const values_t vls={}, const eq_values_t eq_vls={},
              const bt_t v_tpl={}, const bt_t eq_tpl={})
      : status(st), var(v), values(vls), eq_values(eq_vls), v_tuple(v_tpl),
      eq_tuple(eq_tpl), ltau(FP::pinfinity) {}
  };


  template <class CustomisedBrancher>
  struct BranchingChoice : public GC::Choice {
    Branching br;

    bool valid() const noexcept { return br.valid(); }

    BranchingChoice(const CustomisedBrancher& b, const Branching& br = Branching())
      : GC::Choice(b, br.branches_num()), br(br) {
      assert(valid());
    }

  };


  // A customised brancher. Branchings are formed by assigning all possible
  // values to all unassigned variables. A branching with minimal domain
  // size is chosen as the best branching.
  class MinDomValue : public GC::Brancher {
    IntViewArray x;
    mutable int start;

    static bool valid(const IntViewArray x) noexcept { return x.size() > 0; }
    static bool valid(const int s, const IntViewArray x) noexcept {
      return s >= 0 and valid(x) and s < x.size();
    }

  public:

    bool valid() const noexcept { return valid(start, x); }

    MinDomValue(const GC::Home home, const IntViewArray& x)
      : GC::Brancher(home), x(x), start(0) { assert(valid(start, x)); }
    MinDomValue(GC::Space& home, MinDomValue& b)
      : GC::Brancher(home,b), start(b.start) {
      assert(valid(b.x));
      x.update(home, b.x);
      assert(valid(start, x));
    }

    static void post(GC::Home home, const IntViewArray& x) {
      new (home) MinDomValue(home, x);
    }
    virtual GC::Brancher* copy(GC::Space& home) {
      return new (home) MinDomValue(home, *this);
    }
    virtual bool status(const GC::Space&) const {
      assert(valid(start, x));
      for (auto i = start; i < x.size(); ++i)
        if (not x[i].assigned()) { start = i; return true; }
      return false;
    }

    virtual GC::Choice* choice(GC::Space&) {
      const Timing::UserTime timing;
      const Timing::Time_point t0 = timing();
      assert(valid(start, x));
      int var = start;
      size_t width = tr(x[var].size());
      assert(width > 0);
      for (int i = start + 1; i < x.size(); ++i)
        if (not x[i].assigned() and x[i].size() < width) {
          var = i; width = tr(x[var].size());
          assert(width > 0);
        }
      assert(var >= start and var >= 0 and not x[var].assigned());
      values_t values;
      for (GC::Int::ViewValues i(x[var]); i(); ++i)
        values.push_back(i.val());
      assert(not values.empty());
      ++global_stat.inner_nodes;
      Branching br(BrStatus::branching, var, values);
      assert(br.valid());
      const Timing::Time_point t1 = timing();
      global_stat.update_choice_stat(t1-t0);
      return new BranchingChoice<MinDomValue>(*this, br);
    }
    virtual GC::Choice* choice(const GC::Space&, GC::Archive&) {
      return new BranchingChoice<MinDomValue>(*this);
    }

    virtual GC::ExecStatus commit(GC::Space& home, const GC::Choice& c,
                                  const unsigned branch) {
      typedef BranchingChoice<MinDomValue> BrChoice;
      const BrChoice& brc = static_cast<const BrChoice&>(c);
      const Branching& br = brc.br;
      assert(brc.valid() and br.valid());
      const auto& values = br.values;
      const auto var = br.var;
      assert(br.status == BrStatus::branching);
      assert(var >= 0 and not values.empty());
      assert(branch < values.size());
      // Unsatisfiable leaf:
      if (GC::me_failed(x[var].eq(home, values[branch]))) {
        ++global_stat.unsat_leaves;
        return GC::ES_FAILED;
      }
      // Execute branching:
      return GC::ES_OK;
    }

  };

  // A customised brancher. For a variable var and its value val, branching is
  // formed by two branches: var==val and var!=val. The best branching
  // corresponds to a variable with minimal domain size and the minimal
  // variable value.
  class MinDomMinValEq : public GC::Brancher {
    IntViewArray x;
    mutable int start;

    static bool valid(const IntViewArray x) noexcept { return x.size() > 0; }
    static bool valid(const int s, const IntViewArray x) noexcept {
      return s >= 0 and valid(x) and s < x.size();
    }

  public:

    bool valid() const noexcept { return valid(start, x); }

    MinDomMinValEq(const GC::Home home, const IntViewArray& x)
      : GC::Brancher(home), x(x), start(0) { assert(valid(start, x)); }
    MinDomMinValEq(GC::Space& home, MinDomMinValEq& b)
      : GC::Brancher(home,b), start(b.start) {
      assert(valid(b.x));
      x.update(home, b.x);
      assert(valid(start, x));
    }

    static void post(GC::Home home, const IntViewArray& x) {
      new (home) MinDomMinValEq(home, x);
    }
    virtual GC::Brancher* copy(GC::Space& home) {
      return new (home) MinDomMinValEq(home, *this);
    }
    virtual bool status(const GC::Space&) const {
      assert(valid(start, x));
      for (auto i = start; i < x.size(); ++i)
        if (not x[i].assigned()) { start = i; return true; }
      return false;
    }

    virtual GC::Choice* choice(GC::Space&) {
      const Timing::UserTime timing;
      const Timing::Time_point t0 = timing();
      assert(valid(start, x));
      int var = start;
      size_t width = tr(x[var].size());
      assert(width > 0);
      for (int i = start + 1; i < x.size(); ++i)
        if (not x[i].assigned() and x[i].size() < width) {
          var = i; width = tr(x[var].size());
          assert(width > 0);
        }
      assert(var >= start and var >= 0 and not x[var].assigned());
      values_t values{x[var].min()};
      assert(values.size() == 1);
      eq_values_t eq_values = {true, false};
      ++global_stat.inner_nodes;
      Branching br(BrStatus::branching, var, values, eq_values);
      assert(br.valid());
      const Timing::Time_point t1 = timing();
      global_stat.update_choice_stat(t1-t0);
      return new BranchingChoice<MinDomMinValEq>(*this, br);
    }
    virtual GC::Choice* choice(const GC::Space&, GC::Archive&) {
      return new BranchingChoice<MinDomMinValEq>(*this);
    }

    virtual GC::ExecStatus commit(GC::Space& home, const GC::Choice& c,
                                  const unsigned branch) {
      typedef BranchingChoice<MinDomMinValEq> BrChoice;
      const BrChoice& brc = static_cast<const BrChoice&>(c);
      const Branching& br = brc.br;
      assert(brc.valid() and br.valid());
      assert(br.status == BrStatus::branching);
      assert(br.values.size() == 1);
      const auto var = br.var;
      const auto val = br.values[0];
      const auto& eq_values = br.eq_values;
      assert(var >= 0);
      assert(eq_values.size() == 2);
      assert(branch == 0 or branch == 1);
      if ( (eq_values[branch] == true and GC::me_failed(x[var].eq(home, val))) or
           (eq_values[branch] == false and GC::me_failed(x[var].nq(home, val))) ) {
        ++global_stat.unsat_leaves;
        return GC::ES_FAILED;
      }
      return GC::ES_OK;
    }

  };


  // A customised LA-based brancher for finding all solutions. Branchings are
  // formed by assigning all possible values to all unassigned variables. The
  // best branching is chosen via the tau-function.
  template <class ModSpace>
  class LookaheadValueAllSln : public GC::Brancher {
    IntViewArray x;
    mutable int start;
    measure_t measure;

    static bool valid(const IntViewArray x) noexcept { return x.size() > 0; }
    static bool valid(const int s, const IntViewArray x) noexcept {
      return s >= 0 and valid(x) and s < x.size();
    }

  public:

    bool valid() const noexcept { return valid(start, x); }

    LookaheadValueAllSln(const GC::Home home, const IntViewArray& x,
      const measure_t measure) : GC::Brancher(home), x(x), start(0), measure(measure) {
    assert(valid(start, x)); }
    LookaheadValueAllSln(GC::Space& home, LookaheadValueAllSln& b)
      : GC::Brancher(home,b), start(b.start), measure(b.measure) {
      assert(valid(b.x));
      x.update(home, b.x);
      assert(valid(start, x));
    }

    static void post(GC::Home home, const IntViewArray& x, const measure_t measure) {
      new (home) LookaheadValueAllSln(home, x, measure);
    }
    virtual GC::Brancher* copy(GC::Space& home) {
      return new (home) LookaheadValueAllSln(home, *this);
    }
    virtual bool status(const GC::Space&) const {
      assert(valid(start, x));
      for (auto i = start; i < x.size(); ++i)
        if (not x[i].assigned()) { start = i; return true; }
      return false;
    }

    virtual GC::Choice* choice(GC::Space& home) {
      const Timing::UserTime timing;
      const Timing::Time_point t0 = timing();
      assert(valid(start, x));
      assert(start < x.size());
      Branching best_br;

      ModSpace* const m = &(static_cast<ModSpace&>(home));
      assert(m->status() == GC::SS_BRANCH);

      const auto msr = measure(m->at());

      // For remaining variables (all before 'start' are assigned):
      for (int v = start; v < x.size(); ++v) {
        // v is a variable, view is the values in Gecode format:
        const IntView view = x[v];
        // Skip assigned variables:
        if (view.assigned()) continue;
        assert(view.size() >= 2);
        bt_t v_tuple; values_t vls;
        BrStatus status = BrStatus::branching;
        // For all values of the current variable:
        for (IntVarValues j(view); j(); ++j) {
          // Assign value, propagate, and measure:
          const int val = j.val();
          auto subm = subproblem<ModSpace>(m, v, val, true);
          Timing::Time_point t1 = Timing::Time_point();
          auto subm_st = subm->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          // Skip unsatisfiable branches:
          if (subm_st != GC::SS_FAILED) {
            // Calculate delta of measures:
            float_t dlt = msr - measure(subm->at());
            assert(dlt > 0);
            vls.push_back(val);
            if (subm_st == GC::SS_SOLVED) status = BrStatus::sat;
            else v_tuple.push_back(dlt);
          }
        }
        Branching br(status, v, vls, {}, v_tuple);
        assert(br.valid());
        bool brk = br.update_v();
        if (brk) { best_br = br; break; }
        // Compare branchings by the ltau value:
        best_br = std::min(best_br, br);
      }
      if (best_br.status != BrStatus::unsat) ++global_stat.inner_nodes;
      [[maybe_unused]] const auto var = best_br.var;
      assert(var >= 0 and var >= start and not x[var].assigned());
      assert(best_br.valid());
      const Timing::Time_point t1 = timing();
      global_stat.update_choice_stat(t1-t0);
      return new BranchingChoice<LookaheadValueAllSln>(*this, best_br);
    }

    virtual GC::Choice* choice(const GC::Space&, GC::Archive&) {
      return new BranchingChoice<LookaheadValueAllSln>(*this);
    }

    virtual GC::ExecStatus commit(GC::Space& home, const GC::Choice& c,
                                  const unsigned branch) {
      typedef BranchingChoice<LookaheadValueAllSln> BrChoice;
      const BrChoice& brc = static_cast<const BrChoice&>(c);
      const Branching& br = brc.br;
      assert(brc.valid() and br.valid());
      const auto status = br.status;
      const auto var = br.var;
      const auto& values = br.values;
      assert(status == BrStatus::unsat or branch < values.size());
      // If unsatisfiable branching, stop executing:
      if (status == BrStatus::unsat or
          GC::me_failed(x[var].eq(home, values[branch]))) {
        ++global_stat.unsat_leaves;
        return GC::ES_FAILED;
      }
      // Execute branching:
      return GC::ES_OK;
    }

  };

  // A customised LA-based brancher for finding one solution. Branchings are formed
  // by assigning all possible values to all unassigned variables. The best
  // branching is chosen via the tau-function.
  template <class ModSpace>
  class LookaheadValueOneSln : public GC::Brancher {
    IntViewArray x;
    mutable int start;
    measure_t measure;

    static bool valid(const IntViewArray x) noexcept { return x.size() > 0; }
    static bool valid(const int s, const IntViewArray x) noexcept {
      return s >= 0 and valid(x) and s < x.size();
    }

  public:

    bool valid() const noexcept { return valid(start, x); }

    LookaheadValueOneSln(const GC::Home home, const IntViewArray& x,
      const measure_t measure) : GC::Brancher(home), x(x), start(0), measure(measure) {
    assert(valid(start, x)); }
    LookaheadValueOneSln(GC::Space& home, LookaheadValueOneSln& b)
      : GC::Brancher(home,b), start(b.start), measure(b.measure) {
      assert(valid(b.x));
      x.update(home, b.x);
      assert(valid(start, x));
    }

    static void post(GC::Home home, const IntViewArray& x, const measure_t measure) {
      new (home) LookaheadValueOneSln(home, x, measure);
    }
    virtual GC::Brancher* copy(GC::Space& home) {
      return new (home) LookaheadValueOneSln(home, *this);
    }
    virtual bool status(const GC::Space&) const {
      assert(valid(start, x));
      for (auto i = start; i < x.size(); ++i)
        if (not x[i].assigned()) { start = i; return true; }
      return false;
    }

    virtual GC::Choice* choice(GC::Space& home) {
      const Timing::UserTime timing;
      const Timing::Time_point t0 = timing();
      assert(valid(start, x));
      assert(start < x.size());
      Branching best_br;

      ModSpace* m = &(static_cast<ModSpace&>(home));
      assert(m->status() == GC::SS_BRANCH);

      const auto msr = measure(m->at());

      // For remaining variables (all before 'start' are assigned):
      for (int v = start; v < x.size(); ++v) {
        // v is a variable, view is the values in Gecode format:
        const IntView view = x[v];
        // Skip assigned variables:
        if (view.assigned()) continue;
        assert(view.size() >= 2);
        bt_t v_tuple; values_t vls;
        BrStatus status = BrStatus::branching;
        // For all values of the current variable:
        for (IntVarValues j(view); j(); ++j) {
          // Assign value, propagate, and measure:
          const int val = j.val();
          auto subm = subproblem<ModSpace>(m, v, val);
          Timing::Time_point t1 = Timing::Time_point();
          auto subm_st = subm->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          // Stop if a solution is found:
          if (subm_st == GC::SS_SOLVED) {
            v_tuple.clear(); vls = {val};
            status = BrStatus::sat;
            break;
          }
          else if (subm_st == GC::SS_BRANCH) {
            // Calculate delta of measures:
            float_t dlt = msr - measure(subm->at());
            assert(dlt > 0);
            vls.push_back(val); v_tuple.push_back(dlt);
          }
        }
        Branching br(status, v, vls, {}, v_tuple);
        assert(br.valid());
        bool brk = (status == BrStatus::sat) or br.update_v();
        if (brk) { best_br = br; break; }
        best_br = std::min(best_br, br);
      }
      if (best_br.status != BrStatus::unsat) ++global_stat.inner_nodes;
      [[maybe_unused]] const auto var = best_br.var;
      assert(var >= 0 and var >= start and not x[var].assigned());
      assert(best_br.valid());
      const Timing::Time_point t1 = timing();
      global_stat.update_choice_stat(t1-t0);
      return new BranchingChoice<LookaheadValueOneSln>(*this, best_br);
    }

    virtual GC::Choice* choice(const GC::Space&, GC::Archive&) {
      return new BranchingChoice<LookaheadValueOneSln>(*this);
    }

    virtual GC::ExecStatus commit(GC::Space& home, const GC::Choice& c,
                                  const unsigned branch) {
      typedef BranchingChoice<LookaheadValueOneSln> BrChoice;
      const BrChoice& brc = static_cast<const BrChoice&>(c);
      const Branching br = brc.br;
      assert(brc.valid() and br.valid());
      const auto status = br.status;
      const auto var = br.var;
      const auto& values = br.values;
      assert(status == BrStatus::unsat or branch < values.size());
      // If unsatisfiable branching, stop executing:
      if (status == BrStatus::unsat or
          GC::me_failed(x[var].eq(home, values[branch]))) {
        ++global_stat.unsat_leaves;
        return GC::ES_FAILED;
      }
      // Execute branching:
      return GC::ES_OK;
    }

  };

  // A customised LA-based brancher for finding all solutions. For a variable var
  // and its value val, branching is formed by two branches: var==val and
  // var!=val. The best branching is chosen via the tau-function.
  template <class ModSpace>
  class LookaheadEqAllSln : public GC::Brancher {
    IntViewArray x;
    mutable int start;
    measure_t measure;

    static bool valid(const IntViewArray x) noexcept { return x.size() > 0; }
    static bool valid(const int s, const IntViewArray x) noexcept {
      return s >= 0 and valid(x) and s < x.size();
    }

  public:

    bool valid() const noexcept { return valid(start, x); }

    LookaheadEqAllSln(const GC::Home home, const IntViewArray& x,
      const measure_t measure) : GC::Brancher(home), x(x), start(0), measure(measure) {
    assert(valid(start, x)); }
    LookaheadEqAllSln(GC::Space& home, LookaheadEqAllSln& b)
      : GC::Brancher(home,b), start(b.start), measure(b.measure) {
      assert(valid(b.x));
      x.update(home, b.x);
      assert(valid(start, x));
    }

    static void post(GC::Home home, const IntViewArray& x, const measure_t measure) {
      new (home) LookaheadEqAllSln(home, x, measure);
    }
    virtual GC::Brancher* copy(GC::Space& home) {
      return new (home) LookaheadEqAllSln(home, *this);
    }
    virtual bool status(const GC::Space&) const {
      assert(valid(start, x));
      for (auto i = start; i < x.size(); ++i)
        if (not x[i].assigned()) { start = i; return true; }
      return false;
    }

    virtual GC::Choice* choice(GC::Space& home) {
      const Timing::UserTime timing;
      const Timing::Time_point t0 = timing();
      assert(valid(start, x));
      assert(start < x.size());
      Branching best_br;

      ModSpace* m = &(static_cast<ModSpace&>(home));
      assert(m->status() == GC::SS_BRANCH);

      const auto msr = measure(m->at());

      for (int v = start; v < x.size(); ++v) {
        const IntView view = x[v];
        if (view.assigned()) continue;
        assert(view.size() >= 2);
        bool brk = false;
        for (IntVarValues j(view); j(); ++j) {
          const int val = j.val();
          bt_t eq_tuple; eq_values_t eq_vls;
          BrStatus status = BrStatus::branching;
          // variable == value:
          auto subm_eq = subproblem<ModSpace>(m, v, val, true);
          Timing::Time_point t1 = Timing::Time_point();
          auto subm_eq_st = subm_eq->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          if (subm_eq_st != GC::SS_FAILED) {
            float_t dlt = msr - measure(subm_eq->at());
            assert(dlt > 0);
            eq_vls.push_back(true);
            if (subm_eq_st == GC::SS_SOLVED) status = BrStatus::sat;
            else eq_tuple.push_back(dlt);
          }
          // variable != value:
          auto subm_neq = subproblem<ModSpace>(m, v, val, false);
          t1 = Timing::Time_point();
          auto subm_neq_st = subm_neq->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          if (subm_neq_st != GC::SS_FAILED) {
            float_t dlt = msr - measure(subm_neq->at());
            assert(dlt > 0);
            eq_vls.push_back(false);
            if (subm_neq_st == GC::SS_SOLVED) status = BrStatus::sat;
            else eq_tuple.push_back(dlt);
          }
          Branching br(status, v, {val}, eq_vls, {}, eq_tuple);
          assert(br.valid());
          brk = br.update_eq();
          if (brk) { best_br = br; break; }
          // Compare branchings by ltau value:
          best_br = std::min(best_br, br);
        }
        if (brk) break;
      }
      if (best_br.status != BrStatus::unsat) ++global_stat.inner_nodes;
      [[maybe_unused]] const auto var = best_br.var;
      assert(var >= 0 and var >= start and not x[var].assigned());
      assert(best_br.valid());
      const Timing::Time_point t1 = timing();
      global_stat.update_choice_stat(t1-t0);
      return new BranchingChoice<LookaheadEqAllSln>(*this, best_br);
    }

    virtual GC::Choice* choice(const GC::Space&, GC::Archive&) {
      return new BranchingChoice<LookaheadEqAllSln>(*this);
    }

    virtual GC::ExecStatus commit(GC::Space& home, const GC::Choice& c,
                                  const unsigned branch) {
      typedef BranchingChoice<LookaheadEqAllSln> BrChoice;
      const BrChoice& brc = static_cast<const BrChoice&>(c);
      const Branching& br = brc.br;
      assert(brc.valid() and br.valid());
      const auto status = br.status;
      if (status == BrStatus::unsat) {
        ++global_stat.unsat_leaves; return GC::ES_FAILED;
      }
      const auto var = br.var;
      const auto& values = br.values;
      const auto& eq_values = br.eq_values;
      assert(var >= 0 and values.size() == 1);
      assert(eq_values.size() == 1 or eq_values.size() == 2);
      assert(branch == 0 or branch == 1);
      assert(branch < eq_values.size());
      const auto val = values[0];
      if ( (eq_values[branch] == true and GC::me_failed(x[var].eq(home, val))) or
           (eq_values[branch] == false and GC::me_failed(x[var].nq(home, val))) ) {
        ++global_stat.unsat_leaves; return GC::ES_FAILED;
      }
      return GC::ES_OK;
    }

  };

  // A customised LA-based brancher for finding one solution. For a variable var
  // and its value val, branching is formed by two branches: var==val and
  // var!=val. The best branching is chosen via the tau-function.
  template <class ModSpace>
  class LookaheadEqOneSln : public GC::Brancher {
    IntViewArray x;
    mutable int start;
    measure_t measure;

    static bool valid(const IntViewArray x) noexcept { return x.size() > 0; }
    static bool valid(const int s, const IntViewArray x) noexcept {
      return s >= 0 and valid(x) and s < x.size();
    }

  public:

    bool valid() const noexcept { return valid(start, x); }

    LookaheadEqOneSln(const GC::Home home, const IntViewArray& x,
      const measure_t measure) : GC::Brancher(home), x(x), start(0), measure(measure) {
    assert(valid(start, x)); }
    LookaheadEqOneSln(GC::Space& home, LookaheadEqOneSln& b)
      : GC::Brancher(home,b), start(b.start), measure(b.measure) {
      assert(valid(b.x));
      x.update(home, b.x);
      assert(valid(start, x));
    }

    static void post(GC::Home home, const IntViewArray& x, const measure_t measure) {
      new (home) LookaheadEqOneSln(home, x, measure);
    }
    virtual GC::Brancher* copy(GC::Space& home) {
      return new (home) LookaheadEqOneSln(home, *this);
    }
    virtual bool status(const GC::Space&) const {
      assert(valid(start, x));
      for (auto i = start; i < x.size(); ++i)
        if (not x[i].assigned()) { start = i; return true; }
      return false;
    }

    virtual GC::Choice* choice(GC::Space& home) {
      const Timing::UserTime timing;
      const Timing::Time_point t0 = timing();
      assert(valid(start, x));
      assert(start < x.size());
      Branching best_br;

      ModSpace* m = &(static_cast<ModSpace&>(home));
      assert(m->status() == GC::SS_BRANCH);

      const auto msr = measure(m->at());

      for (int v = start; v < x.size(); ++v) {
        const IntView view = x[v];
        if (view.assigned()) continue;
        assert(view.size() >= 2);
        bool brk = false;
        for (IntVarValues j(view); j(); ++j) {
          const int val = j.val();
          BrStatus status = BrStatus::branching;
          bt_t eq_tuple; eq_values_t eq_vls;
          // variable == value:
          auto subm_eq = subproblem<ModSpace>(m, v, val, true);
          Timing::Time_point t1 = Timing::Time_point();
          auto subm_eq_st = subm_eq->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          if (subm_eq_st != GC::SS_FAILED) {
            float_t dlt = msr - measure(subm_eq->at());
            assert(dlt > 0);
            eq_vls.push_back(true);
            if (subm_eq_st == GC::SS_SOLVED) {
              status = BrStatus::sat;
              eq_tuple.clear(); eq_vls = {true};
            }
            else eq_tuple.push_back(dlt);
          }
          // variable != value:
          auto subm_neq = subproblem<ModSpace>(m, v, val, false);
          t1 = Timing::Time_point();
          auto subm_neq_st = subm_neq->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          if (subm_neq_st != GC::SS_FAILED) {
            float_t dlt = msr - measure(subm_neq->at());
            assert(dlt > 0);
            eq_vls.push_back(false);
            if (subm_neq_st == GC::SS_SOLVED) {
              status = BrStatus::sat;
              eq_tuple.clear(); eq_vls = {false};
            }
            else eq_tuple.push_back(dlt);
          }
          Branching br(status, v, {val}, eq_vls, {}, eq_tuple);
          assert(br.valid());
          brk = (status == BrStatus::sat) or br.update_eq();
          if (brk) { best_br = br; break; }
          best_br = std::min(best_br, br);
        }
        if (brk) break;
      }
      if (best_br.status != BrStatus::unsat) ++global_stat.inner_nodes;
      [[maybe_unused]] const auto var = best_br.var;
      assert(var >= 0 and var >= start and not x[var].assigned());
      assert(best_br.valid());
      const Timing::Time_point t1 = timing();
      global_stat.update_choice_stat(t1-t0);
      return new BranchingChoice<LookaheadEqOneSln>(*this, best_br);
    }

    virtual GC::Choice* choice(const GC::Space&, GC::Archive&) {
      return new BranchingChoice<LookaheadEqOneSln>(*this);
    }

    virtual GC::ExecStatus commit(GC::Space& home, const GC::Choice& c,
                                  const unsigned branch) {
      typedef BranchingChoice<LookaheadEqOneSln> BrChoice;
      const BrChoice& brc = static_cast<const BrChoice&>(c);
      const Branching& br = brc.br;
      assert(brc.valid() and br.valid());
      const auto status = br.status;
      if (status == BrStatus::unsat) {
        ++global_stat.unsat_leaves; return GC::ES_FAILED;
      }
      const auto var = br.var;
      const auto& values = br.values;
      const auto& eq_values = br.eq_values;
      assert(var >= 0 and values.size() == 1);
      assert(eq_values.size() == 1 or eq_values.size() == 2);
      assert(branch == 0 or branch == 1);
      assert(branch < eq_values.size());
      const auto val = values[0];
      if ( (eq_values[branch] == true and GC::me_failed(x[var].eq(home, val))) or
           (eq_values[branch] == false and GC::me_failed(x[var].nq(home, val))) ) {
        ++global_stat.unsat_leaves; return GC::ES_FAILED;
      }
      return GC::ES_OK;
    }

  };

  // A customised LA-based brancher for finding all solutions. For a variable var,
  // branching is formed by eq-branches and val-branches.
  template <class ModSpace>
  class LookaheadEqValAllSln : public GC::Brancher {
    IntViewArray x;
    mutable int start;
    measure_t measure;

    static bool valid(const IntViewArray x) noexcept { return x.size() > 0; }
    static bool valid(const int s, const IntViewArray x) noexcept {
      return s >= 0 and valid(x) and s < x.size();
    }

  public:

    bool valid() const noexcept { return valid(start, x); }

    LookaheadEqValAllSln(const GC::Home home, const IntViewArray& x,
      const measure_t measure) : GC::Brancher(home), x(x), start(0), measure(measure) {
    assert(valid(start, x)); }
    LookaheadEqValAllSln(GC::Space& home, LookaheadEqValAllSln& b)
      : GC::Brancher(home,b), start(b.start), measure(b.measure) {
      assert(valid(b.x));
      x.update(home, b.x);
      assert(valid(start, x));
    }

    static void post(GC::Home home, const IntViewArray& x, const measure_t measure) {
      new (home) LookaheadEqValAllSln(home, x, measure);
    }
    virtual GC::Brancher* copy(GC::Space& home) {
      return new (home) LookaheadEqValAllSln(home, *this);
    }
    virtual bool status(const GC::Space&) const {
      assert(valid(start, x));
      for (auto i = start; i < x.size(); ++i)
        if (not x[i].assigned()) { start = i; return true; }
      return false;
    }

    virtual GC::Choice* choice(GC::Space& home) {
      const Timing::UserTime timing;
      const Timing::Time_point t0 = timing();
      assert(valid(start, x));
      assert(start < x.size());

      ModSpace* m = &(static_cast<ModSpace&>(home));
      assert(m->status() == GC::SS_BRANCH);

      Branching best_br;
      const auto msr = measure(m->at());

      for (int v = start; v < x.size(); ++v) {
        const IntView view = x[v];
        if (view.assigned()) continue;
        assert(view.size() >= 2);
        bool brk = false;
        bt_t v_tuple;
        values_t vls;
        BrStatus status = BrStatus::branching;
        for (IntVarValues j(view); j(); ++j) {
          const int val = j.val();
          bt_t eq_tuple; eq_values_t eq_vls;
          // XXX likely better handling eq and neq in branching-class XXX
          auto subm_eq = subproblem<ModSpace>(m, v, val, true);
          Timing::Time_point t1 = Timing::Time_point();
          auto subm_eq_st = subm_eq->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          if (subm_eq_st != GC::SS_FAILED) {
            float_t dlt = msr - measure(subm_eq->at());
            assert(dlt > 0);
            eq_vls.push_back(true); vls.push_back(val);
            if (subm_eq_st == GC::SS_SOLVED) status = BrStatus::sat;
            else { eq_tuple.push_back(dlt); v_tuple.push_back(dlt); }
          }
          auto subm_neq = subproblem<ModSpace>(m, v, val, false);
          t1 = Timing::Time_point();
          auto subm_neq_st = subm_neq->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          if (subm_neq_st != GC::SS_FAILED) {
            float_t dlt = msr - measure(subm_neq->at());
            assert(dlt > 0);
            eq_vls.push_back(false);
            if (subm_neq_st == GC::SS_SOLVED) status = BrStatus::sat;
            else eq_tuple.push_back(dlt);
          }
          Branching br(status, v, {val}, eq_vls, {}, eq_tuple);
          assert(br.valid());
          brk = br.update_eq();
          if (brk) { best_br = br; break; }
          best_br = std::min(best_br, br);
        }
        if (brk) break;
        Branching br(status, v, vls, {}, v_tuple);
        assert(br.valid());
        brk = br.update_v();
        if (brk) { best_br = br; break; }
        best_br = std::min(best_br, br);
      }
      if (best_br.status != BrStatus::unsat) ++global_stat.inner_nodes;
      [[maybe_unused]] const auto var = best_br.var;
      assert(var >= 0 and var >= start and not x[var].assigned());
      assert(best_br.valid());
      const Timing::Time_point t1 = timing();
      global_stat.update_choice_stat(t1-t0);
      return new BranchingChoice<LookaheadEqValAllSln>(*this, best_br);
    }

    virtual GC::Choice* choice(const GC::Space&, GC::Archive&) {
      return new BranchingChoice<LookaheadEqValAllSln>(*this);
    }

    virtual GC::ExecStatus commit(GC::Space& home, const GC::Choice& c,
                                  const unsigned branch) {
      typedef BranchingChoice<LookaheadEqValAllSln> BrChoice;
      const BrChoice& brc = static_cast<const BrChoice&>(c);
      const Branching& br = brc.br;
      assert(brc.valid() and br.valid());
      const auto status = br.status;
      if (status == BrStatus::unsat) {
        ++global_stat.unsat_leaves; return GC::ES_FAILED;
      }
      const auto var = br.var;
      const auto& values = br.values;
      const auto& eq_values = br.eq_values;
      assert(var >= 0);
      assert(not values.empty() or not eq_values.empty());
      // Equality-branching:
      if (not eq_values.empty()) {
        assert(values.size() == 1);
        assert(branch == 0 or branch == 1);
        assert(eq_values.size() == 1 or eq_values.size() == 2);
        const auto val = values[0];
        if ( (eq_values[branch] == true and GC::me_failed(x[var].eq(home, val))) or
             (eq_values[branch] == false and GC::me_failed(x[var].nq(home, val))) ) {
          ++global_stat.unsat_leaves; return GC::ES_FAILED;
        }
      }
      // Value-branching:
      else {
        assert(not values.empty());
        assert(branch < values.size());
        if (GC::me_failed(x[var].eq(home, values[branch]))) {
          ++global_stat.unsat_leaves; return GC::ES_FAILED;
        }
      }

      return GC::ES_OK;
    }

  };

  // A customised LA-based brancher for finding one solution. For a variable var,
  // branching is formed by eq-branches and val-branches.
  template <class ModSpace>
  class LookaheadEqValOneSln : public GC::Brancher {
    IntViewArray x;
    mutable int start;
    measure_t measure;

    static bool valid(const IntViewArray x) noexcept { return x.size() > 0; }
    static bool valid(const int s, const IntViewArray x) noexcept {
      return s >= 0 and valid(x) and s < x.size();
    }

  public:

    bool valid() const noexcept { return valid(start, x); }

    LookaheadEqValOneSln(const GC::Home home, const IntViewArray& x,
      const measure_t measure) : GC::Brancher(home), x(x), start(0), measure(measure) {
    assert(valid(start, x)); }
    LookaheadEqValOneSln(GC::Space& home, LookaheadEqValOneSln& b)
      : GC::Brancher(home,b), start(b.start), measure(b.measure) {
      assert(valid(b.x));
      x.update(home, b.x);
      assert(valid(start, x));
    }

    static void post(GC::Home home, const IntViewArray& x, const measure_t measure) {
      new (home) LookaheadEqValOneSln(home, x, measure);
    }
    virtual GC::Brancher* copy(GC::Space& home) {
      return new (home) LookaheadEqValOneSln(home, *this);
    }
    virtual bool status(const GC::Space&) const {
      assert(valid(start, x));
      for (auto i = start; i < x.size(); ++i)
        if (not x[i].assigned()) { start = i; return true; }
      return false;
    }

    virtual GC::Choice* choice(GC::Space& home) {
      const Timing::UserTime timing;
      const Timing::Time_point t0 = timing();
      assert(valid(start, x));
      assert(start < x.size());

      ModSpace* m = &(static_cast<ModSpace&>(home));
      assert(m->status() == GC::SS_BRANCH);

      Branching best_br;
      const auto msr = measure(m->at());

      for (int v = start; v < x.size(); ++v) {
        const IntView view = x[v];
        if (view.assigned()) continue;
        assert(view.size() >= 2);
        bool brk = false;
        bt_t v_tuple;
        values_t vls;
        BrStatus status = BrStatus::branching;
        for (IntVarValues j(view); j(); ++j) {
          const int val = j.val();
          bt_t eq_tuple; eq_values_t eq_vls;
          auto subm_eq = subproblem<ModSpace>(m, v, val, true);
          Timing::Time_point t1 = Timing::Time_point();
          auto subm_eq_st = subm_eq->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          if (subm_eq_st != GC::SS_FAILED) {
            float_t dlt = msr - measure(subm_eq->at());
            assert(dlt > 0);
            eq_vls.push_back(true); vls.push_back(val);
            if (subm_eq_st == GC::SS_SOLVED) {
              status = BrStatus::sat;
              eq_tuple.clear(); eq_vls = {true};
            }
            else { eq_tuple.push_back(dlt); v_tuple.push_back(dlt); }
          }
          auto subm_neq = subproblem<ModSpace>(m, v, val, false);
          t1 = Timing::Time_point();
          auto subm_neq_st = subm_neq->status();
          global_stat.propag_time += Timing::Time_point() - t1;
          if (subm_neq_st != GC::SS_FAILED) {
            float_t dlt = msr - measure(subm_neq->at());
            assert(dlt > 0);
            eq_vls.push_back(false);
            if (subm_neq_st == GC::SS_SOLVED) {
              status = BrStatus::sat;
              eq_tuple.clear(); eq_vls = {false};
            }
            else eq_tuple.push_back(dlt);
          }
          Branching br(status, v, {val}, eq_vls, {}, eq_tuple);
          assert(br.valid());
          brk = (status == BrStatus::sat) or br.update_eq();
          if (brk) { best_br = br; break; }
          best_br = std::min(best_br, br);
        }
        if (brk) break;
        Branching br(status, v, vls, {}, v_tuple);
        assert(br.valid());
        brk = br.update_v();
        if (brk) { best_br = br; break; }
        best_br = std::min(best_br, br);
      }
      if (best_br.status != BrStatus::unsat) ++global_stat.inner_nodes;
      [[maybe_unused]] const auto var = best_br.var;
      assert(var >= 0 and var >= start and not x[var].assigned());
      assert(best_br.valid());
      const Timing::Time_point t1 = timing();
      global_stat.update_choice_stat(t1-t0);
      return new BranchingChoice<LookaheadEqValOneSln>(*this, best_br);
    }

    virtual GC::Choice* choice(const GC::Space&, GC::Archive&) {
      return new BranchingChoice<LookaheadEqValOneSln>(*this);
    }

    virtual GC::ExecStatus commit(GC::Space& home, const GC::Choice& c,
                                  const unsigned branch) {
      typedef BranchingChoice<LookaheadEqValOneSln> BrChoice;
      const BrChoice& brc = static_cast<const BrChoice&>(c);
      const Branching& br = brc.br;
      assert(brc.valid() and br.valid());
      const auto status = br.status;
      if (status == BrStatus::unsat) {
        ++global_stat.unsat_leaves; return GC::ES_FAILED;
      }
      const auto var = br.var;
      const auto& values = br.values;
      const auto& eq_values = br.eq_values;
      assert(var >= 0);
      assert(not values.empty() or not eq_values.empty());
      // Equality-branching:
      if (not eq_values.empty()) {
        assert(values.size() == 1);
        assert(branch == 0 or branch == 1);
        assert(eq_values.size() == 1 or eq_values.size() == 2);
        const auto val = values[0];
        if ( (eq_values[branch] == true and GC::me_failed(x[var].eq(home, val))) or
             (eq_values[branch] == false and GC::me_failed(x[var].nq(home, val))) ) {
          ++global_stat.unsat_leaves; return GC::ES_FAILED;
        }
      }
      // Value-branching:
      else {
        assert(not values.empty());
        assert(branch < values.size());
        if (GC::me_failed(x[var].eq(home, values[branch]))) {
          ++global_stat.unsat_leaves; return GC::ES_FAILED;
        }
      }

      return GC::ES_OK;
    }

  };


  template <class ModSpace>
  inline void post_branching(GC::Home home, const GC::IntVarArgs& V,
                             const option_t options) noexcept {
    assert(not home.failed());
    const BrTypeO brt = std::get<BrTypeO>(options);
    const BrSourceO brsrc = std::get<BrSourceO>(options);
    const BrMeasureO brm = std::get<BrMeasureO>(options);
    const BrSolutionO brsln = std::get<BrSolutionO>(options);
    const IntViewArray y(home, V);
    if (brt == BrTypeO::mind and brsrc == BrSourceO::eq) {
      MinDomMinValEq::post(home, y); }
    else if (brt == BrTypeO::mind and brsrc == BrSourceO::val) {
      MinDomValue::post(home, y);
    }
    else if (brt == BrTypeO::la) {
      measure_t measure = (brm == BrMeasureO::mu0) ? mu0 : mu1;
      if (brsrc == BrSourceO::eq and brsln == BrSolutionO::one) {
        LookaheadEqOneSln<ModSpace>::post(home, y, measure);
      }
      else if (brsrc == BrSourceO::eq and brsln == BrSolutionO::all) {
        LookaheadEqAllSln<ModSpace>::post(home, y, measure);
      }
      else if (brsrc == BrSourceO::val and brsln == BrSolutionO::one) {
        LookaheadValueOneSln<ModSpace>::post(home, y, measure);
      }
      else if (brsrc == BrSourceO::val and brsln == BrSolutionO::all) {
        LookaheadValueAllSln<ModSpace>::post(home, y, measure);
      }
      else if (brsrc == BrSourceO::eqval and brsln == BrSolutionO::one) {
        LookaheadEqValOneSln<ModSpace>::post(home, y, measure);
      }
      else if (brsrc == BrSourceO::eqval and brsln == BrSolutionO::all) {
        LookaheadEqValAllSln<ModSpace>::post(home, y, measure);
      }
    }
  }


  template <class ModSpace>
  void find_all_solutions(const std::shared_ptr<ModSpace> m,
                                const bool print = false) noexcept {
    assert(m->valid());
    typedef std::shared_ptr<ModSpace> node_ptr;
    GC::DFS<ModSpace> e(m.get());
    while (const node_ptr s{e.next()}) {
      if (print) s->print();
      ++global_stat.solutions;
    }
    global_stat.engine = e.statistics();
  }
  template <class ModSpace>
  void find_one_solution(const std::shared_ptr<ModSpace> m,
                                const bool print = false) noexcept {
    assert(m->valid());
    typedef std::shared_ptr<ModSpace> node_ptr;
    GC::DFS<ModSpace> e(m.get());
    if (const node_ptr s{e.next()}) {
      if (print) s->print();
      ++global_stat.solutions;
    }
    global_stat.engine = e.statistics();
  }
  template <class ModSpace>
  SearchStat solve(const std::shared_ptr<ModSpace> m,
                   const bool printsol = false) noexcept {
    assert(m->valid());
    global_stat.reset();
    global_stat.br_options = m->branching_options();
    auto const st = m->status();
    if (st == GC::SS_FAILED) global_stat.unsat_leaves = 1;
    const option_t options = m->branching_options();
    const BrSolutionO brsln = std::get<BrSolutionO>(options);
    switch (brsln) {
    case BrSolutionO::all : find_all_solutions(m, printsol); break;
    default : find_one_solution(m, printsol);}
    global_stat.update_nodes();
    return global_stat;
  }

}


#endif
