/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  intersect_plane.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/geo/d3_plane.h>
#include <LEDA/geo/d3_rat_plane.h>
#include <LEDA/numbers/vector.h>
#include <LEDA/numbers/vector.h>
//#include <LEDA/random_d3_rat_point.h>

using namespace leda;

using std::cout;
using std::cin;
using std::endl;



void get_para(d3_plane& pl,double& A,double& B,double& C,double& D)
{
 vector n=pl.normal();
 A=n.xcoord();
 B=n.ycoord();
 C=n.zcoord();
 d3_point onpl=pl.point1();

 D=-A*onpl.xcoord()-B*onpl.ycoord()-C*onpl.zcoord(); 
}

void get_para(d3_rat_plane& pl,integer& A,integer& B,integer& C,integer& D)
{
 rat_vector n=pl.normal();
 d3_rat_point onpl=pl.point1();
 integer wt=onpl.W();

 A=n.hcoord(0)*wt;
 B=n.hcoord(1)*wt;
 C=n.hcoord(2)*wt;

 D=-A*onpl.X()-B*onpl.Y()-C*onpl.Z(); 
}

int intersect(d3_plane& pl, d3_point po1, d3_point po2, d3_point& cutpt)
// 0 - Line through po1,po2 parallel
// 1- Line through po1,po2 intersects pl in a single point
// 2- Line is in the plane
{
 double A,B,C,D;
 double x1,y1,z1;
 double l,m,n;

 get_para(pl,A,B,C,D);

 x1=po1.xcoord(); y1=po1.ycoord(); z1=po1.zcoord();

 l=po2.xcoord()-po1.xcoord();
 m=po2.ycoord()-po1.ycoord();
 n=po2.zcoord()-po1.zcoord();

 double wert1,wert2;

 wert1=A*l+B*m+C*n;
 wert2=A*x1+B*y1+C*z1+D;
 //cout << "wert1:" << wert1 << " wert2:" << wert2 << "\n";

 if (wert1==0){ //parallel or in the plane ...
   if (wert2==0) return 2; // in the plane...
   else return 0; //parallel
 }

 // intersection in a single point ...
 double h= wert2/wert1;
 double xs,ys,zs;

 xs=x1-l*h;
 ys=y1-m*h;
 zs=z1-n*h;

 cutpt=d3_point(xs,ys,zs);
 return 1;
}

int intersect(d3_rat_plane& pl, d3_rat_point po1, d3_rat_point po2, d3_rat_point& cutpt)
// 0 - Line through po1,po2 parallel
// 1- Line through po1,po2 intersects pl in a single point
// 2- Line is in the plane
{
 integer A,B,C,D;
 integer x1,y1,z1;
 integer l,m,n;

 get_para(pl,A,B,C,D);

 x1=po1.X(); y1=po1.Y(); z1=po1.Z();

 l=po2.X()*po1.W() - po1.X()*po2.W();
 m=po2.Y()*po1.W() - po1.Y()*po2.W();
 n=po2.Z()*po1.W() - po1.Z()*po2.W();

 integer wert1,wert2;
 integer fkt= po1.W()*po2.W();

 wert1=A*l+B*m+C*n; 
 wert2=(A*x1+B*y1+C*z1+D)*fkt;
 //cout << "wert1:" << wert1 << " wert2:" << wert2 << "\n";

 if (wert1==0){ //parallel or in the plane ...
   if (wert2==0) return 2; // in the plane...
   else return 0; //parallel
 }

 // intersection in a single point ...
 integer xzl,yzl,zzl;
 integer nn;
 integer f2=fkt*wert1;

 xzl=x1*f2-l*wert2;
 yzl=y1*f2-m*wert2;
 zzl=z1*f2-n*wert2;
 nn =fkt*wert1;

 cutpt=d3_rat_point(xzl,yzl,zzl,nn);
 return 1;
}



int main(int argc, char* argv[])
{
 d3_point pl1(0,0,0),pl2(1,0,0),pl3(0,1,0);
 d3_plane E(pl1,pl2,pl3);
 d3_point S1,S2;
 d3_point ct;
 int wert,wert2;

 /*
 cout << "Floating point:\n";
 cout << "---------------\n";
 cout << "Punkte eingeben:";
 cin >> S1; cin >> S2;

 wert=intersect(E,S1,S2,ct);
 cout << wert << "\n"; cout << ct << "\n";
 cout << "coplanar? " << coplanar(ct,pl1,pl2,pl3) << "\n";
 wert=intersect(E,S2,S1,ct);
 cout << wert << "\n"; cout << ct << "\n";
 cout << "coplanar? " << coplanar(ct,pl1,pl2,pl3) << "\n";
 */

 d3_rat_point R1,R2,R3;
 if (argc <= 1) {
   cout << "Rational:\n";
   cout << "---------\n";
   cout << "3 Punkte fuer die Ebene eingeben:";
   cin >> R1; cin >> R2; cin >> R3;
 }
 else {
   R1 = pl1; R2 = pl2; R3 = pl3;
 }

 d3_rat_plane ER(R1,R2,R3);

 //checking
 int i;
 d3_rat_point pr1,pr2,ctr1,ctr2;

 if (argc <= 1) {
   cout << "Punkt1:"; cin >> pr1;
   cout << "Punkt2:"; cin >> pr2;
 }
 else {
   pr1 = d3_point(1,1,0); pr2 = d3_point(1,1,1);
 }

 wert=intersect(ER,pr1,pr2,ctr1); 
 cout << wert << " " << ctr1 << "\n";


 for (i=0;i<100000;i++){
   pr1=random_d3_rat_point_in_cube(200);
   pr2=random_d3_rat_point_in_cube(200);
   wert=intersect(ER,pr1,pr2,ctr1);
   wert2=intersect(ER,pr2,pr1,ctr2);

   if (wert!=wert2 || ctr1!=ctr2) cout << "Fehler!\n";
   if (wert==2) {
     if (! (coplanar(R1,R2,R3,pr1) && coplanar(R1,R2,R3,pr2))) cout << "Fehler!\n";
   }
   if (wert==1) {
     if ( ! (collinear(pr1,pr2,ctr1) && coplanar(R1,R2,R3,ctr1))) cout << "Fehler!\n";
   }
 }
return 0;
}






