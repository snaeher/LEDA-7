/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3_delau_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/misc.h>

using namespace leda;

#include "d3_complex.h"

using std::cout;
using std::endl;


void generate_input(int n, int input, int maxc, list<d3_rat_point>& L)
{ 
  L.clear();

  switch (input) {
   case 0: random_points_in_cube(n,3*maxc/4,L); break;
   case 1: random_points_in_ball(n,maxc,L); break;
   case 2: random_points_in_square(n,maxc,L); break;
   case 3: random_points_on_paraboloid(n,maxc,L); break;
   case 4: lattice_points(n,3*maxc/4,L); break;
   case 5: random_d3_rat_points_on_sphere(n,maxc,L);  break;
   case 6: random_points_on_segment(n,maxc,L); break;
  }
}

using std::cin;
using std::cout;
using std::endl;

main()
{
  d3_simpl_complex C;
  GRAPH<d3_rat_point,int> G;
  GRAPH<d3_rat_point,int> G2;

  int testfall=0; // cube
  int lz; 
  int pz; 
  int a,wt;
  bool wert;
  float T;
  list<d3_rat_point> input;

  cout << "Test runs:"; cin >> lz;
  cout << "Points   :"; cin >> pz;

  for (a=0;a<lz;a++){
     generate_input(pz,0,1000,input);
     C.clear();
     T=used_time();
     D3_DELAU(input,C);
     //D3_DELAU(input,G);
     //D3_TRIANG(input,G2);
     
     cout << "Time:" << used_time(T) << " sec.!\n"; 

     //checking
     //wt=C.check(1,cout);
     //if (wt==0) cout << "Check ok\n"; else cout << "Check not ok\n";

     wert=C.delaunay_check(1); 
     cout << a << "-";
     if (wert) cout << "OK!\n "; else cout << "error!\n ";
    
     /*
     list<sc_simplex> SL= C.all_simplices();
     cout << SL.length() << " simplices!\n";
     sc_simplex akt;
     forall(akt,SL){
       wert=C.global_delaunay(akt); cout << wert << " ";
     } 
     */
  }
  
}
