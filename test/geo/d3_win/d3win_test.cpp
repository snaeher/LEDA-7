/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3win_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/d3_window_stream.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


int main()
{
 d3_window_stream DW("Test");
 d3_point p(100,200,100);
 d3_point p1(80,50,10),p2(120,130,15);
 d3_line seg(p1,p2);
 d3_plane pl(p,p1,p2);

 DW.set_color(red);
 
 DW << p;
 DW << seg;
 DW << pl;
 
 DW.set_color(grey2);
 list<node> LN;
 list<edge> LE;
 
 d3_point pb1(0,0,0), pb2(200,300,100);
 
 d3_window_item i_d3;
 DW.insert_box(pb1,pb2,i_d3);

 // d3 simplex ...
 d3_point ps1(20,30,30),ps2(70,10,-10);
 d3_point ps3(50,10,100),ps4(60,100,30);
 d3_simplex sim(ps1,ps2,ps3,ps4);
 
 DW.set_color(red);
 DW << sim;
 
 // d3 sphere ...
 vector vec(90,40,30);
 d3_sphere sph(ps1+vec, ps2+vec, ps3+vec, ps4+vec);
 
 DW.set_color(green2);
 //DW << sph;

 // d2 types ...
 point pa(100,100), pb(200,200);
 segment s(pa,pb);
 ray  ri(pa,pb);
 
 DW.set_color(black);
 DW << pa << pb;
 DW << s;
 DW << ri;

 list<point> PL;
 point pc(500,700), pd(0,400);
 PL.append(pa); PL.append(pb); PL.append(pc); PL.append(pd);
 
 DW << polygon(PL);
 
 point pc1(200,200), pc2(500,200), pc3(200,500);
 circle circ(pc1,pc2,pc3);
 
 DW.set_color(blue);
 polygon pol = hilbert(3,-200,-200,400,400);
 DW << pol;
 
 DW.set_color(green);
 DW << circ;
 
 DW.set_color(blue2);
 rectangle rect(-500,-400,-100,-30);
 DW << rect;
 
 triangle Tr(-500,-400,-300,-300, -400,-200);
 DW << Tr;
 
 bool b = DW.show();
 cout << b << "\n";

 return 0;
}





