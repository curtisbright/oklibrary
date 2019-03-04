// Oliver Kullmann, 4.3.2019 (Swansea)
/* Copyright 2019 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/* Timing of discard-function for Mersenne-Twister

Example (annotation on next line)

Random> time ./TimingDiscard 1e7 1e3
# number of discarded states and number of rounds (these are the default values)
TimingDiscard 0.1.0 4.3.2019 88d743a28de5bf0d1840e4fa83bc88ab2295488f
# name, version, last change-date, git ID
g++ 8.3.0 Mar  4 2019 17:25:19
# compiler-version, compilation-date
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
# optimisation options
10000000 1000 6991338432609355100
# Both arguments plus the output of the generator at the end
1e+10
# The product of discards and rounds, in float80-precision.

real    0m13.112s
user    0m13.077s
sys     0m0.002s

Results:

On csltok:
XXX

*/

#include <iostream>

#include <Numerics/FloatingPoint.hpp>

#include "Numbers.hpp"

namespace {

  const std::string version = "0.1.0";
  const std::string date = "4.3.2019";
  const std::string program = "TimingDiscard";

  const std::string compilation_date = __DATE__ " " __TIME__;
  const std::string compiler_version =
#ifdef __GNUC__
   "g++ " __VERSION__
#else
   "Compiler not gcc"
#endif
;
  const std::string git_id = GITID;
  const std::string optimisation = OPTIMISATION;

  using namespace RandGen;

  constexpr gen_uint_t discard_default = 1e7L;
  constexpr gen_uint_t rounds_default = 1000;

}

int main(const int argc, const char* const argv[]) {

  randgen_t g;

  const gen_uint_t discard = (argc == 1) ? discard_default : FloatingPoint::toUInt(argv[1]);
  const gen_uint_t rounds = (argc <= 2) ? rounds_default : FloatingPoint::toUInt(argv[2]);


  for (gen_uint_t i = 0; i < rounds; ++i) g.discard(discard);


  std::cout << program << " " << version << " " << date << " " << git_id << "\n";
  std::cout << compiler_version << " " << compilation_date << "\n";
  std::cout << optimisation << "\n";

  std::cout << FloatingPoint::Wrap(discard) << " " << FloatingPoint::Wrap(rounds) << " " << g() << "\n";

  std::cout << FloatingPoint::float80(discard) * rounds << "\n";

}
