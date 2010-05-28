// Oliver Kullmann, 25.2.2009 (Swansea)
/* Copyright 2009 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*!
  \file Experimentation/Investigations/RamseyTheory/GreenTaoProblems/plans/GreenTao_2-3-7.hpp
  \brief Investigations on greentao_2(3,7)

  Problems are generated by output_greentao2nd_stdname(3,7,n) at Maxima level,
  or by "GTSat 3 7 n" at C++ level.


  \todo Finding the best algorithm from ubcsat
  <ul>
   <li> Is adaptnovelty+ still best? </li>
  </ul>


  \todo Survey propagation
  <ul>
   <li> For n=5000, 7500, 10000, 12500 survey propagation apparently thinks the
   problem is unsatisfiable, i.e., we always get
   \verbatim
> survey_propagation -l GreenTao_2-3-7_XX000.cnf
<bias>:nan
fixed 1 biased var (+0 ucp)
.:-)
<bias>:nan
contradiction
   \endverbatim
   However these problems are all satisfiable. </li>
   <li> the same behaviour for n=15000 (but here we don't know whether
   it's satisfiable). </li>
  </ul>


  \todo greentao_2(3,7) > 13800
  <ul>
   <li> n=5000: ubcsat::adaptnovelty+ finds it satisfiable with just a
   cutoff=10000. </li>
   </li>
   <li> n=7500: ubcsat::adaptnovelty+ finds it satisfiable with just a
   cutoff=100000. </li>
   </li>
   <li> n=10000: ubcsat::adaptnovelty+ finds it satisfiable with just a
   cutoff=1000000 (seed=3610283050). </li>
   <li> n=12500
    <ol>
     <li>
     \verbatim
> ubcsat-okl -alg adaptnovelty+ -runs 10 -cutoff 1000000 -i GreenTao_2-3-7_12500.cnf
Clauses = 4722593
Variables = 12500
TotalLiterals = 14173471
FlipsPerSecond = 6550
BestStep_Mean = 779318.300000
Steps_Mean = 1000000.000000
Steps_Max = 1000000.000000
PercentSuccess = 0.00
BestSolution_Mean = 14.200000
BestSolution_Median = 14.000000
BestSolution_Min = 8.000000
BestSolution_Max = 19.000000
     \endverbatim
     </li>
     <li> Found satisfiable (first run) with cutoff=10*10^6 (seed=348690330).
     </li>
    </ol>
   </li>
   <li> n=13750
    <ol>
     <li> A cutoff=100*10^6 yields in 10 runs a minimum of 2 and a maximum
     of 4. </li>
     <li> cutoff=10^9:
     \verbatim
> ubcsat-okl -alg adaptnovelty+ -runs 10 -cutoff 1000000000 -i GreenTao_2-3-7_13750.cnf
       sat  min     osteps     msteps       seed
      1 0     2  162139042 1000000000 1446773825
      2 0     1  481815784 1000000000 3328367368
      3 1     0  569156743  569156743 1397463094
     \endverbatim
     </li>
    </ol>
   </li>
   <li> n=13800
    <ol>
     <li> cutoff=10^8: In 36 runs one solution was found:
     \verbatim
> E = read_ubcsat("GreenTao_2-3-7_13800.cnf_OUT2")
 0  1  2  3  4
 1  3 10 17  5
36
> E[E$min==0,]
   sat min   osteps   msteps      seed
21   1   0 77868065 77868065 793605254
     \endverbatim
     </li>
     <li> cutoff=10^9: three runs yield twice min=1 and once min=2. </li>
     <li> cutoff=2*10^9: one run yields min=2. </li>
     <li> So perhaps a cutoff=10^8 is more appropriate here. </li>
    </ol>
   </li>
   <li> n=13825, cutoff=10^8, adaptnovelty+ </li>
   <li> n=13850
    <ol>
     <li> cutoff=10^7 (adaptnovelty+):
     \verbatim
 5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 27
 1  2  4 27 27 36 48 46 38 41 37 16 14 15 10  4  1  2  1
370
     \endverbatim
     </li>
     <li> cutoff=2*10^7 (adaptnovelty+):
     \verbatim
 3  4  5  6  7  8  9 10 12 13
 5 15 24 17 15 22 16 11  3  1
129
     \endverbatim
     </li>
     <li> cutoff=4*10^7 (adaptnovelty+):
     \verbatim
 1  2  3  4  5  6  7  8  9
 1 10 38 64 54 22 11  1  1
202
     \endverbatim
     </li>
     <li> cutoff=10^8 (adaptnovelty+):
     \verbatim
 1  2  3  4  5
 1  2 10  5  3
21
2 3 4
5 8 7
20
 2  3  4  5  6
15 49 31  2  3
100
     \endverbatim
     </li>
     <li> cutoff=2*10^8 (adaptnovelty+):
     \verbatim
 1  2  3
 1  3 10
14
     \endverbatim
     </li>
     <li> So lets consider this for now as unsatisfiable. </li>
    </ol>
   </li>
   <li> n=14000
    <ol>
     <li> cutoff=10^9: 4 runs yield min=3. </li>
     <li> cutoff=2*10^9: 1 run yields min=3. </li>
     <li> So let's consider this for now as unsatisfiable. </li>
    </ol>
   </li>
   <li> n=14375
    <ol>
     <li> cutoff=10^9: three times min=6. So let's consider this unsatisfiable
     for now. </li>
    </ol>
   </li>
   <li> n=15000
    <ol>
     <li>
     \verbatim
> ubcsat-okl -alg adaptnovelty+ -runs 10 -cutoff 10000000 -i GreenTao_2-3-7_15000.cnf
       sat  min     osteps     msteps       seed
      1 0    36    9340701   10000000 1782442807
      2 0    39    5704494   10000000  470832140
      3 0    37    8891559   10000000 3612813694
      4 0    33    9055929   10000000 1719584600
      5 0    41    9854280   10000000  433897223
     \endverbatim
     </li>
     <li> cutoff multiplied with 10:
     \verbatim
> ubcsat-okl -alg adaptnovelty+ -runs 10 -cutoff 100000000 -i GreenTao_2-3-7_15000.cnf
       sat  min     osteps     msteps       seed
      1 0    17   43895806  100000000 1613156495
      2 0    23   98468694  100000000 3089531980
      3 0    25   57245388  100000000  293693136
     \endverbatim
     If this should be satisfiable then it's tough. </li>
     <li> cutoff=10^9 yields
     \verbatim
> ubcsat-okl -alg adaptnovelty+ -runs 10 -cutoff 1000000000 -i GreenTao_2-3-7_15000.cnf
       sat  min     osteps     msteps       seed
      1 0    19  760636579 1000000000 1303145336
      2 0    19  960310544 1000000000 1126725316
     \endverbatim
     One would need cutoff=4*10^9, but perhaps it's unsatisfiable. </li>
     <li>
     \verbatim
> ubcsat-okl -alg adaptnovelty+ -runs 10 -cutoff 4000000000 -i GreenTao_2-3-7_15000.cnf
       sat  min     osteps     msteps       seed
      1 0    15 3386525961 4000000000 1699695039
     \endverbatim
     So if it's still satisfiable, then it's a rather hard problem. </li>
    </ol>
   </li>
  </ul>

*/

