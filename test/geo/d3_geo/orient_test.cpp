/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  orient_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_rat_point.h>
#include <LEDA/numbers/matrix.h>

using namespace leda;

using std::cout;
using std::endl;



double  AD[16] = { 1.0, 1.0, 1.0, 1.0,
                   0.0, 1.0, 0.0, 0.0,
                   0.0, 0.0, 1.0, 0.0,
                   0.0, 0.0, 0.0, 1.0 };


random_source& operator>>(random_source& ran, d3_rat_point& p)
{ int x,y,z,w;
  ran >> x >> y >> z >> w;
  if (w == 0) w = 1;
  p = d3_rat_point(x,y,z,w);
  return ran;
}


int main(int argc, char* argv[])
{
  random_source ran(-50,+50);
  int count = 0;

  int n_max = 0;
  if (argc > 1) n_max = atoi(argv[1]);

  for(int n = 1; n != n_max; ++n)
  { 
    d3_rat_point a,b,c;
    ran >> a >> b >> c;

    cout << ++count << endl;

    if (collinear(a,b,c)) continue;

    d3_rat_point d = point_on_positive_side(a,b,c);

    if (orientation(a,b,c,d)  <= 0)
    { cout << a << endl;
      cout << b << endl;
      cout << c << endl;
      cout << d << endl;
      break;
     }
  }

/*
  matrix A(4,4,AD);
  cout << "det = " << A.det() <<endl;
*/

  return 0;
}

