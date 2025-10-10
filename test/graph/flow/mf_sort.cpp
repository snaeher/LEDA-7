#include <LEDA/graph/graph.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/system/assert.h>

using namespace leda;

using std::cout;
using std::cin;


static edge_array<int>* pcap;

int cmp(const edge& v, const edge& w)
{
  int v_cap = (*pcap)[v];
  int w_cap = (*pcap)[w];

  int c = compare(v_cap,w_cap);
  if (c == 0)
    return compare(index(w),index(v));
  else
   return c;
}

int main(int argc, char** argv)
{
  graph G;
  node s,t;
  edge_array<int>  cap(G);

 pcap = &cap;

  dimacs_mf D(cin);
  D.translate(G,s,t,cap);

  G.sort_edges(cmp);

  if (argc > 1 && string(argv[1]) == "-r")
  { list<edge> E = G.all_edges();
    E.reverse_items();
    G.sort_edges(E);
   }

  write_dimacs_mf(cout,G,s,t,cap);

  return 0;
}

