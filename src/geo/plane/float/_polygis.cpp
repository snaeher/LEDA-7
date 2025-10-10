/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _polygis.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/polygis.h>
#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE 
 
const GPOLYGON&    polygis_polygon::get_poly()     
{ if (constructed != POLYGON_OBJ)
    LEDA_EXCEPTION(1,"polygis_polygon: polygon not closed.");
  return poly; 
}

const list<POINT>& polygis_polygon::get_polyline() 
{ if (constructed != LINE_OBJ)
    LEDA_EXCEPTION(1,"polygis_polygon: polyline not closed.");
  return plist; 
}



polygis_polygon::polygis_polygon(const GPOLYGON& pol)
{ 
  const list<POLYGON> poly_list = pol.polygons();

  poly = pol;

  plist.clear();

  if (poly_list.empty()) return;

  POINT p0 = poly_list.head().vertices().head();

  POLYGON P;
  forall(P,poly_list)
  { list<POINT> L = P.vertices();
    POINT q0 = L.head();
    L.append(q0);
    if (q0 != p0) L.append(p0);
    plist.conc(L);
  }
 constructed = POLYGON_OBJ;
}


polygis_polygon::polygis_polygon(const list<POINT>& L)
{
  plist = L;
  constructed = LINE_OBJ;
}
 

   
void polygis_polygon::construct_polygon()
{ 
  GPOLYGON pol;

  if (plist.empty())  
  { poly = pol;
    return;
   }


  list<POINT> pl = plist;

  POINT q0 = pl.head();

  list_item it = pl.first();
  do it = pl.succ(it); while (pl[it] != q0);
  
  pl.insert(q0,it,leda::behind);


  while (pl.size() > 1)
  {
    list<POINT> pl1;

    POINT p0 = pl.pop();
    POINT p  = pl.pop();
    pl1.append(p0);
    while (p != p0) 
    { pl1.append(p);
      p = pl.pop();
     }

    if (pol.polygons().empty() || pol.outside(pl1.head()))
       pol = pol.unite(GPOLYGON(pl1,GPOLYGON::NO_CHECK,
                                      GPOLYGON::DISREGARD_ORIENTATION));
    else
       pol = pol.diff(GPOLYGON(pl1,GPOLYGON::NO_CHECK,
                                     GPOLYGON::DISREGARD_ORIENTATION));

    POINT q = pl.pop();
    assert(q == q0);
   }

  poly = pol;
  constructed = POLYGON_OBJ;
}

   
void polygis_polygon::construct_polyline()
{
  constructed = LINE_OBJ;
 }



void polygis_polygon::write_points(double* xc, double* yc)
{ 
  POINT p;
  forall(p,plist)
  { point q = p.to_float();
    *xc++ = q.xcoord();
    *yc++ = q.ycoord();
   }
}
 
LEDA_END_NAMESPACE 
