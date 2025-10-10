/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _logo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/geo/circle.h>
#include <LEDA/geo/line.h>
#include <LEDA/geo/segment.h>
#include <LEDA/geo/logo.h>
#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE 

static void poly_arc(point p, point q, point c, int refine, list<point>& pol)
{ segment s0(c,p);
  segment s1(c,q);
  double delta = LEDA_PI/refine;
  double r  = s0.length();
  double a0 = s0.angle();
  double a1 = s1.angle();
  if (a0 < a1)
    for(double a=a0; a < a1; a+=delta) 
    { point p = c.translate_by_angle(a,r);
      pol.append(point(truncate(p.xcoord(),10),truncate(p.ycoord(),10)));
    }
  else
    for(double a=a0; a > a1; a-=delta) 
    { point p = c.translate_by_angle(a,r);
      pol.append(point(truncate(p.xcoord(),10),truncate(p.ycoord(),10)));
     }

  assert(!pol.empty());
 }


gen_polygon leda_logo(double sz)
{
  int DIST   = 32;
  int THICK  = 18;
  int REFINE = 100;

   list<polygon> plist;

   double d    = 0.01*THICK*sz;
   double d1   = 0.95 * d;

   double eps  = 0.001*DIST*sz;
   double eps2 = 1.75*eps;

   double R0 = sqrt(5.0)*sz;
   double R1 = R0 - d;

   circle c1(0,-sz,R0);
   circle c2(0,-sz,R1);
   circle c3(0,0,sz);
   circle c4(0,0,sz-d);

   point A,B,C,D,E,F,G,H,I,J;
   segment s0,s1,s2;

   list<point> pol;

   // L

 //s0 = segment(-sz-eps2,0,-sz-eps2,2);
   s0 = segment(-sz-eps2,0,-sz-eps2,1000);

   s1 = segment(-2*sz,d,0,d);

   A = c1.intersection(s0).head(); 
   B = c2.intersection(s0).head(); 
   C = c2.intersection(s1).head(); 
   D = point(-sz-eps2,d);
   E = point(-sz-eps2,0);
   F = point(-2*sz,0);

   pol.append(A);
   //pol.append(B);
   poly_arc(B,C,point(0,-sz),REFINE,pol);
   pol.append(C);
   pol.append(D);
   pol.append(E);
   //pol.append(F);
   poly_arc(F,A,point(0,-sz),REFINE,pol);
   
   plist.append(polygon(pol));


   // E

   double sp = (sz - 2*d - d1)/2;
   double sp1 = 0.9*sp;

   s0 = segment(0,d+sp1+d1,-2*sz,d+sp1+d1);
   s1 = segment(0,d+sp1,-2*sz,d+sp1);
   s2 = segment(0,d,-2*sz,d);

   A = point(-eps,sz);
   B = point(-eps,sz-d);
   C = c4.intersection(s0).head();
   D = point(-eps,C.ycoord());
   E = point(-eps,C.ycoord()-d1);
   F = c4.intersection(s1).head();
   G = c4.intersection(s2).head();
   H = point(-eps,G.ycoord());
   I = point(-eps,0);
   J = point(-sz,0);

   pol.clear();

   pol.append(A);
   //pol.append(B);
   poly_arc(B,C,point(0,0),REFINE,pol);
   pol.append(C);
   pol.append(D);
   pol.append(E);
   //pol.append(F);
   poly_arc(F,G,point(0,0),REFINE,pol);
   pol.append(G);
   pol.append(H);
   pol.append(I);
   //pol.append(J);
   poly_arc(J,A,point(0,0),REFINE,pol);

   plist.append(polygon(pol));


   // D

   s0 = segment(0,d,2*sz,d);
   s1 = segment(d+eps,0,d+eps,2*sz);

   A = c4.intersection(s0).head();
   B = c4.intersection(s1).head();
   C = point(d+eps,d);

   D = point(sz,0);
   E = point(eps,sz);
   F = point(eps,0);

   pol.clear();
   //pol.append(D);
   poly_arc(D,E,point(0,0),REFINE,pol);
   pol.append(E);
   pol.append(F);

   plist.append(polygon(pol).complement());

   pol.clear();
   //pol.append(A);
   poly_arc(A,B,point(0,0),REFINE,pol);
   pol.append(B);
   pol.append(C);
   plist.append(polygon(pol));


   // A

 //s0 = segment(sz+eps2,0,sz+eps2,2);
   s0 = segment(sz+eps2,0,sz+eps2,1000);

   s1 = segment(0,d,2*sz,d);

   A = point(2*sz,0);
   B = c1.intersection(s0).head();
   C = point(sz+eps2,0);
   D = point(sz+eps2+d,0);
   E = point(sz+eps2+d,d);
   F = c2.intersection(s1).head();
   s1 = segment(0,0,2*sz,0);
   G = c2.intersection(s1).head();

   pol.clear();
   //pol.append(A);
   poly_arc(A,B,point(0,-sz),REFINE,pol);
   pol.append(B);
   pol.append(C);
   pol.append(D);
   pol.append(E);
   //pol.append(F);
   poly_arc(F,G,point(0,-sz),REFINE,pol);
   pol.append(G);
  

   plist.append(polygon(pol).complement());

   s0 = segment(0,d+d1,2*sz,d+d1);
   s1 = segment(sz+eps2+d,0,sz+eps2+d,2*sz);

   A = c2.intersection(s0).head();
   B = c2.intersection(s1).head();
   C = point(sz+eps2+d,d+d1);

   pol.clear();
   //pol.append(A);
   poly_arc(A,B,point(0,-sz),REFINE,pol);
   pol.append(B);
   pol.append(C);

   plist.append(polygon(pol));

   // bounding box

   pol.clear();
   pol.append(point(-2*sz-d, -d));
   pol.append(point( 2*sz+d, -d));
   pol.append(point( 2*sz+d,sz+d));
   pol.append(point(-2*sz-d,sz+d));
   plist.push(polygon(pol));

   return gen_polygon(plist);
}

LEDA_END_NAMESPACE 
