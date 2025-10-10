#include <LEDA/graph/graph.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/system/assert.h>

using namespace leda;

using std::cout;
using std::cin;


int main(int argc, char** argv)
{
  graph G;
  node s,t;
  edge_map<int>  cap(G);

  dimacs_mf D(cin);
  D.translate(G,s,t,cap);

  int U = 0;
  edge e;
  forall_out_edges(e,s) U+= cap[e];

  node s1 = G.new_node();
  edge e1 = G.new_edge(s1,s);
  cap[e1] = U;

  write_dimacs_mf(cout,G,s1,t,cap);

  return 0;
}

