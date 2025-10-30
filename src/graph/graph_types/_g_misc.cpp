/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_misc.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_map2.h>
#include <LEDA/graph/series_parallel.h>

#include <LEDA/core/b_queue.h>

//------------------------------------------------------------------------------
// last modified  2010:  replaced (recursive) dfs by bfs
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE


static int bfs(const graph& G, node v, node_array<bool>& reached, 
               node* min_deg_ptr = 0)
{ 
  int n = G.number_of_nodes();
  b_queue<node> Q(n);

  reached[v] = true;
  Q.push(v);

  int count = 1;
  node min_v = v;

  while (!Q.empty())
  { node u = Q.pop();
    edge e;
    forall_inout_edges(e,u)
    { node v = G.opposite(u,e);
      if (reached[v]) continue;
      Q.append(v);
      reached[v] = true;
      if (G.degree(v) < G.degree(min_v)) min_v = v;
      count++;
     }
  }

  if (min_deg_ptr) *min_deg_ptr = min_v;

  return count;
}


//------------------------------------------------------------------------------

void CopyGraph(graph& H, const graph& G) 
{ H.clear();
  H.set_node_bound(G.number_of_nodes());
  H.set_edge_bound(G.number_of_edges());
  node_array<node> v_in_H(G); 
  node v;
  forall_nodes(v,G) v_in_H[v] = H.new_node();
  forall_nodes(v,G)
  { node u = v_in_H[v];
    edge e;
    forall_adj_edges(e,v) 
    { node s = source(e);
      node t = target(e);
      if (s == v) H.new_edge(u,v_in_H[t]);
     }
   }
}


void CopyGraph(GRAPH<node,edge>& H, const graph& G) 
{ H.clear();
  H.set_node_bound(G.number_of_nodes());
  H.set_edge_bound(G.number_of_edges());
  node_array<node> v_in_H(G); 
  node v;
  forall_nodes(v,G) v_in_H[v] = H.new_node(v);
  forall_nodes(v,G)
  { node u = v_in_H[v];
    edge e;
    forall_adj_edges(e,v) 
    { node s = source(e);
      node t = target(e);
      if (s == v) H.new_edge(u,v_in_H[t],e);
     }
   }
}

void CopyGraph(GRAPH<node,edge>& H, const graph& G, const list<node>& V,
                                                    const list<edge>& E) 
{ // constructs a copy of the subgraph (V,E) of G
  // predondition: E is a subset of VxV
  H.clear();
  H.set_node_bound(G.number_of_nodes());
  H.set_edge_bound(G.number_of_edges());
  node_array<node> v_in_H(G,nil); 
  node v;
  forall(v,V) v_in_H[v] = H.new_node(v);
  edge e;
  forall(e,E)
  { node s = v_in_H[source(e)];
    node t = v_in_H[target(e)];
    if (s == nil || t == nil)
       LEDA_EXCEPTION(1,"CopyGraph(H,G,V,E): E must be a subset of VxV.");
    H.new_edge(s,t,e);
   }
}


void CopyGraph(GRAPH<node,edge>& H, const graph& G, const list<edge>& E) 
{ // constructs a copy of the subgraph of G induced by the edges of E
  H.clear();
  H.set_node_bound(G.number_of_nodes());
  H.set_edge_bound(G.number_of_edges());
  node_array<node> v_in_H(G,nil); 
  edge e;
  forall(e,E)
  { node u = source(e);
    node v = target(e);
    node& s = v_in_H[u];
    node& t = v_in_H[v];
    if (s == nil) s = H.new_node(u);
    if (t == nil) t = H.new_node(v);
    H.new_edge(s,t,e);
   }
}



static node_array<int>* num_ptr;
  
static int source_num(const edge& e) { return (*num_ptr)[source(e)]; }
static int target_num(const edge& e) { return (*num_ptr)[target(e)]; }


bool Is_Simple(const graph& G, list<edge>& el)  
{ 
  // return true iff G is simple, i.e, has no parallel edges

  el = G.all_edges();

  if (el.empty()) return true;
  
  int n = 0;
  
  node_array<int> num(G);

  node v;
  forall_nodes(v,G) num[v]= n++;
  
  num_ptr= &num;
  el.bucket_sort(0,n-1,&source_num);
  el.bucket_sort(0,n-1,&target_num);
  
  edge e0 = nil;
  edge e;
  forall(e,el)
  { if (e0 && source(e0) == source(e) && target(e0) == target(e) )
       return false;
    else
       e0 = e;
   }

  return true;
}
  


bool Is_Simple(const graph& G)  
{ 
  list<edge> el; 
  return Is_Simple(G,el);
}
  
  
list<node> Delete_Loops(graph& G)
{ list<edge> loops;
  list<node> L;
  edge e;
  forall_edges(e,G)
  { node v = source(e);
    node w = target(e);
    if (v == w) 
    { L.append(v);
      loops.append(e);
     }
   }
  forall(e,loops) G.del_edge(e);
  return L;
 }


list<edge> Make_Simple(graph& G)
{ 
  list<edge> L;
  list<edge> el;

  if (Is_Simple(G,el)) return L;

  bool deleted = false;
  edge e0 = el.pop();

  edge e;
  forall(e,el)  
    if (source(e0) == source(e) && target(e0) == target(e)) 
     { G.del_edge(e);
       if (!deleted) L.append(e0);
       deleted = true;
      }
    else 
     { deleted = false;
       e0 = e;
      }

  if (G.is_undirected())
  { edge_array<edge> rev(G);
    Is_Bidirected(G,rev);
    edge e;
    forall_edges(e,G)
    { edge r = rev[e];
      if (r == nil) continue;
      G.del_edge(e);
      rev[r] = nil;
      L.append(r);
     }
   }
  
  return L;
}


static int edge_ord1(const edge& e) { return index(source(e)); }
static int edge_ord2(const edge& e) { return index(target(e)); }


bool Is_Bidirected(const graph& G, edge_array<edge>& reversal) 
{
 // computes for every edge e = (v,w) in G its reversal reversal[e] = (w,v)
 // in G ( nil if not present). Returns true if every edge has a
 // reversal and false otherwise.


  int n = G.max_node_index();
  int count = 0;

  edge e,r;

  forall_edges(e,G) reversal[e] = 0;

  list<edge> EST = G.all_edges();
  
  int number_of_undefined_reversals = EST.length();
  
  list<edge> ETS = EST; ETS.reverse();

  EST.bucket_sort(0,n,&edge_ord2); // secondary key
  EST.bucket_sort(0,n,&edge_ord1); // primary key

  ETS.bucket_sort(0,n,&edge_ord1); // secondary key
  ETS.bucket_sort(0,n,&edge_ord2); // primary key

  // merge EST and ETS to find corresponding edges

  while (! EST.empty() && ! ETS.empty())
  { e = EST.head();
    r = ETS.head();

    if ( reversal[e] != nil ) { EST.pop(); continue; }
    if ( reversal[r] != nil ) { ETS.pop(); continue; }

    if ( target(r) == source(e) )
    { if ( source(r) == target(e) )
      { ETS.pop(); EST.pop();
        if ( e != r )
        { reversal[e] = r; reversal[r] = e; 
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



void Make_Bidirected(graph& G, list<edge>& R)
{
  // make graph bi-directed by inserting reversal edges
  // appends new edges to R

  edge_array<edge> rev(G,nil);

  if (Is_Bidirected(G,rev)) return;

  // build list L of edges having no reversals

  list<edge> L;
  edge e;
  forall_edges(e,G)
   if (rev[e] == nil) L.append(e);

  // insert missing reversals
  forall(e,L)
  { edge r = G.new_edge(target(e),source(e));
    R.append(r);
   }
}


list<edge> Make_Bidirected(graph& G)
{ list<edge> R;
  Make_Bidirected(G,R);
  return R;
 }

static void dfs(node v, int& count1, int& count2, node_array<int>& dfsnum, 
                                                  node_array<int>& compnum)
{ dfsnum[v] = ++count1;
  edge e;
  forall_adj_edges(e,v)
  { node w = target(e);
    if (dfsnum[w] == 0)
      dfs(w,count1,count2,dfsnum,compnum);
   }
  compnum[v] = ++count2;
}
 

bool Is_Acyclic(const graph& G, list<edge>& back)
{ 
  //compute dfs and completion numbers
  node_array<int> dfsnum(G,0);
  node_array<int> compnum(G,0);
  int count1 = 0;
  int count2 = 0;
  node v;
  forall_nodes(v,G)
    if (dfsnum[v] == 0)
        dfs(v,count1,count2,dfsnum,compnum);

  // compute back edges
  back.clear();
  edge e;
  forall_edges(e,G)
  { node v = source(e);
    node w = target(e);
    if (v == w || (dfsnum[v] > dfsnum[w] && compnum[v] < compnum[w]))
      back.append(e);
   }

  return back.empty();
}


bool Is_Acyclic(const graph& G)
{ list<edge> dummy;
  return Is_Acyclic(G,dummy);
}


void Make_Acyclic(graph& G)
{ list<edge> back;
  Is_Acyclic(G,back);
  edge e;
  forall(e,back) G.del_edge(e);
}


bool Is_Connected(const graph& G)
{
  node_array <bool>  reached(G, false);

  node s = G.first_node();
  int  count = 0;

  if (s != nil)
    count = bfs(G,s,reached);

  return count == G.number_of_nodes();
 }


void Make_Connected(graph& G, list<edge>& L)
{
  node_array <bool> reached(G,false);
  node u = nil;

  node v;
  forall_nodes(v, G) 
  { if (reached[v]) continue; 
    node min_d = v;
    bfs(G,v,reached,&min_d);        // explore connected comp with root v 
    if (u == 0)                    // return node with minimal degree
      u = min_d;                   // and connect with first component 
    else
      L.append(G.new_edge(u,min_d));
   }
 }



list<edge> Make_Connected(graph& G)
{ list<edge> L;
  if (G.number_of_nodes() > 0) Make_Connected(G,L);
  return L;
 }





static void make_bicon_dfs(graph& G, node v, int& dfs_count, 
                                list<edge>& L,
                                node_array<int>&  dfsnum, 
                                node_array<int>&  lowpt,
                                node_array<node>& parent)
{ node u = nil;

  dfsnum[v] = dfs_count++;
  lowpt[v]  = dfsnum[v];

  edge e;
  forall_inout_edges(e,v) 
  { 
    node w = G.opposite(v,e);

    if (v == w) continue; // ignore loops

    if (u == nil) u = w;  // first child

    if ( dfsnum[w] == -1) // w not reached before; e is a tree edge
      { 
        parent[w] = v;
  
        make_bicon_dfs(G, w, dfs_count, L, dfsnum, lowpt, parent);
  
        if (lowpt[w] == dfsnum[v])      
        { // |v| is an articulation point. We now add an edge. If |w| is the
          // first child and |v| has a parent then we connect |w| and
          // |parent[v]|, if |w| is a first child and |v| has no parent then
          // we do nothing. If |w| is not the first child then we connect |w|
          // to the first child. The net effect of all of this is to link all
          // children of an articulation point to the first child and the first
          // child to the parent (if it exists)
  
          if (w == u && parent[v]) 
          { L.append(G.new_edge(w, parent[v]));
            //L.append(G.new_edge(parent[v], w)); (if bidirected)
           }
  
          if (w != u)
          { L.append(G.new_edge(u, w));
            //L.append(G.new_edge(w, u)); (if bidirected)
           }
         }
  
         lowpt[v] = leda_min(lowpt[v], lowpt[w]);
       } 
      else // non tree edge
       lowpt[v] = leda_min(lowpt[v], dfsnum[w]);

  }

}



static node is_bicon_dfs(const graph& G, node v, int& dfs_count, 
                                         node_array<int>&  dfsnum, 
                                         node_array<int>&  lowpt,
                                         node_array<node>& parent)
{ // return articulation point or nil
  node u = nil;
  edge e;

  dfsnum[v] = dfs_count++;
  lowpt[v]  = dfsnum[v];

  forall_inout_edges(e,v) 
  { node w = G.opposite(v,e);
    if ( dfsnum[w] == -1 )
     { if (u == nil) u = w;
       parent[w] = v;
       node art = is_bicon_dfs(G, w, dfs_count, dfsnum, lowpt, parent);
       if (art) return art;
       if (lowpt[w] == dfsnum[v] && (w != u || parent[v]))  return v;
       lowpt[v] = leda_min(lowpt[v], lowpt[w]);
      } 
    else 
       lowpt[v] = leda_min(lowpt[v], dfsnum[w]);
  }

  return nil;
}


bool Is_Biconnected(const graph & G, node& art)
{ if (G.empty()) return true;
  if ( ! Is_Connected(G) ) 
  { art = nil;
    return false;
   }
  node_array<int>  lowpt(G);
  node_array<int>  dfsnum(G,-1);
  node_array<node> parent(G,nil);
  int dfs_count = 0;
  art = is_bicon_dfs(G, G.first_node(), dfs_count, dfsnum, lowpt, parent);
  return (art == nil);
}                               


bool Is_Biconnected(const graph & G)
{ node x;
  return Is_Biconnected(G,x); }                         


bool Is_Triconnected(const graph & G0, node& s1, node& s2)
{ 
  if ( ! Is_Biconnected(G0,s1) ) 
  { s2 = s1;
    return false;
   }

  if (G0.number_of_nodes() <= 3) return true;

  // we work on a local copy G of the original graph G0

  GRAPH<node,edge> G;
  CopyGraph(G,G0);

  node x;
  forall_nodes(x,G)
  { if (G[x] == nil) continue;
    G.hide_node(x);
    node y;
    if (!Is_Biconnected(G,y))
    { s1 = G[x];
      s2 = G[y];
      return false;
     }
    G[x] = nil;
    G.restore_node(x);
    G.restore_all_edges();
  }
  return true;
}


bool Is_Triconnected(const graph & G)
{ node x,y;
  return Is_Triconnected(G,x,y);
}


    

void Make_Biconnected(graph& G, list<edge>& L)
{
  if (G.number_of_nodes() == 0) return;

  Make_Connected(G,L);

  node_array<int>  lowpt(G);
  node_array<int>  dfsnum(G,-1);  // dfsnum[v] == -1  <=>  v not reached 
  node_array<node> parent(G,nil);

  int dfs_count = 0;

  make_bicon_dfs(G, G.first_node(), dfs_count, L, dfsnum, lowpt, parent);
}                               


list<edge> Make_Biconnected(graph & G)
{ list<edge> L;
  Make_Biconnected(G,L);
  return L;
 }

bool Is_Loopfree(const graph& G)
{ edge e;
 forall_edges(e,G) 
   if (G.source(e) == G.target(e) ) return false;
 return true;
} 

bool Is_Simple_Loopfree(const graph& G)
{ return Is_Simple(G) && Is_Loopfree(G); } 

 

/*
bool Is_Simple_Undirected(const graph& G)
{ edge e;
  node_map2<bool> E(G,false);
  forall_edges(e,G)
  { node v = G.source(e);
    node w = G.target(e);
    if ( v == w ) return false;
    if ( E(v,w) ) return false;
    E(v,w) = E(w,v) = true;
  }
  return true;
}
*/


static node_array<int>* num_ptr1;
  
static int min_num(const edge& e) 
{ return leda_min((*num_ptr1)[source(e)],(*num_ptr1)[target(e)]); }

static int max_num(const edge& e) 
{ return leda_max((*num_ptr1)[source(e)],(*num_ptr1)[target(e)]); }



bool Is_Undirected_Simple(const graph& G)  
{ 
  // return true iff G is simple as undirected graph, 
 // i.e, has neither parallel nor anti-parallel edges

  list<edge> el = G.all_edges();

  if (el.empty()) return true;
  
  int n = 0;
  
  node_array<int> num(G);

  node v;
  forall_nodes(v,G) num[v]= n++;
  
  num_ptr1= &num;
  el.bucket_sort(0,n-1,&min_num);
  el.bucket_sort(0,n-1,&max_num);
  
  edge e0 = nil;
  edge e;
  forall(e,el)
  { if (e0 && min_num(e0) == min_num(e) && max_num(e0) == max_num(e))
       return false;
    else
       e0 = e;
   }

  return true;
}
  


static edge bi_bfs(const graph& G, node s, node_array<int>& side,
                                           node_array<node>& pred)
{ 
  list<node> Q;

  Q.append(s);
  side[s] = 0;

  while ( ! Q.empty() )
  { node v = Q.head();
    edge e;
    forall_inout_edges(e,v)
    { node w = G.opposite(v,e);
      if (side[v] == side[w]) return e;
      if (side[w] == -1)
      { Q.append(w); 
        pred[w] = v;
        side[w] = 1 - side[v];
       }
     }
    Q.pop();
  }

  return nil;
}


bool Is_Bipartite(const graph& G, list<node>& A, list<node>& B)
{
  node_array<int>  side(G,-1);
  node_array<node> pred(G,nil);
  node v;

  forall_nodes(v,G)
  { if (side[v] != -1) continue;
    edge e = bi_bfs(G,v,side,pred);
    if (e != nil)
    { // construct odd-length circle
      node x = source(e);
      node y = target(e);
      node u;
      for(u=x; u != nil; u = pred[u]) side[u] = -1; 
      for(u = y; side[u] != -1; u = pred[u]) A.append(u);
      A.append(u);
      while (x != u) 
      { A.push(x);
        x = pred[x];
       }
     return false;
    }
   }

  forall_nodes(v,G)
  { if (side[v] == 0) A.append(v);
    if (side[v] == 1) B.append(v);
   }

  return true;

 }


bool Is_Bipartite(const graph& G)
{ list<node> A,B;
  return Is_Bipartite(G,A,B);
}


int COMPONENTS(const graph&G, node_array<int>&);


int Genus(const graph& G)
{ if ( !Is_Map(G) ) LEDA_EXCEPTION(1,"Genus only applies to maps");
  int n = G.number_of_nodes();
  if ( n == 0 ) return 0;
  int nz = 0;
  node v;
  forall_nodes(v,G) 
     if ( outdeg(v) == 0 )  nz++;
  int m = G.number_of_edges();
  node_array<int> cnum(G);
  int c = COMPONENTS(G,cnum);

  edge_array<bool> considered(G,false);
  int fc = 0;
  edge e;
  forall_edges(e,G)
  { if ( !considered[e] )
    { // trace the face to the left of e
      edge e1 = e;
      do { considered[e1] = true;
           e1 = G.face_cycle_succ(e1);
         }
      while (e1 != e);
      fc++;
    }
  }
  return (m/2 - n - nz - fc + 2*c)/2;
}


/* replaces the function below that handles isolated nodes incorrectly.

int Genus(const graph& G)
{ int n = G.number_of_nodes();
  int m = G.number_of_edges()/2;  // G is bidirected

  edge_array<bool> considered(G,false);
  int f = 0;
  edge e;
  forall_edges(e,G)
  { if (G.reversal(e) == nil)
       LEDA_EXCEPTION(1,"Genus: graph must be a map.");
    if ( !considered[e] )
    { // trace the face to the left of e
      f++;
      edge x = e;
      do { considered[x] = true;
           x = G.face_cycle_succ(x);
         }
      while (x != e);
    }
  }

  node_array<int> cnum(G);
  int c = COMPONENTS(G,cnum)-1;

  return 1 - (n+m-f+c)/2;
}

*/




/*
void copy_graph(const graph& G, GRAPH<node,edge>& H, 
                node_array<node>& v_in_H, edge_array<edge>& e_in_H)
{

  node v;
  forall_nodes(v,G) v_in_H[v] = H.new_node(v);

  edge e;
  forall_edges(e,G) e_in_H[e] =
      H.new_edge(v_in_H[source(e)],v_in_H[target(e)],e);
}
*/


bool Is_Map(const graph& G)
{ edge e;
  forall_edges(e,G)
  { edge r = G.reversal(e);
    if ( r == nil || r == e || G.reversal(r) != e || source(e) != target(r)
                                                  || source(r) != target(e) ) 
       return false; 
   }
  return true;
}


bool Is_Planar_Map(const graph& G) { return  Is_Map(G) && Genus(G) == 0; }

bool Is_Plane_Map(const graph& G) { return  Is_Map(G) && Genus(G) == 0; }

bool Is_Planar(const graph& G)
{ graph G1 = G;
  return PLANAR(G1);
}

bool Is_Series_Parallel(const graph& G)
{ spq_tree spqt(G); 
  return spqt.is_series_parallel(); 
}

LEDA_END_NAMESPACE
