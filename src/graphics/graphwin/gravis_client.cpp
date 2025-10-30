/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gravis_client.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>
#include <stdio.h>
#include <stdlib.h>


LEDA_BEGIN_NAMESPACE

static string  gravis_module_path;

static void run_gravis_module(GraphWin& gw, string module) 
{ 
  if (gw.get_graph().number_of_nodes() < 2) return;

  module = gravis_module_path + "/" + module + ".mod";

  gw.message("\\bf Calling\\blue ~~" + module);

  string gml_file = tmp_file_name() + ".gml";
  string log_file = tmp_file_name();

  gw.save_gml(gml_file);

#if defined(__unix__)
  if (system(module + " " + gml_file + " >" + log_file + " 2>&1"))
#else
  if (system(module + " " + gml_file + " >" + log_file))
#endif
  { gw.message("");
    panel P;
    P.text_item("");
    P.text_item("\\bf Error in the execution of");
    P.text_item("");
    P.text_item("\\bf\\blue " + module);
    P.text_item("");
    P.button("continue",0);
    P.button("show log",1);
    if (gw.open_panel(P) == 1) LedaFileViewer(log_file);
    delete_file(gml_file);
    delete_file(log_file);
    return;
   }

  gw.message("\\bf\\blue Loading Layout ...");

/*
  //bool z = gw.set_zoom_objects(false);
  int stat = gw.read_gml(gml_file);
  //gw.set_zoom_objects(z);
  gw.redraw();
*/

  ifstream in(gml_file);
  int stat = gw.load_gml_layout(in);

  gw.message("");

  if (stat != 0) 
    gw.acknowledge("GML parse error", gml_file);

  delete_file(gml_file);
  delete_file(log_file);
}




static void call_module(GraphWin& gw)
{ window*    cw = window::get_call_window();
  panel_item ci = window::get_call_item();
  run_gravis_module(gw,cw->get_label(ci));
}


static void gravis_error(GraphWin& gw)
{
  panel P;
  P.text_item("");
  P.text_item("\\bf Could not find any GraVis modules.");
  P.text_item("");
  P.text_item("Either GraVis is not installed correctly on this system or the\
               environment variable \\bf GRAVIS\\rm is not set to the path\
               name of the GraVis root directory. For more information see");
  P.text_item("");
  P.text_item("\\blue http://www-pr.informatik.uni-tuebingen.de/GraVis");
  P.text_item("");
  P.button("continue",0);
  P.button("open url",1);
  if (gw.open_panel(P) == 1) 
        open_url("http://www-pr.informatik.uni-tuebingen.de/GraVis");
}



void GraphWin::init_gravis_menu(int menu_id)
{
   string gravis = "/usr/local/gravis";

   char* gravis_env = getenv("GRAVIS");
   if (gravis_env) gravis = gravis_env;

   gravis_module_path = gravis + "/lib/modules";

   list <string> alg_menu_list;

   if (is_directory(gravis_module_path))
     alg_menu_list = get_files(gravis_module_path);


   if (alg_menu_list.empty())
   { add_simple_call(gravis_error,"&GraVis",menu_id);
     return;
    }

   int gv_menu = add_menu("&GraVis Modules",menu_id);

   string s;
   forall(s,alg_menu_list) 
   { if (s.tail(4) != ".mod") continue;
     add_simple_call(call_module,s.replace(".mod",""),gv_menu);
    }
}

LEDA_END_NAMESPACE


