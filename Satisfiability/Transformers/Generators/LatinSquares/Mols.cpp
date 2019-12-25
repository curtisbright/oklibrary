// Oliver Kullmann, 20.12.2019 (Swansea)
/* Copyright 2019 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*


Examples:

Fuer 1 <= N <= 11:

https://oeis.org/A002860 :
Number of Latin squares of order n; or labeled quasigroups
1, 2, 12, 576, 161280,
812851200, 61479419904000, 108776032459082956800, 5524751496156892842531225600, 9982437658213039871725064756920320000,
776966836171770144107444346734230682311065600000

> ./Mols_debug 5 1 | ctawSolver -cin
s SATISFIABLE
c max_occurring_variable                125
c number_of_clauses                     825
c maximal_clause_length                 5
c number_of_literal_occurrences         1875
c running_time(sec)                     0.67
c number_of_nodes                       322559
c number_of_binary_nodes                161279
c number_of_1-reductions                3349386
c number_of_solutions                   161280
c reading-and-set-up_time(sec)          0.001
c p_param_variables                     125
c p_param_clauses                       825
c number_tautologies                    0
c file_name                             -cin
c options                               "A19"

> ./Mols_debug 6 1 | ctawSolver -cin
s SATISFIABLE
c max_occurring_variable                216
c number_of_clauses                     1728
c maximal_clause_length                 6
c number_of_literal_occurrences         3888
c running_time(sec)                     3497.44
c number_of_nodes                       1625702399
c number_of_binary_nodes                812851199
c number_of_1-reductions                17390690182
c number_of_solutions                   812851200
c reading-and-set-up_time(sec)          0.001
c p_param_variables                     216
c p_param_clauses                       1728
c number_tautologies                    0
c file_name                             -cin
c options                               "A19"


*/

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <optional>

#include <cassert>
#include <cstdint>
#include <cstdlib>

#include <ProgramOptions/Environment.hpp>
#include "../Random/ClauseSets.hpp"

namespace {

  const Environment::ProgramInfo proginfo{
        "0.2.0",
        "25.12.2019",
        __FILE__,
        "Oliver Kullmann",
        "https://github.com/OKullmann/oklibrary/blob/master/Satisfiability/Transformers/Generators/LatinSquares/Mols.cpp",
        "GPL v3"};

  const std::string error = "ERROR[" + proginfo.prg + "]: ";

  enum class Error {
    conversion = 11,
    too_big = 12,
    too_small = 13,
    file_open = 14,
  };


  typedef std::uint16_t dim_t;
  typedef std::uint64_t var_t;

  constexpr dim_t N_default = 6;
  constexpr dim_t k_default = 2;


  bool show_usage(const int argc, const char* const argv[]) {
    assert(argc >= 1);
    if (argc != 2 or not Environment::is_help_string(argv[1])) return false;
    const std::string& program = proginfo.prg;
    std::cout << "USAGE:\n"
    "> " << program << " [-v | --version]\n"
    " shows version information and exits.\n"
    "> " << program << " [-h | --help]\n"
    " shows help information and exits.\n"
    "> " << program << " [N=" << N_default << "] [k=" << k_default << "] [output=-cout]\n"
    " computes the SAT-translation.\n"
    " Trailing arguments can be left out, using their default-values.\n"
    " \"\" for the output means the default output-filename.\n"
;
    return true;
  }


  struct Param {
    dim_t N;
    dim_t k;
  };
  std::ostream& operator <<(std::ostream& out, const Param p) {
    return out << p.N << "," << p.k;
  }

  struct NumVarsCls {
    var_t nbls, // number of variables per single latin-square
          nls, // number of variables for all single latin-squares
          nbes, // number of variables for single euler-square
          nes, // number of variables for all euler-squares
          n; // number of variables in total
    var_t cbls, cls, c;
  };
  constexpr NumVarsCls numvarscls(const Param p) noexcept {
    const FloatingPoint::float80 N = p.N;
    const auto N2 = N*N;
    const auto N3 = N2*N;
    const auto N4 = N2*N2;
    using FloatingPoint::fbinomial_coeff;

    const auto nbls = N3;
    const auto nls = nbls * p.k;
    const auto nbes = N4;
    const auto nes = nbes * fbinomial_coeff(p.k, 2);
    const auto n = nls + nes;

    if (n >= FloatingPoint::P264) {
      std::cerr << error << "Parameters " << p << " yield total number of variables >= 2^64.\n";
      std::exit(int(Error::too_big));
    }

    const auto cbls = 3 * N2 * (1 + fbinomial_coeff(N, 2));
    const auto cls = cbls * p.k;
    const auto c = cls;

    if (c >= FloatingPoint::P264) {
      std::cerr << error << "Parameters " << p << " yield total number of clauses >= 2^64.\n";
      std::exit(int(Error::too_big));
    }

    return NumVarsCls
                  {var_t(nbls), var_t(nls), var_t(nbes), var_t(nes), var_t(n),
                   var_t(cbls), var_t(cls), var_t(c)};
  }

  struct IndexEuler {
    dim_t p, q; // p < q
  };
  constexpr bool operator <(const IndexEuler e1, const IndexEuler e2) noexcept {
    return e1.q < e2.q or (e1.q == e2.q and e1.p < e2.p);
  }
  constexpr var_t index(const IndexEuler e) noexcept {
    if (e.q % 2 == 0) return e.p + (e.q/2) * (e.q-1);
    else return e.p + e.q * ((e.q-1) / 2);
  }
  static_assert(index({0,1}) == 0);
  static_assert(index({0,2}) == 1);
  static_assert(index({1,2}) == 2);
  static_assert(index({0,3}) == 3);

  struct Encoding {
    const dim_t N;
    const dim_t k;
    const NumVarsCls nv;

    const var_t N2 = var_t(N)*N;
    const var_t N3 = N2 * N;

    constexpr Encoding(const Param ps) noexcept : N(ps.N), k(ps.k), nv(numvarscls(ps)) {}

    constexpr var_t operator()(const dim_t i, const dim_t j, const dim_t eps, const dim_t p) const noexcept {
      assert(i < N);
      assert(j < N);
      assert(eps < N);
      assert(p < k);
      return 1 + i * N2 + j * N + eps + p * nv.nbls;
    }
    constexpr var_t operator()(const dim_t i, const dim_t j, const dim_t eps, const IndexEuler pq) const noexcept {
      assert(i < N);
      assert(j < N);
      assert(eps < N2);
      assert(pq.p < pq.q);
      assert(pq.q < k);
      return nv.nls + 1 + i * N3 + j * N2 + eps + index(pq) * nv.nbes;
    }

  };
  static_assert(Encoding({1,1})(0,0,0,0) == 1);
  static_assert(Encoding({2,1})(0,0,0,0) == 1);
  static_assert(Encoding({2,1})(0,0,1,0) == 2);
  static_assert(Encoding({2,1})(0,1,0,0) == 3);
  static_assert(Encoding({2,1})(0,1,1,0) == 4);
  static_assert(Encoding({2,1})(1,0,0,0) == 5);
  static_assert(Encoding({2,1})(1,0,1,0) == 6);
  static_assert(Encoding({2,1})(1,1,0,0) == 7);
  static_assert(Encoding({2,1})(1,1,1,0) == 8);
  static_assert(Encoding({2,2})(0,0,0,1) == 9);
  static_assert(Encoding({2,2})(0,0,0,{0,1}) == 17);
  static_assert(Encoding({2,2})(0,0,1,{0,1}) == 18);
  static_assert(Encoding({2,3})(1,1,3,{1,2}) == 72);


  using RandGen::Var;
  using RandGen::Lit;
  using RandGen::Clause;

  void eo_primes(std::ostream& out, const Clause& C) {
    if (C.size() >= 2) {
      auto current_end = C.cbegin(); ++current_end;
      do {
        const Lit y = -*current_end;
        for (auto i = C.cbegin(); i != current_end; ++i)
          out << Clause{-*i, y};
      } while (++current_end != C.end());
    }
    out << C;
  }

  void ls(std::ostream& out, const Encoding& enc) {
    for (dim_t p = 0; p < enc.k; ++p) {
      // EO(i,-,eps,p) :
      for (dim_t i = 0; i < enc.N; ++i)
        for (dim_t eps = 0; eps < enc.N; ++eps) {
          Clause C;
          for (dim_t j = 0; j < enc.N; ++j)
            C.push_back(Lit{enc(i,j,eps,p),1});
          eo_primes(out, C);
        }
      // EO(-,j,eps,p) :
      for (dim_t j = 0; j < enc.N; ++j)
        for (dim_t eps = 0; eps < enc.N; ++eps) {
          Clause C;
          for (dim_t i = 0; i < enc.N; ++i)
            C.push_back(Lit{enc(i,j,eps,p),1});
          eo_primes(out, C);
        }
      // EO(i,j,-,p) :
      for (dim_t i = 0; i < enc.N; ++i)
        for (dim_t j = 0; j < enc.N; ++j) {
          Clause C;
          for (dim_t eps = 0; eps < enc.N; ++eps)
            C.push_back(Lit{enc(i,j,eps,p),1});
          eo_primes(out, C);
        }
    }
  }


  std::string default_filestem() {
    return "MOLS2SAT_BASIC";
  }
  std::string default_param(const Param p) {
    return std::to_string(p.N) + "_" + std::to_string(p.k);
  }
  std::string default_filesuffix() {
    return ".dimacs";
  }
  std::string default_filename(const Param p) {
    return default_filestem() + "_" + default_param(p) + default_filesuffix();
  }


  dim_t read_dim(const std::string arg) {
    unsigned long d;
    std::size_t converted;
    try { d = std::stoul(arg, &converted); }
    catch (const std::invalid_argument& e) {
      std::cerr << error << "The argument \"" << arg << "\" is not a valid integer.\n";
      std::exit(int(Error::conversion));
    }
    catch (const std::out_of_range& e) {
      std::cerr << error << "The argument \"" << arg << "\" is too big for unsigned long.\n";
      std::exit(int(Error::too_big));
    }
    if (converted != arg.size()) {
      std::cerr << error << "The argument \"" << arg << "\" contains trailing characters: \""
        << arg.substr(converted) << "\".\n";
      std::exit(int(Error::conversion));
    }
    if (d == 0) {
      std::cerr << error << "An argument is 0.\n";
      std::exit(int(Error::too_small));
    }
    const dim_t cd = d;
    if (cd != d) {
      std::cerr << error << "The argument \"" << arg << "\" is too big for dim_t.\n";
      std::exit(int(Error::too_big));
    }
    return cd;
  }

}

int main(const int argc, const char* const argv[]) {

  if (Environment::version_output(std::cout, proginfo, argc, argv)) return 0;
  if (show_usage(argc, argv)) return 0;

  Environment::Index index;

  const dim_t N = argc <= index ? N_default : read_dim(argv[index++]);
  const dim_t k = argc <= index ? k_default : read_dim(argv[index++]);
  const Param p{N,k};

  std::ofstream out;
  std::string filename;
  if (index == argc or std::string_view(argv[index]) == "-cout") {
    out.basic_ios<char>::rdbuf(std::cout.rdbuf());
    filename = "-cout";
  }
  else {
    filename = argv[index];
    if (filename.empty()) filename = default_filename(p);
    out.open(filename);
    if (not out) {
      std::cerr << error << "Can't open file \"" << filename << "\"\n";
      return int(Error::file_open);
    }
    std::cout << "Output to file \"" << filename << "\".\n";
  }
  index++;

  index.deactivate();

  const Encoding enc(p);


  out << Environment::Wrap(proginfo, Environment::OP::dimacs);
  using Environment::DHW;
  using Environment::DWW;
  using Environment::qu;
  out << DHW{"Parameters"}
            << DWW{"command-line"};
  Environment::args_output(out, argc, argv);
  out << "\n"
            << DWW{"N"} << N << "\n"
            << DWW{"k"} << k << "\n"
            << DWW{"output"} << qu(filename) << "\n"
      << DHW{"Sizes"}
            << DWW{"nls"} << enc.nv.nls << "\n"
            << DWW{"nes"} << enc .nv.nes << "\n"
            << DWW{"n"} << enc.nv.n << "\n"
            << DWW{"cls"} << enc.nv.cls << "\n"
;

  out << RandGen::dimacs_pars{enc.nv.n, enc.nv.c};
  ls(out, enc);

}
