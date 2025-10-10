/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _ortho.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include "ortho_map.h"
#include <LEDA/graph/face_map.h>
#include <LEDA/graph/dimacs.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/set.h>


/*
#define MAXCAP MAXINT
#define MAXCAP (1<<16)
*/

#define MAXCAP 1000


LEDA_BEGIN_NAMESPACE


int ORTHO_EMBEDDING(const graph& G, const node_array<bool>& crossing,
                                    const edge_array<int>& bound,
                                    node_array<int>& x_pos,
                                    node_array<int>& y_pos,
                                    edge_array<list<int> >& x_bends,
                                    edge_array<list<int> >& y_bends)
{

  // crossing[v]: handle v as a crossing (degree == 4)

  int n = G.number_of_nodes();

  if (n == 0)
    return 0;

  if (n == 1) {
    node v = G.first_node();
    x_pos[v] = 0;
    y_pos[v] = 0;
    return 0;
  }

  ortho_map P(G);

  //set bound for maximal number of bends on edges of P
  edge_map<int>curr_bound(P, MAXINT);
  edge e;
  forall_edges(e, P) curr_bound[e] = bound[P.get_orig(e)];

  //Transformation into a 4-graph
  node_map<edge> corr_cage_edge(P);  //store one edge of cage that replaced big node 
  edge_array<list<node> > b_nodes(G);  
  edge_array<node> b_nodes_first(G, NULL);  
  edge_array<node> b_nodes_last(G, NULL);

  list<node> all_nodes = P.all_nodes();

  node v;
  forall(v, all_nodes) { 
    if (outdeg(v) <= 4) continue;

    P.set_type(v, v_big);
    int d = outdeg(v), i = 0;
    array<edge> out(d);
    edge e_cage = 0;

	//split all out edges e of v
    forall_out_edges(e, v) {
      out[i++] = e;
      edge e_orig = P.get_orig(e);
      edge e_split = P.split_map_edge(e); 
      node c_i = source(e_split);
      P.set_type(c_i, v_bend);
      b_nodes_first[e_orig] = c_i;
      b_nodes_last[G.reversal(e_orig)] = c_i;
    }

	//insert cage edges
    for (i = 0; i<d; i++) {
      e_cage = P.new_edge(SUCC(out[i]), REV(out[(i + 1) % d]));
      P.init_maps(e_cage, 90, EPS, true, true);
      edge r_cage = P.reversal(e_cage);
      P.init_maps(r_cage, 90, EPS, true, false);
    }
    
	//remove star inside cage
    corr_cage_edge[v] = e_cage;
    forall_out_edges(e, v) P.join_faces(e);

  }
  //cages created


  //transform into network flow problem

  P.compute_faces();

  //choose outer face as biggest face that is no cage
  face f_0 = 0;
  int f_0_deg = 0;

  face f;
  forall_faces(f, P)
    if (!IS_CAGE(f) && P.size(f)> f_0_deg) {
    f_0 = f;
    f_0_deg = P.size(f_0);
  }

  forall_face_edges(e, f_0) P.set_inner(e, false);

  //define network
  graph N;  
  node s, t;  


  list<node> V_hat;
  list<node> F;
  node_map<node> NtoV(N);
  node_map<face> NtoF(N);
  face_map<node> FtoN(P);

  //new source and target
  s = N.new_node();
  t = N.new_node();

  //all nodes of P with outdeg <= 3
  forall_nodes(v, P) if (outdeg(v)> 0 && outdeg(v) <= 3) {
    node n = N.new_node();
    NtoV[n] = v;
    V_hat.append(n);
  }

  //all faces
  forall_faces(f, P) {
    node n = N.new_node();
    NtoF[n] = f;
    FtoN[f] = n;
    F.append(n);
  }
    
  //edges from s to all inner faces of P with <= 3 edges on boundary
  //edges to t from all faces with >= 5 edges on boundary
  list<edge> A_s_v, A_s_f, A_v, A_v_cage, A_f, A_f_t;

  list<edge> A_x;

  forall_faces(f, P) {
    int size = P.size(f);
	if (!IS_OUTER(f) && size <= 3) {
      A_s_f.append(N.new_edge(s, FtoN[f]));
	}
	if (IS_OUTER(f) || size>= 5) {
      A_f_t.append(N.new_edge(FtoN[f], t));
	}
  }

  //edges from s to all nodes representing nodes of P and
  forall(v, V_hat) {A_s_v.append(N.new_edge(s, v));}
 
  //edges from all nodes representing nodes of P to 
  //nodes representing adjacent faces
  forall(v, V_hat) {

  /* modification by Thomas Ziegler (27-Aug-2002)
     replace list<face>  by set<face>  to avoid multiple 
     occurences of the same face in F_adj
   */

    set<face> F_adj;  
    forall_adj_faces(f, NtoV[v]) F_adj.insert(f);

/*
    list<face> F_adj;  
    forall_adj_faces(f, NtoV[v]) F_adj.append(f);
*/

    forall(f, F_adj)
    { edge e = N.new_edge(v,FtoN[f]);
      if (IS_CAGE(f))
        A_v_cage.append(e);
      else
        A_v.append(e);
     }
  }

#define MAX_BENDS_PER_EDGE MAXINT

   //edges between adjacent faces, loops for bridges
  edge_array<bool> marked(P, false);
  edge_map<edge> partner(N), a_in_P(N);
  edge a1, a2;
  forall_faces(f, P) 
  { edge e;
    forall_face_edges(e, f) {
   
    if (marked[e]) continue;

    face g = P.face_of(REV(e));
    if (f == g) {
      // bridge
      a1 = N.new_edge(FtoN[f], FtoN[f]);
      A_f.append(a1);
      partner[a1] = a1;
      a_in_P[a1] = e;
    }
    else {
      a1 = N.new_edge(FtoN[f], FtoN[g]);
      a2 = N.new_edge(FtoN[g], FtoN[f]);
      A_f.append(a1);
      A_f.append(a2);
      partner[a1] = a2;
      partner[a2] = a1;  
      a_in_P[a1] = e;
      a_in_P[a2] = e;

      bool s_crossing = crossing[P.get_orig(source(e))];
      bool t_crossing = crossing[P.get_orig(target(e))];

      if (s_crossing != t_crossing)
      { edge a3;
        if (s_crossing)
          { a3 = N.new_edge(FtoN[f], FtoN[g]);
            partner[a3] = a1;
           }
        else
         { a3 = N.new_edge(FtoN[g], FtoN[f]);
           partner[a3] = a2;
          }

     //cout << "special edge: "; N.print_edge(a3); cout << endl;

       A_x.append(a3);
       a_in_P[a3] = e;

     }

    //cout <<endl;

      edge x;
      for(x = e; P.face_of(REV(x)) == g && !marked[x]; x = SUCC(x))
         marked[x] = marked[REV(x)] = true;

      for(x = PRED(e); P.face_of(REV(x)) == g && !marked[x]; x = PRED(x))
         marked[x] = marked[REV(x)] = true;
     }
   }

  }

  //assign capacities and costs

  edge_array<int> cap(N,0);
  edge_array<int> cost(N,0);
  edge_array<int> l(N,0);
/*
  cap.init(N, 0);
  cost.init(N, 0);
  l.init(N, 0);
*/

/*
  { edge e;
    forall_edges(e,N) 
    { cap[e] = 0;
      cost[e] = 0;
      l[e] = 0;
     }
  }
*/

  edge a;
  forall(a, A_s_f) cap[a] = 4 - P.size(NtoF[target(a)]);
  forall(a, A_s_v) cap[a] = 4 - outdeg(NtoV[target(a)]);
  forall(a, A_v) cap[a] = 3;
  forall(a, A_v_cage) {
    cap[a] = 3;
    l[a] = 1;
  }

  forall(a, A_f) {
    face g = NtoF[target(a)];
    face f = NtoF[source(a)];
    cap[a] = IS_CAGE(g) ? 0 : curr_bound[a_in_P[a]];
    cost[a] = IS_CAGE(f) ? 0 : 1;
  }

  forall(a, A_x) {
    cap[a] = 1;
    cost[a] = 0;
  }

  forall(a, A_f_t) {
    face f = NtoF[source(a)];
    if (IS_OUTER(f))
      cap[a] = P.size(f) + 4;
    else
      cap[a] = P.size(f) - 4;
  }


  int z_s = 0;
  forall_out_edges(a, s) z_s += cap[a];

  int z = 0;  //flow value
  forall_in_edges(a, t) z += cap[a];

  A_v.conc(A_v_cage);

  if (z != z_s)
  { cout << "z   = " << z   << endl;
    cout << "z_s = " << z_s << endl;
    LEDA_EXCEPTION(1, ERR_INVALID_NETWORK);
   }

  edge_array<int> flow(N);
  node_array<int> supply(N, 0);
  supply[s] = z;
  supply[t] = -z;

  //compute minimum cost flow

  bool feasible = MIN_COST_FLOW(N, l, cap, cost, supply, flow);

  if (!feasible)  
  { ofstream out("ortho0.dimacs");
    Write_Dimacs_MCF(out,N,supply,l,cap,cost);
    //LEDA_EXCEPTION(0, ERR_NO_FEASIBLE_FLOW);
    LEDA_EXCEPTION(0, "No feasible flow");
    return -1;
  }

forall(a, A_x) {
   int f = flow[a];
   cout << "f = " << f << endl;
   flow[partner[a]] += f;
}
  
  forall(a, A_v) {
    node v = NtoV[source(a)];
    face f = NtoF[target(a)];
    edge e_in;
    bool flag = false;
    forall_in_edges(e_in, v)
      if (P.face_of(e_in) == f && !flag) {
      P.set_a(e_in, (flow[a] + 1) * 90);
      flag = true;
    }
  }

  marked.init(N, false);
  int no_of_bends = 0;
  forall(a, A_f) if (!marked[a]) {
    edge a_rev = partner[a];
    marked[a] = true;
    marked[a_rev] = true;
    e = a_in_P[a];

    bool bridge = (a == a_rev);

    P.set_s(e, flow[a], flow[a_rev], bridge);	
    P.set_rev_s(REV(e), flow[a], flow[a_rev], bridge);	
    no_of_bends += (flow[a] + flow[a_rev]);
  }

  //Replace bends by dummy nodes
  marked.init(P, false);
  list<edge> all_edges = P.all_edges();
  forall(e, all_edges) if (!marked[e]) {
    marked[e] = true;
    marked[REV(e)] = true;
    edge e_orig = P.get_orig(e);
    while (P.get_s(e) != EPS) {
      edge e_split = P.split_bend_edge(e);
      if (e_orig) {
        b_nodes[e_orig].push(source(e_split));
        b_nodes[G.reversal(e_orig)].append(source(e_split));
      }
    }
    if (e_orig) {
      if (b_nodes_first[e_orig]) {
        b_nodes[e_orig].push(b_nodes_first[e_orig]);
        b_nodes[G.reversal(e_orig)].append(b_nodes_first[e_orig]);
      }
      if (b_nodes_last[e_orig]) {
        b_nodes[e_orig].append(b_nodes_last[e_orig]);
        b_nodes[G.reversal(e_orig)].push(b_nodes_last[e_orig]);
      }
    }
  }

  //Replace Regions by Rectangles

  //build list all_faces
  list<face> all_faces;  
  forall_faces(f, P) if (IS_OUTER(f))
    all_faces.push(f);
  else {
    if (IS_CAGE(f)) {
      int pred_angle = P.get_a(PRED(P.first_face_edge(f)));
      forall_face_edges(e, f) {
        int act_angle = P.get_a(e);
        if (pred_angle == 90 && act_angle == 90) {
          edge e_split = P.split_map_edge(e);
          P.set_a(e, 180);
          P.set_a(REV(e_split), 180);
          P.set_type(source(e_split), v_dissection);
        }
        pred_angle = P.get_a(e);
      }
    }
    all_faces.append(f);
  }

  forall(f, all_faces) {
    stack<edge> S;
    int size = 0;
	forall_face_edges(e, f) size += std::abs(2 - P.get_a(e) / 90);
    e = P.succ_corner_edge(P.first_face_edge(f));
    int state = 0;



//cout << "size = " << size << endl;

    while (size > 4) {

//cout << "size = " << size << "   state = " << state << endl;

      if (state == 0 && !S.empty()) {
        e = P.succ_corner_edge(S.top());
        state = 1;
      }

      int angle = P.get_a(e);
      switch (angle) {
        case 90:
          if (state == 2)
            state = 3;
          if (state == 1)
            state = 2;
          break;
        case 270:
        case 360:
          if (state == 2 && IS_OUTER(f))
            state = 4;
          else {
			S.push(e);
            state = 1;
            if (angle == 360) {
              S.push(e);
			}
          }
          break;
      }

      if (state == 3) 
      { edge e1 = S.pop();
        edge e2 = P.succ_corner_edge(e1);
        edge e3 = P.succ_corner_edge(e2);
        edge e4 = P.succ_corner_edge(e3);
        int a4 = P.get_a(e4);
        edge e5 = P.split_map_edge(e4);
        P.set_type(source(e5), v_dissection);
        P.set_a(e1, P.get_a(e1) - 90);
        P.set_a(e5, a4);
        P.set_a(REV(e5), 180);
        P.set_a(e4, 90);
        edge e6 = P.new_edge(P.face_cycle_succ(e1), e5);
        P.init_maps(e6, 90, EPS, P.get_inner(e1), false);
        P.init_maps(REV(e6), 90, EPS, true, false);
        P.set_inner(e2, true);
        P.set_inner(e3, true);
        P.set_inner(e4, true);
        size -= 2;
        state = 0;
        e = e6;
      }

      if (state == 4) 
      {	
        edge e1 = S.pop();
        edge e2 = P.succ_corner_edge(e1);
        edge e3 = P.succ_corner_edge(e2);
        edge e4 = P.new_edge(P.face_cycle_succ(e1), P.face_cycle_succ(e3));
        P.init_maps(e4, P.get_a(e3) - 90, "1", false, false);
        P.init_maps(REV(e4), 90, "0", true, false);
        edge e5 = P.split_bend_edge(e4);
        P.set_a(e1, P.get_a(e1) - 90);
        P.set_inner(e2, true);
        P.init_maps(e3, 90, EPS, true, false);
        P.set_type(source(e5), v_dissection);
        S.push(e4);
        size -= 2;
        state = 0;
      }

      e = P.succ_corner_edge(e);
    }
 
  }

 
  P.init_rest();
  P.assign_directions(P.first_edge(), north);

  graph N_h, N_v;
  face_map<node> FtoN_h(P), FtoN_v(P);
  edge_map<edge> EtoA(P);

  node s_h = N_h.new_node(), t_h = N_h.new_node(), s_v = N_v.new_node(),
   t_v = N_v.new_node();

  forall_faces(f, P) {
    if (!IS_OUTER(f)) {
      FtoN_h[f] = N_h.new_node();
      FtoN_v[f] = N_v.new_node();
    }
  }

  forall_faces(f, P) forall_face_edges(e, f) {
    face g = P.face_of(REV(e));
    bool out_f = IS_OUTER(f), out_g = IS_OUTER(g);
    switch (P.get_dir(e)) {
      case north:
        if (out_f && !out_g)
          EtoA[e] = N_h.new_edge(FtoN_h[g], t_h);
        if (out_g && !out_f)
          EtoA[e] = N_h.new_edge(s_h, FtoN_h[f]);
        if (out_f && out_g)
          EtoA[e] = N_h.new_edge(s_h, t_h);
        if (!out_f && !out_g)
          EtoA[e] =
            N_h.new_edge(FtoN_h[g], FtoN_h[f]);
        break;
      case west:
        if (out_f && !out_g)
          EtoA[e] = N_v.new_edge(FtoN_v[g], t_v);
        if (out_g && !out_f)
          EtoA[e] = N_v.new_edge(s_v, FtoN_v[f]);
        if (out_f && out_g)
          EtoA[e] = N_v.new_edge(s_v, t_v);
        if (!out_f && !out_g)
          EtoA[e] =
            N_v.new_edge(FtoN_v[g], FtoN_v[f]);
        break;
      default:
        break;
    }
  }

  edge e_h = N_h.new_edge(s_h, t_h);
  edge e_v = N_v.new_edge(s_v, t_v);

  edge_array<int> cap_h(N_h, MAXCAP);
  edge_array<int> cap_v(N_v, MAXCAP);

  edge_array<int> cost_h(N_h, 1);
  edge_array<int> cost_v(N_v, 1);

  edge_array<int> l_h(N_h, 1);
  edge_array<int> l_v(N_v, 1);

  edge_array<int> flow_h(N_h, 0);
  edge_array<int> flow_v(N_v, 0);

  node_array<int> b_h(N_h, 0);
  node_array<int> b_v(N_v, 0);

  forall_nodes(v, N_h)
	  b_h[s_h] += std::abs(outdeg(v)-indeg(v));

  b_h[s_h] =  b_h[s_h]/2 + 1;
  b_h[t_h] = -b_h[s_h];

  forall_nodes(v, N_v)
	  b_v[s_v] += std::abs(outdeg(v)-indeg(v));

  b_v[s_v] =  b_v[s_v]/2 + 1;
  b_v[t_v] = -b_v[s_v];

  cost_h[e_h] = 0;
  cost_v[e_v] = 0;

  l_h[e_h] = 0;
  l_v[e_v] = 0;

 
  /* modification by Thomas Ziegler (23-Aug-2002)
     problem: MIN_COST_FLOW() does not work for loops with lower bounds.
     We set the flow on loops manually to one and hide the 
     loops befor calling MIN_COST_FLOW().
   */

  list<edge> loop;
  forall_edges(a,N_h) 
  { if (source(a)==target(a)) 
    { loop.append(a);
      flow[a]=1;
      N_h.hide_edge(a);
     }
  }

  //flow on loops is one, and loops are hidden

  bool feasible1 = MIN_COST_FLOW(N_h, l_h, cap_h, cost_h, b_h, flow_h);

  N_h.restore_all_edges();  //make loops visible again

  if (!feasible1)
  { ofstream out("ortho1.dimacs");
    Write_Dimacs_MCF(out,N_h,b_h,l_h,cap_h,cost_h);
    //LEDA_EXCEPTION(0, ERR_NO_FEASIBLE_FLOW);
    LEDA_EXCEPTION(0, "No feasible flow1");
    return -1;
   }

  forall_edges(a,N_v) 
  { if (source(a)==target(a)) 
    { loop.append(a);
      flow[a]=1;
      N_v.hide_edge(a);
     }
  }

  //flow on loops is one, and loops are hidden 

  bool feasible2 = MIN_COST_FLOW(N_v, l_v, cap_v, cost_v, b_v, flow_v);

  N_v.restore_all_edges();  //make loops visible again

  if (!feasible2 ) 
  { ofstream out("ortho2.dimacs");
    Write_Dimacs_MCF(out,N_v,b_v,l_v,cap_v,cost_v);
    //LEDA_EXCEPTION(0, ERR_NO_FEASIBLE_FLOW);
    LEDA_EXCEPTION(0, "No feasible flow2");
    return -1;
   }


  forall_edges(e, P) {
    switch (P.get_dir(e)) {
      case north:
        P.set_length(e, flow_h[EtoA[e]]);
        P.set_length(REV(e), flow_h[EtoA[e]]);
        break;
      case west:
        P.set_length(e, flow_v[EtoA[e]]);
        P.set_length(REV(e), flow_v[EtoA[e]]);
        break;
      default:
        break;
    }
  }

  forall_nodes(v, P)
    if (P.get_type(v) == v_real) break;

  x_pos.init(G);
  y_pos.init(G);
  node_array<bool> seen(P, false);
  P.determine_position(v, 0, 0, seen);
  P.norm_positions();

  int max_x = 0; 
  int max_y = 0;

  forall_nodes(v, P)
  { 
    if (P.get_type(v) != v_real) continue;

    int x = P.get_x(v);
    int y = P.get_y(v);
    x_pos[P.get_orig(v)] = x;
    y_pos[P.get_orig(v)] = y;
    max_x = leda_max(max_x, x);
    max_y = leda_max(max_y, y);
  }

  forall_nodes(v, P)
  { 
    if (P.get_type(v) != v_big) continue;

    list<int>lx;
    list<int>ly;

    face f_cage = P.face_of(corr_cage_edge[v]);

    forall_face_edges(e, f_cage) 
    { if (P.get_type(source(e)) == v_dissection) continue;
      int x = P.get_x(source(e));
      int y = P.get_y(source(e));
      lx.append(x);
      ly.append(y);
     }

    lx.sort();
    ly.sort();

    int x_big = (lx.head() + lx.tail()) / 2;
    int y_big = (ly.head() + ly.tail()) / 2;

    list_item it;
    forall_items(it, lx)
      if (lx[it] != lx.head() && lx[it] != lx.tail()
      && lx[it] == lx[lx.cyclic_succ(it)])
      x_big = lx[it];

    forall_items(it, ly)
      if (ly[it] != ly.head() && ly[it] != ly.tail()
      && ly[it] == ly[ly.cyclic_succ(it)])
      y_big = ly[it];

    x_pos[P.get_orig(v)] = x_big;
    y_pos[P.get_orig(v)] = y_big;
  }

  x_bends.init(G);
  y_bends.init(G);

  forall_edges(e, G) {
    forall(v, b_nodes[e]) {
    x_bends[e].append(P.get_x(v));
    y_bends[e].append(P.get_y(v));
   }
  }

  return leda_max(max_x, max_y);
}

int ORTHO_EMBEDDING(const graph& G, const node_array<bool>& crossing,
                                    const edge_array<int>& bound,
                                    node_array<double>& x_pos,
                                    node_array<double>& y_pos,
                                    edge_array<list<double> >& x_bends,
                                    edge_array<list<double> >& y_bends)
{
  node_array<int> XPOS(G);
  node_array<int> YPOS(G);
  edge_array<list<int> > XBENDS(G);
  edge_array<list<int> > YBENDS(G);

  int max_c = ORTHO_EMBEDDING(G, crossing, bound, XPOS, YPOS, XBENDS, YBENDS);

  if (max_c < 0) return max_c;

  x_pos.init(G);
  y_pos.init(G);
  x_bends.init(G);
  y_bends.init(G);

  node v;
  forall_nodes(v, G) {
    x_pos[v] = XPOS[v];
    y_pos[v] = YPOS[v];
  }

  edge e;
  forall_edges(e, G) {
    list<int>& XL = XBENDS[e];
    list<int >& YL = YBENDS[e];
    x_bends[e].clear();
    y_bends[e].clear();
    int x;
    forall(x, XL) x_bends[e].append(x);
    int y;
    forall(y, YL) y_bends[e].append(y);
  }

  return max_c;
}

LEDA_END_NAMESPACE


