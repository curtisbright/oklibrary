// Oliver Kullmann, 1..3.2002 (Swansea)

// PartAssign.hpp

#ifndef WAECHTERPARTASSIGN

#include <map>

#include "VarSet.hpp"
#include "Literal.hpp"
#include "Clause.hpp"
#include "ClauseSet.hpp"

namespace PartAssignments {

  using namespace Variables;
  using namespace Literals;
  using namespace Clauses;
  using namespace Clausesets;

  namespace Error {

    struct not_in_domain {};
    struct inconsistent_extension {};

  }

  class Pass {

  public :

    Pass () {}
    Pass (Lit);
    Pass (const Cl&);

    bool is_empty() const;

    set_length n() const;

    set<Var> var() const;

    bool in_domain(Var) const;
    bool val(Var) const;

    Pass& composition_left(const Pass&);
    // Attention: the argument is composed from the *left*

    Pass& restrict(const set<Var>&);

    Cl clause() const;

    Cls operator * (const Cls&) const;

    map<Var, bool> pa;

  };

  inline Pass::Pass(const Lit x) {
    pa = map<Var, bool>();
    pa[x.var()] = x.val();
  }

  inline Pass::Pass (const Cl& C) {
    pa = map<Var, bool>();
    for (set<Lit>::const_iterator i = C.ls.begin(); i != C.ls.end(); i++)
      pa[i -> var()] = i -> val();
  }

  inline bool Pass::is_empty() const {
    return pa.empty();
  }

  inline set<Var> Pass::var() const {
    set<Var> V;
    for (map<Var,bool>::const_iterator i = pa.begin(); i != pa.end(); i++)
      V.insert(i -> first);
    return V;
  }

  inline set_length Pass::n() const {
    return pa.size();
  }

  inline bool Pass::in_domain(const Var v) const {
    return pa.find(v) != pa.end();
  }

  inline bool Pass::val(const Var v) const {
    map<Var,bool>::const_iterator p = pa.find(v);
    if (p == pa.end())
      throw Error::not_in_domain();
    return p -> second;
  }

  inline Pass& Pass::composition_left(const Pass& phi) {
    pa.insert(phi.pa.begin(), phi.pa.end());
    return *this;
  }

  inline Pass& Pass::restrict(const set<Var>& V) {
    for (set<Var>::const_iterator i = V.begin(); i != V.end(); i++)
      pa.erase(*i);
    return *this;
  }

  inline Cl Pass::clause() const {
    Cl C;
    for (map<Var,bool>::const_iterator i = pa.begin(); i != pa.end(); i++)
      C.ls.insert(Lit(i -> first, i -> second));
    return C;
  }

  inline Cls Pass::operator * (const Cls& F) const {
    Cls G;
    for (set<Cl>::iterator i = F.cls.begin(); i != F.cls.end(); i++)
      if ((*i & this -> clause().comp()).is_empty())
	G.add(*i - this -> clause());
    return G;
  }

}

#endif

#define WAECHTERPARTASSIGN

