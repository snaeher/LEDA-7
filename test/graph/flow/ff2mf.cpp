#include <LEDA/graph/graph.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/system/assert.h>

using namespace leda;

using std::cin;
using std::cout;
using std::cerr;
using std::flush;
using std::endl;

int main(int argc, char** argv)
{
  graph G;

  node_map<int> supply(G,0);
  edge_map<int> lcap(G,0);
  edge_map<int> ucap(G,0);
  edge_map<int> cost(G,0);

  dimacs_mcf D(cin);
  D.translate(G,supply,lcap,ucap,cost);

  //eliminate lower caps

  node v;
  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v)
    { int lc = lcap[e];
      if (lc == 0) continue;                  // nonzero lower capacity bound
      node w  = G.target(e);
      ucap[e] -= lc;
      supply[v] -= lc;
      supply[w] += lc;
     }
  }



  node s = G.new_node();
  node t = G.new_node();

  forall_nodes(v,G)
  { if (v == s) continue;
    if (v == t) continue;
    int b = supply[v];
    if (b > 0)
    { edge x = G.new_edge(s,v);
      ucap[x] = b;
     }
    if (b < 0)
    { edge x = G.new_edge(v,t);
      ucap[x] = -b;
     }
   }

  write_dimacs_mf(cout,G,s,t,ucap);

  return 0;
}




