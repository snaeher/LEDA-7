/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  dimacs.c
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
#include <LEDA/core/queue.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/system/stream.h>

#include <math.h>

using namespace leda;

using std::istream;
using std::ifstream;
using std::cout;
using std::cerr;
using std::flush;
using std::endl;



string read_dimacs(istream& in, GRAPH<int,int>& G, node& s, node& t)
{
  G.clear();
  s = nil;
  t = nil;

  string problem;

  node* V = 0;

  char c;

  int  n;
  int  m;

  while (in >> c)
  {
    switch (c) {

    case 'c' : { // comment 
                 read_line(in);
                 break;
                }


    case 't' : { // type ?
                 read_line(in);
                 break;
                }



    case 'p' : { // problem definition
                 in >> problem >> n >> m;
                 read_line(in);
                 // construct nodes
                 V = new node[n+1];
                 for(int i=1; i<=n; i++) V[i] = G.new_node(i);
                 break;
                }

    case 'n' : { // node definition (source / target)

                 int  x;

                 if (problem == "sp")
                 { // shortest path (read source node)
                   in >> x;
                   read_line(in);
                   s = V[x];
                   break;
                 }

                 if (problem == "max")
                 { // maxflow
                   char c;
                   in >> x >> c;
                   read_line(in);
                   if (c == 's') s = V[x];
                   else
                   if (c == 't') t = V[x];
                   else error_handler(1,"read_dimacs: illegal node definition");
                   break;
                 }
                }
             

    case 'a' : { // arc definition
                 int i,j,c;
                 in >> i >> j >> c;
                 read_line(in);
                 G.new_edge(V[i],V[j],c);
                 break;
                }
    }

  }

 if (problem != "sp" && problem != "max")
    error_handler(1, string("unknown problem type: ") + problem);

 if (s == nil) 
    error_handler(1,"read_dimacs: no source defined.");

 if (t == nil && problem == "max") 
    error_handler(1,"read_dimacs: no target defined.");

 delete[] V;

 cerr << string ("nodes : %6d",G.number_of_nodes()) << endl;
 cerr << string ("edges : %6d",G.number_of_edges()) << endl;
 cerr << endl;

 return problem;

}


int main(int argc, char** argv)
{
  if (argc < 2) 
  { cerr << endl;
    cerr << "usage: dimacs file" << endl;
    cerr << endl;
    return 1;
   }

  ifstream in(argv[1]);

  GRAPH<int,int> G;
  node s,t;

  string problem = read_dimacs(in,G,s,t);

  if (problem == "max")
  { edge_array<int> flow(G);
    cout << "MAX_FLOW              " << flush;
    float T  = used_time();
    int f = MAX_FLOW(G,s,t,G.edge_data(),flow) ;
    cout << string("time: %6.2f sec  f = %d",used_time(T),f) << endl;
   }

  if (problem == "sp")
  { node_array<int> dist(G);
    node_array<edge> pred(G);
    cout << "Dijkstra<int>             " << flush;
    float T  = used_time();
    DIJKSTRA(G,s,G.edge_data(),dist,pred) ;
    cout << string("time: %6.2f sec",used_time(T)) << endl;
   }

  return 0;
}
  
