#include <LEDA/graph/graph.h>
#include <LEDA/graph/mc_matching.h>
#include <LEDA/core/array.h>

#include <assert.h>

#include <math.h>

using namespace leda;

using std::cout;
using std::endl;
using std::flush;


node complete_g(graph& G, int m)  // sqrt(2m) nodes and about m edges
{ int n = 2* (int) sqrt(m/2.0); 
  array<node> a(n); int i, j;
  for (i = 0; i < n; i++) 
    a[i] = G.new_node(); 
  for (i = 0; i < n; i++) 
    for (j = i+1; j < n; j++) G.new_edge(a[i],a[j]);   

  return a[0];
}

void chain(graph& G, int k, node z){ // k new nodes and 2k - 1 edges 
  array<node> a(k); int i;

  for (i = 0; i < k; i++) a[i] = G.new_node();
  
  G.new_edge(a[0],z);
  
  for (i = 1; i < k-1; i++) 
  { G.new_edge(a[i],a[i+1]);
    G.new_edge(a[i],z);      
  }
  G.new_edge(a[0],a[1]);
}

void mc_worst_case_gen(graph& G, int n, int m, int mode = 1)
// sqrt(2m) + n + (mode == 1) n nodes and m + 2n + (mode == 1) 2n edges.
{ node z = complete_g(G,m);
  int k = 4;
  for (int j = 0; j < n/8; j++) chain(G,2*k,z); 
  if (mode == 1) {
    for (k = 5; k < sqrt(n); k++)
    chain(G,2*k,z);
  }
}

void triangle_graph(graph& G, int k, int mode = 1){
  int K = pow(2,k);
  int n = 3*K;
  array<node> v(n);
  for (int i = 0; i < n; i++) v[i] = G.new_node();
  for (int j = 0; j < k; j++){
    G.new_edge(v[3*j], v[3*j + 1]);
    G.new_edge(v[3*j + 1], v[3*j + 2]);
    G.new_edge(v[3*j + 2], v[3*j]);
    if (j < k-1 && mode == 1) G.new_edge(v[3*j + j % 3], v[3*(j+1) + j % 3]);
    if (j < k-1 && mode == 2){
      G.new_edge(v[3*j], v[3*(j+1)]);
      G.new_edge(v[3*j + 1], v[3*(j+1) + 1]);
      G.new_edge(v[3*j + 2], v[3*(j+1) + 2]);
    }
  }
  //permute(G);
}


void permute(graph& G)
{ 
  list<edge> E = G.all_edges();
  E.permute();
  G.sort_edges(E); 
   
  list<node> V = G.all_nodes();
  V.permute();
  G.sort_nodes(V);
}


int main(int argc, char** argv)
{
  int n_start = 10000;
  int n_delta = 10000;
  int n_steps = 10;

  if (argc > 1) {
    n_start = atoi(argv[1]);
    n_steps = 1;
  }

  //rand_int.set_seed(1234567); (not necessary)

  cout << endl;

  for(int i = 0; i<n_steps; i++) 
  {
    int n = n_start + i*n_delta;
    int m = 3*n; 
  
    graph G; 
    mc_worst_case_gen(G,n,m,0);

    n = G.number_of_nodes();
    m = G.number_of_edges();

    double T = cpu_time(); 

    cout << string("|V| =%6d |E| = %6d |M| = ", n,m) << flush;

    node_array<int> OSC(G);
    list<edge> M = MAX_CARD_MATCHING(G,OSC,1);

    double t = cpu_time(T);

    //double f = (1e9*t)/(double(n)*double(n)); 

    //double f = (1e9*t)/(double(n)*double(n)*log(n)); 
    //double f = (1e9*t)/(double(n)*double(n)*sqrt(n)); 
    //double f = (1e10*t)/(double(n)*double(n)*sqrt(n)); 
    //double f = (1e10*t)/(double(n)*double(n)*sqrt(n)); 
    //double f = (1e10*t)/(double(n)*double(n)*log(n)*log(n)); 

    double f = (1e9*t)/(double(n)*double(m)); 

    cout << string("%5d  t =%6.2f = %5.2f*n^2/10^9 s",M.length(),t,f) << endl;

    CHECK_MAX_CARD_MATCHING(G,M,OSC);
  }

  return 0;
}

