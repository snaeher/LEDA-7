/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  lookup_compare.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:14 $

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



void test_float_lookup(int number)
{
 cout << "The static and dynamic point set and d2 dictionary store " << number << " points.\n";

 list<point> LP; 
 point piter;
 random_points_in_square(number, 1000, LP);
 
 static_point_set T;
 
 float tm = used_time();
 T.init(LP);
 cout << "Time for building static point set:" << used_time(tm) << "\n";
 
 
 tm = used_time();
 point_set S(LP);
 cout << "Time for building dynamic point set:" << used_time(tm) << "\n"; 
 
 tm = used_time();
 d2_dictionary<double,double,bool> D;
 forall(piter,LP) D.insert(piter.xcoord(), piter.ycoord(), true);
 cout << "Time for building d2 dictionary:" << used_time(tm) << "\n";  
 
 cout << "Now we perform 50000 lookup operations on the point sets and the d2 dictionary.\n";
 
 list<point> lookup_test;
 random_points_in_square(50000, 1100, lookup_test);
 //forall(piter, LP) lookup_test.append(piter);
 
 int cnt = 0;

 tm = used_time();
 // lookup ...
 forall(piter, lookup_test){
   bool b = T.lookup(piter);
   if (b) cnt++;
 }
 cout << "Time for lookup operations on static point set:" << used_time(tm) << "\n";
 
 cout << "static point set: We located " << cnt << " Points!\n";
 cnt =0;
 
 tm = used_time();
 forall(piter, lookup_test){
   node v = S.lookup(piter);
   if (v) cnt++;
 } 
 cout << "Time for lookup operations on dynamic point set:" << used_time(tm) << "\n";
 
 cout << "dynamic point set: We located " << cnt << " Points!\n";
 
 cnt =0;
 
 tm = used_time();
 forall(piter, lookup_test){
   dic2_item d2i = D.lookup(piter.xcoord(), piter.ycoord());
   if (d2i) cnt++;
 } 
 cout << "Time for lookup operations on d2_dictionary:" << used_time(tm) << "\n";
 
 cout << "d2_dictionary: We located " << cnt << " Points!\n\n"; 
}



void test_rat_lookup(int number)
{
 cout << "The static and dynamic point set and d2 dictionary store " << number << " points.\n";

 list<rat_point> LP; 
 rat_point piter;
 random_points_in_square(number, 1000, LP);
 
 rat_static_point_set T;
 
 float tm = used_time();
 T.init(LP);
 cout << "Time for building static point set:" << used_time(tm) << "\n";
 
 
 tm = used_time();
 rat_point_set S(LP);
 cout << "Time for building dynamic point set:" << used_time(tm) << "\n"; 
 
 tm = used_time();
 d2_dictionary<rational,rational,bool> D;
 forall(piter,LP) D.insert(piter.xcoord(), piter.ycoord(), true);
 cout << "Time for building d2 dictionary:" << used_time(tm) << "\n";  
 
 cout << "Now we perform 50000 lookup operations on the point sets and the d2 dictionary.\n";
 
 list<rat_point> lookup_test;
 random_points_in_square(50000, 1100, lookup_test);
 //forall(piter, LP) lookup_test.append(piter);
 
 int cnt = 0;

 tm = used_time();
 // lookup ...
 forall(piter, lookup_test){
   bool b = T.lookup(piter);
   if (b) cnt++;
 }
 cout << "Time for lookup operations on static point set:" << used_time(tm) << "\n";
 
 cout << "static point set: We located " << cnt << " Points!\n";
 cnt =0;
 
 tm = used_time();
 forall(piter, lookup_test){
   node v = S.lookup(piter);
   if (v) cnt++;
 } 
 cout << "Time for lookup operations on dynamic point set:" << used_time(tm) << "\n";
 
 cout << "dynamic point set: We located " << cnt << " Points!\n";
 
 cnt =0;
 
 tm = used_time();
 forall(piter, lookup_test){
   dic2_item d2i = D.lookup(piter.xcoord(), piter.ycoord());
   if (d2i) cnt++;
 } 
 cout << "Time for lookup operations on d2_dictionary:" << used_time(tm) << "\n";
 
 cout << "d2_dictionary: We located " << cnt << " Points!\n\n"; 
}

int main(void)
{
 cout << "We compare the lookup operations on the static and dynamic point set and the d2 dictionary.\n\n\n";

 cout << "float kernel:\n";
 cout << "-------------\n";
 
 int number = 1000;
 test_float_lookup(number);
 number = 10000;
 test_float_lookup(number);
 number = 30000;
 test_float_lookup(number);
 number = 70000;
 test_float_lookup(number);  
 
 cout << "rational kernel:\n";
 cout << "----------------\n";
 
 number = 1000;
 test_rat_lookup(number);
 number = 10000;
 test_rat_lookup(number);
 number = 30000;
 test_rat_lookup(number);
 number = 70000;
 test_rat_lookup(number);   
 
 return 0;
} 



