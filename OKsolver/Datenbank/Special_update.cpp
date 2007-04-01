// Oliver Kullmann, 28.6.2002 (Swansea)

/*!
  \file Transitional/OKsolver/Datenbank/Special_update.cpp
  \brief Some ad-hoc repair
  \deprecated Every development now takes place in Transitional/OKDatabase.
*/

#include <string>

#include "DatabaseHandler01.hpp"
#include "StringHandling.hpp"

using namespace std;

const string database = "OKRandGen";

const int start = 1400001;
const int end = 5000000;
const int jump = 1000;

const string update = "update bcls_info set an = (select an from b_measures where b_measures.b_info_id = bcls_info.b_info_id)";
const string counter_field = "b_info_id";

int main() {

  using namespace DatabaseHandler01;
  using namespace StringHandling;

  Connection Conn(database);
  Command C(Conn);

  const string update_query_0 = update + " where " + counter_field + " >= ";
  const string update_query_1 = " and " + counter_field + " < ";

  for (int i = start; i <= end; i += jump) {
    C.issue(update_query_0 + toString(i) + update_query_1 + toString(i + jump) + ";");
    if (C.changed_tuples() != jump)
      break;
  }

  return 0;

}


