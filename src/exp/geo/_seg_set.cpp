/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _seg_set.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/segment_set.h>

LEDA_BEGIN_NAMESPACE 

/*
#if !defined(mips) || defined(__GNUC__)
*/

/*
#if __GNUC__ < 4
*/

typedef double  COORD;
typedef point   POINT;
typedef segment SEGMENT;
typedef line    LINE;

SEGMENT SegmentSet::rotate_about_origin_by_alpha(const SEGMENT& s, bool pos_dir) const
{
	if (r90 == 0) return s;
	if (r90 > 0)  return s.rotate90(POINT(), pos_dir ? r90 : -r90);
	return s.rotate(POINT(), pos_dir ? alpha : -alpha);
}

SegmentSet::SegmentSet(int rot90)
{
  r90 = rot90 % 4;
  while (r90 < 0) r90 += 4;
  alpha = LEDA_PI_2 * r90;
}

SegmentSet::SegmentSet(double angle)
 : r90(-1), alpha(angle)
{
       if (alpha ==    LEDA_PI_2) r90 =  1;
  else if (alpha == -  LEDA_PI_2) r90 = -1;
  else if (alpha ==    LEDA_PI)   r90 =  2;
  else if (alpha == -  LEDA_PI)   r90 = -2;
  else if (alpha ==  3*LEDA_PI_2) r90 =  3;
  else if (alpha == -3*LEDA_PI_2) r90 = -3;
}

SEGMENT  SegmentSet::key(seg_item it) const
{ double x0 = segment_tree<double,double,GenPtr>::x0(it);
  double y  = segment_tree<double,double,GenPtr>::y(it);
  double x1 = segment_tree<double,double,GenPtr>::x1(it);
  SEGMENT s(x0,y,x1,y);
  //SEGMENT s(x0(it),y(it),x1(it),y(it)); 
  return rotate_about_origin_by_alpha(s, true);
 }

seg_item SegmentSet::insert(const SEGMENT& s, GenPtr p)
{ SEGMENT t = rotate_about_origin_by_alpha(s, false);
  COORD x0 = leda_min(t.xcoord1(), t.xcoord2());
  COORD x1 = leda_max(t.xcoord1(), t.xcoord2());
  COORD  y = t.ycoord1();
  return segment_tree<COORD,COORD,GenPtr>::insert(x0,x1,y,p);
}

seg_item SegmentSet::lookup(const SEGMENT& s) const
{ SEGMENT t = rotate_about_origin_by_alpha(s, false);
  COORD x0 = leda_min(t.xcoord1(), t.xcoord2());
  COORD x1 = leda_max(t.xcoord1(), t.xcoord2());
  COORD  y = t.ycoord1();
  return segment_tree<COORD,COORD,GenPtr>::lookup(x0,x1,y);
}

void     SegmentSet::del(const SEGMENT& s)
{ SEGMENT t = rotate_about_origin_by_alpha(s, false);
  COORD x0 = leda_min(t.xcoord1(), t.xcoord2());
  COORD x1 = leda_max(t.xcoord1(), t.xcoord2());
  COORD  y = t.ycoord1();
  (void)segment_tree<COORD,COORD,GenPtr>::del(x0,x1,y); 
}

list<seg_item>  SegmentSet::intersection(const SEGMENT& q) const
{ list<seg_item> L;
  SEGMENT t = rotate_about_origin_by_alpha(q, false);
  COORD  x = t.xcoord1();
  COORD y0 = leda_min(t.ycoord1(), t.ycoord2());
  COORD y1 = leda_max(t.ycoord1(), t.ycoord2());
  L = segment_tree<COORD,COORD,GenPtr>::query(x,y0,y1); 
  return L;
}

list<seg_item>  SegmentSet::intersection(const LINE& q) const
{ list<seg_item> L;
  SEGMENT t = rotate_about_origin_by_alpha(q.seg(), false);
  COORD x = t.x_proj(0);
  L = segment_tree<COORD,COORD,GenPtr>::y_infinity_query(x);
  return L;
}

list<seg_item>  SegmentSet::intersection_sorted(const SEGMENT& q) const
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

list<seg_item>  SegmentSet::intersection_sorted(const LINE& q) const
{ list<seg_item> L;
  SEGMENT t = rotate_about_origin_by_alpha(q.seg(), false);
  COORD x = t.x_proj(0);
  L = segment_tree<COORD,COORD,GenPtr>::y_infinity_query_sorted(x);
  if (t.ycoord1() > t.ycoord2()) L.reverse();
  return L;
}

/*
#endif
*/

LEDA_END_NAMESPACE 
