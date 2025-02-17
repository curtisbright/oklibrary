/*********************************************************************
tawSolver -- A basic and efficient DLL SAT solver
Copyright (c) 2007-2013 Tanbir Ahmed http://users.encs.concordia.ca/~ta_ahmed/
Copyright 2013, 2015, 2016, 2017, 2018, 2020, 2021, 2022 Oliver Kullmann http://www.cs.swan.ac.uk/~csoliver/

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************/

/*
  An efficient implementation of the original DLL algorithm, using the
  theory of look-ahead heuristics as developed in

  Fundaments of Branching Heuristics, Handbook of Satisfiability, Chapter 7
  http://cs.swan.ac.uk/~csoliver/papers.html#Handbook2009Fundaments
  https://doi.org/10.3233/978-1-58603-929-5-205

  This implementation is discussed in

  On the van der Waerden numbers w(2;3,t)
  http://www.cs.swan.ac.uk/~csoliver/papers.html#VDW232011
  http://dx.doi.org/10.1016/j.dam.2014.05.007

  The original DLL algorithm is from

  A machine program for theorem-proving
  https://doi.org/10.1145/368273.368557

  The Wikipedia page
  https://en.wikipedia.org/wiki/DPLL_algorithm
  contains some historical inaccuracies (e.g., "DLL" is the only correct name
  for this special algorithm), but is otherwise reasonable.


  COMPILE with

> g++ --std=c++11 -Wall -Ofast -DNDEBUG -o tawSolver tawSolver.cpp

  possibly with additional optimisation-options

    -funroll-loops -funsafe-loop-optimizations -fno-math-errno
    -funsafe-math-optimizations -fwhole-program

  or with

> g++ --std=c++11 -Wall -g -o tawSolver_debug tawSolver.cpp

  for debugging.

  Using the macros below, many compilation-variations are possible; the main
  alternative is to compute all solutions (represented by partial assignments),
  which is obtained (similar to above) via

> g++ --std=c++11 -Wall -Ofast -DNDEBUG -DALL_SOLUTIONS -o ctawSolver tawSolver.cpp


  Alternatively the makefile (called "makefile") in this
  directory (of the OKlibrary https://github.com/OKullmann/oklibrary )
  can be used: it contains various options, but with

> make all

  it creates the four versions

    tawSolver, ttawSolver, ctawSolver, cttawSolver

  where prefix "t" stands for "tau", the alternative heuristics, while "c"
  stands for "counting". With

> make allall

  four additional debug-versions (named "*_debug") are created. With

> make clean

  the debug-versions are removed, and with

> make cleanall

  additionally also the four (optimised) programs.


  USAGE:

> tawSolver [argument1] [argument2] [argument3]

   - without arguments shows usage and exits with 0
   - with argument1= "-v" or "--version" shows information and exits with 0;
   - with argument1=filename or "-cin" runs the SAT solver, with input from
     file or standard input;
   - with argument2=filename or "-cout" or "-cerr" or "-nil" the solutions
     are output to file, standard output, standard error or are ignored;
     if argument2 is not given, then the default is -cout, except for
     ALL_SOLUTIONS is defined, where then the default is -nil;
   - with argument3=filename or "-cout" or "-cerr" or "-nil" the statistics
     are output to file, standard output, standard error or are ignored;
     if VAR_MARGINALS is defined (which includes ALL_SOLUTIONS), then at
     the end of the statistics for the variables the pairs "v:count" are
     output, where count is the number of satisfying assignments with v=true;
     if argument3 is not given, then the default is -cout.

  Output to file means appending.

  Empty clauses or unit-clauses in the input are errors.
  The parameters in the p-line are considered as upper bounds (so not reaching
  them is not considered an error), where tautological clauses are ignored.

  When sending SIGINT to the program (for example via CTRL-C from the calling
  terminal), then the current state of statistics is output, and computation
  is aborted. When sending SIGUSR1, then the statistics is output, and the
  computation is continued.

  There are the following macros to control compilation:

   - LIT_TYPE (default std::int32_t)
   - WEIGHT_2, WEIGHT_4, WEIGHT_5, WEIGHT_6 and WEIGHT_BASIS_OPEN:
     the weight for clause-length k=3 is standardised to 1, the weights for
     k = 2, 4, 5, 6 are given by the first four macros, and the
     non-predetermined weights (w.r.t. the initialisation of vector "weights")
     are obtained by repeatedly dividing the last predetermined weight by
     WEIGHT_BASIS_OPEN.
   - UCP_STRATEGY (default 1): 0 means BFS-processing, 1 means DFS-processing
     of unit-clauses.
   - PURE_LITERALS: if defined (default is undefined), then pure literals are
     eliminated (as they are encountered, not iteratively).
   - TAU_ITERATION: if defined (default is undefined), then  the in principle
     more accurate, but also more costly tau-function as projection (instead
     of the product);
     If TAU_ITERATION, then TWEIGHT_2, TWEIGHT_4, TWEIGHT_5, TWEIGHT_6 and
     TWEIGHT_BASIS_OPEN are used.
   - ALL_SOLUTIONS: if defined (default is undefined), then all solutions are
     computed, and they are output as soon as when they are found;
     incompatible with PURE_LITERALS.
   - VAR_MARGINALS, which implies ALL_SOLUTIONS means that the statistics
     list at the end for each variable the number of satisfying assignments
     where this variable is set to true.
   - COUNT_T: the count-type in case of ALL_SOLUTIONS; by default an unsigned
     integral type with at least 64 bits (so modular arithmetic is performed,
     namely modulo 2^count_bits);
     can also be defined as e.g. "double" or "long double", in which case the
     result may become "inf".

  To provide further version-information, there are two macros, which are
  only relevant if they are defined:
   - GIT_ID (for the identity hash-value of the Git repository)
   - OKLIB (with the url (without prefix "http://") for the OKlibrary.

  Remarks on the statistics output:
   - The (only) reduction on the input is removal of tautological clauses,
     and contraction of repeated literals; the reported number_of_clauses,
     maximal_clause_length and number_of_literal_occurrences refer to the
     result of this reduction.
   - p_param_variables is the n-value on the p-line (as an upper
     bound on the maximal absolute value of literals in the input).
   - max_occurring_variable is the maximum of absolute values of literals in
     the input.
   - "running_time(sec)" is only solver-time.
   - running_time + reading-and-set-up_time is total time.
   - A "binary node" is one with (exactly) two children.
   - A "1-reduction" is an assignment due to unit-clause propagation.
   - "options" yields a summary of the main options:
    - "B" for UCP_STRATEGY = 0
    - "P" for PURE_LITERALS
    - "T" followed with its value if defined
    - "A" for ALL_SOLUTIONS, plus "F" in case of floating-point counting, and
      followed by the number of (decimal) digits.
    - If VAR_MARGINALS is defined, then instead of "A" there is "M".

  If solutions-output is cout, then it comes after the statistics, except for
  the case where ALL_SOLUTIONS is defined, where then naturally the solutions
  come before the (final) statistics.

  For counting of solutions, the value reported in p_param_variables is used
  as the number of variables. So for example we have

> echo "p cnf 63 0" | ctawSolver -cin
s SATISFIABLE
c max_occurring_variable                0
c number_of_clauses                     0
c maximal_clause_length                 0
c number_of_literal_occurrences         0
c running_time(sec)                     0.00
c number_of_nodes                       1
c number_of_binary_nodes                0
c number_of_1-reductions                0
c number_of_solutions                   9223372036854775808
c reading-and-set-up_time(sec)          0.000
c p_param_variables                     63
c p_param_clauses                       0
c number_tautologies                    0
c file_name                             -cin
c options                               "A19"

  that is, we have zero clauses, but 2^63 solutions: the point is that
  the input specified there are 63 variables (which here are merely "formal",
  they do not occur in the input), and since there are no clauses, there
  are no restrictions on them.

  A time-out is currently not provided by the solver, but can be achieved
  with the tool "timeout" (Linux/Unix), for example a time-out of 0.7s:

> timeout --signal=SIGINT 0.7 tawSolver [options]
or
> timeout -s SIGINT 0.7 tawSolver [options]

*/

#include <limits>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <type_traits>
#include <iomanip>
#include <exception>
#include <algorithm>
#include <chrono>

#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cassert>
#include <csignal>

namespace {

#define is_pod(X) std::is_standard_layout_v<X> and std::is_trivial_v<X>

// --- General input and output ---

const std::string version = "2.20.2";
const std::string date = "16.4.2022";

#if defined WEIGHT_2 | defined WEIGHT_4 | defined WEIGHT_5 | defined WEIGHT_6 | defined WEIGHT_BASIS_OPEN | defined TWEIGHT_2 | defined TWEIGHT_4 | defined TWEIGHT_5 | defined TWEIGHT_6 | defined TWEIGHT_BASIS_OPEN
# define WEIGHT_DEFINED
#endif

#ifdef FIRST_VARIABLE
# if defined TAU_ITERATION | defined LAMBDA | defined ALPHA | defined WEIGHT_DEFINED
#  error "FIRST_VARIABLE incompatible with any other heuristics-option.\n"
# endif
# ifndef PURE_LITERALS
#  error "FIRST_VARIABLE must be combined with PURE_LITERALS.\n"
# endif
#endif

const std::string program =
#if defined ALL_SOLUTIONS
  "c"
#elif defined VAR_MARGINALS
  "m"
#endif
#ifdef TAU_ITERATION
  "t"
#endif
#ifdef FIRST_VARIABLE
  "f"
#endif
  "tawSolver"
#ifdef WEIGHT_DEFINED
  "_wa"
#endif
#ifdef LAMBDA
  "_lambda"
#endif
#ifdef ALPHA
  "_alpha"
#endif
#ifndef NDEBUG
  "_debug"
#endif
;

enum Error_codes {
  file_reading_error=1,
  file_writing_error=2,
  file_pline_error=3,
  num_vars_error=4,
  allocation_error=5,
  literal_read_error=6,
  variable_value_error=7,
  number_clauses_error=8,
  empty_clause_error=9,
  unit_clause_error=11
};

// The following three return-values are not errors:
enum Result_value { unsat=20, sat=10, unknown=0 };

typedef bool DLL_return_t;
inline constexpr Result_value interprete_run(const DLL_return_t result) noexcept {
  return result ? sat : unsat;
}


/* Class for output-objects solout, logout, errout, which are initialised
   by function set_output from the command-line parameters. The two
   public members (besides the constructor) are
     out << x;
     out.endl();
  which send the output to the internally stored ostream *p, if set.
  The destructor deletes *p iff member del = true.
*/
class Output {
  std::ostream* p = nullptr;
  bool del = false;
  friend void set_output(const int, const char* const*);
  friend class Outputerr;
public :
  bool active() const noexcept { return p != nullptr; }
  ~Output() { if (del) delete p; }
  template <typename T>
  const Output& operator <<(const T& x) const { if (p) *p << x; return *this; }
  void endl() const { if (p) {*p << "\n"; p->flush();} }
  std::streamsize precision(const std::streamsize pr) const noexcept {
    assert(p); return p->precision(pr);
  }
};
Output solout;
Output logout;

// Error output with ERROR-prefix, and each on a new line:
struct Outputerr : Output {
  const std::string e = "ERROR[" + program + "]: ";
  template <typename T>
  const Outputerr& operator <<(const T& x) const {
    if (p) *p << e << x << "\n";
    return *this;
  }
};
Outputerr errout;


// --- Data structures for literals and variables ---

/*
  The basic classes are Var (variables) and Lit (literals), where
  Lit contains Lit_int, "Literals as integers", which are signed integers,
  while variables are unsigned.

  Polarities pos, neg are expressed via the enumeration-type Polarity.

  Operations for Lit_int x, Lit y,y', Var v, Polarity p:

   - Lit() (the singular literal)
   - copy-construction, assignment for Lit
   - Lit(x) (non-converting)
   - Lit(v, p)
   - bool(y) (explicit; true iff x is not singular)
   - -y, -p
   - y == y', y != y'
   - var(y) (yields Var)
   - sign(y) (yields Polarity)
   - ostream << y, istream >> y

   Lit-literals are constructed by n_l for unsigned long-long n.
*/

#ifndef LIT_TYPE
# define LIT_TYPE std::int32_t
#endif
typedef LIT_TYPE Lit_int;
static_assert(std::is_signed<Lit_int>::value, "Type \"Lit_int\" must be signed integral.");
static_assert(sizeof(Lit_int) != 1, "Lit_int = char (or int8_t) doesn't work with reading (since not numbers are read, but characters).");
constexpr Lit_int max_lit = std::numeric_limits<Lit_int>::max();
static_assert(- -max_lit == max_lit, "Problem with negating max_lit.");

typedef std::make_unsigned<Lit_int>::type Var;
static_assert(Lit_int(Var(max_lit)) == max_lit, "Problem with Var and Lit_int.");
inline constexpr bool valid(const Var v) noexcept { return v <= Var(max_lit); }

enum Polarity { pos=0, neg=1 };
inline constexpr Polarity operator -(const Polarity p) noexcept {
  return (p==pos) ? neg:pos;
}

class Lit {
  Lit_int x;
public :
  Lit() = default;
  constexpr explicit Lit(const Lit_int x) noexcept : x(x) {}
  constexpr Lit(const Var v, const Polarity p) noexcept : x(p==pos?v:-Lit_int(v)) {}
  constexpr explicit operator bool() const noexcept { return x; }
  constexpr Lit operator -() const noexcept { return Lit(-x); }
  constexpr bool operator ==(const Lit y) const noexcept { return x == y.x; }
  constexpr bool operator !=(const Lit y) const noexcept { return x != y.x; }
  friend constexpr Var var(const Lit x) noexcept { return std::abs(x.x); }
  friend constexpr Polarity sign(const Lit x) noexcept {return (x.x >= 0) ? pos : neg;}
  friend std::ostream& operator <<(std::ostream& out, const Lit x) {
    return out << x.x;
  }
  friend std::istream& operator >>(std::istream& in, Lit& x) {
    return in >> x.x;
  }
};
static_assert(is_pod(Lit), "Lit is not POD.");

inline constexpr Lit operator"" _l(const unsigned long long x) noexcept {return Lit(x);}
static_assert(0_l == Lit(), "Problem with default construction of Lit.");
// Remark: As usual, as a local variable, the declaration "Lit x;" does not
// initialise x.
static_assert(1_l==Lit(1) and -1_l==Lit(-1), "Problem with construction of Lit.");
static_assert(not 0_l, "Problem with conversion of singular literal to bool.");
static_assert(1_l and -1_l, "Problem with conversion of valid literal to bool.");
static_assert(1_l != -1_l, "Problem with negation and/or inequality.");
static_assert(- -1_l == 1_l, "Problem with double negation.");
static_assert(0_l == -0_l, "Problem with negation of singular literal.");
static_assert(var(-1_l) == 1, "Problem with var().");
static_assert(sign(-1_l) == neg, "Problem with sign().");
static_assert(1_l == Lit(1,pos) and -1_l == Lit(1,neg), "Problem with polarity.");

typedef std::vector<Lit> Lit_vec;


// --- Data structures for clauses ---

/*
  Clauses have
    - static weights of type Weight_t
    - and static indices of type Clause_index.

  Counting clauses happens via unsigned integer type Count_clauses, with
  the global variable r_clauses gives the current number of still active
  clauses.

  Clauses are static except of that their
   - current status (satisfied or still active of length >= 1)
   - and their current length, if active,
  is maintained.

  So the main data structure is "mostly lazy". To see what a clause currently
  really is, one has to consider the static (original) clause plus the current
  partial assignment -- except of that length and satisfaction are handled
  "eagerly", that is, are kept current.

  Class Clause represents a single clause, as a range via
   - begin() and
   - end().
  The other public members are
   - length() of type Clause_index,
   - and the explicit conversion bool() for being active (true) or satisfied,
     i.e., inactive (false).

  For changes there are the private member functions
   - decrement() ("removal" of a falsified literal)
   - increment() (undoing the "removal" of a falsified literal)
   - deactivate() (has been satisfied)
   - activate() (undoing the deactivation).
  Note that they just affect length and status (for the latter two functions).

  Output of clauses via <<.

  Clauses are typically handled via pointers, and ClauseP is the typedef
  for pointers to Clause.

*/

typedef double Weight_t; // weights and their sums
static_assert(is_pod(Weight_t), "Weight_t is not POD.");
typedef std::vector<Weight_t> Weight_vector;

typedef Var Clause_index;
static_assert(std::numeric_limits<Clause_index>::max() <= std::numeric_limits<Weight_vector::size_type>::max(), "Type Clause_index too large for weight vector (conversions cost too much time here).");

typedef std::uint_fast64_t Count_clauses;

// Used for input-reading and initialisation:
typedef std::vector<std::array<Count_clauses,2>> Count_vec;

Count_clauses r_clauses; // "r" = "remaining"

class Clause {
  const Lit* b; // the array of literals in the clause (as in the input)
  const Lit* e; // one past-the-end
  Clause_index length_; // the current length, or 0 iff clause is satisfied
  Clause_index old_length; // if satisfied, the length before satisfaction
  // The following function (for initialisation) sets these data members
  // (while not using the private member functions below):
  friend void add_a_clause_to_formula(const Lit_vec&, Count_vec&);

  // The friends below (for updating the length) only access the following
  // member functions:
  void decrement() noexcept { assert(length_ >= 2); --length_; }
  void increment() noexcept { assert(length_ >= 1); ++length_; }
  void deactivate() noexcept {
    assert(length_ >= 1);
    old_length = length_;
    length_ = 0;
    assert(r_clauses >= 1);
    --r_clauses;
  }
  void activate() noexcept {
    assert(length_ == 0);
    length_ = old_length;
    ++r_clauses;
  }
  friend class ChangeManagement;
  friend void assign_0(Lit) noexcept;
  friend void assign_1(Lit) noexcept;
public :
  const Lit* begin() const noexcept { return b; }
  const Lit* end() const noexcept { return e; }
  Clause_index length() const noexcept { return length_; }
  explicit operator bool() const noexcept { return length_; }
};
static_assert(is_pod(Clause), "Clause is not POD.");

std::ostream& operator <<(std::ostream& out, const Clause& C) {
  for (const Lit x : C) out << x << " ";
  return out << "0\n";
}

typedef Clause* ClauseP;

typedef std::vector<Clause> Clause_vec;
typedef std::vector<ClauseP> ClauseP_vec;


// --- Data structures for literal occurrences ---

class LiteralOccurrences {

  class Literal_occurrences {
    const ClauseP* b; // array with clause-pointers
    const ClauseP* e; // one past-the-end
    friend class LiteralOccurrences;
  public :
    const ClauseP* begin() const noexcept { return b; }
    const ClauseP* end() const noexcept { return e; }
  };
static_assert(is_pod(Literal_occurrences), "Literal_occurrences is not POD.");

  typedef std::array<Literal_occurrences,2> Variable_occurrences;
  typedef std::vector<Variable_occurrences> vec_varocc;
  typedef vec_varocc::size_type size_type;

  vec_varocc varocc;
  ClauseP_vec all_lit_occurrences;

  void init(const size_type s, const Var n, const Clause_vec& clauses, Count_vec& count) {
    if (s == 0) return;
    varocc.resize(n+1);
    all_lit_occurrences.resize(s);
    const ClauseP* pointer = &all_lit_occurrences[0];
    for (Var v = 1; v <= n; ++v)
      for (int p = 0; p <= 1; ++p) {
        auto& L = varocc[v][p];
        L.b = pointer;
        pointer += count[v][p];
        L.e = pointer;
      }
    assert(pointer == &all_lit_occurrences[0] + s);
    {const auto clend = clauses.cend();
     for (auto i = clauses.cbegin(); i != clend; ++i) {
       for (const Lit x : *i) {
         const Var v = var(x); const Polarity p = sign(x);
         *const_cast<ClauseP*>(varocc[v][p].e - count[v][p]--) =
           const_cast<ClauseP>(&*i);
       }
     }
    }
  }
  LiteralOccurrences(const LiteralOccurrences&) = delete;
  LiteralOccurrences(const LiteralOccurrences&&) = delete;
  friend void read_formula(const std::string&);

public :
  LiteralOccurrences() = default;
  const Variable_occurrences& operator[](const Var i) const noexcept {return varocc[i];}
};


// --- Basic global variables ---

class Clauses {
  Clause_vec cl;

  friend void read_formula_header(std::istream&);
  friend void add_a_clause_to_formula(const Lit_vec&, Count_vec&);
  friend void read_formula(const std::string&);

  Clauses(const Clauses&) = delete;
  Clauses(Clauses&&) = delete;
public :
  Clauses() = default;
  // for debugging:
  friend std::ostream& operator <<(std::ostream& out, const Clauses& F) {
    for (const Clause& C : F.cl) out << C;
    return out;
  }
} clauses; /* After construction no direct access anymore to variable "clauses"
  (the clauses are handled via pointers to the elements of cl); "clauses" is
  the only instance of class Clauses. */

LiteralOccurrences lits;
// via lits[v][pos/neg] the sequence of literal-occurrences is obtained

Count_clauses n_header_clauses, n_clauses, n_taut; // r_clauses see above
Count_clauses n_lit_occurrences;
Var n_vars, max_occ_var;
Clause_index max_clause_length;

typedef std::uint_fast64_t Count_statistics;

Count_statistics n_nodes;
Count_statistics n_backtracks;
Count_statistics n_units;

  Count_statistics n_evaluations;
#ifndef FIRST_VARIABLE
  Count_statistics n_proj, n_proj2zero, n_proj1zero,
    n_nofirst, n_withfirst, n_nosecond, n_withsecond;

Weight_t sum_first, sum_second, sumsq_first, sumsq_second,
  min_first = std::numeric_limits<Weight_t>::infinity(),
  min_second = min_first,
  max_first = -std::numeric_limits<Weight_t>::infinity(),
  max_second = max_first;
#endif

#ifdef VAR_MARGINALS
# define ALL_SOLUTIONS
#endif
#ifdef ALL_SOLUTIONS
# ifdef PURE_LITERALS
#  error "ALL_SOLUTIONS not compatible with PURE_LITERALS."
# endif
#endif

#if defined PURE_LITERALS
Count_statistics n_pure_literals;
#endif

#ifdef TAU_ITERATION
# ifdef ALPHA
#  error "ALPHA not compatible with TAU_ITERATION."
# endif
#endif

#ifdef TAU_ITERATION
Count_statistics wtau_calls, tau_iterations;
#endif

#ifdef ALL_SOLUTIONS
# ifndef COUNT_T
#  define COUNT_T std::uint_fast64_t
# endif
typedef COUNT_T Count_solutions;
constexpr bool floating_count = std::is_floating_point<Count_solutions>::value;
static_assert(floating_count or std::is_unsigned<Count_solutions>::value, "If using an integral type for counting, it must be unsigned.");
constexpr Var count_bits = std::numeric_limits<Count_solutions>::digits;
constexpr int count_digits = std::numeric_limits<Count_solutions>::digits10;
template <typename CT, bool IS_FLOATING_TYPE> struct Pow2;
template <typename CT> struct Pow2<CT,true> {
  constexpr CT operator()(const Var n) noexcept { return std::pow(2,n); }
};
template <typename CT> struct Pow2<CT,false> {
  constexpr CT operator()(const Var n) noexcept {
    return n<count_bits ? CT(1) << n : 0;
  }
};
Pow2<Count_solutions,floating_count> pow2;
Count_solutions n_solutions;
#endif
#ifdef VAR_MARGINALS
typedef std::vector<Count_solutions> SolCount_vec;
#endif


// --- Input ---

// Only used in read_formula_header and read_a_clause_from_file:
typedef std::int_fast64_t Rounds;
static_assert(std::numeric_limits<Rounds>::digits <= std::numeric_limits<Count_clauses>::digits, "Problem with types Rounds versus Count_clauses.");

// Sets n_vars and n_header_clauses, and calls
// clauses.cl.resize(n_header_clauses); aborts via std::exit in case of
// input-errors:
void read_formula_header(std::istream& f) {
  std::string line;
  assert(f.good());
  while (true) { // skipping comments
    std::getline(f, line);
    if (f.bad()) {
      errout << "System failure causing reading error.";
      std::exit(file_reading_error);
    }
    if (f.eof()) {
      if (not f.fail()) {
        const auto c = line[0];
        assert(c != '\0');
        if (c != 'p')
          errout << "End of file encountered while attempting to read comment.";
        else
          errout << "End of file encountered while attempting to read p-line.";
        std::exit(file_reading_error);
      }
      else {
        errout << "Empty line at end of file (no p-line found).";
        std::exit(file_reading_error);
      }
    }
    assert(f.good());
    const auto c = line[0];
    if (c == '\0') { // empty line
      errout << "Empty line (no p-line found).";
      std::exit(file_reading_error);
    }
    if (c == 'p') break;
    if (c != 'c') {
      errout << "Comment lines must start with \"c\".";
      std::exit(file_reading_error);
    }
  }
  assert(line[0] == 'p');
  std::stringstream s(line);
  {std::string inp; s >> inp;
   if (inp != "p") {
     errout << "Syntax error in parameter line (\"p\" not followed by space).";
     std::exit(file_pline_error);
   }
   assert(s);
   if (s.eof()) {
     errout << "Syntax error in parameter line (p-line ends after \"p\").";
     std::exit(file_pline_error);
   }
   s >> inp;
   if (not s or inp != "cnf") {
     errout << "Syntax error in parameter line (no \"cnf\").";
     std::exit(file_pline_error);
   }
  }
  if (s.eof()) {
    errout << "Syntax error in parameter line (nothing after \"cnf\").";
    std::exit(file_pline_error);
  }
  s >> n_vars;
  if (not s) {
    errout << "Reading error with parameter maximal-variable-index "
      "(too big or not-a-number).";
    std::exit(file_pline_error);
  }
  if (not valid(n_vars)) {
    errout << "Parameter maximal-variable-index n=" << n_vars <<
      " is too big for numeric_limits<Lit_int>::max=" << max_lit << ".";
    std::exit(num_vars_error);
  }
  if (s.eof()) {
    errout << "Syntax error in parameter line (nothing after n-parameter).";
    std::exit(file_pline_error);
  }
  s >> n_header_clauses;
  if (not s) {
    errout << "Reading error with parameter number-of-clauses "
      "(too big or not-a-number).";
    std::exit(file_pline_error);
  }
  if (not s.eof()) {
    errout << "Syntax error in parameter line (something after c-parameter).";
    std::exit(file_pline_error);
  }
  if (n_header_clauses > Count_clauses(std::numeric_limits<Rounds>::max())) {
    errout << "Parameter number-of-clauses too big for round-counter.";
    std::exit(file_pline_error);
  }
  try { clauses.cl.resize(n_header_clauses); }
  catch (const std::bad_alloc&) {
    errout << "Allocation error for clauses-vector of size " <<
      n_header_clauses << " (the number-of-clauses).";
    std::exit(allocation_error);
  }
}

// Returns false iff no (further) clause was found;
// reference-parameter C is empty iff a tautological clause was found:
inline bool read_a_clause_from_file(std::istream& f, Lit_vec& C) {
  {static std::vector<Rounds> literal_table(n_vars+1,0);
   static Rounds round = 0;
   Lit x;
   assert(f.good());
   f >> x;
   if (f.eof()) {
     literal_table.clear(); literal_table.shrink_to_fit();
     return false;
   }
   C.clear();
   assert(round != std::numeric_limits<Rounds>::max());
   ++round;
   while (true) { // reading literals into C
     if (not f) {
       errout << "Invalid literal-read.";
       std::exit(literal_read_error);
     }
     assert(f.good());
     if (not x) break; // end of clause
     const Var v = var(x);
     if (v > n_vars) {
       errout << "Literal " << x << " contradicts n=" << n_vars << ".";
       std::exit(variable_value_error);
     }
     const auto t = literal_table[v];
     const auto comp = (sign(x) == pos) ? round : -round;
     if (t == -comp) { // tautology
       C.clear();
       do
         if (not (f >> x)) {
           errout << "Invalid literal-read in tautological clause.";
           std::exit(literal_read_error);
         }
       while (x);
       return true;
     }
     else if (t != comp) {
       C.push_back(x);
       literal_table[v] = comp;
     }
     f >> x;
   }
  }
  if (C.empty()) {
    errout << "Found empty clause in input.";
    std::exit(empty_clause_error);
  }
  if (C.size() == 1) {
    errout << "Found unit-clause in input.";
    std::exit(unit_clause_error);
  }
  return true;
}

// Error only if announced number of clauses too small (but may be too big):
inline void add_a_clause_to_formula(const Lit_vec& D, Count_vec& count) {
  const auto n = D.size();
  if (n == 0) { // means tautology here
    ++n_taut;
    return;
  }
  if (n_clauses >= n_header_clauses) {
    errout << "More than " << n_header_clauses << " clauses, contradicting cnf-header.";
    std::exit(number_clauses_error);
  }
  auto& C = clauses.cl[n_clauses];
  C.length_ = n;
  C.b = new Lit[n];
  C.e = C.b + n;
  if (n > max_clause_length) max_clause_length = n;
  for (Clause_index i = 0; i < n; ++i) {
    const Lit x = D[i];
    const_cast<Lit*>(C.b)[i] = x;
    const Var v = var(x);
    ++count[v][sign(x)];
    max_occ_var = std::max(v, max_occ_var);
  }
  ++n_clauses;
  n_lit_occurrences += n;
}

void read_formula(const std::string& filename) {
  class Input {
    const std::string& f;
    std::istream* const p;
    const bool del;
    Input(const Input&) = delete;
    Input(Input&&) = delete;
  public :
    Input(const std::string& f) : f(f), p(f == "-cin" ? &std::cin : new std::ifstream(f)), del(f != "-cin") {
      if (not *p) {
        errout << "Invalid input filename:" << f;
        std::exit(file_reading_error);
      }
    }
    ~Input() { if (del) delete p; }
    std::istream& operator *() const { return *p; }
  };
  const Input in(filename);
  read_formula_header(*in);
  Count_vec count;
  try { count.resize(n_vars+1); }
  catch (const std::bad_alloc&) {
    errout << "Allocation error for counting-vector of size " <<
      n_vars << " (the maximal-variable-index).";
    std::exit(allocation_error);
  }
  {Lit_vec C;
   while (read_a_clause_from_file(*in,C)) add_a_clause_to_formula(C,count);
  }
  if (not (r_clauses = n_clauses)) return;
  try { lits.init(n_lit_occurrences, max_occ_var, clauses.cl, count); }
  catch (const std::bad_alloc&) {
    errout << "Allocation error for ClauseP-vector of size " <<
       n_lit_occurrences << " (the number of literal occurrences).";
    std::exit(allocation_error);
  }
}


// --- Data structures for partial assignments and unit-clause propagation ---

/* The current assignment: pass[v] is 0 iff variable
   v is unassigned, otherwise it is v in case v->true and else -v.
*/
class Pass {
  Lit_vec pass;
  void init() { pass.resize(max_occ_var+1); }
  friend void initialisation();
public :
  Lit operator[] (const Var v) const noexcept { return pass[v]; }
  Lit& operator[] (const Var v) noexcept { return pass[v]; }
  Lit_vec::size_type size() const noexcept { return pass.size(); }
  Var n() const noexcept {
    Var res = 0;
    assert(pass[0] == 0_l);
    for (auto x : pass) res += bool(x);
    return res;
  }
  friend std::ostream& operator <<(std::ostream& out, const Pass& p) {
    out << "v ";
    for (Var i=1; i < p.size(); ++i) if (p[i]) out << p[i] << " ";
    return out << "0" << std::endl;
  }
};
Pass pass;
#ifdef VAR_MARGINALS
class VarMarginals {
  SolCount_vec sc;
  void init() { sc.resize(n_vars+1); }
  friend void initialisation();
  friend DLL_return_t dll0();
public :
  Count_solutions operator[] (const Var v) const noexcept { return sc[v]; }
  Count_solutions& operator[] (const Var v) noexcept { return sc[v]; }
  SolCount_vec::size_type size() const noexcept { return sc.size(); }
  friend std::ostream& operator <<(std::ostream& out, const VarMarginals& M) {
    for (Var i=1; i < M.size(); ++i)
      out << " " << i << ":" << M[i];
    return out << std::endl;
  }
};
VarMarginals marginals;
#endif

class ChangeManagement {
  ClauseP_vec changes;
  const ClauseP* begin;
  ClauseP* next;
  void init() {
    const auto s = n_lit_occurrences + 3 * max_occ_var;
    assert(s >= 1);
    changes.resize(s);
    begin = next = &*changes.begin();
  }
  friend void initialisation();
public :
  void start_new() noexcept { *(next++) = nullptr; }
  void push(const ClauseP C) noexcept { *(next++) = C; }
  void reactivate_0() noexcept {
    while (const ClauseP C = *(--next)) C->increment();
  }
  void reactivate_1() noexcept {
    while (const ClauseP C = *(--next)) C->activate();
  }
};
ChangeManagement changes;

/* Buffers for unit-clauses (to be processed), either
    - first-in first-out (BFS), or
    - last-in first-out (DFS).
*/

#ifndef UCP_STRATEGY
# define UCP_STRATEGY 1
#endif
#if UCP_STRATEGY == 0 // BFS
class Unit_stack {
  typedef Lit_vec stack_t;
  static stack_t stack;
  static Lit* end_;
  static Lit* open_;
  const Lit* const begin_;
  static void init() {
    stack.resize(max_occ_var);
    assert(max_occ_var);
    end_ = &stack[0];
  }
  friend void initialisation();
public :
  static void push(const Lit x) noexcept {
    assert(end_ - &stack[0] < max_occ_var);
    *(end_++) = x;
  }
  static Lit pop() noexcept {
    assert(open_ != end_);
    return *(open_++);
  }
  Unit_stack() noexcept : begin_(end_) { open_ = end_; }
  ~Unit_stack() noexcept {
    const auto end = end_;
    for (auto p = begin_; p != end; ++p) pass[var(*p)] = 0_l;
    end_ = const_cast<Lit*>(begin_);
  }
  explicit operator bool() const noexcept { return end_ != open_; }
  const Lit* begin() const noexcept { return begin_; }
  static const Lit* end() noexcept { return end_; }
};
Unit_stack::stack_t Unit_stack::stack;
Lit* Unit_stack::end_;
Lit* Unit_stack::open_;

#else // DFS

class Unit_stack {
  typedef Lit_vec stack_t;
  static stack_t main_stack;
  static stack_t input_stack;
  static Lit* begin_input;
  static Lit* end_input;
  static Lit* end_main;
  const Lit* const begin_main;
  static Lit push_main(const Lit x) noexcept {
    assert(end_main - &main_stack[0] < max_occ_var);
    return *(end_main++) = x;
  }
  static Lit pop_input() noexcept {
    assert(end_input != begin_input);
    return *(--end_input);
  }
  static void init() {
    main_stack.resize(max_occ_var);
    input_stack.resize(max_occ_var);
    assert(max_occ_var);
    end_main = &main_stack[0];
    begin_input = end_input = &input_stack[0];
  }
  friend void initialisation();
public :
  static void push(const Lit x) noexcept {
    assert(end_input - begin_input < max_occ_var);
    *(end_input++) = x;
  }
  static Lit pop() noexcept { return push_main(pop_input()); }
  Unit_stack() noexcept : begin_main(end_main) { end_input = begin_input; }
  ~Unit_stack() {
    const auto mend = end_main;
    for (auto p = begin_main; p != mend; ++p) pass[var(*p)] = 0_l;
    const auto iend = end_input;
    for (auto p = begin_input; p != iend; ++p) pass[var(*p)] = 0_l;
    end_main = const_cast<Lit*>(begin_main);
  }
  explicit operator bool() const noexcept { return end_input != begin_input; }
  const Lit* begin() const noexcept { return begin_main; }
  static const Lit* end() noexcept { return end_main; }
};
Unit_stack::stack_t Unit_stack::main_stack;
Unit_stack::stack_t Unit_stack::input_stack;
Lit* Unit_stack::begin_input;
Lit* Unit_stack::end_input;
Lit* Unit_stack::end_main;
#endif

class Push_unit_clause {
  bool contradiction_ = false;
public :
  // returns false iff contradiction found:
  bool operator() (const Clause& C) noexcept {
    assert(C.length() == 1);
    assert(not contradiction_);
    for (const Lit y : C) {
      Lit& val = pass[var(y)];
      if (not val) {
        Unit_stack::push(y);
        val = y;
        return true;
      }
      else if (val == y) return true;
    }
    contradiction_ = true;
    return false;
  }
  bool contradiction() noexcept {
    if (contradiction_) {contradiction_ = false; return true;}
    else return false;
  }
};
Push_unit_clause push_unit_clause;

#ifdef PURE_LITERALS
void assign_1(Lit) noexcept;
class PureLiterals {
  typedef Lit_vec stack_t;
  static stack_t stack;
  static const Lit* new_begin;
  static Lit* end_;
  const Lit* const begin_;
  const Lit* begin() const noexcept { return begin_; }
  static const Lit* end() noexcept { return end_; }
  static void push(const Lit x) noexcept {
    assert(end_ - &stack[0] < max_occ_var);
    *(end_++) = x;
  }
  static void init() {
    stack.resize(max_occ_var);
    assert(max_occ_var);
    end_ = &stack[0];
  }
  friend void initialisation();
public :
  static void clear() noexcept { new_begin = end_; }
  static bool set(const Var v, const Polarity s) noexcept {
    const Lit pl{v,s};
    pass[v] = pl;
    push(pl);
    assign_1(pl);
    ++n_pure_literals;
    return r_clauses == 0;
  }
  PureLiterals() noexcept : begin_(new_begin) {}
  ~PureLiterals() {
    for (const Lit x : *this) pass[var(x)] = 0_l;
    end_ = const_cast<Lit*>(begin_);
  }
};
PureLiterals::stack_t PureLiterals::stack;
const Lit* PureLiterals::new_begin;
Lit* PureLiterals::end_;
#endif


// --- Weight handling ---

#ifdef TAU_ITERATION
# ifdef TWEIGHT_2
  constexpr Weight_t weight_2 = TWEIGHT_2;
# else
  constexpr Weight_t weight_2 = 7.0;
# endif
# ifdef TWEIGHT_4
  constexpr Weight_t weight_4 = TWEIGHT_4;
# else
  constexpr Weight_t weight_4 = 0.31;
# endif
# ifdef TWEIGHT_5
  constexpr Weight_t weight_5 = TWEIGHT_5;
# else
  constexpr Weight_t weight_5 = 0.19;
# endif
# ifdef TWEIGHT_6
  constexpr Weight_t weight_6 = TWEIGHT_6;
# else
  constexpr Weight_t weight_6 = 0.19/1.7;
# endif
# ifdef TWEIGHT_BASIS_OPEN
  constexpr Weight_t basis_open = TWEIGHT_BASIS_OPEN;
# else
  constexpr Weight_t basis_open = 1.70;
# endif

#else

# ifdef WEIGHT_2
  constexpr Weight_t weight_2 = WEIGHT_2;
# else
  constexpr Weight_t weight_2 = 4.85;
# endif
# ifdef WEIGHT_4
  constexpr Weight_t weight_4 = WEIGHT_4;
# else
  constexpr Weight_t weight_4 = 0.354;
# endif
# ifdef WEIGHT_5
  constexpr Weight_t weight_5 = WEIGHT_5;
# else
  constexpr Weight_t weight_5 = 0.11;
# endif
# ifdef WEIGHT_6
  constexpr Weight_t weight_6 = WEIGHT_6;
# else
  constexpr Weight_t weight_6 = 0.0694;
# endif
# ifdef WEIGHT_BASIS_OPEN
  constexpr Weight_t basis_open = WEIGHT_BASIS_OPEN;
# else
  constexpr Weight_t basis_open = 1.46;
# endif

#endif

/* The current parameter values have been obtained mainly via optimisation on
   VanDerWaerden_2-3-12_135.cnf (van der Waerden problem with two colours,
   first colour arithmetic progressions of size 3, second colour
   arithmetic progressions of size 12, with n = 135, the first n such that
   every partition of {1,...,n} must contain an arithmetic progression of
   size 3 in the first part or of size 12 in the second part).
*/

constexpr Clause_index first_open_weight = 7;
static_assert(first_open_weight >= 4, "Wrong value of first_open_weight.");
constexpr std::array<Weight_t, first_open_weight> predetermined_weights {{0,0, weight_2, 1, weight_4, weight_5, weight_6}};
static_assert(predetermined_weights[0] == 0, "weights[0] != 0.");
static_assert(predetermined_weights[2] != 0, "Zero weight.");
static_assert(predetermined_weights[3] == 1, "weights[3] != 1.");
static_assert(predetermined_weights[first_open_weight-1] != 0, "Zero weight.");
/* Remarks:
    - predetermined_weights[1] is arbitrary (since not used).
    - If special weights for clause-lengths k = 4,5,... are to be used, then
      these weights are written into predetermined_weights, and
      first_open_weight is to be adapted accordingly.
*/
void output_weights(Output& out) {
  const auto pr = out.precision(std::numeric_limits<Weight_t>::digits10 + 2);
  out << predetermined_weights[2];
  for (Clause_index i = 3; i < first_open_weight; ++i)
    out << "," << predetermined_weights[i];
  out << ";" << basis_open;
  out.precision(pr);
}

#ifdef TAU_ITERATION
static_assert(std::numeric_limits<Weight_t>::has_infinity, "Tau-computation needs +inf.");
constexpr Weight_t inf_weight = std::numeric_limits<Weight_t>::infinity();
#endif

// weight[k] is the weight for clause-length k >= 2:
class Weights {
  constexpr static Weight_t min_weight = std::numeric_limits<Weight_t>::min();
  static_assert(min_weight != 0, "Error with min_weight.");
  constexpr static Weight_t set_min(const Weight_t w) noexcept {
    return (w == 0) ? min_weight : w;
  }
  // the weights for clause of length >= first_open_weight:
  constexpr static Weight_t wopen(const Clause_index clause_length) noexcept {
    return set_min(predetermined_weights[first_open_weight-1] *
      std::pow(basis_open,-Weight_t(clause_length)+first_open_weight-1));
  }

  Weight_vector weights;
#ifdef LAMBDA
  Weight_vector sweights;
#endif
  void init() noexcept {
    assert(weights.size() == first_open_weight);
    try { weights.resize(max_clause_length+1); }
    catch (const std::bad_alloc&) {
      errout << "Weights-allocation error for double-vector of size " <<
         max_clause_length << "+1 (the maximal clause-length).";
      std::exit(allocation_error);
    }
    for (Clause_index i = first_open_weight; i <= max_clause_length; ++i)
      weights[i] = wopen(i);
#ifdef LAMBDA
    static_assert(LAMBDA > 0);
    try { sweights.resize(max_clause_length+1); }
    catch (const std::bad_alloc&) {
      errout << "Sweights-allocation error for double-vector of size " <<
         max_clause_length << "+1 (the maximal clause-length).";
      std::exit(allocation_error);
    }
    for (Clause_index i = 2; i <= max_clause_length; ++i)
      sweights[i] = LAMBDA * - std::log1p(- std::pow(2, - Lit_int(i)));
#endif
  }
  friend void initialisation();
public :
  Weights() noexcept : weights(predetermined_weights.begin(), predetermined_weights.end()) {}
  Weight_t operator[] (const Clause_index i) const noexcept {
    return weights[i];
  }
#ifdef LAMBDA
  Weight_t s(const Clause_index i) const noexcept {
    return sweights[i];
  }
#endif
};
constexpr Weight_t Weights::min_weight;
Weights weight;


// --- Initialisation of algorithmic data structures ---

void initialisation() {
  pass.init();
#ifdef VAR_MARGINALS
  marginals.init();
#endif
  changes.init();
  Unit_stack::init();
  weight.init();
#ifdef PURE_LITERALS
  PureLiterals::init();
#endif
}


// --- Assignments to variables ---

inline void assign_0(const Lit x) noexcept {
  assert(x);
  const Var v = var(x);
  assert(v <= max_occ_var);
  assert(not(pass[v] == -x));
  pass[v] = x;
  for (auto C : lits[v][-sign(x)]) {
    if (not *C) continue;
    changes.push(C);
    C->decrement();
    if (C->length() == 1 and not push_unit_clause(*C)) return;
  }
}

inline void assign_1(const Lit x) noexcept {
  assert(x);
  const Var v = var(x);
  assert(v <= max_occ_var);
  assert(pass[v] == x);
  for (auto C : lits[v][sign(x)]) {
    if (not *C) continue;
    C->deactivate();
    changes.push(C);
  }
}


// --- Branching heuristics ---

inline Lit first_branch([[maybe_unused]]const Weight_t pd, [[maybe_unused]]const Weight_t nd, const Var v) noexcept {
#ifdef ALL_SOLUTIONS
  return Lit(v);
#else
  return (pd>=nd) ? Lit(v) : -Lit(v);
#endif
}
#ifdef TAU_ITERATION
class Branching_tau {
  Lit x;
  Weight_t min1, max2;

  static constexpr Weight_t tau_meaneqLW = 2.8811206627473383049862597L;
  inline static constexpr double wtau_elem_lb(const Weight_t ra) noexcept {
    return std::log(4) / (1+ra);
  }
  inline static constexpr Weight_t lambertW0l_lb(const Weight_t l) noexcept {
    assert(l > 0);
    const Weight_t ll = std::log(l);
    return std::fma(-ll, l/(l+1), l);
  }
  inline static constexpr Weight_t lambertW0_lb(const Weight_t x) noexcept {
    assert(x > 1);
    return lambertW0l_lb(std::log(x));
  }
  inline static double wtau(const Weight_t a) noexcept {
    assert(a > 1);
    ++wtau_calls;
    if (std::isinf(a)) return inf_weight;
    const Weight_t ra = 1 /a;
    Weight_t x0 =
      a <= tau_meaneqLW ? wtau_elem_lb(ra) : lambertW0_lb(a);
    while (true) {
      const Weight_t A = std::exp(-x0), B = std::expm1(-ra * x0), N = A+B;
      if (N <= 0) return x0;
      const Weight_t D = 1 / (std::fma(ra, B, ra) + A);
      assert(D > 0);
      const Weight_t x1 = std::fma(N, D, x0);
      assert(x1 >= x0);
      if (x1 == x0) return x0;
      x0 = x1;
      ++tau_iterations;
    }
  }
  inline static Weight_t ltau(Weight_t a, Weight_t b) noexcept {
    assert(a > 0 and b > 0);
    if (a > b) std::swap(a, b);
    if (b == inf_weight) return 0;
    if (a == b) return 0.693147180559945309417232121458L / a;
    return wtau(b / a) / b;
  }

public :
  Branching_tau() noexcept : x{}, min1(inf_weight), max2(0) {}
  operator Lit() const noexcept { return x; }
  void operator()(const Weight_t pd, const Weight_t nd, const Var v) noexcept {
    ++n_proj;
#ifndef PURE_LITERALS
# ifndef LAMBDA
    if (pd == 0 or nd == 0) {
      if (pd == 0 and nd == 0) { ++n_proj2zero; return; }
      ++n_proj1zero;
      if (min1 < inf_weight) return;
      const Weight_t sum = pd + nd;
      if (sum <= max2) return;
      max2=sum;
      x = first_branch(pd,nd,v);
      return;
    }
# else
    if (pd == 0 and nd == 0) { ++n_proj2zero; return; }
# endif
#endif
    assert(pd > 0); assert(nd > 0);
    const Weight_t chi = std::exp(min1 * -pd) + std::exp(min1 * -nd);
    if (chi>1) return;
    const Weight_t sum = pd + nd;
    if (chi==0) { if ((min1=ltau(pd,nd))==inf_weight and sum<=max2) return; }
    else if (chi==1) { if (sum<=max2) return; }
    else min1=ltau(pd,nd);
    max2=sum;
    x = first_branch(pd,nd,v);
  }
  bool no_first() const noexcept { return min1 == inf_weight; }
  bool no_second() const noexcept { return max2 == 0; }
  Weight_t first() const noexcept { return min1; }
  Weight_t second() const noexcept { return max2; }
};
typedef Branching_tau Best_branching;
#elif ! defined FIRST_VARIABLE
# ifdef ALPHA
Weight_t mean(const Weight_t pd, const Weight_t nd) noexcept {
  if constexpr (ALPHA == 0.5) return std::sqrt(pd) + std::sqrt(nd);
  else return std::pow(pd,ALPHA) + std::pow(nd,ALPHA);
}
# endif
class Branching_product {
  Lit x;
  Weight_t max1, max2;
public :
  Branching_product() noexcept : x{}, max1(0), max2(0) {}
  operator Lit() const noexcept { return x; }
  void operator()(const Weight_t pd, const Weight_t nd, const Var v) noexcept {
    ++n_proj;
# ifndef PURE_LITERALS
#  ifndef LAMBDA
    if (pd == 0 or nd == 0) {
      if (pd == 0 and nd == 0) { ++n_proj2zero; return; }
      ++n_proj1zero;
      if (0 != max1) return;
      const Weight_t sum = pd + nd;
      if (sum <= max2) return;
      max2=sum;
      x = first_branch(pd,nd,v);
      return;
    }
#  else
    if (pd == 0 and nd == 0) { ++n_proj2zero; return; }
#  endif
# endif
    assert(pd > 0); assert(nd > 0);
# ifndef ALPHA
    const Weight_t prod = pd * nd;
# else
    const Weight_t prod = mean(pd, nd);
# endif
    if (prod < max1) return;
    const Weight_t sum = pd + nd;
    if (prod > max1) max1=prod;
    else if (sum <= max2) return;
    max2 = sum;
    x = first_branch(pd,nd,v);
  }
  bool no_first() const noexcept { return max1 == 0; }
  bool no_second() const noexcept { return max2 == 0; }
  Weight_t first() const noexcept { return max1; }
  Weight_t second() const noexcept { return max2; }
};
typedef Branching_product Best_branching;
#endif


#ifndef FIRST_VARIABLE
inline Lit branching_literal() noexcept {
  Best_branching br;
# ifdef PURE_LITERALS
  PureLiterals::clear(); changes.start_new();
# endif
  const auto nvar = max_occ_var+1;
  for (Var v = 1; v != nvar; ++v) {
    if (pass[v]) continue;
    const auto Occ = lits[v];
# ifndef LAMBDA
    Weight_t ps = 0;
    for (const auto C : Occ[pos]) ps += weight[C->length()];
# else
    Weight_t ps = 0, sns = 0;
    for (const auto C : Occ[pos]) {
      const auto l = C->length();
      ps += weight[l]; sns += weight.s(l);
    }
# endif
# ifdef PURE_LITERALS
    if (ps == 0) {if (PureLiterals::set(v,neg)) return 0_l; else continue;}
# endif
# ifndef LAMBDA
    Weight_t ns = 0;
    for (const auto C : Occ[neg]) ns += weight[C->length()];
# else
    Weight_t ns = 0, sps = 0;
    for (const auto C : Occ[neg]) {
      const auto l = C->length();
      ns += weight[l]; sps += weight.s(l);
    }
# endif
# ifdef PURE_LITERALS
    if (ns == 0) {if (PureLiterals::set(v,pos)) return 0_l; else continue;}
# endif
# ifndef LAMBDA
    br(ps , ns, v);
# else
    br(ps + sps , ns + sns, v);
# endif
  }

  ++n_evaluations;
  if (not br.no_first()) {
    ++n_withfirst;
    const auto first = br.first();
    sum_first += first; sumsq_first += first * first;
    min_first = std::min(min_first, first);
    max_first = std::max(max_first, first);
  }
  else ++n_nofirst;
  if (not br.no_second()) {
    ++n_withsecond;
    const auto second = br.second();
    sum_second += second; sumsq_second += second * second;
    min_second = std::min(min_second, second);
    max_second = std::max(max_second, second);
  }
  else ++n_nosecond;
  return br;
}
#else
inline Lit branching_literal() noexcept {
  ++n_evaluations;
  PureLiterals::clear(); changes.start_new();
  const auto nvar = max_occ_var+1;
  for (Var v = 1; v != nvar; ++v) {
    if (pass[v]) continue;
    const auto Occ = lits[v];
    bool occurs = false;
    for (const auto C : Occ[pos]) if (C->length()) {occurs = true; break;}
    if (not occurs)
      if (PureLiterals::set(v,neg)) return 0_l; else continue;
    for (const auto C : Occ[neg]) if (C->length()) {occurs = true; break;}
    if (occurs) return Lit(v);
    else if (PureLiterals::set(v,pos)) return 0_l; else continue;
  }
  return 0_l;
}
#endif



// --- The main (backtracking) algorithm ---

Pass sat_pass;

DLL_return_t dll(const Lit x) {
  ++n_nodes;
  assert(x);
  DLL_return_t result = false;

  changes.start_new();
  const Unit_stack unit_stack;
  Unit_stack::push(x);
  assign_0(Unit_stack::pop());
  while (unit_stack) { // unit-clause propagation
    if (push_unit_clause.contradiction()) goto only_neg_units;
    ++n_units;
    assign_0(Unit_stack::pop());
  }

  changes.start_new();
  for (const Lit y : unit_stack) assign_1(y);
  if (not r_clauses) {
#ifdef ALL_SOLUTIONS
    solout << pass;
    result = true;
# ifndef VAR_MARGINALS
    n_solutions += pow2(n_vars - pass.n());
# else
    const Var rem_n = n_vars - pass.n();
    assert(marginals.size() == n_vars+1);
    if (rem_n == 0) {
      ++n_solutions;
      for (Var i=1; i <= n_vars; ++i)
        marginals[i] += sign(pass[i]) == Polarity::pos;
    } else {
      const auto total = pow2(rem_n);
      n_solutions += total;
      const auto totald2 = total / 2;
      for (Var i=1; i <= n_vars; ++i) {
        const Lit x = pass[i];
        Count_solutions& m = marginals[i];
        if (not x) m += totald2;
        else if (sign(x) == Polarity::pos) m += total;
      }
    }
# endif
    goto only_units;
#else
    sat_pass = pass;
    return true;
#endif
  }

  {const Lit y = branching_literal();
#ifdef PURE_LITERALS
   const PureLiterals pure_stack;
   if (not r_clauses) {sat_pass = pass; return true;}
#endif
#ifdef ALL_SOLUTIONS
   result = dll(y);
   ++n_backtracks;
   result = dll(-y) or result;
#else
   if (dll(y)) return true;
   ++n_backtracks;
   if (dll(-y)) return true;
#endif
#ifdef PURE_LITERALS
   changes.reactivate_1();
#endif
  }

#ifdef ALL_SOLUTIONS
only_units :
#endif
  changes.reactivate_1();
only_neg_units :
  changes.reactivate_0();
  return result;
}

DLL_return_t dll0() { // without unit-clauses
  ++n_nodes;
#ifdef ALL_SOLUTIONS
  if (not n_clauses) {
# ifndef VAR_MARGINALS
    n_solutions = pow2(n_vars); return true;
# else
    if (n_vars != 0) {
       marginals.init();
       const auto totald2 = pow2(n_vars-1);
       for (Var i=1; i <= n_vars; ++i) marginals[i] = totald2;
       n_solutions = 2*totald2;
    }
    else n_solutions = 1;
    return true;
#endif
  }
#else
  if (not n_clauses) return true;
#endif
  const Lit x = branching_literal();
#ifdef PURE_LITERALS
  if (not r_clauses) {sat_pass = pass; return true;}
#endif
  const DLL_return_t res1 = dll(x);
#ifndef ALL_SOLUTIONS
  if (res1) return true;
#endif
  ++n_backtracks;
  const DLL_return_t res2 = dll(-x);
  return res1 or res2;
#ifdef PURE_LITERALS
  // pure_stack and changes.reactivate_1() superfluous here, since no backtrack
#endif
}


// --- Output ---

void show_usage() {
  std::cout << "USAGE:\n"
    "> " << program << " (-v | --version)\n"
    " shows version informations and exits.\n"
    "> " << program << " (-cin | filename)\n"
    " runs the solver with input from standard input or filename.\n"
    "> " << program << " (-cin | filename) (-cout | -cerr | filename2 | -nil)\n"
      " furthermore appends satisfying assignments to standard output or standard error or filename2, or ignores them\n "
#ifndef ALL_SOLUTIONS
      "(default is -cout).\n"
#else
      "(default is -nil).\n"
#endif
    "The same redirection can be done with the statistics output (as a third command-argument; default is -cout).\n"
    "For example, with\n"
    "> " << program << " -cin Out -nil\n"
    "input comes from standard input, a satisfying assignment is put to file Out, and the statistics are discarded.\n"
    "While with\n"
    "> " << program << " In Out Out\n"
    "the input comes from file In, and both statistics and assignments are appended to Out "
#ifndef ALL_SOLUTIONS
      "(first the statistics).\n";
#else
      "(first the solutions).\n";
#endif
  std::exit(0);
}

#define S(x) #x
#define STR(x) S(x)

const std::string options = ""
#if UCP_STRATEGY == 0
"B"
#endif
#ifdef PURE_LITERALS
"P"
#endif
#ifdef TAU_ITERATION
"T"
#endif
#ifdef FIRST_VARIABLE
"fv"
#endif
#ifdef ALL_SOLUTIONS
# ifdef VAR_MARGINALS
"M"
# else
"A"
# endif
    + std::string(floating_count ? "F" : "") + std::to_string(count_digits)
#endif
#ifdef LAMBDA
    + std::string("L" STR(LAMBDA))
#endif
#ifdef ALPHA
    + std::string("AL" STR(ALPHA))
#endif
;

void version_information() {
  std::cout << "Program name = " << program << ":\n"
   " authors: Tanbir Ahmed and Oliver Kullmann\n"
   " url's:\n"
   "  http://sourceforge.net/projects/tawsolver/\n"
   "  https://github.com/OKullmann/oklibrary/blob/master/Satisfiability/Solvers/TawSolver/tawSolver.cpp\n"
   " Version: " << version << "\n"
   " Last change date: " << date << "\n"
   " Mapping k -> weight, for clause-lengths k specified at compile-time:\n ";
   std::cout << std::setprecision(4);
   for (Clause_index k = 2; k < first_open_weight; ++k)
     std::cout << "  " << k << "->" << weight[k];
   std::cout << "\n"
   " Divisor for open weights: " << basis_open << "\n"
   " Option summary = \"" << options << "\"\n"
   " Macro settings:\n"
   "  LIT_TYPE = " STR(LIT_TYPE) " (with " << std::numeric_limits<Lit_int>::digits << " binary digits)\n"
   "  UCP_STRATEGY = " << UCP_STRATEGY << "\n"
#ifdef TAU_ITERATION
   "  TAU_ITERATION\n"
#else
   " Compiled without TAU_ITERATION\n"
#endif
#ifdef ALL_SOLUTIONS
   " Compiled with ALL_SOLUTIONS\n"
   "  COUNT_T = " STR(COUNT_T) " (with " << count_bits << " binary digits)\n"
   "   ";
   if (floating_count) std::cout << "a floating-point type";
   else std::cout << "an unsigned integral type";
   std::cout << " with " << count_digits << " decimal digits\n"
#else
   " Compiled without ALL_SOLUTIONS\n"
#endif
#ifdef PURE_LITERALS
   " Compiled with PURE_LITERALS\n"
#else
   " Compiled without PURE_LITERALS\n"
#endif
#ifdef FIRST_VARIABLE
   " Compiled with FIRST_VARIABLE\n"
#endif
#ifdef LAMBDA
   " Compiled with LAMBDA=" STR(LAMBDA) "\n"
#endif
#ifdef ALPHA
   " Compiled with ALPHA=" STR(ALPHA) "\n"
#endif
#ifdef NDEBUG
   " Compiled with NDEBUG defined\n"
#else
   " Compiled with NDEBUG undefined\n"
#endif
#ifdef __OPTIMIZE__
   " Compiled with optimisation on\n"
#else
   " Compiled wit optimisation off\n"
#endif
   " Compilation date: " __DATE__ " " __TIME__ "\n"
#ifdef __GNUC__
   " Compiler version: g++ " __VERSION__ "\n"
#else
   " Compiler not gcc\n"
#endif
#ifdef OKLIB
   " Provided in the OKlibrary " "http://" STR(OKLIB) "\n"
#endif
#ifdef GIT_ID
   " Git ID = " STR(GIT_ID) "\n"
#endif
  ;
  std::exit(0);
}

typedef long double Time_point;
typedef std::array<Time_point, 2> Time_points_2;
constexpr Time_points_2 operator -(const Time_points_2 a, const Time_points_2 b) noexcept {
  return {a[0] - b[0], a[1] - b[1]};
}
#include <sys/time.h>
#include <sys/resource.h>
class UserTime {
  static constexpr Time_point nan =
    std::numeric_limits<Time_point>::quiet_NaN();
  rusage timing;
  rusage* const ptiming;
  Time_point utime() const noexcept {
    return timing.ru_utime.tv_sec + timing.ru_utime.tv_usec / 1'000'000.0;
  }
  Time_point stime() const noexcept {
    return timing.ru_stime.tv_sec + timing.ru_stime.tv_usec / 1'000'000.0;
  }
public :
  UserTime() : ptiming(&timing) {}
  Time_points_2 operator()() noexcept {
    if (getrusage(RUSAGE_SELF, ptiming) != 0) return {nan, nan};
    else return {utime(), stime()};
  }
  long double mem() const noexcept {
    using float80 = long double;
    return float80(timing.ru_maxrss) * 16 / float80(15625);
  }
};
UserTime timing;

Time_points_2 t0; // start of whole computation
Time_points_2 t1; // start of SAT solving

// Wall-clock:
typedef std::chrono::time_point<std::chrono::high_resolution_clock> WTime_point;
WTime_point t0W, t1W;



std::string filename;

void output(const Result_value result) {
  const Time_points_2 elapsed = timing() - t1;
  const WTime_point current = std::chrono::high_resolution_clock::now();
  using diff_t = std::chrono::duration<long double>;

  const unsigned prec_time = 4, prec_time_small = 6,
    prec_heuristics = 7;
  [[maybe_unused]] const unsigned prec_const = 5;
  logout << "s ";
  switch (result) {
    case unknown : logout << "UNKNOWN\n"; break;
    case unsat : logout << "UN"; // FALLTHRU
    case sat : logout << "SATISFIABLE\n";
  }
  const auto sc = std::scientific;
  const auto fi = std::fixed;
  const auto single_child = n_nodes - 2 * n_backtracks - 1;
  const auto inodes = n_nodes == 0 ? 1 : n_backtracks + single_child;
  const auto leaves = n_nodes - inodes;

#ifndef FIRST_VARIABLE
  const auto m_first = sum_first / n_withfirst;
  const auto m_second = sum_second / n_withsecond;
  const auto sd = [](const Count_statistics n, const Weight_t s, const Weight_t sq) {
    return std::sqrt((sq - s*s / n) / (n-1)); };
  const Weight_t sd_first = sd(n_withfirst, sum_first, sumsq_first);
  const Weight_t sd_second = sd(n_withsecond, sum_second, sumsq_second);
#endif

if (logout.active()) {
  logout <<
         "c program_name                          " << program << "\n"
         "c   version_number                      " << version << "\n"
         "c   options                             \"" << options << "\"\n"
         "c   weights                             ";
  output_weights(logout);
  logout << "\n"
         "c file_name                             " << filename << "\n"
         "c   p_param_variables                   " << n_vars << "\n"
         "c   p_param_clauses                     " << n_header_clauses << "\n"
         "c   number_tautologies                  " << n_taut << "\n"
         "c   max_occurring_variable              " << max_occ_var << "\n"
         "c   number_of_clauses                   " << n_clauses << "\n"
         "c   maximal_clause_length               " << max_clause_length << "\n"
         "c   number_of_literal_occurrences       " << n_lit_occurrences << "\n"
         << std::setprecision(prec_time) << fi <<
         "c running_time(sec)                     " << elapsed[0] << "\n"
         "c   system_time                         " << elapsed[1] << "\n"
         "c   elapsed_wall_clock                  " << diff_t(current-t1W).count() << "\n"
         "c   max_memory(MB)                      " << timing.mem() << "\n"
         "c number_of_nodes                       " << n_nodes << "\n"
         "c   number_of_binary_nodes              " << n_backtracks << "\n"
         "c   number_of_single_child_nodes        " << single_child << "\n"
         "c   number_of_leaves                    " << leaves << "\n"
         "c   number_of_internal_nodes            " << inodes << "\n"
         "c     inodes_per_second                 " << sc << inodes / elapsed[0] << fi << "\n"
         "c number_of_1-reductions                " << n_units << "\n"
         "c   1-reductions_per_second             " << sc << n_units / elapsed[0] << fi << "\n"
         "c   1-reductions_per_node               " << double(n_units) / n_nodes << "\n"
#ifdef PURE_LITERALS
         "c   number_of_pure_literals             " << n_pure_literals << "\n"
#endif
         << std::setprecision(prec_heuristics) << fi <<
         "c heuristics_evaluations                " << n_evaluations << "\n"
#ifndef FIRST_VARIABLE
         "c   number_projections                  " << n_proj << "\n"
         "c     average_projections               " << double(n_proj) / n_evaluations << "\n"
         "c     number_onezero                    " << n_proj1zero << "\n"
         "c       freq_onezero                    " << double(n_proj1zero) / n_proj << "\n"
         "c     number_bothzero                   " << n_proj2zero << "\n"
         "c       freq_bothzero                   " << double(n_proj2zero) / n_proj << "\n"
# ifdef TAU_ITERATION
         "c   number_wtau_calls                   " << wtau_calls << "\n"
         "c     average_wtau_calls                " << double(wtau_calls) / n_withfirst << "\n"
         "c   number_tau_iterations               " << tau_iterations << "\n"
         "c     average_tau_iterations            " << double(tau_iterations) / wtau_calls << "\n"
# endif
         "c   number_withfirst                    " << n_withfirst << "\n"
         "c     freq_withfirst                    " << double(n_withfirst) / n_evaluations << "\n"
         "c   number_nofirst                      " << n_nofirst << "\n"
         "c     freq_nofirst                      " << double(n_nofirst) / n_evaluations << "\n"
         "c   min_first                           " << min_first << "\n"
         "c     mean_first                        " << m_first << "\n"
         "c     max_first                         " << max_first << "\n"
         "c     sd_first                          " << sd_first << "\n"
         "c   number_withsecond                   " << n_withsecond << "\n"
         "c     freq_withsecond                   " << double(n_withsecond) / n_evaluations << "\n"
         "c   number_nosecond                     " << n_nosecond << "\n"
         "c     freq_nosecond                     " << double(n_nosecond) / n_evaluations << "\n"
         "c   min_second                          " << min_second << "\n"
         "c     mean_second                       " << m_second << "\n"
         "c     max_second                        " << max_second << "\n"
         "c     sd_second                         " << sd_second << "\n"
# ifdef ALPHA
         << std::setprecision(prec_const) << fi <<
         "c   alpha                               " << ALPHA << "\n"
# endif
#endif

#ifdef ALL_SOLUTIONS
#ifdef LAMBDA
         << std::setprecision(prec_const) << fi <<
         "c   lambda                              " << LAMBDA << "\n"
#endif
         "c number_of_solutions                   " << sc << std::setprecision(count_digits) << n_solutions << "\n"
#endif
         "c reading-and-set-up_time(sec)          " << std::setprecision(prec_time_small) << std::fixed << (t1 - t0)[0] << "\n"
         "c   system                              " << (t1 - t0)[1] << "\n"
         "c   elapsed                             " << diff_t(t1W-t0W).count()
  ;
  logout.endl();
}
#ifndef ALL_SOLUTIONS
  if (result == sat) solout << sat_pass;
#endif
#ifdef VAR_MARGINALS
  logout << marginals;
#endif
}


// Initialising the output objects solout, logout, errout from the
// command-line arguments:
void set_output(const int argc, const char* const argv[]) {
  std::ios_base::sync_with_stdio(false);
  logout.p = &std::cout;
  errout.p = &std::cerr;
  if (argc == 2) {
#ifndef ALL_SOLUTIONS
    solout.p = &std::cout;
#endif
    return;
  }
  const std::string solname(argv[2]);
  if (solname == "-cout") solout.p = &std::cout;
  else if (solname == "-cerr") solout.p = &std::cerr;
  else if (solname != "-nil") {
    if (solname == filename and filename != "-cin") {
      errout << ("Invalid output filename: \"" + solname + "\".");
      std::exit(file_writing_error);
    }
    solout.p = new std::ofstream(solname, std::ios::app);
    if (not *solout.p) {
      errout << ("Invalid output file: \"" + solname + "\".");
      std::exit(file_writing_error);
    }
    solout.del = true;
  }
  if (argc == 3) return;
  const std::string logname(argv[3]);
  if (logname == "-cerr") logout.p = &std::cerr;
  else if (logname == "-nil") logout.p = nullptr;
  else if (logname != "-cout") {
    if (logname == filename and filename != "-cin") {
      errout << ("Invalid output filename: \"" + logname + "\".");
      std::exit(file_writing_error);
    }
    if (logname == solname) logout.p = solout.p;
    else {
      logout.p = new std::ofstream(logname, std::ios::app);
      if (not *logout.p) {
        errout << ("Invalid output file: \"" + logname + "\".");
        std::exit(file_writing_error);
      }
      logout.del = true;
    }
  }
}

void abortion(const int) {
  std::signal(SIGINT, abortion);
  output(unknown);
  std::exit(unknown);
}
void show_statistics(const int) {
  signal(SIGUSR1, show_statistics);
  output(unknown);
}

} // anonymous namespace

int main(const int argc, const char* const argv[]) {
  if (argc == 1) show_usage();
  filename = argv[1];
  if (filename == "-v" or filename == "--version") version_information();

  set_output(argc, argv);

  t0W = std::chrono::high_resolution_clock::now();
  t0 = timing();
  read_formula(filename);
  if (n_clauses) {
    try { initialisation(); }
    catch (const std::bad_alloc&) {
      errout << "Allocation error with initialisation of algorithmic"
       " data structures.";
      return allocation_error;
    }
  }
  std::signal(SIGINT, abortion);
  std::signal(SIGUSR1, show_statistics);
  t1 = timing();
  t1W = std::chrono::high_resolution_clock::now();
  const auto result = dll0();
  const auto ires = interprete_run(result);
  output(ires);
  return ires;
}
