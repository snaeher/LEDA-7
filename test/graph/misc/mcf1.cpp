/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mcf1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/graph/static_graph.h>

//#define MCF_EVENTS

/*
#include <LEDA/templates/mcf_new.t>
*/
#include <LEDA/templates/mcf.h>

using namespace leda;

using std::cout;
using std::cin;
using std::flush;
using std::endl;



template <class T, class EL>
void copy_graph(dimacs_graph<T,EL>& D, graph& G,
          edge_array<T>& lcap, edge_array<T>& ucap,
          edge_array<T>& cost, node_array<T>& supl)
{
  int n = D.number_of_nodes();
  int m = D.number_of_edges();

  G.init(n,m);

  supl.init(G,n,T());

  lcap.init(G,m,T());
  ucap.init(G,m,T());
  cost.init(G,m,T());

  node* V = new node[n];

  int i;
  for(i=0; i<n; i++) V[i] = G.new_node();

  for(i=0; i<n; i++)
  {
    int j = D.first_adj_edge(i);
    while (j != -1)
    {
      node v = V[i];
      node w = V[D.target(j)];

      //edge e = G.new_edge(v,w);

      edge e;
      if (G.indeg(w) == 0)
        e = G.new_edge(v,w);
      else
        e = G.new_edge(v,G.first_in_edge(w),leda::before);

      const EL& inf = D.edge_inf(j);

      lcap[e] = inf.first();
      ucap[e] = inf.second();
      cost[e] = inf.third();

      j = D.next_adj_edge(j);
    }
  }

  forall(i, D.special_nodes())
  {
    node v = V[i];
    supl[v] = D.node_inf(i);
  }

  G.finish_construction();

/*
  G.print();
  node v;
  forall_nodes(v,G)
  { cout << G.index(v) << ": ";
    edge e;
    forall_in_edges(e,v) cout << G.index(G.source(e)) << " ";
    cout << endl;
   }
  cout << endl;
*/


  delete [] V;
}



template <class T, class EL, class GT>
void copy(dimacs_graph<T,EL>& D, GT& G,
          edge_array<T,GT>& lcap, edge_array<T,GT>& ucap,
          edge_array<T,GT>& cost, node_array<T,GT>& supl)
{
  typedef typename GT::node node;
  typedef typename GT::edge edge;

  int n = D.number_of_nodes();
  int m = D.number_of_edges();

  G.init(n,m);

  supl.init(G,n,T());

  lcap.init(G,m,T());
  ucap.init(G,m,T());
  cost.init(G,m,T());

  node* V = new node[n];

  int i;
  for(i=0; i<n; i++) V[i] = G.new_node();

  for(i=0; i<n; i++)
  {
    int j = D.first_adj_edge(i);
    while (j != -1)
    {
      node v = V[i];
      node w = V[D.target(j)];
      edge e = G.new_edge(v,w);

      const EL& inf = D.edge_inf(j);

      lcap[e] = inf.first();
      ucap[e] = inf.second();
      cost[e] = inf.third();

      j = D.next_adj_edge(j);
    }
  }

  forall(i, D.special_nodes())
  {
    node v = V[i];
    supl[v] = D.node_inf(i);
  }

  G.finish_construction();

/*
  G.print();
  node v;
  forall_nodes(v,G)
  { cout << G.index(v) << ": ";
    edge e;
    forall_in_edges(e,v) cout << G.index(G.source(e)) << " ";
    cout << endl;
   }
  cout << endl;
*/

  delete [] V;
}


int main(int argv,char** argc)
{
  double sc = 12;
  if (argv > 1) sc = double(atoi(argc[1]));

  dimacs_graph<int, three_tuple<int,int,int> > D;
  D.read(cin);

  try
  {
    cout << endl << "Standard Graph" << endl;

    graph G(6,6);
    node_array<int> supply;
    edge_array<int> lcap;
    edge_array<int> ucap;
    edge_array<int> cost;
    edge_array<int> flow(G,0);

    //copy(D,G,lcap,ucap,cost,supply);

    copy_graph(D,G,lcap,ucap,cost,supply);

    float t = used_time();

    mincostflow_new::mcf<int> S (G,supply,cost,lcap,ucap);
    S.cost_scaling(flow, sc);

    float t1 = used_time(t);

   double C = 0;
    edge e;
   forall_edges(e,G)
      C += flow[e] * cost[e];

    cout << "costs: " << string("%10.0f",C) << endl;
   cout << "time:  " << string("%10.2f",t1) << endl << endl;

    S.cost_scaling_infos();

 }
  catch (mincostflow_new::mcf<int>::error& e)
  {
    cout << e.message() << endl;
  }

  try
  {
    cout << endl << "LEDA CAPACITY_SCALING" << endl;

    graph G;
    node_array<int> supply;
    edge_array<int> lcap;
    edge_array<int> ucap;
    edge_array<int> cost;

    //copy(D,G,lcap,ucap,cost,supply);
    copy_graph(D,G,lcap,ucap,cost,supply);

    edge_array<int> flow(G,0);

    float t = used_time();

    MCF_CAPACITY_SCALING(G,lcap,ucap,cost,supply,flow);

    float t1 = used_time(t);

    double C = 0;
    edge e;
   forall_edges(e,G)
      C += flow[e] * cost[e];

    cout << "costs: " << string("%10.0f",C) << endl;
   cout << "time:  " << string("%10.2f",t1) << endl << endl;
  }
  catch (int i)
  {

  }

  try
  {
    cout << endl << "Static Graph" << endl;

    typedef static_graph<in_edges_graph,4,4> sgraph;
    typedef static_graph<in_edges_graph,4,4>::edge edge;
    typedef static_graph<in_edges_graph,4,4>::node node;

    sgraph G;
    node_array<int,sgraph> supply;
    edge_array<int,sgraph> lcap;
    edge_array<int,sgraph> ucap;
    edge_array<int,sgraph> cost;
    edge_array<int,sgraph> flow(G,0);

    copy(D,G,lcap,ucap,cost,supply);

    float t = used_time();

    mincostflow_new::mcf<int,sgraph> S(G,supply,cost,lcap,ucap);
    S.cost_scaling(flow, sc);

    float t1 = used_time(t);

   double C = 0;
    edge e;
   forall_edges(e,G)
      C += flow[e] * cost[e];

    cout << "costs: " << string("%10.0f",C) << endl;
   cout << "time:  " << string("%10.2f",t1) << endl << endl;

    S.cost_scaling_infos();

 }
  catch (mincostflow_new::mcf<int,static_graph<in_edges_graph,4,4> >::error&
e)
  {
    cout << e.message() << endl;
  }

 return 0;
}


