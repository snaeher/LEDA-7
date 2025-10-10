#include <LEDA/graph/dimacs.h>
#include <LEDA/core/array2.h>
#include <LEDA/system/assert.h>

using namespace leda;

using std::cout;
using std::cerr;
using std::endl;




int main(int argc, char** argv)
{
  if (argc < 4)
  { cerr << "usage: " << argv[0] << " cols rows seed [b]" << endl;
    return 1;
   }

  graph G;

  int cols = atoi(argv[1]); // s and t
  int rows = atoi(argv[2]);
  int seed = atoi(argv[3]);

  int n = cols*rows;

  array2<node> A(rows,cols);

  node s = G.new_node();

  if (argc == 5 && string(argv[4]) == "bbb") 
  { int first = 0;
    int last = n/2 + n%2;
    for(int k=0; k<n; k++)
    { int pos = (k%2 == 0) ? first++ : last++;
      int i = pos/cols;
      int j = pos%cols;
      A(i,j) = G.new_node();
     }
   }
  else
  if (argc == 5 && string(argv[4]) == "bb") 
  { int first = 0;
    int last = n-1;
    for(int k=0; k<n; k++)
    { int pos = (k%2 == 0) ? first++ : last--;
      int i = pos/cols;
      int j = pos%cols;
      A(i,j) = G.new_node();
     }
   }
  else
  if (argc == 5 && string(argv[4]) == "b") 
    for(int i=0; i<rows; i++)
       for(int j=0; j<cols; j++) A(i,j) = G.new_node();
  else
    for(int j=0; j<cols; j++)
      for(int i=0; i<rows; i++) A(i,j) = G.new_node();


  if (argc == 5 && string(argv[4]) == "r") 
  { for(int i=0; i<rows; i++)
      for(int j=0; j<cols; j++) 
      { int ii = rand_int(0,rows-1);
        int jj = rand_int(0,cols-1);
        swap(A(i,j),A(ii,jj));
       }
  }



  for(int j=0; j<cols; j++) 
  { for(int i=0; i<rows; i++)
    { for(int k=0; k<rows; k++) 
        if (i != k) G.new_edge(A(i,j),A(k,j));
      if (j < cols-1) G.new_edge(A(i,j),A(i,j+1));
     }
   }


  node t = G.new_node();

  for(int i=0; i<rows; i++)
  { G.new_edge(s,A(i,0));
    G.new_edge(A(i,cols-1),t);
   }

  edge_array<int> cap(G);

  rand_int.set_seed(seed);

  edge e;
  forall_edges(e,G)     cap[e] = rand_int(10,1000);
  forall_out_edges(e,s) cap[e] = rand_int(900,1000);
  forall_in_edges(e,t)  cap[e] = rand_int(10,100);

  edge_array<int> index(G);
  int count = 0;

  node v;
  forall_nodes(v,G)
  { edge e;
    forall_out_edges(e,v) index[e] = count++;
   }


  double bw = 0;

  forall_nodes(v,G)
  { edge e_last = G.first_in_edge(v);
    edge e;
    forall_in_edges(e,v)
    { int d = index[e] - index[e_last];
      bw += (d > 0) ? d : -d;
      e_last = e;
      //cerr << index[e] << " " << cap[e] << endl;
     }
   }

  cerr << endl;
  cerr << string("bw = %.0f",bw) << endl;
  cerr << endl;


  Write_Dimacs_MF(cout,G,s,t,cap);

  return 0;
}

