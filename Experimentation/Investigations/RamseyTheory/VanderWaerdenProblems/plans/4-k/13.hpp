// Oliver Kullmann, 11.3.2011 (Swansea)
/* Copyright 2011 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*!
  \file Experimentation/Investigations/RamseyTheory/VanderWaerdenProblems/plans/4-k/13.hpp
  \brief On investigations into vdw_2(4,13)


  \todo Best ubcsat-solver
  <ul>
   <li> We need to find the best algorithm from the ubcsat-1-2-0 suite. </li>
  </ul>


  \todo vanderwaerden_2(4,13) > 562
  <ul>
   <li> Certificate for n=562:
   \verbatim
8,21,22,30,35,38,44,45,53,55,
56,60,61,64,74,76,79,80,81,85,
87,91,93,95,96,102,109,118,119,132,
141,144,150,153,156,157,158,163,167,173,
177,178,181,182,188,190,199,202,206,216,
225,229,232,238,239,241,243,249,250,255,
258,260,268,273,274,275,278,279,281,285,
287,289,290,299,303,312,322,329,336,338,
340,344,346,350,351,355,361,365,367,370,
375,378,386,387,393,396,409,410,419,423,
432,437,441,444,447,448,449,452,454,458,
462,464,467,468,469,475,479,483,484,490,
497,498,506,507,520,523,529,530,532,540,
541,544,545,546,551,561
   \endverbatim
   (found by ddfw, adapted from n=537), or, another run under the same
   conditions:
   \verbatim
8,21,22,30,35,38,44,45,53,55,
56,60,61,64,70,74,76,79,80,81,
85,87,91,93,95,96,102,109,118,119,
132,141,144,150,153,156,157,158,163,167,
173,177,178,181,182,188,190,199,202,206,
216,225,229,232,238,239,241,243,249,250,
255,258,260,268,273,274,275,278,279,281,
285,287,289,290,299,303,312,322,329,336,
338,340,344,346,350,351,355,361,365,367,
370,375,378,386,387,393,396,409,410,419,
423,432,437,441,444,447,448,449,452,454,
458,462,464,467,468,469,475,479,483,484,
490,497,498,506,507,520,523,529,530,532,
540,541,544,545,546,551,555,561
   \endverbatim
   </li>
   <li> Start with the palindromic solution for n=537. XXX </li>
   <li> Finding best ubcsat-algorithm:
   \verbatim
> VanderWaerdenCNF-O3-DNDEBUG 4 13 537

> E=run_ubcsat("VanDerWaerden_2-4-13_537.cnf",runs=100,cutoff=1000000)
> eval_ubcsat_dataframe(E,FALSE)
1. anovp:
16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
 2  3  4  4  8  8 13 10 12 15 10  4  4  2  1
fps: 93405
2. dano:
16 18 19 20 21 22 23 24 25 26 27 28 29 31 33
 2  1  4  3  7 15 12 14 11 11  7  9  2  1  1
fps: 93264
3. anovpp:
18 19 20 21 22 23 24 25 26 27 28 31
 2  9  4  6 13 16 18 10  8  9  3  2
fps: 93366
4. ddfw:
18 19 20 21 22 23 24 25 26
 1  1  4  9 16 25 22 18  4
fps: 27533
5. ag2wsat:
21 22 23 24 25 26 27 28 29
 2  5 11  7 18 26 20  8  3
fps: 146774
6. rots:
21 22 23 24 25 26 27 28
 2  3 13 18 27 20 14  3
fps: 154040
7. rnovp:
21 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
 1  4  3  3  3  5 10  8  9 11 14  7 14  4  1  3
fps: 93723
8. gsatt:
22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 39 40 42 60
 1  2 12  9 10  7  6  7 14  9  8  2  5  2  1  1  1  1  1  1
fps: 224972
9. rnov:
22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
 1  1  3  2  2  5  4  5 10  9  9 11  3 11 10  7  5  2
fps: 93557
10. samd:
23 24 25 26 27 28 29 30 31 32 33 34 35 36 40
 3  3 12  6 13 11 11 11  6 10  6  4  1  2  1
fps: 225124
11. gwsat:
23 26 27 28 29 30 31 32 33 34 35
 1  3  3  4 10 16 28 17 10  7  1
fps: 156250

E2=run_ubcsat("VanDerWaerden_2-4-13_537.cnf",runs=100,cutoff=10000000, include_algs=list("anovp","dano","anovpp","ddfw","ag2wsat","rots"))
ddfw
 0 16 18 19 20 21 22 23
 1  1  5 16 27 34 15  1
FlipsPerSecond = 27539
anovpp
14 15 16 17 18 19 20 21 22
 1  1  2  7 19 24 32 13  1
FlipsPerSecond = 93276
dano
15 16 17 18 19 20 21 22 23
 1  5  8 13 20 25 19  7  2
FlipsPerSecond = 93319
anovp
16 17 18 19 20 21 22
 1  4 27 24 27 15  2
FlipsPerSecond = 93285
rots
16 20 21 22 23 24 25
 1  3 26 31 28 10  1
FlipsPerSecond = 154644
ag2wsat
18 19 20 21 22 23 24 25
 1  5 14 20 23 26  9  2
FlipsPerSecond = 147198

> cat ubcsat_tmp_VanDerWaerden_2-4-13_537.cnf_2011-09-27-142225/ddfw.run_ubcsat_result
     23 1     0              1173076              1173076 1620845361
> ubcsat-okl -alg ddfw -seed 1620845361 -cutoff 1173076 -i VanDerWaerden_2-4-13_537.cnf -solve > VanDerWaerden_2-4-13_537_OUT
> cat VanDerWaerden_2-4-13_537_OUT | extract_solution_ubcsat > VanDerWaerden_2-4-13_537.sol
   \endverbatim
   Might be pure chance, that ddfw is best. But so well, let's try it. </li>
   <li> "RunVdWk1k2 4 13 537 ddfw 1000 10000000 VanDerWaerden_2-4-13_537.sol"
   for n=537,...,562 finds solutions in the first run with the old solution,
   and then
   \verbatim
> E=read_ubcsat("VanDerWaerden_2-4-13_563.cnf_OUT")
 1 24 25 26 27 28 29 30 31 32
 1  2  9 14 19 54 85 45 26  2
257
   \endverbatim
   Running the same command again: XXX
   </li>
  </ul>


  \todo vdw_2^pd(4,13) >= (519,538)
  <ul>
   <li> The conjecture is vdw_2^pd(4,13) = (519,538) (due to a very clear
   separation between satisfiable und supposedly unsatisfiable instances).
   </li>
   <li> Certificates:
    <ol>
     <li> n=518:
     \verbatim
11,13,26,27,28,31,34,38,43,49,
52,65,66,79,82,88,89,93,100,103,
104,105,111,118,120,123,124,128,129,131,
135,137,140,146,153,163,172,176,179,186,
188,190,196,197,200,205,207,211,215,217,
220,221,222,226,228,234,236,237,246,250,
259
     \endverbatim
     </li>
     <li> n=537:
     \verbatim
2,15,19,21,22,26,34,35,38,39,
44,46,59,68,70,72,73,75,77,78,
80,82,91,99,104,106,111,112,115,116,
128,129,131,134,135,136,148,156,159,165,
167,170,175,185,196,201,203,209,210,212,
213,215,216,225,226,229,231,233,238,240,
253,256,262,264,266,267,269
     \endverbatim
    </ol>
   </li>
   <li> Splitting the problem for n=520:
   \verbatim
> SplittingViaOKsolver -D36 VanDerWaerden_pd_2-4-13_520.cnf
> cd SplitViaOKsolver_D36VanDerWaerden_pd_2413_520cnf_2011-08-16-014738
kullmann-0:SplitViaOKsolver_D36VanDerWaerden_pd_2413_520cnf_2011-08-16-014738> cat Md5sum
d7d5e27d64ecbc33f704eb5ea4dcdd17
> cat Statistics
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
   36.0    36.0    38.0    38.1    39.0    74.0
> table(E$n)
    36     37     38     39     40     41     42     43     44     45     46
456354 206435 200482 177376 133722  86738  49517  25284  12916   6934   3842
    47     48     49     50     51     52     53     54     55     56     57
  2255   1379    777    480    288    187    133     90     68     32     33
    58     59     60     61     62     63     64     65     67     70     71
    19     19     10     14     12      2      6      5      5      1      1
    72     74
     1      1
> cat Result
s UNKNOWN
c sat_status                            2
c initial_maximal_clause_length         13
c initial_number_of_variables           260
c initial_number_of_clauses             27605
c initial_number_of_literal_occurrences 158420
c number_of_2-clauses_after_reduction   87
c running_time(sec)                     96449.7
c number_of_nodes                       2730839
c number_of_single_nodes                0
c number_of_quasi_single_nodes          0
c number_of_2-reductions                70732
c number_of_pure_literals               0
c number_of_autarkies                   0
c number_of_missed_single_nodes         0
c max_tree_depth                        32
c file_name                             VanDerWaerden_pd_2-4-13_520.cnf
c splitting_directory                   SplitViaOKsolver_D36VanDerWaerden_pd_2413_520cnf_2011-08-16-014738/Instances
c splitting_cases                       1365418

# on cs-oksvr:
> nohup ProcessSplitViaOKsolver SplitViaOKsolver_D36VanDerWaerden_pd_2413_520cnf_2011-08-16-014738 &
# intermediate result:
> E=read_processsplit_minisat()
16474: 16.479h, sum-cfs=1.005552e+09, mean-t=3.601s, mean-cfs=61039
$t:
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
  0.056   0.508   1.204   3.601   3.035 610.200
sd= 11.42147
      95%       96%       97%       98%       99%      100%
 12.66100  15.30628  19.05492  25.35824  40.18330 610.16200
sum= 59325.6
$cfs:
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
     33    9899   23690   61040   57470 7620000
sd= 157447.3
      95%       96%       97%       98%       99%      100%
 217752.9  256392.7  312282.4  402164.9  608723.5 7619780.0
sum= 1005552013
$t ~ $cfs:
               Estimate  Std. Error  t value  Pr(>|t|)
(Intercept) -7.9971e-01  1.0517e-02  -76.039 < 2.2e-16 ***
E$cfs        7.2100e-05  6.2283e-08 1157.614 < 2.2e-16 ***
R-squared: 0.9879
   \endverbatim
   One should try D=40. XXX </li>
   <li> We need to find the best algorithm from the ubcsat-1-2-0 suite.
   \verbatim
> E=run_ubcsat("VanDerWaerden_pd_2-4-13_537.cnf",runs=100,cutoff=1000000)
1. ddfw:
 0  1 11 12 13 14
35 25  3 11 21  5
fps: 41288
2. ag2wsat:
 0  1 11 12 13 14 15 16
24 11  4  4 20 22 10  5
fps: 162346
3. dano:
 0  1  9 10 11 12 13 14 15 16 17
13  8  1  2  6  6 10 31 21  1  1
fps: 115064
4. anovpp:
 0  1 10 11 12 13 14 15 16
 8  7  1  9  7 25 25 15  3
fps: 115129
5. anovp:
 0  1 10 11 12 13 14 15 16
 8  5  2  3  6 26 26 16  8
fps: 115063
6. g2wsat:
 0  1 14 15 16 17 18 19 20
 8  5  2  1  6 13 23 30 12
fps: 167276
7. paws:
 0  1  2  3  4  6  9 14 15 16 17 18 19
 6 11  2  1  1  4  1  3  7 18 27 18  1
fps: 154535
8. rots:
 0  1 10 11 12 13 14 15 16
 5  3  1  2  6 20 30 30  3
fps: 201676
9. rsaps:
 0  1  2  3  4  5  6  7  8 10 14 15 16 17 18 19
 3 17  7  5  6  3  1  1  1  1  1 12 12 15 14  1
fps: 155015
10. sapsnr:
 0  1  2  3  4  5  6  7  8 13 14 15 16 17 18 19
 2 14  7  3  4  2  2  1  1  2  2  8 13 22 16  1
fps: 140922
11. rnovp:
 0  1  2  9 12 13 14 15 16 17 18 19
 2  9  3  1  2 11 11 21 27  7  5  1
fps: 115518
12. wsatt:
 0 24 28 29 30 31 32 33 34 35 36 37
 2  1  1  2  3  6  6 17 22 22 15  3
fps: 175738
13. samd:
 0  1 10 11 12 13 14 15
 1  3  3 13 23 27 24  6
   \endverbatim
   </li>
   <li> So ddfw is best, followed by adaptg2wsat (which was determined as best
   for vdw_2^pd(5,8) in VanderWaerdenProblems/plans/5-k/general.hpp). </li>
   <li> "RunPdVdWk1k2 4 13 adaptg2wsat 100 1000000" yields
   \verbatim
Break point 1: 519
Break point 2: 538
   \endverbatim
   </li>
   <li> "RunPdVdWk1k2 4 13 adaptg2wsat 100 10000000" yields
   \verbatim
Break point 1: 519
Break point 2: 538
   \endverbatim
   All solutions found in the first round using the previous solution (using
   at most 2609468 steps).
   </li>
   <li> "RunPdVdWk1k2 4 13 ddfw 100 20000000" yields also the breakpoints
   (519,538), where for n=520,539 we have 100 x min=1, and where all
   solutions were found in the first run with the old solution. </li>
  </ul>

*/
