// Oliver Kullmann, 11.6.2005 (Swansea)

/*!
  \file Concepts/ResultElements.hpp
  \brief Concepts for result elements, representing results of SAT solvers (especially
  for competitions).

  \todo floating_point_type.hpp and natural_number_type.hpp should go to
  SATCompetition/Traits.
  \todo To be completed (see module SAT-Competition).
*/

#ifndef RESULTELEMENT_6Tg5Yh
#define RESULTELEMENT_6Tg5Yh

#include <Transitional/Concepts/LibraryBasics.hpp>

#include <Transitional/SATCompetition/traits/string_type.hpp>
#include <Transitional/SATCompetition/traits/floating_point_type.hpp>
#include <Transitional/SATCompetition/traits/natural_number_type.hpp>

namespace OKlib {

  namespace Concepts {

    /*!
      \class ResultElement
      \brief Root class for the concept hierarchy about result elements (requiring only types).

      Requires string_type, floating_point_type, natural_number_type.
    */

    struct ResultElement_tag : virtual BasicRequirements_tag, virtual FullyConstructible_tag {};

    template <typename Res>
    struct ResultElement {
      typedef typename OKlib::SATCompetition::traits::string_type<Res>::type string_type;
      typedef typename OKlib::SATCompetition::traits::floating_point_type<Res>::type floating_point_type;
      typedef typename OKlib::SATCompetition::traits::natural_number_type<Res>::type natural_number_type;
      void constraints() {
        OKLIB_MODELS_CONCEPT_TAG(Res, ResultElement);

        OKLIB_MODELS_CONCEPT_REQUIRES(Res, BasicRequirements);
        OKLIB_MODELS_CONCEPT_TAG(Res, BasicRequirements);
        OKLIB_MODELS_CONCEPT_REQUIRES(Res, FullyConstructible);
        OKLIB_MODELS_CONCEPT_TAG(Res, FullyConstructible);
      }
    };

    class ResultElement_Archetype {
      struct X {};
      struct Y {};
      struct Z {};
    public :
      typedef ResultElement_tag concept_tag;
      typedef X string_type;
      typedef Y floating_point_type;
      typedef Z natural_number_type;
    };
    
    // --------------------------------------------------------------------------------------------------------------------------------------

    /*!
      \class ResultElementWithOrder
      \brief Refines concept ResultElement by requiring a linear order on result elements.
    */

    struct ResultElementWithOrder_tag : virtual ResultElement_tag, virtual EqualitySubstitutable_tag, virtual LinearOrder_tag {};

    template <typename Res>
    struct ResultElementWithOrder {
      void constraints() {
        OKLIB_MODELS_CONCEPT_TAG(Res, ResultElementWithOrder);

        OKLIB_MODELS_CONCEPT_REQUIRES(Res, ResultElement);
        OKLIB_MODELS_CONCEPT_TAG(Res, ResultElement);
        OKLIB_MODELS_CONCEPT_REQUIRES(Res, EqualitySubstitutable);
        OKLIB_MODELS_CONCEPT_TAG(Res, EqualitySubstitutable);
        OKLIB_MODELS_CONCEPT_REQUIRES(Res, LinearOrder);
        OKLIB_MODELS_CONCEPT_TAG(Res, LinearOrder);
      }
    };

    class ResultElementWithOrder_Archetype : public ResultElement_Archetype {
    protected :
      struct convertible_to_bool {
        operator bool() { return bool(); }
      };
    public :
      typedef ResultElementWithOrder_tag concept_tag;
      convertible_to_bool operator ==(const ResultElementWithOrder_Archetype&) const { return convertible_to_bool(); }
      convertible_to_bool operator !=(const ResultElementWithOrder_Archetype&) const { return convertible_to_bool(); }
      convertible_to_bool operator <(const ResultElementWithOrder_Archetype&) const { return convertible_to_bool(); }
      convertible_to_bool operator >(const ResultElementWithOrder_Archetype&) const { return convertible_to_bool(); }
      convertible_to_bool operator <=(const ResultElementWithOrder_Archetype&) const { return convertible_to_bool(); }
      convertible_to_bool operator >=(const ResultElementWithOrder_Archetype&) const { return convertible_to_bool(); }
    };
    
    // --------------------------------------------------------------------------------------------------------------------------------------

    /*!
      \class ResultElementWithName
      \brief Refines concept ResultElementWithOrder by adding the possibility to
      construct result elements with a given name, and to query this name.
    */

    struct ResultElementWithName_tag : virtual ResultElementWithOrder_tag {};

    template <typename Res>
    struct ResultElementWithName {
      typedef typename ::OKlib::SATCompetition::traits::string_type<Res>::type string_type;
      void constraints() {
        OKLIB_MODELS_CONCEPT_TAG(Res, ResultElementWithName);

        OKLIB_MODELS_CONCEPT_REQUIRES(Res, ResultElementWithOrder);
        OKLIB_MODELS_CONCEPT_TAG(Res, ResultElementWithOrder);

        OKLIB_MODELS_CONCEPT_REQUIRES(string_type, ConstCorrect);
        OKLIB_MODELS_CONCEPT_REQUIRES(string_type, EqualitySubstitutable);
        OKLIB_MODELS_CONCEPT_REQUIRES(string_type, LinearOrder);
        OKLIB_MODELS_CONCEPT_REQUIRES(string_type, FullyConstructible);

        Res(s);
        Res(sc);
        static_cast<string_type>(r.name());
        static_cast<string_type>(rc.name());
      }
      string_type s; const string_type sc;
      Res r; const Res rc;
    };

    class ResultElementWithName_Archetype : public ResultElementWithOrder_Archetype {
    public :
      struct string_type {
    protected :
        struct convertible_to_bool {
          operator bool() { return bool(); }
        };
      public :
        convertible_to_bool operator ==(const string_type&) const { return convertible_to_bool(); }
        convertible_to_bool operator !=(const string_type&) const { return convertible_to_bool(); }
        convertible_to_bool operator <(const string_type&) const { return convertible_to_bool(); }
        convertible_to_bool operator >(const string_type&) const { return convertible_to_bool(); }
        convertible_to_bool operator <=(const string_type&) const { return convertible_to_bool(); }
        convertible_to_bool operator >=(const string_type&) const { return convertible_to_bool(); }
      };
      typedef ResultElementWithName_tag concept_tag;
      ResultElementWithName_Archetype() {}
      ResultElementWithName_Archetype(const string_type&) {}
      string_type name() const { return string_type(); }
    };
    
    // --------------------------------------------------------------------------------------------------------------------------------------

    
  }

}

#endif
