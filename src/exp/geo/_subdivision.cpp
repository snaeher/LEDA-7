/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _subdivision.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/segment.h>
#include <LEDA/geo/subdivision.h>
#include <LEDA/core/p_queue.h>
#include <LEDA/core/array.h>

// use partially persistent dictionaries instead of fully persistent dicts
// (which should be more efficient)
#define USE_PP_DICT

#ifdef USE_PP_DICT
#include <LEDA/core/pp_dictionary.h>
#define PersDict pp_dictionary
#define PersDictItem pp_dic_item
#define UPDATE_DICT(Dict, UpdCommand) UpdCommand
#else
#include <LEDA/core/p_dictionary.h>
#define PersDict p_dictionary
#define PersDictItem p_dic_item
#define UPDATE_DICT(Dict, UpdCommand) Dict = (UpdCommand)
#endif

//-----------------------------------------------------------------------------
// planar subdivisions 
//
// S. N"aher (1991)
//-----------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

static double x_sweep;

//static 
int cmp_segments_sd(const segment& s1,const segment& s2)
{ if (s1 == s2) return 0;
  point p1 = s1.source();
  point p2 = s1.target();
  point q1 = s2.source();
  point q2 = s2.target();

  if (identical(p1,p2)) return orientation(s2,p1);
  if (identical(q1,q2)) return -orientation(s1,q1);

  if (p1 == q1) return cmp_slopes(s1,s2);
  if (p2 == q2) return cmp_slopes(s2,s1);

  return compare(s1.y_proj(x_sweep),s2.y_proj(x_sweep));
}

DEFINE_LINEAR_ORDER(segment,cmp_segments_sd,segment1)

typedef PersDict<segment1,edge>  strip;


struct strip_list {

array<double>  X;
array<strip>   S;

strip_list(int n) : X(n), S(n) { }

};




SubDivision::SubDivision(const graph& G) : planar_map(G)
{ 
  // compute strips

  int n = G.number_of_nodes();
  int i = 0;

  strip_ptr = new strip_list(n+1);

  strip_list& strips = *(strip_list*)strip_ptr;

  p_queue<point,edge> X;  // X-structure (event queue)

  edge e;
  forall_edges(e,*this) 
  { point p = position(source(e));
    point q = position(target(e));
    if (p.xcoord() < q.xcoord()) 
    { X.insert(p,e);
      X.insert(q,e);
     }
   }


  // sweep

  strip Y;  // empty strip

  x_sweep = -MAXDOUBLE;

  strips.X[i] = x_sweep;
  strips.S[i] = Y;
  i++;

  while( !X.empty() )
  { point p = X.prio(X.find_min());
    x_sweep = p.xcoord();

    // construct lists of ending (E) and starting (S) segments
    list<edge> E,S;

    while ( !X.empty() )
    { pq_item it = X.find_min();
      point    q = X.prio(it);
      edge     e = X.inf(it);

      if (q.xcoord() != p.xcoord()) break;

      if (q == position(source(e)))  // left  end
          S.append(e);    
      else                           // right end
          E.append(e);    

      X.del_item(it);
     }

    // insert new strip into version List

    edge e;
    forall(e,E) UPDATE_DICT(Y, Y.del(edge_seg(e)));
    forall(e,S) UPDATE_DICT(Y, Y.insert(edge_seg(e),e));

    strips.X[i] = x_sweep;
    strips.S[i] = Y;
    i++;
   }
   while (i < n+1) strips.X[i++] = MAXDOUBLE;

   // compute an edge of the outer face
   Y = strips.S[1];
   outer_edge  = Y.inf(Y.max());
}



face SubDivision::locate_point(point p) const
{ strip_list& strips = *(strip_list*)strip_ptr;
  int i = strips.X.binary_locate(p.xcoord());
  strip Y = strips.S[i];
  x_sweep = p.xcoord();
  PersDictItem it = Y.locate_pred(segment(p,p));
  edge e = it ? Y.inf(it) : outer_edge;
  return adj_face(e);
 }





void SubDivision::print_stripes() const
{ 
  strip_list& strips = *(strip_list*)strip_ptr;

  int N = strips.X.high();

  for(int i = 0; i<=N; i++)
  { strip Y = strips.S[i];
    double x = strips.X[i];
    if (x == +MAXDOUBLE) break;
    if (x == -MAXDOUBLE) continue;
    cout << string("strip %2d (x=%.2f):",i,x) << endl;
    PersDictItem it2;
    forall_items(it2,Y)
      cout << Y.key(it2) << " F = " << index(adj_face(Y.inf(it2))) << endl;
    cout << endl;
  }
  cout << endl;
 }

SubDivision::~SubDivision() { delete (strip_list*)strip_ptr; }

LEDA_END_NAMESPACE 
