/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _max_flow.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/list.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/max_flow.h>
#include <LEDA/graph/scale_weights.h>


LEDA_BEGIN_NAMESPACE

bool MAX_FLOW_SCALE_CAPS(const graph& G, node s, edge_array<double>& cap)
{ return scale_weights(G,cap,cap,(double)G.outdeg(s)); }


bool CHECK_MAX_FLOW(const graph& G, node s, node t, const edge_array<int>& cap,
                                                    const edge_array<int>& f)
{ return CHECK_MAX_FLOW_T(G,s,t,cap,f); }


bool CHECK_MAX_FLOW(const graph& G, node s, node t,
                                            const edge_array<double>& cap,
                                            const edge_array<double>& f)
{ return CHECK_MAX_FLOW_T(G,s,t,cap,f); }


LEDA_END_NAMESPACE


#if (defined(__SUNPRO_CC) && __SUNPRO_CC <= 0x550) || (defined(_MSC_VER) && _MSC_VER < 1300) || defined(__BORLANDC__) || defined(__mipspro__) || defined(__DMC__) || defined(__HP_aCC)

// no static graphs available use  MAX_FLOW_T - functions on std graph

#include <LEDA/graph/templates/max_flow.h>


LEDA_BEGIN_NAMESPACE

int MAX_FLOW(const graph& G, node s, node t, const edge_array<int>& cap,
                                             edge_array<int>& f)
{ int W = MAXINT/(1 + G.outdeg(s));
  check_weights(G,cap,-W,+W,"MAX_FLOW<int>");
  return MAX_FLOW_T(G,s,t,cap,f); 
}


double MAX_FLOW(const graph& G, node s, node t, const edge_array<double>& cap,
                                                edge_array<double>& f)
{ edge_array<double> cap1(G);
  scale_weights(G,cap,cap1,(double)G.outdeg(s));
  return MAX_FLOW_T(G,s,t,cap1,f); 
}


int MAX_FLOW(const graph& G, node s, node t, const edge_array<int>& cap,
                                             edge_array<int>& f, 
                                             list<node>& st_cut)
{ //int W = MAXINT/(1 + G.outdeg(s));
  //check_weights(G,cap,-W,+W,"MAX_FLOW<int>");
  return MAX_FLOW_T(G,s,t,cap,f,st_cut); 
}


double MAX_FLOW(const graph& G, node s, node t, const edge_array<double>& cap,
                                                edge_array<double>& f, 
                                                list<node>& st_cut)
{ edge_array<double> cap1(G);
  scale_weights(G,cap,cap1,(double)G.outdeg(s));
  return MAX_FLOW_T(G,s,t,cap1,f,st_cut); 
}


LEDA_END_NAMESPACE

#else

#define MF_STATIC_GRAPH
#include <LEDA/graph/templates/max_flow.h>

// construct a static copy and use max_flow class template 

#include <LEDA/graph/static_fgraph.h>
#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/edge_slot.h>
#include <LEDA/graph/node_slot1.h>
#include <LEDA/graph/edge_slot1.h>


LEDA_BEGIN_NAMESPACE

template<class st_graph, class st_node, class _cap_array, class cap_array>
void build_network(const graph& _gr, node _s, node _t, const _cap_array& _cap,
                   st_graph& G, st_node& s, st_node& t, cap_array& cap)  
{

  typedef typename st_graph::edge st_edge;
  //typedef typename _cap_array::value_type NT;

  node_array<st_node> V(_gr);

  int n = _gr.number_of_nodes();
  int m = _gr.number_of_edges();

  G.start_construction(n,m);

  node vv;
  forall_nodes(vv,_gr) V[vv] = G.new_node();

  forall_nodes(vv,_gr) 
  { st_node v = V[vv];
    edge x;
    forall_out_edges(x,vv)
    { st_node w = V[_gr.target(x)];
      st_edge e = G.new_edge(v,w);
      cap[e] = _cap[x];
     }
   }

  G.finish_construction();

  s = V[_s];
  t = V[_t];
}

template<class st_graph, class _flow_array, class flow_array>
void assign_flow(const graph& _gr, _flow_array& _flow,
                 const st_graph& G, const flow_array& flow)  
{
  //typedef typename st_graph::node st_node;
  typedef typename st_graph::edge st_edge;

  st_edge e = G.first_edge();

  node vv;
  forall_nodes(vv,_gr)
  { edge x; 
    forall_out_edges(x,vv) 
    { _flow[x] = flow[e];
      e = G.succ_edge(e);
     }
   }

}



template<class NT>
NT MAX_FLOW_NT(const graph& _gr, node _src, node _tgt, 
                                          const edge_array<NT>& _cap,
                                          edge_array<NT>& _flow,
                                          list<node>* cut_ptr)
{
  typedef typename max_flow<NT>::node_data nd_type;
  typedef typename max_flow<NT>::edge_data ed_type;
  
  const int d = sizeof(NT)/4;

  const int ns = sizeof(nd_type)/4;
  const int es = sizeof(ed_type)/4;
  
  typedef 
     static_fgraph<opposite_graph,data_slots<ns>,data_slots<es+2*d> > st_graph;
  typedef typename st_graph::node st_node;
//typedef typename st_graph::edge st_edge;
  
  typedef node_slot1<nd_type,st_graph,0> node_data_array;
  typedef edge_slot1<ed_type,st_graph,0> edge_data_array;

  st_graph G;
  st_node s,t;
  edge_slot1<NT,st_graph,es+0> cap(G);
  edge_slot1<NT,st_graph,es+d> flow(G);

  build_network(_gr,_src,_tgt,_cap, G,s,t,cap);

  max_flow<NT, st_graph, node_data_array, edge_data_array> mf;
  NT fval = mf.run(G,s,t,cap,flow);

  assign_flow(_gr,_flow,G,flow);

  if (cut_ptr)
  { list<st_node> cut;
    mf.compute_min_st_cut(G,s,t,cap,flow,cut);
    node_array<bool,st_graph> in_cut(G,false);
    st_node v;
    forall(v,cut) in_cut[v] = true;
    node vv = _gr.first_node();
    forall_nodes(v,G)
    { if (in_cut[v]) cut_ptr->append(vv);
      vv = _gr.succ_node(vv);
     }
   }

  return fval;
}




int MAX_FLOW(const graph& G, node s, node t, const edge_array<int>& cap,
                                              edge_array<int>& flow)
{ return MAX_FLOW_NT(G,s,t,cap,flow,0); }

int MAX_FLOW(const graph& G, node s, node t, const edge_array<int>& cap,
                                              edge_array<int>& flow,
                                              list<node>& cut)
{ return MAX_FLOW_NT(G,s,t,cap,flow,&cut); }



double MAX_FLOW(const graph& G, node s, node t, const edge_array<double>& cap,
                                                edge_array<double>& flow)
{ edge_array<double> cap1(G);
  scale_weights(G,cap,cap1,(double)G.outdeg(s));
  return MAX_FLOW_NT(G,s,t,cap1,flow,0); 
}

double MAX_FLOW(const graph& G, node s, node t, const edge_array<double>& cap,
                                                edge_array<double>& flow,
                                                list<node>& cut)
{ edge_array<double> cap1(G);
  scale_weights(G,cap,cap1,(double)G.outdeg(s));
  return MAX_FLOW_NT(G,s,t,cap1,flow,&cut); 
}

LEDA_END_NAMESPACE


#endif


LEDA_BEGIN_NAMESPACE

// max flow with lower capacities

template<class NT>
NT MAX_FLOW_LCAPS(graph& G, node s, node t, const edge_array<NT>& lcap, 
                                            const edge_array<NT>& ucap, 
                                            edge_array<NT>& flow)
{
  // setup auxiliary graph

  // add new source and target
  node ss = G.new_node();
  node tt = G.new_node(); 

  node_array<NT> excess(G, 0);
  edge_map<NT> _ucap(G);
  edge e;
  forall_edges(e, G) 
  { excess[source(e)] -= lcap[e];
    excess[target(e)] += lcap[e];
    _ucap[e] = ucap[e] - lcap[e];
   }

  NT total = 0;
  node n;
  forall_nodes(n, G) 
  { 
    if (n == ss || n == tt) continue;

    if (excess[n] > 0) 
    { _ucap[ G.new_edge(ss, n) ] = excess[n];
      // excess is simulated by ss -> ss tries to push 
      // excess[n] units into the network
      total += excess[n];
    }
    else 
    { _ucap[ G.new_edge(n, tt) ] = -excess[n];
      // demand is simulated by tt -> tt tries to withdraw 
      // -excess[n] from the network
    }
  }

  edge e_ts = G.new_edge(t, s); 
  _ucap[e_ts] = total; // infinity loop from t -> s

  // compute max flow in auxiliary graph
  edge_array<NT> ff_offset(G);

  MAX_FLOW(G, ss, tt, _ucap, ff_offset);

  // check feasibility (i.e. ss succeeds to push everything, 
  // and tt is able to withdraw all it wants to)

  bool feasible = true;
  forall_out_edges(e, ss) 
     if (_ucap[e] != ff_offset[e]) { feasible = false; break; }

  if (feasible) 
  {  forall_in_edges(e, tt)
     if (_ucap[e] != ff_offset[e]) { feasible = false; break; }
   }

  G.del_node(ss); 
  G.del_node(tt); 
  G.del_edge(e_ts);

  if (!feasible) return -1;

  // feasible flow: ff[e] = ff_offset[e] + lcap[e]

  // prepare final max flow computation
  edge_map<edge> reversal(G,nil);

  forall_edges(e, G) 
  { if (ff_offset[e] > 0) 
    { edge e_rev = G.new_edge(target(e), source(e));
      _ucap[e] -= ff_offset[e];
      _ucap[e_rev] = ff_offset[e];
      reversal[e_rev] = e;
     }
  }

  edge_array<NT> _flow(G);

  MAX_FLOW(G, s, t, _ucap, _flow);

  // remove the reversal edges and update flow
  forall_edges(e,G)
  { edge e_rev = reversal[e];
    if (e_rev == nil) continue;
    _flow[e_rev] -= _flow[e];
    G.del_edge(e);
  }

  // compute the final flow
  forall_edges(e, G) flow[e] = lcap[e] + ff_offset[e] + _flow[e];

  // and its value
  NT flow_val = 0;
  forall_out_edges(e, s) flow_val += flow[e];
  forall_in_edges(e, s) flow_val -= flow[e];

  return flow_val;
}


int MAX_FLOW(graph& G, node s, node t, const edge_array<int>& lcap, 
                                       const edge_array<int>& ucap,
                                       edge_array<int>& f)
{ return MAX_FLOW_LCAPS(G,s,t,lcap,ucap,f); }


double MAX_FLOW(graph& G, node s, node t, const edge_array<double>& lcap, 
                                          const edge_array<double>& ucap,
                                          edge_array<double>& f)
{ edge_array<double> ucap1(G);
  scale_weights(G,ucap,ucap1,(double)G.outdeg(s));
  edge_array<double> lcap1(G);
  scale_weights(G,lcap,lcap1,(double)G.outdeg(s));
  return MAX_FLOW_LCAPS(G,s,t,lcap1,ucap1,f); 
}


LEDA_END_NAMESPACE
