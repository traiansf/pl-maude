#include<string>
#include<iostream>
#include<fstream>

//#include<vc.h>
//#include<command_line_flags.h>

#include<z3++.h>

#include"smta.h"


using namespace std;
using namespace z3;
//using namespace CVC3;


__gnu_cxx::crope SMTLibAdaptor::query(__gnu_cxx::crope queryCrope)
{
  //return queryCVC3(queryCrope);
  return queryZ3(queryCrope);
}
  
__gnu_cxx::crope SMTLibAdaptor::queryCVC3(__gnu_cxx::crope queryCrope)
{
  __gnu_cxx::crope answerCrope;

//  // save cerr streambuf and set it to a dummy streambuf
//  streambuf *cerrbuf = cerr.rdbuf(new stringbuf(ios_base::out));
//
//  CLFlags flags = ValidityChecker::createFlags();
//  ValidityChecker* vc = ValidityChecker::create(flags);
//  vc->setTimeLimit(SMTA_TIMEOUT / 100); // not in milliseconds
//
//  vc->cmdsFromString(string(queryCrope.c_str()), SMTLIB_V2_LANG);
//  switch(vc->query(vc->falseExpr())) {
//    case UNSATISFIABLE:
//      answerCrope = "unsat";
//      break;
//    case SATISFIABLE:
//      answerCrope = "sat";
//      break;
//    default:
//      answerCrope = "unknown";
//  }
//
//  // restore the cerr streambuf and delete the dummy streambuf
//  delete cerr.rdbuf(cerrbuf);

  return answerCrope;
}

__gnu_cxx::crope SMTLibAdaptor::queryZ3(__gnu_cxx::crope queryCrope)
{
  __gnu_cxx::crope answerCrope;

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

