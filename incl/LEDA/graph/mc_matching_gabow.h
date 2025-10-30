/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mc_matching_gabow.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#ifndef LEDA_MC_MATCHING_GABOW
#define LEDA_MC_MATCHING_GABOW

#include <LEDA/graph/mc_matching.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/array.h>

LEDA_BEGIN_NAMESPACE

class __exportC G_card_matching 
{
  class __exportC simple_queue // local class
  {
    int D;     // Q[0] to Q[D-1] are empty
    int max_d; // maximum value + 1
    array<list<edge> > Q;

    public: 

    simple_queue(const graph& G): D(0), 
                                  max_d(G.number_of_nodes()/2 + 1),
                                  Q(max_d) {}
  
    void init() {
      for (int i=0; i<max_d; i++) Q[i].clear();
      D = 0;
    }
  
    void insert(edge e, int d) { 
      // insert e into queue Q[d]
      if (d >=  max_d) return;
      Q[d].append(e);
    }
  
    edge delete_at_Delta(const int& Delta) {
      if (Delta > D) D = Delta;
      if (Delta >= max_d || Q[Delta].empty()) return nil;
      return Q[Delta].pop();
    }
 };


  const graph& G;

  list<edge> M; // matching in G
  node_array<node> mate; // mates in M
  node_partition base; // dbase and base are almost the same
  node_partition dbase; // the maximal positive blossoms in G
  node_array<node> rep; // rep[v] is the base of the maximal positive blossom containing v
  node_array<node> parent; // parent[v] is the parent of v in the alternating tree
  node_array<node> source_bridge; // bridges close blossoms; a blossom consists of two paths
  node_array<node> target_bridge; // x--z and y--z plus the edge xy; z is the base of the blossom.
  // the nodes on the path from x to z store x as source_bridge and y as target_bridge.
  node_array<double> path1;
  node_array<double> path2;
  edge_array<int> w; // w[e] = 2 for e in M and w[e] = 0 otherwise

//node_array<LABEL> label;
  node_array<int> label; // sn 01/2025

  simple_queue PQ;
  list<node> P;
  list<node> T;
  node_array<int> bd; // initialized to 1
  node_array<int> bDelta; // correct value for free nodes
  graph H; // H is obtained from G by contracting the maximal positive blossoms
  node_array<node> mateHG;
  edge_array<bool> is_edge_of_H;
  node_array<list<node> > contracted_into;

// node_partition baseHG;

//node_array<LABEL> labelHG;
  node_array<int> labelHG; // sn 01/2025

  node_array<edge> parentHG; // only odd nodes use it. 
  node_array<int> even_timeHG; // timestep at which a node of H became even;
  node_array<edge> bridgeHG;
  node_array<int> dirHG;
  int size_of_M;
  int tG;
  int Delta;
  double strue;
  node_array<int> num;
  node_array<int> even_time;
  int even_count;

  bool phase_1();
  void phase_2();

  int d(const node& v);
  void scan_edge(const edge& e, const node& z);
  void shrink_path(node b, node x, node y, list<node>& dunions);
  node find_aug_path(node v, node v0);
  void find_path(node x, node y);
  node find_apHG(node vh);
  void find_path_in_HG(list<edge>& p, node vh, node uh);
  void augmentG(const list<edge>& aph);
  void find_path_in_G(list<node>& p, node v, node u);

/*
  void scan(node v, node v0, list<node>& SN, list<edge>& SE);
  void shrink_path(node b, node v, node w, 
            node_partition& base, node_array<node>& mate, 
            node_array<node>& pred, node_array<node>& source_bridge, 
            node_array<node>& target_bridge, node_slist& Q);
*/

  int init();
  int init(const list<edge>& M);

public: 

  G_card_matching(const graph& G);

  list<edge> solve(node_array<int>& OSC, int& number_of_iterations,
                                          double heur_factor = 1, 
                                          bool heur = true);
};


LEDA_END_NAMESPACE
#endif
