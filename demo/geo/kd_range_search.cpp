/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  kd_range_search.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:13 $

#include <LEDA/geo/point_set.h>
#include <LEDA/geo/point_set.h>
#include <LEDA/system/misc.h>

// point generation ...
#include <LEDA/geo/random_point.h>
#include <LEDA/geo/random_rat_point.h>

#include <LEDA/geo/point_set.h>
#include <LEDA/geo/point_set.h>

#include <LEDA/geo/d2_dictionary.h>

using namespace leda;



typedef static_point_set   KD_TREE_2;
typedef point_set   POINT_SET;
typedef line        LINE;
typedef segment     SEGMENT;
typedef point       POINT;
typedef rectangle   RECTANGLE;
typedef circle      CIRCLE;
typedef double      NUMB;

/*
typedef rat_static_point_set   KD_TREE_2;
typedef rat_point_set   POINT_SET;
typedef rat_line        LINE;
typedef rat_segment     SEGMENT;
typedef rat_point       POINT;
typedef rat_rectangle   RECTANGLE;
typedef rat_circle      CIRCLE;
typedef rational        NUMB;
*/

typedef d2_dictionary<NUMB,NUMB,bool> D2_DICTIONARY;


list<POINT> range_search_trivial(const list<POINT>& LP, POINT& p1, POINT& p2)
{
 RECTANGLE R(p1,p2);
 POINT piter;
 list<POINT> found;
 
 forall(piter, LP){
  if (! R.outside(piter)) {
   found.append(piter);
  }
 }
 
 return found;
}


int main(void)
{
 cout << "We compare rectangular range searches using small rectangles on static and dynamic point sets\n";
 cout << "and on a d2 dictionary with a trivial range search.\n";

 // generate a "big" point set ...
 list<POINT> LP;
 POINT piter;
 
 random_points_in_square(50000, 1000, LP);
 //LP.sort();
 //LP.unique();
 
 KD_TREE_2 T;
 
 float tm = used_time();
 T.init(LP);
 cout << "Time for building static point set:" << used_time(tm) << "\n";
 
 
 tm = used_time();
 POINT_SET S(LP);
 cout << "Time for building dynamic point set:" << used_time(tm) << "\n"; 
 
 tm = used_time();
 D2_DICTIONARY D;
 forall(piter,LP) D.insert(piter.xcoord(), piter.ycoord(), true);
 cout << "Time for building d2 dictionary:" << used_time(tm) << "\n";  
 
 // range searches ...
 list<POINT> CORNER1;
 list<POINT> CORNER2;
 
 cout << "\n5000 rectangular range searches:\n\n";
 
 random_points_in_square(5000, 1100, CORNER1); 
 //random_points_in_square(100, 1100, CORNER2);
 forall(piter, CORNER1) CORNER2.append(piter.translate(NUMB(10.0), NUMB(10.0)));

 list_item it1 = CORNER1.first(), it2 = CORNER2.first();
 
 tm = used_time();
 for(;it1 != NULL; it1 = CORNER1.succ(it1), it2 = CORNER2.succ(it2)){
   list<POINT> Lh = T.range_search(CORNER1[it1], CORNER2[it2]);
 }
 cout << "Time for rectangular range searches on static point set:" << used_time(tm) << "\n"; 
 cout << "\n\n";
 
 it1 = CORNER1.first(); it2 = CORNER2.first();
 
 tm = used_time();
 for(;it1 != NULL; it1 = CORNER1.succ(it1), it2 = CORNER2.succ(it2)){
   list<node> Lnh = S.range_search(CORNER1[it1], CORNER2[it2]);
 }
 cout << "Time for rectangular range searches on dynamic point set:" << used_time(tm) << "\n"; 

 cout << "\n\n";
 
 it1 = CORNER1.first(); it2 = CORNER2.first();
 
 tm = used_time();
 for(;it1 != NULL; it1 = CORNER1.succ(it1), it2 = CORNER2.succ(it2)){  
   NUMB x0,x1,y0,y1;
   if (POINT::cmp_x(CORNER1[it1], CORNER2[it2])) { x0 = CORNER1[it1].xcoord(); x1 = CORNER2[it2].xcoord(); }
   else { x1 = CORNER1[it1].xcoord(); x0 = CORNER2[it2].xcoord(); }
   if (POINT::cmp_y(CORNER1[it1], CORNER2[it2])) { y0 = CORNER1[it1].ycoord(); y1 = CORNER2[it2].ycoord(); }
   else { y1 = CORNER1[it1].ycoord(); y0 = CORNER2[it2].ycoord(); }
   
   list<dic2_item> Ld2 = D.range_search(x0,x1,y0,y1);
   //cout << Ld2.size() << "\n";
 }
 cout << "Time for D2_DICTIONARY rectangular range searches:" << used_time(tm) << "\n";   
 cout << "\n\n";
 
 it1 = CORNER1.first(); it2 = CORNER2.first();
 
 tm = used_time();
 for(;it1 != NULL; it1 = CORNER1.succ(it1), it2 = CORNER2.succ(it2)){
   list<POINT> Lh = range_search_trivial(LP, CORNER1[it1], CORNER2[it2]);
 }
 cout << "Time for trivial range searches:" << used_time(tm) << "\n"; 
 
 return 0;
}
