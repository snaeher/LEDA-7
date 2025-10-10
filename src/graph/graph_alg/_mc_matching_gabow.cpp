/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _mc_matching_gabow.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/graph/mc_matching.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/array.h>

LEDA_BEGIN_NAMESPACE

// sn 01/2025
typedef int LABEL;
enum { EVEN, ODD, UNLABELED };

bool G_card_matching::phase_1() {
    node v;
    edge e; // the generic vertex and edge
    Delta = 0;
    int n = G.number_of_nodes();
    bool found_sap = false;

    PQ.init();

    // reinitialize the part of the node-partitions used in the previous iteration. 
    base.split(T);
    dbase.split(T);

    forall(v, T) label[v] = ((mate[v] == nil) ? EVEN : UNLABELED);

    list_item it;
    forall_items(it, T)
    if (mate[T[it]] == nil)
      forall_inout_edges(e, T[it]) scan_edge(e, T[it]);
    else
      T.del_item(it);
    list < node > dunions;
    while (2 * Delta <= n) {
      while ((e = PQ.delete_at_Delta(Delta)) != nil) {
        node x = G.source(e), y = G.target(e); // one of the endpoints must be even, none odd
        if (label[base(x)] != EVEN) swap(x, y);
        if (y == mate[x] || base(x) == base(y) || label[base(y)] == ODD) continue;
        // only non-matching and non-self-loops and y not odd
        if (label[base(y)] == UNLABELED) {
          node z = mate[y];
          bd[y] = bd[z] = 1;
          bDelta[y] = bDelta[z] = Delta;
          parent[z] = y;
          parent[y] = x;
          label[y] = ODD;
          label[z] = EVEN;
          T.push(y);
          T.push(z);
          forall_inout_edges(e, z)
          scan_edge(e, z);

        } else
        if (label[base(y)] == EVEN) { // chunk sets found_sap if an augmenting path is found
          strue++;
          node hx = base(x), hy = base(y);
          path1[hx] = path2[hy] = strue;

          while ((path1[hy] != strue && path2[hx] != strue) && (mate[hx] != nil || mate[hy] != nil)) {
            // hy does not lie on the first path, hx does not lie on the second, and one is not free
            if (mate[hx] != nil) {
              hx = base(parent[mate[hx]]);
              path1[hx] = strue;
            }
            if (mate[hy] != nil) {
              hy = base(parent[mate[hy]]);
              path2[hy] = strue;
            }
          }

          if (path1[hy] == strue || path2[hx] == strue) {

            node b = (path1[hy] == strue) ? hy : hx; // base

            shrink_path(b, x, y, dunions);
            shrink_path(b, y, x, dunions);

          } else {
            found_sap = true;
          }

        }
      } // there is no further tight edge and no augmenting path at level Delta
      if (found_sap) {

/*
        node vh;
*/
        node v;
        node u;
        edge e;

        int number_of_nodes_of_H = 0;

        forall(v, T) {
          contracted_into[dbase(v)].append(v);
          mateHG[v] = nil;
        }

        forall(u, T)
        forall_inout_edges(e, u) is_edge_of_H[e] = false;

        forall(u, T) {
          node uh = dbase(u);
          forall_out_edges(e, u) {
            is_edge_of_H[e] = false;
            node v = G.target(e);
            node vh = dbase(v);
            if (uh != vh && d(u) + d(v) == w[e]) {
              is_edge_of_H[e] = true; //assert( in_T[u] == in_T[v] && ((in_T[u] && in_T[v]) || mate[u] == v));
              if (w[e] == 2) { // e is a matching edge
                mateHG[uh] = vh;
                mateHG[vh] = uh;
              }
            }
          }
        }
        return true;
      }
      while (!dunions.empty()) {
        node u = dunions.pop();
        node v = dunions.pop();
        if (u == v) dbase.make_rep(u);
        else dbase.union_blocks(u, v);
      } // see section 3.4 Construction of H
      Delta++;
    } // end Delta-Loop
    return false; // no augmenting path found
  }

  int G_card_matching::d(const node & v) {
    if (label[base(v)] == UNLABELED) return 1;
    if (label[base(v)] == EVEN) return bd[v] - (Delta - bDelta[v]);
    return bd[v] + (Delta - bDelta[v]);
  }

  void G_card_matching::scan_edge(const edge & e,
    const node & z) {
    node u = G.opposite(e, z);
    if (mate[u] == z || label[base(u)] == ODD) return;
    int p = d(z) + d(u);
    if (label[u] == UNLABELED)
      PQ.insert(e, Delta + p);
    else
      PQ.insert(e, Delta + p / 2);
    return;
  }

  void G_card_matching::shrink_path(node b, node x, node y, list < node > & dunions) {
    node v = base(x);
    while (v != b) {
      base.union_blocks(v, b);
      dunions.append(v);
      dunions.append(b);
      v = mate[v];
      base.union_blocks(v, b);
      dunions.append(v);
      dunions.append(b);
      base.make_rep(b);
      source_bridge[v] = x;
      target_bridge[v] = y;
      // label[v] = EVEN; we don't have to do this because we always ask for the label of the base of v.
      bd[v] = bd[v] + (Delta - bDelta[v]);
      bDelta[v] = Delta;
      edge e;
      forall_inout_edges(e, v)
      scan_edge(e, v);
      v = base(parent[v]);
    }
    dunions.append(b);
    dunions.append(b); // signal to dbase that this round of unions is over. 
  }

  node G_card_matching::find_aug_path(node v, node v0) { // we are growing a tree with root v0
    edge e;
    node w;
    forall_inout_edges(e, v) {
      w = G.opposite(e, v);
      if (v == w) continue;
      if (mate[w] == nil && label[w] == UNLABELED) {
        parent[w] = v;
        T.append(w);
        label[w] = ODD;
        return w;
      }
    }
    // no immediate break-through
    forall_inout_edges(e, v) {
      w = G.opposite(e, v);
      node bw = dbase(w);
      if (label[bw] == ODD || w == v) continue;
      if (label[bw] == UNLABELED) {
        label[w] = ODD;
        parent[w] = v;
        T.append(w);
        node mw = mate[w];
        label[mw] = EVEN;
        T.append(mw);
        even_time[mw] = even_count++;
        node s = find_aug_path(mw, v0);
        if (s != nil)
          return s;
      } else {
        node bv = dbase(v);
        node bw = dbase(w);
        list < node > tmp;
        if (even_time[bv] < even_time[bw]) { //blossom_step along forward edge
          // walk down from bw to bv and perform unions
          while (bw != bv) { // doing the unions carefully, so that only one make_rep is needed
            node mate_bw = mate[bw];
            dbase.union_blocks(bw, mate_bw);
            bw = dbase(parent[mate_bw]);
            dbase.union_blocks(mate_bw, bw);
            tmp.push_front(mate_bw);
            source_bridge[mate_bw] = w;
            target_bridge[mate_bw] = v;
          }
          dbase.make_rep(bv);
          forall(w, tmp) {
            node s = find_aug_path(w, v0);
            if (s != nil)
              return s;
          }
        }
      }
    }
    return nil;
  }

  void G_card_matching::find_path(node x, node y) {
    /* traces the even length alternating path from x to y; if non-trivial it starts with
    the matching edge incident to x; collects the non-matching edges on this
    path as pairs of nodes */
    if (x == y) return;

    if (label[x] == EVEN) {
      node mate_x = mate[x], par_mate_x = parent[mate_x];
      P.append(mate_x);
      P.append(par_mate_x);
      find_path(par_mate_x, y);
      return;
    } else { // x is ODD
      find_path(source_bridge[x], mate[x]);
      P.append(source_bridge[x]);
      P.append(target_bridge[x]);
      find_path(target_bridge[x], y);
      return;
    }
  }

  void G_card_matching::phase_2() {
    node v;
    labelHG.init(G, UNLABELED);
    forall(v, T) rep[v] = dbase(v);
    list < list < edge > > PG;

    node vh;
    forall(vh, T) {
      if (vh != rep[vh]) continue; // vh does not represent a node of H
      if (labelHG[vh] == UNLABELED && mateHG[vh] == nil) {
        labelHG[vh] = EVEN;
        even_timeHG[vh] = tG++;
        node free = find_apHG(vh);
        if (free != nil) {
          list < edge > apG;
          edge e = parentHG[free];
          apG.append(e);
          find_path_in_HG(apG, rep[rep[G.source(e)] == free ? G.target(e) : G.source(e)], vh);
          PG.append(apG);
        }
      }
    }

    list < edge > aphG;
    forall(aphG, PG) augmentG(aphG); // augment all paths found

    forall(v, T) contracted_into[v].clear(); // clear H
  }

  node G_card_matching::find_apHG(node vh) {
    edge eh;
    node v = 0;
    forall(v, contracted_into[vh]) { // v is a node of G, vh = rep[vh] is a node of G representing a node of H
      forall_inout_edges(eh, v) {
        if (!is_edge_of_H[eh]) continue;
        node uh = rep[G.opposite(v, eh)];
        if (mateHG[vh] == uh) continue;
        if (labelHG[uh] == UNLABELED) { // grow step
          node mateHG_uh = mateHG[uh];
          if (mateHG_uh == nil) { // uh is free and we found an augmenting path
            labelHG[uh] = ODD;
            parentHG[uh] = eh;
            return uh;
          } else { // extend the path by two edges
            labelHG[uh] = ODD;
            labelHG[mateHG_uh] = EVEN;
            parentHG[uh] = eh;
            even_timeHG[mateHG_uh] = tG++;
            node s = find_apHG(mateHG_uh);
            if (s != nil)
              return s;
          }
        } else {
          node bh = dbase(vh);
          node zh = dbase(uh);
          if (even_timeHG[bh] < even_timeHG[zh]) { // blossom step along forward edge
            list < node > tmp;
            list < node > endpoints_of_M;
            while (zh != bh) {
              endpoints_of_M.append(zh);
              zh = mateHG[zh];
              endpoints_of_M.append(zh);
              tmp.push_front(zh); // zh is odd and we add it to the front of tmp
              zh = dbase(rep[rep[G.source(parentHG[zh])] == zh ?
                G.target(parentHG[zh]) : G.source(parentHG[zh])]);
            }

            forall(zh, endpoints_of_M) dbase.union_blocks(zh, bh);
            dbase.make_rep(bh);

            forall(zh, tmp) {
              //even_timeHG[zh] = tG++; // not needed
              bridgeHG[zh] = eh; // assert(eh != nil); 
              dirHG[zh] = (G.target(eh) == v ? 1 : -1); // dirHG = 1 iff eh = (_,v)
            }

            forall(zh, tmp) { //the new even node closest to bh comes first
              node s = find_apHG(zh);
              if (s != nil)
                return s;
            }
          }
        }
      }
    }
    return nil;
  }

  void G_card_matching::find_path_in_HG(list < edge > & p, node vh, node uh) {
    if (vh == uh) return;
    if (labelHG[vh] == EVEN) {
      node mvh = mateHG[vh];
      edge e = parentHG[mvh];
      p.append(e);
      find_path_in_HG(p, rep[rep[G.source(e)] == mvh ? G.target(e) : G.source(e)], uh);
      return;
    } else {
      edge bridgeHG_vh = bridgeHG[vh];
      find_path_in_HG(p, rep[dirHG[vh] == 1 ?
        G.source(bridgeHG_vh) : G.target(bridgeHG_vh)], rep[mateHG[vh]]);
      p.append(bridgeHG[vh]);
      find_path_in_HG(p, rep[dirHG[vh] == 1 ?
        G.target(bridgeHG_vh) : G.source(bridgeHG_vh)], uh);
      return;
    }
  }

  void G_card_matching::augmentG(const list < edge > & aph) {
    list < node > ap; // augmenting path in G as a list of nodes, two nodes for each non-matching edge
    edge e;
    forall(e, aph) {
      node u = G.source(e);
      node v = G.target(e);
      ap.append(u);
      ap.append(v);
      find_path_in_G(ap, u, rep[u]);
      find_path_in_G(ap, v, rep[v]);
    }
    while (!ap.empty()) {
      node u = ap.pop();
      node v = ap.pop();
      mate[u] = v;
      mate[v] = u;
    }
    size_of_M++;
  }

  void G_card_matching::find_path_in_G(list < node > & p, node v, node u) {
    /* finding the even path from v to u in the original graph using blossom bridges;
    only returns the non-matching edges, for each edge the two endpoints */
    if (v == u) return;
    if (label[v] == EVEN) {
      p.append(mate[v]);
      p.append(parent[mate[v]]);
      find_path_in_G(p, parent[mate[v]], u);
      return;
    } else {
      find_path_in_G(p, source_bridge[v], mate[v]);
      p.append(source_bridge[v]);
      p.append(target_bridge[v]);
      find_path_in_G(p, target_bridge[v], u);
      return;
    }
  }

  /*void scan(node v, node v0, list<node>& SN, list<edge>& SE){
    edge e; //cerr << "\nscanning node " << num[v];
    SN.push(v);
    SE.push(nil);
    forall_inout_edges(e,v){
      node w = G.opposite(e,v);
      if( dbase(v) == dbase(w) ) continue;
      SE.push(e); //cerr << "\npushed edge to " << num[w];
      if( mate[w] == nil && w != v0 ) return;
    }
    }
  */

  /*
  void G_card_matching::shrink_path(node b, node v, node w, 
            node_partition& base, node_array<node>& mate, 
            node_array<node>& pred, node_array<node>& source_bridge, 
            node_array<node>& target_bridge, node_slist& Q)
  { node x = base(v);
    while (x != b)
    { base.union_blocks(x,b);
      x = mate[x];
      base.union_blocks(x,b);
      base.make_rep(b);
      Q.append(x);
      source_bridge[x] = v;  
      target_bridge[x] = w;
      x = base(pred[x]);
    }
    }
  */


  G_card_matching::G_card_matching(const graph & g) : 
              G(g), 
              mate(node_array < node > (G, nil)),
              base(node_partition(G)),
              dbase(node_partition(G)),
              rep(node_array < node > (G)),
              parent(node_array < node > (G)),
              source_bridge(node_array < node > (G)),
              target_bridge(node_array < node > (G)),
              path1(node_array < double > (G)),
              path2(node_array < double > (G)),
              w(edge_array < int > (G, 0)),
              label(node_array < LABEL > (G, UNLABELED)),
              PQ(simple_queue(G)),
              P(list < node > ()),
              T(list < node > ()),
              bd(node_array < int > (G)),
              bDelta(node_array < int > (G)),
              mateHG(node_array < node > (G)),
              is_edge_of_H(edge_array < bool > (G, false)),
              contracted_into(node_array < list < node > > (G)),
              labelHG(node_array < LABEL > (G, UNLABELED)),
              parentHG(node_array < edge > (G)),
              even_timeHG(node_array < int > (G, 0)),
              bridgeHG(node_array < edge > (G)),
              dirHG(node_array < int > (G)),
              size_of_M(0),
              tG(0),
              strue(0),
              num(node_array < int > (G)),
              even_time(node_array < int > (G)),
              even_count(0) {}


  int G_card_matching::init() {
    edge e;
    forall_edges(e, G) {
      node u = G.source(e);
      node v = G.target(e);
      if (u != v && mate[u] == nil && mate[v] == nil) {
        mate[u] = v;
        mate[v] = u;
        size_of_M++;
      }
    }
    return size_of_M;
  }


  int G_card_matching::init(const list < edge > & M0) {
    edge e;
    forall(e, M0) {
      node u = G.source(e);
      node v = G.target(e);
      if (u != v && mate[u] == nil && mate[v] == nil) {
        mate[u] = v;
        mate[v] = u;
        size_of_M++;
      }
    }
    return G.number_of_nodes(); // there is no guarantee on the size of |M0|
  }

  list < edge > G_card_matching::solve(node_array < int > & OSC, 
                      int & number_of_iterations,
                      double heur_factor, 
                      bool heur) 
  {
    int max_size_of_M = min(G.number_of_nodes() / 2, 2 * init());
    // the size of a greedy matching is at least half of the maximum matching.
    number_of_iterations = 0;
    node v;
    edge e;
    int count = 1;
    T = G.all_nodes();
    forall_nodes(v, G) num[v] = count++;
    while (true) {

      node z;
      forall(z, T)
      forall_out_edges(e, z) {
        node u = G.source(e);
        node v = G.target(e);
        w[e] = 0;
        if (v == mate[u]) {
          w[e] = 2;
          mate[u] = mate[v] = nil; // parallel edges; only one of them is in the matching.
        }
      }

      forall(z, T)
      forall_out_edges(e, z) {
        node u = G.source(e);
        node v = G.target(e);
        if (w[e] == 2) {
          mate[u] = v;
          mate[v] = u;
        }
      }

      number_of_iterations++;
      if (heur && (number_of_iterations > 0.5 * heur_factor * (max_size_of_M - size_of_M))) {

        dbase.split(G.all_nodes());

        node v0, v, w;

        forall_nodes(v0, G) label[v0] = UNLABELED;

        forall_nodes(v0, G) {
          if (mate[v0] != nil) continue;
          label[v0] = EVEN;
          T.clear();
          T.append(v0);
          even_time[v0] = even_count++;

          if ((w = find_aug_path(v0, v0)) != nil) {
            node pw = parent[w];
            P.push(w);
            P.push(pw);
            find_path(pw, v0);

            while (!P.empty()) {
              node a = P.pop();
              node b = P.pop();
              mate[a] = b;
              mate[b] = a;
            }

            forall(v, T) label[v] = UNLABELED;
            dbase.split(T);
            size_of_M++;
          }
        }

        break;
      } else {
        if (phase_1()) // returns true if there is an augmenting path
          phase_2();
        else break;
      }
    }

    M.clear(); // make empty list
    forall_edges(e, G) {
      node u = G.source(e);
      node v = G.target(e);
      if (mate[u] == v) {
        M.append(e);
        mate[u] = mate[v] = nil; // only one edge from a bundle of parallel edges
      }
    }

    OSC.init(G, -1);

    int number_of_unlabeled = 0;
    node arb_u_node = nil;

    forall_nodes(v, G)
    if (label[v] == UNLABELED) {
      number_of_unlabeled++;
      arb_u_node = v;
    }

    int L = 0;
    if (number_of_unlabeled > 0) {
      OSC[arb_u_node] = 1;
      if (number_of_unlabeled > 2) L = 2;
      forall_nodes(v, G)
      if (label[v] == UNLABELED && v != arb_u_node) OSC[v] = L;
    }

    int K = (L == 0 ? 2 : 3);

    forall_nodes(v, G)
    if (dbase(v) != v && OSC[dbase(v)] == -1) OSC[dbase(v)] = K++;

    forall_nodes(v, G) {
      if (dbase(v) == v && OSC[v] == -1) {
        if (label[v] == EVEN) OSC[v] = 0;
        if (label[v] == ODD) OSC[v] = 1;
      }
      if (dbase(v) != v) OSC[v] = OSC[dbase(v)];
    }

    return M;
  }

LEDA_END_NAMESPACE
