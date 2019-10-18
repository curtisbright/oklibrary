// Oliver Kullmann, 17.7.2019 (Swansea)
/* Copyright 2019 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

#include <iostream>

#include <cassert>

#include <ProgramOptions/Environment.hpp>

#include "Numbers.hpp"
#include "DQClauseSets.hpp"

namespace {

  const Environment::ProgramInfo proginfo{
        "0.0.8",
        "18.10.2019",
        __FILE__,
        "Oliver Kullmann",
        "https://github.com/OKullmann/oklibrary/blob/master/Satisfiability/Transformers/Generators/Random/TestDQClauseSets.cpp",
        "GPL v3"};

  using namespace RandGen;

  std::ostream& operator <<(std::ostream& out, const ae_pair p) {
    return out << "(" << p.first << "," << p.second << ")";
  }
  [[maybe_unused]] std::ostream& operator <<(std::ostream& out, const dep_edges& depe) {
    for (const auto e : depe) out << " " << e;
    return out;
  }

}

int main(const int argc, const char* const argv[]) {
  if (Environment::version_output(std::cout, proginfo, argc, argv))
    return 0;

  {assert((extract_numvars({{2,Q::both},{1,Q::fa},{{2,2},Q::ex}}) == ae_numvars{1,1}));
   assert((extract_numvars({{7,Q::both},{2,Q::fa},{{3,5},Q::ex},{{6,6},Q::fa},{{7,7},Q::ex}}) == ae_numvars{3,4}));
  }

  {assert((num_dependencies({{1,Q::ex},{1,Q::ex}}) == 0));
   assert((num_dependencies({{10,Q::ex},{1,Q::ex},{{2,10},Q::ex}}) == 0));
   assert((num_dependencies({{10,Q::both},{9,Q::fa},{{10,10},Q::ex}}) == 9));
   assert((num_dependencies({{20,Q::both},{9,Q::fa},{{10,10},Q::ex},{{11,15},Q::ex},{{16,16},Q::fa},{{17,20},Q::ex}}) == 6*9 + 4*10));
  }

  {assert((extract(block_v{{1,Q::ex},{1,Q::ex}}) == ablock_v{}));
   assert((extract(block_v{{10,Q::ex},{1,Q::ex},{{2,10},Q::ex}}) == ablock_v{}));
   assert((extract(block_v{ {21,Q::both},{9,Q::fa},{{10,10},Q::ex},{{11,15},Q::ex},{{16,16},Q::fa},{{17,20},Q::fa}, {{21,21},Q::ex} }) == ablock_v{ {{1,9},{1,9}}, {{16,16},{10,10}}, {{17,20},{11,14}} }));
  }

  {AccessA aa(block_v{{1,Q::ex},{1,Q::ex}});
   assert(aa.max == 0);
   assert(aa.na == 0);
   assert(aa(0) == 0);
   assert(aa[0] == 0);
  }
  {AccessA aa(block_v{ {21,Q::both},{9,Q::fa},{{10,10},Q::ex},{{11,15},Q::ex},{{16,16},Q::fa},{{17,20},Q::fa}, {{21,21},Q::ex} });
   assert(aa.max == 20);
   assert(aa.na == 14);
   assert(aa(0) == 14);
   assert(aa(1) == 1);
   assert(aa(9) == 9);
   assert(aa(16) == 10);
   assert(aa(17) == 11);
   assert(aa(20) == 14);
   assert(aa[0] == 20);
   assert(aa[1] == 1);
   assert(aa[9] == 9);
   assert(aa[10] == 16);
   assert(aa[11] == 17);
   assert(aa[14] == 20);
  }
  {assert((translate(vec_eseed_t{}, 0, 1, block_v{ {1,Q::ex},{1,Q::ex} }, DepOp::from_scratch) == dep_edges{}));
   assert((translate(vec_eseed_t{}, 1, 1, block_v{ {2,Q::both},{1,Q::fa},{{2,2},Q::ex} }, DepOp::from_scratch) == dep_edges{}));
   assert((translate(vec_eseed_t{0}, 1, 1, block_v{ {2,Q::both},{1,Q::fa},{{2,2},Q::ex} }, DepOp::from_scratch) == dep_edges{{1,0}}));
   assert((translate(vec_eseed_t{0,3}, 2, 2, block_v{ {4,Q::both},{2,Q::fa},{{3,4},Q::ex} }, DepOp::from_scratch) == dep_edges{{1,0}, {2,1}}));
   assert((translate(vec_eseed_t{0,1,3,4,22,24}, 5, 5, block_v{ {10,Q::both},{2,Q::fa},{{3,4},Q::ex},{{5,7},Q::fa},{{8,10},Q::ex} }, DepOp::from_scratch) == dep_edges{{1,0},{2,0},{6,0},{7,0}, {5,4},{7,4}}));

   assert((translate(vec_eseed_t{}, 0, 1, block_v{ {1,Q::ex},{1,Q::ex} }, DepOp::subtract) == dep_edges{}));
   assert((translate(vec_eseed_t{}, 1, 1, block_v{ {2,Q::both},{1,Q::fa},{{2,2},Q::ex} }, DepOp::subtract) == dep_edges{}));
   assert((translate(vec_eseed_t{0}, 1, 1, block_v{ {2,Q::both},{1,Q::fa},{{2,2},Q::ex} }, DepOp::subtract) == dep_edges{{1,0}}));
   assert((translate(vec_eseed_t{0,3}, 2, 2, block_v{ {4,Q::both},{2,Q::fa},{{3,4},Q::ex} }, DepOp::subtract) == dep_edges{{1,0}, {2,1}}));
   assert((translate(vec_eseed_t{0,1,3,4,15,18}, 5, 5, block_v{ {10,Q::both},{2,Q::fa},{{3,4},Q::ex},{{5,7},Q::fa},{{8,10},Q::ex} }, DepOp::subtract) == dep_edges{{1,0},{2,0},{2,1}, {1,2},{2,4},{7,4}}));
   // XXX
  }

}
