#include "ViewVariable.h"


// publics
//
void ViewVariable::display(int x, int y) 
{ 
  if(wp->is_open()) return;
    
  gwp->display(x_WindowPos = x != -1 ? x : x_WindowPos, y_WindowPos = y != -1 ? y : y_WindowPos); 

/*
  if(ShowTexture) 
    gwp->set_bg_xpm(slate_xpm);
  else 
    gwp->set_bg_color(white);
*/

  gwp->set_bg_color(green2);
    
  // attention: very important
  //
  gwp->set_flush(false);
  gwp->zoom_to_point(point(0,0));
  gwp->set_flush(true);
}



void ViewVariable::open_with_timeout(int x , int y, int time)
{
  display(x, y);
  wait_with_timeout(time);
  close();    
}




// open the file fname and set frame to the first line 
//
void ViewVariable::extract_frame_label(const string fname)
{ 
  ifstream in(fname);
  if(in) 
    gwp->set_frame_label(window_label = read_line(in)); 
  else 
  {
    error_handler(0, "ViewVariable::extract_frame_label\\n \
                      Can't open file: " + fname + " !");  
    gwp->set_frame_label(window_label = "");  
  }
}


void ViewVariable::wait_with_timeout(const int time)
{
  int val;
  double x;
  double y;
  unsigned long t = 0;

  int event;
  while((event = wp->read_event(val, x, y, t, time)) != no_event &&
         event != button_press_event && 
         event != key_press_event); 
}




void ViewVariable::extract_node_color(list<string>& arg_list, array<color>& ColArg)
{
  string Color[] = { "\\white ","\\black ", "\\red ",    "\\green ",  
                     "\\blue ", "\\yellow ","\\violett ","\\orange ", 
                     "\\cyan ", "\\brown ", "\\pink ",   "\\green2 ",
                     "\\blue2 ","\\grey1 ", "\\grey2 ",  "\\grey3 ",
                     "\\ivory " };
  
  int count = -1;
  list_item it;
  forall_items(it, arg_list)
  {
    string s = arg_list.inf(it); 
    count++;

    ColArg[count] = 1;    
    
    int i = 0;
    do
    { 
      int k;
      if((k = s.index(Color[i])) != -1) 
      {
        arg_list.assign(it, s.del(k, Color[i].length() -1));
        ColArg[count] = i;      
      }
    } while(++i < 17);
  }
}





void ViewVariable::create_table(const list<string> L1, const list<string> L2)
{
  if(L1.size() < 1 || L2.size() < 1) return;
  
  ArgList = L1;
  AtrList = L2;
  
  array<color> ColArg(ArgList.size());
  extract_node_color(ArgList, ColArg);
  
  gp->clear(); 
  gwp->set_flush(false); 
  gwp->update_graph();
  
  gwp->set_node_label_font(fixed_font, FontSize);
  gwp->set_node_height(int(FontSize * 1.8), true);

  // compute the rectangle width
  //
  string s;
  double tmp;
  double node_width  = 0;
  forall(s, ArgList)
    if(node_width < (tmp = wp->text_width(s) + 2))
      node_width = tmp;

  forall(s, AtrList)
  if(node_width < (tmp = wp->text_width(s) + 2))
      node_width = tmp;


  gwp->set_node_width(int(node_width * 1.1));
  
  int Tupel        = TupelperLine == -1 ? ArgList.size() : TupelperLine;
  int MaxLines     = TupelperLine == -1 ? 1 : int(ceil(double(ArgList.size()) / double(TupelperLine)));
  double TupelDist = MaxLines == 1 ? 0 : DistY * gwp->get_node_height();
  double Start_x   = -(Tupel - 1) * gwp->get_node_width() / 2;
  double Start_y   = gwp->get_node_height() * (MaxLines - 1/2) + (MaxLines - 1) * TupelDist / 2;
  Start_y -= 1 * GridDistance;
    
  int count1 = 0;
  int count2 = 1;
  list_item it_Arg;
  forall_items(it_Arg, ArgList)
  {
    node v = gwp->new_node(point(Start_x, Start_y));
    gwp->set_label(v, ArgList.inf(it_Arg));
    gwp->set_color(v, grey1);
    gwp->set_label_color(v, ColArg[count1++]);
    gwp->set_border_width(v, 1);
    
    if(++count2 > TupelperLine) 
    {
      count2 = 1;
      Start_y -= TupelDist + (2 * gwp->get_node_height());
      Start_x = -((Tupel - 1)  * gwp->get_node_width()) / 2;
    }
    else
      Start_x += gwp->get_node_width();   
  }

  Start_y  = gwp->get_node_height() * (MaxLines - 1/2) + (MaxLines - 1) * TupelDist / 2 - gwp->get_node_height();
  Start_y -= 1 * GridDistance;
  Start_x  = -(Tupel - 1) * gwp->get_node_width() / 2;
  
  count2 = 1;
  list_item it_Atr;
  forall_items(it_Atr, AtrList)
  {
    node w = gwp->new_node(point(Start_x, Start_y));
    gwp->set_label(w, AtrList.inf(it_Atr));
    gwp->set_color(w, gwp->get_bg_color());
    gwp->set_label_color(w, black);
    gwp->set_border_width(w, 1);

    if(++count2 > TupelperLine) 
    {
      count2 = 1;
      Start_y -= TupelDist + (2 * gwp->get_node_height());
      Start_x = -(Tupel - 1) * gwp->get_node_width() / 2;
    }
    else
      Start_x += gwp->get_node_width();
  }

  gwp->redraw();
  gwp->set_flush(true);
}




void ViewVariable::update_table(const list<string> L2)
{
  if(gp->empty()) 
  {
    error_handler(0, "ViewVariable::update_table \\n  No table created !");  
    return;
  }
  
  gwp->set_flush(false);
  
  AtrList = L2;
  
  int  i = gp->number_of_nodes();
  node w = gp->last_node();
  while(!AtrList.empty() && i-- > ArgList.size())
  {
    gwp->set_label(w, AtrList.pop_back());  
    w = gp->pred_node(w);
  }
  
  gwp->redraw();
  gwp->set_flush(true);
}




// privates
//
// initalization of some default parameters
//
void ViewVariable::init_parameters()
{ 
  wp = &gwp->get_window();

  x_WindowPos = window::center;
  y_WindowPos = window::center;
  
  ShowTexture = true;

  // GraphWin Parameters
  //
  gwp->set_default_menu(0);
  gwp->set_show_status(false);
  gwp->set_grid_dist(GridDistance = 10);
  gwp->set_grid_mode(0);

  //  Global Node Parameters
  //
  gwp->set_node_shape(rectangle_node);
  gwp->set_node_label_type(user_label);
  gwp->set_zoom_labels(true);
  gwp->set_zoom_objects(true);

  FontSize     = 14;
  TupelperLine = -1; // all tupels in same line
  DistY        = 1;
}

