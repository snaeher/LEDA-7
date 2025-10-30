/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  short_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/dimacs.h>
#include <LEDA/system/stream.h>
#include <LEDA/system/assert.h>

#include <LEDA/graph/node_pq.h>
#include <LEDA/graph/b_node_pq.h>
//#include <LEDA/graph/b_node_pq1.h>


using namespace leda;


using std::cout;
using std::cerr;
using std::flush;
using std::endl;
using std::ofstream;




int  dijkstra0(GRAPH<int,int>& g, node source, node target) 
{ 
  node_array<int>& dist = g.node_data();
  edge_array<int>& cost = g.edge_data();
  node_array<edge> pred;
  assert(pred.use_node_data(g,0));
  DIJKSTRA(g,source,cost,dist,pred);
  return dist[target];
}

int  dijkstra01(GRAPH<int,int>& g, node source, node target) 
{ node_array<int>& dist = g.node_data();
  edge_array<int>& cost = g.edge_data();
  node_array<edge> pred;
  assert(pred.use_node_data(g,0));
  DIJKSTRA(g,source,target,cost,dist,pred);
  return dist[target];
}


/*
int  dijkstra1(GRAPH<int,int>& g, node source, node target) 
{ 
  node_array<int>& dist = g.node_data();
  edge_array<int>& cost = g.edge_data();
  node_array<edge> pred;
  assert(pred.use_node_data(g,0));
  DIJKSTRA1(g,source,cost,dist,pred);
  return dist[target];
}

int  dijkstra11(GRAPH<int,int>& g, node source, node target) 
{ 
  node_array<int>& dist = g.node_data();
  edge_array<int>& cost = g.edge_data();
  node_array<edge> pred;
  assert(pred.use_node_data(g,0));
  DIJKSTRA1(g,source,target,cost,dist,pred);
  return dist[target];
}
*/
int dijkstra2(GRAPH<int,int>& g, node s, node t) 
{
  // use a bounded node priority queue (b_node_pq)

  node_array<int>& dist = g.node_data();
  edge_array<int>& cost = g.edge_data();

  b_node_pq<101> PQ(t);  

  node v;
  forall_nodes(v,g) dist[v] = MAXINT;

  g[s] = 0;
  PQ.insert(s,0);

  while ( (v = PQ.del_min()) != t )
  { int dv = dist[v];
    edge e;
    forall_adj_edges(e,v) 
    { node w = g.opposite(v,e);
      int d = dv + cost[e];
      int& dw = dist[w];
      if (d < dw)
      { if (dw != MAXINT) PQ.del(w,dw);
	dw = d;
        PQ.insert(w,d);
       }
     }
   }

  return dist[t];
}



/*
int dijkstra3(GRAPH<int,int>& g, node s, node t) 
{
  // use a bounded node priority queue (b_node_pq)

  node_array<int>& dist = g.node_data();
  edge_array<int>& cost = g.edge_data();

  node_array<node> succ;
  assert(succ.use_node_data(g));

  node_array<node> pred;
  assert(pred.use_node_data(g));

  //b_node_pq1<101,graph,node_array<node>,node_array<node> > PQ(succ,pred,t);  
  b_node_pq1<101,graph,node_array<node>,node_array<node> > PQ(succ,pred,t);  

  node v;
  forall_nodes(v,g) dist[v] = MAXINT;

  g[s] = 0;
  PQ.insert(s,0);

  while ( (v = PQ.del_min()) != t )
  { int dv = dist[v];
    edge e;
    forall_adj_edges(e,v) 
    { node w = g.opposite(v,e);
      int d = dv + cost[e];
      int& dw = dist[w];
      if (d < dw)
      { if (dw != MAXINT) PQ.del(w,dw);
	dw = d;
        PQ.insert(w,d);
       }
     }
   }

  return dist[t];
}

*/



int  moore0(GRAPH<int,int>& g, node s, node t)
{ node_array<int>& dist = g.node_data();
  edge_array<int>& cost = g.edge_data();
  MOORE(g,s,cost,dist,t);
  return dist[t];

}


int  moore1(GRAPH<int,int>& g, node s, node t) 
{
  node_array<int>& dist = g.node_data();
  edge_array<int>& cost = g.edge_data();

  //node v;
  //forall_nodes(v,g) dist[v] = MAXINT;

  dist.init(g,MAXINT);
  dist[s] = 0;

  node_list labeled;             // deque of candidate nodes
  labeled.append(s);

  while (! labeled.empty()) 
  { 
    node v = labeled.pop();
    int dv = dist[v];

    if (dv > dist[t]) continue;

    edge e;

    forall_adj_edges(e,v)
    { node w = g.opposite(v,e);
      int  d = dv + cost[e];
      if (d < dist[w]) 
      { if ( ! labeled(w) ) 
        { if (dist[w] == MAXINT)
	       labeled.append(w);
	    else
	       labeled.push(w);
	   }
	  dist[w] = d;
       }
     }

  }

  return dist[t];
}




int main (int argc, char** argv) 
{

  GRAPH<int,int> G(3,0);

  int sourcename;
  int targetname;
  int len;

  string filename = "grid100";

  if (argc > 1) filename = argv[1];

  // read names of source and target from file <filename>

  file_istream  infile (filename);

  if ( ! (infile >> sourcename >> targetname) )
  { cerr << "Cannot read file " << filename << endl;
    return 1;
   }

  cout << "Source node: " << sourcename << endl;
  cout << "Target node: " << targetname << endl;
  cout << endl;

  // read graph from file <filename>.graph

  float T = used_time();

  if (G.read(filename + ".graph") != 0)
  { cerr << "Cannot read graph from file " << filename << ".graph" << endl;
    return 1;
   }

  cout << string("Time for reading:  %5.2f",used_time(T)) << endl;
  cout << endl;

  // search for source and target nodes

  node source = nil;
  node target = nil;

  node v;
  forall_nodes(v,G) 
  { if (G[v] == sourcename) source = v;
    if (G[v] == targetname) target = v;
   }

  G.make_undirected();

  T = used_time();

  len = dijkstra0(G, source, target);
  cout <<string("Time for dijkstra0: %5.3f pathlength: %d",used_time(T),len);
  cout << endl;

  len = dijkstra01(G, source, target);
  cout <<string("Time for dijkstra0: %5.3f pathlength: %d",used_time(T),len);
  cout << endl;
/*
  len = dijkstra1(G, source, target);
  cout <<string("Time for dijkstra1: %5.3f pathlength: %d",used_time(T),len);
  cout << endl;

  len = dijkstra11(G, source, target);
  cout <<string("Time for dijkstra1: %5.3f pathlength: %d",used_time(T),len);
  cout << endl;
*/
  len = dijkstra2(G, source, target);
  cout <<string("Time for dijkstra2: %5.3f pathlength: %d",used_time(T),len);
  cout << endl;
  /*
  len = dijkstra3(G, source, target);
  cout <<string("Time for dijkstra3: %5.3f pathlength: %d",used_time(T),len);
  cout << endl;
*/

  
  len = moore0(G, source, target);
  cout <<string("Time for moore0:    %5.3f pathlength: %d",used_time(T),len);
  cout << endl;
  
  len = moore1(G, source, target);
  cout <<string("Time for moore1:    %5.3f pathlength: %d",used_time(T),len);
  cout << endl;

  return 0;
}
