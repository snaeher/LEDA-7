/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_sort_edges_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/graphwin.h>
// #include <LEDA/graph/graph_alg.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


graph G;


int sign_of(const double& x)
{ if ( x == 0 ) return 0;
  else if (x > 0 ) return 1;
       else return -1;
}

int compare_vectors(const double& x1, const double& y1, const double& x2, const double& y2)
{ 
  if (x1 == x2 && y1 == y2) return 0;
  if (x1 == 0 && y1 == 0) return -1;
  if (x2 == 0 && y2 == 0) return +1;
  // vectors are distinct and non-zero

  int sy1 = sign_of(y1);  int sy2 = sign_of(y2); 

  int upper1 = ( sy1 != 0 ? sy1 : sign_of(x1) );
  int upper2 = ( sy2 != 0 ? sy2 : sign_of(x2) );

  if ( upper1 == upper2 ) return sign_of(x2*y1 - x1*y2);

  return upper2 - upper1;
}



class cmp_edges_by_angle : public leda_cmp_base<edge> 
{
  const node_array<double>* X;
  const node_array<double>* Y;

public:

cmp_edges_by_angle(const node_array<double>& x, const node_array<double>& y)
: X(&x), Y(&y) {}

int operator()(const edge& e, const edge& f) const
{ if ( source(e) != source(f) ) 
    return compare(index(source(e)),index(source(f)));
  else
  { node v = source(e); 
    node w1 = target(e); node w2 = target(f);
    double x1 = (*X)[w1] - (*X)[v]; double y1 = (*Y)[w1] - (*Y)[v];
    double x2 = (*X)[w2] - (*X)[v]; double y2 = (*Y)[w2] - (*Y)[v];
    return compare_vectors(x1,y1,x2,y2);
   }
} 

};



void sort(GraphWin& gw)
{ 
  node_array<double> x(G);
  node_array<double> y(G);

  node v;
  forall_nodes(v,G)
  { point p = gw.get_position(v);
      x[v] = p.xcoord();
      y[v] = p.ycoord();
  }

  cmp_edges_by_angle cmp(x,y);

  list<edge> L = G.all_edges();
  L.sort(cmp);
  G.sort_edges(L);

  cout << "\n\nThe adjacency lists are:\n";
  G.print();
}


void init_handler(GraphWin& gw)
{ list<edge> L;
  G.make_map(L);
  sort(gw);
}


void new_edge_handler(GraphWin& gw, edge e)  
{ G.set_reversal(e,gw.new_edge(G.target(e),G.source(e)));
  sort(gw);
}


bool del_edge_handler(GraphWin& gw, edge e) 
{ gw.del_edge(G.reversal(e)); return true; }
void new_node_handler(GraphWin& gw,node)   {}
void del_node_handler(GraphWin& gw)        {}


int main()
{ GraphWin gw(G,"Plane Map from Geometry");
 
  gw.set_init_graph_handler(init_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(true);

  gw.display();
  gw.edit();

  return 0;
}


