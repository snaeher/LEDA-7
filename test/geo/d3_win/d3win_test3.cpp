/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3win_test3.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/d3_window_stream.h>

using namespace leda;

int main()
{
 d3_window_stream DW(600,600,"Test");

 DW.set_color(red);
 
 d3_point p1(0,0,0),p2(50,60,80),p3(-20,10,30),p4(-10,-10,-30);
 d3_sphere S1(p1,p2,p3,p4);
 
 DW << S1;
 
 DW.show();
 
 d3_point p5(-40,-40,-35);
 d3_sphere S2(p1,p2,p3,p5);
 
 DW.set_color(blue);
 DW << S2;
 
 DW.show();
 
 return 0;
}
