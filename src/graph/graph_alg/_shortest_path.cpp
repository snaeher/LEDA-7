/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _shortest_path.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/shortest_path.h>
#include <LEDA/graph/templates/shortest_path.h>


LEDA_BEGIN_NAMESPACE


bool SHORTEST_PATH(const graph& G, node s, 
                     const edge_array<int>& c, 
                     node_array<int>& dist, 
                     node_array<edge>& pred )
{ return SHORTEST_PATH_T(G,s,c,dist,pred); }


bool SHORTEST_PATH(const graph& G, node s, 
                     const edge_array<double>& c, 
                     node_array<double>& dist, 
                     node_array<edge>& pred )
{ return SHORTEST_PATH_T(G,s,c,dist,pred); }


list<edge> COMPUTE_SHORTEST_PATH(const graph& G, node s, node t, 
								 const node_array<edge>& pred)
{
	list<edge> path;
	node cur = t;
	while (cur != s) {
		edge e = pred[cur];
		if (e == nil) {
			LEDA_EXCEPTION(1, "COMPUTE_SHORTEST_PATH: pred not valid for s");
			break;
		}
		path.push_front(e);
		cur = source(e);
	}
	return path;
}

node_array<int> CHECK_SP(const graph& G, node s, 
                           const edge_array<int>& c, 
                           const node_array<int>& dist,
                           const node_array<edge>& pred)
{ return CHECK_SP_T(G,s,c,dist,pred); }


node_array<int> CHECK_SP(const graph& G, node s, 
                           const edge_array<double>& c, 
                           const node_array<double>& dist,
                           const node_array<edge>& pred)
{ return CHECK_SP_T(G,s,c,dist,pred); }


void ACYCLIC_SHORTEST_PATH(const graph& G, node s, 
                             const edge_array<int>& c, 
                             node_array<int>& dist,
                             node_array<edge>& pred)
{ ACYCLIC_SHORTEST_PATH_T(G,s,c,dist,pred); }


void ACYCLIC_SHORTEST_PATH(const graph& G, node s, 
                             const edge_array<double>& c, 
                             node_array<double>& dist,
                             node_array<edge>& pred)
{ ACYCLIC_SHORTEST_PATH_T(G,s,c,dist,pred); }




void DIJKSTRA(const graph& G, node s, const edge_array<int>& cost,
                    node_array<int>& dist, node_array<edge>& pred)
{ DIJKSTRA_T(G,s,cost,dist,pred); }


void DIJKSTRA(const graph& G, node s, const edge_array<double>& cost,
                    node_array<double>& dist, node_array<edge>& pred)
{ DIJKSTRA_T(G,s,cost,dist,pred); }


void DIJKSTRA(const graph& G, node s, const edge_array<int>& cost,
                    node_array<int>& dist)
{ DIJKSTRA_T(G,s,cost,dist); }


void DIJKSTRA(const graph& G, node s, const edge_array<double>& cost,
                    node_array<double>& dist)
{ DIJKSTRA_T(G,s,cost,dist); }


int DIJKSTRA(const graph& G, node s, node t, 
              const edge_array<int>& c,
              node_array<edge>& pred)
{ return DIJKSTRA_T(G,s,t,c,pred); }


double DIJKSTRA(const graph& G, node s, node t, 
              const edge_array<double>& c,
              node_array<edge>& pred)
{ return DIJKSTRA_T(G,s,t,c,pred); }



int DIJKSTRA(const graph& G, node s, node t, 
              const edge_array<int>& c,
              node_array<int>& dist,
              node_array<edge>& pred)
{ return DIJKSTRA_T(G,s,t,c,dist,pred); }


double DIJKSTRA(const graph& G, node s, node t, 
              const edge_array<double>& c,
              node_array<double>& dist,
              node_array<edge>& pred)
{ return DIJKSTRA_T(G,s,t,c,dist,pred); }



bool BELLMAN_FORD_B(const graph& G, node s, 
                        const edge_array<int>& c, 
                        node_array<int>& dist, 
                        node_array<edge>& pred) 
{ return BELLMAN_FORD_B_T(G,s,c,dist,pred); }


bool BELLMAN_FORD_B(const graph& G, node s, 
                        const edge_array<double>& c, 
                        node_array<double>& dist, 
                        node_array<edge>& pred) 
{ return BELLMAN_FORD_B_T(G,s,c,dist,pred); }


bool BELLMAN_FORD(const graph& G, node s, 
                       const edge_array<int> & c,
                       node_array<int> & dist,
                       node_array<edge> & pred)
{ return BELLMAN_FORD_T(G,s,c,dist,pred); }


bool BELLMAN_FORD(const graph& G, node s, 
                       const edge_array<double> & c,
                       node_array<double> & dist,
                       node_array<edge> & pred)
{ return BELLMAN_FORD_T(G,s,c,dist,pred); }


bool ALL_PAIRS_SHORTEST_PATHS(graph& G, 
                              const edge_array<int>& c, 
                              node_matrix<int>& DIST)
{ return ALL_PAIRS_SHORTEST_PATHS_T(G,c,DIST); }

bool ALL_PAIRS_SHORTEST_PATHS(graph& G, 
                              const edge_array<double>& c, 
                              node_matrix<double>& DIST)
{ return ALL_PAIRS_SHORTEST_PATHS_T(G,c,DIST); }


void  MOORE(const graph& g, node s, const edge_array<int>& cost,
            node_array<int>& dist, node t )
{ MOORE_T(g,s,cost,dist,t); }

void  MOORE(const graph& g, node s, const edge_array<double>& cost,
            node_array<double>& dist, node t )
{ MOORE_T(g,s,cost,dist,t); }

// new functions

void report_path(list<list<edge>* >& paths, const stack<edge>& es)
{
	stack<edge> stack_copy(es);
	list<edge>* edge_list = new list<edge>;
	while(!stack_copy.empty())
	{
		(*edge_list).push_front(stack_copy.pop());
	}
	paths.append(edge_list);
}


//KSPR(int), using ANSPR0
// nops gives the number of paths, usually k, but in case there are more than k shortest paths or there are less than k paths, it reports their number
bool K_SHORTEST_PATHS(graph& G, node s, node t, const edge_array<int>& c, int k, list<list<edge>* >& shortest_paths, int& nops)
{
	if (k == 0) return true;
	int n = G.number_of_nodes();
	int high = n*maxval(G, c);
	int low = 0;
	double eps(0);
	// binary search in [low..high], starting with [0 .. n*c_max];
	// goal: low + 1 = high, number of paths for low is <= k, number for high > k
	list<list<edge>* > shortest_paths_local;
	node_array<int> dist(G);
	node_array<edge> pred(G);
	int nop; // number of enumerated paths

	SHORTEST_PATH_T(G, s, c, dist, pred);
	int Lm = dist[t];

	// than eps = low / L_min
	// if not more than k paths
	// eps = high / L_min
	// and so on

	NEAR_SHORTEST_PATHS_RESTRICTED(G, s, t, c, 0, shortest_paths, k); 

	if (shortest_paths.length() > k)
	{
		nops = shortest_paths.length();
		return true;
	}

	shortest_paths.clear();
	eps = double(high)/Lm;

	NEAR_SHORTEST_PATHS_RESTRICTED(G, s, t, c, eps, shortest_paths, k);
	
	if (shortest_paths.length() < k)
	{
		nops = shortest_paths.length();
		return false;
	}

	shortest_paths.clear();
	nops = k;
	
	while(high > low + 1)
	{
		int mid = int(std::floor(double((high - low)/2)));
		mid += low;
		eps = double(mid)/Lm;
		NEAR_SHORTEST_PATHS_RESTRICTED(G, s, t, c, eps, shortest_paths_local, k);
		if (shortest_paths_local.length() <= k)
		{
			low = mid;
		}
		else
			high = mid;
		shortest_paths_local.clear();
	}

	eps = double(low)/Lm;
	NEAR_SHORTEST_PATHS_RESTRICTED(G, s, t, c, eps, shortest_paths);
	nop = shortest_paths.length();
	if(nop == k)
		return true;
	else
	{
		eps = double(high)/Lm;
		NEAR_SHORTEST_PATHS_RESTRICTED(G, s, t, c, eps, shortest_paths_local,k);
		list<edge>* lp;

		int length(0);
		forall(lp, shortest_paths)
		{
			int pl = path_length(c, (*lp));
			if (pl > length)
				length = pl;
		}

		if (length < (Lm + low)) {
                   forall(lp, shortest_paths_local)
		   {
			if(path_length(c, (*lp)) == Lm + low)
			{
				
				list<edge>* nlp = new list<edge>;
				(*nlp) = *lp;
				shortest_paths.append(nlp);
				if (++nop == k)
					return true;
			}
		    }
                }

		forall(lp, shortest_paths_local)
			if(path_length(c, (*lp)) == Lm + high)
			{
				list<edge>* nlp = new list<edge>;
				(*nlp) = *lp;
				shortest_paths.append(nlp);
				if (++nop == k)
					return true;
			}
	}

	return true;
}

LEDA_END_NAMESPACE
