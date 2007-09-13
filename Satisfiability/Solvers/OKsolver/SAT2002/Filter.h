// Oliver Kullmann, 3.2.2001 (Toronto)

/*!
  \file Solvers/OKsolver/SAT2002/Filter.h
  \brief Header file for the variable-"filtering" function, used for selecting the branching variable
*/

                       /* OKsolver; 5.3.1998 */

/* Autor: Oliver Kullmann, Universitaet Frankfurt am Main, Germany */
/* ab Februar 1999: University of Toronto, Computer Science Department */

/* Angelegt: 3.2.2001 */

#ifndef GELESENFILTER_hTrEw09u
#define GELESENFILTER_hTrEw09u

#include "VarLitKlm.h"
/* #include "Klauselmenge.h" */

extern void Filter (VAR v);
extern void InitFilter ( void );

extern bool erfuellt;
extern VZ Zweig;
extern bool Wahl;
extern bool Single;
extern bool reduziert;
extern unsigned int Schalter;
extern unsigned int DeltaN [ 2 ] [ 2 ];
extern unsigned int LaP [ 2 ] [ 2 ];

#ifndef BAUMRES
typedef LIT StapeleintragF;
#else
typedef struct {
  LIT l;
  KLN k;
} StapeleintragF;
#endif
typedef StapeleintragF * StapeleintragFZ;

extern StapeleintragFZ Huelle [ 2 ] [ 2 ];

extern int *DeltaK [ 2 ] [ 2 ];
extern unsigned int *NeuK [ 2 ] [ 2 ];
extern unsigned int *LaAnzK [ 2 ] [ 2 ];

#endif



