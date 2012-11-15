#ifndef _SMTA_hh_
#define _SMTA_hh_

#include<string>
#include<ext/rope>
#include<iostream>
#include<fstream>

//#include<vc.h>
//#include<command_line_flags.h>

#include<z3++.h>

#define SMTA_TIMEOUT 5000 // in milliseconds

using namespace __gnu_cxx;

class SMTLibAdaptor {
  public:
    static crope query(crope queryCrope);
  private:
    static crope queryCVC3(crope queryCrope);
    static crope queryZ3(crope queryCrope);
};

#endif

