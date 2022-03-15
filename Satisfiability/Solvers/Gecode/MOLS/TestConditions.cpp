// Oliver Kullmann, 8.3.2022 (Swansea)
/* Copyright 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

#include <iostream>
#include <sstream>

#include <cassert>

#include <ProgramOptions/Environment.hpp>

#include "Conditions.hpp"

namespace {

  const Environment::ProgramInfo proginfo{
        "0.3.1",
        "15.3.2022",
        __FILE__,
        "Oliver Kullmann",
        "https://github.com/OKullmann/OKlib-MOLS/blob/master/Satisfiability/Solvers/Gecode/MOLS/TestConditions.cpp",
        "GPL v3"};

  using namespace Conditions;

  template <class X>
  constexpr bool eqp(const X& lhs, const X& rhs) noexcept {
    return lhs == rhs;
  }
}

int main(const int argc, const char* const argv[]) {
  if (Environment::version_output(std::cout, proginfo, argc, argv))
  return 0;

  {assert(toUC("") == UC(0));
   assert(toUC("UNDEF") == UC(0));
   assert(toUC("rls") == UC(1));
   assert(toUC("antisymm") == UC(maxUC));
  }

  {assert(toVS("") == VS::id);
   assert(toVS("id") == VS::id);
   assert(toVS("at") == VS::at);
  }

  {Versions v;
   assert(v.choices().size() == 1);
   assert(eqp(v, {}));
   assert(eqp(v.choices(), {VS::id}));
   assert(not v.insert(VS::id));
   assert(v.contains(VS::id));
   assert(not v.contains(VS::at));
   assert(v.insert(VS::at));
   assert(v.choices().size() == 2);
   assert(v.contains(VS::at));
   assert(Versions{} < v);
   const Versions old(v);
   assert(not v.insert(VS::at));
   assert(v == old);
   Versions v2(v), v3(v);
   assert(v2.insert(VS::c213));
   assert(v3.insert(VS::c312));
   assert(v2 != v3);
   assert((v2 < v3));
   assert(not (v3 <= v2));
   assert((v3 == Versions{VS::id, VS::at, VS::c312}));
   assert((v3 == Versions({VS::id, VS::at, VS::c312})));
  }

  {// Square s; not allowed
   Square s{1};
   assert(s == Square(1));
   assert(eqp(s, {1}));
   assert(s == Square(1,VS::id));
   assert(s.v == VS::id);
   const Square s2(1, VS::at);
   assert(s < s2);
   s.v = VS::at;
   assert(s == s2);
   assert(eqp(s, {1,VS::at}));
  }

  {const Equation e({0},{1});
   assert(e == Equation({1},{0}));
   assert(eqp(e.lhs(), {0}));
   assert(eqp(e.rhs(), {1}));
   const Equation e2({5,VS::c321},{3,VS::c231});
   assert(eqp(e2.lhs(), {3,VS::c231}));
   assert(eqp(e2.rhs(), {5,VS::c321}));
   assert(e < e2);
   static_assert(Equation{{0},{1}} == Equation{{1},{0}});
   std::ostringstream ss;
   ss << e;
   assert(ss.str() == "= 0  1");
   ss.str("");
   ss << e2;
   assert(ss.str() == "= c231 3  c321 5");
  }

  {assert(toPT("") == PT(0));
   assert(toPT("rprod") == PT::rprod);
   assert(toPT("cprod") == PT::cprod);
  }

  {const ProdEq pe({0},{1},{2});
   assert(eqp(pe.r(), {0}));
   assert(eqp(pe.f2(), {1}));
   assert(eqp(pe.f1(), {2}));
   assert(pe.pt() == PT::rprod);
   assert(pe == ProdEq({0},{1},{2}));
   const ProdEq pe2({0},{1},{2}, PT::cprod);
   assert(pe < pe2);
   std::ostringstream ss;
   ss << pe;
   assert(ss.str() == "rprod 0  1  2");
   ss.str("");
   ss << pe2;
   assert(ss.str() == "cprod 0  1  2");
  }

  {const AConditions C(0);
   assert(C.k == 0);
   assert(eqp(C.versions(), {}));
   assert(eqp(C.map(), {}));
   assert(eqp(C.eq(), {}));
   assert(eqp(C.peq(), {}));
   assert(C.num_squares() == 0);
   assert(not C.contains({0}));
   assert(not C.contains(UC::diag, {0}));
   assert(not C.contains({{0}, {0}}));
   assert(not C.valid(Square{0}));
   assert(not C.valid(Equation{0,0}));
   assert(not C.valid(ProdEq{0,0,0}));
   std::ostringstream ss;
   ss << C;
   assert(ss.str() == "squares\n");
   assert(not ss.bad());
  }

  {const AConditions C(1);
   assert(C.k == 1);
   assert(eqp(C.versions(), {{}}));
   assert(C.map().empty());
   assert(eqp(C.eq(), {}));
   assert(eqp(C.peq(), {}));
   assert(C.num_squares() == 1);
   assert(C.contains({0}));
   assert(not C.contains(UC::diag, {0}));
   assert(not C.contains({{0}, {0}}));
   assert(C.valid(Square{0}));
   assert(C.valid(Equation{0,0}));
   assert(C.valid(ProdEq{0,0,0}));
   {std::ostringstream ss;
    ss << C;
    assert(ss.str() == "squares 0\n");
    assert(not ss.bad());
   }
   AConditions C2(C);
   assert(C2 == C);
   assert(not C2.insert(Square{0}));
   assert(C2 == C);
   assert(C2.insert(Square{0,VS::at}));
   assert(C2.map().empty());
   assert(C2 != C);
   assert(C2.num_squares() == 2);
   assert(C2.contains(Square{0,VS::at}));
   assert(C2.sqs(UC::diag).empty());
   assert(C2.insert(UC::diag, Square{0,VS::at}));
   assert(C2.map().size() == 1);
   assert(eqp(C2.sqs(UC::diag), {{0,VS::at}}));
   assert(C2.num_squares() == 2);
   assert(not C2.contains(Equation{0,0}));
   {std::ostringstream ss;
    ss << C2;
    assert(ss.str() == "squares 0\ndiag \tat 0\n");
    assert(not ss.bad());
   }
   const AConditions C3(C2);
   assert(C2.insert(Equation{0,0}));
   assert(C3 != C2);
   assert(C2.contains(Equation{0,0}));
   assert(C2.insert(Equation{0,{0,VS::at}}));
   const AConditions C4(C2);
   assert(not C2.contains(ProdEq{0,0,0}));
   assert(C2.insert(ProdEq{0,0,0}));
   assert(eqp(C2.peq(), {{0,0,0}}));
   assert(C4 != C2);
   {std::ostringstream ss;
    ss << C2;
    assert(ss.str() == "squares 0\ndiag \tat 0\n= 0  0\n= 0  at 0\nrprod 0  0  0\n");
    assert(not ss.bad());
   }
   assert(C2.insert(UC::diag, Square{0,VS::c312}));
   assert(C2.insert(UC::ls, Square{0,VS::c321}));
   {std::ostringstream ss;
    ss << C2;
    assert(ss.str() == "squares 0\nls \tc321 0\ndiag \tc312 0  at 0\n= 0  0\n= 0  at 0\nrprod 0  0  0\n");
    assert(not ss.bad());
   }
  }

}
