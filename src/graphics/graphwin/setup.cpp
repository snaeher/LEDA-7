/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  setup.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/pixmaps/texture.h>
#include <LEDA/system/file.h>

#if defined(_MSC_VER)
#pragma warning(disable:4305)
#pragma warning(disable:4309)
#endif


#include "bitmaps.h"
#include "local.h"

LEDA_BEGIN_NAMESPACE

/*
static unsigned char* direction_bits[]= { 
  undirected_bits, directed_bits, redirected_bits, bidirected_bits,
  mid_bidirected_bits, mid_directed_bits, mid_redirected_bits };
*/

static unsigned char* direction_bits[]= { 
  directed_bits, redirected_bits, 
  mid_directed_bits, mid_redirected_bits };

static const char* agd_format_str[] = { "gw", "gml", "layout" };


#define lpos_width	nlab_c_width
#define lpos_height	nlab_c_height



static unsigned char* lpos_bits[]= { 
     nlab_lt_bits, nlab_t_bits, nlab_rt_bits, 
     nlab_l_bits,  nlab_c_bits, nlab_r_bits, 
     nlab_lb_bits, nlab_b_bits, nlab_rb_bits };


#define nshape_width	rectangle_width
#define nshape_height	rectangle_height

static unsigned char* nshape_bits[]= { 
    circle_bits, ellipse_bits, square_bits, rectangle_bits, 
    roundrect_bits, ovalrect_bits, rhombus_bits };

#define elab_width  elab_c_width
#define elab_height elab_c_height

static unsigned char* elab_bits[]= { elab_l_bits, elab_c_bits, elab_r_bits };

//----------------------------------------------------------------------------


enum gw_epos {lpos, cpos, rpos };

struct panel_node_info {  // node_info with members usable for panel-items
  color  clr;
  int    shape;
  color  border_clr;
  int    border_w;
//double x;
//double y;
  int    x;
  int    y;
  int    width;
  int    height;
  string label;
  string data_label;
  color  label_clr;
  int    label_pos;
  int    label_t;
  bool   selected;
  char*  pixmap;
};


struct panel_edge_info {  // edge_info with members usable for panel-items
  color  clr;
  int    shape;
  int    style;
  int    dir;
  int    width;
  string label;
  string data_label;
  color  label_clr;
  int    label_pos;
  int    label_t;
  list<point> p;
  bool   selected;
};

//----------------------------------------------------------------------------

static panel*           current_panel;
static panel_item       current_wpit;
static panel_item       current_hpit;
static panel_item       current_ltpit;
static panel_node_info* current_pinf;
static panel_edge_info* current_einf;
static node_array<node_info>* current_nsave_i;
static edge_array<edge_info>* current_esave_i;


static void e_color_handler(int c)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_color(e,color(c)); 
  gw->e_animation_step();
}

static void e_lcolor_handler(int c)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_label_color(e,color(c)); 
  gw->e_animation_step();
}

static void e_width_handler(int w)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_width(e,w); 
  gw->e_animation_step();
}

static void e_shape_handler(int s)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_shape(e,(gw_edge_shape)s); 
  gw->e_animation_step();
}


static void e_style_handler(int s)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_style(e,(gw_edge_style)s); 
  gw->e_animation_step();
}

static void e_dir_handler(int d)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_direction(e,(gw_edge_dir)d); 
  gw->e_animation_step();
}



static void e_ltype_handler(int t)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_label_type(e,(gw_label_type)t); 
  gw->e_animation_step();
}


static void e_lpos_handler(int p)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L)
  { gw_position pos = central_pos;
    switch (p) {
      case lpos: pos = west_pos;    break;
      case rpos: pos = east_pos;    break;
     }
    gw->set_label_pos(e,pos); 
  }
  gw->e_animation_step();
}

static void e_label_handler(char* s) 
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_user_label(e,s); 
  gw->e_animation_step();
}

static void e_data_label_handler(char* s) 
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<edge>& L = gw->get_e_animation_list();
  edge e;
  forall(e,L) gw->set_data(e,s); 
  gw->e_animation_step();
}



static void make_panel(GraphWin& gw, panel& P, panel_edge_info& p, long mask) {

  graph& G = gw.get_graph();

  // create panel_items 
  if (mask & E_SHAPE) 
  { list<string> L;
    L.append("poly");
    L.append("circle");
    L.append("bezier");
    L.append("spline");
    P.choice_item("shape",p.shape,L,e_shape_handler);
  }

  if (mask & E_COLOR) 
    P.color_item("color",p.clr,e_color_handler);


  if (mask & E_WIDTH) 
    P.lwidth_item("width",p.width,e_width_handler);

  if (mask & E_STYLE) 
  { //P.lstyle_item("style",(line_style&)p.style,e_style_handler);
    union { int* ptr; line_style* ls; } u = {&p.style};
    P.lstyle_item("style",*u.ls,e_style_handler);
   }


  if (mask & E_DIR) 
/*
     P.choice_item("direction",p.dir,7,undirected_width,undirected_height,
                                                        direction_bits,
                                                        e_dir_handler);
*/
/*
     P.choice_mult_item("direction",p.dir,4,directed_width,directed_height,
                                                        direction_bits,
                                                        e_dir_handler);
*/

     P.choice_mult_item("direction",p.dir,4,arrow56_width,arrow56_height,
                                                         direction_bits,
                                                          e_dir_handler);



  P.text_item("");
  P.text_item("\\bf\\blue Label");

  if (mask & E_LTYPE) 
  { list<string> L;
    L.append("user");
    L.append("data");
    L.append("index");
    P.choice_mult_item("type",p.label_t,L,e_ltype_handler);
   }

  if (mask & E_LABEL) 
    P.string_item("user",p.label,e_label_handler);

  string et = G.edge_type(); 
  if (et != "void")
    P.string_item(string("data (%s)",~et),p.data_label,e_data_label_handler);

  if (mask & E_LCOLOR) 
     P.color_item("color",p.label_clr,e_lcolor_handler);

  if (mask & E_LPOS) 
     P.choice_item("position",p.label_pos,3,elab_width,elab_height,
                                                       elab_bits,
                                                       e_lpos_handler);
  P.text_item("");

} 



static void n_color_handler(int c)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();

  node v;
  forall(v,L) gw->set_color(v,color(c)); 
  gw->n_animation_step();
}

static void n_lcolor_handler(int c)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L) gw->set_label_color(v,color(c)); 
  gw->n_animation_step();
}


static void n_xpos_handler(int x)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L)
  { point p = gw->get_position(v);
    gw->set_position(v,point(x,p.ycoord())); 
   }
  gw->n_animation_step();
}


static void n_ypos_handler(int y)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L)
  { point p = gw->get_position(v);
    gw->set_position(v,point(p.xcoord(),y)); 
   }
  gw->n_animation_step();
}




static void n_width_handler(int w)
{ 
  GraphWin* gw = window::get_call_window()->get_graphwin();

  if (current_pinf->shape == square_node || current_pinf->shape == circle_node)
  { current_pinf->height = w;
    if (current_hpit) current_panel->redraw_panel(current_hpit);
   }  

  const list<node>& L = gw->get_n_animation_list();

  node v;
  forall(v,L) 
  { gw->set_width(v,w); 
    if (gw->get_shape(v) == square_node || gw->get_shape(v) == circle_node)
      gw->set_height(v,w);
   }
  gw->n_animation_step();
}

static void n_height_handler(int h)
{ GraphWin* gw = window::get_call_window()->get_graphwin();

  if (current_pinf->shape == square_node || current_pinf->shape == circle_node)
  { current_pinf->width = h;
    if (current_wpit) current_panel->redraw_panel(current_wpit);
   }  

  const list<node>& L = gw->get_n_animation_list();

  node v;
  forall(v,L) 
  { gw->set_height(v,h); 
    if (gw->get_shape(v) == square_node || gw->get_shape(v) == circle_node)
      gw->set_width(v,h);
   }
  gw->n_animation_step();
}


static void adjust_nodes_to_label(GraphWin* gw, const list<node>& L)
{ node v;
  forall(v,L) gw->adjust_node_to_label(v);
  if (!L.empty())
  { v = L.head();
    current_pinf->height = gw->get_height(v);
    current_pinf->width = gw->get_width(v);
    if (current_hpit) current_panel->redraw_panel(current_hpit);
    if (current_wpit) current_panel->redraw_panel(current_wpit);
   }
}


static void n_shape_handler(int s)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  int h = current_pinf->height;

  const list<node>& L = gw->get_n_animation_list();

  if (s == square_node || s == circle_node)
  { current_pinf->width = h;
    if (current_wpit) current_panel->redraw_panel(current_wpit);
    node v;
    forall(v,L) gw->set_width(v,gw->get_height(v));
   }

  if (L.length() == 1)
  { node v = L.head();
    int s0 = gw->get_shape(v);
    if ((s != square_node  && s != circle_node) && 
        (s0 == square_node || s0 == circle_node))
    { current_pinf->width = 2*h;
      if (current_wpit) current_panel->redraw_panel(current_wpit);
       gw->set_width(v,2*h);
     }
   }

  node v;
  forall(v,L) gw->set_shape(v,(gw_node_shape)s); 

  gw->n_animation_step();
}


static void n_border_color_handler(int c)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L) gw->set_border_color(v,color(c)); 
  gw->n_animation_step();
}

static void n_border_width_handler(int w)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L) gw->set_border_width(v,w); 
  gw->n_animation_step();
}





static void n_ltype_handler(int t)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L)
  { gw_label_type lt = (gw_label_type)t; 
    if ((lt & user_label) == 0)
    { if (gw->get_user_label(v).index("@") > -1)
         gw->get_window().del_pixrect(gw->set_pixmap(v,0));
     }
    gw->set_label_type(v,lt); 
   }
/*
  if (gw->get_adjust_to_label() && gw->get_node_label_type() != index_label)
     adjust_nodes_to_label(gw,L);
*/
  gw->n_animation_step();
}


static void n_lpos_handler(int p)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L) gw->set_label_pos(v,(gw_position)p); 
  gw->n_animation_step();
}

static void n_label_handler(char* s) 
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L) 
  { if (string(s).index("@") > -1) 
    { gw->set_label_type(v,user_label);
      current_pinf->label_t = user_label; 
      if (current_ltpit) current_panel->redraw_panel(current_ltpit);
     }
    gw->set_user_label(v,s); 
   }
  if (current_pinf->label_t & user_label) 
  { if (gw->get_adjust_to_label())
         adjust_nodes_to_label(gw,L);
    gw->n_animation_step();
   }
}


static void n_data_label_handler(char* s) 
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  const list<node>& L = gw->get_n_animation_list();
  node v;
  forall(v,L) gw->set_data(v,s); 
  if (current_pinf->label_t == data_label) 
  { if (gw->get_adjust_to_label() && gw->get_node_label_type() != index_label)
       adjust_nodes_to_label(gw,L);
    gw->n_animation_step();
   }
}



static void make_panel(GraphWin& gw, panel& P, panel_node_info& p, long mask) {

  graph& G = gw.get_graph();

  current_panel = &P;

  // create panel_items 

  if (mask & N_SHAPE) 
      P.choice_item("shape",p.shape,7,nshape_width,nshape_height,nshape_bits,
                                                             n_shape_handler);
  if (mask & N_COLOR) 
      P.color_item("color",p.clr,n_color_handler);

  if (mask & N_XPOS) 
      P.int_item("xcoord",p.x,(int)gw.get_xmin(),(int)gw.get_xmax(),
                                                  n_xpos_handler);

  if (mask & N_YPOS) 
      P.int_item("ycoord",p.y,(int)gw.get_ymin(),(int)gw.get_ymax(),
                                                  n_ypos_handler);

//int W = gw.get_window().width()/2;
  int W = 235;

  if (mask & N_WIDTH) 
    current_wpit = P.int_item("width",p.width,0,W,n_width_handler);
  else 
    current_wpit = 0;

  if (mask & N_HEIGHT) 
    current_hpit = P.int_item("height",p.height,0,W,n_height_handler);
  else 
    current_hpit = 0;


  P.text_item("");
  P.text_item("\\bf\\blue Border");

  if (mask & N_BWIDTH) 
    P.lwidth_item("width",p.border_w,n_border_width_handler);

  if (mask & N_BCOLOR) 
      P.color_item("color",p.border_clr,n_border_color_handler);




  P.text_item("");
  P.text_item("\\bf\\blue Label");

  if (mask & N_LTYPE) 
  { list<string> L;
    //L.append("none");
    L.append("user");
    L.append("data");
    L.append("index");
    current_ltpit = P.choice_mult_item("type",p.label_t,L,n_ltype_handler);
   }

  list<string> xpm_list;

  string xpm_dir = gw.get_xpm_path();

  while (xpm_dir != "")
  { int pos = xpm_dir.index(":");
    int len = xpm_dir.length();
    if (pos < 0) pos = len;
    string dir = xpm_dir(0,pos-1);
    xpm_dir = xpm_dir(pos+1,len-1);
    if (is_directory(dir))
    { list<string> flist = get_files(dir,"*.xpm");
      string s;
      forall(s,flist) xpm_list.append("@"+s);
     }
  }

  xpm_list.push("@hatched3");
  xpm_list.push("@hatched2");
  xpm_list.push("@hatched1");

  if (mask & N_LABEL) 
  { if (xpm_list.empty())
       P.string_item("user",p.label,n_label_handler);
    else
       P.string_item("user",p.label,xpm_list,8,n_label_handler);
   }

  string nt = G.node_type(); 
  if (nt != "void")
    P.string_item(string("data (%s)",~nt),p.data_label,n_data_label_handler);

  if (mask & N_LCOLOR) 
    P.color_item("color",p.label_clr,n_lcolor_handler);
   

  if (mask & N_LPOS) 
     P.choice_item("position",p.label_pos,9,lpos_width,lpos_height,
                                                       lpos_bits,
                                                       n_lpos_handler);

  P.text_item("");

} 

//----------------------------------------------------------------------------

static long find_diff(GraphWin& gw, const list<edge>& L) {

  // returns or'd members of the edge_info-struct that are invalid  

  if (L.empty()) return E_COMPLETE;

  long mask=0;

  const edge_info& ei=gw.get_info(L.head());

  edge e;

  forall(e,L) {
    const edge_info& ni=gw.get_info(e);
    if (ei.clr       != ni.clr)		 mask |= E_COLOR;
    if (ei.shape     != ni.shape)	 mask |= E_SHAPE;
    if (ei.style     != ni.style)	 mask |= E_STYLE;
    if (ei.dir       != ni.dir) 	 mask |= E_DIR;
    if (ei.width     != ni.width)      	 mask |= E_WIDTH;
    if (ei.label     != ni.label)	 mask |= E_LABEL;
    if (ei.label_t   != ni.label_t)	 mask |= E_LTYPE;
    if (ei.label_clr != ni.label_clr)	 mask |= E_LCOLOR;
    if (ei.label_pos != ni.label_pos)	 mask |= E_LPOS;
    if (mask == E_COMPLETE) break;	// all invalid
  }

  return mask;
}

static long find_diff(GraphWin& gw, const list<node>& L) {

  // returns or'd members of the node_info-struct that are invalid  

  if (L.empty()) return N_COMPLETE;

  long mask=0;

  const node_info& ni=gw.get_info(L.head());

  double x=ni.pos.xcoord();
  double y=ni.pos.ycoord();

  node v;

  forall(v,L) {
    const node_info& vi=gw.get_info(v);
    if (ni.clr       != vi.clr)		 mask |= N_COLOR;
    if (ni.shape     != vi.shape)	 mask |= N_SHAPE;
    if (ni.border_clr!= vi.border_clr)	 mask |= N_BCOLOR;
    if (ni.border_w  != vi.border_w)     mask |= N_BWIDTH;
    if (x            != vi.pos.xcoord()) mask |= N_XPOS;
    if (y            != vi.pos.ycoord()) mask |= N_YPOS;
    if (ni.r1        != vi.r1) 		 mask |= N_WIDTH;
    if (ni.r2        != vi.r2) 		 mask |= N_HEIGHT;
    if (ni.label     != vi.label)	 mask |= N_LABEL;
    if (ni.label_t   != vi.label_t)	 mask |= N_LTYPE;
    if (ni.label_clr != vi.label_clr)	 mask |= N_LCOLOR;
    if (ni.label_pos != vi.label_pos)	 mask |= N_LPOS;
    if (mask == N_COMPLETE) break;	// all invalid
  }

  return mask;
}

//----------------------------------------------------------------------------

static long make_panel_info(GraphWin& gw,const list<edge>& L,panel_edge_info& e)
{
  window& W = gw.get_window();

  edge_info ei;
  long mask = 0;

  if (L.empty())
     ei = gw.get_edge_info();
  else
   { mask = find_diff(gw,L);
     ei   = gw.get_info(L.head());
    }

  int e_pwidth = W.real_to_pix(ei.width);
  if (e_pwidth == 0 && ei.width > 0) e_pwidth = 1;

  e.clr       = ( mask & E_COLOR  ? color(-4) : ei.clr );
  e.shape     = ( mask & E_SHAPE  ? -1 : ei.shape );
  e.style     = ( mask & E_STYLE  ? -1 : ei.style );
  e.dir       = ( mask & E_DIR    ? -1 : ei.dir );
  e.width     = ( mask & E_WIDTH  ? -1 : e_pwidth);
  e.label_t   = ( mask & E_LTYPE  ?  0 : ei.label_t);
  e.label_clr = ( mask & E_LCOLOR ? color(-4) : ei.label_clr);

  //  e.label_pos = ( mask & E_LPOS   ? -1 : ei.label_pos );

  if (mask & E_LPOS) e.label_pos = -1;
  else {
    switch(ei.label_pos) {
      case west_pos: e.label_pos = lpos; break;
      case east_pos: e.label_pos = rpos; break;
      default      : e.label_pos = cpos; break;
    }
  }

  if (mask & E_LABEL) 
     e.label = ""; 
  else 
     e.label = ei.label;

  e.data_label = "";
  if (!L.empty()) e.data_label = gw.get_data(L.head());

  e.selected  = false;

  return mask;
}



static long make_panel_info(GraphWin& gw, const list<node>& L, 
                                          panel_node_info& n) 
{
  window& W = gw.get_window();

  node_info ni;
  long mask = 0;

   if (L.empty())
     { mask = N_POS;
       ni = gw.get_node_info();
      }
   else
     { mask = find_diff(gw,L);
       ni   = gw.get_info(L.head());
     }

/*
// NO POSITION SLIDERS (never)
  mask &= ~N_POS;
*/


  n.x         = ( mask & N_XPOS   ) ? -MAXINT : (int)ni.pos.xcoord();
  n.y         = ( mask & N_YPOS   ) ? -MAXINT : (int)ni.pos.ycoord();
  n.clr       = ( mask & N_COLOR  ) ? color(-4) : ni.clr;
  n.shape     = ( mask & N_SHAPE  ) ? -1 : ni.shape;
  n.border_clr= ( mask & N_BCOLOR ) ? color(-4) : ni.border_clr;
  n.border_w  = ( mask & N_BWIDTH ) ? -1 : W.real_to_pix(ni.border_w);
  n.width     = ( mask & N_WIDTH  ) ? -1 : gw.radius2width(ni.r1);
  n.height    = ( mask & N_HEIGHT ) ? -1 : gw.radius2height(ni.r2);
  n.label_clr = ( mask & N_LCOLOR ) ? color(-4) : ni.label_clr;
  n.label_t   = ( mask & N_LTYPE  ) ?  0 : ni.label_t;
  n.label_pos = ( mask & N_LPOS   ) ? -1 : ni.label_pos;


  n.pixmap    = ( mask & N_PIXMAP ) ? 0 : ni.pixmap;

  if (mask & N_LABEL) 
      n.label = ""; 
  else 
      n.label = ni.label;

  n.data_label = "";
  if (!L.empty()) n.data_label = gw.get_data(L.head());


  n.selected  = false;

  return mask;
}

//----------------------------------------------------------------------------

static long make_info(GraphWin& gw, const panel_edge_info& p,
		      edge_info& i, long mask) {

  // creates edge_info from panel_edge_info

  window& W = gw.get_window();

  if (mask & E_COLOR) {
    if (p.clr != -4) i.clr = p.clr;
    else mask &= ~E_COLOR; 
  }
  if (mask & E_SHAPE) {
    if (p.shape != -1) i.shape = (gw_edge_shape) p.shape;
    else mask &= ~E_SHAPE;
  }
  if (mask & E_STYLE) {
    if (p.style != -1) i.style = (gw_edge_style) p.style;
    else mask &= ~E_STYLE;
  }
  if (mask & E_DIR) {
    if (p.dir != -1) i.dir = (gw_edge_dir) p.dir;
    else mask &= ~E_DIR;
  }
  if (mask & E_WIDTH) { 
    if (p.width != -1) i.width = (float)W.pix_to_real(p.width);
    else mask &= ~E_WIDTH;
  }

  if (mask & E_LTYPE) i.label_t = (gw_label_type)p.label_t;

  if (mask & E_LABEL) {
     if (p.label != "") i.label = p.label;
     else mask &= ~N_LABEL;
  }

  if (mask & E_LCOLOR)i.label_clr = p.label_clr;

  if (mask & E_LPOS) {
    if (p.label_pos != -1) {

      switch (p.label_pos) {

	case lpos: i.label_pos = west_pos;    break;
	case rpos: i.label_pos = east_pos;    break;
	default  : i.label_pos = central_pos; break;

      }

      //      i.label_pos = (gw_position) p.label_pos; 
    }
    else mask &= ~E_LPOS;
  }

  return mask;
}

static long make_info(GraphWin& gw, const panel_node_info& p,
		      node_info& i, long mask) {

  // creates node_info from panel_node_info

  window& W = gw.get_window();

  if (mask & N_COLOR) {
    if (p.clr != -4) i.clr = p.clr;
    else mask &= ~N_COLOR; 
  }
  if (mask & N_SHAPE) {
    if (p.shape != -1) i.shape = (gw_node_shape) p.shape;
    else mask &= ~N_SHAPE;
  }

  if (mask & N_BCOLOR) {
    if (p.border_clr != -4) i.border_clr = p.border_clr;
    else mask &= ~N_BCOLOR;
  }

  if (mask & N_BWIDTH) {
    if (p.border_w != -1) i.border_w = (float)W.pix_to_real(p.border_w);
    else mask &= ~N_BWIDTH;
  }

  if (mask & N_POS) {
    if (p.x == -MAXINT) mask &= ~N_XPOS;
    if (p.y == -MAXINT) mask &= ~N_YPOS;
    i.pos = point(p.x,p.y);
  }

  if (mask & N_WIDTH) { 
    if (p.width != -1) i.r1 = gw.width2radius(p.width);
    else mask &= ~N_WIDTH;
  }
  if (mask & N_HEIGHT) {
    if (p.height != -1) i.r2 = gw.height2radius(p.height); 
    else mask &= ~N_HEIGHT;
  }

  if (mask & N_LTYPE) i.label_t = (gw_label_type)p.label_t;

  if (mask & N_LABEL) {
     if (p.label != "") i.label = p.label;
     else mask &= ~N_LABEL;
  }

  if (mask & N_LCOLOR) i.label_clr = p.label_clr;

  if (mask & N_LPOS) {
    if (p.label_pos != -1) i.label_pos = (gw_position) p.label_pos; 
    else mask &= ~N_LPOS;
  }

  if (mask & N_PIXMAP) i.pixmap = p.pixmap;


  return mask;
}

//----------------------------------------------------------------------------


#define APPLY_NONE   0
#define APPLY_NEW    1
#define APPLY_SELECT 2
#define APPLY_ALL    4

#define P_RESET 101
#define P_DONE  102
#define P_UNDO  103


static void e_apply_to_all_handler(int b)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  graph& G = gw->get_graph();
  bool f = gw->set_flush(false);
  edge e;
  if (b)
    { edge_info i;
      long mask = make_info(*gw,*current_einf,i,E_COMPLETE & ~E_SELECT & ~E_POLYGON);
      forall_edges(e,G) gw->set_info(e,i,mask);
      gw->e_animation_end();
      gw->e_animation_start(G.all_edges());
     }
  else
    { forall_edges(e,G) gw->set_info(e,(*current_esave_i)[e],E_COMPLETE & ~E_POLYGON);
      gw->e_animation_end();
      list<edge> empty;
      gw->e_animation_start(empty);
     }
  gw->set_flush(f);
  gw->e_animation_step();
}



static string make_edge_string(const graph& G, const list<edge>& L)
{ 
  string s = "\\bf\\blue ";

  if (L.size() == 0) 
     s += "Edge Default Parameters";
  else 
  { if (L.size() == 1) 
       s += "Edge: ";
    else 
       s += "Edges: ";
    int count = 4;
    edge e;
    forall(e,L)
    { if (count-- == 0) { s += "  ... "; break; }
      node v = source(e);
      node w = target(e);
      s += string(" %d---%d",G.index(v),G.index(w)); 
    }
  }  
  return s;
}


static void display_setup_panel(panel& P, window& W)
{
  int xleft,xright,ytop,ybot;
  W.frame_box(xleft,ytop,xright,ybot);

  int screen_width = window::screen_width();
  
  if (xleft+xright > screen_width)
    P.display(-(xleft-1),ytop);
  else
    P.display(xright+1,ytop);

  P.frame_box(xleft,ytop,xright,ybot);

  int min_x = 30;
  int max_x = screen_width - 30;

  int xpos = xleft;

  if (xright > max_x) 
    xpos = xleft + (max_x-xright);
  else
    if (xleft < min_x) xpos = min_x;

  if (xpos != xleft) P.resize(xpos,ytop+130,P.width(),P.height());

}

static void panel_close_handler(window* wp)
{ int but = wp->get_button("done");
  if (but != -1) wp->set_focus_button(but);
}



bool gw_setup(GraphWin& gw, const list<edge>& L0, long mask)
{
  gw.changing_layout(L0);

  gw.edge_setup_active = true;

  gw_action save_action = gw.get_action(A_LEFT|A_EDGE);
  gw.set_action(A_LEFT|A_EDGE, gw_select_edge);

  window& W = gw.get_window();
  graph&  G = gw.get_graph();

  mask &= ~E_POLYGON;
  mask &= ~E_SELECT;

  panel_edge_info p;
  current_einf = &p;

  list<edge> L = L0;
  bool apply_to_all = false;
  bool default_setup = false;

  long m = make_panel_info(gw,L,p);

  panel P("GraphWin Edge Setup");
  gw.set_gwin(P);

  P.set_window_close_handler(panel_close_handler);

  if (L.size() == 0) default_setup = true;

  string s = make_edge_string(G,L);

  if (L.size() != 1) mask &= ~E_POLYGON;
   
  P.text_item(s);

  edge_array<edge_info> save_i(G);
  current_esave_i = &save_i;

  edge e;
  forall_edges(e,G) save_i[e] = gw.get_info(e);

  gw.e_animation_start(L);

  make_panel(gw,P,p,mask & ~(m & E_POLYGON)); 

  bool set_defaults = false;

  P.text_item("");
  if (default_setup) 
    P.bool_item("apply",apply_to_all,e_apply_to_all_handler);
  else
    P.bool_item("as default",set_defaults);

  P.button("undo",  P_UNDO,  "undo all changes");
  P.button("reset", P_RESET, "reset to defaults");
  P.fbutton("done",  P_DONE,  "close and apply");

  P.buttons_per_line(4);

  W.disable_panel();
  display_setup_panel(P,W);

  for(;;)
  { //int but = P.read_mouse()

    window* wp;
    int but;
    double x,y;

    if (read_event(wp,but,x,y) != button_press_event) continue;

    if (wp == &W)
    { gw.e_animation_end();
      gw.found_node = nil;
      gw.found_frame = nil;
      gw.found_edge = nil;
      gw.handle_button_event(but,x,y);
      L = gw.get_selected_edges();
      m = make_panel_info(gw,L,p);
      P.set_text(P.first_item(),make_edge_string(G,L));
      P.redraw_panel();
      gw.changing_layout(L);
      gw.e_animation_start(L);
      continue;
     }

    if (wp != &P) continue;

    if (but == P_DONE) break;
   
    switch (but)  {

    case P_RESET: {

      edge_info i;

      if (default_setup)
      { p.label_t=DefEdgeLabelType;
        p.clr=DefEdgeColor;
        p.shape=DefEdgeShape;
        p.style=DefEdgeStyle;
        p.dir=DefEdgeDir;
        p.width=int(DefEdgeWidth);
        if (p.width == 0 && DefEdgeWidth > 0) p.width = 1;
        p.label=DefEdgeLabel;
        p.label_clr=DefEdgeLabelClr;
        switch(DefEdgeLabelPos) {
        case west_pos: p.label_pos = lpos; break;
        case east_pos: p.label_pos = rpos; break;
        default      : p.label_pos = cpos; break;
        }
      }
      else
      { list<edge> empty;
        make_panel_info(gw,empty,p);
       }

      make_info(gw,p,i,E_COMPLETE);

      const list<edge>& L = gw.get_e_animation_list();
      edge e;
      forall(e,L) gw.set_info(e,i,E_COMPLETE & ~E_SELECT & ~E_POLYGON);
      gw.e_animation_step();
      P.redraw_panel();
      break;
    }

    case P_UNDO: { 
      const list<edge>& L = gw.get_e_animation_list();
      edge e;
      forall(e,L) gw.set_info(e,save_i[e],E_COMPLETE & ~E_POLYGON);
      gw.e_animation_step();
      make_panel_info(gw,L,p);
      P.redraw_panel();
      break;
     }

   }

  }

  gw.e_animation_end();
  
  P.close();

  //gw.enable_calls();
  W.enable_panel();

  if (default_setup || set_defaults)
  { edge_info i;
    mask=make_info(gw,p,i,mask & ~E_POLYGON); 
    gw.set_edge_info(i,mask);
   }

  gw.edge_setup_active = false;
  gw.set_action(A_LEFT|A_EDGE, save_action);

  return p.selected;
}



static void n_apply_to_all_handler(int b)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  graph& G = gw->get_graph();
  bool f = gw->set_flush(false);
  node v;
  if (b)
    { node_info i;
      long mask = make_info(*gw,*current_pinf,i,N_COMPLETE & ~N_SELECT);
      forall_nodes(v,G) gw->set_info(v,i,mask);
      gw->n_animation_end();
      gw->n_animation_start(G.all_nodes());
     }
  else
    { forall_nodes(v,G) gw->set_info(v,(*current_nsave_i)[v],N_COMPLETE);
      gw->n_animation_end();
      list<node> empty;
      gw->n_animation_start(empty);
     }
  gw->set_flush(f);
  gw->n_animation_step();
}


static string make_node_string(const graph& G, const list<node>& L)
{ 
  string s = "\\bf\\blue ";

  if (L.size() == 0) 
     s += "Node Default Parameters";
  else 
  { if (L.size() == 1) 
       s += "Node: ";
    else 
       s += "Nodes: ";
    int count = 10;
    node v;
    forall(v,L)
    { if (count-- == 0) { s += "  ... "; break; }
      s += string(" %d",G.index(v)); 
    }
  }  
  return s;
}


bool gw_setup(GraphWin& gw, const list<node>& L0, long mask)
{
  gw.changing_layout(L0);

  gw.node_setup_active = true;
  gw_action save_action = gw.get_action(A_LEFT|A_NODE);
  gw.set_action(A_LEFT|A_NODE, gw_select_node);

  window& W = gw.get_window();
  graph&  G = gw.get_graph();

  mask &= ~N_SELECT;

  panel_node_info p;
  current_pinf = &p;

  list<node> L = L0;
  bool apply_to_all = false;
  bool default_setup = false;

  //long m = 
  make_panel_info(gw,L,p);

  panel P("GraphWin Node Setup");
  gw.set_gwin(P);

  P.set_window_close_handler(panel_close_handler);

  if (L.size() == 0) 
  { default_setup = true;
    mask &= ~N_POS;
   }

  string  s =  make_node_string(G,L);
  P.text_item(s);

  gw.n_animation_start(L);

  node_array<node_info> save_i(G);
  current_nsave_i = &save_i;

  node v;
  forall_nodes(v,G) save_i[v] = gw.get_info(v);


  make_panel(gw,P,p,mask);

  bool set_defaults = false;

  P.text_item("");
  if (default_setup)
    P.bool_item("apply",apply_to_all,n_apply_to_all_handler);
  else
    P.bool_item("as default",set_defaults);

  P.text_item("");
  P.button("undo",  P_UNDO,  "undo all changes");
  P.button("reset", P_RESET, "reset to defaults");
  P.fbutton("done",  P_DONE,  "close and apply");

  P.buttons_per_line(4);

  W.disable_panel();
  display_setup_panel(P,W);

  for(;;)
  { //int but = P.read_mouse();

    window* wp;
    int but;
    double x,y;

    if (read_event(wp,but,x,y) != button_press_event) continue;

    if (wp == &W)
    { gw.n_animation_end();
      gw.found_node = nil;
      gw.found_edge = nil;
      gw.handle_button_event(but,x,y);
      L = gw.get_selected_nodes();
      //m = 
      make_panel_info(gw,L,p);
      P.set_text(P.first_item(),make_node_string(G,L));
      P.redraw_panel();
      gw.changing_layout(L);
      gw.n_animation_start(L);
      continue;
     }

    if (wp != &P) continue;

    if (but == P_DONE) break;

    if (but == P_RESET)
    { node_info i;
      if (default_setup)
        { p.label_t=DefNodeLabelType;
          p.clr=DefNodeColor;
          p.shape=DefNodeShape;
          p.border_clr=DefNodeBorderColor;
          p.border_w=int(DefNodeBorderWidth);
          if (p.border_w == 0 && DefNodeBorderWidth > 0) p.border_w = 1;
          p.width  = gw.radius2width(DefNodeRadius1);
          p.height = gw.radius2height(DefNodeRadius2);
          p.label=DefNodeLabel;
          p.label_clr=DefNodeLabelClr;
          p.label_pos=DefNodeLabelPos;
          p.pixmap=DefNodePixmap;
       }
       else
       { list<node> empty;
         make_panel_info(gw,empty,p);
         if (L.size() == 1)
         { point pos = gw.get_position(L.head());
           p.x = (int)pos.xcoord();
           p.y = (int)pos.ycoord();
          }
        }
      make_info(gw,p,i,N_COMPLETE);
      const list<node>& L = gw.get_n_animation_list();
      node v;
      forall(v,L) gw.set_info(v,i,N_COMPLETE & ~N_POS & ~N_SELECT);
      gw.n_animation_step();
      P.redraw_panel();
    }

    if (but == P_UNDO) 
    { const list<node>& L = gw.get_n_animation_list();
      node v;
      forall(v,L) gw.set_info(v,save_i[v],N_COMPLETE);
      gw.n_animation_step();
      make_panel_info(gw,L,p);
      P.redraw_panel();
     }
  }


  gw.n_animation_end();

  P.close();

  //gw.enable_calls();
  W.enable_panel();

  if (default_setup || set_defaults)
  { node_info i;
    mask = make_info(gw,p,i,mask & ~N_POS & ~N_PIXMAP);
    gw.set_node_info(i,mask);
  }

  gw.node_setup_active = false;
  gw.set_action(A_LEFT|A_NODE, save_action);

  return p.selected;
}

//----------------------------------------------------------------------------

/*
static void status_handler(int b)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  gw->set_show_status(b);
}

static void grid_handler(int d)
{ GraphWin* gw = window::get_call_window()->get_graphwin();
  window& W = gw->get_window();
  W.set_grid_mode(-d);
  gw->redraw();
}
*/



void gw_window_setup(GraphWin& gw)
{
  window& W = gw.get_window();

  bool d3_look     = gw.get_d3_look();
  color bg_col     = gw.get_bg_color();
  string pm_name   = gw.get_bg_pmname();

  int  grid_dist  = gw.get_grid_dist();
  int  g_style    = (int)gw.get_grid_style();

  bool status      = gw.get_show_status();
  int  show_mask1  = (int)gw.status_show_mask & 15;
  int  show_mask2  = (int)gw.status_show_mask >> 4 ;

  list<string> pm_list;
  pm_list.append("none");
  int i;
  for(i=0; i<num_texture; i++) pm_list.append(name_texture[i]);

  list<string> show_list1;
  show_list1.append("nodes");
  show_list1.append("edges");
  show_list1.append("loops");
  show_list1.append("planar");

  list<string> show_list2;
  show_list2.append("flush");
  show_list2.append("grid");
  show_list2.append("coord");
  show_list2.append("undo");

  bool status_save = status;
  int  grid_save   = grid_dist;


  int max_dist = 64;
  //int max_dist = int(gw.get_xmax() - gw.get_xmin())/8;


  double xmin = gw.get_xmin();
  double xmax = gw.get_xmax();
  double ymin = gw.get_ymin();


  panel P;
  gw.set_gwin(P);

/*
  P.text_item("\\bf\\blue Window Coordinates");
  P.double_item("xmin",xmin);
  P.double_item("xmax",xmax);
  P.double_item("ymin",ymin);
*/

  P.text_item("\\bf\\blue Background");
  P.color_item("solid color",bg_col);
  P.string_item("pm texture",pm_name,pm_list,6);
  //P.bool_item("3d look",d3_look);
  P.text_item("");

  P.text_item("\\bf\\blue Grid Settings");
  P.int_item("grid dist",grid_dist,0,max_dist);
  P.choice_item("grid style",g_style,"invisible", "points", "lines");
  P.text_item("");

  P.text_item("\\bf\\blue Status Line");
  P.bool_item("show status",status);
  P.choice_mult_item("status entries",show_mask1,show_list1);
  P.choice_mult_item("              ",show_mask2,show_list2);
  P.text_item("");


  P.fbutton("apply",   2);
  P.button("defaults",1);
  P.button("cancel",  0);

  int but;

  while ((but = gw.open_panel(P,false)) == 1 )
  { xmin       = DefXMin;
    xmax       = DefXMax;
    ymin       = DefYMin;
    d3_look    = DefD3Look;
    bg_col     = DefBgColor;
    pm_name    = "none";
    grid_dist  = DefGridDist;
    g_style    = DefGridStyle;
    status     = DefShowStatus;
    show_mask1 = DefShowMask & 15;
    show_mask2 = DefShowMask >> 4 ;
   }

  if (but == 0) 
  { bool b = gw.set_flush(false);
    gw.set_show_status(status_save);
    gw.set_grid_dist(grid_save);
    gw.set_flush(b);
    gw.redraw();
    return; 
   }

  bool b = gw.set_flush(false);

  if (pm_name == "none")
     gw.set_bg_pixmap(0);
  else
  { list_item it = pm_list.first();
    int i = 0;
    while (it && pm_list[it] != pm_name) { i++; it = pm_list.succ(it); }
    if (it != nil) 
      { const char** xpm_data = xpm_texture[i-1];
        gw.set_bg_xpm(xpm_data);
       }
    else
     { char* pm = W.create_pixrect(pm_name);
       gw.set_bg_pixmap(pm);
      }
   }

  gw.set_d3_look(d3_look);
  gw.set_bg_color(bg_col);
  gw.set_bg_pmname(pm_name);
  gw.set_grid_style((grid_style)g_style);
  gw.set_grid_dist(grid_dist);
  gw.status_show_mask = show_mask1 | (show_mask2 << 4);
  gw.set_show_status(status);
  gw.set_flush(b);
  gw.redraw();
}


void gw_global_setup(GraphWin& gw)
{
  bool adj_to_label  = gw.adjust_to_label;
  bool ortho_mode    = gw.get_ortho_mode();
  bool auto_create   = gw.auto_create_target;
  int  edge_dist     = gw.get_edge_distance();

  string  n_index_format = gw.get_node_index_format();
  string  e_index_format = gw.get_edge_index_format();

  bool e_border = gw.get_edge_border();

  gw_font_type node_font_t;
  int          node_font_sz;
  gw.get_node_label_font(node_font_t, node_font_sz);

  gw_font_type edge_font_t;
  int          edge_font_sz;
  gw.get_edge_label_font(edge_font_t, edge_font_sz);

  int nft = node_font_t;
  int eft = edge_font_t;

  list<string> font_list;
  font_list.append("roman");
  font_list.append("bold");
  font_list.append("italic");
  font_list.append("fixed");

  list<string> n_index_format_list;
  n_index_format_list.append("%d");
  n_index_format_list.append("%D");
  n_index_format_list.append("%c");
  n_index_format_list.append("%C");
  n_index_format_list.append("v_{%d}");
  n_index_format_list.append("$v_{%d}$");

  list<string> e_index_format_list;
  e_index_format_list.append("%d");
  e_index_format_list.append("%D");
  e_index_format_list.append("%c");
  e_index_format_list.append("%C");
  e_index_format_list.append("e_{%d}");
  e_index_format_list.append("$e_{%d}$");


  panel P;
  gw.set_gwin(P);

  P.set_window_close_handler(panel_close_handler);

  P.text_item("\\bf\\blue Global Node Options");
  P.choice_item("label font type",nft,font_list);
  P.int_item("label font size",node_font_sz,0,64);
  P.string_item("index format",n_index_format,n_index_format_list,6);
  P.bool_item("adjust to label",adj_to_label);
  P.text_item("");

  P.text_item("\\bf\\blue Global Edge Options");
  P.choice_item("label font type",eft,font_list);
  P.int_item ("label font size",edge_font_sz,0,64);
  P.string_item ("index format",e_index_format,e_index_format_list,6);
  P.int_item ("multi-edge dist",edge_dist,1,10);

  P.bool_item("edge borders",e_border);
  P.bool_item("auto create target",auto_create);
  P.bool_item("orthogonal mode",ortho_mode);

  P.text_item("");


  P.fbutton("apply",   2);
  P.button("defaults",1);
  P.button("cancel",  0);

  int but;

  while ((but = gw.open_panel(P,false)) == 1 )
  { node_font_sz   = int(DefNodeLabelFontSize);
    edge_font_sz   = int(DefEdgeLabelFontSize);
    n_index_format = DefNodeIndexFormat;
    e_index_format = DefEdgeIndexFormat;
    e_border       = DefEdgeBorder;
    adj_to_label   = DefNodeAdjLabel;
    auto_create    = DefAutoCreateTarget;
    ortho_mode     = DefOrthoMode;
    edge_dist      = DefMultiEdgeDist;
   }

  if (but == 0) 
  { gw.redraw();
    return; 
   }

  node_font_t = (gw_font_type)nft;
  edge_font_t = (gw_font_type)eft;


  bool b = gw.set_flush(false);
  gw.set_node_label_font(node_font_t,node_font_sz);
  gw.set_edge_label_font(edge_font_t,edge_font_sz);
  gw.set_node_index_format(n_index_format);
  gw.set_edge_index_format(e_index_format);
  gw.set_edge_border(e_border);
  gw.adjust_to_label = adj_to_label;
  gw.set_edge_distance(edge_dist);
  gw.auto_create_target = auto_create;
  gw.set_ortho_mode(ortho_mode);
  gw.set_flush(b);

  gw.redraw();
}


void gw_agd_setup(GraphWin& gw)
{
  string  host = gw.agd_host;
  int     port = gw.agd_port;
  int  in_form = gw.agd_input_format; 
//int out_form = gw.agd_output_format; 
  bool optdiag = gw.agd_option_diag;

  list<string> host_list;

  host_list.append("localhost");
  host_list.append("leda.uni-trier.de");
  host_list.append("chomsky.uni-trier.de");

  string s;
  if (get_environment("AGDHOST",s)) host_list.append(s);

  panel P;
  gw.set_gwin(P);

  P.text_item("");
  P.text_item("\\bf\\blue AGD Server Options");
  P.text_item("");
  P.string_item("host name",host,host_list,4);
  P.int_item("port number",port);
  P.text_item("");
  P.choice_item("graph format",in_form,agd_format_str[0],agd_format_str[1]);
//P.choice_item("output format",out_form,agd_format_str[0],agd_format_str[1]);
  P.bool_item("options dialog",optdiag);
  P.text_item("");

  P.fbutton("connect",2);
  P.button("defaults",1);
  P.button("cancel",  0);

  int but;
  while ((but = gw.open_panel(P,false)) == 1 )
  { host  = DefAgdHost;
    port  = DefAgdPort;
    in_form = DefAgdInputFormat;
  //out_form = DefAgdOutputFormat;
    optdiag = DefAgdOptionDiag;
   }

  P.close();
  gw.redraw();

  if (but == 0) return; 

  gw.agd_host = host;
  gw.agd_port = port;
  gw.agd_input_format = in_form;
//gw.agd_output_format = out_form;
  gw.agd_option_diag = optdiag;

  gw.init_agd_menu(gw.get_menu("Layout"),false);
}




void gw_animation_setup(GraphWin& gw) 
{
  int  anim_steps  = gw.get_animation_steps();
  //bool flush       = gw.get_flush();
  int max_items    = gw.get_max_move_items();
//int node_move    = (int) gw.get_animation_mode();

  panel P;
  gw.set_gwin(P);

  P.text_item("\\bf\\blue Animation");
  //P.bool_item("animation on/off",flush);
  P.int_item("animation steps",anim_steps,0,2*DefAnimationSteps);
  P.int_item("max moving items",max_items,0,2*DefMaxMoveItems);
  //P.text_item("To turn off animation set \\it animation steps = 0.");
  P.text_item("To turn off animation set \\it animation steps = 0.");
  P.text_item("");

//P.choice_item("mode",node_move,"move single"," move all");

  P.fbutton("apply",   2);
  P.button("defaults",1);
  P.button("cancel",  0);

  int but;

  while ((but = gw.open_panel(P,false)) == 1 )
  { //flush      = DefFlush;
    anim_steps = DefAnimationSteps;
    max_items  = DefMaxMoveItems;
   }

  if (but == 2) 
  { gw.set_animation_steps(anim_steps);
    //gw.set_flush(flush);
  //gw.set_animation_mode((node_move_t)node_move);
    gw.set_max_move_items(max_items);
   }

  gw.redraw();
}


void gw_zooming_setup(GraphWin& gw) 
{
  int  zoom_factor  = int(100 * gw.get_zoom_factor());
  bool zoom_objects = gw.get_zoom_objects();
  bool zoom_labels  = gw.get_zoom_labels();
  bool hide_labels  = gw.get_zoom_hide_labels();

  panel P;
  gw.set_gwin(P);

  P.text_item("\\bf\\blue Zooming");
  P.int_item ("zoom factor (percent)",zoom_factor,100,200);
  P.bool_item("change size of objects", zoom_objects);
  P.bool_item("change size of labels", zoom_labels);
  P.bool_item("hide labels during zoom", hide_labels);
  P.text_item("");

  P.fbutton("apply",   2);
  P.button("defaults",1);
  P.button("cancel",  0);

  int but;
  while ((but = gw.open_panel(P,false)) == 1 )
  { zoom_factor = int(100*DefZoomFactor);
    zoom_objects = DefZoomObjects;
    zoom_labels = DefZoomLabels;
    hide_labels = DefZoomHideLabels;
   }

  if (but == 2) 
  { gw.set_zoom_objects(zoom_objects);
    gw.set_zoom_labels(zoom_labels);
    gw.set_zoom_hide_labels(hide_labels);
    gw.set_zoom_factor(zoom_factor/100.0);
   }

  gw.redraw();
}


void gw_postscript_setup(GraphWin& gw) 
{
  string ps_fontname = gw.get_ps_fontname();

  int zoom_mode = int(-gw.ps_factor);

  panel P;
  gw.set_gwin(P);
  
  P.text_item("\\bf\\blue Postscript");
  P.string_item("font",ps_fontname,gw.get_ps_fontlist(),4);
  P.choice_item("zoom",zoom_mode,"window","graph","1:1");
  P.text_item("");

  P.fbutton("apply",   2);
  P.button("defaults",1);
  P.button("cancel",  0);

  int but;
  while ((but = gw.open_panel(P,false)) == 1 ) { 
    ps_fontname = DefPSFontName;
    zoom_mode = 0;
  }

  if (but == 2) { 
    gw.set_ps_fontname(ps_fontname);
    gw.ps_factor = -zoom_mode;
  }

}


void gw_svg_setup(GraphWin& gw) 
{
  int  edge_trans = int(100 * gw.svg_edge_transparency);
  bool whole_graph = gw.svg_whole_graph;

  panel P;
  gw.set_gwin(P);
  
  P.text_item("\\bf\\blue SVG Output");
  P.int_item("edge transparency",edge_trans,0,100);
  P.bool_item("whole graph",whole_graph);
  P.text_item("");

  P.fbutton("apply",   2);
  P.button("defaults",1);
  P.button("cancel",  0);

  int but;
  while ((but = gw.open_panel(P,false)) == 1 ) { 
    edge_trans  = 0;
    whole_graph = true;
  }

  if (but == 2) { 
    gw.svg_edge_transparency = edge_trans/100.0;
    gw.svg_whole_graph = whole_graph;
  }

}


LEDA_END_NAMESPACE
