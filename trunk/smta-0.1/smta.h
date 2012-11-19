#ifndef _SMTA_hh_
#define _SMTA_hh_

#include<ext/rope>


#define SMTA_TIMEOUT 5000 // in milliseconds

class SMTLibAdaptor {
  public:
    static __gnu_cxx::crope query(__gnu_cxx::crope queryCrope);
  private:
    static __gnu_cxx::crope queryCVC3(__gnu_cxx::crope queryCrope);
    static __gnu_cxx::crope queryZ3(__gnu_cxx::crope queryCrope);
};

#endif

