#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
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


  node_array<int>  compnum1(G);

  node_array<int>  compnum2;
  compnum2.use_node_data(G);

  scc_test<graph>  scc;

  cout << endl;
  cout << "Strongly Connected Components (leda::graph)" << endl;
  cout << endl;

  cout << "             ";
  cout << "  Cheriyan/Mehlhorn                      Tarjan  (Kosaraju/Sharir)";
  cout << endl;

  cout << "             ";
  cout << " orig  new   non-rec                orig  new   non-rec" << endl;
  cout << endl;

  cout << "node array   ";
  scc.run(G,compnum1);

  cout << "dynamic slot ";
  scc.run(G,compnum2);

  cout << endl;

 return 0;
}

