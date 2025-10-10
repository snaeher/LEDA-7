#include <LEDA/graph/mc_matching.h>
#include <LEDA/graph/mc_matching_gabow.h>


using namespace leda;

using std::cout;
using std::endl;
using std::flush;




int main()
{

  for(int i = 0; i <= 10; i++) 
  {
    int n = 10000 + 5000*i;
    int m = 2*n;

    cout << endl;
    cout << string("|V| = %d  |E| = %d",n,m) << endl;
    cout << endl;
   
    graph G;
    random_graph(G,n,m);
   
    float T = used_time();

   
    G_card_matching P(G);
   
    cout << "MAX_CARD_MATCHING_GABOW:       " << flush;
    node_array<int> OSC(G);
    int num_phases = 0;
    list<edge> M = P.solve(OSC,num_phases);
    cout << string("|M| = %d  time: %.2f sec  (%d phases)", M.length(),
                                                            used_time(T),
                                                            num_phases) << endl;


    cout << "MAX_CARD_MATCHING_KECECIOGLU:  " << flush;
    node_array<int> OSC1(G);
    list<edge> M1 = MAX_CARD_MATCHING_KECECIOGLU(G,OSC1,0);
    cout << string("|M| = %d  time: %.2f sec  heur=0", M1.length(),
                                                       used_time(T)) << endl;

    cout << "MAX_CARD_MATCHING_KECECIOGLU:  " << flush;
    M1 = MAX_CARD_MATCHING_KECECIOGLU(G,OSC1,1);
    cout << string("|M| = %d  time: %.2f sec  heur=1", M1.length(),
                                                       used_time(T)) << endl;

    cout << "MAX_CARD_MATCHING_KECECIOGLU1: " << flush;
    node_array<int> OSC2(G);
    list<edge> M2 = MAX_CARD_MATCHING_KECECIOGLU1(G,OSC2,0);
    cout << string("|M| = %d  time: %.2f sec  heur=0", M2.length(),
                                                       used_time(T)) << endl;

    cout << "MAX_CARD_MATCHING_KECECIOGLU1: " << flush;
    M2 = MAX_CARD_MATCHING_KECECIOGLU1(G,OSC2,1);
    cout << string("|M| = %d  time: %.2f sec  heur=1", M2.length(),
                                                       used_time(T)) << endl;

    cout << "MAX_CARD_MATCHING_EDMONDS:     " << flush;
    node_array<int> OSC3(G);
    list<edge> M3 = MAX_CARD_MATCHING_EDMONDS(G,OSC3,0);
    cout << string("|M| = %d  time: %.2f sec  heur=0", M3.length(),
                                                       used_time(T)) << endl;

    cout << "MAX_CARD_MATCHING_EDMONDS:     " << flush;
    M3 = MAX_CARD_MATCHING_EDMONDS(G,OSC3,1);
    cout << string("|M| = %d  time: %.2f sec  heur=1", M3.length(),
                                                       used_time(T)) << endl;
   
    assert(M.length() == M1.length());
    assert(M.length() == M2.length());
    assert(M.length() == M3.length());

    assert(CHECK_MAX_CARD_MATCHING(G, M,  OSC));
    assert(CHECK_MAX_CARD_MATCHING(G, M1, OSC1));
    assert(CHECK_MAX_CARD_MATCHING(G, M2, OSC2));
    assert(CHECK_MAX_CARD_MATCHING(G, M3, OSC3));
   
  }
 

  return 0;

}

