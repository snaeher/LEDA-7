/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_git_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


/********************************************************************
*                                                                   *
*        Graphiterator Example Program  git_demo.c                  *
*                                                                   *
*        (C) 1998                                                   *
*              Marco Nissen (marco@mpi-sb.mpg.de)                   *
*                                                                   *
*        Last update 13.8.1998 (Marco Nissen)                       *
*                                                                   *
********************************************************************/
 

#include <LEDA/graph/graph_iterator.h>
#include <LEDA/system/stream.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/panel.h>
#include <LEDA/core/h_array.h>

#include <math.h>
#include <time.h>

using namespace leda;

#if defined(LEDA_STD_HEADERS)
using std::cout;
using std::endl;
#endif


template < class T >
  class Find_It {
    public:
      const int START_SEARCH, END_SEARCH, PREPARE, START, END, WALKLEFT, WALKRIGHT,
      ABNORMAL_END;
  private:
    typedef DIJKSTRA_It < T > LEFTD;
    typedef Rev_DIJKSTRA_It < T > RIGHTD;
    LEFTD leftd;
    RIGHTD *rightd;
    int trav;      // indicates state
    /* 0: beginning
    1: both traversals ended
    2: left path updates right path
    3: begin of path traversal
    4: successful end
    5: walk left
    6: walk right
    7: abnormal end */
    node k, j;
    edge e;
    node_array < T >* distr;
    node_array < edge > predl, predr;
    node_array_da < edge > dpredl, dpredr;
    node_array < bool > markl, markr;
    node_array_da < bool > dmarkl, dmarkr;
  
    node_array_da<T> ddist;
    edge_array_da<T> dlen;
    NodeIt it;
    OutAdjIt ai;
  public:
    Find_It (graph & G, node s, node t, node_array < T > &dist, 
                                        edge_array < T > &len) :

      START_SEARCH (0), END_SEARCH (1), PREPARE (2), START (3),
      END (4), WALKLEFT (5), WALKRIGHT (6), ABNORMAL_END (7), 
      leftd (G, s, dist, len), 
      trav (START_SEARCH), 
      predl (G, nil), predr (G, nil), dpredl (predl), dpredr (predr),
      markl (G, false), markr (G, false), dmarkl (markl), dmarkr (markr),
      ddist(dist), dlen(len), it (G), ai (G) {

      distr = new node_array<T>(G,0);
      rightd = new RIGHTD (G, t, *distr, len);
      j = nil;
      e = nil;
      j = leftd.get_node ();
      set (dmarkl, leftd, (bool) true);
      set (dmarkr, *rightd, (bool) true);
  }
  ~Find_It () {
    delete rightd; delete distr;
  }
  void left () {
    j = leftd.get_node ();
    if (leftd.valid ())
      ++leftd;
    else {
      trav = ABNORMAL_END;
      return;
    } if (leftd.has_edge ())
    trav = WALKLEFT;
    j = leftd.get_node ();
    set (dmarkl, leftd, (bool) true);
    if (leftd.is_pred ()) {
      e = leftd.get_edge ();
      j = target (e);
      it.update (j);
      set (dpredl, it, e);
      set (dmarkl, it, (bool) true);
      if ((bool) get (dmarkr, it)) {
        trav = END_SEARCH;
        k = j;
      }
    }
    
  }
  void right () {
    j = rightd->get_node ();
    if (rightd->has_edge ())
      trav = WALKRIGHT;
    if (rightd->valid ())
      ++(*rightd);
    else {
      trav = ABNORMAL_END;
      return;
    } j = rightd->get_node ();
    set (dmarkr, (*rightd), (bool) true);
    if (rightd->is_pred ()) {
      e = rightd->get_edge ();
      j = source (e);
      it.update (j);
      set (dpredr, it, e);
      set (dmarkr, it, (bool) true);
      if (get (dmarkl, it)) {
        trav = END_SEARCH;
        k = j;
      }
    }
    
  }
  void reset () {
    leftd.reset ();
    predl.init (rightd->current_search ().current ().get_graph (), nil);
    predr.init (rightd->current_search ().current ().get_graph (), nil);
    markr.init (rightd->current_search ().current ().get_graph (), false);
    markl.init (rightd->current_search ().current ().get_graph (), false);
    rightd->reset ();
    trav = START_SEARCH;
    j = nil;
    e = nil;
    } void make_invalid () {
    reset ();
    } bool valid () {
    return (trav != END && trav != ABNORMAL_END);
  }
  bool eol () {
    return (trav == END || trav == ABNORMAL_END);
    } bool success () {
    return trav == END;
    } node get_node () const {
    return j;
  }
  edge get_edge () const {
    return e;
  }
  bool has_edge () const {
    return e != nil;
    } Find_It & operator++ () {
    if (trav == START_SEARCH || trav == WALKRIGHT)
      left ();
    else if (trav == START_SEARCH || trav == WALKLEFT)
      right ();
    if (trav == END_SEARCH) {
      j = k;
      trav = PREPARE;
    }
    if (trav == PREPARE) {
      it.update (j);
      e = get (dpredl, it);
      if (e == nil) {
        trav = START;
        e = get (dpredr, it);
      }
      else {
        j = source (e);
        it.update (j);
        set (dpredr, it, e);
      }
    }
    else if (trav == START) {
      it.update (j);
      e = get (dpredr, it);
      if (e == nil) {
        trav = END;
        j = nil;
      }
      else { ai.update(j,e);
        j = target (e); it.update(j);
        set(ddist,it,get(ddist,ai)+get(dlen,ai));  
       }
    }
    return *this;
  }
  int get_status () {
    return trav;
  }
};

#define GRAPHADD(x,y)                           \
if (!M.defined(index(x)*n*10+index(y))) \
{ G.new_edge(x,y);                      \
    mm++;                                 \
M[index(x)*n*10+index(y)]=true; }

void
random_acyclic_graph (graph & G, int n, int m)
{
  node V[10000];
  G.clear ();
  for (int i = 0; i < n; i++)
    V[i] = G.new_node ();
  h_array < int, bool > M;
  int  mm = 0;
  node n1 = nil, n2 = nil;
  int counter = 0;
  while (mm < m) {
    n1 = n2;
    counter = 0;
    while (n1 == n2 && counter < 10000) {
      int a = rand_int (0, n - 1);
      int b = rand_int (0, n - 1);
      n1 = n2;
      if (a < b) {
        n1 = V[a];
        n2 = V[b];
        counter++;
      }
    }
    if (counter < 10000)
      GRAPHADD (n1, n2);
  }
}


int
main ()
{
  GRAPH < int, double >G;
  edge e;
  node v;

  //panel P1(2*W.width()/3,-1);
  
  panel P1 ("Demonstration of Graph Iterators");
  P1.text_item ("This program demonstrates the use of the graph iterator LEP.");
  P1.text_item ("");
  P1.text_item ("You may now select one of the following demonstrations:");
  int st = P1.button ("topological iterator demo");
  int ss = P1.button ("strongly connected components iterator demo");
  int sd = P1.button ("path find demo");
  P1.button ("quit");
  int action;
  if ((action = P1.open ()) == st) {
    panel P2 ("Demonstration of Graph Iterators");
    P2.text_item ("The next window shows a graph editor window (GraphWin)");
    P2.text_item ("and you can enter sample graph.");
    P2.text_item ("For this demonstration you should enter an acyclic graph.");
    P2.text_item ("If you don't know what an acyclic graph is or if you don't");
    P2.text_item ("want to enter the graph by yourself, select 'generate'.");
    P2.text_item ("");
    P2.button ("continue");
    int sg = P2.button ("generate");
    int action2 = P2.open ();
    G.clear ();
    if (action2 == sg) {
      panel P2 ("Size of acyclic graph");
      P2.text_item ("Please enter the size of the generated acyclic graph.");
      int n = 10, m = 20;
      P2.int_item ("Number of nodes", n);
      P2.int_item ("Number of edges", m);
      P2.open ();
      random_acyclic_graph (G, n, m);
    }
  }
  else {
    panel P2 ("Demonstration of Graph Iterators");
    P2.text_item ("The next window shows a graph editor window (GraphWin)");
    P2.text_item ("and you can enter sample graph.");
    P2.text_item ("If you don't want to enter the graph by yourself, select 'generate'.");
    P2.text_item ("");
    P2.button ("continue");
    int sg = P2.button ("generate");
    int action2 = P2.open ();
    G.clear ();
    if (action2 == sg) {
      panel P2 ("Size of graph");
      P2.text_item ("Please enter the size of the generated graph.");
      int n = 10, m = 20;
      P2.int_item ("Number of nodes", n);
      P2.int_item ("Number of edges", m);
      P2.open ();
      random_graph (G, n, m);
    }
    
  }
  GraphWin GW (G);
#ifndef _MSC_VER
  GW.set_directed (true);
#endif
  GW.open ();
  if (action == st) {
    panel P3 ("Topological Sorting");
    P3.text_item ("Now we want to sort this graph topologically.");
    P3.text_item ("");
    P3.text_item ("Clearly, LEDA contains already algorithms for this.");
    P3.text_item ("Here, we consider something different - the ");
    P3.text_item ("algorithmic iterator TOPO_It.");
    P3.text_item ("");
    P3.text_item ("Declaration: TOPO_It it(G); ");
    P3.text_item ("");
    P3.text_item ("(where G is a LEDA-graph)");
    P3.text_item ("");
    P3.text_item ("");
    
    P3.text_item ("Now you can visit each node in the topological ordering.");
    P3.open ();
    
    TOPO_It it (G);
    while (it.valid ()) {
      GW.set_color (it.get_node (), green);
      GW.redraw ();
      GW.wait ("Next node");
      ++it;
    }
    
    panel P4 ("Finished Traversal");
    P4.text_item ("Now you saw a topological ordering by using ");
    P4.text_item ("the algorithmic iterator TOPO_It.");
    P4.text_item ("");
    P4.button ("end");
    P4.open ();
  }
  else if (action == ss) {
    panel P3 ("Strongly Connected Components");
    P3.text_item ("Now we want to compute the strongly connected components of the graph.");
    P3.text_item ("");
    P3.text_item ("Clearly, LEDA contains already algorithms for this.");
    P3.text_item ("Here, we consider something different - the ");
    P3.text_item ("algorithmic iterator SCC_It.");
    P3.text_item ("");
    P3.text_item ("Declaration: SCC_It it(G); ");
    P3.text_item ("");
    
    P3.text_item ("(where G is a LEDA-graph)");
    P3.text_item ("");
    P3.text_item ("");
    
    P3.text_item ("Now nodes in different strongly");
    P3.text_item ("connected components are colored in different colors.");
    P3.open ();
    
    SCC_It it (G);
    while (it.valid ()) {
      GW.set_color (it.get_node (), it.compnumb ());
      GW.redraw ();
      ++it;
    }
    GW.wait ("All components were computed.");
    
    panel P4 ("Finished Computation");
    P4.text_item ("Now you saw a strongly connected components computation by using ");
    P4.text_item ("the algorithmic iterator SCC_It.");
    P4.text_item ("");
    P4.button ("end");
    P4.open ();
    
  }
  else if (action == sd) {
    panel P3 ("Find Path Demo");
    P3.text_item ("Now we want to compute a (nearly shortest) path in the graph.");
    P3.text_item ("");
    P3.text_item ("Here, we consider something different - the ");
    P3.text_item ("algorithmic iterator Find_It, which is an application of the iterators DIJKSTRA_It and Rev_DIJKSTRA_It.");
    P3.text_item ("");
    P3.text_item ("(This class is not available in the normal LEDA include directory).");
    P3.text_item ("");
    P3.text_item ("Declaration: Find_It it(G,s,t,distance,cost); ");
    P3.text_item ("");
    
    P3.text_item ("(where G is a LEDA-graph, s source node, t target node, distance is a node array to store distances and cost is an edge array for the cost of edges)");
    P3.text_item ("");
    P3.open ();
    GW.acknowledge ("Please select a source node.");
    node s = GW.read_node ();
    GW.acknowledge ("Please select a  target node.");
    node t = GW.read_node ();
    
    node_array < double >dist (G, 0);
    edge_array < double >len (G);
    
    forall_nodes (v, G)
      GW.set_color (v, white);
    forall_edges (e, G) {
      point a = GW.get_position (source (e)), b = GW.get_position (target (e));
      len[e] = a.distance (b);
    }
    Find_It < double >bit (G, s, t, dist, len);
    while (bit.valid ()) {
      if (bit.get_node () != nil)
        if (bit.get_status () == bit.WALKLEFT)
        GW.set_color (bit.get_node (), red);
      if (bit.get_status () == bit.WALKRIGHT)
        GW.set_color (bit.get_node (), blue);
      if (bit.get_status () == bit.START)
        GW.set_color (bit.get_node (), green);
      if (bit.get_status () == bit.START)
        GW.set_color (bit.get_edge (), bit.get_status ());
      ++bit;
    }

    // cout << "Distance of target " << dist[t] << endl;

    GW.redraw ();
    if (bit.success ()) {
      panel P5 ("Successful Traversal");
      P5.text_item ("The algorithm started from the start and end node to find a path in the graph.");
      P5.text_item ("During the search procedure, all nodes visited from the first dijkstra iterator are coloured red. All nodes visited from the second dijkstra iterator, coming from the target node are coloured blue.");
      P5.text_item ("");
      P5.text_item ("Since a path was found during the computation, the predecessor indices computed by the first iterator are adapted to fit into the same direction as the predecessor indices from the second iterator. After that, the actual traversal begins: all green nodes are the nodes on the path, and the path is defined by the predecessor entries (which are hold internally in the dijkstra iterator).");
      P5.text_item ("");
      P5.text_item ("Now, everything which is green lies on the path.");
      P5.open ();
    }
    else {
      panel P5 ("No Path found");
      P5.text_item ("The algorithm started from the start and end node to find the path in the graph.");
      P5.text_item ("During the search procedure, all nodes visited from the first dijkstra iterator are coloured red. All nodes visited from the second dijkstra iterator, coming from the target node are coloured blue.");
      P5.text_item ("");
      P5.text_item ("Unfortunately, no path was found during the computation because there is no path at all.");
      P5.text_item ("Please try again this demonstration program on another example.");
      P5.text_item ("");
      P5.open ();
    }
  }
  return 0;
}
