/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_explore.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>

/*
#include <LEDA/pixmaps/texture/wall.xpm>
#include <LEDA/pixmaps/algosol.xpm>
*/

using namespace leda;



node construct_tree(GraphWin& gw, string s, double x,  double& y, 
                                            double dx, double dy, node last)
{ //graph&  G = gw.get_graph();
   window& W = gw.get_window();

   node w = nil;

   node u = gw.new_node(point(x,y));
   gw.set_label(u,s);

   node u0 = gw.new_node(point(x+gw.get_radius1(u),y));

   gw.new_edge(u,u0);

   gw.set_width(u0,1);
   gw.set_height(u0,1);
   gw.set_shape(u0,circle_node);
   gw.set_color(u0,invisible);
   gw.set_border_color(u0,invisible);
   gw.set_label_pos(u0,east_pos);
   gw.set_label_color(u0,blue);
   //gw.set_label_color(u0,ivory);

   if (is_link(s)) gw.set_label(u0,"-->");

   if (last)
   { w = gw.new_node(point(x-dx,y));
     gw.set_label(w,"-");
     gw.set_width(w,8);
     gw.set_height(w,8);
     gw.set_shape(w,circle_node);
     //gw.set_width(w,10);
     //gw.set_height(w,10);
     //gw.set_shape(w,rhombus_node);
     gw.new_edge(last,w);
     gw.new_edge(w,u);
    }


   double ymin = gw.get_ymin() + W.pix_to_real(16);

   if (y < ymin + dy)
      gw.zoom_area(0,gw.get_ymin()-dy,500,gw.get_ymax()-dy);
   else
      gw.redraw();

   if (is_directory(s) && !is_link(s))
   { 
     set_directory(s);
     list<string> all_files  = get_entries(".");

     all_files.sort();

     string fname;
     int count = 0;

     forall(fname,all_files)
     { if (fname[0] == '.' || is_directory(fname)) continue;
       gw.set_label(u0," " + fname);
       gw.redraw(gw.get_xmin(),y-dy,gw.get_xmax(),y+dy);
       count++;
      }

     gw.set_label(u0,string(" (%d)",count));
     gw.redraw(gw.get_xmin(),y-dy,gw.get_xmax(),y+dy);


     forall(fname,all_files)
     { if (fname[0] == '.' || !is_directory(fname)) continue;
       y -= dy;
       u = construct_tree(gw,fname,x+dx,y,dx,dy,u);
      }

     set_directory("..");
    }

  return w;
}



#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

   string root; 

   if (argc > 1) 
     root = argv[1];
   else
     { root = "/LEDA/SRC";
       if (!is_directory(root)) root = ".";
      }

   GraphWin gw(600,600,"Explore " + root);

   gw.win_init(0,800,0);

   gw.set_show_status(false);
   gw.set_node_label_font(roman_font,12);
   gw.set_node_label_type(data_label);
   gw.set_node_shape(ovalrect_node);
   gw.set_edge_direction(undirected_edge);
   gw.set_grid_style(invisible_grid);
   gw.set_zoom_hide_labels(false);

   gw.set_node_radius1(50);
   gw.set_node_radius2(10);

   //gw.set_edge_color(grey1);

   //gw.set_bg_xpm(wall_xpm);
   //gw.set_bg_xpm(algosol_xpm);

   gw.display(window::center,window::center);
   gw.set_flush(false);

   if (!is_directory(root))
     error_handler(1,root + ": no such directory.");

   set_directory(root);
   root = get_directory();

   double y = gw.get_ymax() - 100;
   double x = 100;
   construct_tree(gw,root,x,y,100,30,nil);

   gw.set_zoom_hide_labels(true);
   gw.edit();

   return 0;
}

