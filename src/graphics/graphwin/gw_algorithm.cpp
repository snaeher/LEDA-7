/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_algorithm.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/graph_draw.h>

#include <stdlib.h>

LEDA_BEGIN_NAMESPACE

// basic algorithms

int gw_components(GraphWin& gw, node_array<int>& compnum)
{ return COMPONENTS(gw.get_graph(),compnum); }

int gw_strong_components(GraphWin& gw, node_array<int>& compnum)
{ return STRONG_COMPONENTS(gw.get_graph(),compnum); }

int gw_biconnected_components(GraphWin& gw, 
                                    edge_array<int>& compnum)
{ return BICONNECTED_COMPONENTS(gw.get_graph(),compnum); }


list<edge> gw_spanning_tree(GraphWin& gw)
{ return SPANNING_TREE(gw.get_graph()); }

GRAPH<node,edge> gw_transitive_closure(GraphWin& gw)
{ return TRANSITIVE_CLOSURE(gw.get_graph()); }

GRAPH<node,edge> gw_transitive_reduction(GraphWin& gw)
{ return TRANSITIVE_REDUCTION(gw.get_graph()); }

void gw_make_transitively_closed(GraphWin& gw)
{ MAKE_TRANSITIVELY_CLOSED(gw.get_graph()); }

void gw_make_transitively_reduced(GraphWin& gw)
{ MAKE_TRANSITIVELY_REDUCED(gw.get_graph()); }



bool gw_is_planar(GraphWin& gw) { return PLANAR(gw.get_graph()); }

int gw_kuratowski(GraphWin& gw, list<node>& V, list<edge>& E, 
                                      node_array<int>& deg)
{ return KURATOWSKI(gw.get_graph(),V,E,deg); }


void gw_straight_line_embedding(GraphWin& gw, node_array<double>& x,
                                              node_array<double>& y)
{ STRAIGHT_LINE_EMBEDDING(gw.get_graph(),x,y); }


void gw_d3_spring_embedding(const graph& G, node_array<double>& xpos, 
                                            node_array<double>& ypos,
                                            node_array<double>& zpos,
                                            double xmin, double xmax, 
                                            double ymin, double ymax,
                                            double zmin, double zmax,
                                            int n)
{ D3_SPRING_EMBEDDING(G,xpos,ypos,zpos,xmin,xmax,ymin,ymax,zmin,zmax,n); }


int gw_max_flow(GraphWin& gw, node s, node t, const edge_array<int>& cap, 
                                       edge_array<int>& flow)
{ return MAX_FLOW(gw.get_graph(),s,t,cap,flow); }


int gw_min_cost_flow(GraphWin& gw, const edge_array<int>& lcap, 
                                         const edge_array<int>& ucap,
                                         const edge_array<int>& cost,
                                         const node_array<int>& supply,
                                         edge_array<int>& flow)
{ return MCF_COST_SCALING(gw.get_graph(),lcap,ucap,cost,supply,flow); }



int gw_min_cost_max_flow(GraphWin& gw, node s, node t, 
                                         const edge_array<int>& cap, 
                                         const edge_array<int>& cost,
                                         edge_array<int>& flow)
{ return MCMF_COST_SCALING(gw.get_graph(),s,t,cap,cost,flow); }





static int mflow1(graph& G, node s, node t, const edge_array<int>& cap,
                                                  edge_array<int>& flow)
{ return MAX_FLOW(G,s,t,cap,flow); }


typedef int (*flowfunc)(graph&, node, node, const edge_array<int>&,
                                                   edge_array<int>& flow);

static void run_maxflow(GraphWin& gw, flowfunc MAXFLOW)
{ graph& G = gw.get_graph();
  if (G.empty()) return;
  node s = G.first_node();
  node t = G.last_node();
  edge_array<int> cap(G);
  edge_array<int> flow(G);
  edge e;
  forall_edges(e,G) cap[e] = atoi(gw.get_user_label(e));

  int f = MAXFLOW(G,s,t,cap,flow);

  forall_edges(e,G) 
  { string s = gw.get_user_label(e);
    gw.set_user_label(e,string("%d/%d",cap[e],flow[e]));
   }

  gw.message(string("flow = %d",f));
  gw.redraw();

  CHECK_MAX_FLOW(G,s,t,cap,flow);
}

void gw_maxflow(GraphWin& gw)  { run_maxflow(gw,mflow1); }


LEDA_END_NAMESPACE

