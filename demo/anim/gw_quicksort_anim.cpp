/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_quicksort_anim.c
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


#include "gw_qs_observer.h"
#include "test_qs_observer.h"
#include "Qsort.c"
#include "ViewVariable.c"
#include "gw_qs_observer.c"



//  general adjustments  
//
static int ArraySize      = 15;                
static int max_ArraySize  = 15;         
static int PrevSortMode   = 2;              
static array<node> A_node(max_ArraySize);
static array<node> A_index(max_ArraySize);
static int *A_int;

static string LabelPath = "win_and_panel_labels/";

static string engl = "doc/quicksort/engl/";
static string deut = "doc/quicksort/deut/";

static string ButtonPath = "doc/quicksort/buttons/";             

static string LanguagePath = engl;             
                     

static GRAPH<int, int> G;
static int grid_dist = 10;

//  standard quicksort 
//
static void qsort(int A[], int l, int r)
{
  if (l >= r) return;     
  int i = l;             
  int j = r+1;               
  int v = A[l]; 
  for(;;)  {
    do { i++; } while(A[i] < v && i < r);     
    do { j--; } while(A[j] > v );  
    if(i >= j) break;     
    int t = A[i];
    A[i] = A[j];
    A[j] = t;
  }                        
  int t = A[l];
  A[l] = A[j];
  A[j] = t;
  qsort(A, l, j-1);
  qsort(A, j+1, r); 
}


static void DVI_Print_panel(window& gw)
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

  while((but = DVI_Print.open(gw, window::center, window::center)) != 0)
    switch (but) 
    {
      case 1: { // start dvi 
                set_directory(LanguagePath + "elaboration/");
                if(is_file("Quicksort.dvi"))
                {
                  system(xdvi_cmd + " Quicksort.dvi &");  
                  set_directory("../../..");
                }
                else 
                {
                  if(LanguagePath == deut)
                    error_handler(0, "Datei Quicksort.dvi kann nicht gefunden werden !"); 
                  else
                    error_handler(0, "File Quicksort.dvi not found !");
                  set_directory("../../..");
                }
                break; 
              }
      case 2: { // start lp daemon
                set_directory(LanguagePath + "elaboration/");
                if(is_file("Quicksort.dvi"))
                {
                  system(lpr_cmd + " Quicksort"); 
                  set_directory("../../..");
                }
                else 
                {
                  if(LanguagePath == deut)
                    error_handler(0, "Datei Quicksort.dvi kann nicht gefunden werden !"); 
                  else
                    error_handler(0, "File Quicksort.dvi not found !");
                  set_directory("../../..");
                }
                break;  
              }
    }
}


// Compare Algorithms : standard quicksort (SQ) vs. quicksort with events (EQ)
//
static void CompareAlgorithms_panel(window& gw)
{
  string fn = "CompareAlgorithms_panel.labels";
  
  ifstream in(LanguagePath + LabelPath + fn);
  if(!in) 
  { 
    error_handler(0, "Can't open file : " + fn + " !"); 
    return; 
  }

  string ArraySize_stg   = read_line(in);
  string Property_stg    = read_line(in);
  string to_rise_stg     = read_line(in);       
  string to_descend_stg  = read_line(in);       
  string randomize_stg   = read_line(in);       
  string constant_stg    = read_line(in);       
  string cancel_stg      = read_line(in);       
  string PanelLabel_stg  = read_line(in); 
  string Start_stg       = read_line(in);
  string SQ_stg          = read_line(in);
  string EQ_stg          = read_line(in);
  string Result_stg      = read_line(in);
  
  int Test_ArraySize = 5000;
  int Test_PrevSortMode = PrevSortMode;

  panel setup_panel;  
  setup_panel.text_item("");
  setup_panel.int_item(ArraySize_stg, Test_ArraySize, 0, 10000);
  setup_panel.choice_item(Property_stg, Test_PrevSortMode, to_rise_stg, to_descend_stg, randomize_stg, constant_stg);
  setup_panel.text_item("");
  setup_panel.button("Ok", 0);
  setup_panel.button(cancel_stg, 1);

  int but;
  while((but = setup_panel.open(gw, window::center, window::center)) != 0 && but != 1);

  if(but == 1) return;

  int *Test_Array_SQ = new int [Test_ArraySize];
  int *Test_Array_EQ = new int [Test_ArraySize];

  switch (Test_PrevSortMode) 
  {
    case 0: { // to rise
              for(int i = 0; i < Test_ArraySize; i++) 
              { 
                Test_Array_SQ[i] = i; 
                Test_Array_EQ[i] = i; 
              }
              break; 
            }
    case 1: { // to descend
              for(int i = 0; i < Test_ArraySize; i++) 
              { 
                Test_Array_SQ[i] = Test_ArraySize - i; 
                Test_Array_EQ[i] = Test_Array_SQ[i]; 
              }
              break; 
            }
    case 2: { // randomize 
              for(int i = 0; i < Test_ArraySize; i++) 
              { 
                Test_Array_SQ[i] = rand_int(0, Test_ArraySize); 
                Test_Array_EQ[i] = Test_Array_SQ[i]; 
              }
              break; 
            }
    case 3: { // constant
              for(int i = 0; i < Test_ArraySize; i++) 
              { 
                Test_Array_SQ[i] = 4; 
                Test_Array_EQ[i] = Test_Array_SQ[i]; 
              }
              break;            
            }
   }

  test_qs_observer observer(QUICKSORT);

  panel SQ_panel(PanelLabel_stg);
  SQ_panel.text_item(Start_stg + SQ_stg + "...");
  SQ_panel.display(window::center, window::center);

  float time1 = used_time();
  qsort(Test_Array_SQ, 0, Test_ArraySize-1);
  float t1 = used_time(time1);

  SQ_panel.close();

  panel EQ_panel(PanelLabel_stg);
  EQ_panel.text_item(Start_stg + EQ_stg + "...");
  EQ_panel.display(window::center, window::center);

  float time2 = used_time();
  QUICKSORT(Test_Array_EQ, 0, Test_ArraySize - 1);
  float t2 = used_time(time2);
  
  EQ_panel.close(); 
  
  delete [] Test_Array_SQ;
  delete [] Test_Array_EQ;
  
  panel Result_panel;
  Result_panel.text_item("\\bf " + Result_stg);
  Result_panel.text_item("");
  Result_panel.text_item(SQ_stg + "~:~" + string("%1.3f", t1) + "~~sec");
  Result_panel.text_item("");
  Result_panel.text_item(EQ_stg + "~:~" + string("%1.3f", t2) + "~~sec");
  Result_panel.text_item("");
  Result_panel.button("Ok", 0);
  while(Result_panel.open(gw, window::center, window::center) != 0);
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

  double xcenter = (gw.get_xmax() - gw.get_xmin()) / 2; 
  double ycenter = (gw.get_ymax() - gw.get_ymin()) / 2;
  double start_x = xcenter - (ArraySize / 2) * gw.get_node_height();
  
  int i;
  for(i = 0; i < ArraySize; i++) 
  { 
    gw.set_label_color(A_index[i], grey2);
    gw.set_color(A_index[i], invisible);
    gw.set_border_width(A_index[i], 0);
    gw.set_position(A_index[i],point(start_x + 2*i*grid_dist, ycenter - 5));    
  }

  for(i = 0; i < ArraySize; i++)  
    gw.set_position(A_node[i], point(start_x + 2 * i * grid_dist, ycenter - 2 * grid_dist)); 

  gw.set_flush(true);   
  gw.set_animation_steps(25);
  zoom_to_intervall(gw, A_node[0], A_node[ArraySize -  1]);   
}

//  init arrays
//
static void InitArrays(GraphWin& gw)
{  
  if(!A_int) 
    delete A_int;
  
  A_int = new int [ArraySize];  

  G.empty();               
  gw.clear_graph();
  
  switch (PrevSortMode) 
  {
    case 0: { // rise array
              for(int i=0; i<ArraySize; i++) 
                A_node[i] = G.new_node(i+1);
              break; 
            }
    case 1: { // descend array
              for(int i=0; i<ArraySize; i++)
                A_node[i] = G.new_node(ArraySize-i); 
              break; 
            }
    case 2: { // randomize array
              for(int i=0; i < ArraySize; i++)
                A_node[i] = G.new_node(rand_int(0, ArraySize)); 
              break; 
            }
    case 3: { // constant array
              for(int i=0; i<ArraySize; i++)
                A_node[i] = G.new_node(4); 
              break; 
            }
  }

  for(int i = 0; i < ArraySize; i++) 
  {
    A_index[i] = G.new_node(i); 
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

  string ArraySize_stg   = read_line(in);
  string Property_stg    = read_line(in);
  string Language_stg    = read_line(in);       
  string to_rise_stg     = read_line(in);       
  string to_descend_stg  = read_line(in);       
  string randomize_stg   = read_line(in);       
  string constant_stg    = read_line(in);       
  string cancel_stg      = read_line(in);       

  int language = 0;

  if (LanguagePath == engl) 
    language = 1;

  panel setup_panel;  
  //setup_panel.set_menu_style(1);
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


void InitLabels(window& gw, panel& main_panel)
{
  string fn = "gw.frame_label";
  
  ifstream in_frame_label(LanguagePath + LabelPath + fn);
  if(!in_frame_label) 
    error_handler(0, "Can't open file : " + fn + " !"); 
  else
  {
    string GraphWindow_label_stg   = read_line(in_frame_label);
    gw.set_frame_label(GraphWindow_label_stg);
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
    string compare_alg_help_stg = read_line(in_main_panel);
    string exit_help_stg        = read_line(in_main_panel);

    main_panel.set_button_help_str(3, run_help_stg);
    main_panel.set_button_help_str(1, adjustments_help_stg);            
    main_panel.set_button_help_str(2, over_help_stg);
    main_panel.set_button_help_str(4, dvi_print_help_stg);
    main_panel.set_button_help_str(5, compare_alg_help_stg);
    main_panel.set_button_help_str(0, exit_help_stg);
  }
}

void func(GraphWin& gw, int but)
{
  window& W = gw.get_window();

    switch(but) 
    {
      case 1: { 
                Setup_panel(gw); 
                InitArrays(gw);
                //InitLabels(gw, main_panel);
                break; 
              }
      case 2: { 
                TextLoader W_about(310, 110);
                W_about.extract_frame_label(LanguagePath + LabelPath +  "W_about.frame_label");
                W_about.load_text(LanguagePath + "about.txt");
                W_about.open_with_timeout(window::center, window::center, 5000);
                break; 
              }
      case 3: { // start simulation
                gw_qs_observer observer(QUICKSORT, gw, A_node, A_int, LanguagePath, ArraySize); 
                QUICKSORT(A_int, 0, ArraySize-1);         
            
                gw.set_flush(false);
                for(int i = 0; i < ArraySize; i++) 
                {
                  A_int[i] = G[A_node[i]];
                  gw.set_color(A_node[i], ivory);
                }
                gw.redraw();
                gw.set_flush(true);
                break;
              }           
      case 4: { 
                DVI_Print_panel(W); 
                break; 
              }
      case 5: { 
                CompareAlgorithms_panel(W); 
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

  if (!is_directory(("doc/quicksort")))
  { string lroot;
    if (get_environment("LEDAROOT",lroot))
    { engl = lroot + "/demo/animation/" + engl;
      deut = lroot + "/demo/animation/" + deut;
      ButtonPath = lroot + "/demo/animation/" + ButtonPath;
     }
   }

   
  LanguagePath = engl;             

  //GraphWin gw(G, 585, 200);  
  GraphWin gw(G, 1000, 400);  
   
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
  gw.set_node_label_font(fixed_font, 8);
  gw.set_zoom_labels(false);
  

  gw.add_simple_call(func3,"run",0);
  gw.add_simple_call(func1,"setup",0);
  gw.add_simple_call(func2,"info",0);
  gw.add_simple_call(func4,"dvi",0);
  gw.add_simple_call(func0,"exit",0);

  if (window::display_type() == "xx")
    gw.display(100,500);
  else
    gw.display(700,500);
  
  InitArrays(gw);

  gw.add_simple_call(func3,"run",0);
  gw.add_simple_call(func1,"setup",0);
  gw.add_simple_call(func2,"info",0);
  gw.add_simple_call(func4,"dvi",0);
  gw.add_simple_call(func0,"exit",0);
    

  gw.edit();
  delete [] A_int;
  return 0;     
}
