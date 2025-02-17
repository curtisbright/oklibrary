// Oliver Kullmann, 28.2.2022 (Swansea)
/* Copyright 2022 Oliver Kullmann
This file is part of the OKlibrary. OKlibrary is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation and included in this library; either version 3 of the
License, or any later version. */

/*

  Bicliques on their own, and coverings and partitions with them

   - DT = DimacsTools

   - from Graphs:
    - AdjVecUInt, id_t, list_t

   - bc_frame
    - valid0/01(list_t, AdjVecUInt)
    - valid1/2(list_t)
    - valid(list_t, AdjVecUInt)
    - valid1/2(bc_frame), valid(bc_frame)
    - disjoint(bc_frame)

   - is_star(id_t, list_t, AdjVecUInt)
   - is_bc(bc_frame, AdjVecUInt)
   - covers(bc_frame, id_t, id_t)

   - Bcc_frame
   - valid2(Bcc_frame), valid(Bcc_frame, AdjVecUInt)
   - disjoint(Bcc_frame)
   - is_bc(Bcc_frame, AdjVecUInt)
   - covers(Bcc_frame, id_t, id_t)
   - is_cover(Bcc_frame, AdjVecUInt)
   - is_bcc(Bcc_frame, AdjVecUInt)

   - numocc(Bcc_frame)
   - numcl(Bcc_frame)

   - bcc2CNF(Bcc_frame)
   - CNF2bcc(DT::DimacsClauseList)

   - sort(bc_frame&)
   - sort(Bcc_frame&)

   - triv_trim(Bcc_frame&)
   - trim(Bcc_frame&)

*/

#ifndef BICLIQUES_zlDSHv4COU
#define BICLIQUES_zlDSHv4COU

#include <algorithm>
#include <vector>
#include <map>
#include <iterator>
#include <utility>

#include <cassert>

#include "Graphs.hpp"
#include "DimacsTools.hpp"
#include "ConflictGraphs.hpp"

namespace Bicliques {

  namespace DT = DimacsTools;

  typedef Graphs::AdjVecUInt AdjVecUInt;
  typedef AdjVecUInt::id_t id_t;
  typedef AdjVecUInt::list_t list_t;
  typedef AdjVecUInt::edge_t edge_t;

  struct bc_frame {
    list_t l, r;

    bool no_edge() const noexcept {return l.empty() or r.empty();}
    bool no_vertex() const noexcept {return l.empty() and r.empty();}

    bool operator ==(const bc_frame& rhs) const noexcept = default;
    friend std::ostream& operator <<(std::ostream& out, const bc_frame& bc) {
      if (bc.l.empty()) out << " |";
      else {
        for (const id_t v : bc.l) out << v << " ";
        out << "|";
      }
      if (bc.r.empty()) out << " ";
      else for (const id_t v : bc.r) out << " " << v;
      return out;
    }
    void output(std::ostream& out, const AdjVecUInt& G) const {
      if (l.empty()) out << " |";
      else {
        for (const id_t v : l) out << G.name(v) << " ";
        out << "|";
      }
      if (r.empty()) out << " ";
      else for (const id_t v : r) out << " " << G.name(v);
    }
  };

  // Valid vertices:
  inline bool valid0(const list_t& L, const AdjVecUInt& G) noexcept {
    const auto n = G.n();
    return std::all_of(L.begin(), L.end(), [&n](const id_t v){return v < n;});
  }
  // Sorted:
  inline bool valid1(const list_t& L) noexcept {
    return std::ranges::is_sorted(L);
  }
  inline bool valid01(const list_t& L, const AdjVecUInt& G) noexcept {
    return valid0(L, G) and valid1(L);
  }
  // All different (given that L is sorted):
  inline bool valid2(const list_t& L) noexcept {
    assert(valid1(L));
    return std::adjacent_find(L.begin(), L.end()) == L.end();
  }
  inline bool valid(const list_t& L, const AdjVecUInt& G) noexcept {
    return valid01(L, G) and valid2(L);
  }
  bool valid1(const bc_frame& b) noexcept {
    return valid1(b.l) and valid1(b.r);
  }
  bool valid2(const bc_frame& b) noexcept {
    return valid2(b.l) and valid2(b.r);
  }
  bool valid(const bc_frame& b, const AdjVecUInt& G) noexcept {
    return valid(b.l, G) and valid(b.r, G);
  }

  inline bool disjoint(const bc_frame& b) noexcept {
    assert(valid1(b));
    return ConflictGraphs::empty_intersection(b.l, b.r);
  }

  inline bool is_star(const id_t v, const list_t& L, const AdjVecUInt& G) noexcept {
    assert(v < G.n());
    assert(valid(L, G));
    return std::ranges::includes(G.neighbours(v), L);
  }
  inline bool is_bc(const bc_frame& b, const AdjVecUInt& G) noexcept {
    assert(valid(b, G));
    assert(G.type() == Graphs::GT::und);
    if (b.l.size() <= b.r.size()) {
      const auto test = [&b, &G](const id_t v){return is_star(v, b.r, G);};
      return std::all_of(b.l.begin(), b.l.end(), test);
    }
    else {
      const auto test = [&b, &G](const id_t v){return is_star(v, b.l, G);};
      return std::all_of(b.r.begin(), b.r.end(), test);
    }
  }
  inline bool covers(const bc_frame& b, const id_t v, const id_t w) noexcept {
    const auto lb = b.l.begin(); const auto le = b.l.end();
    const auto rb = b.r.begin(); const auto re = b.r.end();
    return
      (std::binary_search(lb,le,v) and std::binary_search(rb,re,w)) or
      (std::binary_search(lb,le,w) and std::binary_search(rb,re,v));
  }


  struct Bcc_frame {
    typedef std::vector<bc_frame> v_t;
    v_t L;

    Bcc_frame() noexcept = default;
    explicit Bcc_frame(const id_t n) : L(n) {}
    Bcc_frame(v_t L) noexcept : L(L) {}

    bool operator ==(const Bcc_frame& rhs) const noexcept = default;
    friend std::ostream& operator <<(std::ostream& out, const Bcc_frame& bcc) {
      for (const auto& bc : bcc.L) out << bc << "\n";
      return out;
    }
    void output(std::ostream& out, const AdjVecUInt& G) const {
      for (const auto& bc : L) {
        bc.output(out, G); out << "\n";
      }
    }
  };
  inline bool valid2(const Bcc_frame& B) noexcept {
    return std::all_of(B.L.begin(), B.L.end(),
                       [](const bc_frame& b){return valid2(b);});
  }
  inline bool valid(const Bcc_frame& B, const AdjVecUInt& G) noexcept {
    return std::all_of(B.L.begin(), B.L.end(),
                       [&G](const bc_frame& b){return valid(b, G);});
  }
  inline bool is_bc(const Bcc_frame& B, const AdjVecUInt& G) noexcept {
    assert(valid(B, G));
    return std::all_of(B.L.begin(), B.L.end(),
                       [&G](const bc_frame& b){return is_bc(b, G);});
  }
  inline bool covers(const Bcc_frame& B, const id_t v, const id_t w) noexcept {
    return std::any_of(B.L.begin(), B.L.end(),
                       [v,w](const bc_frame& b){return covers(b,v,w);});
  }
  inline bool is_cover(const Bcc_frame& B, const AdjVecUInt& G) noexcept {
    assert(valid(B, G));
    const auto E = G.alledges();
    return std::all_of(E.begin(), E.end(),
                       [&B](const auto e){return covers(B,e.first,e.second);});
  }
  inline bool is_bcc(const Bcc_frame& B, const AdjVecUInt& G) noexcept {
    return is_bc(B, G) and is_cover(B, G);
  }

  inline bool disjoint(const Bcc_frame& B) noexcept {
    return std::all_of(B.L.begin(), B.L.end(),
                       [](const bc_frame& b){return Bicliques::disjoint(b);});
    }


  // The number of literal occurrences:
  id_t numocc(const Bcc_frame& B) noexcept {
    id_t res = 0;
    for (const bc_frame& b : B.L) { res += b.l.size(); res += b.r.size(); }
    return res;
  }

  // The number of clauses, i.e., the maximal-vertex + 1 in B:
  id_t numcl(const Bcc_frame& B) noexcept {
    id_t res = 0;
    for (const bc_frame& b : B.L) {
      for (const id_t v : b.l) res = std::max(v+1, res);
      for (const id_t v : b.r) res = std::max(v+1, res);
    }
    return res;
  }

  DT::DimacsClauseList bcc2CNF(const Bcc_frame& B, const id_t c0 = 0) {
    const id_t c = std::max(c0,numcl(B));
    const DT::dimacs_pars dp{B.L.size(), c};
    DT::ClauseList F(dp.c);
    for (DT::var_t v = 0; v < dp.n; ++v) {
      for (const id_t i : B.L[v].l) F[i].push_back(DT::Lit{v+1,1});
      for (const id_t i : B.L[v].r) F[i].push_back(DT::Lit{v+1,-1});
    }
    return {dp, F};
  }
  Bcc_frame CNF2bcc(const DT::DimacsClauseList& F) {
    const auto n = F.first.n;
    assert(F.first.c == F.second.size());
    Bcc_frame B(n);
    for (id_t i = 0; i < F.first.c; ++i)
      for (const DT::Lit x : F.second[i]) {
        const auto v = x.v.v;
        assert(1 <= v and v <= n);
        if (x.s) B.L[v-1].l.push_back(i);
        else B.L[v-1].r.push_back(i);
      }
    return B;
  }


  void sort(bc_frame& b) noexcept {
    std::ranges::sort(b.l); std::ranges::sort(b.r);
  }
  void sort(Bcc_frame& B) noexcept {
    for (bc_frame& b : B.L) sort(b);
  }

  // Returns the number of eliminated bicliques:
  id_t triv_trim(Bcc_frame& B) noexcept {
    const auto old_size = B.L.size();
    std::erase_if(B.L, [](const auto& b){return b.no_edge();});
    return old_size - B.L.size();
  }


  /*
    Trimming: removing vertices from bicliques, when all their edges
    are covered elsewhere.

    When going through the vertices of a single biclique, the order
    does not matter -- however the order of the bicliques matter, so
    running through various permutations could yield a better result.

    Returns the number of reduced literal-occurrences.
  */
  id_t trim(Bcc_frame& B) {
    assert(valid2(B));
    const id_t orig = numocc(B);
    triv_trim(B);
    using edge_t = Graphs::AdjVecUInt::edge_t;
    const auto e = [](id_t v, id_t w){
      if (v<=w) return edge_t{v,w}; else return edge_t{w,v};
    };
    std::map<edge_t, id_t> M;
    for (const auto& b : B.L)
      for (const auto v : b.l)
        for (const auto w : b.r)
          ++M[e(v,w)];

    for (bc_frame& b : B.L) {
      {std::vector<id_t> reml;
       for (const id_t v : b.l) {
         const auto t = [&e,&M,&v](id_t w){return M[e(v,w)] >= 2;};
         if (std::all_of(b.r.begin(), b.r.end(), t)) {
           reml.push_back(v);
           for (const auto w : b.r) --M[e(v,w)];
         }
       }
       assert(reml.size() <= b.l.size());
       std::vector<id_t> diff; diff.reserve(b.l.size() - reml.size());
       std::ranges::set_difference(b.l, reml, std::back_inserter(diff));
       b.l = std::move(diff);
      }
      {std::vector<id_t> remr;
       for (const id_t w : b.r) {
         const auto t = [&e,&M,&w](id_t v){return M[e(v,w)] >= 2;};
         if (std::all_of(b.l.begin(), b.l.end(), t)) {
           remr.push_back(w);
           for (const auto v : b.l) --M[e(v,w)];
         }
       }
       assert(remr.size() <= b.r.size());
       std::vector<id_t> diff; diff.reserve(b.r.size() - remr.size());
       std::ranges::set_difference(b.r, remr, std::back_inserter(diff));
       b.r = std::move(diff);
      }
    }

    triv_trim(B);
    return orig - numocc(B);
  }


  // Whether edges e1, e2 can be in the same biclique
  // ("biclique-compatibility"):
  inline bool bccomp(const edge_t e1, const edge_t e2, const AdjVecUInt& G) noexcept {
    const auto [a,b] = e1;
    const auto [c,d] = e2;
    if (c==a or c==b or d==a or d==b) return true;
    return
      (G.adjacent(c,a) and G.adjacent(d,b)) or
      (G.adjacent(c,b) and G.adjacent(d,a));
  }

}

#endif
