/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _check_kuratowski.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph.h>
#include <LEDA/core/list.h>
#include <LEDA/core/map.h>
#include <LEDA/graph/node_map2.h>
#include <LEDA/graph/plane_graph_alg.h>

LEDA_BEGIN_NAMESPACE

bool CHECK_KURATOWSKI(const graph& G, const list<edge>& K)
{ node v,w; edge e;
  map<edge,bool> is_edge(false);
  forall_edges(e,G) is_edge[e] = true;
  edge_array<bool> in_K(G,false);
  node_array<int> deg_in_K(G,0);
  forall(e,K) 
  { if ( !is_edge[e] )
    { cerr << "check Kuratowski: edge does not belong to graph"; 
      return false;
    }
    if ( G.source(e) == G.target(e) )
    { cerr << "check Kuratowski: self loop in set of edges"; 
      return false;
    }
    if ( in_K[e] )
    { cerr << "check Kuratowski: multiple occurrence of edge"; 
      return false;
    } 
    in_K[e] = true; 
    deg_in_K[G.source(e)]++;
    deg_in_K[G.target(e)]++;
  }

  GRAPH<node, edge> AG;
  node_array<node> link(G,nil);

  forall_nodes(v,G) 
  { if ( deg_in_K[v] == 1 ) 
    { cerr << "check Kuratowski: node of degree 1";
      return false;
    }
    if ( deg_in_K[v] > 2 ) link[v] = AG.new_node(v);
  }
  
  int n = AG.number_of_nodes();
  if ( n != 5 && n != 6 ) 
  { cout << "\ncheck_kuratowski: n = " + string("%d",n);
    return false;
  }

  forall_nodes(v,G)
  { if ( !link[v] ) continue;
    edge e; 
    forall_inout_edges(e,v)
    { if ( in_K[e] )
      { // trace path starting with e
        edge f = e; node w = v;
        while (true)
        { in_K[f] = false;
          w = G.opposite(w,f);
          if ( link[w] ) break;
          // observe that w has degree two and hence ...
          forall_inout_edges(f,w)
            if ( in_K[f] ) break;
        }
        AG.new_edge(link[v],link[w]);
      }
    }
  }
        
  // no self-loops and no parallel or anti-parallel edges

  node_map2<bool> E(AG,false);

  forall_edges(e,AG)
  { node v = AG.source(e);
    node w = AG.target(e);
    if ( v == w || E(v,w) || E(w,v) )
    { cerr << "check Kuratowski: self -loop or parallel or anti-parallel edges in induced graph";
      return false;
    }
    E(v,w) = E(w,v) = true;
  }

  if ( n == 5)
    { forall_nodes(v,AG) 
      forall_nodes (w,AG) 
        if (v != w && !E(v,w) )
        { cerr << "check Kuratowski: K_5, missing edge";
          return false;
         } 
     }
  else
    { node_array<int> color(AG,0);
      node v = AG.first_node();
      int D = 0;
      forall_inout_edges(e,v) 
      { node w = AG.opposite(v,e); 
        color[w] = 1; D++;
       }

      if ( D != 3) 
      { cerr << "check Kuratowski: K_3,3, non-bipartite";
        return false;
       }

      forall_nodes(v,AG) 
      { forall_nodes(w,AG) 
         if (color[v] != color[w] && !E(v,w))
         { cerr << "check Kuratowski: K_3,3, missing edge";
          return false;
         } 
      }

      forall_edges(e,AG) 
         if (color[AG.source(e)] == color[AG.target(e)])
         { cerr << "check Kuratowski: K_3,3, too many edges";
           return false;
         } 
    }

  return true;
}

/*
bool CHECK_KURATOWSKI1(graph& G, const list<edge>& el)
{ node v,w; edge e;
  node_array<int> deg(G,0);
  map<edge,bool> is_edge(false);
  forall_edges(e,G) is_edge[e] = true;
  edge_array<bool> in_el(G,false);
  forall(e,el) { assert(is_edge[e]); in_el[e] = true; }
  
  list<edge> EL = G.all_edges();
  forall(e,EL) if (!in_el[e]) G.del_edge(e);
      
  list<node> V = G.all_nodes();
  forall(v,V)
  { list<edge> L  = G.out_edges(v);
    list<edge> L1 = G.in_edges(v);
    L.conc(L1);
    int deg = L.length();
    if ( deg == 0 ) { G.del_node(v); continue; }
    if ( deg == 1 ) LEDA_EXCEPTION(1,"check failed");
    if ( deg  == 2 )
    { edge e1 = L.head();
      edge e2 = L.tail();
      node u = G.opposite(v,e1);
      node w = G.opposite(v,e2);
      assert( u != v && w != v && u != w);
      G.del_node(v);
      G.new_edge(u,w);     
    }
  }
  int n = G.number_of_nodes();
  if ( n != 5 && n != 6 ) 
    LEDA_EXCEPTION(1,"check_kuratowski: n = " + string("%d",n));
  
  // no self-loops and no parallel or anti-parallel edges

  node_map2<bool> E(G,false);

  forall_edges(e,G)
  { node v = G.source(e);
    node w = G.target(e);
    assert( w != v && !E(v,w) && !E(w,v) );
    E(v,w) = E(w,v) = true;
  }


  if ( n == 5)
    { forall_nodes(v,G) 
      forall_nodes (w,G) if (v != w) assert(E(v,w)); }
  else
    { node_array<int> color(G,0);
      node v = G.first_node();
      int D = 0;
      forall_inout_edges(e,v) 
        { node w = G.opposite(v,e); 
          color[w] = 1; D++;
        }
      assert(D == 3);
      forall_nodes(v,G) 
      { forall_nodes(w,G) 
         if (color[v] != color[w]) assert(E(v,w));
      }
      forall_edges(e,G) 
        assert(color[G.source(e)] != color[G.target(e)]);
    }

  return true;
}
*/

LEDA_END_NAMESPACE
