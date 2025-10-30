/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  graphwin.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>

using namespace leda;


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  int screen_w = window::screen_width();
  int screen_h = window::screen_height();
  int dpi = window::dpi();
  
  cout << screen_w << " x " << screen_h << "   " << dpi << " dpi" << endl;

  string fname = "undefined";
  
  if (argc > 1) {
    fname = argv[1];
    fname = fname.replace_all("\"","");
    fname = fname.replace_all("\\","/");
  }

  // start in "Graphs" subdirectory (if it exist)
  if (is_directory("Graphs")) set_directory("Graphs");


/*
  string frame_label("Leda Graph Editor (GraphWin %.1f)",
                                              GraphWin::version());
*/

  string frame_label("LEDA  GraphWin %.1f", GraphWin::version());

  int w = window::default_width();
  int h = window::default_height();

/*
  if (getenv("LEDA_OPEN_MAXIMIZED")) 
  { w = window::screen_width();
    h = window::screen_height();
  }
*/

  GraphWin gw(w,h,frame_label);

  window& W = gw.get_window();

  gw.set_agd_host("");

//gw.set_node_shape(rectangle_node);
//gw.set_edge_direction(undirected_edge);

  gw.display();

  frame_label += string("   %d x %d   %d dpi",W.width(),W.height(),dpi);

  W.set_frame_label(frame_label);

  W.set_icon_label("GraphWin");

  W.set_cursor(XC_watch);

  string home_dir = get_home_directory();
  home_dir = home_dir.replace_all("\\","/");

  string rc_file = home_dir + "/.graphwinrc";

  if (is_file(rc_file))
   { gw.message("Loading Settings from\\blue " + rc_file);
     leda_wait(2.0);
     gw.read_defaults();
    }
/*
 else
   { gw.message("No\\blue " + rc_file + "\\black ~(using built-in defaults).");
     leda_wait(1.8);
    }
*/

  gw.message("");

  W.set_cursor();

  if (is_file(fname))
  {
    gw.message("Reading " + fname);
    leda_wait(1.0);
    gw.load_graph(fname);
    
/*
    string gname;
    if (fname.tail(4) == ".gml")
      { gname = fname.del(".gml");
        int err = gw.read_gml(fname);
        if (err) 
          gw.acknowledge(string("Error (%d) Reading GML-File ",err),fname);
        gw.set_graphname(gname);
        gw.zoom_graph();
       }
    else
      if (fname.tail(7) == ".dimacs")
        { gname = fname.del(".dimacs");
          int err = gw.read_dimacs(fname);
          if (err) 
            gw.acknowledge(string("Error (%d) Reading Dimacs File ",err),fname);
          gw.set_graphname(gname);
          gw.zoom_graph();
         }
      else
        { 
          gname = fname.del(".gw");
          int err = gw.read_gw(fname);
          if (err && err < 5) 
            gw.acknowledge(string("Error (%d) Reading GW-File ",err), fname);
          gw.set_graphname(gname);
         }
*/

  }


  int but = 0;
  int settings_choice = 0;
  bool do_save = false;


  while (but == 0)
  {
    gw.edit();

    panel P;
    P.text_item("\\bf\\blue Exit GraphWin");
    P.text_item("");

    P.choice_item("Local Settings",settings_choice,"save","discard","reset");

    if (gw.unsaved_changes())
    { P.text_item("\\bf Graph not written since last change.\\n");
      P.bool_item("Save Graph",do_save);
     }

    P.text_item("");
    P.button("cancel",0);
    P.fbutton("ok",1);

    //but = P.open();
    but = gw.open_panel(P);
   }

   if (do_save) gw_save_handler(gw);

   if (settings_choice == 0) gw.save_defaults();
   if (settings_choice == 2) gw.reset_defaults();

  if (fname == "CURRENT_GRAPH.gw") delete_file(fname);

  return 0;
}

