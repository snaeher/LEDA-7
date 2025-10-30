/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_heapsort_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/anim/TextLoader.h>

#include <stdlib.h>

using namespace leda;

using std::ifstream;

#include "gw_hs_observer.h"
#include "ViewVariable.c"
#include "AnimationTree.c"
#include "Heapsort.c"
#include "gw_hs_observer.c"



//  general adjustments  
//
static int ArraySize      = 15;                
static int max_ArraySize  = 15;         
static int PrevSortMode   = 2;              
static array<node> A_node(1, max_ArraySize);
static array<node> A_index(1, max_ArraySize);
static int *A_int;


static string engl = "doc/heapsort/engl/";
static string deut = "doc/heapsort/deut/";
static string LabelPath = "win_and_panel_labels/";
static string LanguagePath = deut;             

static string ButtonPath = "doc/heapsort/buttons/";

                     
static GRAPH<int, int> G;
static int grid_dist = 10;


static void DVI_Print_panel(window& W)
{ 
  string xdvi_cmd = "xdvi";     
  string lpr_cmd  = "dvips -q -o !'lpr -h'";
  string fn       = "DVI_Print_panel.labels";
  
  ifstream in(LanguagePath + LabelPath + fn);
  if(!in) 
  { 
    error_handler(0, "Can't open file : " + fn + " !"); 
    return; 
  }

  string lpr_stg    = read_line(in);
  string xdvi_stg   = read_line(in);
  string view_stg   = read_line(in);        
  string print_stg  = read_line(in);
  string cancel_stg = read_line(in);

  panel DVI_Print;
  DVI_Print.text_item("");
  DVI_Print.string_item(lpr_stg, lpr_cmd);
  DVI_Print.string_item(xdvi_stg, xdvi_cmd);
  DVI_Print.text_item("");
  DVI_Print.button(view_stg, 1);
  DVI_Print.button(print_stg, 2);
  DVI_Print.button(cancel_stg, 0);
  
  int but;

  while((but = DVI_Print.open(W, window::center, window::center)) != 0)
    switch (but) 
    {
      case 1: { // start dvi 
                set_directory(LanguagePath + "elaboration/");
                if(is_file("Heapsort.dvi"))
                {
                  system(xdvi_cmd + " Heapsort.dvi &");  
                  set_directory("../../..");
                }
                else 
                {
                  if(LanguagePath == deut)
                    error_handler(0, "Datei Heapsort.dvi kann nicht gefunden werden !"); 
                  else
                    error_handler(0, "File Heapsort.dvi not found !");
                  set_directory("../../..");
                }
                break; 
              }
      case 2: { // start lp daemon
                set_directory(LanguagePath + "elaboration/");
                if(is_file("Heapsort.dvi"))
                {
                  system(lpr_cmd + " Heapsort"); 
                  set_directory("../../..");
                }
                else 
                {
                  if(LanguagePath == deut)
                    error_handler(0, "Datei Heapsort.dvi kann nicht gefunden werden !"); 
                  else
                    error_handler(0, "File Heapsort.dvi not found !");
                  set_directory("../../..");
                }
  						  break;  
              }
    }
}


static void zoom_to_intervall(GraphWin& gw, node w, node v)
{
  double delta_y = gw.get_ymax() - gw.get_ymin();
  double delta_x = gw.get_xmax() - gw.get_xmin();
  
  double f = delta_y / delta_x;

  point p = gw.get_position(w);
  point q = gw.get_position(v);

  double r = gw.get_node_radius1();

  double x0 = p.xcoord() - 2 * r;
  double x1 = q.xcoord() + 2 * r;

  while (x1 - x0 < 26 * r)  
  { 
    x0 -= r;  
    x1 += r; 
  }

  double dy = f * (x1 - x0) / 2;
  double y0 = p.ycoord() - dy;
  double y1 = q.ycoord() + dy;

  gw.zoom_area(x0, y0, x1, y1);
}


// draw arrays 
//
static void redraw(GraphWin& gw)
{  
  gw.set_flush(false);
  gw.update_graph();

  int node_size  = gw.get_node_height();
    
  double xcenter = (gw.get_xmax() - gw.get_xmin()) / 2; 
  double ycenter = (gw.get_ymax() - gw.get_ymin()) / 2;
  double start_x = xcenter - (ArraySize / 2) * node_size;

  gw.set_node_label_font(fixed_font, 3 * node_size / 9);
  
  for(int i = 0; i < ArraySize; i++) 
  { gw.set_label_color(A_index[i+1], grey2);
    gw.set_color(A_index[i+1], invisible);
    gw.set_border_width(A_index[i+1], 0);
    gw.set_position(A_index[i+1],point(start_x + 2*i*grid_dist, ycenter - 5));  
  }

  for(int i = 0; i < ArraySize; i++) {
    gw.set_position(A_node[i+1], point(start_x + 2 * i * grid_dist, 
                                       ycenter - 2 * grid_dist)); 
  }
  
  gw.set_flush(true);   
  gw.set_animation_steps(25);
  zoom_to_intervall(gw, A_node[1], A_node[ArraySize]);                      
}

//  init arrays
//
static void InitArrays(GraphWin& gw)
{  
  if (A_int) delete A_int;
  
  A_int = new int [ArraySize+1];  

  G.empty();               
  gw.clear_graph();
  
  switch (PrevSortMode) 
  {
    case 0: { // rise array
              for(int i=1; i <= ArraySize; i++) 
                A_node[i] = G.new_node(i);
              break; 
            }
    case 1: { // descend array
              for(int i=1; i <= ArraySize; i++)
                A_node[i] = G.new_node(ArraySize-i + 1); 
              break; 
            }
    case 2: { // randomize array
              for(int i=1; i <= ArraySize; i++)
                A_node[i] = G.new_node(rand_int(1, ArraySize)); 
              break; 
            }
    case 3: { // constant array
              for(int i=1; i <= ArraySize; i++)
                A_node[i] = G.new_node(4); 
              break; 
            }
  }

  for(int i = 1; i <= ArraySize; i++) 
  { A_index[i] = G.new_node(i); 
    A_int[i] = G[A_node[i]];
   }

  redraw(gw);
}


// setup panel
//
static void Setup_panel(GraphWin &gw)
{
  string fn = "Setup_panel.labels";
  
  ifstream in(LanguagePath + LabelPath + fn);
  if(!in) 
  { 
    error_handler(0, "Can't open file : " + fn + " !"); 
    return; 
  }

  string ArraySize_stg  = read_line(in);
  string Property_stg   = read_line(in);
  string Language_stg   = read_line(in);        
  string to_rise_stg    = read_line(in);        
  string to_descend_stg = read_line(in);        
  string randomize_stg  = read_line(in);        
  string constant_stg   = read_line(in);        
  string cancel_stg     = read_line(in);        

  int language = 0;
  if (LanguagePath == engl) 
    language = 1;

  panel setup_panel;  
  setup_panel.text_item("");
  setup_panel.int_item(ArraySize_stg, ArraySize, 3, max_ArraySize);
  setup_panel.choice_item(Property_stg, PrevSortMode, to_rise_stg, to_descend_stg, randomize_stg, constant_stg);
  setup_panel.text_item("");
  setup_panel.choice_item(Language_stg, language, " deutsch ", " english ");
  setup_panel.button("Ok", 0);
  setup_panel.button(cancel_stg, 1);
  int but;
  while((but = setup_panel.open(gw.get_window(), window::center, window::center)) != 0 && but != 1);

  if(but == 1) return;
  
  if (language == 1) 
    LanguagePath = engl;
  else 
    LanguagePath = deut;  
}

void InitLabels(window& W, panel& main_panel)
{
  string fn = "W.frame_label";
  
  ifstream in_frame_label(LanguagePath + LabelPath + fn);
  if(!in_frame_label) 
    error_handler(0, "Can't open file : " + fn + " !"); 
  else
  {
    string GraphWindow_label_stg   = read_line(in_frame_label);
    W.set_frame_label(GraphWindow_label_stg);
  }
  
  fn = "Main_panel.help_stg_labels";
  
  ifstream in_main_panel(LanguagePath + LabelPath + fn);
  if(!in_main_panel) 
    error_handler(0, "Can't open file : " + fn + " !"); 
  else
  {
    string run_help_stg         = read_line(in_main_panel);
    string adjustments_help_stg = read_line(in_main_panel);
    string over_help_stg        = read_line(in_main_panel);
    string dvi_print_help_stg   = read_line(in_main_panel);
    string exit_help_stg        = read_line(in_main_panel);

    main_panel.set_button_help_str(3, run_help_stg);
    main_panel.set_button_help_str(1, adjustments_help_stg);            
    main_panel.set_button_help_str(2, over_help_stg);
    main_panel.set_button_help_str(4, dvi_print_help_stg);
    main_panel.set_button_help_str(0, exit_help_stg);
  }
}


static void func(GraphWin& gw, int but) 
{
  window& W = gw.get_window();

    switch(but) 
    {
      case 0: { // exit
                exit_handler(gw);
                break; 
              }

      case 1: { // setup
                Setup_panel(gw); 
                InitArrays(gw);
                //InitLabels(W, main_panel);
                break; 
              }

      case 2: { // about
                TextLoader W_about(310, 110);
                W_about.extract_frame_label(LanguagePath + LabelPath +  
                                            "W_about.frame_label");
                W_about.load_text(LanguagePath + "about.txt");
                W_about.open_with_timeout(window::center, window::center, 5000);
                break; 
              }

      case 3: { // start animation
                gw_hs_observer observer(HEAPSORT, gw, A_node, A_int, 
                                        LanguagePath, ArraySize); 
                HEAPSORT(A_int, ArraySize);         
            
                gw.set_flush(false);
                for(int i = A_node.low(); i <= ArraySize; i++) 
                { A_int[i] = G[A_node[i]];
                  gw.set_color(A_node[i], ivory);
                }
                gw.set_flush(true);
                gw.redraw();
                break;
              }           

      case 4: { 
                DVI_Print_panel(W); 
                break; 
              }

     }
}

static void func0(GraphWin& gw) { func(gw,0); } 
static void func1(GraphWin& gw) { func(gw,1); } 
static void func2(GraphWin& gw) { func(gw,2); } 
static void func3(GraphWin& gw) { func(gw,3); } 
static void func4(GraphWin& gw) { func(gw,4); } 



int main()
{ 
  if (!is_directory(("doc/heapsort")))
  { string lroot;
    if (get_environment("LEDAROOT",lroot))
    { engl = lroot + "/demo/animation/" + engl;
      deut = lroot + "/demo/animation/" + deut;
      ButtonPath = lroot + "/demo/animation/" + ButtonPath;
     }
   }
   
  LanguagePath = deut;             

  GraphWin gw(G, 1200, 700, "Array View");  
  window& W = gw.get_window();
   
  gw.set_default_menu(0);
  gw.set_show_status(false);
  gw.set_grid_dist(grid_dist);
  gw.set_grid_mode(0);  

/*
  gw.set_bg_xpm(slate_xpm);
*/
  
  gw.set_node_label_type(data_label);
  gw.set_node_shape(square_node);
  gw.set_node_radius1(0.85 * grid_dist);  
  gw.set_node_label_font(fixed_font, 12);
  gw.set_zoom_labels(false);

  gw.add_simple_call(func3,"run",0);
  gw.add_simple_call(func1,"setup",0);
  gw.add_simple_call(func2,"info",0);
  gw.add_simple_call(func4,"dvi",0);
  gw.add_simple_call(func0,"exit",0);

  gw.display(700,500);

  InitArrays(gw);

  gw.edit();

  delete [] A_int;

  return 0;     
}
