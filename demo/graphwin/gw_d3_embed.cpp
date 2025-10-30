/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_d3_embed.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_draw.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/d3_window.h>

using namespace leda;


static string about_txt = "\
  \\bf 3d Spring Embedding \\tf\\c4\
  This program demonstrates 3d Spring Embedding of graph. Construct\
  a graph and press the \\blue done \\black button. A 3-dimensional\
  spring embedding will be shown in a second window. Moving the mouse\
  pointer around in this window will rotate the picture. Hold down the\
  \\blue left\\black or\\blue middle\\black button for zooming up or down.\
  Click the\\blue right\\black button to return to GraphWin. \\n\
  See the \\red File - LoadGraph - filename \\black menue for a list\
  of nice 3D graph.";


static string intro_txt = about_txt + 
               "\\8 \\bf Move the mouse pointer to the window on the left.\
                \\4 \\bf Click the right button to return to GraphWin.";


void about(GraphWin& GW) 
{ panel P("About D3 Spring Embedding");
  P.text_item(about_txt);
  P.button("ok");
  GW.open_panel(P); 
}


int main()
{ 

 GraphWin gw;

 gw.display();

 int h_menu = gw.get_menu("Help");
 gw.add_simple_call(about,"About D3 Spring Embed",h_menu);

 gw.set_dirname("Graphs");
 gw.set_graphname("wheel");
 gw.read("Graphs/wheel.gw");

 double x0 = gw.get_xmin();
 double y0 = gw.get_ymin();
 double x1 = gw.get_xmax();
 double y1 = gw.get_ymax();

 double d = (x1-x0)/16;

 gw.place_into_box(x0+2*d,y0+d,x1-2*d,y1-5*d);

 int xpos = gw.get_window().xpos()-8;

 int wi = 450;

 window W(wi,500, "D3 Window");

 W.set_bg_color(grey1);
 W.display(-xpos,0);
 W.init(-180,180,-180);

 int count = 0;

 do { gw.message(intro_txt);

      graph& G = gw.get_graph();

      node_array<double> xpos(G,0);
      node_array<double> ypos(G,0);
      node_array<double> zpos(G,0);
     
      node v;
      edge e;
     
      forall_nodes(v,G)
      { xpos[v] = rand_int(-100,100)/100.0;
        ypos[v] = rand_int(-100,100)/100.0;
        zpos[v] = rand_int(-100,100)/100.0;
       }
     
      D3_SPRING_EMBEDDING(G,xpos,ypos,zpos,-200,200,-200,200,-200,200,250);
     
      double X = 0;
      double Y = 0;
      double Z = 0;
     
      forall_nodes(v,G)
      { X += xpos[v];
        Y += ypos[v];
        Z += zpos[v];
       }
      
      int n = G.number_of_nodes();
     
      node_array<vector> pos(G);
     
      forall_nodes(v,G)
         pos[v] = vector(xpos[v]-X/n,ypos[v]-Y/n,zpos[v]-Z/n);
     

      d3_window d3_win(W,G,pos);
      d3_win.set_draw_graph(true);
      d3_win.set_speed(16);
     
      forall_nodes(v,G) d3_win.set_color(v,gw.get_color(v));
      forall_edges(e,G) d3_win.set_color(e,gw.get_color(e));
     
      d3_win.set_message("\\bf\\blue left:  \\black zoom up   ~~~~~\
                          \\bf\\blue middle:\\black zoom down ~~~~~\
                          \\bf\\blue right: \\black return to GW");
      d3_win.draw();
     
      int but = 0;
     
      while (but != MOUSE_BUTTON(3))
      { but = d3_win.move();
        //D3_SPRING_EMBEDDING(G,xpos,ypos,zpos,-200,200,-200,200,-200,200,5);
        //forall_nodes(v,G)
        //{ G[v] = d3_point(xpos[v],ypos[v],zpos[v]);
        //  d3_win.set_position(v,xpos[v],ypos[v],zpos[v]);
        // }
       }
     
       d3_win.set_message("");
       d3_win.draw();
     
       gw.message("");
     
       if (count++ == 0) gw.fill_window();
     
       gw.message("Edit the graph and press \\bf done\
                   \\rm to restart the 3d-animation.");
     
       intro_txt = "";
     
  } while (gw.edit());

 return 0;
}
