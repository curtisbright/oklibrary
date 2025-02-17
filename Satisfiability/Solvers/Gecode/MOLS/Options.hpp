// Oliver Kullmann, 26.3.2022 (Swansea)
/* Copyright 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*

  Options for Gecode

TODOS

0. LAT has only three values (the branching-type isn't part of it)
    - The branching-type is given by BRT.

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
    count_with_log = 3,
    enumerate_solutions = 4
  };
  constexpr int RTsize = int(RT::enumerate_solutions) + 1;
  constexpr bool valid(const RT rt) noexcept {
    return int(rt) < RTsize;
  }
  constexpr bool with_log(const RT rt) noexcept {
    return rt == RT::count_with_log;
  }


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

  // The branching-type:
  enum class BRT {
    binbr = 0, // binary branching
    enumbr = 1 // enumerative branching
  };
  constexpr int BRTsize = int(BRT::enumbr) + 1;
  // The Gecode-branching-orders:
  enum class GBO {
    asc = 0, // ascending
    desc = 1 // descending
  };
  constexpr int GBOsize = int(GBO::desc) + 1;

  // Values-section for Gecode-branching (also determining the branching-type
  // "binary" vs "enumerativ"); for us "branching-heuristic order":
  enum class BHO {
    binvalmin = 0, // INT_VAL_MIN()
    binvalmax = 1, // INT_VAL_MAX()
    enumvalmin = 2, // INT_VALUES_MIN()
    enumvalmax = 3 // INT_VALUES_MAX()
  };
  constexpr int BHOsize = int(BHO::enumvalmax) + 1;
  constexpr BHO translate(const BRT bt, const GBO bo) noexcept {
    if (bt == BRT::binbr)
      if (bo == GBO::asc) return BHO::binvalmin;
      else return BHO::binvalmax;
    else
      if (bo == GBO::asc) return BHO::enumvalmin;
      else return BHO::enumvalmax;
  }
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

  // Lookahead-type: the implementation of the la-reduction
  enum class LAT {
    binsupereager = 0, // binary branching with supereager lookahead reduction
    bineager = 1, //  eager lookahead reduction
    binlazy = 2, // lazy lookahead reduction
    enumsupereager = 3, // enumerative branching
    enumeager = 4,
    enumlazy = 5
  };
  constexpr int LATsize = int(LAT::enumlazy) + 1;

}
namespace Environment {
  template <> struct RegistrationPolicies<Options::RT> {
    static constexpr int size = Options::RTsize;
    static constexpr std::array<const char*, size>
      string {"sats", "satd", "count", "+count", "enum"};
    static constexpr std::array<const char*, size>
      estring {"sat-solving", "sat-decision", "count-solutions",
        "count-sols-with-log", "enumerate-solutions"};
  };
  template <> struct RegistrationPolicies<Options::PropO> {
    static constexpr int size = Options::PropOsize;
    static constexpr std::array<const char*, size>
      string {"dom", "def", "val", "bnd"};
    static constexpr std::array<const char*, size>
      estring {"domain-prop", "default-prop", "values-prop", "bounds-prop"};
  };
  template <> struct RegistrationPolicies<Options::BHV> {
    static constexpr int size = Options::BHVsize;
    static constexpr std::array<const char*, size>
    string {"first", "mindeg", "maxdeg", "mindom", "maxdom", "mindegdom",
        "maxdegdom"};
    static constexpr std::array<const char*, size>
      estring {"first-var", "min-deg-var", "max-deg-var", "min-dom-var",
        "max-dom-var", "min-deg/dom-var", "max-deg/dom-var"};
  };
  template <> struct RegistrationPolicies<Options::BRT> {
    static constexpr int size = Options::BRTsize;
    static constexpr std::array<const char*, size>
      string {"binbr", "enumbr"};
    static constexpr std::array<const char*, size>
      estring {"binary-branching", "enumerative-branching"};
  };
  template <> struct RegistrationPolicies<Options::GBO> {
    static constexpr int size = Options::GBOsize;
    static constexpr std::array<const char*, size>
      string {"asc", "desc"};
    static constexpr std::array<const char*, size>
      estring {"ascending-order", "descending-order"};
  };
  template <> struct RegistrationPolicies<Options::BHO> {
    static constexpr int size = Options::BHOsize;
    static constexpr std::array<const char*, size>
      string {"bmin", "bmax", "emin", "emax"};
    static constexpr std::array<const char*, size>
      estring {"bin-branch-min", "bin-branch-max", "enum-branch-min",
        "enum-branch-max"};
  };
  template <> struct RegistrationPolicies<Options::LAT> {
    static constexpr int size = Options::LATsize;
    static constexpr std::array<const char*, size>
      string {"binsupeag", "bineag", "binlazy", "enumsupeag", "enumeag",
        "enumlazy"};
    static constexpr std::array<const char*, size>
      estring {"binary-super-eager", "binary-eager", "binary-lazy",
        "enumerate-super-eager", "enumerate-eager", "enumerate-lazy"};
  };
}
namespace Options {
  std::ostream& operator <<(std::ostream& out, const RT rt) {
    return out << Environment::W2(rt);
  }
  std::ostream& operator <<(std::ostream& out, const PropO po) {
    return out << Environment::W2(po);
  }
  std::ostream& operator <<(std::ostream& out, const BHV bvar) {
    return out << Environment::W2(bvar);
  }
  std::ostream& operator <<(std::ostream& out, const BRT bt) {
    return out << Environment::W2(bt);
  }
  std::ostream& operator <<(std::ostream& out, const GBO bo) {
    return out << Environment::W2(bo);
  }
  std::ostream& operator <<(std::ostream& out, const BHO bord) {
    return out << Environment::W2(bord);
  }
  std::ostream& operator <<(std::ostream& out, const LAT lah) {
    return out << Environment::W2(lah);
  }

}

#endif
