// Oliver Kullmann, 26.3.2022 (Swansea)
/* Copyright 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*

  Options for Gecode

TODOS

1. Provide the choice of a random branching-variable:
    - This needs a bit of design how to use our random-number-generator.

*/

#ifndef OPTIONS_JwU0BOX6Tw
#define OPTIONS_JwU0BOX6Tw

#include <array>
#include <ostream>
#include <exception>
#include <string>

#include <gecode/int.hh>

#include <ProgramOptions/Environment.hpp>

namespace Options {

  namespace GC = Gecode;

  // Run-Type:
  enum class RT {
    sat_solving = 0,
    sat_decision = 1,
    count_solutions = 2,
    enumerate_solutions = 3
  };
  constexpr int RTsize = int(RT::enumerate_solutions) + 1;

  // Propagation levels for Gecode-constraints:
  enum class PropO {
    dom=0, // domain propagation (strongest)
    def=1, // default propagation (different for different constraints)
    val=2, // values propagation (weakest)
    bnd=3 // bounds propagation
  };
  constexpr int PropOsize = int(PropO::bnd) + 1;
  GC::IntPropLevel prop_level(const PropO po) noexcept {
    switch (po) {
    case PropO::dom: return GC::IPL_DOM;
    case PropO::def: return GC::IPL_DEF;
    case PropO::val: return GC::IPL_VAL;
    case PropO::bnd: return GC::IPL_BND;
    default : return GC::IPL_DOM;}
  }

  // Variable-selection for Gecode-branching ("branching-heuristic variables").
  // According to Section 8.5.1 of 'Modeling and Programming with Gecode.
  // 6.2.0.', the degree of a variable is the number of propagators depending
  // on it.
  enum class BHV {
    first = 0, // first open
    mindeg = 1, // smallest degree
    maxdeg = 2, // largest degree
    mindom = 3, // smallest domain
    maxdom = 4, // largest domain
    mindegdom = 5, // smallest degree/domain
    maxdegdom = 6, // largest degree/domain
  };
  constexpr int BHVsize = int(BHV::maxdegdom) + 1;
  GC::IntVarBranch var_branch(const BHV bvar) {
    switch (bvar) {
    case BHV::first: return GC::INT_VAR_NONE();
    case BHV::mindeg: return GC::INT_VAR_DEGREE_MIN();
    case BHV::maxdeg: return GC::INT_VAR_DEGREE_MAX();
    case BHV::mindom: return GC::INT_VAR_SIZE_MIN();
    case BHV::maxdom: return GC::INT_VAR_SIZE_MAX();
    case BHV::mindegdom: return GC::INT_VAR_DEGREE_SIZE_MIN();
    case BHV::maxdegdom: return GC::INT_VAR_DEGREE_SIZE_MAX();
    default : throw std::runtime_error("Options::var_branch: UNKNOWN bvar="
                                       + std::to_string(int(bvar)));
    }
  }

  // Lookahead heuristic for Gecode-branching:
  enum class LAH {
    binsupereager = 0, // binary branching with supereager lookahead reduction
    bineager = 1, // binary branching with eager lookahead reduction
    binlazy = 2, // binary branching with lazy lookahead reduction
    enumsupereager = 3, // enumerative branching with supereager lookahead reduction
    enumeager = 4, // enumerative branching with eager lookahead reduction
    enumlazy = 5 // enumerative branching with lazy lookahead reduction
  };
  constexpr int LAHsize = int(LAH::enumlazy) + 1;

  // Values-section for Gecode-branching (also determining the branching-type
  // "binary" vs "enumerativ"); for us "branching-heuristic order":
  enum class BHO {
    binvalmin = 0, // INT_VAL_MIN()
    binvalmax = 1, // INT_VAL_MAX()
    enumvalmin = 2, // INT_VALUES_MIN()
    enumvalmax = 3 // INT_VALUES_MAX()
  };
  constexpr int BHOsize = int(BHO::enumvalmax) + 1;
  GC::IntValBranch val_branch(const BHO bord) {
    switch (bord) {
    case BHO::binvalmin: return GC::INT_VAL_MIN();
    case BHO::binvalmax: return GC::INT_VAL_MAX();
    case BHO::enumvalmin: return GC::INT_VALUES_MIN();
    case BHO::enumvalmax: return GC::INT_VALUES_MAX();
    default : throw std::runtime_error("Options::val_branch: UNKNOWN bord="
                                       + std::to_string(int(bord)));
    }
  }

}
namespace Environment {
  template <> struct RegistrationPolicies<Options::RT> {
    static constexpr int size = Options::RTsize;
    static constexpr std::array<const char*, size>
      string {"sats", "satd", "count", "enum"};
  };
  template <> struct RegistrationPolicies<Options::PropO> {
    static constexpr int size = Options::PropOsize;
    static constexpr std::array<const char*, size>
      string {"dom", "def", "val", "bnd"};
  };
  template <> struct RegistrationPolicies<Options::BHV> {
    static constexpr int size = Options::BHVsize;
    static constexpr std::array<const char*, size>
    string {"first", "mindeg", "maxdeg", "mindom", "maxdom", "mindegdom",
        "maxdegdom"};
  };
  template <> struct RegistrationPolicies<Options::LAH> {
    static constexpr int size = Options::LAHsize;
    static constexpr std::array<const char*, size>
    string {"binsupeag", "bineag", "binlazy", "enumsupeag", "enumeag",
        "enumlazy"};
  };
  template <> struct RegistrationPolicies<Options::BHO> {
    static constexpr int size = Options::BHOsize;
    static constexpr std::array<const char*, size>
      string {"bvalmin", "bvalmax", "evalmin", "evalmax"};
  };
}
namespace Options {

  std::ostream& operator <<(std::ostream& out, const RT rt) {
    switch (rt) {
    case RT::sat_solving : return out << "sat-solving";
    case RT::sat_decision : return out << "sat-decision";
    case RT::count_solutions : return out << "count-solutions";
    case RT::enumerate_solutions : return out << "enumerate-solutions";
    default : return out << "Options::RT: UNKNOWN=" << int(rt);}
  }
  std::ostream& operator <<(std::ostream& out, const PropO po) {
    switch (po) {
    case PropO::dom : return out << "domain-prop";
    case PropO::def : return out << "default-prop";
    case PropO::val : return out << "values-prop";
    case PropO::bnd : return out << "bounds-prop";
    default : return out << "Options::PropO: UNKNOWN=" << int(po);}
  }
  std::ostream& operator <<(std::ostream& out, const BHV bvar) {
    switch (bvar) {
    case BHV::first: return out << "first-var";
    case BHV::mindeg: return out << "min-deg-var";
    case BHV::maxdeg: return out << "max-deg-var";
    case BHV::mindom: return out << "min-dom-var";
    case BHV::maxdom: return out << "max-dom-var";
    case BHV::mindegdom: return out << "min-deg/dom-var";
    case BHV::maxdegdom: return out << "max-deg/dom-var";
    default : return out << "Options::BHV: UNKNOWN=" << int(bvar);}
  }
  std::ostream& operator <<(std::ostream& out, const LAH lah) {
    switch (lah) {
    case LAH::binsupereager: return out << "binary-super-eager";
    case LAH::bineager: return out << "binary-eager";
    case LAH::binlazy: return out << "binary-lazy";
    case LAH::enumsupereager: return out << "enumerate-super-eager";
    case LAH::enumeager: return out << "enumerate-eager";
    case LAH::enumlazy: return out << "enumerate-lazy";
    default : return out << "Options::LAH: UNKNOWN=" << int(lah);}
  }
  std::ostream& operator <<(std::ostream& out, const BHO bord) {
    switch (bord) {
    case BHO::binvalmin: return out << "bin-branch-min";
    case BHO::binvalmax: return out << "bin-branch-max";
    case BHO::enumvalmin: return out << "enum-branch-min";
    case BHO::enumvalmax: return out << "enum-branch-max";
    default : return out << "Options::BHO: UNKNOWN=" << int(bord);}
  }

}

#endif
