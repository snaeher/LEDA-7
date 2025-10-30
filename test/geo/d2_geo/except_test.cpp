/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  except_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/circle.h>

using namespace leda;

using std::cout;
using std::endl;


int main()
{
 // enable exceptions 
 set_error_handler(exception_error_handler);

 point p1(0,0);
 point p2(1,1);
 point p3(2,2);
 
 circle C(p1,p2,p3);
 point p;
 
 try {
  p =  C.center();
 }

 catch(leda_exception e) {
  cout << endl;
  cout << "An exception occured: " << e.get_msg() << endl;
  cout << endl;
  C = circle(point(0,0), point(1,1), point(1,0));
  p =  C.center();
 }
 
 cout << "center of circle: " << p << endl;
 
 return 0;
}
