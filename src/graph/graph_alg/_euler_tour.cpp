/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _euler_tour.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/euler_tour.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/array.h>

LEDA_BEGIN_NAMESPACE

typedef two_tuple<edge,int> dart;

/*
void Eulerian_Graph(graph& G, int n, int m)
{ array<node> V(n);
  for (int i = 0; i < n; i++) V[i] = G.new_node();
  for (int i = 0; i < m; i++)
  { int v = rand_int(0,n-1);
    int w = rand_int(0,n-1);
    G.new_edge(V[v],V[w]);
  }
  Make_Connected(G);
  int i = 0;
  while ( true )
  { while ( i < n && G.degree(V[i]) % 2 == 0 ) i++;
    if ( i == n ) return; 
    int j = i + 1;
    while ( j < n && G.degree(V[j]) % 2 == 0 ) j++;
    leda_assert( j < n,"Make Eulerian Graph",1);
    G.new_edge(V[i],V[j]);
    i = j + 1;
  }
}
*/

void insert_after(list<dart>& T,list_item& cur_item, const dart& t)
{ if (cur_item == nil) 
    cur_item = T.append(t);
  else 
    cur_item = T.insert(t,cur_item);
}

bool Check_Euler_Tour(const graph& G, const list<dart>& T)
{ list_item it;
  leda_assert(G.number_of_edges() == T.length(),"short",1);
  if ( G.number_of_edges() != T.length() ) return false;
  edge_array<bool> used(G,false);

  forall_items(it,T)
  { dart t = T[it];
    edge e = t.first();
    leda_assert(!used[e],"double use",1);
    if ( used[e] ) return false;
    used[e] = true;
    node v = ( t.second() == +1 ? G.target(e) : G.source(e) );
    dart s = T[T.cyclic_succ(it)];
    edge f = s.first();
    node w = ( s.second() == +1 ? G.source(f) : G.target(f) );
    leda_assert(v == w,"fit",1);
    if ( v != w ) return false;
  }
  return true;
}

bool Check_Euler_Tour(const graph& G, const list<edge>& T)
/*{\Mfunc returns true if $T$ is an Euler tour in the directed graph $G$.}*/
{ list_item it;
  if ( G.number_of_edges() != T.length() ) return false;
  edge_array<bool> used(G,false);

  forall_items(it,T)
  { edge e = T[it];
    if ( used[e] ) return false;
    used[e] = true;
    node v = G.target(e);
    edge f = T[T.cyclic_succ(it)];
    node w = G.source(f);
    if ( v != w ) return false;
  }
  return true;
}



bool EULER_TOUR(graph& G, list<dart>& T)
{ node v;
  forall_nodes(v,G)
  { if ( G.degree(v) % 2 != 0 ) return false; }

  if ( !Is_Connected(G) ) return false;

  // G is connected and every node has even degree. Thus G is Eulerian.

  v = G.first_node(); edge e; 
  list_item cur_item = nil;
  while ( true )
  { /* v is a node with positive degree and the edge pointed to by cur_edge

   */
   
    while ( true )
    { if ( ( e = G.first_adj_edge(v) ) == nil ) 
        if ( ( e = G.first_in_edge(v) ) == nil ) break;  // v has degree zero

      if ( v == G.source(e) )
      { insert_after(T,cur_item,dart(e,+1));   
        v = G.target(e);
      }
      else   
      { insert_after(T,cur_item,dart(e,-1)); 
        v = G.source(e);
      }                 
      G.hide_edge(e);
    }

  
    if ( G.number_of_edges() == 0 )
    { G.restore_all_edges();
      return true;
    }

    do { edge e = T[cur_item].first();
         v = G.opposite(v,e);
         cur_item = T.cyclic_pred(cur_item); 
    } while ( G.degree(v) == 0 );
  }
} 


bool Euler_Tour(const graph& G, list<two_tuple<edge,int> >& T)
     /*{\Mfunc The function returns true if the undirected verion of 
$G$ has an Euler tour. An undirected graph has
an Euler tour if it is connected and all nodes have even degree. The Euler
tour is returned in $T$. The items in $T$ are of the form $(e,\pm +1)$,
where the second component indicates the traversal direction $d$ of the edge.
If $d = +1$, the edge is traversed in forward direction, and if $d = -1$, the 
edge is traversed in reverse direction.}*/
{ GRAPH<node,edge> H;
  CopyGraph(H,G);
  if ( !EULER_TOUR(H,T) ) return false;

  list_item it;
  forall_items(it,T) T[it].first() = H[T[it].first()];
  return true;
} 


bool Euler_Tour(graph& G, list<edge>& T)
     /*{\Mfunc The function returns true if the undirected verion of 
$G$ has an Euler tour. An undirected graph has
an Euler tour if it is connected and all nodes have even degree. 
$G$ is reoriented such that every node has indegree equal to its outdegree 
and an Euler tour (of the reoriented graph) is returned in $T$.}*/
{ list<dart> L;
  if ( !EULER_TOUR(G,L) ) return false;
  dart t;
  forall(t,L) 
  { edge e = t.first();
    if ( t.second() == - 1) G.rev_edge(e); 
    T.append(e);
  }
  return true;
} 


LEDA_END_NAMESPACE
