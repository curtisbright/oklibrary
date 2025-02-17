// Oliver Kullmann, 6.3.2022 (Swansea)
/* Copyright 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*
  Solving biclique-covering problems by SAT-solving, computing bcc(G)
  for an input-graph G

  (BCC2SAT computes one single SAT-translation, while this program
   attempts to run a complete chain of SAT-solving, starting with a given
   upper bound B on the number of bicliqes)


EXAMPLES:

Bicliques> ./GraphGen clique 16 | ./BCCbySAT 4 ""
Minisat-call for B=4: returned SAT
  Literal-Reduction by trimming: 0
  Size obtained: 4
Minisat-call for B=3: returned UNSAT

bcc=4
exact 4 4
4 7 10 11 12 13 15 16 | 1 2 3 5 6 8 9 14
1 3 4 9 12 14 15 16 | 2 5 6 7 8 10 11 13
3 4 5 6 7 10 12 14 | 1 2 8 9 11 13 15 16
4 6 7 8 9 11 14 16 | 1 2 3 5 10 12 13 15

Remark: The symmetry-breaking is randomised (100 trials above),
and thus the results are also randomised.


TODOS:

1. Output seeds

2. Read seeds

3. Supply format-options

*/


#include <iostream>

#include <ProgramOptions/Environment.hpp>

#include "Graphs.hpp"
#include "Bicliques2SAT.hpp"

#include "BCC2SAT.hpp"

namespace {

  const Environment::ProgramInfo proginfo{
        "0.2.1",
        "7.3.2022",
        __FILE__,
        "Oliver Kullmann",
        "https://github.com/OKullmann/oklibrary/blob/master/Satisfiability/Transformers/Generators/Bicliques/BCCbySAT.cpp",
        "GPL v3"};

  using namespace Bicliques2SAT;
  using namespace BCC2SAT;

  const std::string error = "ERROR[" + proginfo.prg + "]: ";

  bool show_usage(const int argc, const char* const argv[]) {
    if (not Environment::help_header(std::cout, argc, argv, proginfo))
      return false;
    std::cout <<
    "> " << proginfo.prg << " B=" << default_B << " algo-options"
      " [sb-rounds=" << default_sb_rounds << "]"
      " [timeout(sec)=" << default_sec << "]\n\n"
    " algo-options   : " << Environment::WRP<SB>{} << "\n\n"
    " reads a graph from standard input, and attempts to compute its bcc-number:\n\n"
    "  - Arguments \"\" (the empty string) yield also the default-values.\n"
    "  - Default-values for the options are the first possibilities given.\n\n"
;
    return true;
  }

}

int main(const int argc, const char* const argv[]) {

  if (Environment::version_output(std::cout, proginfo, argc, argv)) return 0;
  if (show_usage(argc, argv)) return 0;

  if (argc < 3) {
    std::cerr << error << "At least two arguments (B, algo-opt)"
      " needed, but only " << argc-1 << " provided.\n";
    return int(Error::missing_parameters);
  }

  const var_t B = read_var_t(argv[1], default_B);
  const alg_options_t algopt =
    Environment::translate<alg_options_t>()(argv[2], sep);
  const var_t sb_rounds = argc >= 4 ?
    read_var_t(argv[3], default_sb_rounds) : default_sb_rounds;
  const auto sec = argc >= 5 ?
    read_uint_t(argv[4], default_sec) : default_sec;

  if (std::get<SB>(algopt) != SB::none and sb_rounds == 0) {
    std::cerr << error <<
      "Symmetry-breaking on, but number of rounds is zero.\n";
    return int(Error::bad_sb);
  }
  if (std::get<SB>(algopt) == SB::extended) {
    std::cerr << error <<
      "Extended symmetry-breaking not implemented yet.\n";
    return int(Error::bad_sb);
  }

  const auto G = Graphs::make_AdjVecUInt(std::cin, Graphs::GT::und);
  BC2SAT trans(G, B);
  const auto res = trans(&std::cout, algopt, sb_rounds, sec);
  std::cout << "\n"; // separation from log-output
  res.output(std::cout, G);

}
