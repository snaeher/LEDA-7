#include "AnimationTree.h"


// publics
//
void AnimationTree::display(int x, int y) 
{   
  if(!wp->is_open()) 
  {
    gwp->display(x_WindowPos = x != -1 ? x : x_WindowPos, y_WindowPos = y != -1 ? y : y_WindowPos); 
  
    if(ShowTexture) 
      gwp->set_bg_xpm(slate_xpm);
    else 
      gwp->set_bg_color(white);
  }
  
  // attention: very importend point(0, 0)
  // see create tree (Start_x, Start_y)
  //
  gwp->set_flush(false);
  gwp->zoom_to_point(point(0,0));
  gwp->set_flush(true);
}



void AnimationTree::open_with_timeout(int x , int y, int time)
{
  display(x, y);
  wait_with_timeout(time);
  close();    
}




void AnimationTree::create_tree(const array<int>& A)
{
  if(A.size() < 1)
  {
    error_handler(0, "AnimationTree::create_tree\\n Array is empty !");  
    return;                   
  }

  gp->clear();
  gwp->set_flush(false);

  /*  
  if(!ap_fg) 
  {
    delete ap_fg; 
    delete ap_bg;
  }
  */

  ap_fg = new array<node>(A.low(), A.high());
  ap_bg = new array<node>(A.low(), A.high());
  
  int TreeDepth  = int(floor(log(double(A.size())) / log(2.0)));

  stack<double> Start_x_Stack;
  stack<double> NodeDistance_Stack;
  
  double DistXDummy  = node_dx;
  int TreeDepthDummy = TreeDepth;

  // compute node position in tree
  //
  while(TreeDepthDummy >= 0)
  {
    int MaxNodes         = 1 << TreeDepthDummy--; 

    double NodeSpaceSize = (MaxNodes - 1) * DistXDummy ;
    double NodeSize      = (MaxNodes - 1) * gwp->get_node_height();

    Start_x_Stack.push(-(NodeSpaceSize + NodeSize) / 2);

    double ActDistX = DistXDummy  + gwp->get_node_height();
    NodeDistance_Stack.push(ActDistX);
    DistXDummy  = ActDistX * 2 - gwp->get_node_height();  
  }
    
  double Start_y  = (TreeDepth) * node_dy / 2;
  double Start_x  = Start_x_Stack.pop();
  int NodeCounter = 0;  
  int MaxNodes    = 1 ; 
  color c         = TreeLevelColor1;
  DistXDummy      = NodeDistance_Stack.pop();
  TreeDepthDummy  = 0;

  // create and set nodes
  //  
  int i;
  for(i = A.low(); i <= A.high(); i++)
  {
    if(NodeCounter == MaxNodes)
    {
      NodeCounter = 0;
      MaxNodes    = 1 << ++TreeDepthDummy;  
      Start_y    -= node_dy;
      Start_x     = Start_x_Stack.pop();
      DistXDummy  = NodeDistance_Stack.pop();
      
      c = c == TreeLevelColor1 ?  TreeLevelColor2 : TreeLevelColor1;
    }

    (*ap_bg)[i] = gwp->new_node(point(Start_x, Start_y));
    gwp->set_height((*ap_bg)[i], int(0.90 * gwp->get_node_height()));
    gwp->set_color((*ap_bg)[i], invisible);
    gwp->set_border_color((*ap_bg)[i], black);
    gwp->set_label((*ap_bg)[i], "");

    (*ap_fg)[i] = gp->new_node(A[i]);
    gwp->update_graph();
    gwp->set_position((*ap_fg)[i], point(Start_x, Start_y));
    gwp->set_color((*ap_fg)[i], c);
    gwp->set_border_color((*ap_fg)[i], black);

    // create and set index nodes
    //
    if(ShowIndex) 
    {
      node v = gwp->new_node(point(Start_x, Start_y + 0.5 * node_dy + 3));
      gwp->set_label_color(v, grey2);
      gwp->set_color(v, invisible);
      gwp->set_border_width(v, 0);
      gwp->set_label(v, string("%d", i));
    }

    Start_x += DistXDummy;
    NodeCounter++;
  }

  // create edges
  //
  for(i = A.low(); i <= (A.high() / 2); i++)
  {
    edge e1 = gwp->new_edge((*ap_bg)[i], (*ap_bg)[(2*i)]);    
    
    // attention: parent with one child possible (index - overflow)
    //
    edge e2 = 0;
    if(A.high() >= (2*i) + 1)
      e2 = gwp->new_edge((*ap_bg)[i], (*ap_bg)[(2*i) + 1]);   
      
    if(!ShowTexture) continue;
      
    gwp->set_color(e1, grey1); 
    gwp->set_color(e2, grey1); 
  }
  
  for(i = A.low() + 1 ; i <= A.high() - 1; i++)
  {
    edge e = gwp->new_edge((*ap_bg)[i], (*ap_bg)[(i+1)]);   
    gwp->set_color(e, invisible);
  }

  gwp->redraw();
  gwp->set_flush(true);
}






// marked the edge between node (*ap)[i] and (*ap)[j]
//
void AnimationTree::marked_edge(int i, int j, color c, int width) 
{
  if(i > (*ap_bg).high() || j > (*ap_bg).high() ||  i < (*ap_bg).low() || j < (*ap_bg).low())
  {
    error_handler(0, "AnimationTree::marked_edge\\n Index overflow !");  
    return;
  } 
  
  gwp->set_flush(false);
  
  edge e;
  forall_edges(e, (*gp))
    if((*ap_bg)[i] == gp->source(e) && (*ap_bg)[j] == gp->target(e))
    {
      default_edge_color = gwp->get_color(e);
      default_edge_width = gwp->get_width(e);
    
      gwp->set_color(e, c);
      gwp->set_width(e, width);     
    }
    
  gwp->set_flush(true); 
  gwp->redraw();
}





// reset the edge color and edge width of default values 
//
void  AnimationTree::demarked_edge(int i, int  j) 
{
  if(i > (*ap_bg).high() || j > (*ap_bg).high() ||  i < (*ap_bg).low() || j < (*ap_bg).low()) 
  {
    error_handler(0, "AnimationTree::demarked_edge\\n Index overflow !");  
    return;
  }

  gwp->set_flush(false);
  
  edge e;
  forall_edges(e, (*gp))
    if((*ap_bg)[i] == gp->source(e) && (*ap_bg)[j] == gp->target(e))
    {
      gwp->set_color(e, default_edge_color);
      gwp->set_width(e, default_edge_width);
    }

  gwp->set_flush(true);
  gwp->redraw();
}





//  swap nodes  (*ap_fg)[i] and (*ap_fg)[j] with speed 
//

void AnimationTree::swap_nodes(int i, int j, int speed) 
{
  if(i > (*ap_fg).high() || j > (*ap_fg).high() ||  
     i < (*ap_fg).low()  || j < (*ap_fg).low())
  {
    error_handler(0, "AnimationTree::swap_nodes\\n No tree create !");  
    return;
  }
  
  swap_nodes_start(i,j);
  while (speed-- > 0) swap_nodes_step();
  swap_nodes_finish(i,j);

}


int AnimationTree::swap_nodes_start(int i, int j)
{
  node v = (*ap_fg)[i];
  node w = (*ap_fg)[j];

  point p = gwp->get_position(v);
  point q = gwp->get_position(w);

  gwp->move_nodes_begin();
  gwp->move_nodes_set_position(v,q);
  gwp->move_nodes_set_position(w,p);
  return gwp->move_nodes_end();
}


void AnimationTree::swap_nodes_step() 
{ gwp->move_nodes_step(); }


void AnimationTree::swap_nodes_finish(int i, int j) 
{ gwp->move_nodes_finish();
  gwp->set_border_color((*ap_fg)[j], black);
  gwp->set_border_color((*ap_fg)[i], black);    
  ap_fg->swap(i,j);
}


// open the file fname and set frame to the first line 
//
void AnimationTree::extract_frame_label(string fname)
{ 
  ifstream in(fname);
  if(in) 
    gwp->set_frame_label(window_label = read_line(in)); 
  else 
  {
    error_handler(0, "AnimationTree::extract_frame_label\\n Can't open file: " + fname + " !");  
    gwp->set_frame_label(window_label = "");  
  }
}


void AnimationTree::wait_with_timeout(int time)
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




// privates
//
// initalization of some default parameters
//
void AnimationTree::init_parameters()
{ 
  wp = &gwp->get_window();

  x_WindowPos = window::center;
  y_WindowPos = window::center;
  
  // GraphWin Parameters
  //
  gwp->set_default_menu(0);
  gwp->set_show_status(false);
  //gwp->set_grid_dist(GridDistance = 10);
  gwp->set_grid_dist(GridDistance = 12);
  gwp->set_grid_mode(0);  

  //  Global Node Parameters
  //
  gwp->set_node_shape(square_node);
  gwp->set_node_label_font(fixed_font, 12);
  gwp->set_node_label_type(data_label);
  gwp->set_zoom_labels(false);
  gwp->set_node_radius1(0.85 * GridDistance);  
  gwp->set_node_radius2(0.85 * GridDistance);  

  TreeLevelColor1 = blue2;
  TreeLevelColor2 = green2;

  ShowIndex = true;
  ShowTexture = true;
  
  node_dx = 1 * gwp->get_node_height();
  //node_dy = 1.8 * gwp->get_node_height();
  node_dy = 10.0 * gwp->get_node_height();
  
  // Global Edge Parameters
  //
  gwp->set_edge_direction(undirected_edge);
  default_edge_color = black;
  default_edge_width = 1;
}

