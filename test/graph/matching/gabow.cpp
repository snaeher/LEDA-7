#include <LEDA/graph/mc_matching.h>
/*
#include <LEDA/graph/mc_matching_gabow.h>
*/

using namespace leda;

using std::cout;
using std::endl;
using std::flush;

int main()
{

  cout << endl;

  for(int i = 0; i <= 10; i++) 
  {
    int n = 5000*i + 10000;
    int m = 2*n;

    graph G;
    random_graph(G,n,m);

    node_array<int> OSC(G);

    float T = used_time();

    cout << string("MAX_CARD_MATCHING_GABOW: |V| = %5d  |E| = %6d  ",n,m);
    cout << flush;

/*
    int num_phases = 0;
    G_card_matching P(G);
    node_array<int> OSC(G);
    list<edge> M = P.solve(OSC,num_phases);
    assert(CHECK_MAX_CARD_MATCHING(G, M, OSC));
    cout << string("|M| = %5d  time: %.2f s  (%d phases)",
          M.length(),used_time(T),num_phases) << endl;
*/

    list<edge> M = MAX_CARD_MATCHING_GABOW(G,OSC);

    cout << string("|M| = %5d  time: %.2f s",M.length(),used_time(T)) << endl;

  }
 

  return 0;

}

