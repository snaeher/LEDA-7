/*******************************************************************************
+
+  LEDA 6.6.1  
+
+
+  graphwin.c
+
+
+  Copyright (c) 1995-2020
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


  string frame_label("Leda Graph Editor (GraphWin %.1f)",
                                              GraphWin::version());

  GraphWin gw(frame_label);

  window& W = gw.get_window();

//gw.set_node_shape(rectangle_node);
  gw.set_agd_host("");

  gw.display();

  W.set_icon_label("GraphWin");

  W.set_cursor(XC_watch);

  string home_dir = get_home_directory();

  home_dir = home_dir.replace_all("\\","/");

  string rc_file = home_dir + "/.graphwinrc";

  if (is_file(rc_file))
   { gw.message("Loading Options from\\blue " + rc_file);
     leda_wait(1.2);
     gw.read_defaults();
    }
/*
  else
   { gw.message("No\\blue " + rc_file + "\\black ~(using built-in defaults).");

     leda_wait(1.8);
    }
*/

  gw.message("");


  if (argc > 1) 
  { string fname = argv[1];

    // remove parantheses
    fname = fname.replace_all("\"","");
    fname = fname.replace_all("\\","/");

    gw.message("Reading " + fname);
    leda_wait(2.0);
    
    string gname;
    if (fname.tail(4) == ".gml")
      { gname = fname.del(".gml");
        int err = gw.read_gml(fname);
        if (err) 
          gw.acknowledge(string("Error (%d) Reading GML-File ",err),fname);
       }
    else
      if (fname.tail(7) == ".dimacs")
        { gname = fname.del(".dimacs");
          int err = gw.read_dimacs(fname);
          if (err) 
            gw.acknowledge(string("Error (%d) Reading Dimacs File ",err),fname);
         }
      else
        { gname = fname.del(".gw");
          int err = gw.read_gw(fname);
          if (err && err < 5) 
            gw.acknowledge(string("Error (%d) Reading GW-File ",err), fname);
         }

    gw.set_graphname(gname);
   }


  int but = 0;
  int def_choice = 0;
  bool do_save = false;

  W.set_cursor();

  while (but == 0)
  {
    gw.edit();

    panel P;
    //P.text_item("");
    P.text_item("\\bf\\blue Exit GraphWin");
    P.choice_item("Settings",def_choice,"keep","write","reset");

    if (gw.unsaved_changes())
    { P.text_item("\\bf Graph not written since last change.\\n");
      //P.button("save",3);
      //P.button("save gml",4);
      P.bool_item("Save Graph",do_save);
     }

    P.button("cancel",  0);
    P.button("ok",1);

    but = gw.open_panel(P);

/*
    if (but == 3) gw_save_handler(gw);
    if (but == 4) gw_save_gml_handler(gw);
*/

   }

   if (do_save) gw_save_handler(gw);

   if (def_choice == 1) gw.save_defaults();
   if (def_choice == 2) gw.reset_defaults();

  return 0;
}

