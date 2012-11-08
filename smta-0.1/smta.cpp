#include "smta.h"


using namespace std;
using namespace z3;
using namespace CVC3;


crope SMTLibAdaptor::query(crope queryCrope)
{
  //return queryCVC3(queryCrope);
  return queryZ3(queryCrope);
}

crope SMTLibAdaptor::queryCVC3(crope queryCrope)
{
  crope answerCrope;

  // save cerr streambuf and set it to a dummy streambuf
  streambuf *cerrbuf = cerr.rdbuf(new stringbuf(ios_base::out));

  CLFlags flags = ValidityChecker::createFlags();
  ValidityChecker* vc = ValidityChecker::create(flags);
  vc->setTimeLimit(SMTA_TIMEOUT / 100); // not in milliseconds

  vc->cmdsFromString(string(queryCrope.c_str()), SMTLIB_V2_LANG);
  switch(vc->query(vc->falseExpr())) {
    case UNSATISFIABLE:
      answerCrope = "unsat";
      break;
    case SATISFIABLE:
      answerCrope = "sat";
      break;
    default:
      answerCrope = "unknown";
  }

  // restore the cerr streambuf and delete the dummy streambuf
  delete cerr.rdbuf(cerrbuf);

  return answerCrope;
}

crope SMTLibAdaptor::queryZ3(crope queryCrope)
{
  crope answerCrope;

  context c;
  solver s(c);
  params p(c);
  p.set(":timeout", static_cast<unsigned>(SMTA_TIMEOUT)); // in milliseconds
  s.set(p);

  Z3_context ctx = Z3_context(c);
  Z3_string str = queryCrope.c_str();
  Z3_ast ast = Z3_parse_smtlib2_string(ctx, str, 0, NULL, NULL, 0, NULL, NULL);
  expr e(c, ast);
  s.add(e);

  switch (s.check()) {
    case unsat:
      answerCrope = "unsat";
      break;
    case sat:
      answerCrope = "sat";
      break;
    default:
      answerCrope = "unknown";
  }  

  return answerCrope;
}

