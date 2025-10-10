/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  misc.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/geo/geo_alg.h>

LEDA_BEGIN_NAMESPACE

void Bounding_Box(const list<POINT>& L, POINT& pl, POINT& pb, 
                                        POINT& pr, POINT& pt)
{
  int n = L.size();
  int i = 0;

  if (n == 0) LEDA_EXCEPTION(1,"Bounding_Box: empty list.");

  array<POINT> A(n);
  POINT p;
  forall(p,L) A[i++] = p;

  A.sort(POINT::cmp_xy);
  pl = A[0];
  pr = A[n-1];

  A.sort(POINT::cmp_yx);
  pb = A[0];
  pt = A[n-1];
}


bool Is_Simple_Polygon(const list<POINT>& L)
{
  list<SEGMENT> seg_list; 

  list_item it;
  forall_items(it,L)
  { POINT p = L[it];
    POINT q = L[L.cyclic_succ(it)];
    seg_list.append(SEGMENT(p,q));
   }

  GRAPH<POINT,SEGMENT> G;

  SWEEP_SEGMENTS(seg_list,G);

  node v;
  forall_nodes(v,G)
    if (G.degree(v) != 2) return false;

  return true;

}


static bool P_is_nested_in_Q(const POLYGON& P, const POLYGON& Q)
/* precond: either P is nested in Q, or Q is nested in P, 
            or P and Q do not overlap (i.e. no common interior point, but
			maybe common vertices)
*/
{
	// try bounding boxes first
	POINT P_xmin, P_xmax, P_ymin, P_ymax;
	POINT Q_xmin, Q_xmax, Q_ymin, Q_ymax;

	P.bounding_box( P_xmin, P_ymin, P_xmax, P_ymax );
	Q.bounding_box( Q_xmin, Q_ymin, Q_xmax, Q_ymax );

	if (   P_xmin.xcoord() < Q_xmin.xcoord() || P_ymin.ycoord() < Q_ymin.ycoord()
		|| P_xmax.xcoord() > Q_xmax.xcoord() || P_ymax.ycoord() > Q_ymax.ycoord() )
	{
		return false;
	}


	POINT vertex;

	forall_vertices(vertex, P) {
		switch (Q.region_of(vertex)) {
			case BOUNDED_REGION:	return true;  // P is nested in Q
			case UNBOUNDED_REGION:	return false; // P is not nested in Q
			case ON_REGION:         break;        // bad luck, try next vertex
		}
	}

	/* All vertices of P lie on the boundary of Q. => P and Q do not overlap.
	   That does NOT mean that P is nested in Q: P might be a square 
	   and Q might be a 4-armed star whose tips coincide with the corners 
	   of P, then Q is nested in P.
     */

	forall_vertices(vertex, Q) {
		switch (P.region_of(vertex)) {
			case BOUNDED_REGION:	return false; // Q is nested in P
			case UNBOUNDED_REGION:	return true;  // P is nested in Q
			case ON_REGION:         break;        // bad luck, try next vertex
		}
	}

	/* We should reach this point iff P and Q are equal. */

	return true;
}

node Nesting_Tree(const GEN_POLYGON& GP, GRAPH<POLYGON, int>& T)
{
	T.clear();

	map<list_item, node> corresp_node;

	node root = T.new_node( POLYGON() );

	list<POLYGON> chains = GP.polygons();

	list_item it;
	forall_items(it, chains) {
		if (chains[it].empty())
			chains.del_item(it);
		else
			corresp_node[it] = T.new_node( chains[it] );
	}

	if (chains.empty()) return root;

	/* heuristics to avoid some nesting queries:
	   Let F be the first polygon in chains. Every polygon between F and the
	   first polygon with the same orientation as F must be directly nested
	   in F.
	*/
	int orient_of_first = chains.head().orientation();
	node node_of_first = corresp_node[ chains.first() ];
	T.new_edge(root, node_of_first);
	forall_items(it, chains) {
		if (chains[it].orientation() != orient_of_first) {
			// chains[it] must be nested in first
			node n = corresp_node[it];
			T.new_edge(node_of_first, n);
		}
		else if (it != chains.first()) {
			break;
		}
	}

	forall_rev_items(it, chains) {
		POLYGON P = chains[it];
		node n_P = corresp_node[it];

		if (T.indeg(n_P) > 0) break; // cf. heuristics

		node father = root;

		while((it = chains.pred(it))) {
			POLYGON Q = chains[it];

			if (P.orientation() != Q.orientation() && P_is_nested_in_Q(P, Q)) {
				father = corresp_node[it];
				break;
			}
		}

		T.new_edge(father, n_P);
	}

	return root;
}


LEDA_END_NAMESPACE
