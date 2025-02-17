// Oliver Kullmann, 18.4.2019 (Swansea)
/* Copyright 2019, 2021, 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

#include <iostream>
#include <sstream>

#include <ProgramOptions/Environment.hpp>

#include "Numbers.hpp"

namespace {

  const Environment::ProgramInfo proginfo{
        "0.3.1",
        "22.3.2022",
        __FILE__,
        "Oliver Kullmann",
        "https://github.com/OKullmann/oklibrary/blob/master/Satisfiability/Transformers/Generators/Random/TestNumbers.cpp",
        "GPL v3"};

  using namespace RandGen;

}

int main(const int argc, const char* const argv[]) {
  if (Environment::version_output(std::cout, proginfo, argc, argv))
  return 0;

  {assert(is_seed_t({}));
   assert(is_seed_t({0}));
   assert(is_seed_t({iexp2(32)-1}));
   assert(not is_seed_t({iexp2(32)}));
   assert(not is_seed_t({randgen_max}));
   assert((is_seed_t({0,0})));
   assert((is_seed_t({iexp2(32)-1,iexp2(32)-1})));
   assert((not is_seed_t({iexp2(32),iexp2(32)-1})));
   assert((not is_seed_t({iexp2(32)-1,iexp2(32)})));
  }

  {assert(transform({}) == vec_seed_t{});
   assert((transform({0}) == vec_seed_t{0,0}));
   assert((transform({1}) == vec_seed_t{1,0}));
   assert((transform({iexp2(32)}) == vec_seed_t{0,1}));
   assert((transform({gen_uint_t(-1)}) == vec_seed_t{seed_t(-1),seed_t(-1)}));
   assert((transform({0,0}) == vec_seed_t{0,0,0,0}));
   assert((transform({1,1}) == vec_seed_t{1,0,1,0}));
   assert((transform(vec_eseed_t(2,iexp2(32))) == vec_seed_t{0,1,0,1}));

   assert(transform({},SP::trunc) == vec_seed_t{});
   assert((transform({0},SP::trunc) == vec_seed_t{0}));
   assert((transform({1},SP::trunc) == vec_seed_t{1}));
   assert((transform({iexp2(32)},SP::trunc) == vec_seed_t{0}));
   assert((transform({gen_uint_t(-1)},SP::trunc) == vec_seed_t{seed_t(-1)}));
   assert((transform({0,0},SP::trunc) == vec_seed_t{0,0}));
   assert((transform({1,1},SP::trunc) == vec_seed_t{1,1}));
   assert((transform(vec_eseed_t(2,iexp2(32)),SP::trunc) == vec_seed_t{0,0}));

   assert(transform({},SP::check) == vec_seed_t{});
   assert((transform({0},SP::check) == vec_seed_t{0}));
   assert((transform({1},SP::check) == vec_seed_t{1}));
   assert((transform({iexp2(32)},SP::check) == vec_seed_t{0,1}));
   assert(transform({gen_uint_t(-1)},SP::check) == vec_seed_t(2,seed_t(-1)));
   assert((transform({0,0},SP::check) == vec_seed_t{0,0}));
   assert((transform({1,1},SP::check) == vec_seed_t{1,1}));
   assert((transform(vec_eseed_t(2,iexp2(32)),SP::check) == vec_seed_t{0,1,0,1}));
   assert((transform(vec_eseed_t{iexp2(32),1},SP::check) == vec_seed_t{0,1,1,0}));
   assert((transform(vec_eseed_t{1,iexp2(32),1},SP::check) == vec_seed_t{1,0,0,1,1,0}));
  }

  {assert(valid_ascii(""));
   assert((valid_ascii({0,127})));
   assert((valid_ascii({0,1,2,126,127})));
   assert((not valid_ascii({char(-1)})));
  }

  {assert(transform({}, EP::four) == vec_seed_t{});
   assert(transform({0}, EP::four) == vec_seed_t{0});
   assert(transform({0}, EP::one) == vec_seed_t{0});
   assert((transform({0,0}, EP::four) == vec_seed_t{0}));
   assert((transform({0,0}, EP::one) == vec_seed_t{0,0}));
   assert((transform({0,0,1,127}, EP::one) == vec_seed_t{0,0,1,127}));
   assert((transform({0,0,1,127}, EP::four) == vec_seed_t{0x7F'01'00'00}));
   assert((transform({1}, EP::four) == vec_seed_t{1}));
   assert((transform({1,2}, EP::four) == vec_seed_t{0x2'01}));
   assert((transform({1,2,3}, EP::four) == vec_seed_t{0x3'02'01}));
   assert((transform({1,2,3,4}, EP::four) == vec_seed_t{0x4'03'02'01}));
   assert((transform({1,2,3,4,5}, EP::four) == vec_seed_t{0x4'03'02'01, 5}));
   assert((transform({1,2,3,4,5}, EP::one) == vec_seed_t{1,2,3,4,5}));
   assert((transform("12345", EP::one) == vec_seed_t{49,50,51,52,53}));
   assert((transform("12345", EP::four) == vec_seed_t{0x34'33'32'31, 0x35}));
   assert((transform("xyz{|}~^", EP::four) == vec_seed_t{0x7b'7a'79'78, 0x5e'7e'7d'7c}));
   assert((transform("xyz{|}~^A", EP::four) == vec_seed_t{0x7b'7a'79'78, 0x5e'7e'7d'7c, 0x41}));
  }

  {seed_t x = 0, y = 0;
   split(0,x,y);
   assert(x == 0 and y == 0);
   split(iexp2(32)-1,x,y);
   assert(x == FloatingPoint::P232m1 and y == 0);
   split(iexp2(32),x,y);
   assert(x == 0 and y == 1);
   split(-1,x,y);
   assert(x == FloatingPoint::P232m1 and y == FloatingPoint::P232m1);
  }

  {seed_t x = 0, y = 0;
   inc(x,y);
   assert(x == 1 and y == 0);
   inc(y,x);
   assert(y == 1 and x == 1);
   x = -1;
   inc(x,y);
   assert(x == 0 and y == 2);
   y = -1; x = -1;
   inc(x,y);
   assert(x == 0 and y == 0);
  }

  {Prob64 p{1,2};
   assert(p == Prob64(11,22));
   Prob64 p2(pair64{4,8});
   assert(p2 == p);
   Prob64 p3(p2);
   assert(p3 == p2);
   p3 = {7,9};
   assert(p3 == Prob64(14,18));
  }

  {const Prob64 p(1,2);
   std::stringstream s;
   s << p;
   assert(s.str() == "1/2");
   assert(not toProb64(""));
   assert(not toProb64("12"));
   assert(not toProb64("12/"));
   assert(not toProb64("2/1"));
   {bool has_thrown = false;
    try { toProb64("-1ab/2"); }
    catch(const std::invalid_argument&) { has_thrown = true; }
    assert(has_thrown);
   }
   assert(not toProb64("-1/-2"));
   assert((toProb64("inf/inf").value() == Prob64{1,1}));
   assert((toProb64("1/inf").value() == Prob64{1,FloatingPoint::P264m1}));
   assert(toProb64("001e10/002e10") == p);
   const auto p2 = toProb64(s.str());
   assert(p2);
   assert(p2.value() == p);
  }

  {randgen_t g;
   for (int i = 0; i < 9'999; ++i) g();
   assert(g() == specval);
  }
  {randgen_t g;
   g.discard(9999);
   assert(g() == specval);
  }

  {RandGen_t g0;
   assert(g0 == RandGen_t());
   {RandGen_t g1(g0), g2({0});
    assert(g1 == g0);
    assert(g2 != g1);
    g2 = g1;
    assert(g2 == g1);
   }
   const randgen_t g1 = g0.extract();
   constexpr int basis = 18; // 32, the whole seed_t-range, indeed must work
   for (gen_uint_t i = 0; i < iexp2(basis); ++i) {
     const randgen_t g(i);
     assert(g != g1);
   }
  }

  {RandGen_t g;
   assert(g() == valempty_1);
   assert(g() == valempty_2);
   assert(g() == valempty_3);
   g.discard(9997);
   assert(g() == valempty_10001);
   g.discard(9999);
   assert(g() == valempty_20001);
   g.discard(9999);
   assert(g() == valempty_30001);
  }

  {RandGen_t g1({});
   randgen_t g2(init({}));
   assert(g1.extract() == g2);
  }
  {RandGen_t g1(vec_eseed_t{});
   randgen_t g2(init({}));
   assert(g1.extract() == g2);
  }
  {RandGen_t g1({1,2});
   randgen_t g2(init({1,2}));
   assert(g1.extract() == g2);
  }
  {RandGen_t g1({1L,2L});
   randgen_t g2(init({1,2}));
   assert(g1.extract() == g2);
  }
  {RandGen_t g1{1L,2L};
   randgen_t g2(init({1,2}));
   assert(g1.extract() == g2);
  }
  {RandGen_t g1(vec_eseed_t{1,2});
   randgen_t g2(init({1,0,2,0}));
   assert(g1.extract() == g2);
  }
  {RandGen_t g1(vec_eseed_t{1,2});
   RandGen_t g2{1,0,2,0};
   assert(g1.extract() == g2.extract());
  }
  {RandGen_t g1(vec_eseed_t{1,2});
   RandGen_t g2(vec_seed_t{1,0,2,0});
   assert(g1.extract() == g2.extract());
  }


  {const gen_uint_t r1 = device_to_eseed();
   const gen_uint_t r2 = device_to_eseed();
   assert(r1 != r2); // equality is very unlikely: probability = 2^-64
  }

  {const gen_uint_t t1 = timestamp_to_eseed();
   const gen_uint_t t2 = timestamp_to_eseed();
   assert(t1 < t2); // creating a timestamp should take a noticable time
  }

  {assert(to_gen_uint_t("0018446744073709551615",false) == 18446744073709551615ULL);
   bool thrown = false;
   try { to_gen_uint_t("18446744073709551615XXX",true); }
   catch (const std::domain_error& e) {
     thrown = true;
     assert(std::string_view(e.what()) == "RandGen::to_gen_uint_t(string), trailing: \"XXX\" in \"18446744073709551615XXX\"");
   }
   assert(thrown);
  }

  {assert(to_eseed("12345678901") == 12345678901ULL);
  }

}
