#include <LEDA/graph/graph_morphism.h>

using namespace leda;
using std::cout;
using std::endl;

int main()
{
  
  graph_morphism<graph ,vf2<graph> > alg;  // or just: graph_morphism<> alg;

  GRAPH<int,int> g1,g2;

  random_graph(g1,5,10);
  //random_graph(g2,5,10);

  g2 = g1;

  if (alg.find_iso(g1,g2))
   cout << "Graphs are ismorphic." << endl;
  
  g1.clear();
  g2.clear();
  complete_graph(g1,10,true);
  g2 = g1;
  if(alg.find_iso(g1,g2))
    cout << "Graphs are ismorphic." << endl;

  return 0;
}



