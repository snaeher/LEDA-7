/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  planar_map.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/planar_map.h>
#include <LEDA/graph/graph_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



void print_face(const PLANAR_MAP<int,int,int>& M, face f)
{ cout << string("F%d: ",M[f]);
  list<node> L1 = M.adj_nodes(f);
  node v;
  forall(v,L1) M.print_node(v);
  cout << endl;
 }


int main()

{  GRAPH<int,int> G;
   node v;
   edge e;
   face f;

   test_graph(G);

   Delete_Loops(G);
   Make_Bidirected(G);


   int count = 0;
   forall_nodes(v,G) G[v] = count++;
   count = 0;
   forall_edges(e,G) G[e] = count++;

   G.make_planar_map();

   PLANAR_MAP<int,int,int>  M(G);

   count = 0;
   forall_faces(f,M) M[f] = count++;

   cout << endl;
   forall_faces(f,M) print_face(M,f);

   cout << endl;
   cout << "faces adjacent to nodes" << endl;

   forall_nodes(v,M)
   { M.print_node(v);
     cout << " : ";
     forall_adj_faces(f,v) cout << string("F%d ",M[f]);
     cout << endl;
    }

/*
   M.triangulate();
*/
   forall_faces(f,M) print_face(M,f);
   cout << endl;


   cout << endl;
   while( Yes("split faces ? ") )
   { list<face> FL= M.all_faces();
      int i = 0;
      forall(f,FL) 
      { cout << "split face: ";
        M.print_face(f);
        cout << endl;
        M.new_node(f,--i);
      }
   
      forall_faces(f,M) print_face(M,f);
      cout << endl;
   }
   
   
   list<edge> E;
   edge_array<bool> marked(M,false);

   forall_edges(e,M)
    if (!marked[e])
    { E.append(e);
      marked[M.reverse(e)] = true;
     } 

   E.pop();

   forall(e,E)
   { Yes("del_edge ");
     M.del_edge(e);
     forall_faces(f,M) print_face(M,f);
     cout << endl;
   }

 return 0;
}
