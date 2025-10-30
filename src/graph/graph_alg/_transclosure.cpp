/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _transclosure.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// Transitive Closure and Reduction
//                                                                              
// S. N"aher (1989)
// S. Thiel  (2001)
//------------------------------------------------------------------------------


#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/b_queue.h>

LEDA_BEGIN_NAMESPACE

/*
  This implements the algorithm in the paper by Klaus Simon: 
  An Improved Algorithm for Transitive Closure on Acyclic Digraph. 
  ICALP 1986: 376-386
 */



static bool topsort(graph& G) 
{ 
  // rearrange nodes and edges using bucket sort

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  if (n == 0 || m == 0) return true;

  node_array<int> node_ord(G,0);
  edge_array<int> edge_ord(G,0);
  node_array<int> indeg(G,0);

  b_queue<node> zero_indeg(n);

  int count=0;

  node v;
  forall_nodes(v,G) 
  { int d = G.indeg(v); 
    indeg[v] = d;
    if (d == 0) zero_indeg.append(v); 
   }

  while (!zero_indeg.empty())
   { v = zero_indeg.pop();
     node_ord[v] = ++count;
     node w;
     forall_adj_nodes(w,v) 
        if (--indeg[w]==0) zero_indeg.append(w);
    }
  
  if (count < G.number_of_nodes()) return false;
     
  edge e;
  forall_edges(e,G) edge_ord[e] = node_ord[target(e)];

  G.bucket_sort_nodes(node_ord);
  G.bucket_sort_edges(edge_ord);
  return true;
}
 
    


static
bool acyclic_transitive_closure_or_reduction(graph& G, bool compute_closure)
{
  if (!topsort(G)) return false;
  // sorts nodes in increasing topolog. order
  // sorts adjacency lists in increasing topolog. order of adjacent nodes

  int n = G.number_of_nodes();
  node*           N = new node[n]; // N[i] = i-th node in topological order
  node_array<int> top_ord(G);      // topologic order number
  int i=0;
  node v;
  forall_nodes(v,G) { top_ord[v] = i; N[i]=v; ++i; }


  // compute chain decomposition C[0], C[1], ..., C[num_chains-1]

  node_array<bool> un_marked(G, true);
  node_array<int> chain_id(G);        // chain_id[v] = i  iff  v in C[i]
  node_list* C = new node_list[n+1];  // chains C[0], C[1], ...
  
  int num_chains=0;
  for (i=0; i<n; ++i) { // increasing order
    node v = N[i];
    if (un_marked[v]) {
      do {
	C[num_chains].append(v); chain_id[v]=num_chains;
	un_marked[v] = false;
	
	node u = nil;
	node w;
	forall_adj_nodes(w,v) {
	  if (un_marked[w]) { 
	    u = w;
	    break; 
	  }
	}
	v = u;
      } while (v);
      ++num_chains;
    }
  }

  // reach[i][h] = min{j ; N[j] in C[h] and there is a path from N[i] to N[j]}
  int** reach = new int*[n];
  int h;
  for (i=0; i<n; ++i) {
    reach[i] = new int[num_chains];
    for (h=0; h<num_chains; ++h)
      reach[i][h] = n+1; // n+1 == infinity
  }

  edge_array<bool> redundant(G, true);
  
  // compute redundant edges and reach for the nodes
  for (i=n-1; i>=0; --i) {     // decreasing order
    node v = N[i];
    int* reach_i = reach[i];
    
    edge e;
    forall_out_edges(e,v) {    // increasing order of target(e)
      node w = target(e);
      int j = top_ord[w];
      if (j < reach_i[chain_id[w]]) {
	redundant[e] = false;
	for (int h=0; h<num_chains; ++h)
	  if (reach[j][h] < reach_i[h]) reach_i[h] = reach[j][h];
      }
    }
    reach_i[chain_id[v]] = i;
  }
  
  if (compute_closure) {
    G.del_all_edges();
    
    for (i=0; i<n; ++i) { 
      v = N[i];
      for(h=0; h<num_chains; ++h) {
	int r = reach[i][h];
	if (r < n+1) // r != infinity
	  for(node w = N[r]; w != nil; w = C[h].succ(w)) 
	    G.new_edge(v,w);
      }
    }
  }
  else {
    edge e;
    forall_edges(e, G)
      if (redundant[e]) G.del_edge(e);
  }
  
  for(h=0; h<num_chains; ++h) C[h].clear();
  delete[] C;

  for(i=0; i<n; ++i) delete[] reach[i];
  delete[] reach;

  delete[] N;

  return true;
}


inline istream& operator>>(istream& in, list<node>*) { return in; }

/*
  Reduces the problem of transitive closure/reduction in general graph
  to acyclic graph as described in 
  Alfred V. Aho, M. R. Garey, Jeffrey D. Ullman: 
  The Transitive Reduction of a Directed Graph. 
  SIAM J. Comput. 1(2): 131-137 (1972)
 */

static
void transitive_closure_or_reduction(graph& G, bool compute_closure)
{
  node v,w;
  edge e;
  int i,j;

  Delete_Loops(G);

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  node_array<int> compnum(G);
  int k = STRONG_COMPONENTS(G,compnum);

  if (k == n) { 
    // graph is acyclic => reduction to G1 can be avoided
    acyclic_transitive_closure_or_reduction(G, compute_closure);
    return;
  }

/* reduce graph G to graph G1 = (V',E') 
   with V' = { V[0],V[1],...,V[k] } = set of scc's of G
   and (V[i],V[j]) in E' iff there is an edge from V[i] to V[j] in G

   G1.inf(V[i]) = set of nodes v in G with v in scc i (i.e. compum[v] = i)
*/

  GRAPH<list<node>*,int> G1;

  G1.set_node_bound(n);
  G1.set_edge_bound(m);

  node*  V = new node[k];

  for(j=0; j<k; j++) V[j] = G1.new_node(new list<node>);

  forall_nodes(v,G) 
  { int i = compnum[v];
    G1[V[i]]->append(v);
   }
  
  forall_edges(e,G)
  { i = compnum[source(e)];
    j = compnum[target(e)];
    if (i!=j) G1.new_edge(V[i],V[j]);
   }

  Make_Simple(G1);  // eliminate parallel edges
  
  // compute transitive closure of acyclic graph G1

  acyclic_transitive_closure_or_reduction(G1, compute_closure);

  if (compute_closure) {
    G.del_all_edges();

    forall_nodes(v,G1)
      { list<node>* plv = G1[v];
      forall_adj_nodes(w,v)
	{ list<node>* plw = G1[w];
	list_item x,y;
	forall_items(x,*plv)
	  forall_items(y,*plw) 
	  G.new_edge(plv->inf(x),plw->inf(y));
	}
      }
  }
  else {
    G.del_all_edges();

    forall_nodes(v,G1) { 
      list<node>* plv = G1[v];

      // every scc of G is replaced by a hamiltonian cycle through all nodes
      // if the scc is non-trivial
      if (plv->size() > 1) {
	list_item x;
	forall_items(x, *plv) {
	  list_item y = plv->cyclic_succ(x);
	  G.new_edge(plv->inf(x),plv->inf(y));
	}
      }

      // for every edge (v,w) in G1 we pick representants in the corresp.
      // scc in G, say v' and w', and insert the (single) edge (v',w') in G
      forall_adj_nodes(w,v) { 
	list<node>* plw = G1[w];
	G.new_edge(plv->head(),plw->head());
      }
    }
  }

  forall_nodes(v,G1) delete G1[v];
  
  delete[] V;
}

// exported functions //////////////////////////////////////////////////////

GRAPH<node,edge> TRANSITIVE_CLOSURE(const graph& G0)
  // Note: edge infos are undefined!!!
{
  GRAPH<node,edge> G;
  CopyGraph(G, G0);
  transitive_closure_or_reduction(G, true);
  return G;
}

GRAPH<node,edge> TRANSITIVE_REDUCTION(const graph& G0)
  // Note: edge infos are undefined!!!
{
  GRAPH<node,edge> G;
  CopyGraph(G, G0);
  transitive_closure_or_reduction(G, false);
  return G;
}

void MAKE_TRANSITIVELY_CLOSED(graph& G)
{
  transitive_closure_or_reduction(G, true);
}

void MAKE_TRANSITIVELY_REDUCED(graph& G)
{
  transitive_closure_or_reduction(G, false);
}



LEDA_END_NAMESPACE
