/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _point_dictionary.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/point_dictionary.h>

LEDA_BEGIN_NAMESPACE 

list<point> Point_Dictionary::all_points()
{ list<point> result;
  list<pd_item> Li = all_items();
  pd_item it;
  forall(it,Li) result.append(point(key1(it),key2(it)));
  return result;
}


pd_item Point_Dictionary::nearest_neighbor(double x, double y)
{
  double  mdist2 = MAXDOUBLE;
  pd_item min_it = nil;

  list<pd_item> Li = all_items();

  pd_item it;
  forall(it,Li)
  { double dx = x - key1(it);
    double dy = y - key2(it);
    double D  = dx*dx + dy*dy;

    if (mdist2 > D)
    { mdist2 = D;
      min_it = it;
     }
   }

   return min_it;
}

LEDA_END_NAMESPACE 
