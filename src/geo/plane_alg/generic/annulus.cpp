/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  annulus.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


LEDA_BEGIN_NAMESPACE


enum {SEG_EDGE, RAY_EDGE, LIN_EDGE};


static int compute_edge(const GRAPH<CIRCLE,POINT>& G,edge e,POINT& p, POINT& q)
{ 
  // computes first and second point (p and q) of line/segment/ray
  // returns kind of edge

  CIRCLE C = G[source(e)];
  CIRCLE D = G[target(e)];

  bool dC = C.is_degenerate();
  bool dD = D.is_degenerate();

  if ( dC && dD )
  { POINT a = C.point1();
    POINT c = C.point3();
    p = midpoint(a,c);
    q = a.rotate90(p); 
    return LIN_EDGE;
  }

  if ( dC && !dD)
  { POINT a = C.point1();
    POINT c = C.point3();
    POINT m = midpoint(a,c);
    VECTOR rv = a.rotate90(m) - m;
    p = D.center();
    q = p - rv;
    return RAY_EDGE;
  }

  if ( !dC && dD )
  { POINT a = D.point1();
    POINT c = D.point3();
    POINT m = midpoint(a,c);
    VECTOR rv = a.rotate90(m) - m;
    p = C.center();
    q = p - rv;
    return RAY_EDGE;
  }

  if ( !dC && ! dD )
  {  p = C.center();
     q = D.center(); 
     return SEG_EDGE;
  }

  // never reached

  return 0;
}


// intersection tests


inline bool seg_seg_intersection(const POINT& a, const POINT& b,
                                 const POINT& c, const POINT& d)
{ return orientation(a,b,c) != orientation(a,b,d) &&
         orientation(c,d,a) != orientation(c,d,b);
}


inline bool ray_seg_intersection(const POINT& a, const POINT& b, const POINT& c,
                                                                 const POINT& d)
{ int acd = orientation(a,c,d);
  return (orientation(a,c,b) == acd && orientation(a,d,b) != acd);
}


inline bool lin_seg_intersection(const POINT& a, const POINT& b, const POINT& c,
                                                                 const POINT& d)
{ return (orientation(a,b,c) != orientation(a,b,d)); }



inline bool ray_ray_intersection(const POINT& a, const POINT& b, const POINT& c,
                                                                 const POINT& d)
{ POINT e = c + (b.to_vector() - a.to_vector());
  int cda = orientation(c,d,a);
  int cde = orientation(c,d,e);
  int cea = orientation(c,e,a);
  return cda && cde && cde != cea && cda != cde;
}


inline bool lin_ray_intersection(const POINT& a, const POINT& b, const POINT& c,
                                                                 const POINT& d)
{ POINT e = c + (b.to_vector() - a.to_vector());
  int ced = orientation(c,e,d);
  int cea = orientation(c,e,a);
  return ced && ced == cea;
}


inline bool lin_lin_intersection(const POINT& a, const POINT& b, const POINT& c,
                                                                 const POINT& d)
{ POINT e = c + (b.to_vector() - a.to_vector());
  return orientation(c,e,d) != 0;
}




static bool intersection(int kn, const POINT& An, const POINT& Bn,
                         int kf, const POINT& Af, const POINT& Bf,
                         POINT& candidate)

{ 
  switch (kn) {

     case SEG_EDGE:
            switch (kf) {
              case SEG_EDGE:
                     if ( !seg_seg_intersection(Af,Bf,An,Bn) ) return false;
                     break;
              case RAY_EDGE:
                     if ( !ray_seg_intersection(Af,Bf,An,Bn) ) return false;
                     break;
              case LIN_EDGE:
                     if ( !lin_seg_intersection(Af,Bf,An,Bn) ) return false;
                     break;
            }
            break;
          
     case RAY_EDGE:
            switch (kf) {
              case SEG_EDGE:
                     if ( !ray_seg_intersection(An,Bn,Af,Bf) ) return false;
                     break;
              case RAY_EDGE:
                     if ( !ray_ray_intersection(Af,Bf,An,Bn) ) return false;
                     break;
              case LIN_EDGE:
                     if ( !lin_ray_intersection(Af,Bf,An,Bn) ) return false;
                     break;
            }
           break;

     case LIN_EDGE:
            switch (kf) {
              case SEG_EDGE:
                     if ( !lin_seg_intersection(An,Bn,Af,Bf) ) return false;
                     break;
              case RAY_EDGE:
                     if ( !lin_ray_intersection(An,Bn,Af,Bf) ) return false;
                     break;
              case LIN_EDGE:
                     if ( !lin_lin_intersection(An,Bn,Af,Bf) ) return false;
                     break;
             }
            break;
    }


  // compute point of intersection


  LINE l1(An,Bn);
  LINE l2(Af,Bf);
  
  l1.intersection(l2,candidate);

  return true;
}



static bool annulus(const GRAPH<POINT,int>& DT, 
                    const GRAPH<POINT,int>& FDT,
                    const GRAPH<CIRCLE,POINT>& VD,
                    const GRAPH<CIRCLE,POINT>& FVD,
                    int (*qual_cmp)(const COORD&,const COORD&,const COORD&,
                                                 const COORD&,const COORD&), 
                    const COORD& r,POINT& center,POINT& ipoint,POINT& opoint)
{ 
 if (FVD.number_of_nodes() <= 2 ) return false;


  COORD best_irs = -1;
  COORD best_ors;

  node v;
  forall_nodes(v,VD)
  { if ( VD.outdeg(v) == 1 ) continue;
    CIRCLE IC = VD[v];
    POINT candidate  = IC.center();
    COORD irs = candidate.sqr_dist(IC.point1());
    POINT icandidate = IC.point1();
    COORD ors = -1; POINT ocandidate;
    node w;
    forall_nodes(w,FDT)
    { COORD r = candidate.sqr_dist(FDT[w]);
      if (r > ors )
      { ors = r;
        ocandidate = FDT[w];
       }     
     }
    
    if ( best_irs < 0 || qual_cmp(ors,irs,best_ors,best_irs,r) < 0 )
    { center = candidate; 
      ipoint = icandidate;
      opoint = ocandidate;
      best_irs = irs;
      best_ors = ors;
      COORD diff = best_ors - best_irs;
      if ( diff == 0 ) return true;
     }
  }


//cout << string("Vertices of VD:  %3.2f",used_time(T)) << endl;

  forall_nodes(v,FVD)
  { if ( FVD.outdeg(v) == 1 ) continue;
    CIRCLE OC = FVD[v];
    POINT candidate  = OC.center();
    COORD ors = candidate.sqr_dist(OC.point1());
    POINT ocandidate = OC.point1();
    node w;
    COORD irs = -1; POINT icandidate;
    forall_nodes(w,DT)
    { COORD r = candidate.sqr_dist(DT[w]);
      if ( irs < 0 || r < irs )
      { irs = r;
        icandidate = DT[w];
       }
     }
    
    if ( best_irs < 0 || qual_cmp(ors,irs,best_ors,best_irs,r) < 0 )
    { center = candidate; 
      ipoint = icandidate;
      opoint = ocandidate;
      best_irs = irs;
      best_ors = ors;
      COORD diff = best_ors - best_irs;
      if ( diff == 0 ) return true;
     }
  }

//cout << string("Vertices of FVD: %3.2f",used_time(T)) << endl;


  edge_array<POINT> Af(FVD);
  edge_array<POINT> Bf(FVD);
  edge_array<int>   kf(FVD);
  edge_array<bool>  reversal_f(FVD,false);

  edge ef;
  forall_edges(ef,FVD)
  { if (!reversal_f[ef])
    { kf[ef] = compute_edge(FVD,ef,Af[ef],Bf[ef]);
      reversal_f[FVD.reversal(ef)] = true;
     }
   }

  edge_array<bool>  considered_n(VD, false);

  edge en;
  forall_edges(en,VD)
  { 
    if ( considered_n[VD.reversal(en)] ) continue;
    considered_n[en] = true;

    POINT An,Bn;
    int kn = compute_edge(VD,en,An,Bn);

    edge ef;
    forall_edges(ef,FVD)
    { 
      if (reversal_f[ef]) continue;

      POINT candidate;
      COORD irs, ors;
      POINT icandidate, ocandidate;

      if ( intersection(kn,An,Bn,kf[ef],Af[ef],Bf[ef],candidate) )
      { 
        icandidate = VD[en];
        irs = candidate.sqr_dist(icandidate);  

        ocandidate = FVD[ef];
        ors = candidate.sqr_dist(ocandidate); 
        
        if ( best_irs < 0 || qual_cmp(ors,irs,best_ors,best_irs,r) < 0 )
        { center = candidate; 
          ipoint = icandidate;
          opoint = ocandidate;
          best_irs = irs;
          best_ors = ors;
          COORD diff = best_ors - best_irs;
          if ( diff == 0 ) return true;
         }

      }
    }
  }

//cout << string("Intersections:   %3.2f",used_time(T)) << endl;
//cout << endl;

 return true;
}


inline int sign_of(const COORD& x)
{ if (x > 0) return 1;
  else if (x < 0) return -1;
       else return 0;
}


static int min_area_qual_cmp (const COORD& ors,const COORD& irs,
                              const COORD& bors,const COORD& birs,const COORD&)
{ 
  return sign_of( (ors - irs) - (bors - birs) ); 
}



static int min_width_qual_cmp (const COORD& ors,const COORD& irs,
                               const COORD& bors,const COORD& birs,const COORD&)
{ 
  COORD A = ors + irs - (bors + birs);
  int a = sign_of(A);
  COORD B = ors*irs - bors*birs;
  int b = sign_of(B);
  if ( a != b ) return a - b ;
  if ( a == 0 ) return 0;
  COORD C = A*A - 4*(ors*irs + bors*birs);
  int c = sign_of(C);
  COORD D = ors*irs*bors*birs;
  int d = - sign_of(D);
  if ( c != d ) return a*(c - d);
  if ( c == 0 ) return 0;
  // D cannot be negative and hence C and D are positive at this point 
  COORD E = C*C;
  COORD F = 64 * D;
  int s = sign_of(E - F);
  return a*c*s;
}


bool MIN_WIDTH_ANNULUS(const list<POINT>& L, 
                       POINT& center, POINT& ipoint, POINT& opoint,
                       LINE& l1, LINE& l2)
{  
   if ( L.length() == 0 ) 
   { l1 = LINE();
     l2 = LINE();
     return false;
    }

   COORD sqr_width = WIDTH(L,l1,l2);

   if ( sqr_width == 0 ) return false;

   GRAPH<POINT,int> DT; 
   GRAPH<POINT,int> FDT;
   GRAPH<CIRCLE,POINT> VD;
   GRAPH<CIRCLE,POINT> FVD;
   DELAUNAY_DIAGRAM(L,DT);
   F_DELAUNAY_DIAGRAM(L,FDT);
   DELAUNAY_TO_VORONOI(DT,VD);
   F_DELAUNAY_TO_VORONOI(FDT,FVD);
   annulus(DT,FDT,VD,FVD,&min_width_qual_cmp,0,center,ipoint,opoint);
   COORD b = center.sqr_dist(opoint);
   COORD c = center.sqr_dist(ipoint);
   COORD d = b + c - sqr_width;

   //return (d <= 0 || 4*b*c >= d*d);

   if (d <= 0 || 4*b*c >= d*d) return true;

   return false;
}

/* We need to explain the return statement.

let a be the square with of the stripe, let b be the square radius of 
the outer circle and let c be the square radius of the inner circle. 
We want to know whether

  sqrt(a) >= sqrt(b) - sqrt(c).

Both sides are positive and hence the relation is not changed by squaring,
i.e., it suffices to decide the relation. 
  
  a >=  b + c - 2 * sqrt(b) * sqrt(c)

This relation holds iff

  2 * sqrt(b) * sqrt(c) >= b + c - a.

If the right hand side is non-positive the relation certainly holds. If the
right hand is positive it holds iff it holds after squaring, i.e., iff

  4*b*c >= (b + c - a)^2.

Thus the test is equivalent to

  b + c - a <= 0 || 4*b*c >= (b + c - a)^2.
  */

 
bool MIN_AREA_ANNULUS(const list<POINT>& L, 
                      POINT& center, POINT& ipoint, POINT& opoint,
                      LINE& l1)
{  if ( L.length() == 0 )
   { l1 = LINE(); return false; }
   LINE l2;
   COORD sqr_width = WIDTH(L,l1,l2);
   if ( sqr_width == 0 ) return false;
   // the points are not collinear and the optimum is a proper annulus
   GRAPH<POINT,int> DT; 
   GRAPH<POINT,int> FDT;
   GRAPH<CIRCLE,POINT> VD;
   GRAPH<CIRCLE,POINT> FVD;
   DELAUNAY_DIAGRAM(L,DT);
   F_DELAUNAY_DIAGRAM(L,FDT);
   DELAUNAY_TO_VORONOI(DT,VD);
   F_DELAUNAY_TO_VORONOI(FDT,FVD);
   annulus(DT,FDT,VD,FVD,&min_area_qual_cmp,0,center,ipoint,opoint);
   return true; 
}

LEDA_END_NAMESPACE
