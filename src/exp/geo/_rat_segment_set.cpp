/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _rat_segment_set.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rat_segment_set.h>

// NOTE: This file is basically a copy of _seg_set.cpp!

LEDA_BEGIN_NAMESPACE 

#if !defined(mips) || defined(__GNUC__)

typedef rational    COORD;
typedef rat_point   POINT;
typedef rat_segment SEGMENT;
typedef rat_line    LINE;

RatSegmentSet::RatSegmentSet(int rot90)
{
  r90 = rot90 % 4;
  while (r90 < 0) r90 += 4;
}

SEGMENT  RatSegmentSet::key(seg_item it) const
{ SEGMENT s(x0(it),y(it),x1(it),y(it)); 
  return rotate_about_origin_by_alpha(s, true);
 }

seg_item RatSegmentSet::insert(const SEGMENT& s, GenPtr p)
{ SEGMENT t = rotate_about_origin_by_alpha(s, false);
  COORD x0 = leda_min(t.xcoord1(), t.xcoord2());
  COORD x1 = leda_max(t.xcoord1(), t.xcoord2());
  COORD  y = t.ycoord1();
  return segment_tree<COORD,COORD,GenPtr>::insert(x0,x1,y,p);
}

seg_item RatSegmentSet::lookup(const SEGMENT& s) const
{ SEGMENT t = rotate_about_origin_by_alpha(s, false);
  COORD x0 = leda_min(t.xcoord1(), t.xcoord2());
  COORD x1 = leda_max(t.xcoord1(), t.xcoord2());
  COORD  y = t.ycoord1();
  return segment_tree<COORD,COORD,GenPtr>::lookup(x0,x1,y);
}

void     RatSegmentSet::del(const SEGMENT& s)
{ SEGMENT t = rotate_about_origin_by_alpha(s, false);
  COORD x0 = leda_min(t.xcoord1(), t.xcoord2());
  COORD x1 = leda_max(t.xcoord1(), t.xcoord2());
  COORD  y = t.ycoord1();
  (void)segment_tree<COORD,COORD,GenPtr>::del(x0,x1,y); 
}

list<seg_item>  RatSegmentSet::intersection(const SEGMENT& q) const
{ list<seg_item> L;
  SEGMENT t = rotate_about_origin_by_alpha(q, false);
  COORD  x = t.xcoord1();
  COORD y0 = leda_min(t.ycoord1(), t.ycoord2());
  COORD y1 = leda_max(t.ycoord1(), t.ycoord2());
  L = segment_tree<COORD,COORD,GenPtr>::query(x,y0,y1); 
  return L;
}

list<seg_item>  RatSegmentSet::intersection(const LINE& q) const
{ list<seg_item> L;
  SEGMENT t = rotate_about_origin_by_alpha(q.seg(), false);
  COORD  x = t.x_proj(0);
  L = segment_tree<COORD,COORD,GenPtr>::y_infinity_query(x); 
  return L;
}

list<seg_item>  RatSegmentSet::intersection_sorted(const SEGMENT& q) const
{ list<seg_item> L;
  SEGMENT t = rotate_about_origin_by_alpha(q, false);
  COORD  x = t.xcoord1();
  COORD y0 = t.ycoord1();
  COORD y1 = t.ycoord2();
  if (y0 <= y1) {
    L = segment_tree<COORD,COORD,GenPtr>::query_sorted(x,y0,y1);
  }
  else {
    L = segment_tree<COORD,COORD,GenPtr>::query_sorted(x,y1,y0);
	L.reverse();
  }
  return L;
}

list<seg_item>  RatSegmentSet::intersection_sorted(const LINE& q) const
{ list<seg_item> L;
  SEGMENT t = rotate_about_origin_by_alpha(q.seg(), false);
  COORD x = t.x_proj(0);
  L = segment_tree<COORD,COORD,GenPtr>::y_infinity_query_sorted(x);
  if (t.ycoord1() > t.ycoord2()) L.reverse();
  return L;
}

#endif

LEDA_END_NAMESPACE 
