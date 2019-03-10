// Oliver Kullmann, 5.3.2019 (Swansea)
/* Copyright 2019 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/* Timing of function bernoulli(g)

Example (annotation on next line):

Random> ./TimingBernoulli12 3e9
# number N of calls of the generator (this is the default value)
TimingBernoulli12 0.1.0 5.3.2019 e02529429074fab816a0bcc281623bfea73ca223
3e+09
# name, version, last change-date, git ID
g++ 8.3.0 Mar  5 2019 00:43:14
# compiler-version, compilation-date
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
# optimisation options
3000000000 1499961918 0.499987306
# the argument plus the count of results "true" and their relative frequency
3e+09
# N again, in float80-precision.


Results:

On csltok:

Random> time ./TimingBernoulli12
0.1.4 10.3.2019 TimingBernoulli12 55c8a21b600a40817a4b729b2948b86cee6b57e0
g++ 8.3.0 Mar 10 2019 04:21:59
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
3000000000 1499961918 0.499987306
3e+09
real    0m14.500s
user    0m14.460s
sys     0m0.001s

So roughly 207e6 generations per sec, which is 91.6% of the pure
generation-speed.


On cs-wsok:

Random> time ./TimingBernoulli12
0.1.4 10.3.2019 TimingBernoulli12 55c8a21b600a40817a4b729b2948b86cee6b57e0
g++ 8.3.0 Mar 10 2019 04:22:40
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
3000000000 1499961918 0.499987306
3e+09
real    0m10.314s
user    0m10.305s
sys     0m0.001s

So roughly 291e6 generations per sec, which is 92.1% of pure
generation-speed.



On csverify:

Random$ time ./TimingBernoulli12
0.1.4 10.3.2019 TimingBernoulli12 55c8a21b600a40817a4b729b2948b86cee6b57e0
g++ 8.2.0 Mar 10 2019 04:24:12
-Ofast -DNDEBUG -march=native -fwhole-program -static -fno-finite-math-only -fno-unsafe-math-optimizations -fno-associative-math -fno-reciprocal-math  -fno-signed-zeros -fno-math-errno -fno-trapping-math
3000000000 1499961918 0.499987306
3e+09
real    0m5.675s
user    0m5.675s
sys     0m0.000

So roughly 529e6 generations per sec, which is 93.4% of pure
generation-speed.


It is actually surprising that adding just one small test to the generation
slows it down by 7%: the generation should be a much more involved computation,
and so the effect of adding this test should be smaller? Apparently the
computation of g() is very fast, so that such a small addition matters.

*/

#include <iostream>

#include <Numerics/FloatingPoint.hpp>

#include "Numbers.hpp"
#include "Environment.hpp"

namespace {

  const Environment::ProgramInfo proginfo{
        "0.1.4",
        "10.3.2019",
        "TimingBernoulli12"};

  using namespace RandGen;

  constexpr gen_uint_t N_default = 3e9L;

}

int main(const int argc, const char* const argv[]) {

  const gen_uint_t N = (argc == 1) ? N_default : FloatingPoint::toUInt(argv[1]);


  gen_uint_t count_true = 0;
  randgen_t g;
  for (gen_uint_t i = 0; i < N; ++i) count_true += bernoulli(g);


  std::cout << proginfo;

  using FloatingPoint::float80;
  std::cout << N << " " << count_true << " " << FloatingPoint::Wrap(float80(count_true) / N) << "\n";
  std::cout << float80(N) << "\n";

}
