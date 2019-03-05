// Oliver Kullmann, 4.3.2019 (Swansea)
/* Copyright 2019 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/* Timing of generation-function for Mersenne-Twister

Example (annotation on next line)

Random> ./TimingGeneration 3e9
# number N of calls of the generator (this is the default value)
TimingGeneration 0.1.0 4.3.2019 4e56d2ade745848a4bf12d947f9b3717b41c6bef
# name, version, last change-date, git ID
g++ 8.3.0 Mar  4 2019 23:34:05
# compiler-version, compilation-date
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
# optimisation options
3000000000 9259490176346246065
# the argument plus the sum of all generated numbers (as unsigned 64bit)
3e+09
# N again, in float80-precision.


Results:

On csltok:

Random> time ./TimingGeneration
TimingGeneration 0.1.0 4.3.2019 e766a1e8ac1eecf83631da3256e11b7ded4db161
g++ 8.3.0 Mar  4 2019 23:49:52
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
3000000000 9259490176346246065
3e+09

real    0m13.392s
user    0m13.285s
sys     0m0.072s

So roughly 226e6 generations per sec.


On cs-wsok:

Random> time ./TimingGeneration
TimingGeneration 0.1.0 4.3.2019 e766a1e8ac1eecf83631da3256e11b7ded4db161
g++ 8.3.0 Mar  4 2019 23:51:50
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
3000000000 9259490176346246065
3e+09
real    0m9.574s
user    0m9.568s
sys     0m0.000s

So roughly 316e6 generations per sec.

On csverify:

Random$ time ./TimingGeneration
TimingGeneration 0.1.0 4.3.2019 e766a1e8ac1eecf83631da3256e11b7ded4db161
g++ 8.2.0 Mar  4 2019 23:53:41
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
3000000000 9259490176346246065
3e+09
real    0m5.307s
user    0m5.307s
sys     0m0.000s

So roughly 566e6 generations per sec.

*/

#include <iostream>

#include <Numerics/FloatingPoint.hpp>

#include "Numbers.hpp"
#include "Environment.hpp"

namespace {

  const std::string version = "0.1.1";
  const std::string date = "5.3.2019";
  const std::string program = "TimingGeneration";

  using namespace RandGen;

  constexpr gen_uint_t N_default = 3e9L;

}

int main(const int argc, const char* const argv[]) {

  const gen_uint_t N = (argc == 1) ? N_default : FloatingPoint::toUInt(argv[1]);


  gen_uint_t sum = 0;
  randgen_t g;
  for (gen_uint_t i = 0; i < N; ++i) sum += g();


  Environment::output_environment(std::cout, program, version, date);

  std::cout << N << " " << sum << "\n";

  std::cout << FloatingPoint::float80(N) << "\n";

}
