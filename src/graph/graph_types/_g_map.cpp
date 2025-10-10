/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _g_map.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE

face& graph::access_face(edge e)  const 
{ return (face&)FaceOf->map_access(e); }

// reversal edges

edge graph::face_cycle_succ(edge e) const 
{ return cyclic_adj_pred(reversal(e)); }

edge graph::face_cycle_pred(edge e) const 
{ return reversal(cyclic_adj_succ(e)); }

edge graph::succ_face_edge(edge e) const 
{ return cyclic_adj_pred(reversal(e)); }

edge graph::pred_face_edge(edge e) const 
{ return reversal(cyclic_adj_succ(e)); }


void graph::set_reversal(edge e, edge r)
{ if ( r == nil || e == r || source(e) != target(r) || target(e) != source(r) )
  LEDA_EXCEPTION(1,"graph::set_reversal: edges are not reversals of each other");
  if (e->reversal()) e->reversal()->set_reversal(nil);
  if (r->reversal()) r->reversal()->set_reversal(nil);
  e->set_reversal(r);
  r->set_reversal(e);
}

/*
the function check_reversal_inf checks reversal information. It does
not assume that the reversal information is either nil of an edge of G
*/
#include <LEDA/core/map.h>


bool check_reversal_inf(const graph& G)
{ map<edge,bool> is_edge_of_G(false);
  edge e;
  forall_edges(e,G) is_edge_of_G[e] = true;
  forall_edges(e,G) 
  { edge r = G.reversal(e);
    if ( r == nil || is_edge_of_G[r]) return false;
  }
  forall_edges(e,G) 
  { edge r = G.reversal(e); 
    if (r == e || G.reversal(r) != e || 
       G.source(e) != G.target(r) || G.target(e) != G.source(r) ) 
    return false;
  }
  return true;
}



static int map_edge_ord1(const edge& e) 
                        { return index(source(e)); }
static int map_edge_ord2(const edge& e) 
                        { return index(target(e)); }

bool graph::make_map() 
{
  int n = max_node_index();
  int count = 0;

  edge e,r;

  list<edge> EST; 
  forall_edges(e,(*this)) 
   if (e->reversal() == nil) EST.append(e);

  int number_of_undefined_reversals = EST.length();
  
  list<edge> ETS = EST; ETS.reverse();

  EST.bucket_sort(0,n,&map_edge_ord2); // secondary key
  EST.bucket_sort(0,n,&map_edge_ord1); // primary key

  ETS.bucket_sort(0,n,&map_edge_ord1); // secondary key
  ETS.bucket_sort(0,n,&map_edge_ord2); // primary key

  // merge EST and ETS to find corresponding edges

  while (! EST.empty() && ! ETS.empty())
  { e = EST.head();
    r = ETS.head();

    if ( e->reversal() != nil ) { EST.pop(); continue; }
    if ( r->reversal() != nil ) { ETS.pop(); continue; }

    if ( target(r) == source(e) )
    { if ( source(r) == target(e) )
      { ETS.pop(); EST.pop();
        if ( e != r )
        { e->set_reversal(r); r->set_reversal(e); 
          count += 2;
        }
        continue;
      }
      else // target(r) == source(e) && source(r) != target(e)
      { if (index(source(r)) < index(target(e)))
          ETS.pop();  // r cannot be matched
        else
          EST.pop();  // e cannot be matched
      }
    }
    else // target(r) != source(e)
    { if (index(target(r)) < index(source(e)))
        ETS.pop();  // r cannot be matched
      else
        EST.pop();  // e cannot be matched
    }

   }

  return count == number_of_undefined_reversals;
}


void graph::make_map(list<edge>& R)     
{ if (make_map()) return;
  list<edge> el = all_edges();
  edge e;
  forall(e,el)
  { if (e->reversal() == nil)
    { edge r = new_edge(target(e),source(e));
      e->set_reversal(r);
      r->set_reversal(e);
      R.append(r);
    }
  }
}

/* the chunk map_map replaces the code below:

static int map_edge_ord1(const edge& e) { return index(source(e)); }
static int map_edge_ord2(const edge& e) { return index(target(e)); }

bool graph::make_map()     
{
 // computes for every edge e = (v,w) in G its reversal r = (w,v)
 // in G ( nil if not present). Returns true if every edge has a
 // reversal and false otherwise.

  int n     = max_node_index();
  int count = 0;

  list<edge> El1 = all_edges();
  list<edge> El2 = El1;

  edge e;
  forall(e,El1) e->set_reversal(0);

  El1.bucket_sort(0,n,&map_edge_ord2);
  El1.bucket_sort(0,n,&map_edge_ord1);
  El2.bucket_sort(0,n,&map_edge_ord1);
  El2.bucket_sort(0,n,&map_edge_ord2);


  // merge El1 and El2 to find corresponding edges

  while (! El1.empty() && ! El2.empty())
  { edge e = El1.head();
    edge r = El2.head();
    if (target(r) == source(e))
      if (source(r) == target(e))
         { e->set_reversal(r);
           El2.pop();
           El1.pop();
           count++;
          }
      else
         if (index(source(r)) < index(target(e)))
             El2.pop();
         else
             El1.pop();

    else
      if (index(target(r)) < index(source(e)))
          El2.pop();
      else
          El1.pop();

   }

  return count == number_of_edges();
}


void graph::make_map(list<edge>& R)     
{ if (make_map()) return;
  list<edge> el = all_edges();
  edge e;
  forall(e,el)
  { if (e->reversal() == nil)
    { edge r = new_edge(target(e),source(e));
      e->set_reversal(r);
      r->set_reversal(e);
      R.append(r);
     }
   }
}
   
*/



//extern bool PLANAR(graph&, bool = false);

void graph::make_planar_map()
{ if (!graph::make_map())
       LEDA_EXCEPTION(1,"graph::make_planar_map: graph is not bidirected");
  if (!PLANAR(*this,true)) 
       LEDA_EXCEPTION(1,"graph::make_planar_map: Graph is not planar."); 
  compute_faces();
}



face  graph::new_face(GenPtr i) 
{ copy_face_entry(i);
  return add_face(i);
 }

face  graph::new_face() 
{ GenPtr i = 0;
  init_face_entry(i);
  return add_face(i);
 }


list<edge> graph::adj_edges(face f) const
{ list<edge> result(f->first_edge());
  edge e1 = face_cycle_succ(f->first_edge());
  while (e1!=f->first_edge())
  { result.append(e1);
    e1 = face_cycle_succ(e1);
   }
  return result;
 }

list<node> graph::adj_nodes(face f) const
{ list<node> result(source(f->first_edge()));
  edge e1 = face_cycle_succ(f->first_edge());
  while (e1!=f->first_edge())
  { result.append(source(e1));
    e1 = face_cycle_succ(e1);
   }
  return result;
 }

list<face> graph::adj_faces(node v) const
{ list<face> result;
  edge e;
  forall_out_edges(e,v) result.append(adj_face(e));
  return result;
 }


void graph::print_face(face f) const
{ cout << string("F[%2d]",index(f));
  cout << "(";
  write_face_entry(cout,f->data(0));
  cout << "): ";
  edge e;
  forall_face_edges(e,f) 
   cout << string("[%2d]",index(target(e)));
 }




void graph::compute_faces()
{ 
  del_all_faces();

  FaceOf = new graph_map<graph>(this,1,0);

  edge e;
  forall_edges(e,*this) 
  { if (e->reversal() == nil) 
      LEDA_EXCEPTION(1,"graph::compute_faces: no map (reversal edge missing)");
    access_face(e) = nil;
   }


  forall_edges(e,*this)
  { if (access_face(e) != nil) continue;
    face f = new_face();
    f->set_first_edge(e);
    edge e1 = e;
    int count = 0;
    do { access_face(e1) = f;
         e1 = face_cycle_succ(e1);
         count++;
       } while (e1 != e);
    f->set_size(count);
   } 
}

LEDA_END_NAMESPACE

#include <LEDA/graph/face_array.h>

LEDA_BEGIN_NAMESPACE

void graph::dual_map(graph& D) const
{ D.clear();
  graph& M = *((graph*)this); // cast away the const
  M.compute_faces();  
  face f; edge e;
  face_array<node> dual(M);
  forall_faces(f,M) dual[f] = D.new_node();
  
  edge_array<edge> dual_edge(M);
  forall_faces(f,M)
  { node df = dual[f];
    forall_face_edges(e,f)
    { face g = M.face_of(M.reversal(e));
      dual_edge[e] = D.new_edge(df,dual[g]);
    }
  }

  forall_edges(e,M) 
    D.set_reversal(dual_edge[e],dual_edge[M.reversal(e)]);
}



edge graph::split_map_edge(edge e)
{ 
  /* splits edge e and its reversal by inserting a new node u (node_inf) 
              e                          e           rr
        ----------->                --------->   --------->
     (v)            (w)   ====>  (v)          (u)          (w)
        <-----------                <---------   <---------
              r                          er          r
     returns edge rr
  */

  edge r = e->reversal();

  if (r == nil)
    LEDA_EXCEPTION(1,"graph::split_map_edge(e): reversal of edge e missing.");

  node v = source(e);
  node w = target(e);
  node u = new_node();

  // remove e and r from corresponding in-lists
  w->del_adj_edge(e,1,1);
  v->del_adj_edge(r,1,1);

  // insert e and r in in-list of u
  e->set_term(1,u);
  r->set_term(1,u);
  u->push_adj_edge(e,1,1);
  u->push_adj_edge(r,1,1);

  // create reverse edges rr and re
  edge rr = graph::new_edge(u,w);
  edge er = graph::new_edge(u,v);

  set_reversal(e,er);
  set_reversal(r,rr);

  face& f_rr = access_face(rr);
  face& f_er = access_face(er);

  f_rr = access_face(e);
  f_er = access_face(r);

  return rr;
}



edge graph::new_map_edge(edge e1, edge e2)
{ edge e = graph::new_edge(e1,source(e2));
  edge r = graph::new_edge(e2,source(e1));
  set_reversal(e,r);
  return e;
}


edge graph::split_face(edge e1, edge e2)
{ 
  face f1 = access_face(e1);
  face f2 = access_face(e2);

  if (f1 != f2)
    LEDA_EXCEPTION(1,"planar_map::new_edge: new edge must lie in a face."); 

  f2 = new_face();

  edge x = graph::new_edge(e1,source(e2));
  edge y = graph::new_edge(e2,source(e1));
  set_reversal(x,y);

  f1->set_first_edge(x);
  f2->set_first_edge(y);

  access_face(x) = f1;

  do { access_face(y) = f2;
       y = face_cycle_succ(y);
     } while (y != f2->first_edge());

  return x;
}

extern int COMPONENTS(const graph& G, node_array<int>& );


list<edge> graph::triangulate_map()
{ node v;
  edge x, e, e1, e2, e3;
  list<edge> L;

  
node_array<int>  comp(*this);
int c = COMPONENTS(*this, comp);
if ( c > 1 )
{ node s = first_node();
  array<bool> still_disconnected(c);
  for (int i = 0; i < c; i++)
    still_disconnected[i] = ( i == comp[s] ? false : true);
  forall_nodes(v,(*this))
  { if ( still_disconnected[comp[v]] )
    { set_reversal(e1 = new_edge(s,v), e2 = new_edge(v,s));
      L.append(e1); L.append(e2);
      still_disconnected[comp[v]] = false;
    }
  }
}

                                            
  if ( !make_map() ) 
  LEDA_EXCEPTION(1,"TRIANGULATE_PLANAR_MAP: graph is not a map.");

  node_array<int>  marked(*this,0);

  forall_nodes(v,*this)
  { list<edge> El = adj_edges(v);

    // mark all neighbors of v
    forall(e1,El) marked[target(e1)] = 1;

    
forall(e,El)
{ 
  e1 = e;
  e2 = face_cycle_succ(e1);
  e3 = face_cycle_succ(e2);
  if (target(e1) == v || target(e2) == v || target(e3) == v)
    continue;

  while (target(e3) != v)
  { node w = target(e2);
    if ( !marked[w] )
    { // we mark w and add the uedge {v,w}
      
      marked[w] = 1;
      L.append(x  = new_edge(e3,v));
      L.append(e1 = new_edge(e1,w));
      set_reversal(x,e1);
      e2 = e3;
      e3 = face_cycle_succ(e2);
    }
    else
    { //add the uedge {source(e2),target(e3)}

      e3 = face_cycle_succ(e3); 
      L.append(x  = new_edge(e3,source(e2)));
      L.append(e2 = new_edge(e2,source(e3)));
      set_reversal(x,e2);
    }
  }//end of while
} //end of stepping through incident faces


    //unmark all neighbors of v
    node w; 
    forall_adj_nodes(w,v) marked[w] = 0;

  } // end of stepping through nodes

 return L;

}


/* replaces the function triangulate_map below. The new implementation
can also handle graph that are not connected

 G is a planar map. This procedure triangulates all faces of G
   without introducing multiple edges. The algorithm was suggested by 
   Christian Uhrig and Torben Hagerup. 

   Description:

   Triangulating a planar graph G, i.e., adding edges
   to G to obtain a chordal planar graph, in linear time:
   
   1) Compute a (combinatorial) embedding of G.
   
   2) Step through the vertices of G. For each vertex u,
   triangulate those faces incident on u that have not
   already been triangulated. For each vertex u, this
   consists of the following:
   
     a) Mark the neighbors of u. During the processing
   of u, a vertex will be marked exactly if it is a
   neighbor of u.
   
     b) Process in any order those faces incident on u
   that have not already been triangulated. For each such
   face with boundary vertices u=x_1,...,x_n,
        I)   If n=3, do nothing; otherwise
        II)  If x_3 is not marked, add an edge {x_1,x_3},
             mark x_3 and continue triangulating the face
             with boundary vertices x_1,x_3,x_4,...,x_n.
        III) If x_3 is marked, add an edge {x_2,x_4} and
             continue triangulating the face with boundary
             vertices x_1,x_2,x_4,x_5,...,x_n.
   
     c) Unmark the neighbors of x_1.
   
   Proof of correctness:
   
   A) All faces are triangulated.
   This is rather obvious.
   
   B) There will be no multiple edges.
   During the processing of a vertex u, the marks on
   neighbors of u clearly prevent us from adding a multiple
   edge with endpoint u. After the processing of u, such an
   edge is not added because all faces incident on u have
   been triangulated. This takes care of edges added in
   step II).
   Whenever an edge {x_2,x_4} is added in step III), the
   presence of an edge {x_1,x_3} implies, by a topological
   argument, that x_2 and x_4 are incident on exactly one
   common face, namely the face currently being processed.
   Hence we never add another edge {x_2,x_4}.



list<edge> graph::triangulate_map()
{ 
  
   
  node v;
  edge x;
  list<edge> L;

  node_array<int>  marked(*this,0);

  if ( !make_map() ) 
  LEDA_EXCEPTION(1,"TRIANGULATE_PLANAR_MAP: graph is not a map.");

  forall_nodes(v,*this)
  {
    list<edge> El = adj_edges(v);
    edge e,e1,e2,e3;
 
    forall(e1,El) marked[target(e1)]=1;

    forall(e,El)
    { 
      e1 = e;
      e2 = face_cycle_succ(e1);
      e3 = face_cycle_succ(e2);

      while (target(e3) != v)
      // e1,e2 and e3 are the first three edges in a clockwise 
      // traversal of a face incident to v and t(e3) is not equal
      // to v.
       if ( !marked[target(e2)] )
        { // we mark w and add the edge {v,w} inside F, i.e., after
          // dart e1 at v and after dart e3 at w.
 
          marked[target(e2)] = 1;
          L.append(x  = new_edge(e3,source(e1)));
          L.append(e1 = new_edge(e1,source(e3)));
          set_reversal(x,e1);
          e2 = e3;
          e3 = face_cycle_succ(e2);
        }
        else
        { // we add the edge {source(e2),target(e3)} inside F, i.e.,
          // after dart e2 at source(e2) and before dart 
          // reversal_of[e3] at target(e3).

          e3 = face_cycle_succ(e3); 
          L.append(x  = new_edge(e3,source(e2)));
          L.append(e2 = new_edge(e2,source(e3)));
          set_reversal(x,e2);
        }
     //end of while

    } //end of stepping through incident faces

   node w; 
   forall_adj_nodes(w,v) marked[w] = 0;

  } // end of stepping through nodes

 return L;

}

*/

/* Kurt made some changes to join_faces:
Let x be the edge to be deleted, let y be its reversal,
and let Fx and Fy be the face cycles containing x and y.

We distinguish cases.

Case 1: Fx != Fy and one of the faces consists of at least two
edges.

Assume first that Fy consists of at least two edges and let
e0 be the face cycle successor of y. We join Fx and Fy 
-- setting the first edge of the resulting face to e0
-- setting the face information of all edges on Fy to the new
   face
-- deleting Fy.

If Fx consists of at least two edges we interchange the role of
Fx and Fy. 

Case 2: Fx != Fy and both faces consist of exactly one edge

Both face faces disappear from the graph and the return value is
nil.


Case 2: F = Fx = Fy

We need to distinguish three subcases.

Case 2.1: F consists of exactly x and y.
 Then the number of face cycles does down by one.

Case 2.2  F consists of at least three edges and x is the face
   cycle successor of y (or y is the face cycle successor of x).

   The removal does not change the number of faces. It makes
   the source of x an isolated node. 

   We make the face cycle successor of x the first edge of the face.

Case 2.3  Neither x nor y is the face cycle successor of the other.
   Then we obtain two faces after the removal. 

 */
/* old version
face graph::join_faces(edge x)
{
  edge y  = reversal(x);

  if (y == nil)
      LEDA_EXCEPTION(1,"join_faces: graph must be a map.");

  if (access_face(x) == nil || access_face(y) == nil)
      LEDA_EXCEPTION(1,"join_faces: no face associated with edges.");

  edge e  = face_cycle_succ(y);
  face F1 = adj_face(x);
  face F2 = adj_face(y);

  if (F1 != F2)
  { edge e = face_cycle_succ(y);
    F1->set_first_edge(e);
    while ( e != y )
    { access_face(e) = F1;
      e = face_cycle_succ(e);
     }
    clear_face_entry(F2->data(0));
    del_face(F2);
   }
  else
  { e = face_cycle_succ(e);
    if (e != y) // no isolated edge
      F1->set_first_edge(e);   
    else 
      { clear_face_entry(F1->data(0));
        del_face(F1);
        F1 = F2;
       }
   }

  graph::del_edge(x);
  graph::del_edge(y);

  return F1;
}
*/

// new version

face graph::join_faces(edge x)
{
  edge y  = reversal(x);

  if (y == nil)
      LEDA_EXCEPTION(1,"join_faces: graph must be a map.");

  if (access_face(x) == nil || access_face(y) == nil)
      LEDA_EXCEPTION(1,"join_faces: no face associated with edges.");

  if ( face_cycle_succ(y) == y) leda_swap(x,y);

  face Fx = adj_face(x);
  face Fy = adj_face(y);

  if (Fx != Fy)
  { if (face_cycle_succ(y) != y)
    { edge e = face_cycle_succ(y);
      Fx->set_first_edge(e);
      while ( e != y )
      { access_face(e) = Fx;
        e = face_cycle_succ(e);
      }
      clear_face_entry(Fy->data(0));
      del_face(Fy);
    }
    else
    { // Fx != Fy and both faces consist of only one edge
      clear_face_entry(Fy->data(0));
      del_face(Fy);
      clear_face_entry(Fx->data(0));
      del_face(Fx);
      Fx = nil;
    }
  } 
  else // F = Fx = Fy
  { if ( y == face_cycle_succ(x) ) leda_swap(x,y);
    
    if ( y == face_cycle_succ(x) )
    { // F consists of exactly two edges.
      clear_face_entry(Fy->data(0));
      del_face(Fy);
      clear_face_entry(Fx->data(0));
      del_face(Fx);
      Fx = nil;
    }
    else // y != face_cycle_succ(x) 
    { if ( x == face_cycle_succ(y) )
        Fx->set_first_edge(face_cycle_succ(x));
      else 
      { // F is split into two
        Fy = new_face();
        Fx->set_first_edge(face_cycle_succ(x));
        Fy->set_first_edge(face_cycle_succ(y));
        edge e = Fx->first_edge();
        do { access_face(e) = Fx;
             e = face_cycle_succ(e);
        }
        while (e != Fx->first_edge());
        e = Fy->first_edge();
        do { access_face(e) = Fy;
             e = face_cycle_succ(e);
        }
        while (e != Fy->first_edge());
      }
    }
  }

  graph::del_edge(x);
  graph::del_edge(y);

  return Fx;
}


void graph::make_bidirected(list<edge>& L) 
{ Make_Bidirected(*this,L); }

void graph::make_bidirected() 
{ Make_Bidirected(*this); }

bool graph::is_bidirected() const
{ edge_array<edge> rev(*this,0);
  return Is_Bidirected(*this,rev);
}

bool graph::is_map() const  
{ return Is_Map(*this); }

LEDA_END_NAMESPACE
