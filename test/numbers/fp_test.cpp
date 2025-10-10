/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  fp_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/numbers/fp.h>
#include <assert.h>
#include <cfenv>

#ifndef NO_FPU
#include <LEDA/numbers/fpu.h>
#endif


using namespace leda;

using std::cout;
using std::cin;
using std::endl;

bool test_power()
{
  bool res = true;
  for(int i = -52; i<63; i++)
  {
    if( ldexp(1.0,i) != fp::power_two(i))
    {
      cout << "fp::power_two("<<i<<")="<<fp::power_two(i)<<endl;
      cout << "ldexp="<< ldexp(1.0,i)<<endl;
      res = false;
    }
  }
  return res;
}

#ifndef NO_FPU
bool test_fpu_rounding()
{
	bool ok = true;

	double d = fp::power_two(52);
	double eps = fp::power_two(-52);
	double r, x;

	//leda_fpu::round_nearest();
        std::fesetround(FE_TONEAREST);
	r = d + eps;
	x = r-d;
	if ( x != 0.0 ) {
          ok = false;
          cout << "round_nearest1: x =" << x << endl;
        }

	//leda_fpu::round_up();
        std::fesetround(FE_UPWARD);
	r = d + eps;
	x = r-d;
	if( x != 1.0) {
          ok = false;
          cout << "round_up: x = " << x << endl;
        }

	//leda_fpu::round_down();
        std::fesetround(FE_DOWNWARD);
	r = d -eps;
	x = r-d;
	if ( x != -0.5) {
          ok = false;
          cout << "round_down: x = " << x << endl;
        }

	//leda_fpu::round_nearest();
        //std::fesetround(FE_TONEAREST);
        std::fesetround(FE_UPWARD);
	r = d - eps;
	x = r-d;
	if ( x != 0.0 ) {
          ok = false;
          cout << "round_nearest2: x = " << x << endl;
        }

	return ok;
}
#endif


int main() {

  cout << "\nStart program fp_test.\n";
  assert( fp::selftest_okay() );
  assert( test_power() );
  assert( test_fpu_rounding() );
  cout << "... all tests successfully completed!\n";
  return 0;

}


