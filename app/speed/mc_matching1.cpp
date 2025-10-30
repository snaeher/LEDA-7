#include <LEDA/graph/mc_matching.h>
#include <LEDA/graph/mc_matching_gabow.h>


using namespace leda;

using std::cout;
using std::endl;
using std::flush;

node complete(graph& G, int m)  
{ 
  // complete graph with
  // sqrt(2m) nodes and about m edges
  // return first node

  int n = 2 * (int) sqrt(m/2.0); 

  array<node> A(n);
  for (int i=0; i < n; i++) A[i] = G.new_node(); 

  for (int i=0; i < n; i++) 
    for (int j = i+1; j < n; j++) G.new_edge(A[i],A[j]);   

  return A[0];
}


void chain(graph& G, int k, node z, list<node>& L)
{ 
  // k new nodes and 2k - 1 edges 

  array<node> A(k); 

  for (int i=0; i < k; i++) {
    node v = G.new_node();
    L.append(v);
    A[i] = v;
  }

  G.new_edge(A[0],z);
  
  for (int i = 1; i < k-1; i++) 
  { G.new_edge(A[i],A[i+1]);
    G.new_edge(A[i],z);      
   }

  G.new_edge(A[0],A[1]);
}


void worst_case_gen(graph& G, int n, int m, int mode)
{ 
  // SHORT: mode == 0 
  // LONG:  mode == 1 

  // #nodes = sqrt(2m) + n*(mode+1) 
  // #edges = m + 2n*(mode+1)

  node z = complete(G,m);

  list<node> L = G.all_nodes();

  list<node> chain_nodes;

  int k = 4;

  for (int j=0; j < n/(2*k); j++) chain(G,2*k,z,chain_nodes); 

  if (mode == 1) {
    for (int k = 5; k < sqrt(n); k++) chain(G,2*k,z,chain_nodes);
  }

  // partial permute (only chain nodes)

  chain_nodes.permute();
  L.conc(chain_nodes);
  G.sort_nodes(L);

}



void run_tests(int i, int gen, int n, int m, bool permute = false)
{
  // we start with a fresh instance of the memory manager

    std_memory.clear();

    graph G;

    switch (gen) {

     case 1: random_graph(G,n,m);
             break;

     case 2: worst_case_gen(G,n,m,0);
             break;

     case 3: worst_case_gen(G,n,m,1);
             break;
    }

    n = G.number_of_nodes();
    m = G.number_of_edges();

    if (permute)
    { cout << endl;
      cout << string("    randomly permuted node list:");
     }
    else
    { cout << endl;
      //cout << string("%2d: |V| = %d  |E| = %d  d = %.1f",i,n,m,double(m)/n);
      cout << string("%2d: |V| = %d  |E| = %d",i,n,m);
     }
    cout << endl;
    
   
    double T = cpu_time();

    if (permute) {
      list<node> V = G.all_nodes();
      V.permute();
      G.sort_nodes(V);
/*
      // may be expensive
      list<edge> E = G.all_edges();
      E.permute();
      G.sort_edges(E);
*/
    }


    cout << "    MAX_CARD_MATCHING_EDMONDS     " << flush;
    node_array<int> OSC1(G);
    list<edge> M1 = MAX_CARD_MATCHING_EDMONDS(G,OSC1,0);
    cout << string("|M| = %5d   time: %5.2f sec ", M1.length(),
                                                 cpu_time(T)) << endl;
/*
    cout << "    MAX_CARD_MATCHING_EDMONDS     " << flush;
    // greedy heuristic
    M1 = MAX_CARD_MATCHING_EDMONDS(G,OSC1,1);
    cout << string("|M| = %5d   time: %5.2f sec  heur=1", M1.length(),
                                                 cpu_time(T)) << endl;
*/

    cout << "    MAX_CARD_MATCHING_KECECIOGLU  " << flush;
    node_array<int> OSC2(G);
    list<edge> M2 = MAX_CARD_MATCHING_KECECIOGLU(G,OSC2);
    cout << string("|M| = %5d   time: %5.2f sec  ", M2.length(),
                                                    cpu_time(T)) << endl;

/*
    cout << "    MAX_CARD_MATCHING_KECECIOGLU  " << flush;
    // greedy heuristic (slow)
    M2 = MAX_CARD_MATCHING_KECECIOGLU(G,OSC2,1);
    cout << string("|M| = %5d   time: %5.2f sec  heur=1", M2.length(),
                                                 cpu_time(T)) << endl;
*/


    cout << "    MAX_CARD_MATCHING_GABOW       " << flush;
    G_card_matching P(G);
    node_array<int> OSC3(G);
    list<edge> M3 = MAX_CARD_MATCHING_GABOW(G,OSC3);
    cout << string("|M| = %5d   time: %5.2f sec", M3.length(),
                                                 cpu_time(T)) << endl;
/*
    double t = cpu_time(T);
    cout << string("c1 = %.10f   c2 = %.10f", t/n, t/(double(n)*n)) << endl;
    double c = t/n;
*/
                                                           

    assert(M1.length() == M2.length());
    assert(M2.length() == M3.length());
   
    assert(CHECK_MAX_CARD_MATCHING(G, M1, OSC1));
    assert(CHECK_MAX_CARD_MATCHING(G, M2, OSC2));
    assert(CHECK_MAX_CARD_MATCHING(G, M3, OSC3));
   
}




int main()
{
 //system("clear");

 cout << endl;
 cout << "\
 We compare the efficiency of LEDA's Maximum Cardinality Matching\n\
 algorithms for general graphs.\n\
 \n\
 MAX_CARD_MATCHING_EDMONDS (old)\n\
 the original blossom-shrinking algorithm by Edmonds/Gabow\n\
 \n\
 MAX_CARD_MATCHING_KECECIOGLU\n\
 a variant of Edmonds/Gabow using an heuristic proposed by Kecelioglu\n\
 \n\
 MAX_CARD_MATCHING_GABOW (new)\n\
 the algorithm by H.N. Gabow implemented by Ansaripour/Danaei/Mehlhorn\n\
 (https://arxiv.org/abs/2409.14849)" << endl;

  cout << endl; 

  cout << " Input graphs can be created by 3 different generators" << endl;
  cout << " (see arxiv.org/abs/2409.14849 for details)" << endl;
  cout << endl;
  cout << " 1. random sparse graphs" << endl;
  cout << endl;
  cout << " 2. worst-case graphs with short chains" << endl; 
  cout << "    expected running time is O(n) for GABOW and O(n^2) for EDMONDS.";
  cout << endl;
  cout << endl;
  cout << " 3. worst-case graphs with short and long chains" << endl;
  cout << "    expected running time is O(n^1.5) for GABOW and O(n^2) for EDMONDS.";
  cout << endl;
  cout << endl;

  int gen = 0;
  while (gen < 1 || gen > 3) gen = read_int(" choose generator (1/2/3) : " );
  cout << endl;

  switch (gen) {

    case 1: cout << " Generator 1: random sparse graphs." << endl;
            break;

    case 2: cout << " Generator 2: worst-case with short chains." <<  endl;
            break;

    case 3: cout << " Generator 3: worst-case with short and long chains." << endl;
            break;
   }

  cout << endl;


  int N = read_int(" #iterations N =  "); 
  cout << endl;

  int density = (gen == 1) ? 2 : 4;

//density = read_int(" density = ");

  int delta = (gen == 1) ? 10000 : 5000;

  for(int i = 1; i <= N; i++)  {
    int n = i*delta;
    int m = n*density;
    run_tests(i,gen,n,m,false);
    run_tests(i,gen,n,m,true);
  }

  cout << endl;
  cout << "FINISHED" << endl;
  cout << endl;
 

  return 0;

}

