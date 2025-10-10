/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  minkowski.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE


static list_item find_min(const list<POINT>& PL)
{ // find point with minimum y-coordinate ...
 list_item min = PL.first();

 list_item it; 
 forall_items(it,PL)
   if (POINT::cmp_yx(PL[min],PL[it]) > 0) min = it;

 return min;
}




static POLYGON minkowski_convex(const POLYGON& P, const POLYGON& R)
{
  // minkowski sum of two convex polygons
 
 list<POINT> target;

 const list<POINT>& pl1 = P.vertices();
 const list<POINT>& pl2 = R.vertices();

 list_item it1 = find_min(pl1);
 list_item it2 = find_min(pl2);

 list_item it_start1 = it1;
 list_item it_start2 = it2;
 
 bool run_it1 = false;
 bool run_it2 = false;
 
 do {

   POINT q = pl1[it1] + pl2[it2].to_vector();

   if (target.empty() || target.tail() != q) target.append(q);

   
   if (run_it1 && it1 == it_start1) 
   { it2 = pl2.cyclic_succ(it2);
     continue;
    }

   if (run_it2 && it2 == it_start2) 
   { it1 = pl1.cyclic_succ(it1);
     continue;
    }

   int c = compare_by_angle(pl1[it1],pl1[pl1.cyclic_succ(it1)],
                            pl2[it2],pl2[pl2.cyclic_succ(it2)]); 

   if (c <= 0) 
   { it1 =  pl1.cyclic_succ(it1); 
     run_it1 = true; 
    }

   if (c >= 0) 
   { it2 = pl2.cyclic_succ(it2); 
     run_it2 = true; 
    }

 } while(it1 != it_start1 || it2 != it_start2);
 

 POLYGON result(target);

 result.normalize();

 return result;
}




GEN_POLYGON MINKOWSKI_SUM(const GEN_POLYGON& P0, const POLYGON& R)
{
  GEN_POLYGON P = P0;

  if (P.empty() || P.full() || R.empty()) return P;


  P.normalize();
  R.normalize();

  bool unbounded = false;

  if (P.polygons().head().orientation() < 0) 
  { 
    unbounded = true;

    // reduce open polygons by intersecting with a bounding box 

    RECTANGLE bbox = BoundingBox(P);
  
    POINT p1 = bbox.upper_left().translate(-1,1);
    POINT p2 = bbox.lower_left().translate(-1,-1);
    POINT p3 = bbox.lower_right().translate(1,-1);
    POINT p4 = bbox.upper_right().translate(1,1);
    
    list<POINT> box_points;
    box_points.append(p1); 
    box_points.append(p2); 
    box_points.append(p3); 
    box_points.append(p4);
    
    P = P.intersection(GEN_POLYGON(box_points));
  }



  if (P.size() == 1) {
    POINT pt = P.vertices().head();
    return GEN_POLYGON(R.translate(pt.to_vector()));
  }
 
  if (R.size() == 1) {
    POINT pt = R.vertices().head();
    return GEN_POLYGON(P.translate(pt.to_vector()));
  }
  

  // compute convex parts of P and R

  list<POLYGON> parts_P = CONVEX_COMPONENTS(P);  
  list<POLYGON> parts_R = CONVEX_COMPONENTS(R);  

  list<GEN_POLYGON> L;

  POLYGON p,r;
  forall(p,parts_P)
    forall(r,parts_R)
      L.append(minkowski_convex(p,r));

  //GEN_POLYGON mink(L); // union 

  GEN_POLYGON mink = GEN_POLYGON::unite(L);

  if (unbounded) 
  { list<POLYGON> Lpol = mink.polygons();
    if (!Lpol.empty()) Lpol.pop();
    mink = GEN_POLYGON(Lpol);
   }

  return mink;     
}


GEN_POLYGON MINKOWSKI_SUM(const POLYGON& P, const POLYGON& R) 
{
  if (P.empty() || R.empty()) return P;

  if (P.orientation() == -1) 
     LEDA_EXCEPTION(1,"MINKOWSKI: negativ orientation of P.");

  if (R.orientation() == -1) 
     LEDA_EXCEPTION(1,"MINKOWSKI: negativ orientation of R.");

  return MINKOWSKI_SUM(GEN_POLYGON(P), R);
}




GEN_POLYGON MINKOWSKI_DIFF(const GEN_POLYGON& P, const POLYGON& R)
{ return MINKOWSKI_SUM(P, R.reflect(POINT(0,0,1))); }

GEN_POLYGON MINKOWSKI_DIFF(const POLYGON& P, const POLYGON& R)
{ return MINKOWSKI_SUM(P,R.reflect(POINT(0,0,1))); }



// -----------------------------------------------------------------------------
// variantss of MINKOWSKI_DIFF / MINKOWSKI_SUM :
//  - you can provide a user-defined function for computing convex parts of 
//    the polygons 
//  - you can provide a user-defined function for the union of a list of 
//    convex polygons
//  - caching of minimal vertices
//  - new angle comparison 
// -----------------------------------------------------------------------------

static POLYGON minkowski_convex_new(const POLYGON& P, const POLYGON& R, 
                                    list_item min_p, list_item min_r)
{
  // minkowski sum of two convex polygons P and R; min_p and min_r are the positions of the minimal
  // vertices in the vertex lists of P and R 
 
 list<POINT> target;

 const list<POINT>& pl1 = P.vertices();
 const list<POINT>& pl2 = R.vertices();

 list_item it1 = min_p;
 list_item it2 = min_r;

 list_item it_start1 = it1;
 list_item it_start2 = it2;
 
 bool run_it1 = false;
 bool run_it2 = false;
 
 do {

   POINT q = pl1[it1] + pl2[it2].to_vector();

   if (target.empty() || target.tail() != q) target.append(q);

   
   if (run_it1 && it1 == it_start1) 
   { it2 = pl2.cyclic_succ(it2);
     continue;
    }

   if (run_it2 && it2 == it_start2) 
   { it1 = pl1.cyclic_succ(it1);
     continue;
    }

   int c = compare_by_angle(pl1[it1],pl1[pl1.cyclic_succ(it1)],
                            pl2[it2],pl2[pl2.cyclic_succ(it2)]); 

   if (c <= 0) 
   { it1 =  pl1.cyclic_succ(it1); 
     run_it1 = true; 
    }

   if (c >= 0) 
   { it2 = pl2.cyclic_succ(it2); 
     run_it2 = true; 
    }

 } while(it1 != it_start1 || it2 != it_start2);
 

 POLYGON result(target);

 result.normalize();

 return result;
}



GEN_POLYGON MINKOWSKI_SUM(const GEN_POLYGON& P0, const POLYGON& R,
                          void (*conv_partition)(const GEN_POLYGON&, 
                                                 const POLYGON&, 
                                                 list<POLYGON>& , 
                                                 list<POLYGON>& ),
			  GEN_POLYGON (*conv_unite)(const list<GEN_POLYGON>&))
{
  GEN_POLYGON P = P0;

  if (P.empty() || P.full() || R.empty()) return P;


  P.normalize();
  R.normalize();

  bool unbounded = false;

  if (P.polygons().head().orientation() < 0) 
  { 
    unbounded = true;

    // reduce open polygons by intersecting with a bounding box 

    RECTANGLE bbox = BoundingBox(P);
  
    POINT p1 = bbox.upper_left().translate(-1,1);
    POINT p2 = bbox.lower_left().translate(-1,-1);
    POINT p3 = bbox.lower_right().translate(1,-1);
    POINT p4 = bbox.upper_right().translate(1,1);
    
    list<POINT> box_points;
    box_points.append(p1); 
    box_points.append(p2); 
    box_points.append(p3); 
    box_points.append(p4);
    
    P = P.intersection(GEN_POLYGON(box_points));
  }

  if (P.size() == 1) {
    POINT pt = P.vertices().head();
    return GEN_POLYGON(R.translate(pt.to_vector()));
  }
 
  if (R.size() == 1) {
    POINT pt = R.vertices().head();
    return GEN_POLYGON(P.translate(pt.to_vector()));
  }
  

  // compute convex parts of P and R

  list<POLYGON> parts_P;  
  list<POLYGON> parts_R;  

  if (conv_partition){
   conv_partition(P,R, parts_P, parts_R);
  }
  else {
   parts_P = CONVEX_COMPONENTS(P);  
   parts_R = CONVEX_COMPONENTS(R);  
  }

  list<GEN_POLYGON> L;
  POLYGON p,r;
  
  // precompute and store the minimal vertices of every
  // polygon ...
  
  list<list_item>  min_p;
  list<list_item>  min_r;
  
  list_item  m_it, ip, ir;
  
  forall(p,parts_P){
    const list<POINT>& vert = p.vertices();
    m_it = find_min(vert);
    min_p.push_back(m_it);
  }
  forall(p,parts_R){
    const list<POINT>& vert = p.vertices();
    m_it = find_min(vert);
    min_r.push_back(m_it); 
  }    

  // ... and reuse the minimal vertices in the loop ...
  list_item ip_act = min_p.first(), ir_act;
  
  forall(p,parts_P) {
    ip = min_p[ip_act];
    ip_act = min_p.succ(ip_act);
    
    ir_act = min_r.first();
      
    forall(r,parts_R) {   
      ir = min_r[ir_act];
      ir_act = min_r.succ(ir_act);
      
      L.append(minkowski_convex_new(p,r, ip,ir));
    }
  }

/*  
  forall(p,parts_P)
    forall(r,parts_R)
      L.append(minkowski_convex(p,r));
*/
  GEN_POLYGON mink;
  
  if (conv_unite) mink = conv_unite(L);
  else mink = GEN_POLYGON::unite(L);   

  if (unbounded) 
  { list<POLYGON> Lpol = mink.polygons();
    if (!Lpol.empty()) Lpol.pop();
    mink = GEN_POLYGON(Lpol);
   }

  return mink;     
}


GEN_POLYGON MINKOWSKI_SUM(const POLYGON& P, const POLYGON& R,
                          void (*conv_partition)(const GEN_POLYGON&, 
                                                 const POLYGON&, 
                                                 list<POLYGON>& , 
                                                 list<POLYGON>& ),
			  GEN_POLYGON (*conv_unite)(const list<GEN_POLYGON>&)) 
{
  if (P.empty() || R.empty()) return P;

  if (P.orientation() == -1) 
     LEDA_EXCEPTION(1,"MINKOWSKI: negativ orientation of P.");

  if (R.orientation() == -1) 
     LEDA_EXCEPTION(1,"MINKOWSKI: negativ orientation of R.");

  return MINKOWSKI_SUM(GEN_POLYGON(P), R, conv_partition, conv_unite);
}


GEN_POLYGON MINKOWSKI_DIFF(const GEN_POLYGON& P, const POLYGON& R,
                           void (*conv_partition)(const GEN_POLYGON&, 
                                                  const POLYGON&, 
                                                  list<POLYGON>& , 
                                                  list<POLYGON>& ),
			   GEN_POLYGON (*conv_unite)(const list<GEN_POLYGON>&) )
{ return MINKOWSKI_SUM(P, R.reflect(POINT(0,0,1)), conv_partition, conv_unite); }

GEN_POLYGON MINKOWSKI_DIFF(const POLYGON& P, const POLYGON& R,
                           void (*conv_partition)(const GEN_POLYGON&, 
                                                  const POLYGON&, 
                                                  list<POLYGON>& , 
                                                  list<POLYGON>& ),
			   GEN_POLYGON (*conv_unite)(const list<GEN_POLYGON>&) )
{ return MINKOWSKI_SUM(P,R.reflect(POINT(0,0,1)), conv_partition, conv_unite); }

// ----------------------------------------------------------------------------



static
void forbidden_space(const array<POLYGON>& AP, int low, int high, 
                     const POLYGON& R, GEN_POLYGON& result)
{
  int sz = high-low;
  if (sz==0) result = MINKOWSKI_DIFF(AP[low], R);
  else {
    GEN_POLYGON gp1,gp2;
    int spl = low + sz/2;
    forbidden_space(AP, low, spl, R, gp1);
    forbidden_space(AP, spl+1, high, R, gp2);
    result = gp1.unite(gp2);
  }
}


static
void forbidden_space(const list<POLYGON>& LP, const POLYGON& R, 
                                              GEN_POLYGON& result)
{
   array<POLYGON> AP(LP.size());
   int arr_index=0;
   POLYGON p;
   forall(p,LP) AP[arr_index++] = p; 
   forbidden_space(AP, 0, LP.size() -1, R, result);
}




GEN_POLYGON FREE_SPACE(const list<POLYGON>& LP, const POLYGON& R)
{  GEN_POLYGON P;
   forbidden_space(LP, R, P);
   return P.complement();
}

GEN_POLYGON FREE_SPACE(const list<POLYGON>& LP, const POLYGON& R, 
                                                const GEN_POLYGON& border)
{  GEN_POLYGON P;
   forbidden_space(LP, R, P);
   return border.intersection(P.complement());
}


LEDA_END_NAMESPACE


