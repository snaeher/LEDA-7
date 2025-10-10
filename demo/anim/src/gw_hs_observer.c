#include "gw_hs_observer.h"


// public functions
//
void gw_hs_observer::on_start(alg_heapsort&) 
{  
  int gw_xmin;
  int gw_xmax;
  int gw_ymin;
  int gw_ymax;

  (gw.get_window()).frame_box(gw_xmin, gw_ymin, gw_xmax, gw_ymax);

  AT_ptr_tree->extract_frame_label(LanguagePath + "win_and_panel_labels/W_tree.frame_label");
  AT_ptr_tree->set_node_distance(2.2, 1.7); 
  AT_ptr_tree->create_tree((*ap));
  AT_ptr_tree->set_display_position(gw_xmin, gw_ymax + 20);

  int i;
  for(i = A_node.low(); i <= ArraySize; i++)
    gw.set_color(A_node[i], AT_ptr_tree->get_node_color(i));

  TL_ptr_source->set_scrollbar(true);
  TL_ptr_source->set_bg_color(blue);
  TL_ptr_source->extract_frame_label(LanguagePath + "win_and_panel_labels/W_source.frame_label");
  TL_ptr_source->load_text_with_display(LanguagePath + "heapsort.txt", 
                                        gw_xmax, gw_ymin);  
  
  TL_ptr_run->extract_frame_label(LanguagePath + "win_and_panel_labels/W_run.frame_label");
  TL_ptr_run->set_button_space(30);
  TL_ptr_run->set_scrollbar(true);
  TL_ptr_run->create_menu(LanguagePath + "win_and_panel_labels/W_run.button_labels");
/*
  TL_ptr_run->set_display_position(gw_xmin, gw_ymin - TL_ptr_run->get_window_height() - 47);
*/
  TL_ptr_run->set_display_position(gw_xmin, -(gw_ymin-70));

  goto_label = TL_ptr_run->get_button_label(1);

  // dirty menu effects 
  //
  goto_label = goto_label.del(goto_label.length() - 3);  
  TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 8));

  delay_label = TL_ptr_run->get_button_label(2);

  // dirty menu effects 
  //
  delay_label = delay_label.del(delay_label.length() - 3);  
  TL_ptr_run->set_button_label(2, delay_label + string("%d ", AnimationDelay));

  VariableName.clear();                 
  VariableName.append("i");       
  VariableName.append("k");
  VariableName.append("j");         
  VariableName.append("\\red A[k]");      
  VariableName.append("A[j]");    
  VariableName.append("A[j+1]");    
  VariableName.append("N");   
  VariableName.append("swaps");     
  VariableName.append("compares"); 

  VariableValue.clear();
  for(i = 0; i < VariableName.size(); i++)
    VariableValue.append("");
    
  VV_ptr_state->extract_frame_label(LanguagePath +  "win_and_panel_labels/W_state.frame_label");
  VV_ptr_state->set_tupel_per_line(3);
  VV_ptr_state->set_fontsize(18);
  VV_ptr_state->create_table(VariableName, VariableValue);

/*
  VV_ptr_state->display(gw_xmax, 
                        gw_ymin -  AT_ptr_tree->get_window_height() - 27); 
*/
  VV_ptr_state->display(gw_xmax, -(gw_ymin-70));
}



// function heapsort
//
void gw_hs_observer::on_1(alg_heapsort&, int N) 
{
  if(Abort) return;
  
  show_active_line(14);

  TL_ptr_run->load_text_with_display(LanguagePath + "start.txt");

  AT_ptr_tree->display();
  
  actual_N = N;
  update_status_window();
  
  if(GotoLine == 8) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 9));
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}



void gw_hs_observer::on_2(alg_heapsort&, int i) 
{   
  if(Abort) return;
  
  show_active_line(16);

  TL_ptr_run->load_text_with_display(LanguagePath + "for.txt");
  
  actual_i = i;
  Compares++;
  update_status_window();

  if(GotoLine == 9) 
  {
    GotoLine = actual_i >= 1 ? 10 : 11; 
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}



void gw_hs_observer::on_3(alg_heapsort&, int, int) 
{
  if(Abort) return;
  
  show_active_line(17);

  TL_ptr_run->load_text_with_display(LanguagePath + "heapify_call1.txt");

  if(GotoLine == 10) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 1));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}




// function heapify
//
void gw_hs_observer::on_4(alg_heapsort&, int k) 
{
  if(Abort) return;
  
  show_active_line(1);

  TL_ptr_run->load_text_with_display(LanguagePath + "heapify_call2.txt");
  
  actual_i = -1;
  actual_k = k;
  update_status_window();

  col_k = AT_ptr_tree->set_node_color(actual_k, red);
  gw.set_color(A_node[actual_k], red);

  if(GotoLine == 1) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 2));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}





void gw_hs_observer::on_5(alg_heapsort&, int, int) 
{
  if(Abort) return;

  show_active_line(3);

  TL_ptr_run->load_text_with_display(LanguagePath + "while1.txt");
  
  Compares++;
  update_status_window();

  if(GotoLine == 2) 
  {
    if(actual_k * 2 > actual_N)
      GotoLine = BuildHeap ? 9 : 11;
    else 
      GotoLine = 3; 
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);


  if(actual_k * 2 > actual_N)
  {
    if(isZoom)
      zoom_to_interval(A_node[1], A_node[actual_N]);

    if(GotoLine != -1 && !SemiAuto)
      GotoLine = BuildHeap ? 9 : 11;

    AT_ptr_tree->set_node_color(actual_k, col_k);
    gw.set_color(A_node[actual_k], col_k);
  
    actual_k = -1;
    actual_j = -1;
  } 
  else 
    if(GotoLine != -1 && !SemiAuto)
      GotoLine = 3;
}




void gw_hs_observer::on_6(alg_heapsort&, int j) 
{
  if(Abort) return;

  show_active_line(5);

  TL_ptr_run->load_text_with_display(LanguagePath + "new_j.txt");
    
  actual_j = j;
  update_status_window();

  col_j = AT_ptr_tree->get_node_color(actual_j);

  if(GotoLine == 3) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 4));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}

void gw_hs_observer::on_7(alg_heapsort&, int N, int j) 
{    
  if(Abort) return;

  if(isZoom)
  { if(actual_j < actual_N)
      zoom_to_interval(A_node[actual_k], A_node[actual_j+1]);
    else 
      zoom_to_interval(A_node[actual_k], A_node[actual_j+1]);
   }
  
  show_active_line(6);

  TL_ptr_run->load_text_with_display(LanguagePath + "if1.txt"); 

  actual_N = N;
  actual_j = j;

  if(actual_j < actual_N)
  { 
    col_j = AT_ptr_tree->get_node_color(actual_j);
    
    col_marked = col_j == green2 ? green : blue;
    
    AT_ptr_tree->marked_edge(actual_j, actual_j + 1, ivory, 3);       
    TL_ptr_run->wait_with_timeout(AnimationDelay * 100);
    AT_ptr_tree->demarked_edge(actual_j, actual_j + 1);       

    if(A_int[actual_j] < A_int[actual_j+1])
      actual_j++;
      
    AT_ptr_tree->set_node_color(actual_j, col_marked);  
    gw.set_color(A_node[actual_j], col_marked);
  }
  
  Compares++;
  Compares++;
  update_status_window();

  if(GotoLine == 4) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 5));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}



void gw_hs_observer::on_8(alg_heapsort&, int, int) 
{
  if(Abort) return;

  show_active_line(7);

  TL_ptr_run->load_text_with_display(LanguagePath + "if2.txt");
  
  if(A_int[actual_k] >= A_int[actual_j])    
    AT_ptr_tree->marked_edge(actual_k, actual_j, grey1, 3);       
  else
    AT_ptr_tree->marked_edge(actual_k, actual_j, ivory, 3);       

  TL_ptr_run->wait_with_timeout(AnimationDelay * 100);
  AT_ptr_tree->demarked_edge(actual_k, actual_j);       
  
  Compares++;
  update_status_window();

  if(GotoLine == 5) 
  {
    if(A_int[actual_k] >= A_int[actual_j])
      GotoLine = BuildHeap ? 9 : 11;
    else
      GotoLine = 6;
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);

  if(A_int[actual_k] >= A_int[actual_j])
  {
    if(isZoom)
      zoom_to_interval(A_node[1], A_node[actual_N]);

    if(GotoLine != -1 && !SemiAuto)
      GotoLine = BuildHeap ? 9 : 11;

    AT_ptr_tree->set_node_color(actual_k, col_k);
    AT_ptr_tree->set_node_color(actual_j, col_j);

    gw.set_color(A_node[actual_k], col_k);
    gw.set_color(A_node[actual_j], col_j);

    actual_k = -1;
    actual_j = -1;
  }
  else 
    if(GotoLine != -1 && !SemiAuto)
      GotoLine = 6;
}





void gw_hs_observer::on_9(alg_heapsort&, int, int) 
{
  if(Abort) return;

  show_active_line(8);

  TL_ptr_run->load_text_with_display(LanguagePath + "swap1.txt");

  col_j = col_j == blue2 ? green2 : blue2;
  col_k = col_k == blue2 ? green2 : blue2;

  swap_nodes(actual_k,actual_j);

  AT_ptr_tree->set_node_color(actual_k, col_j);
  gw.set_color(A_node[actual_k], col_j);        
  
  Swaps++;
  update_status_window();

  if(GotoLine == 6) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 7));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}




void gw_hs_observer::on_10(alg_heapsort&, int, int j) 
{
  if(Abort) return;

  show_active_line(9);

  TL_ptr_run->load_text_with_display(LanguagePath + "j_to_k.txt");

  actual_k = j;
  update_status_window();

  if(GotoLine == 7) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 2));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}






// continue heapsort
//
void gw_hs_observer::on_11(alg_heapsort&, int) 
{
  BuildHeap = false;
  
  if(Abort) return;

  show_active_line(18);

  TL_ptr_run->load_text_with_display(LanguagePath + "while2.txt");
  
  Compares++;
  update_status_window();

  if(GotoLine == 11) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 12));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}






void gw_hs_observer::on_12(alg_heapsort&, int) 
{
  if(Abort) return;

  show_active_line(20);

  TL_ptr_run->load_text_with_display(LanguagePath + "swap2.txt");
    
  swap_nodes(1,actual_N);

  AT_ptr_tree->set_node_color(actual_N, grey1);
  AT_ptr_tree->set_node_color(1, blue2);
  AT_ptr_tree->marked_edge(actual_N/2, actual_N, grey3, 1);
  
  gw.set_color(A_node[actual_N], grey1);
  gw.set_color(A_node[1], blue2);
  
  Swaps++;
  update_status_window();

  if(GotoLine == 12) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 13));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}





void gw_hs_observer::on_13(alg_heapsort&, int) 
{
  if(Abort) return;

  show_active_line(21);

  TL_ptr_run->load_text_with_display(LanguagePath + "heapify_call3.txt");
  
  actual_N--;
  update_status_window();

  if(GotoLine == 13) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 1));    
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}




void gw_hs_observer::on_finish(alg_heapsort&) 
{
  // colour the first node
  //
  if(!Abort)
  {
    gw.set_color(A_node[1], grey1);
    AT_ptr_tree->set_node_color(1, grey1);
  }
  
  zoom_to_interval(A_node[1], A_node[ArraySize]); 

  TL_ptr_run->disable_all_buttons();
  TL_ptr_run->enable_button(0);
  
  TL_ptr_source->set_string_style("", 1, "");
  TL_ptr_source->load_text_with_display(LanguagePath + "heapsort.txt"); 

  if(!Abort) 
  {
    TL_ptr_run->load_text(LanguagePath + "finish.txt");
    while(TL_ptr_run->open_with_read_button() != 0);
  }   
  
  TL_ptr_run->close(); 
  TL_ptr_source->close(); 
  VV_ptr_state->close();
  AT_ptr_tree->close(); 
}




// private functions

void gw_hs_observer::show_active_line(int i)
{ 
  TL_ptr_source->set_string_style("\\red", i, "1\\");
  TL_ptr_source->load_text_with_display(LanguagePath + "heapsort.txt");
}
  

void gw_hs_observer::delay_panel()
{
  TL_ptr_run->disable_all_buttons();
  
  window *wpt_run = TL_ptr_run->get_window_ptr();
  
  panel speed_panel;  
  speed_panel.text_item("");
  speed_panel.int_item(delay_label, AnimationDelay, 1,  20);
  speed_panel.button("Ok", 0);
  while(speed_panel.open((*wpt_run), window::center, window::center) != 0);

  TL_ptr_run->set_button_label(2, delay_label + string("%d", AnimationDelay));
  
  TL_ptr_run->enable_all_buttons();
}




void gw_hs_observer::wait_of()  
{
  bool Continue = false;
  
  SemiAuto = false;
  
  int but;
  while(!Abort && GotoLine != -1 && !Continue && (but = TL_ptr_run->read_button()) != 0)
    switch (but) 
    {
      case 1 : {                  
                  TL_ptr_run->disable_all_buttons();
  
                  window* wpt_run = TL_ptr_run->get_window_ptr();
  
                  string fn = "Goto_panel.button_labels";
  
                  ifstream in(LanguagePath + "win_and_panel_labels/" + fn);
                  if(!in) 
                  { 
                    error_handler(0, "Can't open file : " + fn + " !"); 
                    return; 
                  }

                  string Continue_stg = read_line(in);
                  string Auto_stg     = read_line(in);
                  string Cancel_stg   = read_line(in);

                  int SaveGotoLine = GotoLine;
                  bool SaveZoom = isZoom;
                  
                  panel goto_panel;
                  goto_panel.text_item("");
                  //panel_item it = 
                  goto_panel.int_item(goto_label, GotoLine, 1, 13);
                  goto_panel.bool_item(" Zoom", isZoom);
                  goto_panel.button(Continue_stg, 0);
                  goto_panel.button(Auto_stg, 1);
                  goto_panel.button(Cancel_stg, 2);
  
                  int but = goto_panel.open((*wpt_run), window::center, window::center);

                  switch (but) 
                  {
                    case 0 : {
                                if(SaveGotoLine != GotoLine) SemiAuto = true;
                                TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine)); 
                                Continue = true;
                                break;
                             }
                    case 1 : {
                                GotoLine = -1; 
                                TL_ptr_run->set_button_label(1, Auto_stg);
                                break;
                             }
                    case 2 : {
                                GotoLine = SaveGotoLine;
                                isZoom = SaveZoom;
                                TL_ptr_run->enable_all_buttons();
                                break;
                             }
                  }
  
                  if(!isZoom) 
                    zoom_to_interval(A_node[1], A_node[ArraySize]); 

                  break;
               }
      case 2 : { 
                  delay_panel(); 
                  break;
               }
      case 3 : {
                  Abort = true; 
                  break;
               }
      case 4 : {
                  TL_ptr_run->disable_all_buttons();                  
                  TextLoader W_help(520, 280);
                  W_help.set_scrollbar(true);
                  W_help.extract_frame_label(LanguagePath + "win_and_panel_labels/W_help.frame_label");
                  W_help.create_menu(LanguagePath + "win_and_panel_labels/W_help.button_labels");
                  W_help.load_text(LanguagePath + "help.txt");
                  W_help.display();
                  while(W_help.read_button() != 0);
                  TL_ptr_run->enable_all_buttons();
                  break;
                }
    }
}



void gw_hs_observer::update_status_window()
{
  string str_actual_i = "";
  if(actual_i != -1) str_actual_i = string("%d", actual_i);
  
  string str_actual_k = "";
  if(actual_k != -1) str_actual_k = string("%d", actual_k);

  string str_A_int_k = "";
  if(actual_k > 0  && actual_k <= ArraySize) str_A_int_k = string("%d", A_int[actual_k]);

  string str_actual_j = "";
  string str_A_int_j = "";  
  string str_A_int_jp1 = "";  
  if(actual_j != -1)
  {
    str_actual_j = string("%d", actual_j);
    if(actual_j <= ArraySize && actual_j > 0)
      str_A_int_j  = string("%d", A_int[actual_j]);
    if(actual_j + 1 <= ArraySize && actual_j + 1 > 0)
      str_A_int_jp1  = string("%d", A_int[actual_j+1]);
  }
    
  VariableValue.clear();
  VariableValue.append(str_actual_i);
  VariableValue.append(str_actual_k);
  VariableValue.append(str_actual_j); 
  VariableValue.append(str_A_int_k);
  VariableValue.append(str_A_int_j);
  VariableValue.append(str_A_int_jp1);
  VariableValue.append(string("%d", actual_N));
  VariableValue.append(string("%d", Swaps));
  VariableValue.append(string("%d", Compares));

  VV_ptr_state->update_table(VariableValue);
  VV_ptr_state->display();
}



int gw_hs_observer::swap_nodes_start(int i, int j)
{ 
  node v = A_node[i];
  node w = A_node[j];

  point p = gw.get_position(v);
  point q = gw.get_position(w);
 
  gw.move_nodes_begin();
  gw.move_nodes_set_position(v,q);
  gw.move_nodes_set_position(w,p);
  return gw.move_nodes_end();
}

void gw_hs_observer::swap_nodes_step()
{ gw.move_nodes_step(); }

void gw_hs_observer::swap_nodes_finish()
{ gw.move_nodes_finish(); }




void gw_hs_observer::swap_nodes(int i, int j)
{
  GraphWin& gw2 = AT_ptr_tree->get_graphwin();

  int speed = (j-i)*AnimationDelay;

  gw.set_animation_steps(speed);
  gw2.set_animation_steps(speed);

  swap_nodes_start(i,j);
  AT_ptr_tree->swap_nodes_start(i,j);

  while (speed-- > 0) 
  { swap_nodes_step();
    AT_ptr_tree->swap_nodes_step();
   }

  swap_nodes_finish();
  AT_ptr_tree->swap_nodes_finish(i,j);

  leda_swap(A_node[i],A_node[j]);
}


void gw_hs_observer::zoom_to_interval(node v, node w)
{ 
  double delta_x = gw.get_xmax() - gw.get_xmin();
  double delta_y = gw.get_ymax() - gw.get_ymin();
  double f = delta_y / delta_x;

  point p = gw.get_position(v);
  point q = gw.get_position(w);
  double r = gw.get_node_radius1();

  double x0 = p.xcoord() - 2 * r;
  double x1 = q.xcoord() + 2 * r;

  while ((x1 - x0) < (26 * r))  
  { 
    x0 -= r;  
    x1 += r;  
  }

  double dy = f * (x1 - x0) / 2;
  double y0 = p.ycoord() - dy;
  double y1 = q.ycoord() + dy;

  gw.set_animation_steps(20);
  gw.zoom_area(x0, y0, x1, y1);
}

