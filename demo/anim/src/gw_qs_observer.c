#include "gw_qs_observer.h"


// public functions

void gw_qs_observer::on_start(alg_qsort&) 
{  
  int gw_xmin;
  int gw_xmax;
  int gw_ymin;
  int gw_ymax;

  (gw.get_window()).frame_box(gw_xmin, gw_ymin, gw_xmax, gw_ymax);
  
  TL_ptr_source->set_scrollbar(true);
  TL_ptr_source->set_bg_color(blue);
  TL_ptr_source->extract_frame_label(LanguagePath + "win_and_panel_labels/W_source.frame_label");
  TL_ptr_source->load_text_with_display(LanguagePath + "qsort.txt", 
                                        gw_xmax + 5, gw_ymin); 
  
  TL_ptr_run->set_scrollbar(true);
  TL_ptr_run->extract_frame_label(LanguagePath + "win_and_panel_labels/W_run.frame_label");
  TL_ptr_run->set_button_space(30);
  TL_ptr_run->create_menu(LanguagePath + "win_and_panel_labels/W_run.button_labels");
/*
  TL_ptr_run->set_display_position(gw_xmin, gw_ymin - TL_ptr_run->get_window_height() - 47);
*/
  TL_ptr_run->set_display_position(gw_xmin, -gw_ymin);

  goto_label = TL_ptr_run->get_button_label(1);

  // dirty menu effects 
  //
  goto_label = goto_label.del(goto_label.length() - 3);  
  TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 1));

  delay_label = TL_ptr_run->get_button_label(2);

  // dirty menu effects 
  //
  delay_label = delay_label.del(delay_label.length() - 3);  
  TL_ptr_run->set_button_label(2, delay_label + string("%d ", AnimationDelay));

  VariableName.clear();
  VariableName.append("l");       
  VariableName.append("r");       
  VariableName.append("\\blue2 i");       
  VariableName.append("\\green2 j");      
  VariableName.append("\\red v");         
  VariableName.append("A[i]");    
  VariableName.append("A[j]");    
  VariableName.append("swaps");     
  VariableName.append("compares"); 

  VariableValue.clear();
  for(int i = 0; i < VariableName.size(); i++)
    VariableValue.append("");
    
  VV_ptr_state->extract_frame_label(LanguagePath +  "win_and_panel_labels/W_state.frame_label");
  VV_ptr_state->set_tupel_distance(0.5);
  VV_ptr_state->set_tupel_per_line(5);
  VV_ptr_state->set_fontsize(11);
  VV_ptr_state->create_table(VariableName, VariableValue);  
  VV_ptr_state->display(gw_xmin, gw_ymax + 5);
}


void gw_qs_observer::on_1(alg_qsort&, int left, int right) 
{
  if(Abort) return;
   
  // attention: index over- or underflow possible
  //
  if(isZoom && left < ArraySize && right > 0) 
    zoom_to_interval(A_node[left], A_node[right]);
  
  show_active_line(2);

  actual_v = -1;
  actual_j = -1;
  actual_i = -1;
  actual_l = left;
  actual_r = right;
  update_status_window();
  
  TL_ptr_run->load_text_with_display(LanguagePath + "start.txt");
  TL_ptr_run->disable_all_buttons();
  
  if(GotoLine == 1) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 2));
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}

void gw_qs_observer::on_2(alg_qsort&, int left, int right) 
{   
  if(Abort) return;

  show_active_line(4);

  Compares++;
  update_status_window();

  // attention: index over- or underflow possible
  //
  if(left >= right) 
  {
    if(right < 0) 
      gw.set_color(A_node[left], grey2);
    else 
      gw.set_color(A_node[right], grey2);

    if(GotoLine != -1 && !SemiAuto) 
      GotoLine = firstQuicksortCall ? 13 : 1; 
  }

  TL_ptr_run->load_text_with_display(LanguagePath + "comp_l_r.txt");
  TL_ptr_run->disable_all_buttons();

  if (GotoLine == 2) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 3));
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}

void gw_qs_observer::on_3(alg_qsort&, int i) 
{
  if(Abort) return;

  show_active_line(5);

  actual_i = i;
  update_status_window();

  TL_ptr_run->load_text_with_display(LanguagePath + "new_i.txt");
  TL_ptr_run->disable_all_buttons();

  if(GotoLine == 3) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 4));
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}

void gw_qs_observer::on_4(alg_qsort&, int j) 
{
  if(Abort) return;

  show_active_line(6);

  actual_j = j;
  update_status_window();

  TL_ptr_run->load_text_with_display(LanguagePath + "new_j.txt");
  TL_ptr_run->disable_all_buttons();

  if(GotoLine == 4)  
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 5));
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}

void gw_qs_observer::on_5(alg_qsort&, int v) 
{
  if(Abort) 
    return;

  show_active_line(7);

  gw.set_color(A_node[v], red); 

  actual_v = v;
  update_status_window();

  TL_ptr_run->load_text_with_display(LanguagePath + "pivot.txt");
  TL_ptr_run->disable_all_buttons();

  if (GotoLine == 5) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 6));
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}

void gw_qs_observer::on_6(alg_qsort&) 
{
  if(Abort) return;

  show_active_line(8);

  TL_ptr_run->load_text_with_display(LanguagePath + "for.txt");
  TL_ptr_run->disable_all_buttons();

  if (GotoLine==6) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 7));
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}

void gw_qs_observer::on_7(alg_qsort&, int i, int v) 
{
  if(Abort) return;

  show_active_line(10);

  gw.set_color(A_node[i], blue2);
  gw.set_color(A_node[v], red); 
 
  Compares = Compares + 2;
  actual_i = i;
  update_status_window();
 
  TL_ptr_run->load_text_with_display(LanguagePath + "while1.txt");
  TL_ptr_run->disable_all_buttons();

  if(GotoLine == 7) 
  {
    TL_ptr_run->enable_all_buttons();
    GotoLine = A_int[i] >= A_int[v] || i >= actual_r ? 8 : 7; 

    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine));
    
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70); 
}


void gw_qs_observer::on_8(alg_qsort& , int j, int v) 
{    
  if(Abort) return;

  show_active_line(11);
  
  gw.set_color(A_node[j], green2);
  gw.set_color(A_node[v], red); 

  Compares++;
  actual_j = j;
  update_status_window();
    
  TL_ptr_run->load_text_with_display(LanguagePath + "while2.txt");
  TL_ptr_run->disable_all_buttons();

  if(GotoLine == 8) 
  {
    TL_ptr_run->enable_all_buttons();
    GotoLine = A_int[j] <= A_int[v] ? 9 : 8;

    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine));

    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}


void gw_qs_observer::on_9(alg_qsort&, int i, int j) 
{
  if(Abort) return;
  
  show_active_line(12);

  Compares++;
  actual_j = j;
  actual_i = i;
  update_status_window();
  
  TL_ptr_run->load_text_with_display(LanguagePath + "if_i_j.txt");
  TL_ptr_run->disable_all_buttons();
  
  if(GotoLine == 9) 
  {
    TL_ptr_run->enable_all_buttons();
    GotoLine = i >= j ? 11 : 10;

    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine));

    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}



void gw_qs_observer::on_10(alg_qsort&, int i, int j) 
{
  if(Abort) return;

  show_active_line(13);

  swap_nodes(i, j);
  gw.set_color(A_node[j], green2);
  gw.set_color(A_node[i], blue2);
  
  Swaps++;
  update_status_window();
  
  TL_ptr_run->load_text_with_display(LanguagePath + "swap1.txt");
  TL_ptr_run->disable_all_buttons();
  
  if(GotoLine == 10) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 7));
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70); 
}


void gw_qs_observer::on_11(alg_qsort&, int left, int j) 
{
  if(Abort) return;
    
  show_active_line(15);

  swap_nodes(left, j);
  gw.set_color(A_node[j], grey2);

  for(int i = actual_l; i <= actual_r; i++)
    if(i != j) 
      gw.set_color(A_node[i], ivory);

  Swaps++;
  update_status_window();

  TL_ptr_run->load_text_with_display(LanguagePath + "swap2.txt");
  TL_ptr_run->disable_all_buttons();
  
  if(GotoLine == 11) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 12));
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}


void gw_qs_observer::on_12(alg_qsort&, int left, int j) 
{
  // first Quicksort call 
  //
  firstQuicksortCall = true;
  
  if(Abort) return;

  show_active_line(16);

  TL_ptr_run->load_text_with_display(LanguagePath + "new_qsort1.txt");
  TL_ptr_run->disable_all_buttons();

  if (GotoLine == 12) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 1));
    wait_of();
  } 
  else  
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70); 
}

void gw_qs_observer::on_13(alg_qsort&, int i, int right) 
{
  // second Quicksort call 
  //
  firstQuicksortCall = false;

  if(Abort) return;

  show_active_line(17);

  TL_ptr_run->load_text_with_display(LanguagePath + "new_qsort2.txt");
  TL_ptr_run->disable_all_buttons();
  
  if (GotoLine == 13) 
  {
    TL_ptr_run->enable_all_buttons();
    TL_ptr_run->set_button_label(1, goto_label + string("%d", GotoLine = 1));
    wait_of();
  } 
  else 
    TL_ptr_run->wait_with_timeout(AnimationDelay * 70);
}

void gw_qs_observer::on_finish(alg_qsort&) 
{
  zoom_to_interval(A_node[0], A_node[ArraySize - 1]); 

  TL_ptr_run->enable_all_buttons();
  TL_ptr_run->disable_button(1);
  TL_ptr_run->disable_button(2);
  TL_ptr_run->disable_button(3);
  TL_ptr_run->disable_button(4);
  
  TL_ptr_source->set_string_style("", 1, "");
  TL_ptr_source->load_text_with_display(LanguagePath + "qsort.txt");  

  if(!Abort) 
  {
    TL_ptr_run->load_text(LanguagePath + "finish.txt");
    while(TL_ptr_run->open_with_read_button() != 0);
  }   
  
  TL_ptr_run->close(); 
}


// private functions

void gw_qs_observer::show_active_line(int i)
{
  TL_ptr_source->set_string_style("\\red", i, "1\\");
  TL_ptr_source->load_text_with_display(LanguagePath + "qsort.txt");
}
  


void gw_qs_observer::delay_panel()
{
  TL_ptr_run->disable_all_buttons();
  
  window *wpt_run = TL_ptr_run->get_window_ptr();
  
  panel speed_panel;  
  speed_panel.text_item("");
  speed_panel.int_item(delay_label, AnimationDelay, 1,  10);
  speed_panel.button("Ok", 0);
  while(speed_panel.open((*wpt_run), window::center, window::center) != 0);

  TL_ptr_run->set_button_label(2, delay_label + string("%d", AnimationDelay));
  
  TL_ptr_run->enable_all_buttons();
}





void gw_qs_observer::wait_of()  
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
                    zoom_to_interval(A_node[0], A_node[ArraySize-1]); 

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


void gw_qs_observer::update_status_window()
{
  string str_actual_i;
  string str_actual_v;
  string str_A_int_i;
  
  if(actual_i != -1)
  {
    str_actual_i = string("%d", actual_i);
    str_A_int_i  = string("%d", A_int[actual_i]);
  }
  
  string str_actual_j;
  string str_A_int_j;
  
  if(actual_j != -1)
  {
    str_actual_j = string("%d", actual_j);
    if(actual_j < ArraySize)
      str_A_int_j  = string("%d", A_int[actual_j]);
  }
    
  if(actual_v != -1)
    str_actual_v = string("%d", A_int[actual_v]);
  
  VariableValue.clear();
  VariableValue.append(string("%d", actual_l));
  VariableValue.append(string("%d", actual_r));
  VariableValue.append(str_actual_i);   
  VariableValue.append(str_actual_j); 
  VariableValue.append(str_actual_v);
  VariableValue.append(str_A_int_i);
  VariableValue.append(str_A_int_j);
  VariableValue.append(string("%d", Swaps));
  VariableValue.append(string("%d", Compares));

  VV_ptr_state->update_table(VariableValue);
}



void gw_qs_observer::swap_nodes(int i, int j)
{
  node v = A_node[i];
  node w = A_node[j];
  
  graph& G = gw.get_graph();
  window& W = gw.get_window();
 
  node_array<point> pos(G);
  node u;
  forall_nodes(u,G) 
    pos[u] = gw.get_position(u);

  double xv = pos[v].xcoord();
  double yv = pos[v].ycoord();
  double xw = pos[w].xcoord();
  double yw = pos[w].ycoord();

  double dy = 20;

  int xpix = W.real_to_pix(xw-xv);
  int ypix = W.real_to_pix(dy);

  gw.set_flush(true);

  pos[v] = point(xv,yv-dy);
  pos[w] = point(xw,yw-dy);

  gw.set_animation_steps(AnimationDelay + ypix * 1/4);
  gw.set_position(pos);


  pos[v] = point(xw,yw-dy);
  pos[w] = point(xv,yv-dy);

  gw.set_animation_steps(AnimationDelay + xpix * 1/4);
  gw.set_position(pos);

  pos[v] = point(xw,yw);
  pos[w] = point(xv,yv);

  gw.set_animation_steps(AnimationDelay + ypix * 1/4);
  gw.set_position(pos);

  A_node[i] = w; 
  A_node[j] = v; 
}



void gw_qs_observer::zoom_to_interval(node v, node w)
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

