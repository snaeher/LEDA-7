/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3_window.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/geo/d3_point.h>
#include <LEDA/graphics/d3_window.h>
#include <math.h>

using namespace leda;


void make_poly(GRAPH<d3_point,int>& poly, int N)
{ 
    poly.clear();
  
    node* L = new node[N];
    node* R = new node[N];
  
    double d = 2*LEDA_PI/N;

    poly.clear();
  
    int i;
    for(i=0; i<N; i++)
    { point origin(0,0);
      point p = origin.translate_by_angle(i*d,100);
      L[i] = poly.new_node(d3_point((int)p.xcoord(),(int)p.ycoord(), 120,1));
      R[i] = poly.new_node(d3_point((int)p.xcoord(),(int)p.ycoord(),-120,1));
     }

    node v0 = poly.new_node(d3_point(0,0,-30,1));
  
    for(i=1; i<N; i++)
    { poly.new_edge(L[i],L[i-1]);
      poly.new_edge(L[i-1],L[i]);
      poly.new_edge(R[i],R[i-1]);
      poly.new_edge(R[i-1],R[i]);
      poly.new_edge(L[i],R[i]);
      poly.new_edge(R[i],L[i]);
      poly.new_edge(v0,R[i]);
      poly.new_edge(R[i],v0);
     }
  
    poly.new_edge(L[0],L[N-1]);
    poly.new_edge(L[N-1],L[0]);
    poly.new_edge(R[0],R[N-1]);
    poly.new_edge(R[N-1],R[0]);
    poly.new_edge(L[0],R[0]);
    poly.new_edge(R[0],L[0]);
    poly.new_edge(v0,R[0]);
    poly.new_edge(R[0],v0);

    poly.make_map();

    //if (!PLANAR(poly,true)) error_handler(1,"graph not planar !");

/*
    // compute an interior point M and move the origin to this point
  
    d3_point M(3);
    integer mx = 0;
    integer my = 0;
    integer mz = 0;
  
    forall_nodes(v,poly)
    {  mx += poly[v][0];
       my += poly[v][1];
       mz += poly[v][2];
     }
  
  
    M[0] /= poly.number_of_nodes();
    M[1] /= poly.number_of_nodes();
    M[2] /= poly.number_of_nodes();
  
  
    forall_nodes(v,poly)
    { poly[v][0] -= M[0];
      poly[v][1] -= M[1];
      poly[v][2] -= M[2];
     }
*/
  
}



void make_sphere(GRAPH<d3_point,int>& G, int n)
{
  G.clear();

  int m = 2*n;

  node*  V = new node[m];

  node north = G.new_node(d3_point(0,0,1,1));

  int i;
  int j;
  double phi1 = 0;
  double phi2 = 0;
  double d1 = LEDA_PI/n;
  double d2 = LEDA_PI/n;

  for(i=0; i<m; i++) V[i] = north;

  for(phi1=d1, j=1; j<n; phi1+=d1, j++)
  { double z = cos(phi1);
    double r = sin(phi1);

    for(phi2=0, i=0; i<m; phi2+=d2, i++)
    {
      double x = r*cos(phi2);
      double y = r*sin(phi2);

      node v = G.new_node(d3_point(x,y,z,1));

      if(i==0) 
      { G.new_edge(v,V[i]);
        G.new_edge(V[i],v);
        if (j > 1)  
        { G.new_edge(V[0],V[m-1]);
          G.new_edge(V[m-1],V[0]);
         }
       }
      else
       { G.new_edge(v,V[i-1]);
         G.new_edge(V[i-1],v);
         G.new_edge(v,V[i]);
         G.new_edge(V[i],v);
        }
      V[i] = v;
     }

   }

  node south = G.new_node(d3_point(0,0,-1,1));

  G.new_edge(V[m-1],V[0]);
  G.new_edge(V[0],V[m-1]);

  for (i=0;i<m;i++) 
  { G.new_edge(south,V[i]);
    G.new_edge(V[i],south);
   }


  node v;

  forall_nodes(v,G) G[v] = 100*G[v].to_vector();

  G.make_map();
  //if (!PLANAR(G,true)) error_handler(1," G not planar !");

  delete[] V;
}


void leda_side(GRAPH<d3_point,int>& poly, node* L, float z)
{
  L[0]= poly.new_node(d3_point(-70,-20,z,1));
  L[1]= poly.new_node(d3_point(-40,-20,z,1));
  L[2]= poly.new_node(d3_point(-40,-10,z,1));
  L[3]= poly.new_node(d3_point(-60,-10,z,1));
  L[4]= poly.new_node(d3_point(-60,+20,z,1));
  L[5]= poly.new_node(d3_point(-70,+20,z,1));

  poly.new_edge(L[0],L[5],0);
  int i;
  for(i = 1; i<=5; i++) poly.new_edge(L[i], L[i-1], 0);

  L[6] = poly.new_node(d3_point(-30,-20,z,1));
  L[7] = poly.new_node(d3_point( -5,-20,z,1));
  L[8] = poly.new_node(d3_point( -5,-10,z,1));
  L[9] = poly.new_node(d3_point(-20,-10,z,1));
  L[10]= poly.new_node(d3_point(-20, -5,z,1));
  L[11]= poly.new_node(d3_point( -5, -5,z,1));
  L[12]= poly.new_node(d3_point( -5, +5,z,1));
  L[13]= poly.new_node(d3_point(-20, +5,z,1));
  L[14]= poly.new_node(d3_point(-20,+10,z,1));
  L[15]= poly.new_node(d3_point( -5,+10,z,1));
  L[16]= poly.new_node(d3_point( -5,+20,z,1));
  L[17]= poly.new_node(d3_point(-30,+20,z,1));

  poly.new_edge(L[6],L[17],0);
  for(i = 7; i<=17; i++) poly.new_edge(L[i], L[i-1], 0);

  L[18]= poly.new_node(d3_point( 5,-20,z,1));
  L[19]= poly.new_node(d3_point(20,-20,z,1));
  L[20]= poly.new_node(d3_point(35,-10,z,1));
  L[21]= poly.new_node(d3_point(35,+10,z,1));
  L[22]= poly.new_node(d3_point(20,+20,z,1));
  L[23]= poly.new_node(d3_point( 5,+20,z,1));
  L[24]= poly.new_node(d3_point( 5,-20,z,1));

  L[25]= poly.new_node(d3_point(15,-10,z,1));
  L[26]= poly.new_node(d3_point(15,+10,z,1));
  L[27]= poly.new_node(d3_point(20,+10,z,1));
  L[28]= poly.new_node(d3_point(25, +5,z,1));
  L[29]= poly.new_node(d3_point(25, -5,z,1));
  L[30]= poly.new_node(d3_point(20,-10,z,1));
  L[31]= poly.new_node(d3_point(15,-10,z,1));

  poly.new_edge(L[18],L[31],1);
  for(i = 19; i<=24; i++) poly.new_edge(L[i], L[i-1], 0);

  poly.new_edge(L[25],L[24],1);
  for(i = 26; i<=31; i++) poly.new_edge(L[i], L[i-1], 0);


  L[32]= poly.new_node(d3_point(55, 20,z,1));
  L[33]= poly.new_node(d3_point(40,-20,z,1));
  L[34]= poly.new_node(d3_point(50,-20,z,1));
  L[35]= poly.new_node(d3_point(55, -5,z,1));
  L[36]= poly.new_node(d3_point(65, -5,z,1));
  L[37]= poly.new_node(d3_point(70,-20,z,1));
  L[38]= poly.new_node(d3_point(80,-20,z,1));
  L[39]= poly.new_node(d3_point(65, 20,z,1));
  L[40]= poly.new_node(d3_point(55, 20,z,1));

  L[41]= poly.new_node(d3_point(60,15,z,1));
  L[42]= poly.new_node(d3_point(65, 0,z,1));
  L[43]= poly.new_node(d3_point(55, 0,z,1));
  L[44]= poly.new_node(d3_point(60,15,z,1));

  poly.new_edge(L[32],L[44],1);
  for(i = 33; i<=40; i++) poly.new_edge(L[i], L[i-1], 0);

  poly.new_edge(L[41],L[40],1);
  for(i = 42; i<=44; i++) poly.new_edge(L[i], L[i-1], 0);
}


void make_leda(GRAPH<d3_point,int>& poly)
{ 
  poly.clear();

  node L[45];
  node R[45];
  
  leda_side(poly,L,-5);
  leda_side(poly,R,+5);

  for(int i = 0; i<45; i++) poly.new_edge(L[i],R[i]);

  poly.make_bidirected();
  poly.make_map();

  node v;
  forall_nodes(v,poly) poly[v] = 2 * poly[v].to_vector();
}



int main()
{ 
 int     N = 4;

// bool  ran = true;

 //window W(540,580);
 window W(1000,1000);

 W.int_item("# vertices",N,3,16);
 W.button("poly1",0);
 W.button("poly2",1);
 W.button("leda", 2);
 W.button("quit", 3);
 
 W.display(window::center,window::center);
 W.init(-220,220,-220);
 

 GRAPH<d3_point,int> poly;
 d3_window anim(W,poly);
 //anim.set_solid(false);
 anim.set_solid(true);
 anim.set_elim(true);
 anim.set_speed(2);

 int but = 2;

 while (but != 3)
 { 
   if (but >= 0 && but <= 2)
   { switch (but) {
      case 0: make_sphere(poly,N);
              break;
      case 1: make_poly(poly,N);
             break;
      case 2: make_leda(poly);
             break;
      }
     node_array<vector> pos(poly);
     node v;
     forall_nodes(v,poly) pos[v] = poly[v].to_vector();
     anim.init(pos);

     edge e;
     forall_edges(e,poly) 
     { if (poly[e] == 0) continue;
       edge r = poly.reversal(e);
       anim.set_color(e,invisible);
       anim.set_color(r,invisible);
      }

     W.clear();
     anim.draw();
   }
   but = anim.move();
 }

 return 0;
}
