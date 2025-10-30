/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _st_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/st_matching.h>
#include <LEDA/core/h_array.h> // we need forall_defined

LEDA_BEGIN_NAMESPACE

bool CheckStableMatching(const graph& G, const list<node>& A, const list<node>& B,
                          const list<edge>& M)
{ node_array<edge> mate(G,nil);
  // for each node in B the edge (in the direction from A to B) mating it

  node a,b; edge e;

  edge_array<int> rank(G);

  if ( !G.is_map() ) LEDA_EXCEPTION(1,"G must be a map");

  forall(b,B)
  { int r = 0;
    forall_adj_edges(e,b) rank[e] = rank[G.reversal(e)] = r++;
  }

  forall(e,M) mate[G.target(e)] = e;

  forall(a,A)
  { forall_adj_edges(e,a)
    { node b = G.target(e);
      edge e1 = mate[b];
      if ( e1 == nil ) LEDA_EXCEPTION(1,"M is not maximal");
      if ( e1 == e ) break;
      if ( rank[e] < rank[e1] ) LEDA_EXCEPTION(1,"M is not stable");
    }
  }
  return true;
}



void StableMatching(const graph& G, const list<node>& A,
                    const list<node>& B, list<edge>& M)
{ node_array<edge> mate(G,nil);
  // for each node in B the edge (in the direction from A to B) mating it

  node_array<edge> current_edge(G);
  edge_array<int> rank(G);

  if ( !G.is_map() ) LEDA_EXCEPTION(1,"G must be a map");

  node a, b; edge e;
  forall(a,A) current_edge[a] = G.first_adj_edge(a);

  forall(b,B)
  { int r = 0;
    forall_adj_edges(e,b) rank[e] = rank[G.reversal(e)] = r++;
    // the order in B gives the ranks
  }
//  forall_edges(e, G)
//  cout << G.index(e) << ": " << G.index(G.source(e)) << " " << G.index(G.target(e)) << " " << rank[e] << endl;

  list<node> free_in_A = A;

  while( ! free_in_A.empty() )
  { a = free_in_A.pop();
    e = current_edge[a];
    if ( e == nil ) continue;   // a is dead
    b = G.target(e);
    edge e1 = mate[b];
    if ( e1 == nil ) { mate[b] = e; continue; }
    node a1 = G.source(e1);

    edge e_good; node v_bad;

    if ( rank[e] < rank[e1] )
       { e_good = e; v_bad = a1; }
    else { e_good = e1; v_bad = a; }

    mate[b] = e_good;
    current_edge[v_bad] = G.adj_succ(current_edge[v_bad]);
    free_in_A.append(v_bad);
  }

  M.clear();
  forall(b,B) if ( mate[b] != nil ) M.append(mate[b]);


}


void CreateInputGraph(graph& G,
                      list<node>& A,
                      list<node>& B,
                      node_map<int>& nodes_a, // nodes of objects in A
                      node_map<int>& nodes_b, // nodes of objects in B
                      const list<int>& InputA,
                      const list<int>& InputB,
                      const map<int, list<int> >& preferencesA,
                      const map<int, list<int> >& preferencesB)
{
	// make sure that G owns the two node_maps
	if (nodes_a.get_owner() != &G) { nodes_a.init(G); }
	if (nodes_b.get_owner() != &G) { nodes_b.init(G); }

// cout << "Entering CreateInputGraph(g. list2, nmap2, list2, map2)" << endl;
	int aint=0, bint=0;
	edge e1, e2;
	h_array<int, h_array<int, int> > exists;
	// for each node of A store all partners
#if defined(__BORLANDC__)
	edge_array<int> order(G);
#else
	edge_map<int> order(G);
#endif
	map<int, node> node_of_a;
	map<int, node> node_of_b;
	//int i;
	int counter(0);
	forall(aint, InputA) // only nodes in A are considered
	{ if (preferencesA.defined(aint))
	  { forall(bint, preferencesA[aint])
	    { exists[aint][bint] = counter++; // multiple occurences are not tested!!
	      // the order of the partners is recorded by counter
	     }
           }
	 }

// cout << "Counter: " << counter << endl;
	int i1, i2;
	forall_defined(i1, exists)
	forall_defined(i2, exists[i1])
// cout << i1 << " : " << i2 << ": " << exists[i1][i2] << endl;
//int zaehler(0);
	forall(bint, InputB) // only nodes in B are considered
	{ if (preferencesB.defined(bint))
	  { forall(aint, preferencesB[bint])
	    { if(exists.defined(aint))
	      { if(exists[aint].defined(bint))
		 {
// cout << zaehler++ << ": " << aint << ", " << bint << endl;
		   if(!node_of_a.defined(aint))
		   { node v = G.new_node();
		     node_of_a[aint] = v;
		     A.append(v);
		     nodes_a[v] = aint;
		    }
	     	    if(!node_of_b.defined(bint))
		    { node v = G.new_node();
		      node_of_b[bint] = v;
		      B.append(v);
		      nodes_b[v] = bint;
		      }
// cout << "aint: " << aint << " bint: " << bint << endl;
				e1 = G.new_edge(node_of_a[aint], node_of_b[bint]);
				order[e1] = exists[aint][bint];
				// restore the order of the preferences of elements in A
				e2 = G.new_edge(node_of_b[bint], node_of_a[aint]);
				G.set_reversal(e1, e2);
				order[e2] = counter++;
				// the order of the partners is recorded by counter
			}
                        }
			}
		}
	}
// cout << "zaehler " << zaehler << endl;
	G.make_map();
	G.sort_edges(order);
	// arrange the order of the nodes
	// if ( e1 == nil ) LEDA_EXCEPTION(1,"M is not maximal");

// cout << "Leaving CreateInputGraph(g. list2, nmap2, list2, map2)" << endl;
}


LEDA_END_NAMESPACE
