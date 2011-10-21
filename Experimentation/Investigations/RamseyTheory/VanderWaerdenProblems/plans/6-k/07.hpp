// Oliver Kullmann, 12.3.2011 (Swansea)
/* Copyright 2011 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*!
  \file Experimentation/Investigations/RamseyTheory/VanderWaerdenProblems/plans/6-k/07.hpp
  \brief On investigations into vdw_2(6,7)


  \todo Best complete solver for palindromic problems


  \todo Best local-search solver for palindromic problems
  <ul>
   <li> Best ubcsat-algorithm:
   \verbatim
> PdVanderWaerdenCNF-O3-DNDEBUG 6 7 1155

> E=run_ubcsat("VanDerWaerden_pd_2-6-7_1155.cnf",runs=100,cutoff=1000000)

saps
  2 498 499 504 505 506 507 508 509 510 511 512 513 514 515 516 517 518 519 520
  1   1   1   2   2   1   3   3   6   6   4   3  11   4   4   7  14   9   7   2
521 522
  5   4
gwsat
432 438 440 442 443 444 445 446 447 448 449 450 451 452 453 454 455 456 457 458
  1   1   2   1   1   1   3   1   9   3   4   8   1   2   2   4   4   3   6   6
459 460 461 462 463 464 465 466 467 468 469 470 471 473 474 476 479 481
  3   3   5   2   2   1   1   3   4   1   1   3   2   1   1   1   1   2
rots
444 445 447 449 451 452 453 454 455 456 457 458 459 460 461 462 463 464 466 467
  1   2   1   2   4   1   4   1   2   2   2   4   7   4   5   6   6   4   3   6
468 469 470 471 472 473 474 475 478 480 481 482 483 489 490 496
  5   2   4   2   2   3   2   4   2   1   1   1   1   1   1   1
ag2wsat
444 447 448 449 450 451 452 453 455 456 457 458 459 460 461 462 463 464 465 466
  1   1   1   1   2   1   3   3   2   3   6   5   3   5   4   6  12   9   8   5
467 468 469 470 471 476
  5   5   4   3   1   1
samd
448 450 454 462 463 464 465 466 467 468 469 472 473 474 475 477 478 479 480 481
  1   1   1   2   1   1   3   4   1   1   2   2   3   5   4   4   4   4   1   2
482 483 484 485 487 488 489 490 491 492 493 494 495 496 497 498 499 502 506 507
  5   5   2   2   1   4   2   1   5   4   3   3   5   1   1   2   1   1   1   2
512 525
  1   1
gsatt
450 451 454 458 459 460 461 462 464 466 467 469 470 471 472 473 474 475 476 477
  1   1   1   1   1   2   1   1   4   2   1   4   3   1   2   3   3   3   4   5
478 479 480 481 482 483 484 485 486 487 489 490 491 492 493 496 497 498 499 501
  4   3   4   3   7   6   3   2   3   3   3   1   2   2   2   2   1   1   1   2
505
  1
irots
452 456 459 460 461 462 463 464 465 466 467 468 469 470 471 472 473 474 475 476
  1   1   3   2   1   2   1   1   4   4   4   4   7   7   4   8   9   7   6   4
477 478 479 480 483 484
  5   6   2   5   1   1
ddfw
455 460 461 463 464 465 466 467 468 469 470 471 472 473 474 475 476 477 478 479
  1   1   1   2   1   2   1   5   3   4   5   1   8   4  10  14   8   6   7  10
480 481
  3   3

> E=run_ubcsat("VanDerWaerden_pd_2-6-7_1155.cnf",runs=100,cutoff=10000000)
ag2wsat
  0  88 363 368 369 372 375 439 442 443 444 445 446 447 448 449 450 451 452 453
  1   1   1   1   2   1   2   1   1   3   2   2   7   5   3   2  10   5   6   6
454 455 456 457 458 459 461 462
  5   6   9   4   8   1   3   2
rsaps
  0  97 487 489 496 497 498 499 500 502 503 504 505 506 507 508 509 510 511 512
  1   1   1   1   3   3   3   1   5  10   7   5   5   8   7  11   9   7   7   1
513 514
  3   1
sapsnr
  0 100 104 496 497 498 499 500 501 502 503 504 505 506 507 508 509 510 511 512
  1   1   1   3   1   3   2   2   4   4   5   6   5   5  11   6  13  14   2   3
513 514
  7   1
irots
372 374 376 381 437 450 451 453 454 455 456 457 458 459 460 461 462 463 464 465
  1   1   1   1   1   2   1   1   2   3   3   4   4   4   9   3   6   9   4  11
466 467 468 469 470 471
 10   2   4   9   3   1
g2wsat
376 465 469 470 471 472 473 474 475 476 477 478 479 480 481 482 483 484 485 486
  1   1   1   1   1   3   3   1   4   6   4  11   9  12   2   8  12   8   7   1
487 488
  2   2
   \endverbatim
   Perhaps ag2wsat scales better than rsaps and sapsnr? </li>
  </ul>


  \todo vdw_2^pd(6,7) >= (591,1156)
  <ul>
   <li> The conjecture is that vdw_2^pd(6,7)_1 = 591. </li>
   <li> minisat-2.2.0:
    <ol>
     <li>
     \verbatim
CRunPdVdWk1k2 6 7 minisat-2.2.0

> E=read_crunpdvdw_minisat()
$t:
     Min.   1st Qu.    Median      Mean   3rd Qu.      Max.
0.000e+00 1.999e-03 9.998e-03 9.900e+01 2.600e-02 8.558e+03
sd= 655.9847
       95%        96%        97%        98%        99%       100%
  33.20532  125.83874  596.18256  978.36942 3393.56790 8558.04000
sum= 35836.84
$cfs:
     Min.   1st Qu.    Median      Mean   3rd Qu.      Max.
0.000e+00 0.000e+00 2.000e+00 2.039e+06 2.875e+01 1.586e+08
sd= 12829676
      95%       96%       97%       98%       99%      100%
  1056456   3543016  14246239  22715025  70358654 158598561
sum= 738155209
$t ~ $cfs:
               Estimate  Std. Error  t value Pr(>|t|)
(Intercept) -5.0736e+00  2.1064e+00  -2.4087  0.01651 *
E$cfs        5.1037e-05  1.6237e-07 314.3353  < 2e-16 ***
R-squared: 0.9964

> max(E$v)
[1] 368
> E[E$v==368,]
      v  rn    rc       t sat       cfs       dec    rts         r1 mem ptime
362 368 184 12034 8558.04   1 158598561 185340735 196604 3492420991  79  0.01
    stime        cfl
362  0.04 5613364596
     \endverbatim
     </li>
     <li> One sees a steep increase at the end of running time. And over the
     range considered the problems with odd v appear to be harder. </li>
    </ol>
   </li>
   <li> Certificates:
    <ol>
     <li> n=590:
     \verbatim
6,9,11,13,14,21,22,24,26,27,
28,29,31,35,38,39,43,44,45,49,
54,57,62,63,64,65,66,69,70,73,
74,75,76,77,82,85,90,94,95,96,
98,100,101,104,108,110,111,112,113,115,
117,118,119,122,124,125,127,128,130,133,
134,135,137,139,140,141,142,144,148,151,
152,154,156,157,158,162,167,170,175,176,
177,178,179,182,183,186,187,188,189,190,
195,198,203,207,208,209,211,213,214,217,
221,223,224,225,226,228,230,231,232,235,
237,238,240,241,243,246,247,248,250,252,
253,254,255,257,261,264,265,267,269,270,
271,275,280,283,288,289,290,291,292,295
     \endverbatim
     or (small variation)
     \verbatim
6,11,13,14,21,22,24,26,28,29,
31,35,38,39,43,44,45,49,54,57,
62,63,64,65,66,69,70,73,74,75,
76,77,82,85,90,94,95,96,98,100,
101,104,108,110,111,112,113,115,117,118,
119,122,124,125,126,127,128,130,133,134,
135,137,139,140,141,142,144,148,151,152,
154,156,157,158,162,167,170,175,176,177,
178,179,182,183,186,187,188,189,190,195,
198,203,207,208,209,211,213,214,217,221,
223,224,225,226,228,230,231,232,235,237,
238,240,241,243,246,247,248,250,252,253,
254,255,257,261,264,265,267,269,270,271,
275,280,283,288,289,290,291,292,295
     \endverbatim
     </li>
     <li> n=1155:
     \verbatim
7,12,13,20,22,23,24,25,26,28,
33,37,38,44,47,51,53,54,55,59,
61,62,64,66,67,70,71,74,76,79,
81,82,83,87,89,90,91,93,94,96,
97,98,99,100,103,104,105,107,108,109,
110,112,118,121,122,123,124,126,128,129,
130,131,134,140,142,143,144,145,147,148,
149,152,153,154,155,156,158,159,161,162,
163,165,169,170,171,173,176,178,181,182,
185,186,188,190,191,193,197,198,199,201,
205,208,214,215,219,224,226,227,228,229,
230,232,233,238,239,240,245,246,248,249,
250,251,252,254,259,263,264,270,273,277,
279,280,281,285,287,288,290,292,293,296,
297,300,302,305,307,308,309,313,315,316,
317,319,320,322,323,324,325,326,329,330,
331,333,334,335,336,338,344,347,348,349,
350,352,354,355,356,357,360,366,368,369,
370,371,373,374,375,378,379,380,381,382,
384,385,387,388,389,391,395,396,397,399,
402,404,407,408,411,412,414,416,417,419,
423,424,425,427,431,434,440,441,445,450,
452,453,454,455,456,458,459,464,466,471,
472,474,475,476,477,478,480,485,489,490,
496,499,503,505,506,507,511,513,514,516,
518,519,522,523,526,528,531,533,534,535,
539,541,542,543,545,546,548,549,550,551,
552,555,556,557,559,560,561,562,564,570,
573,574,575,576,578
     \endverbatim
    </ol>
   </li>
   <li> "RunPdVdWk1k2 6 7 adaptg2wsat 100 4000000" yields
   \verbatim
Break point 1: 393
Break point 2: 594
   \endverbatim
   </li>
   <li> "RunPdVdWk1k2 6 7 adaptg2wsat 100 20000000" yields
   \verbatim
Break point 1: 587
Break point 2: 1156
   \endverbatim
   </li>
   <li> "RunPdVdWk1k2 6 7 adaptg2wsat 100 40000000" yields
   \verbatim
Break point 1: 591
Break point 2: 1156
   \endverbatim
   where all solutions were found within the first 5 rounds. </li>
   <li> "RunPdVdWk1k2 6 7 rsaps 200 100000000" yields: the solution found for
   n=1155 is identical to the above solution(!):
   \verbatim
> cat Statistics
    374     2     715659 2984853093  p
    379     2   69113576  170472211  p
    384     3   18236901  389234388  p
    386     7   18739768 1130352551  r
    595     2   11828240 3858090860  p
> E=read_ubcsat("VanDerWaerden_pd_2-6-7_592.cnf_OUT",nrows=200)
 1 13 25 26 27 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 47 50 52
37  1  1  2  1  4  7  7 11  9 15 11 14  7 14  9  9 13  6 10  4  4  2  1  1
200
> E=read_ubcsat("VanDerWaerden_pd_2-6-7_1157.cnf_OUT")

  1   2  97 106 476 486 488 489 490 491 493 494 495 496 497 498 499 500 501 502
 23   5   2   1   2   1   2   2   1   4   2   6   5   7  16  12   9  17  11   8
503 504 505
 15   7   5
163
   \endverbatim
   confirming the values already found. </li>
   <li> Using SplittingViaOKsolver:
    <ol>
     <li> Finding good D-values for n=592:
     \verbatim
# Statistics:
n=284 c=31224 l=186632
 length   count
      3     112
      4     188
      6   30924

> SplittingViaOKsolver -D20 VanDerWaerden_pd_2-6-7_592.cnf
> cat Md5sum
436243f5b098b0323d70b01934b9b6fa
> cat Statistics
> E=read.table("Data")
> summary(E$n)
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
  20.00   20.00   20.00   20.61   21.00   30.00
> table(E$n)
   20    21    22    23    24    25    26    27    28    29    30
51905 19998  8762  2949   857   258    86    28    20     7     4
> summary(E$d)
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
  10.00   16.00   17.00   17.16   18.00   20.00
> table(E$d)
   10    11    12    13    14    15    16    17    18    19    20
    1    16   126   686  2464  6929 15086 22869 22409 11620  2668
> cat Result
s UNKNOWN
c sat_status                            2
c initial_maximal_clause_length         7
c initial_number_of_variables           296
c initial_number_of_clauses             31512
c initial_number_of_literal_occurrences 203065
c running_time(sec)                     5899.5
c number_of_nodes                       169747
c number_of_2-reductions                349
c number_of_pure_literals               0
c number_of_autarkies                   0
c max_tree_depth                        20
c file_name                             VanDerWaerden_pd_2-6-7_592.cnf
c splitting_directory                   SplitViaOKsolver_D20VanDerWaerden_pd_267_592cnf_2011-09-25-215748/Instances
c splitting_cases                       84874

> E=read_processsplit_minisat()
72: 8.828h, sum-cfs=3.680676e+08, mean-t=441.407s, mean-cfs=5112050
$t:
     Min.   1st Qu.    Median      Mean   3rd Qu.      Max.
    0.318    10.230    71.540   441.400   320.300 11440.000
sd= 1420.393
      95%       96%       97%       98%       99%      100%
 1729.138  1973.672  2048.499  2590.174  5427.553 11438.200
sum= 31781.31
$cfs:
     Min.   1st Qu.    Median      Mean   3rd Qu.      Max.
     6960    185800   1217000   5112000   4292000 107200000
sd= 13678960
      95%       96%       97%       98%       99%      100%
 19932510  21892704  22955287  29222378  54946867 107161581
sum= 368067618
$t ~ $cfs:
               Estimate  Std. Error t value  Pr(>|t|)
(Intercept) -8.7440e+01  1.5530e+01 -5.6304 3.472e-07 ***
E$cfs        1.0345e-04  1.0700e-06 96.6808 < 2.2e-16 ***
R-squared: 0.9926

# So it's a very hard problem. D must be set higher, say D=30.
# On cs-oksvr:
> nohup SplittingViaOKsolver -D30 VanDerWaerden_pd_2-6-7_592.cnf &
XXX
     \endverbatim
     </li>
    </ol>
   </li>
  </ul>

*/
