
template<class NT>
NT MAX_FLOW_T(graph& G, node s, node t, const edge_array<NT>& lcap, 
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
/*
  max_flow<NT, graph> mf;
  mf.run(G, ss, tt, _ucap, ff_offset);
*/
  MAX_FLOW_T(G, ss, tt, _ucap, ff_offset);

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
      _ucap[e] -= ff_offset[e]; _ucap[e_rev] = ff_offset[e];
      reversal[e_rev] = e;
     }
  }

  edge_array<NT> _flow(G);

/*
  mf.run(G, s, t, _ucap, _flow);
*/
  MAX_FLOW_T(G, s, t, _ucap, _flow);

  // remove the reversal edges and update flow
  forall_edges(e,G)
  { edge e_rev = reversal[e];
    if (e_rev == nil) continue;
    _flow[e_rev] -= _flow[e];
    G.del_edge(e);
  }

  // compute the final flow
  forall_edges(e, G) 
      flow[e] = lcap[e] + ff_offset[e] + _flow[e];

  // and its value
  NT flow_val = 0;
  forall_out_edges(e, s) flow_val += flow[e];
  forall_in_edges(e, s) flow_val -= flow[e];

  return flow_val;
}

