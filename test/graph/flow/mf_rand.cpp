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
  edge_array<int>  cap(G);

  dimacs_mf D(cin);
  D.translate(G,s,t,cap);

  if (argc > 1) rand_int.set_seed(atoi(argv[1]));

/*
  list<edge> E = G.all_edges();
  //E.reverse_items();
  E.permute();
  G.sort_edges(E);
*/
  G.permute_edges();

  write_dimacs_mf(cout,G,s,t,cap);

  return 0;
}

