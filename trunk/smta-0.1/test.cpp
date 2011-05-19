#include "smta.h"


using namespace std;


int main()
{
  crope queryCrope;

  queryCrope = "";
  queryCrope += "(set-logic AUFNIRA)";
  queryCrope += "(declare-fun x () Int)";
  queryCrope += "(assert (< x 0))";
  cout << queryCrope << endl;
  cout << SMTLibAdaptor::query(queryCrope) << endl << "===" << endl;

  queryCrope = "";
  queryCrope += "(set-logic AUFNIRA)";
  queryCrope += "(declare-fun x () Int)";
  queryCrope += "(assert (< (* x x) 0))";
  cout << queryCrope << endl;
  cout << SMTLibAdaptor::query(queryCrope) << endl << "===" << endl;
 
  return 0;
}

