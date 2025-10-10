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
  node s=0;
  node t=0;


  edge_map<int> ucap(G,0);


  cerr << endl;
  cerr << "reading DIMACS maxflow problem from std input ... ";
  cerr << flush;

  dimacs_mf D(cin);
  D.translate(G,s,t,ucap);

  cerr << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cerr << string("%6d nodes",n) << endl;
  cerr << string("%6d edges",m) << endl;
  cerr << endl;

  edge ts = G.new_edge(t,s);

  node_array<int> supply(G,0);
  edge_array<int> lcap(G,0);
  edge_array<int> cost(G,0);

  int F = 0;
  edge e;
  forall_out_edges(e,s) F += ucap[e];

  ucap[ts] = F;
  cost[ts] = -1;

  write_dimacs_mcf(cout,G,supply,lcap,ucap,cost);


  return 0;
}




