// Oliver Kullmann, 10.5.2010 (Swansea)
/* Copyright 2010 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*!
  \file Experimentation/Competition/TwoSolverComparison/plans/TwoSolverComparison.hpp
  \brief Plans regarding the method for comparing two solvers according to [Mladen Nikolic, 2010]

  [Mladen Nikolic, 2010, Statistical Methodology for Comparison of
  SAT Solvers] is to appear in SAT 2010 proceedings.


  \todo The wider framework
  <ul>
   <li> See the "SAT Evaluation Toolkit"
   (Experimentation/Competition/plans/general.hpp). </li>
   <li> DONE (see TwoSolverComparison.R)
   The first stage is to implement the statistical evaluation in R. </li>
  </ul>


  \todo Improving TwoSolverComparison.R
  <ul>
   <li> Improve filename:
    <ol>
     <li> The submodule carries the generic name, while every file in it
     should be named according to its *specific* content. </li>
     <li> So "TwoSolverComparison.R" (and accordingly this plans-file) should
     be named according to the specific method implemented. </li>
    </ol>
   </li>
   <li> Enter file-creation-date. </li>
   <li> Specify functions:
    <ol>
     <li> gwtest </li>
     <li> gehanw </li>
     <li> rcor </li>
     <li> jackknifevariance </li>
     <li> bootstrapvariance </li>
     <li> TwoSolverComparison: this function also needs to be renamed, so that
     the specific method becomes clear (and no use of camel-case). </li>
    </ol>
   </li>
  </ul>


  \todo Write docus
  <ul>
   <li> Input specification:
    <ol>
     <li> DONE (yes, this is the case)
     It appears that the input is just a data-frame file, which per row
     contains the times for the two solvers A, B to be compared. </li>
     <li> DONE (yes, needed)
     Is the knowledge of the cutoff-time needed for the evaluation? </li>
     <li> DONE (one row per instance
     As discussed in the paper, there are actually dependencies between
     the rows, in that a block of rows belongs to a single instances and its
     shuffled variations --- however it seems for the computations performed
     this is irrelevant? </li>
    </ol>
   </li>
  </ul>

*/
