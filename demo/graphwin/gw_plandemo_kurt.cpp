/*******************************************************************************
+
+  LEDA 5.0.1  
+
+
+  gw_plandemo.c
+
+
+  Copyright (c) 1995-2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2005/04/14 10:44:27 $


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>

using namespace leda;



void highlight(graph& G, GraphWin& gw, list<node> V, list<edge> E, 
                             node_array<int>& kind)
{
  //const graph& G = gw.get_graph();

  bool flush0 = gw.set_flush(false);

  node v;
  forall_nodes(v,G) {
    switch (kind[v]) {

      case  0: gw.set_color(v,grey1);
               gw.set_border_color(v,grey1);
               gw.set_label_color(v,grey2);
               break;
        
      case  2: gw.set_color(v,grey1);
               gw.set_label_type(v,no_label);
               gw.set_width(v,8);
               gw.set_height(v,8);
               break;

      case  3:
      case  4: gw.set_shape(v,rectangle_node);
               gw.set_color(v,red);
               break;

      case -3: gw.set_shape(v,rectangle_node);
               gw.set_color(v,blue2);
               break;
    }
  }

  edge e;
  forall_edges(e,G) gw.set_color(e,grey1);
        
  forall(e,E)
  { gw.set_color(e,black);
    gw.set_width(e,3);
   }

 gw.redraw();
 gw.set_flush(flush0);
}

int n = 11; int m = 18; 


int main() 
{ graph G; 
  
  GraphWin gw(G, "Planarity Test Demo");

  gw.display(window::center,window::center);

  gw.set_animation_steps(400); 
 
  gw.set_edge_width(3);

  gw.set_edge_direction(undirected_edge);

  while (true) 
  { 
    //graph& G = gw.get_graph();
    random_simple_undirected_graph(G,n,m);
    node_array<double> xpos(G); 
    node_array<double> ypos(G); 
    gw.update_graph();
    array<node> V(0,10);
    node v; int i = 0;
    forall_nodes(v,G) { V[i] = v; i++; }
  
    for (i = 0; i < 11; i++) { xpos[V[i]] = i; ypos[V[i]] = i*i; }
    

    SPRING_EMBEDDING(G,xpos,ypos,0,100,0,100);
    gw.adjust_coords_to_win(xpos,ypos);
    gw.set_layout(xpos,ypos);
    //gw.redraw();
    
    string message;
    
    if (PLANAR(G))  message = "This graph is planar."; else message = "This graph is not planar.";
    gw.message("\\bf " + message);
    //gw.message("\\bf This graph is planar"); else gw.message("\\bf This graph is not planar");


    leda::wait(4);
    gw.wait("\\bf " + message + "\\bf Do you want to see a proof? If yes, click done.");

if (PLANAR(G))
{ if (G.number_of_nodes() < 3) continue;
      node_array<double> xcoord(G);
      node_array<double> ycoord(G);
      STRAIGHT_LINE_EMBEDDING(G,xcoord,ycoord);
      gw.adjust_coords_to_win(xcoord,ycoord);     // !!!
      gw.set_layout(xcoord,ycoord);
    }
    else 
    { list<node> V_k;
      list<edge> E_k;
      node_array<int> kind(G);
      KURATOWSKI(G,V_k,E_k,kind);
      
      //gw.save_all_attributes();
      highlight(G,gw,V_k,E_k,kind);
      //gw.restore_all_attributes();
    }
   leda::wait(1);
   gw.wait("Click done to continue.");

  }
  
  return 0;
}

