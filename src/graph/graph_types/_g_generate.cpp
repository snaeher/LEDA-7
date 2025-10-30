/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_generate.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/ugraph.h>
#include <LEDA/graph/graph_misc.h>

#include <LEDA/numbers/vector.h>
#include <LEDA/numbers/matrix.h>
#include <LEDA/core/array2.h>

#include <LEDA/graph/series_parallel.h>

#include <stdlib.h>


LEDA_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// some graph generators
//
// S. Naeher  (1995-1996)
//------------------------------------------------------------------------------

void complete_graph(graph& G, int n, bool directed)
{ 
  G.clear();

  node* V = new node[n];

  for(int i=0;i<n;i++) V[i] = G.new_node();

  if (directed)
    { //memory_allocate_block(sizeof(node_struct),n);
      for(int i=0;i<n;i++) 
        for(int j=0;j<n;j++) G.new_edge(V[i],V[j]);
     }
  else
    { //memory_allocate_block(sizeof(edge_struct),n*n/2);
      for(int i=0; i<n; i++) 
        for(int j=i+1; j<n; j++) G.new_edge(V[i],V[j]);
     }

  delete[] V;
}



void grid_graph(graph& G, int n) 
{ node_array<double> xcoord; 
  node_array<double> ycoord;
  grid_graph(G,xcoord,ycoord,n);
 }

void d3_grid_graph(graph& G, int n) 
{ node_array<double> xcoord; 
  node_array<double> ycoord;
  node_array<double> zcoord;
  d3_grid_graph(G,xcoord,ycoord,zcoord,n);
 }


static list<node> make_grid(graph& G, node_array<double>& xcoord, 
                                      node_array<double>& ycoord, int n) 
{
  array2<node>  A(n,n);
  int x,y;

  list<node> L;

  double d = 1.0/(n+1);

  for(y=0; y<n; y++)
    for(x=0; x<n; x++)
      { node v = G.new_node();
        A(x,y) = v;
        L.append(v);
        xcoord[v] = (x+1)*d;
        ycoord[v] = (y+1)*d;
       }

  for(x=0; x<n; x++)
    for(y=0; y<n; y++)
       { if (x < n-1) G.new_edge(A(x,y),A(x+1,y));
         if (y < n-1) G.new_edge(A(x,y),A(x,y+1));
        }

  return L;
}



void grid_graph(graph& G, node_array<double>& xcoord, 
                          node_array<double>& ycoord, int n) 
{ int N = n*n;
  G.clear();
  xcoord.init(G,N,0);
  ycoord.init(G,N,0);
  make_grid(G,xcoord,ycoord,n);
}


void d3_grid_graph(graph& G, node_array<double>& xcoord, 
                             node_array<double>& ycoord, 
                             node_array<double>& zcoord, int n) 
{ int N = n*n*n;

  G.clear();
  xcoord.init(G,N,0);
  ycoord.init(G,N,0);
  zcoord.init(G,N,0);

  list<node> L;
  for(int i=0; i<n; i++)
  { list<node> L1 = make_grid(G,xcoord,ycoord,n); 
    node v;
    forall(v,L1) zcoord[v] = float(i)/n;
    list_item it = L1.first();
    forall(v,L) 
    { G.new_edge(v,L1[it]);
      it = L1.succ(it);
     }
    L = L1;
   }
}



void complete_bigraph(graph& G, int n1, int n2, list<node>& A, list<node>& B)
{ 
  G.clear();

  while (n1--)  A.append(G.new_node());
  while (n2--)  B.append(G.new_node());

  list_item a,b;
  forall_items(a,A) 
    forall_items(b,B) 
      G.new_edge(A[a],B[b]);
}

void user_graph(graph& G)
{ int  n = read_int("|V| = ");
  int  i,j;

  node* V = new node[n];
  for(j=0; j<n; j++) V[j] = G.new_node();

  for(j=0; j<n; j++) 
  { list<int> il;
    int ok = false;
    while (!ok)
    { ok = true;
      cout << "edges from [" << j << "] to: ";
      il.read();
      forall(i,il) 
        if (i < 0 || i >= n) 
        { ok=false;
          cout << "illegal node " << i << "\n";
         }
     }
    forall(i,il) G.new_edge(V[j],V[i]);
  }
  G.print();
  if (Yes("save graph ? ")) G.write(read_string("file: "));

  delete[] V;
}



void test_graph(graph& G)
{ 
  G.clear();
  char c;

  do c=read_char("graph: f(ile) r(andom) c(omplete) p(lanar) g(rid) u(ser): ");
  while (c!='f' && c!='r' && c!='c' && c!='p' && c!='g' && c!='u');   

  switch (c) {

   case 'f' : { G.read(read_string("file: "));
                break;
               }

   case 'u' : { user_graph(G);
                break;
               }

   case 'c' : { complete_graph(G,read_int("|V| = "));
                break;
               }

   case 'r' : { int n = read_int("|V| = ");
                int m = read_int("|E| = ");
                random_graph(G,n,m);
                break;
               }

   case 'p' : { random_planar_graph(G,read_int("|V| = "));
                break;
               }

   case 'g' : { grid_graph(G,read_int("n = "));
                break;
               }
   }
}

void test_ugraph(ugraph& G)
{ 
  G.clear();
  char c;

  do c = read_char("graph: f(ile) r(andom) c(omplete) p(lanar) u(ser): ");
  while (c!='f' && c!='r' && c!='c' && c!='p'&& c!='u');   

  int  i;
  node v;
  
  switch (c) {

  case 'f' : { G.read(read_string("file: "));
               break;
              }

   case 'u' : { int  n = read_int("|V| = ");
                int  j = 0;
                node* V = new node[n];
                for(i=0; i<n; i++) V[i] = G.new_node();
                forall_nodes(v,G)
                  { list<int> il;
                    cout << "edges from " << j++ << " to: ";
                    il.read();
                    forall(i,il) 
                      if (i >= 0 && i < n) G.new_edge(v,V[i]);
                      else cerr << "illegal node " << i << " (ignored)\n";
                   }
                G.print();
                if (Yes("save graph ? ")) G.write(read_string("file: "));
                delete[] V;
                break;
               }

   case 'c' : { int n = read_int("|V| = ");
                complete_graph(G,n);
                break;
               }

   case 'r' : { int n = read_int("|V| = ");
                int m = read_int("|E| = ");
                random_graph(G,n,m);
                break;
               }

   }//switch

}





void test_bigraph(graph& G, list<node>& A, list<node>& B)
{
  int a,b;
  int n1 = 0;
  int n2 = 0;
  char c;

  do c = read_char("bipartite graph: f(ile) r(andom) c(omplete) u(ser): ");
  while (c!='f' && c!='r' && c!='c' && c!='u');   

  A.clear();
  B.clear();
  G.clear();

  if (c!='f') 
   { n1 = read_int("|A| = ");
     n2 = read_int("|B| = ");
    }
  
  
  switch (c) {

  case 'f' : { G.read(read_string("file: "));
               node v;
               forall_nodes(v,G) 
               if (G.outdeg(v) > 0) A.append(v);
               else B.append(v);

               break;
              }

   case 'u' : { node* AV = new node[n1+1];
                node* BV = new node[n2+1];

                for(a=1; a<=n1; a++)  A.append(AV[a] = G.new_node());
                for(b=1; b<=n2; b++)  B.append(BV[b] = G.new_node());

                for(a=1; a<=n1; a++)
                { list<int> il;
                  cout << "edges from " << a << " to: ";
                  il.read();
                  forall(b,il) 
                    if (b<=n2) G.new_edge(AV[a],BV[b]);
                    else break;
                  if (b>n2) break;
                 }
                delete[] AV;
                delete[] BV;
                break;
               }

   case 'c' : complete_bigraph(G,n1,n2,A,B);
              break;

   case 'r' : { int m = read_int("|E| = ");
                random_bigraph(G,n1,n2,m,A,B);
                break;
               }

       } // switch

}



void cmdline_graph(graph& G, int argc, char** argv)
{ 
  // construct graph from cmdline arguments

  if (argc == 1)           // no arguments 
     { test_graph(G);
       return;
      }
  else 
     if (argc == 2)       // one argument
        { if (isdigit(argv[1][0])) 
             { cout << "complete graph |V| = " << argv[1];
               cout << endl;
               cout << endl;
               complete_graph(G,atoi(argv[1]));
              }
          else 
             { cout << "reading graph from file " << argv[1];
               cout << endl;
               cout << endl;
               G.read(argv[1]);
              }
          return;
         }
     else
        if (argc == 3 && isdigit(argv[1][0]) && isdigit(argv[1][0])) 
           { cout << "random graph |V| = " << argv[1] << "  |E| = " << argv[2];
             cout << endl;
             cout << endl;
             random_graph(G,atoi(argv[1]),atoi(argv[2]));
             return;
            }

  LEDA_EXCEPTION(1,"cmdline_graph: illegal arguments");
}



//------------------------------------------------------------------------------
// triangulated planar graph
//------------------------------------------------------------------------------


struct triang_point {

double x;
double y;
node   v;
edge   e;   // if v lies on CH, unbounded face is to the left of e

LEDA_MEMORY(triang_point)

triang_point(double a=0, double b = 0) 
{ x = a; y = b; v = nil; e = nil; }

triang_point(const triang_point& p)    
{ x = p.x; y = p.y; v = p.v; e = p.e; }

~triang_point() {};

friend bool right_turn(const triang_point& a, 
                       const triang_point& b, 
                       const triang_point& c)
{ return (a.y-b.y)*(a.x-c.x)+(b.x-a.x)*(a.y-c.y) > 0; }

friend bool left_turn(const triang_point& a, 
                      const triang_point& b, 
                      const triang_point& c)
{ return (a.y-b.y)*(a.x-c.x)+(b.x-a.x)*(a.y-c.y) < 0; }


friend bool operator==(const triang_point& a, const triang_point& b)
{ return a.x == b.x && a.y == b.y; }

friend ostream& operator<<(ostream& out, const triang_point& p)
{ return out << p.x << " " << p.y; }

friend istream& operator>>(istream& in, triang_point& p)
{ return in >> p.x >> p.y; }

};


int cmp_points(const triang_point& p, const triang_point& q)
{ int c = compare(p.x,q.x);
  return (c==0) ? compare(p.y,q.y) : c;
 }



void triangulation_map(graph& G, 
                       list<node>& outer_face,
                       node_array<double>& xcoord,
                       node_array<double>& ycoord, 
                       int n)
{ 
  G.clear();   outer_face.clear();

  if ( n <= 0 ) return;

  list<triang_point> L;

  int K = 1024 * 1024;   // 2^{20}

  int N = n;

  while (L.length() != N)
  { n = N - L.length();

    while(n--)
    { double x = rand_int(0,K-1)/((double) K); 
      double y = rand_int(0,K-1)/((double) K);  
      L.append(triang_point(x,y));
    }

    L.sort(cmp_points);  // sort triang_points lexicographically

    // eliminate multiple triang_points

    list_item it;
    forall_items(it,L)
    { list_item it1 = L.succ(it);
      while (it1 != nil && L[it1] == L[it])
      { L.del(it1);
        it1 = L.succ(it);
      }
    }
  }

  list<triang_point> CH;
  list_item last, it;
  triang_point p,q;

  n = L.length();

  xcoord.init(G,n,0);
  ycoord.init(G,n,0);

  forall_items(it,L)
  { node v = G.new_node();
    xcoord[v] = L[it].x;
    ycoord[v] = L[it].y;
    L[it].v = v;
   }

  // initialize convex hull with first two points

  p = L.pop();
  list_item itp = CH.append(p);

  if ( n > 1)
  { q = L.pop();
    last = CH.append(q);
  
    // CH contains points on hull in clockwise order.
    // if p lies on hull then p.e is edge to successor on hull
    // unbounded face is to the left of p.e
  
    CH[itp].e = G.new_edge(p.v,q.v);
    CH[last].e = G.new_edge(q.v,p.v);
    
    G.set_reversal(CH[itp].e,CH[last].e);
  
  
    // scan remaining points
  
    forall(p,L)
    {
      node v = p.v;
  
      // compute upper tangent (p,up)
  
      list_item up = last;
      list_item it = CH.cyclic_pred(up);
  
      while (left_turn(CH[it],CH[up],p))
      { up = it;
        it = CH.cyclic_pred(up);
       }
  
  
      // compute lower tangent (p,low)
  
      list_item low = last;
      it = CH.cyclic_succ(low);
  
      while (right_turn(CH[it],CH[low],p))
      { low = it;
        it = CH.cyclic_succ(low);
       }
  
  
      // add edges and remove all points between up and low
  
      edge r;
  
      it = up;
      while (true)
      { //assert(CH[it].v != nil);
        //assert(CH[it].e != nil);
        edge e = G.new_edge(CH[it].e,v,leda::behind);
        r = G.new_edge(v,CH[it].v);
        G.set_reversal(e,r);
        if (it == up) CH[it].e = e;
        if (it == low) break;
        list_item it1 = CH.cyclic_succ(it);
        if (it != up ) CH.del(it);
        it = it1;
      }
  
      //assert(Is_Map(G));
      //assert(Genus(G) == 0);
  
      
      // insert new point
  
        p.e = r;
        last = CH.insert(p,up,leda::behind);
    
      
  
     }

  } // if (n > 1)
  
  // compute outer face

  forall(p,CH) outer_face.append(p.v);

  if (!Is_Map(G)) LEDA_EXCEPTION(0,"triangulation map: no map");
  if (Genus(G) != 0) LEDA_EXCEPTION(0,"triangulation map: genus is non-zero");;

 
}


void triangulation_map(graph& G, int n)
{ node_array<double> xcoord;
  node_array<double> ycoord;
  list<node> L;
  triangulation_map(G,L,xcoord,ycoord,n);
}


void triangulation_map(graph& G, 
                             node_array<double>& xcoord, 
                             node_array<double>& ycoord, 
                             int n)
{ list<node> L;
  triangulation_map(G,L,xcoord,ycoord,n); }

void random_planar_map(graph& G, 
                       node_array<double>& xcoord, 
                       node_array<double>& ycoord, 
                       int n, int m)
{ triangulation_map(G,xcoord,ycoord,n);

 list<edge> E;

 edge_array<bool> marked(G,false);
 edge e;
 forall_edges(e,G)
 { if (!marked[e]) E.append(e);
   marked[e] = true;
   marked[G.reversal(e)] = true;
  }
      

 E.permute();

 while (E.length() > m)
 { edge e = E.pop();
   edge r = G.reversal(e);
   G.del_edge(e);
   G.del_edge(r);
  }

}



void triangulation_graph(graph& G, 
                             list<node>& outer_face,
                             node_array<double>& xcoord, 
                             node_array<double>& ycoord, 
                             int n)
{ triangulation_map(G,outer_face,xcoord,ycoord,n);
  list<edge> E;

  edge_array<bool> marked(G,false);
  edge e;
  forall_edges(e,G)
  { if (!marked[e]) E.append(e);
    marked[e] = true;
    marked[G.reversal(e)] = true;
   }

  forall(e,E) G.del_edge(e);
}

void triangulation_graph(graph& G, 
                             node_array<double>& xcoord, 
                             node_array<double>& ycoord, 
                             int n)
{ list<node> outer_face;
  triangulation_graph(G,outer_face,xcoord,ycoord,n);
}
  




void triangulation_graph(graph& G, int n)
{ list<node> outer_face;
  node_array<double> xcoord;
  node_array<double> ycoord;
  triangulation_graph(G,outer_face,xcoord,ycoord,n);
}
  



void random_planar_graph(graph& G, 
                         node_array<double>& xcoord, 
                         node_array<double>& ycoord, 
                         int n, int m)
{
  random_planar_map(G,xcoord,ycoord,n,m);
  list<edge> E;

  edge_array<bool> marked(G,false);
  edge e;
  forall_edges(e,G)
  { if (!marked[e]) E.append(e);
    marked[e] = true;
    marked[G.reversal(e)] = true;
   }

  forall(e,E) G.del_edge(e);
}


void triangulated_planar_graph(graph& G, list<node>& outer_face,
                                         node_array<double>& xcoord,
                                         node_array<double>& ycoord, int n)
{ triangulation_graph(G,outer_face,xcoord,ycoord,n); }


void triangulated_planar_graph(graph& G, int m)
{ node_array<double> xcoord;
  node_array<double> ycoord;
  list<node> L;
  triangulated_planar_graph(G,L,xcoord,ycoord,m);
 }


static bool tutte_embed(const graph& G, const node_array<bool>& fixed,
                        node_array<double>& xpos, node_array<double>& ypos)
{ node v,w;
  edge e;

  list<node> other_nodes;
  forall_nodes(v,G) 
      if(!fixed[v]) other_nodes.append(v);

  node_array<int> ind(G);          // position of v in other_nodes and A

  int i = 0;
  forall(v,other_nodes) ind[v] = i++;

  int n = other_nodes.size();   // #other nodes
  vector coord(n);              // coordinates (first x then y)
  vector rhs(n);                // right hand side
  matrix A(n,n);                // equations

  // initialize non-zero entries in matrix A
  forall(v,other_nodes)
  {
    double one_over_d = 1.0/double(G.degree(v));
    forall_inout_edges(e,v)
    {
      // get second node of e
      w = (v == source(e)) ? target(e) : source(e);
      if(!fixed[w]) A(ind[v],ind[w]) = one_over_d;
    }
    A(ind[v],ind[v]) = -1;
  }

  if(!A.det()) return false;

  // compute right hand side for x coordinates
  forall(v,other_nodes)
  { rhs[ind[v]] = 0;
    double one_over_d = 1.0/double(G.degree(v));
    forall_inout_edges(e,v)
    { // get second node of e
      w = (v == source(e)) ? target(e) : source(e);
      if(fixed[w]) rhs[ind[v]] -= (one_over_d*xpos[w]);
    }
  }

  // compute x coordinates
  coord = A.solve(rhs);
  forall(v,other_nodes) xpos[v] = coord[ind[v]];

  // compute right hand side for y coordinates
  forall(v,other_nodes)
  { rhs[ind[v]] = 0;
    double one_over_d = 1.0/double(G.degree(v));
    forall_inout_edges(e,v)
    { // get second node of e
      w = (v == source(e)) ? target(e) : source(e);
      if(fixed[w]) rhs[ind[v]] -= (one_over_d*ypos[w]);
    }
  }

  // compute y coordinates
  coord = A.solve(rhs);
  forall(v,other_nodes) ypos[v] = coord[ind[v]];

  return true;
}


void triangulated_planar_graph(graph& G, node_array<double>& xcoord,
                                         node_array<double>& ycoord, int n)
{ list<node> L;
  triangulated_planar_graph(G,L,xcoord,ycoord,n);

  if (n > 128) return;

  node_array<bool> fixed(G,false);

  double step  = 6.2832/L.length();
  double alpha = 0;
  node v;
  forall(v,L)
  { xcoord[v] = cos(alpha);
    ycoord[v] = sin(alpha);
    alpha+=step;
    fixed[v] = true;
  }
 
  tutte_embed(G,fixed,xcoord,ycoord);
 }



void random_sp_graph(graph& G, int nodes, int edges)
{ spq_tree T;
  if (!random_sp_graph(G,T,nodes,edges))
    LEDA_EXCEPTION(1,"random_sp_graph: could not generate sp-graph");
}

LEDA_END_NAMESPACE
