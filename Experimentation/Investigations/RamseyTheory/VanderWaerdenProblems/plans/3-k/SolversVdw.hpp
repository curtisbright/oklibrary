// Oliver Kullmann, 3.6.2013 (Swansea)
/* Copyright 2013 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*!
  \file Experimentation/Investigations/RamseyTheory/VanderWaerdenProblems/plans/3-k/SolversVdw.hpp
  \brief On SAT solvers computing vdW-numbers vdw_2(3,k) in general


  \todo Performance of OKsolver-2002
  <ul>
   <li> k=4, n=17,18: 7 resp. 9 nodes. </li>
   <li> k=5, n=21,22: 6 resp. 19 nodes. </li>
   <li> k=6, n=31,32: 24 resp. 51 nodes. </li>
   <li> k=7, n=45,46: 11 resp. 139 nodes. </li>
   <li> k=8, n=57,58: 109 resp. 479 nodes. </li>
   <li> k=9, n=76,77: 702 resp. 1591 nodes. </li>
   <li> k=10, n=96,97: 129 resp. 10545 nodes. </li>
   <li> k=11, n=113,114:
    <ol>
     <li> 37374 resp. 41563 nodes. </li>
     <li> With minisat2-preprocessor: identical results (so apparently
     the preprocessor does nothing). </li>
    </ol>
   </li>
   <li> k=12, n=134, 135 (cswsok, 3GHz):
   \verbatim
> OKsolver_2002-O3-DNDEBUG VanDerWaerden_2-3-12_134.cnf
s SATISFIABLE
c sat_status                            1
c initial_maximal_clause_length         12
c initial_number_of_variables           134
c initial_number_of_clauses             5172
c initial_number_of_literal_occurrences 22266
c number_of_initial_unit-eliminations   0
c reddiff_maximal_clause_length         0
c reddiff_number_of_variables           0
c reddiff_number_of_clauses             0
c reddiff_number_of_literal_occurrences 0
c number_of_2-clauses_after_reduction   0
c running_time(sec)                     222.1
c number_of_nodes                       283568
c number_of_single_nodes                0
c number_of_quasi_single_nodes          0
c number_of_2-reductions                2060055
c number_of_pure_literals               35
c number_of_autarkies                   1
c number_of_missed_single_nodes         0
c max_tree_depth                        36
c proportion_searched                   9.996262e-01
c proportion_single                     0.000000e+00
c total_proportion                      0.9996261596679688
c number_of_table_enlargements          0
c number_of_1-autarkies                 516
c number_of_new_2-clauses               0
c maximal_number_of_added_2-clauses     0
c file_name                             VanDerWaerden_2-3-12_134.cnf

# without tree-pruning: precisely the same.

> OKsolver_2002-O3-DNDEBUG VanDerWaerden_2-3-12_135.cnf
s UNSATISFIABLE
c sat_status                            0
c initial_maximal_clause_length         12
c initial_number_of_variables           135
c initial_number_of_clauses             5251
c initial_number_of_literal_occurrences 22611
c number_of_initial_unit-eliminations   0
c reddiff_maximal_clause_length         0
c reddiff_number_of_variables           0
c reddiff_number_of_clauses             0
c reddiff_number_of_literal_occurrences 0
c number_of_2-clauses_after_reduction   0
c running_time(sec)                     220.8
c number_of_nodes                       281381
c number_of_single_nodes                0
c number_of_quasi_single_nodes          0
c number_of_2-reductions                2049274
c number_of_pure_literals               29
c number_of_autarkies                   0
c number_of_missed_single_nodes         0
c max_tree_depth                        36
c proportion_searched                   1.000000e+00
c proportion_single                     0.000000e+00
c total_proportion                      1
c number_of_table_enlargements          0
c number_of_1-autarkies                 490
c number_of_new_2-clauses               0
c maximal_number_of_added_2-clauses     0
c file_name                             VanDerWaerden_2-3-12_135.cnf
   \endverbatim
   </li>
   <li> k=13, n=159, 160 (cswsok, 3GHz):
   \verbatim
> OKsolver_2002-O3-DNDEBUG VanDerWaerden_2-3-13_159.cnf
s SATISFIABLE
c sat_status                            1
c initial_maximal_clause_length         13
c initial_number_of_variables           159
c initial_number_of_clauses             7216
c initial_number_of_literal_occurrences 31398
c number_of_initial_unit-eliminations   0
c reddiff_maximal_clause_length         0
c reddiff_number_of_variables           0
c reddiff_number_of_clauses             0
c reddiff_number_of_literal_occurrences 0
c number_of_2-clauses_after_reduction   0
c running_time(sec)                     2118.0
c number_of_nodes                       1590139
c number_of_single_nodes                0
c number_of_quasi_single_nodes          0
c number_of_2-reductions                12785699
c number_of_pure_literals               455
c number_of_autarkies                   0
c number_of_missed_single_nodes         0
c max_tree_depth                        43
c proportion_searched                   4.645918e-02
c proportion_single                     0.000000e+00
c total_proportion                      0.04645918309688568
c number_of_table_enlargements          0
c number_of_1-autarkies                 12836
c number_of_new_2-clauses               0
c maximal_number_of_added_2-clauses     0
c file_name                             VanDerWaerden_2-3-13_159.cnf

> OKsolver_2002-O3-DNDEBUG VanDerWaerden_2-3-13_160.cnf
s UNSATISFIABLE
c sat_status                            0
c initial_maximal_clause_length         13
c initial_number_of_variables           160
c initial_number_of_clauses             7308
c initial_number_of_literal_occurrences 31804
c number_of_initial_unit-eliminations   0
c reddiff_maximal_clause_length         0
c reddiff_number_of_variables           0
c reddiff_number_of_clauses             0
c reddiff_number_of_literal_occurrences 0
c number_of_2-clauses_after_reduction   0
c running_time(sec)                     3806.0
c number_of_nodes                       2970723
c number_of_single_nodes                0
c number_of_quasi_single_nodes          0
c number_of_2-reductions                24138237
c number_of_pure_literals               706
c number_of_autarkies                   0
c number_of_missed_single_nodes         0
c max_tree_depth                        43
c proportion_searched                   1.000000e+00
c proportion_single                     0.000000e+00
c total_proportion                      1
c number_of_table_enlargements          0
c number_of_1-autarkies                 24075
c number_of_new_2-clauses               0
c maximal_number_of_added_2-clauses     0
c file_name                             VanDerWaerden_2-3-13_160.cnf

> OKsolver_2002-O3-DNDEBUG VanDerWaerden_2-3-14_186.cnf
s UNSATISFIABLE
c sat_status                            0
c initial_maximal_clause_length         14
c initial_number_of_variables           186
c initial_number_of_clauses             9795
c initial_number_of_literal_occurrences 43014
c number_of_initial_unit-eliminations   0
c reddiff_maximal_clause_length         0
c reddiff_number_of_variables           0
c reddiff_number_of_clauses             0
c reddiff_number_of_literal_occurrences 0
c number_of_2-clauses_after_reduction   0
c running_time(sec)                     47597.9
c number_of_nodes                       22470241
c number_of_single_nodes                0
c number_of_quasi_single_nodes          0
c number_of_2-reductions                200801343
c number_of_pure_literals               2161
c number_of_autarkies                   0
c number_of_missed_single_nodes         9
c max_tree_depth                        52
c proportion_searched                   1.000000e+00
c proportion_single                     0.000000e+00
c total_proportion                      1
c number_of_table_enlargements          0
c number_of_1-autarkies                 60234
c number_of_new_2-clauses               0
c maximal_number_of_added_2-clauses     0
c file_name                             VanDerWaerden_2-3-14_186.cnf
   \endverbatim
   </li>
  </ul>


  \todo Performance of march_pl
  <ul>
   <li> k=12:
    <ol>
     <li> k=12, n=134: 28s (5567 nodes, cswsok (3GHz)) </li>
     <li> k=12, n=135: 213s (47963 nodes, cswsok (3GHz)) </li>
    </ol>
   </li>
   <li> k=13, n=160 (cswsok, 3GHz):
   \verbatim
c main():: nodeCount: 279061
c main():: dead ends in main: 552
c main():: lookAheadCount: 71404700
c main():: unitResolveCount: 1831368
c main():: time=1849.209961
c main():: necessary_assignments: 37675
c main():: bin_sat: 0, bin_unsat 0
c main():: doublelook: #: 2653564, succes #: 2089797
c main():: doublelook: overall 3.822 of all possible doublelooks executed
c main():: doublelook: succesrate: 78.754, average DL_trigger: 312.059
   \endverbatim
   </li>
   <li> k=14, n=186 (cswsok, 3GHz):
   \verbatim
c main():: nodeCount: 1975338
c main():: dead ends in main: 3875
c main():: lookAheadCount: 578164648
c main():: unitResolveCount: 14151563
c main():: time=17018.310547
c main():: necessary_assignments: 284061
c main():: bin_sat: 0, bin_unsat 0
c main():: doublelook: #: 20709282, succes #: 16337811
c main():: doublelook: overall 3.680 of all possible doublelooks executed
c main():: doublelook: succesrate: 78.891, average DL_trigger: 348.623
   \endverbatim
   </li>
   <li> k=15, n=218 (cswsok, 3GHz):
   \verbatim
c main():: nodeCount: 11959263
c main():: dead ends in main: 22968
c main():: lookAheadCount: -212374051
c main():: unitResolveCount: 92354095
c main():: time=175614.406250
c main():: necessary_assignments: 1870576
c main():: bin_sat: 0, bin_unsat 0
c main():: doublelook: #: 139136152, succes #: 111145034
c main():: doublelook: overall 3.496 of all possible doublelooks executed
c main():: doublelook: succesrate: 79.882, average DL_trigger: 125.399
s UNSATISFIABLE
   \endverbatim
   </li>
  </ul>


  \todo Performance of satz215
  <ul>
   <li> k=12:
    <ol>
     <li> k=12, n=134: 73s (NB_BRANCHE= 240158, cswsok (3 GHz)) </li>
     <li> k=12, n=135: 79s (NB_BRANCHE= 262304, cswsok (3 GHz)) </li>
    </ol>
   </li>
   <li> k=13, n=160 (cswsok, 3GHz):
   \verbatim
> satz215 VanDerWaerden_2-3-13_160.cnf
**** The instance is unsatisfiable. *****
NB_MONO= 363, NB_UNIT= 39344343, NB_BRANCHE= 1698185, NB_BACK= 863252
Program terminated in 711.470 seconds.
satz215 VanDerWaerden_2-3-13_160.cnf 711.470 1698185 863252 219047856 8567070 0 160 7308 0 4263617 1867228
   \endverbatim
   Compared to OKsolver (see above) we have only 1,698,185 nodes (versus
   2,970,723), and we have 8,567,070 r_2_reductions (versus 24,138,237), and
   1,867,228 r_3-reductions. The OKsolver needs 5-times more time. We need to
   understand this difference! Is the satz-heuristic here better?! Or are the
   r_3-reductions here of key importance?
   </li>
   <li> k=14, n=186 (cswsok, 3GHz):
   \verbatim
> satz215 VanDerWaerden_2-3-14_186.cnf
NB_MONO= 1435, NB_UNIT= 277867725, NB_BRANCHE= 10822316, NB_BACK= 5500793
Program terminated in 6233.370 seconds.
satz215 VanDerWaerden_2-3-14_186.cnf 6233.370 10822316 5500793 1573930613 58462301 0 186 9795 0 29371201 12582731
   \endverbatim
   </li>
   <li> k=15, n=218 (cswsok, 3GHz):
   \verbatim
> satz215 VanDerWaerden_2-3-15_218.cnf
NB_MONO= 3848, NB_UNIT= 1902278418, NB_BRANCHE= 66595028, NB_BACK= 33775013
Program terminated in 54913.090 seconds.
satz215 VanDerWaerden_2-3-15_218.cnf 54913.090 66595028 33775013 11030537203 387523942 0 218 13362 0 314567493 92988051
   \endverbatim
   (that's 15.3 hours). A lot of r2-reductions (387523942) and r3-reductions
   (92988051).
   </li>
   <li> k=16, n=238 (cswsok, 3GHz):
   \verbatim
> satz215 VanDerWaerden_2-3-16_238.cnf
NB_MONO= 87981, NB_UNIT= 17845374254, NB_BRANCHE= 599520428, NB_BACK= 304003189
Program terminated in 562161.320 seconds.
satz215 VanDerWaerden_2-3-16_238.cnf 562161.320 599520428 304003189 101411640715 3643082746 0 238 15812 0 3402007271 917339083
   \endverbatim
   (that's 156 hours).
   </li>
  </ul>


  \todo tawSolver
  <ul>
   <li> On cswsok (3 GHz):
   \verbatim
for F in *.cnf; do echo $F; ./ta_solver75 $F | tee -a Output_ta75; done

VanDerWaerden_2-3-12_134.cnf
N_UNITS: 20977611, N_BRANCHES: 1721206, N_BACK: 860593
Running time: 44.09 seconds

VanDerWaerden_2-3-12_135.cnf
N_UNITS: 21978048, N_BRANCHES: 1790733, N_BACK: 895366
Running time: 46.64 seconds

VanDerWaerden_2-3-13_159.cnf
N_UNITS: 821693, N_BRANCHES: 65615, N_BACK: 32796
Running time: 2.01 seconds

VanDerWaerden_2-3-13_160.cnf
N_UNITS: 180658356, N_BRANCHES: 13722975, N_BACK: 6861487
Running time: 462.84 seconds

VanDerWaerden_2-3-14_185.cnf
N_UNITS: 1002639318, N_BRANCHES: 69102886, N_BACK: 34551429
Running time: 3030.05 seconds

VanDerWaerden_2-3-14_186.cnf
N_UNITS: 1498228640, N_BRANCHES: 102268511, N_BACK: 51134255
Running time: 4576.51 seconds

VanDerWaerden_2-3-15_217.cnf
N_UNITS: 4924868287, N_BRANCHES: 309583287, N_BACK: 154791621
Running time: 18378.29 seconds

VanDerWaerden_2-3-15_218.cnf
N_UNITS: 12442138772, N_BRANCHES: 774872707, N_BACK: 387436353
Running time: 47005.83 seconds

VanDerWaerden_2-3-16_238.cnf
N_UNITS: 134192977183, N_BRANCHES: 8120609615, N_BACK: 4060304807
Running time: 532416.20 seconds

# version b45bc81344b378d0130138af761e397f2b3a87c7

> tawSolver VanDerWaerden_2-3-12_134.cnf
c running_time(sec)                     37.13
c number_of_nodes                       1721206
c number_of_binary_nodes                860593
c number_of_1-reductions                20977611
c max_number_changes                    9912
> tawSolver VanDerWaerden_2-3-12_135.cnf
c running_time(sec)                     39.46
c number_of_nodes                       1790733
c number_of_binary_nodes                895366
c number_of_1-reductions                21978048
c max_number_changes                    10029
> tawSolver VanDerWaerden_2-3-13_159.cnf
c running_time(sec)                     1.68
c number_of_nodes                       65615
c number_of_binary_nodes                32796
c number_of_1-reductions                821693
c max_number_changes                    13797
> tawSolver VanDerWaerden_2-3-13_160.cnf
c running_time(sec)                     388.87
c number_of_nodes                       13722975
c number_of_binary_nodes                6861487
c number_of_1-reductions                180658356
c max_number_changes                    14061
> tawSolver VanDerWaerden_2-3-14_185.cnf
c running_time(sec)                     2545.72
c number_of_nodes                       69102886
c number_of_binary_nodes                34551429
c number_of_1-reductions                1002639318
c max_number_changes                    18792
> tawSolver VanDerWaerden_2-3-14_186.cnf
c running_time(sec)                     3859.84
c number_of_nodes                       102268511
c number_of_binary_nodes                51134255
c number_of_1-reductions                1498228640
c max_number_changes                    19139
> tawSolver VanDerWaerden_2-3-15_217.cnf
c running_time(sec)                     15252.00
c number_of_nodes                       309583287
c number_of_binary_nodes                154791621
c number_of_1-reductions                4924868287
c max_number_changes                    25796
> tawSolver VanDerWaerden_2-3-15_218.cnf
c running_time(sec)                     39229.11
c number_of_nodes                       774872707
c number_of_binary_nodes                387436353
c number_of_1-reductions                12442138772
c max_number_changes                    26075
> tawSolver VanDerWaerden_2-3-16_238.cnf
c running_time(sec)                     456846.11
c number_of_nodes                       8120609615
c number_of_binary_nodes                4060304807
c number_of_1-reductions                134192977183
c max_number_changes                    30874


# ID d72626596917101a573c05cfb90cda9ab25f4214
# Now new basis-weight 3.0.

> tawSolver VanDerWaerden_2-3-12_134.cnf
c running_time(sec)                     28.37
c number_of_nodes                       1498256
c number_of_binary_nodes                749117
c number_of_1-reductions                18113899
c max_number_changes                    9959
> tawSolver VanDerWaerden_2-3-12_135.cnf
c running_time(sec)                     33.00
c number_of_nodes                       1713023
c number_of_binary_nodes                856511
c number_of_1-reductions                20916887
c max_number_changes                    10052
> tawSolver VanDerWaerden_2-3-13_159.cnf
c running_time(sec)                     2.44
c number_of_nodes                       107389
c number_of_binary_nodes                53680
c number_of_1-reductions                1336239
c max_number_changes                    13804
> tawSolver VanDerWaerden_2-3-13_160.cnf
c running_time(sec)                     335.17
c number_of_nodes                       13358805
c number_of_binary_nodes                6679402
c number_of_1-reductions                175121629
c max_number_changes                    14060
> tawSolver VanDerWaerden_2-3-14_185.cnf
c running_time(sec)                     2153.95
c number_of_nodes                       66557682
c number_of_binary_nodes                33278826
c number_of_1-reductions                960346804
c max_number_changes                    18970
> tawSolver VanDerWaerden_2-3-14_186.cnf

# ID 6a22ecdde554f19b56cbf7faaaa40429cf3d650d
# Basically final implementation performance-wise.

c number_of_variables                   134
c number_of_clauses                     5172
c maximal_clause_length                 12
c running_time(sec)                     24.86
c number_of_nodes                       1498256
c number_of_binary_nodes                749117
c number_of_1-reductions                18113899
c max_number_changes                    9959
c file_name                             VanDerWaerden_2-3-12_134.cnf

c number_of_variables                   135
c number_of_clauses                     5251
c maximal_clause_length                 12
c running_time(sec)                     28.93
c number_of_nodes                       1713023
c number_of_binary_nodes                856511
c number_of_1-reductions                20916887
c max_number_changes                    10052
c file_name                             VanDerWaerden_2-3-12_135.cnf

c number_of_variables                   159
c number_of_clauses                     7216
c maximal_clause_length                 13
c running_time(sec)                     2.12
c number_of_nodes                       107389
c number_of_binary_nodes                53680
c number_of_1-reductions                1336239
c max_number_changes                    13804
c file_name                             VanDerWaerden_2-3-13_159.cnf

c number_of_variables                   160
c number_of_clauses                     7308
c maximal_clause_length                 13
c running_time(sec)                     290.38
c number_of_nodes                       13358805
c number_of_binary_nodes                6679402
c number_of_1-reductions                175121629
c max_number_changes                    14060
c file_name                             VanDerWaerden_2-3-13_160.cnf

c number_of_variables                   185
c number_of_clauses                     9689
c maximal_clause_length                 14
c running_time(sec)                     1852.41
c number_of_nodes                       66557682
c number_of_binary_nodes                33278826
c number_of_1-reductions                960346804
c max_number_changes                    18970
c file_name                             VanDerWaerden_2-3-14_185.cnf

c number_of_variables                   186
c number_of_clauses                     9795
c maximal_clause_length                 14
c running_time(sec)                     2823.03
c number_of_nodes                       99576555
c number_of_binary_nodes                49788277
c number_of_1-reductions                1450713952
c max_number_changes                    19126
c file_name                             VanDerWaerden_2-3-14_186.cnf

c number_of_variables                   217
c number_of_clauses                     13239
c maximal_clause_length                 15
c running_time(sec)                     11666.76
c number_of_nodes                       313770776
c number_of_binary_nodes                156885365
c number_of_1-reductions                4945351409
c max_number_changes                    25728
c file_name                             VanDerWaerden_2-3-15_217.cnf

c number_of_variables                   218
c number_of_clauses                     13362
c maximal_clause_length                 15
c running_time(sec)                     29014.19
c number_of_nodes                       763714403
c number_of_binary_nodes                381857201
c number_of_1-reductions                12163387462
c max_number_changes                    26148
c file_name                             VanDerWaerden_2-3-15_218.cnf

# ID c730dc830fb5df241fdc81697c389afcce6ba117
# Now with product as projection, and weight-basis 2.

c running_time(sec)                     15.52
c number_of_nodes                       957164
c number_of_binary_nodes                478575
c number_of_1-reductions                10859220
c max_number_assignments                134
c max_number_changes                    9780
c file_name                             VanDerWaerden_2-3-12_134.cnf

c running_time(sec)                     18.45
c number_of_nodes                       1122651
c number_of_binary_nodes                561325
c number_of_1-reductions                12787692
c max_number_assignments                130
c max_number_changes                    9900
c file_name                             VanDerWaerden_2-3-12_135.cnf

c running_time(sec)                     47.02
c number_of_nodes                       2242579
c number_of_binary_nodes                1121280
c number_of_1-reductions                27564612
c max_number_assignments                157
c max_number_changes                    13715
c file_name                             VanDerWaerden_2-3-13_159.cnf

c running_time(sec)                     140.48
c number_of_nodes                       6634485
c number_of_binary_nodes                3317242
c number_of_1-reductions                81937917
c max_number_assignments                157
c max_number_changes                    13866
c file_name                             VanDerWaerden_2-3-13_160.cnf

c running_time(sec)                     74.68
c number_of_nodes                       2616032
c number_of_binary_nodes                1308005
c number_of_1-reductions                35647489
c max_number_assignments                184
c max_number_changes                    18300
c file_name                             VanDerWaerden_2-3-14_185.cnf

c running_time(sec)                     1223.29
c number_of_nodes                       44231759
c number_of_binary_nodes                22115879
c number_of_1-reductions                602435895
c max_number_assignments                181
c max_number_changes                    18714
c file_name                             VanDerWaerden_2-3-14_186.cnf

c running_time(sec)                     1534.47
c number_of_nodes                       39433610
c number_of_binary_nodes                19716795
c number_of_1-reductions                584526688
c max_number_assignments                217
c max_number_changes                    25185
c file_name                             VanDerWaerden_2-3-15_217.cnf

c running_time(sec)                     12776.23
c number_of_nodes                       336982019
c number_of_binary_nodes                168491009
c number_of_1-reductions                5027250007
c max_number_assignments                217
c max_number_changes                    25808
c file_name                             VanDerWaerden_2-3-15_218.cnf

# ID 215c1f65d3368dab47052b512de966741940ab94
# The weights are now w_2=7.5, w_3=1, w_4=2^-1, w_5=2^-2, ...

c running_time(sec)                     14.06
c number_of_nodes                       857366
c number_of_binary_nodes                428676
c number_of_1-reductions                9983978
c max_number_assignments                134
c max_number_changes                    9815
c file_name                             VanDerWaerden_2-3-12_134.cnf

c running_time(sec)                     16.82
c number_of_nodes                       1003777
c number_of_binary_nodes                501888
c number_of_1-reductions                11774917
c max_number_assignments                132
c max_number_changes                    9884
c file_name                             VanDerWaerden_2-3-12_135.cnf

c running_time(sec)                     39.88
c number_of_nodes                       1875227
c number_of_binary_nodes                937604
c number_of_1-reductions                23829208
c max_number_assignments                157
c max_number_changes                    13710
c file_name                             VanDerWaerden_2-3-13_159.cnf

c running_time(sec)                     124.75
c number_of_nodes                       5811929
c number_of_binary_nodes                2905964
c number_of_1-reductions                74119386
c max_number_assignments                157
c max_number_changes                    13848
c file_name                             VanDerWaerden_2-3-13_160.cnf

c running_time(sec)                     63.27
c number_of_nodes                       2229826
c number_of_binary_nodes                1114902
c number_of_1-reductions                31487282
c max_number_assignments                184
c max_number_changes                    18247
c file_name                             VanDerWaerden_2-3-14_185.cnf

c running_time(sec)                     1059.86
c number_of_nodes                       38107857
c number_of_binary_nodes                19053928
c number_of_1-reductions                533296743
c max_number_assignments                182
c max_number_changes                    18748
c file_name                             VanDerWaerden_2-3-14_186.cnf

c running_time(sec)                     906.07
c number_of_nodes                       23469490
c number_of_binary_nodes                11734735
c number_of_1-reductions                363222334
c max_number_assignments                217
c max_number_changes                    25253
c file_name                             VanDerWaerden_2-3-15_217.cnf

c running_time(sec)                     9155.65
c number_of_nodes                       243443409
c number_of_binary_nodes                121721704
c number_of_1-reductions                3761900684
c max_number_assignments                213
c max_number_changes                    25535
c file_name                             VanDerWaerden_2-3-15_218.cnf

# ID 7da84e6c2727eb51d61117aea7a7938092d5712c

c running_time(sec)                     13.63
c number_of_nodes                       857366
c number_of_binary_nodes                428676
c number_of_1-reductions                9983978
c file_name                             VanDerWaerden_2-3-12_134.cnf

c running_time(sec)                     16.31
c number_of_nodes                       1003777
c number_of_binary_nodes                501888
c number_of_1-reductions                11774917
c file_name                             VanDerWaerden_2-3-12_135.cnf

c running_time(sec)                     38.85
c number_of_nodes                       1875227
c number_of_binary_nodes                937604
c number_of_1-reductions                23829208
c file_name                             VanDerWaerden_2-3-13_159.cnf

c running_time(sec)                     122.10
c number_of_nodes                       5811929
c number_of_binary_nodes                2905964
c number_of_1-reductions                74119386
c file_name                             VanDerWaerden_2-3-13_160.cnf

c running_time(sec)                     61.67
c number_of_nodes                       2229826
c number_of_binary_nodes                1114902
c number_of_1-reductions                31487282
c file_name                             VanDerWaerden_2-3-14_185.cnf

c running_time(sec)                     1039.53
c number_of_nodes                       38107857
c number_of_binary_nodes                19053928
c number_of_1-reductions                533296743
c file_name                             VanDerWaerden_2-3-14_186.cnf

c running_time(sec)                     891.87
c number_of_nodes                       23469490
c number_of_binary_nodes                11734735
c number_of_1-reductions                363222334
c file_name                             VanDerWaerden_2-3-15_217.cnf

c running_time(sec)                     8972.42
c number_of_nodes                       243443409
c number_of_binary_nodes                121721704
c number_of_1-reductions                3761900684
c file_name                             VanDerWaerden_2-3-15_218.cnf

# ID 4d644cd9007a1351d8c263db7dfbb50df33db946
# weight_2=7.0, weight_4=0.31, weight_5=0.19, basis_open = 1.70

c running_time(sec)                     13.53
c number_of_nodes                       864314
c number_of_binary_nodes                432150
c number_of_1-reductions                10105278
c file_name                             VanDerWaerden_2-3-12_134.cnf

c running_time(sec)                     15.73
c number_of_nodes                       977415
c number_of_binary_nodes                488707
c number_of_1-reductions                11522820
c file_name                             VanDerWaerden_2-3-12_135.cnf

c running_time(sec)                     36.00
c number_of_nodes                       1771688
c number_of_binary_nodes                885835
c number_of_1-reductions                22477004
c file_name                             VanDerWaerden_2-3-13_159.cnf

c running_time(sec)                     117.12
c number_of_nodes                       5642269
c number_of_binary_nodes                2821134
c number_of_1-reductions                72084484
c file_name                             VanDerWaerden_2-3-13_160.cnf

c running_time(sec)                     39.91
c number_of_nodes                       1476187
c number_of_binary_nodes                738082
c number_of_1-reductions                20813471
c file_name                             VanDerWaerden_2-3-14_185.cnf

c running_time(sec)                     950.33
c number_of_nodes                       35499881
c number_of_binary_nodes                17749940
c number_of_1-reductions                500275401
c file_name                             VanDerWaerden_2-3-14_186.cnf

c running_time(sec)                     503.95
c number_of_nodes                       13876432
c number_of_binary_nodes                6938206
c number_of_1-reductions                215404699
c file_name                             VanDerWaerden_2-3-15_217.cnf

c running_time(sec)                     7108.35
c number_of_nodes                       198353325
c number_of_binary_nodes                99176662
c number_of_1-reductions                3078439723
c file_name                             VanDerWaerden_2-3-15_218.cnf

c running_time(sec)                     59385.40
c number_of_nodes                       1534688845
c number_of_binary_nodes                767344422
c number_of_1-reductions                24555644336
c file_name                             VanDerWaerden_2-3-16_238.cnf

# ID 3a87d8d3cccb7056bb23597a2c6fcfb5eddc68ca

c running_time(sec)                     11.99
c file_name                             VanDerWaerden_2-3-12_134.cnf

c running_time(sec)                     13.78
c file_name                             VanDerWaerden_2-3-12_135.cnf

c running_time(sec)                     31.62
c file_name                             VanDerWaerden_2-3-13_159.cnf

c running_time(sec)                     102.23
c file_name                             VanDerWaerden_2-3-13_160.cnf

c running_time(sec)                     34.64
c file_name                             VanDerWaerden_2-3-14_185.cnf

c running_time(sec)                     834.54
c file_name                             VanDerWaerden_2-3-14_186.cnf

c running_time(sec)                     435.86
c file_name                             VanDerWaerden_2-3-15_217.cnf

c running_time(sec)                     6176.06
c file_name                             VanDerWaerden_2-3-15_218.cnf

c running_time(sec)                     51988.62
c file_name                             VanDerWaerden_2-3-16_238.cnf

# ID 7c0e6b352e43e587a9d8d113387d940402253029
# Version 1.9.1

c number_of_variables                   134
c number_of_clauses                     5172
c maximal_clause_length                 12
c number_of_literal_occurrences         22266
c running_time(sec)                     11.73
c number_of_nodes                       864314
c number_of_binary_nodes                432150
c number_of_1-reductions                10105278
c file_name                             VanDerWaerden_2-3-12_134.cnf

c number_of_variables                   135
c number_of_clauses                     5251
c maximal_clause_length                 12
c number_of_literal_occurrences         22611
c running_time(sec)                     13.58
c number_of_nodes                       977415
c number_of_binary_nodes                488707
c number_of_1-reductions                11522820
c file_name                             VanDerWaerden_2-3-12_135.cnf

c number_of_variables                   159
c number_of_clauses                     7216
c maximal_clause_length                 13
c number_of_literal_occurrences         31398
c running_time(sec)                     30.83
c number_of_nodes                       1771688
c number_of_binary_nodes                885835
c number_of_1-reductions                22477004
c file_name                             VanDerWaerden_2-3-13_159.cnf

c number_of_variables                   160
c number_of_clauses                     7308
c maximal_clause_length                 13
c number_of_literal_occurrences         31804
c running_time(sec)                     100.09
c number_of_nodes                       5642269
c number_of_binary_nodes                2821134
c number_of_1-reductions                72084484
c file_name                             VanDerWaerden_2-3-13_160.cnf

c number_of_variables                   185
c number_of_clauses                     9689
c maximal_clause_length                 14
c number_of_literal_occurrences         42542
c running_time(sec)                     34.03
c number_of_nodes                       1476187
c number_of_binary_nodes                738082
c number_of_1-reductions                20813471
c file_name                             VanDerWaerden_2-3-14_185.cnf

c number_of_variables                   186
c number_of_clauses                     9795
c maximal_clause_length                 14
c number_of_literal_occurrences         43014
c running_time(sec)                     816.46
c number_of_nodes                       35499881
c number_of_binary_nodes                17749940
c number_of_1-reductions                500275401
c file_name                             VanDerWaerden_2-3-14_186.cnf

c number_of_variables                   217
c number_of_clauses                     13239
c maximal_clause_length                 15
c number_of_literal_occurrences         58617
c running_time(sec)                     425.88
c number_of_nodes                       13876432
c number_of_binary_nodes                6938206
c number_of_1-reductions                215404699
c file_name                             VanDerWaerden_2-3-15_217.cnf

c number_of_variables                   218
c number_of_clauses                     13362
c maximal_clause_length                 15
c number_of_literal_occurrences         59166
c running_time(sec)                     6027.82
c number_of_nodes                       198353325
c number_of_binary_nodes                99176662
c number_of_1-reductions                3078439723
c file_name                             VanDerWaerden_2-3-15_218.cnf

c number_of_variables                   238
c number_of_clauses                     15812
c maximal_clause_length                 16
c number_of_literal_occurrences         70446
c running_time(sec)                     50810.66
c number_of_nodes                       1534688845
c number_of_binary_nodes                767344422
c number_of_1-reductions                24555644336
c file_name                             VanDerWaerden_2-3-16_238.cnf

# ID d8cab850756bfc0d1a8e7c1d61012630aa79f3d3
# Version 1.10.0
# cswsok with one other process running.

c running_time(sec)                     11.40
c number_of_nodes                       864314
c number_of_binary_nodes                432150
c number_of_1-reductions                9949777
c file_name                             VanDerWaerden_2-3-12_134.cnf

c running_time(sec)                     13.11
c number_of_nodes                       977415
c number_of_binary_nodes                488707
c number_of_1-reductions                11324097
c file_name                             VanDerWaerden_2-3-12_135.cnf

c running_time(sec)                     30.42
c number_of_nodes                       1771688
c number_of_binary_nodes                885835
c number_of_1-reductions                22100927
c file_name                             VanDerWaerden_2-3-13_159.cnf

c running_time(sec)                     100.51
c number_of_nodes                       5642269
c number_of_binary_nodes                2821134
c number_of_1-reductions                70960525
c file_name                             VanDerWaerden_2-3-13_160.cnf

c running_time(sec)                     34.30
c number_of_nodes                       1476187
c number_of_binary_nodes                738082
c number_of_1-reductions                20254109
c file_name                             VanDerWaerden_2-3-14_185.cnf

c running_time(sec)                     815.40
c number_of_nodes                       35499881
c number_of_binary_nodes                17749940
c number_of_1-reductions                488455067
c file_name                             VanDerWaerden_2-3-14_186.cnf

c running_time(sec)                     429.64
c number_of_nodes                       13876432
c number_of_binary_nodes                6938206
c number_of_1-reductions                207943615
c file_name                             VanDerWaerden_2-3-15_217.cnf

c running_time(sec)                     6122.86
c number_of_nodes                       198353325
c number_of_binary_nodes                99176662
c number_of_1-reductions                2988176312
c file_name                             VanDerWaerden_2-3-15_218.cnf

# ID ce0dd7e1da0f70bfe0c8b560a74e0110a01e2099
# Version 1.10.1

c running_time(sec)                     9.62
c file_name                             VanDerWaerden_2-3-12_134.cnf

c running_time(sec)                     11.19
c file_name                             VanDerWaerden_2-3-12_135.cnf

c running_time(sec)                     25.70
c file_name                             VanDerWaerden_2-3-13_159.cnf

c running_time(sec)                     84.94
c file_name                             VanDerWaerden_2-3-13_160.cnf

c running_time(sec)                     29.10
c file_name                             VanDerWaerden_2-3-14_185.cnf

c running_time(sec)                     690.75
c file_name                             VanDerWaerden_2-3-14_186.cnf

c running_time(sec)                     366.86
c file_name                             VanDerWaerden_2-3-15_217.cnf

c running_time(sec)                     5197.32
c file_name                             VanDerWaerden_2-3-15_218.cnf

# Version 2:
# cswsok, with one other process running

tawSolver:
 author: Tanbir Ahmed
 url: http://sourceforge.net/projects/tawsolver/
 Changes by Oliver Kullmann
 Version: 1.10.2
 Last change date: 14.7.2013
 Clause-weight parameters: 7, 1.7
  Mapping k -> weight for weights specified at compile-time:
   2->7  3->1  4->0.31  5->0.19
 Macro settings:
  LIT_TYPE = std::int32_t (with 31 binary digits)
 Compiled with NDEBUG
 Compiled with optimisation options
 Compilation date: Jul 14 2013 12:53:10
 Compiler: g++, version 4.7.3
 Provided in the OKlibrary http://www.ok-sat-library.org
 Git ID = 6d82c68830eccc0a5f3490bfddbdc24ea5fa8ce7

s SATISFIABLE
c number_of_variables                   134
c number_of_clauses                     5172
c maximal_clause_length                 12
c number_of_literal_occurrences         22266
c running_time(sec)                     9.52
c number_of_nodes                       864314
c number_of_binary_nodes                432150
c number_of_1-reductions                10105278
c file_name                             VanDerWaerden_2-3-12_134.cnf

s UNSATISFIABLE
c number_of_variables                   135
c number_of_clauses                     5251
c maximal_clause_length                 12
c number_of_literal_occurrences         22611
c running_time(sec)                     11.09
c number_of_nodes                       977415
c number_of_binary_nodes                488707
c number_of_1-reductions                11522820
c file_name                             VanDerWaerden_2-3-12_135.cnf

s SATISFIABLE
c number_of_variables                   159
c number_of_clauses                     7216
c maximal_clause_length                 13
c number_of_literal_occurrences         31398
c running_time(sec)                     25.79
c number_of_nodes                       1771688
c number_of_binary_nodes                885835
c number_of_1-reductions                22477004
c file_name                             VanDerWaerden_2-3-13_159.cnf

s UNSATISFIABLE
c number_of_variables                   160
c number_of_clauses                     7308
c maximal_clause_length                 13
c number_of_literal_occurrences         31804
c running_time(sec)                     84.30
c number_of_nodes                       5642269
c number_of_binary_nodes                2821134
c number_of_1-reductions                72084484
c file_name                             VanDerWaerden_2-3-13_160.cnf

s SATISFIABLE
c number_of_variables                   185
c number_of_clauses                     9689
c maximal_clause_length                 14
c number_of_literal_occurrences         42542
c running_time(sec)                     28.77
c number_of_nodes                       1476187
c number_of_binary_nodes                738082
c number_of_1-reductions                20813471
c file_name                             VanDerWaerden_2-3-14_185.cnf

s UNSATISFIABLE
c number_of_variables                   186
c number_of_clauses                     9795
c maximal_clause_length                 14
c number_of_literal_occurrences         43014
c running_time(sec)                     688.16
c number_of_nodes                       35499881
c number_of_binary_nodes                17749940
c number_of_1-reductions                500275401
c file_name                             VanDerWaerden_2-3-14_186.cnf

s SATISFIABLE
c number_of_variables                   217
c number_of_clauses                     13239
c maximal_clause_length                 15
c number_of_literal_occurrences         58617
c running_time(sec)                     365.22
c number_of_nodes                       13876432
c number_of_binary_nodes                6938206
c number_of_1-reductions                215404699
c file_name                             VanDerWaerden_2-3-15_217.cnf

s UNSATISFIABLE
c number_of_variables                   218
c number_of_clauses                     13362
c maximal_clause_length                 15
c number_of_literal_occurrences         59166
c running_time(sec)                     5195.18
c number_of_nodes                       198353325
c number_of_binary_nodes                99176662
c number_of_1-reductions                3078439723
c file_name                             VanDerWaerden_2-3-15_218.cnf

s UNSATISFIABLE
c number_of_variables                   238
c number_of_clauses                     15812
c maximal_clause_length                 16
c number_of_literal_occurrences         70446
c running_time(sec)                     43659.21
c number_of_nodes                       1534688845
c number_of_binary_nodes                767344422
c number_of_1-reductions                24555644336
c file_name                             VanDerWaerden_2-3-16_238.cnf
   \endverbatim
   </li>
   <li> From version 1.0 to version 1.8 a big improvement; when comparing the
   run-times and node-counts:
   \verbatim
> round(c(47,463,4577,47006,532416)/c(16,117,950,7108,59385), digits=1)
[1] 2.9 4.0 4.8 6.6 9.0
> round(c(1790733,13722975,102268511,774872707,8120609615)/c(977415,5642269,35499881,198353325,1534688845), digits=1)
[1] 1.8 2.4 2.9 3.9 5.3
   \endverbatim
   </li>
   <li> This solver is the fastest single solver, and in the new version even
   faster than Cube&Conquer via OKsolver&minisat22 (but basically comparable).
   </li>
   <li> We should also try C&C with this solver XXX. </li>
  </ul>


  \todo Performance of conflict-driven solvers
  <ul>
   <li> minisat2: (cswsok, 3GHz)
    <ol>
     <li> k=12, n=134: 1387192 conflicts, 22 restarts (258s). </li>
     <li> k=12, n=135: 1454696 conflicts, 22 restarts (273s). </li>
     <li> k=13, n=159: 525039 conflicts,  20 restarts (116s). </li>
     <li> k=13, n=160: 9298288 conflicts, 27 restarts (3022s). </li>
     <li> k=14, n=186: 60091581 conflicts, 32 restarts (33391s) </li>
     <li> k=15, n=218: 314678660 conflicts, 36 restarts (274457s) </li>
    </ol>
   </li>
   <li> minisat-2.2.0:
    <ol>
     <li> k=12, n=134: 61s (3605914 conflicts; cswsok (3GHz)) </li>
     <li> k=12, n=135: 107s (5963349 conflicts; cswsok (3GHz)) </li>
     <li> k=13, n=159: 13s (701558 conflicts; cswsok (3GHz)) </li>
     <li> k=13, n=160: 1716s (63901998 conflicts; cswsok (3GHz)) </li>
     <li> k=14, n=185: 147s (5619881 conflicts; cswsok (3GHz)) </li>
     <li> k=14, n=186: 16836s (463984635 conflicts; cswsok (3GHz)) </li>
     <li> k=15, n=217: 26319s (500756444 conflicts; cswsok (3GHz)) </li>
     <li> k=15, n=218: 190211s (3205639994 conflicts; cswsok (3GHz)) </li>
    </ol>
   </li>
   <li> cryptominisat-2.9.0:
    <ol>
     <li> k=12, n=134: 91s (619145 conflicts; csltok) </li>
     <li> k=12, n=135: 763s (2815643 conflicts; csltok) </li>
    </ol>
   </li>
   <li> cryptominisat-2.9.6:
    <ol>
     <li> k=12, n=134: 155s (1693268 conflicts; cswsok (3 GHz)) </li>
     <li> k=12, n=135: 212s (2109106 conflicts; cswsok (3 GHz)) </li>
     <li> k=13, n=160: 4630s (18137202 conflicts; cswsok (3 GHz)) </li>
     <li> k=14, n=186: 141636s (205583043 conflicts; cswsok (3GHz)) </li>
    </ol>
   </li>
   <li> picosat913:
    <ol>
     <li> k=12, n=134: 7s (368890 conflicts; cswsok (3 GHz)) </li>
     <li> k=12, n=135: 259s (9643671 conflicts; cswsok (3 GHz)) </li>
     <li> k=13, n=160: 4258s (82811468 conflicts; cswsok (3 GHz)) </li>
     <li> k=14, n=186: 48372s (576692221 conflicts; cswsok (3GHz)) </li>
    </ol>
   </li>
   <li> precosat236:
    <ol>
     <li> k=12, n=134: 52s (1145491 conflicts; cswsok (3 GHz)) </li>
     <li> k=12, n=135: 205s (3583785 conflicts; cswsok (3 GHz)) </li>
    </ol>
   </li>
   <li> precosat-570.1:
    <ol>
     <li> k=12, n=134: 91s (1531799 conflicts; cswsok (3 GHz)) </li>
     <li> k=12, n=135: 211s (2425722 conflicts; cswsok (3 GHz)) </li>
     <li> k=13, n=160: 2777s (16978254 conflicts; cswsok (3 GHz)) </li>
     <li> k=14, n=186: 47624s (140816236 conflicts; cswsok (3 GHz)) </li>
    </ol>
   </li>
   <li> lingelingala-b02aa1a-121013:
    <ol>
     <li> k=12, n=134: 205s (1659607 conflicts; cswsok (3 GHz)) </li>
     <li> k=12, n=135: 519s (3435610 conflicts; cswsok (3 GHz)) </li>
     <li> k=13, n=160: 7651s (24124525 conflicts; cswsok (3 GHz)) </li>
     <li> k=14, n=186: 107243s (176909499 conflicts; cswsok (3 GHz)) </li>
    </ol>
   </li>
   <li> glucose-1.0:
    <ol>
     <li> k=12, n=134: 39s (425399 conflicts; csltok) </li>
     <li> k=12, n=135: 191s (1356325 conflicts; csltok) </li>
     <li> k=13, n=159: 139s (957255 conflicts; csltok) </li>
     <li> k=13, n=160: 3274s (9907932 conflicts; csltok) </li>
    </ol>
   </li>
   <li> glucose-2.0:
    <ol>
     <li> k=12, n=134: 5s (169420 conflicts; cswsok (3 GHz)) </li>
     <li> k=12, n=135: 58s (1263087 conflicts; cswsok (3 GHz)) </li>
     <li> k=13, n=159: 1s (50528 conflicts; cswsok (3 GHz)) </li>
     <li> k=13, n=160: 781s (8377487 conflicts; cswsok (3 GHz)) </li>
     <li> k=14, n=185: 5133s (31516583 conflicts; cswsok (3 GHz)) </li>
     <li> k=14, n=186: 84334s (163500051 conflicts; cswsok (3 GHz)) </li>
    </ol>
   </li>
   <li> glucose-2.2:
    <ol>
     <li> k=12, n=134: 84s (1350204 conflicts; cswsok (3 GHz)) </li>
     <li> k=12, n=135: 94s (1444017 conflicts; cswsok (3 GHz)) </li>
     <li> k=13, n=159: 87s (1332829 conflicts; cswsok (3 GHz)) </li>
     <li> k=13, n=160: 1412s (10447051 conflicts; cswsok (3 GHz)) </li>
     <li> k=14, n=186: Aborted (cswsok (3 GHz)):
     \verbatim
c | 12782852     54452       53 |     186     9795    43014 |   727  2068402      151 687161597 |  0.003 % |
Terminated
real    15723m27.101s
user    15667m20.240s
sys     1m11.570s
     \endverbatim
     </li>
    </ol>
    Performance much worse -- apparently due to much more restarts!
   </li>
   <li> minisat-2.2.0 and glucose-2.0 seem best (for the conflict-driven
   solvers, while satz215 seems best overall); however SplittingViaOKsolver
   with minisat-2.2 is much better than satz215. </li>
  </ul>


  \todo SplittingViaOKsolver
  <ul>
   <li> Using minisat-2.2.0 (cswsok, 3 GHz):
   \verbatim
> SplittingViaOKsolver -D20 VanDerWaerden_2-3-13_160.cnf
> cd SplitViaOKsolver_D20VanDerWaerden_2313_160cnf_2013-05-17-001649
> more Md5sum
16c1083271af4be87d875298d869a6c7
> more Statistics
> summary(E$n)
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
  20.00   20.00   23.00   23.23   25.00   37.00
> table(E$n)
 20  21  22  23  24  25  26  27  28  29  30  31  32  34  37
409 111 125 196 233 201 151  96  52  12   2   2   4   2   3
> summary(E$d)
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
   5.00   12.00   13.00   13.21   15.00   18.00
> table(E$d)
  5   6   7   8   9  10  11  12  13  14  15  16  17  18
  3  10  28  40  61  86 138 193 264 240 217 195 112  12
> more Result
s UNKNOWN
c sat_status                            2
c initial_maximal_clause_length         13
c initial_number_of_variables           160
c initial_number_of_clauses             7308
c initial_number_of_literal_occurrences 31804
c number_of_initial_unit-eliminations   0
c reddiff_maximal_clause_length         0
c reddiff_number_of_variables           0
c reddiff_number_of_clauses             0
c reddiff_number_of_literal_occurrences 0
c number_of_2-clauses_after_reduction   0
c running_time(sec)                     9.7
c number_of_nodes                       3197
c number_of_single_nodes                0
c number_of_quasi_single_nodes          0
c number_of_2-reductions                110
c number_of_pure_literals               0
c number_of_autarkies                   0
c number_of_missed_single_nodes         0
c max_tree_depth                        18
c proportion_searched                   0.000000e+00
c proportion_single                     0.000000e+00
c total_proportion                      0
c number_of_table_enlargements          0
c number_of_1-autarkies                 0
c number_of_new_2-clauses               0
c maximal_number_of_added_2-clauses     0
c file_name                             VanDerWaerden_2-3-13_160.cnf
c splitting_directory                   SplitViaOKsolver_D20VanDerWaerden_2313_160cnf_2013-05-17-001649/Insta
nces
c splitting_cases                       1599

> cd ..
> ProcessSplitViaOKsolver SplitViaOKsolver_D20VanDerWaerden_2313_160cnf_2013-05-17-001649
> cat Process_SplitViaOKsolver_D20VanDerWaerden_2313_160cnf_2013-05-17-001649_2013-05-17-002115/Result
  2:35

> SplittingViaOKsolver -D30 VanDerWaerden_2-3-14_186.cnf
c running_time(sec)                     145.7
c splitting_cases                       13527
> ProcessSplitViaOKsolver SplitViaOKsolver_D30VanDerWaerden_2314_186cnf_2013-05-17-002903/
> cat Process_SplitViaOKsolver_D30VanDerWaerden_2314_186cnf_2013-05-17-002903_2013-05-17-003312/Result
  22:08

> SplittingViaOKsolver -D30 VanDerWaerden_2-3-15_218.cnf
c running_time(sec)                     231.8
c splitting_cases                       12922
> ProcessSplitViaOKsolver SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710
> cat Process_SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710_2013-05-17-020436/Result
  141:27
> E=read_processsplit_minisat()
12922: 2.226h, sum-cfs=3.806417e+08, mean-t=0.620s, mean-cfs=29457, sat: 0
$t:
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
 0.0150  0.2320  0.3969  0.6203  0.7576 15.5700
sd= 0.6646329
      95%       96%       97%       98%       99%      100%
 1.866720  2.026010  2.252030  2.599404  3.274080 15.569600
sum= 8015.144
$cfs:
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
    408   11500   19900   29460   36550  636800
sd= 29454.45
      95%       96%       97%       98%       99%      100%
 85033.00  92782.64 102557.00 117306.90 145427.27 636785.00
sum= 380641711
$t ~ $cfs:
               Estimate  Std. Error  t value  Pr(>|t|)
(Intercept) -4.3157e-02  5.0876e-04  -84.827 < 2.2e-16 ***
E$cfs        2.2522e-05  1.2214e-08 1844.020 < 2.2e-16 ***
R-squared: 0.9962

> SplittingViaOKsolver -D35 VanDerWaerden_2-3-15_218.cnf
c running_time(sec)                     821.3
c splitting_cases                       32331
> ProcessSplitViaOKsolver SplitViaOKsolver_D35VanDerWaerden_2315_218cnf_2013-05-17-090420
> cat Process_SplitViaOKsolver_D35VanDerWaerden_2315_218cnf_2013-05-17-090420_2013-05-17-092127/Result
  134:31
> E=read_processsplit_minisat()
32331: 1.913h, sum-cfs=3.615115e+08, mean-t=0.213s, mean-cfs=11182, sat: 0
$t:
    Min.  1st Qu.   Median     Mean  3rd Qu.     Max.
0.005999 0.089990 0.158000 0.213100 0.274000 3.906000
sd= 0.1911241
     95%      96%      97%      98%      99%     100%
0.574912 0.619905 0.676897 0.752885 0.910861 3.906410
sum= 6888.165
$cfs:
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
     41    4755    8441   11180   14400  180100
sd= 9784.68
     95%      96%      97%      98%      99%     100%
 29808.0  32137.8  34737.2  38804.8  46902.0 180053.0
sum= 361511501
$t ~ $cfs:
               Estimate  Std. Error  t value  Pr(>|t|)
(Intercept) -4.8847e-03  1.0623e-04  -45.985 < 2.2e-16 ***
E$cfs        1.9491e-05  7.1493e-09 2726.231 < 2.2e-16 ***
R-squared: 0.9957

> SplittingViaOKsolver -D40 VanDerWaerden_2-3-16_238.cnf
c running_time(sec)                     3247.6
c splitting_cases                       104797
> ProcessSplitViaOKsolver SplitViaOKsolver_D40VanDerWaerden_2316_238cnf_2013-05-17-140911
> E=read_processsplit_minisat()
104797: 22.238h, sum-cfs=3.723995e+09, mean-t=0.764s, mean-cfs=35535, sat: 0
$t:
     Min.   1st Qu.    Median      Mean   3rd Qu.      Max.
 0.009998  0.266000  0.459900  0.763900  0.853900 29.560000
sd= 0.9925851
      95%       96%       97%       98%       99%      100%
 2.414630  2.746580  3.184759  3.804420  4.983320 29.558500
sum= 80056.43
$cfs:
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
     71   12830   22290   35540   40960 1128000
sd= 42953.37
      95%       96%       97%       98%       99%      100%
 110594.8  124136.6  142799.4  167161.1  218275.0 1127634.0
sum= 3723995162
$t ~ $cfs:
               Estimate  Std. Error t value  Pr(>|t|)
(Intercept) -5.5472e-02  2.6144e-04 -212.18 < 2.2e-16 ***
E$cfs        2.3059e-05  4.6897e-09 4916.80 < 2.2e-16 ***
R-squared: 0.9957

> SplittingViaOKsolver -D50 VanDerWaerden_2-3-17_279.cnf
c running_time(sec)                     23546.1
c number_of_nodes                       1399505
c number_of_2-reductions                82906
c max_tree_depth                        41
c splitting_cases                       699751
> ProcessSplitViaOKsolver SplitViaOKsolver_D50VanDerWaerden_2317_279cnf_2013-05-18-154809
> cat Process_SplitViaOKsolver_D50VanDerWaerden_2317_279cnf_2013-05-18-154809_2013-05-19-084513/Result
  17439:22
> E=read_processsplit_minisat()
699751: 11.652d, sum-cfs=3.593149e+10, mean-t=1.439s, mean-cfs=51349, sat: 0
$t:
    Min.  1st Qu.   Median     Mean  3rd Qu.     Max.
  0.0100   0.4569   0.8199   1.4390   1.5770 199.0000
sd= 2.289549
      95%       96%       97%       98%       99%      100%
  4.56531   5.16921   5.95509   7.39288  10.27740 199.02700
sum= 1006718
$cfs:
   Min. 1st Qu.  Median    Mean 3rd Qu.    Max.
     31   17740   30880   51350   58130 4782000
sd= 71854.53
      95%       96%       97%       98%       99%      100%
 158454.0  177082.0  203221.5  249985.0  335532.0 4781946.0
sum= 35931491146
$t ~ $cfs:
               Estimate  Std. Error t value  Pr(>|t|)
(Intercept) -1.8913e-01  3.3949e-04  -557.1 < 2.2e-16 ***
E$cfs        3.1701e-05  3.8441e-09  8246.8 < 2.2e-16 ***
R-squared: 0.9898
   \endverbatim
   A big speed-up!
   </li>
   <li> Using glucose 2.0 (cswsok, 3 GHz):
   \verbatim
> solver="glucose-2.0" ProcessSplitViaOKsolver SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710
> cat Process_SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710_2013-05-17-180553/Result
  187:11
   \endverbatim
   Reasonable fast, but minisat-2.2.0 is faster.
   </li>
   <li> Using glucose 2.2 (cswsok, 3 GHz):
   \verbatim
> solver="glucose-2.2" ProcessSplitViaOKsolver SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710
> cat Process_SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710_2013-05-19-084751/Result
  132:00

> solver="glucose-2.2" ProcessSplitViaOKsolver SplitViaOKsolver_D40VanDerWaerden_2316_238cnf_2013-05-17-140911
  1288:01
   \endverbatim
   This is now faster than glucose-2.0, but basically the same as
   minisat-2.2.0 (for k=16 a bit faster).
   </li>
   <li> Using satz (cswsok, 3 GHz):
   \verbatim
> solver="satz215" ProcessSplitViaOKsolver SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710
# aborted after 2071 from 12922 sub-instances:
> E=read_processsplit_satz()
2071: 21.955h, sum-nds=1.363216e+08, mean-t=38.165s, mean-nds=65824, sat: 0
$t:
    Min.  1st Qu.   Median     Mean  3rd Qu.     Max.
   0.120    0.890    1.420   38.160    2.305 6503.000
sd= 339.4337
     95%      96%      97%      98%      99%     100%
   4.845    5.630    6.979   39.830 1316.095 6503.010
sum= 79038.94
$nds:
    Min.  1st Qu.   Median     Mean  3rd Qu.     Max.
      34     1438     2465    65820     4323 11620000
sd= 586093.9
       95%        96%        97%        98%        99%       100%
    9509.0    11161.4    14417.8    83698.2  2248985.2 11621345.0
sum= 136321627
$t ~ $nds:
              Estimate Std. Error   t value Pr(>|t|)
(Intercept) 6.2299e-02 2.3939e-01    0.2602   0.7947
E$nds       5.7885e-04 4.0600e-07 1425.7567   <2e-16 ***
R-squared: 0.999
   \endverbatim
   That is cleary worse than just plain satz215. </li>
   <li> Using tawSolver (cswsok, 3 GHz):
   \verbatim
> solver="tawSolver" ProcessSplitViaOKsolver SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710
> cat Process_SplitViaOKsolver_D30VanDerWaerden_2315_218cnf_2013-05-17-015710_2013-07-10-015133/Result
  129:07

> solver="tawSolver" ProcessSplitViaOKsolver SplitViaOKsolver_D35VanDerWaerden_2315_218cnf_2013-05-17-090420/
> cat Process_SplitViaOKsolver_D35VanDerWaerden_2315_218cnf_2013-05-17-090420_2013-07-10-042958/Result
  156:32

> solver="tawSolver" ProcessSplitViaOKsolver SplitViaOKsolver_D40VanDerWaerden_2316_238cnf_2013-05-17-140911
> cat Process_SplitViaOKsolver_D40VanDerWaerden_2316_238cnf_2013-05-17-140911_2013-07-10-113315/Result
  1478:36
XXX report the details XXX
   \endverbatim
   </li>
  </ul>

*/
