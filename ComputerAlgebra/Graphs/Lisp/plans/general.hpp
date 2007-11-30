// Oliver Kullmann, 24.11.2007 (Swansea)
/* Copyright 2007 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*!
  \file ComputerAlgebra/Graphs/Lisp/plans/general.hpp
  \brief Plans for graphs via computer-algebra-systems in general


  \todo Create milestones


  \todo Graph concepts
  <ul>
   <li> A graph is just a 2-element list, consisting of the set of
   vertices, and a set of 2-element vertex sets. </li>
   <li> How to handle "properties" ? </li>
   <li> Can we tag such objects as being "graphs" ? </li>
  </ul>


  \todo Generators.mac
  <ul>
   <li> Extend the documentation. </li>
   <li> Write documentation. </li>
   <li> Output :
    <ol>
     <li> Can we add comments to the graph-output? </li>
     <li> Should we give the graphs in the dot-output a dedicated name? </li>
     <li> Generalise the graph output, so that arbitrary graphs
     can be output (making function "string_vertex" a parameter). </li>
     <li> Move graph output to dedicated "InputOutput.mac". </li>
    </ol>
   </li>
   <li> Generalisations of the Kneser graphs:
    <ol>
     <li> The Johnson graphs J(n,k,i), consisting like the Kneser
     graphs of all k-subsets of n, while we have an edge joining
     two vertices if the intersection has exactly size i. </li>
     <li> The generalised Kneser graph K(n,k,t), the union
     of J(n,k,i) for 0 <= i < t. </li>
     <li> The Kneser graph K(G) of a hypergraph G, with vertices
     the hyperedges, joined by an edge if disjoint. </li>
    </ol>
   </li>
  </ul>

*/

