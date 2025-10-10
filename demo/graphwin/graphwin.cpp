/*******************************************************************************
+
+  LEDA 7.2  
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

#include <stdlib.h>

using namespace leda;


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  string frame_label("Leda Graph Editor  (GraphWin %.1f)",
                                               GraphWin::version());

  GraphWin gw(frame_label);

  //gw.set_agd_host("");

  window& W = gw.get_window();

gw.set_node_shape(square_node);


  gw.display(400,window::center);

  W.set_icon_label("GraphWin");

  W.set_cursor(XC_watch);

  string rc_file;

  char* home = getenv("HOME");

  if (home) rc_file = string(home).replace_all("\\","/") + "/";

  rc_file += ".graphwinrc";


  if (is_file(rc_file))
   { gw.message("Loading Options from\\blue " + rc_file);
     leda_wait(1.2);
     gw.read_defaults();
    }
  else
   { gw.message("No\\blue " + rc_file + "\\black ~(using built-in defaults).");

     leda_wait(1.8);
    }

  gw.message("");


  if (argc > 1) 
  { string fname = argv[1];
    gw.set_graphname(fname.del(".gw"));
    if (fname.tail(4) == ".gml")
      { int err = gw.read_gml(fname);
        if (err) gw.acknowledge("Error Reading GML-File", fname);
       }
    else
      { int err = gw.read_gw(fname);
        if (err && err < 5) gw.acknowledge("Error Reading GW-File", fname);
       }
   }


  int but = 0;

  //bool save_defaults = false;

  int def_choice = 0;

  W.set_cursor();

  while (but == 0)
  {
    gw.edit();

    panel P;
    P.text_item("");
    P.text_item("\\bf\\blue Exit GraphWin");

    if (gw.unsaved_changes())
    { P.text_item("");
      P.text_item("\\bf Graph not written since last change !\\n");
     }

    P.choice_item("Options (\~/.graphwinrc)",def_choice,"keep","write","reset");

    if (gw.unsaved_changes()) 
    { P.button("save gw",3);
      P.button("save gml",4);
     }
    P.button("cancel",  0);
    P.button("exit",1);
    P.buttons_per_line(4);

    but = gw.open_panel(P);

    if (but == 3) gw_save_handler(gw);
/*
    if (but == 4) gw_save_gml_handler(gw);
*/

    if (but != 1 && gw.unsaved_changes()) but = 0;
   }

   if (def_choice != 0) 
   { if (def_choice == 2) gw.reset_defaults();
     gw.save_defaults();
    }

  return 0;
}

