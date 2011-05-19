#include "smta.h"


using namespace std;
using namespace CVC3;


crope SMTLibAdaptor::query(crope queryCrope)
{
  return queryCVC3(queryCrope);
}

crope SMTLibAdaptor::queryCVC3(crope queryCrope)
{
  crope answer;

  // save cerr streambuf and set it to a dummy streambuf
  streambuf *cerrbuf = cerr.rdbuf(new stringbuf(ios_base::out));

  CLFlags flags = ValidityChecker::createFlags();
  ValidityChecker* vc = ValidityChecker::create(flags);
  vc->setTimeLimit(SMTA_TIMEOUT);

  vc->cmdsFromString(string(queryCrope.c_str()), SMTLIB_V2_LANG);
  switch(vc->query(vc->falseExpr())) {
    case UNSATISFIABLE:
      answer = "unsat";
      break;
    case SATISFIABLE:
      answer = "sat";
      break;
    default:
      answer = "unknown";
  }

  // restore the cerr streambuf and delete the dummy streambuf
  delete cerr.rdbuf(cerrbuf);

  return answer;
}

crope SMTLibAdaptor::queryZ3(crope queryCrope)
{
  return "unknown";
}

