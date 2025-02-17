// Oliver Kullmann, 21.3.2022 (Swansea)
/* Copyright 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*
  Partial squares

  Internally and for input and output the numbers are 0, ..., N-1.

TODOS:

1. Clarify the relations to Random/LatinSquares.hpp
 - Especially Random::ls_ip_t is similar to PSquares.
 - Though the context is different: in Random/LatinSquares.hpp one is
   concerned with direct computation with latin squares, while here
   we are concerned with representing conditions on (arbitrary) squares.

*/

#ifndef PARTIALSQUARES_xfWDmQELnQ
#define PARTIALSQUARES_xfWDmQELnQ

#include <vector>
#include <algorithm>
#include <istream>
#include <exception>

#include <ProgramOptions/Strings.hpp>
#include <Numerics/NumInOut.hpp>

#include "Conditions.hpp"

namespace PartialSquares {

  namespace CD = Conditions;

  typedef CD::size_t size_t;


  // index i being true means value i is *disabled*:
  typedef std::vector<bool> cell_t;
  struct Cell {
    cell_t c;

    Cell() noexcept = default;
    Cell(const cell_t c) : c(c) {}
    Cell(const size_t N) : c(N) {}

    size_t size() const noexcept { return c.size(); }
    Cell& flip() noexcept { c.flip(); return *this; }
    bool consistent() const noexcept {
      return std::ranges::find(c, 0) != c.end();
    }
    bool unit() const noexcept {
      const auto first = std::ranges::find(c, 0);
      const auto end = c.end();
      if (first == end) return false;
      return std::find(first+1, end, 0) == end;
    }

    bool operator ==(const Cell&) const noexcept = default;
    auto operator <=>(const Cell&) const noexcept = default;
  };
  std::ostream& operator <<(std::ostream& out, const Cell& c) {
    std::vector<size_t> content;
    for (size_t i = 0; i < c.size(); ++i) if (not c.c[i]) content.push_back(i);
    if (content.empty()) out << "E";
    else Environment::out_line(out, content, ",");
    return out;
  }


  typedef std::vector<Cell> prow_t;
  typedef std::vector<prow_t> psquare_t;

  bool valid(const prow_t& pr, const size_t N) noexcept {
    return pr.size() == N and
      std::ranges::all_of(pr, [&N](const Cell& c){return c.size()==N;});
  }
  bool valid(const prow_t& pr) noexcept {
    return valid(pr, pr.size());
  }
  bool valid(const psquare_t& ps, const size_t N) noexcept {
    return ps.size() == N and
      std::ranges::all_of(ps, [&N](const prow_t& r){return valid(r,N);});
  }
  bool valid(const psquare_t& ps) noexcept {
    return valid(ps, ps.size());
  }

  bool consistent(const prow_t& pr) noexcept {
    return std::ranges::all_of(pr, [](const Cell& c){return c.consistent();});
  }
  bool consistent(const psquare_t& ps) noexcept {
    return std::ranges::all_of(ps, [](const prow_t& r){return consistent(r);});
  }

  prow_t empty_prow(const size_t N) {
    return prow_t(N, cell_t(N));
  }
  prow_t full_prow(const size_t N) {
    return prow_t(N,cell_t(N,1) );
  }
  psquare_t empty_psquare(const size_t N) {
    return psquare_t(N, empty_prow(N));
  }
  psquare_t full_psquare(const size_t N) {
    return psquare_t(N, full_prow(N));
  }

  void flipm(prow_t& pr) noexcept {
    for (Cell& c : pr) c.flip();
  }
  void flipm(psquare_t& ps) noexcept {
    for (prow_t& pr : ps) flipm(pr);
  }


  struct PSquare {
    psquare_t ps;
    CD::Square s;

    PSquare(const size_t N) : ps(empty_psquare(N)), s(0) {}
    PSquare(const psquare_t ps, const CD::Square s) : ps(ps), s(s) {}

    bool consistent() const noexcept { return PartialSquares::consistent(ps); }

    bool operator ==(const PSquare&) const noexcept = default;
    auto operator <=>(const PSquare&) const noexcept = default;
  };
  std::ostream& operator <<(std::ostream& out, const PSquare& ps) {
    out << ps.s << "\n";
    Environment::out_lines(out, ps.ps);
    return out;
  }

  bool valid(const PSquare& ps, const size_t N) {
    return valid(ps.ps, N);
  }
  void flipm(PSquare& ps) noexcept {
    flipm(ps.ps);
  }
  PSquare flip(const PSquare& ps) noexcept {
    PSquare res(ps);
    flipm(res);
    return res;
  }


  struct PSquares {
    typedef std::vector<PSquare> psquares_t;
    psquares_t psqs;
    const size_t N;

    PSquares(const size_t N, std::istream& in) : psqs(read(in,N)), N(N) {}
    PSquares(const size_t N, const psquares_t p) : psqs(p), N(N) {
      assert(std::ranges::all_of(psqs, [&N](const auto& s){
                                   return valid(s,N);}));
    }

    bool consistent() const noexcept {
      return std::ranges::all_of(psqs,
        [](const PSquare& ps){return ps.consistent();});
    }

    struct Error : std::runtime_error {
      Error(const std::string s) noexcept :
      std::runtime_error("ERROR[PSquares]: " + s) {}
    };

    // For the stream-input, the values are numbers 1, ..., N:
    static psquares_t read(std::istream& in, const size_t N) {
      const auto lines = Environment::split2_cutoff(in, '\n', '#');
      if (lines.size() % (N+1) != 0) {
        std::ostringstream s;
        s << "The number of lines should be a multiple of N+1=" <<
          N+1 << ", but is " << lines.size() << ".";
        throw Error(s.str());
      }
      const size_t k = lines.size() / (N+1);
      psquares_t res(k, N);

      for (size_t i = 0; i < k; ++i) { // k squares
        const size_t i0 = i*(N+1);
        size_t j = 0;
        const auto osq = CD::Square::read(lines[i0], j);
        if (not osq) {
          std::ostringstream s;
          s << "Invalid name of square number " << i+1 <<
            " in the header-line: \"";
          Environment::out_line(s, lines[i0]);
          s << "\".";
          throw Error(s.str());
        }
        res[i].s = osq.value();
        if (j < lines[i0].size()) {
          std::ostringstream s;
          s << "In square number " << i+1 <<
            " there is content after the name \"" << res[i].s <<
            "\" of the square.";
          throw Error(s.str());
        }

        for (size_t ip = 0; ip < N; ++ip) { // N rows of square i
          const size_t i1 = i0+1 + ip;
          const auto& line = lines[i1];
          if (line.size() != N) {
            std::ostringstream s;
            s << "In square number " << i+1 << " in row " << ip+1 <<
              " there should be exactly " << N << " entries, but there are " <<
              line.size() << ";\n the whole line is: \"";
             Environment::out_line(s, line);
             s << "\".";
             throw Error(s.str());
          }
          for (size_t j = 0; j < N; ++j) { // N entries of line ip
            if (line[j] == "*") continue;
            const bool exclude = line[j][0] == '-';
            const bool sign = exclude or (line[j][0] == '+');
            if (not exclude) res[i].ps[ip][j].flip();
            const auto items = Environment::split(
              sign ? line[j].substr(1) : line[j], ',');
            for (const std::string item : items) {
              size_t x;
              try {x = FloatingPoint::to_UInt(item);}
              catch (std::exception& e) {
                std::ostringstream s;
                s << "In square number " << i+1 << ", row " << ip+1 <<
                  ", the cell " << j+1 << " is \"" << line[j] << "\".\n" <<
                  " Its item \"" << item << "\" could not"
                  " be parsed as a proper natural number:\n"
                  " The parsing-exception says \"" << e.what() << "\"";
                throw Error(s.str());
              }
              if (x >= N) {
                std::ostringstream s;
                s << "In square number " << i+1 << ", row " << ip+1 <<
                  ", cell " << j+1 << ", the item \"" << item << "\" has "
                  "value " << x << " >= " << N << ".";
                throw Error(s.str());
              }
              res[i].ps[ip][j].c[x] = exclude;
            }
          }
        }
      }
      return res;
    }

    bool operator ==(const PSquares&) const noexcept = default;
  };
  std::ostream& operator <<(std::ostream& out, const PSquares& psqs) {
    Environment::out_line(out, psqs.psqs, "");
    return out;
  }

  bool included(const PSquares& ps, const CD::AConditions& ac) noexcept {
    return std::ranges::all_of(ps.psqs, [&ac](const PSquare& p){
                                 return ac.contains(p.s);});
  }

  bool valid(const PSquares& ps, const CD::AConditions& ac) noexcept {
    return included(ps, ac) and ps.consistent();
  }

}

#endif


