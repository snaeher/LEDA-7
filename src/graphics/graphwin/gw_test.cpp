/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/graph_draw.h>

LEDA_BEGIN_NAMESPACE

void gw_test_bipart(GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  list<node> A;
  list<node> B;

  if ( ! Is_Bipartite(G,A,B) )
  { 
    gw.save_node_attributes();
    gw.save_edge_attributes();

    bool fl = gw.set_flush(false);

    node v;
    forall_nodes(v,G)
    { gw.set_color(v,grey1);
      gw.set_border_color(v,grey1);
      gw.set_label_color(v,grey2);
     }

    edge e;
    forall_edges(e,G)
    { gw.set_color(e,grey1);
      gw.set_width(e,1);
     }

    v = A.tail();
    int count = 0;
    node u;
    forall(u,A)
    { gw.set_color(v,yellow);
      gw.set_border_color(v,black);
      gw.set_label_color(v,black);
      gw.set_label(v,string("%d",++count));
      edge e = nil;
      forall_inout_edges(e,u)
          if (opposite(u,e) == v) break;
      gw.set_width(e,2);
      gw.set_direction(e,undirected_edge);
      gw.set_color(e,red);
      v = u;
     }
    gw.redraw();
    gw.set_flush(fl);

    gw.wait("\\bf This graph is \\red not bipartite\\black - \
             it contains an \\red Odd Length Circle\\black .");

    gw.restore_node_attributes();
    gw.restore_edge_attributes();
    return;
   }

  gw.message("\\bf This graph is \\blue bipartite\\black - \
              here is a possible \\blue Bipartite Partitioning\\blue .");

  node_array<point> pos(G);
  double x0 = gw.get_xmin();
  double y0 = gw.get_ymin();
  double x1 = gw.get_xmax();
  double y1 = gw.get_ymax();

  double dx = (x1 - x0)/6;
  double dy = (y1 - y0)/(A.length() +1); 
  double y = dy/2;

  node v;
  forall(v,A) 
  { pos[v] = point(x0+dx,y);
    y += dy;
   }

  dy = (y1 - y0)/(B.length() +1); 
  y = dy/2;
  forall(v,B) 
  { pos[v] = point(x1-dx,y);
    y += dy;
   }

  gw.save_node_attributes();
  gw.save_edge_attributes();

  bool fl = gw.set_flush(true);

  gw.set_layout(pos);

  gw.wait();
  gw.message("");

  gw.set_flush(false);
  gw.restore_node_attributes();
  gw.restore_edge_attributes();
  gw.redraw();
  gw.set_flush(fl);
}



void gw_test_con(GraphWin& gw)
{ graph& G = gw.get_graph();

  bool f = gw.get_flush();
  gw.save_node_attributes();
  gw.save_edge_attributes();

  if ( Is_Connected(G) )
  { 
    gw.message("\\bf This graph is \\blue connected\\black,\
                  I construct a spanning tree.");

    list<edge> T = SPANNING_TREE(G);

    gw.set_flush(false);

    edge e;
    forall_edges(e,G) 
    { gw.set_color(e,grey1);
      gw.set_width(e,1);
      G.hide_edge(e);
     }

    forall(e,T) 
    { G.restore_edge(e);
      gw.set_color(e,black);
      gw.set_direction(e,undirected_edge);
      gw.set_width(e,2);
     }



    double xleft  = gw.get_xmin();
    double xright = gw.get_xmax();
    double ybottom= gw.get_ymin();
    double ytop   = gw.get_ymax();
  
    node_array<double> xpos(G);
    node_array<double> ypos(G);
   
    node v;
    forall_nodes(v,G) {
      point p = gw.get_position(v);
      xpos[v] = p.xcoord();
      ypos[v] = p.ycoord();
    }

    SPRING_EMBEDDING(G,xpos,ypos,xleft,xright,ybottom,ytop,400);

    G.restore_all_edges();

    gw.set_flush(true);
    gw.adjust_coords_to_win(xpos,ypos);
    gw.set_layout(xpos,ypos);
    gw.wait();
    gw.message("");
   }
  else
  { node_array<int> compnum(G,0);
    int num = COMPONENTS(G,compnum);
    node v;
    forall_nodes(v,G)
    { //color col = color(2+compnum[v]%14);
      color col = color::get(2+compnum[v]%14);
      gw.set_color(v,col);
      edge e;
      forall_adj_edges(e,v) 
      { gw.set_color(e,col);
        gw.set_direction(e,undirected_edge);
       }
     }
    gw.wait(string("\\bf This graph has \\red %d\\black connected components.",num));
   }

  gw.set_flush(false);
  gw.restore_node_attributes();
  gw.restore_edge_attributes();
  gw.redraw();
  gw.set_flush(f);
}


void gw_test_bicon(GraphWin& gw)
{ node s;
  if ( Is_Biconnected(gw.get_graph(),s) )
  { gw.wait("\\bf This graph is \\blue biconnected\\black .");
    return;
   }

  if (s == nil) // not even connected
  { gw.wait("\\bf This graph is \\red not connected\black .");
    return;
   }

  bool fl = gw.set_flush(false);
  
  // high-light split pair
  gw.save_node_attributes();

  node v;
  forall_nodes(v,gw.get_graph()) 
  { if (v == s) 
      gw.set_color(v,red);
    else
      gw.set_color(v,grey1);
   }

  gw.redraw();
  gw.wait("\\bf This graph is \\red not biconnected\\black - \
                it contains a \\red cut vertex\\black .");

  gw.restore_node_attributes();
  gw.redraw();
  gw.set_flush(fl);
}


void gw_test_tricon(GraphWin& gw)
{ node s1,s2;

  graph& G = gw.get_graph();

  if ( Is_Triconnected(G,s1,s2) )
  { gw.wait("\\bf This graph is \\blue triconnected\\black .");
    return;
   }

  if (s1 == nil) // not connected at all
  { gw.wait("\\bf This graph is \\red not connected\\black .");
    return;
   }

  gw.update_edges();

  bool fl = gw.set_flush(false);
  
  // high-light split pair
  gw.save_node_attributes();
  gw.save_edge_attributes();

  node v;
  forall_nodes(v,G) 
  { if (v == s1 || v == s2) 
       gw.set_color(v,red); 
    else
       gw.set_color(v,grey1); 
   }

  edge e;
  forall_edges(e,G)
  { if ((source(e) == s1 && target(e) == s2) || 
        (source(e) == s2 && target(e) == s1))
    { gw.set_color(e,red);
      gw.set_width(e,2);
     }
   }
 
  gw.redraw();
  
  if (s1 != s2)
     gw.wait("\\bf This graph is \\red not triconnected\\black - \
                   it contains a \\red split pair\\black .");
  else
     gw.wait("\\bf This graph is \\red not triconnected\\black - \
                   it contains a \\red cut vertex\\black .");

  // restore colors
  gw.restore_node_attributes();
  gw.restore_edge_attributes();
  gw.redraw();
  gw.set_flush(fl);
}



void gw_test_planar(GraphWin& gw) 
{
  graph& G = gw.get_graph();

  if (G.number_of_nodes() < 3)
  { gw.wait("\\bf This graph is \\blue planar\\black .");
    return;
   }

  if (PLANAR(G)) 
  { 
    gw.message("\\bf This graph is \\blue planar\\black,\
                I construct a planar drawing.");

    edge e;
    forall_edges(e,G) 
         if (source(e) == target(e)) G.hide_edge(e);

    list<edge> dummy_edges;
    Make_Biconnected(G,dummy_edges);
    Make_Bidirected(G,dummy_edges);


    PLANAR(G,true);

    node_array<double> xcoord(G);
    node_array<double> ycoord(G);

    STRAIGHT_LINE_EMBEDDING(G,xcoord,ycoord);

    G.del_edges(dummy_edges);
    G.restore_all_edges();
    gw.update_edges();

    double xmax=0;
    double ymax=0;

    node v;
    forall_nodes(v,G)
    { if (xcoord[v] > xmax) xmax = xcoord[v];
      if (ycoord[v] > ymax) ymax = ycoord[v];
     }

    double dx,dy,fx,fy;
    double d = gw.get_window().pix_to_real(40);
    double wx0 = gw.get_xmin()+d;
    double wy0 = gw.get_ymin()+d;
    double wx1 = gw.get_xmax()-d;
    double wy1 = gw.get_ymax()-d; 
    gw.fill_win_params(wx0,wy0,wx1,wy1,0,0,xmax,ymax,dx,dy,fx,fy);

    forall_nodes(v,G)
    { xcoord[v] = dx +fx*xcoord[v];
      ycoord[v] = dy +fy*ycoord[v];
     }

    gw.save_node_attributes();
    gw.save_edge_attributes();

    bool fl = gw.set_flush(true);

    gw.set_layout();
    gw.set_layout(xcoord,ycoord);

    gw.set_flush(false);
    
    forall_nodes(v,G)
    { gw.set_label_type(v,no_label);
      gw.set_width(v,10);
      gw.set_height(v,10);
     }

    forall_edges(e,G)
      gw.set_direction(e,undirected_edge);

    gw.redraw();

    gw.wait();
    gw.message("");

    gw.set_flush(false);
    gw.restore_node_attributes();
    gw.restore_edge_attributes();
    gw.redraw();
    gw.set_flush(fl);

    return;
  }

  list<node> V;
  list<edge> E;
  node_array<int> kind(G);

  KURATOWSKI(G,V,E,kind);

  gw.save_node_attributes();
  gw.save_edge_attributes();

  bool fl = gw.set_flush(false);

  string kname = "K5";

  node v;
  forall_nodes(v,G) 
  { 
    switch (kind[v]) {

      case  0: // outside of Kuratowski graph
               gw.set_color(v,grey1);
               gw.set_border_color(v,grey3);
               gw.set_label_color(v,grey3);
               break;
        
      case  2: // subdivision point
               gw.set_color(v,ivory);
               gw.set_label_type(v,no_label);
               gw.set_shape(v,circle_node);
               gw.set_width(v,15);
               gw.set_height(v,15);
               break;

      case  3: // left side of K33
               gw.set_shape(v,circle_node);
               gw.set_label_type(v,no_label);
               gw.set_width(v,40);
               gw.set_height(v,40);
               gw.set_color(v,yellow);
               break;

      case -3: // right side of K33
               gw.set_shape(v,rectangle_node);
               //gw.set_shape(v,circle_node);
               gw.set_label_type(v,no_label);
               gw.set_width(v,40);
               gw.set_height(v,40);
               gw.set_color(v,orange);
               kname = "K33";
               break;

      case  4: // K5
               gw.set_shape(v,circle_node);
               gw.set_label_type(v,no_label);
               gw.set_width(v,40);
               gw.set_height(v,40);
               gw.set_color(v,red);
               break;
    }
  }


  edge e;
  forall_edges(e,G) 
  { gw.set_style(e,solid_edge);
    //gw.set_color(e,grey1);
    gw.set_color(e,grey2);
    gw.set_width(e,1);
   }
        
  forall(e,E)
  { gw.set_color(e,black);
    gw.set_direction(e,undirected_edge);
    gw.set_width(e,2);
    gw.set_source_anchor(e,point(0,0));
    gw.set_target_anchor(e,point(0,0));
   }

  if (gw.wait(string("\\bf This graph is \\red not planar\\black - it contains")
                + " a\\red " + kname + "\\black Kuratowski-Subdivision."))
  { gw.restore_node_attributes();
    gw.restore_edge_attributes();
   }

  gw.redraw();
  gw.set_flush(fl);
}


void gw_test_series_parallel(GraphWin& gw) 
{
  graph& G = gw.get_graph();

  if (G.number_of_nodes() < 3)
  { gw.wait("\\bf This graph is \\blue planar\\black .");
    return;
   }

  if (Is_Series_Parallel(G)) 
  { 
    gw.message("\\bf This graph is \\blue series-parallel\\black,\
                I construct a series-parallel drawing.");

    node_array<double> xpos(G);
    node_array<double> ypos(G);
    node_array<double> xrad(G);
    node_array<double> yrad(G);
    edge_array<list<double> > xbends(G);
    edge_array<list<double> > ybends(G);
    edge_array<double> xsan(G);
    edge_array<double> ysan(G);
    edge_array<double> xtan(G);
    edge_array<double> ytan(G);
  
    SP_EMBEDDING(G,xpos,ypos,xrad,yrad,xbends,ybends,xsan,ysan,xtan,ytan);

    double dx,dy,fx,fy;
    gw.fill_win_params(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);
    gw.transform_layout(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);

    gw.save_node_attributes();
    gw.save_edge_attributes();

    bool fl = gw.set_flush(true);

    gw.set_node_shape(rectangle_node);
    gw.set_node_label_type(no_label);
    gw.set_layout(xpos,ypos,xrad,yrad,xbends,ybends,xsan,ysan,xtan,ytan);

    gw.wait();
    gw.message("");

    gw.set_flush(false);
    gw.restore_node_attributes();
    gw.restore_edge_attributes();
    gw.redraw();
    gw.set_flush(fl);
  }
}

LEDA_END_NAMESPACE
