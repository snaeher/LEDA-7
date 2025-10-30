/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3_leda.c
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

#include <LEDA/graphics/pixmaps/algosol.xpm>

using namespace leda;


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

//int     N = 4;
//bool  ran = true;

 window W(700,600);

/*
 char* pmap = W.create_pixrect(algosol_xpm);
 W.set_bg_pixmap(pmap);
*/
 
 W.display(window::center,window::center);
 W.init(-160,160,-140);

 GRAPH<d3_point,int> poly;
 d3_window anim(W,poly);

 //anim.set_solid(false);
 anim.set_solid(true);
 anim.set_elim(true);
 anim.set_draw_edges(false);


 make_leda(poly);
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

 anim.set_center(0,0,0);
 anim.draw();

 while (anim.move() != MOUSE_BUTTON(3));

 return 0;
}
