#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/templates/strong_components.h>

#include <assert.h>
#include <stdlib.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


int main(int argc, char** argv) 
{

  int n,m;

  if (argc < 3) 
  { n = read_int("n = ");
    m = read_int("m = ");
   }
  else
  { n = atoi(argv[1]);
    m = atoi(argv[2]);
   }


  graph G(1,0);
  random_graph(G,n,m);

/*
  edge e;
  forall_edges(e,G)
     if (G.source(e) == G.target(e)) G.del_edge(e);
*/

  typedef static_graph<directed_graph,data_slots<1>,data_slots<0> > st_graph;

  st_graph Gs;
  Gs.start_construction(n,m);

  node_array<st_graph::node> N(G);

  node v;
  forall_nodes(v,G)
     N[v] = Gs.new_node();

  forall_nodes(v,G)
  { st_graph::node x = N[v];
    edge e;
    forall_out_edges(e,v)
    { st_graph::node y = N[G.target(e)]; 
      Gs.new_edge(x,y);
     }
   }

  Gs.finish_construction();


  node_array<int,st_graph>  compnum1(Gs);

  node_array<int,st_graph>  compnum2;
  compnum2.use_node_data(Gs);

  node_slot<int,st_graph,0> compnum3(Gs);


  scc_test<st_graph>  scc;

  cout << endl;
  cout << "Strongly Connected Components (leda::static_graph)" << endl;
  cout << endl;

  cout << "             ";
  cout << "  Cheriyan/Mehlhorn                      Tarjan  (Kosaraju/Sharir)";
  cout << endl;

  cout << "             ";
  cout << " orig  new   non-rec                orig  new   non-rec" << endl;
  cout << endl;

  cout << "node array   ";
  scc.run(Gs,compnum1);

  cout << "dynamic slot ";
  scc.run(Gs,compnum2);

  cout << "static slot  ";
  scc.run(Gs,compnum3);

  cout << endl;

 return 0;
}


