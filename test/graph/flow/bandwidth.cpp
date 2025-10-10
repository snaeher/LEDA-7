#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/system/assert.h>


using namespace leda;

using std::cout;
using std::cin;
using std::cerr;
using std::flush;
using std::endl;
using std::ofstream;



int main(int argc, char** argv)
{
  typedef static_graph<opposite_graph> st_graph;
  typedef st_graph::node node;
  typedef st_graph::edge edge;


  st_graph G;
  node s=0;
  node t=0;

  edge_array<int,st_graph>   cap(G);
  edge_array<int,st_graph>  flow(G);

  cout << endl;
  cout << "reading DIMACS maxflow problem from std input ... ";
  cout << flush;

  dimacs_mf D(cin);
  D.translate(G,s,t,cap);

  cout << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;


  cout << string("bw1 = %.0f",G.bandwidth()) << endl;
  cout << endl;

  G.rebuild_in_edges();
  
  cout << string("bw2 = %.0f",G.bandwidth()) << endl;
  cout << endl;


  G.permute_in_edges();
  
  cout << string("bw3 = %.0f",G.bandwidth()) << endl;
  cout << endl;


  return 0;
}




