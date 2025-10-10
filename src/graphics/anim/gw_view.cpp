/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_view.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/anim/gw_view.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE


double gw_element::pix_to_real(double pix) const
{ if (gwp) 
     return gwp->get_window().pix_to_real((int)pix);
  else
     return pix/40;
}


gw_element::gw_element(double x, int w, int h, color c_color, 
                                               color v_color,
                                               gw_node_shape c_shape,
                                               gw_node_shape v_shape)
{ gwp = 0;
  width = w;
  height = h;
  cell_shape = c_shape;
  value_shape = v_shape;
  cell_color = c_color;
  value_color = v_color;
  cell_node = nil;
  value_node = nil;
  value_edge = nil;
  val = x;
  pos = point(0,0);
 }

gw_element::gw_element(const gw_element* target, int w, int h, color c_color, 
                                                       color v_color,
                                                       gw_node_shape c_shape)
{ gwp = 0;
  width = w;
  height = h;
  cell_shape = c_shape;
  cell_color = c_color;
  value_color = v_color;
  cell_node = nil;
  value_node = nil;
  if (target) value_node = target->get_cell_node();
  val = 0;
  pos = point(0,0);
 }
  

void gw_element::attach(GraphWin& gw)
{ gwp = &gw;
  
  cell_node = gw.new_node(get_position());
  gw.set_shape(cell_node,cell_shape);
  gw.set_width(cell_node,width);
  gw.set_height(cell_node,height);
  
  if (value_node)  // pointer
  { point pos  = gw.get_position(value_node);
    node u = gw.new_node(pos);
    gw.set_shape(u,gw.get_shape(value_node));
    gw.set_width(u,gw.get_width(value_node));
    gw.set_height(u,gw.get_height(value_node));
    gw.set_color(u,invisible);
    gw.set_border_color(u,invisible);
    value_node = u;
    value_edge = gw.new_edge(cell_node,value_node);
    gw.set_color(value_edge,value_color);
   }
  else
  { value_node = gw.new_node(value_position(pos,val));
    gw.set_shape(value_node,value_shape);
    gw.set_width(value_node,width-2);
    gw.set_height(value_node,height-2);
    gw.set_color(value_node,value_color);
    if (value_color == invisible)
    { gw.set_border_color(value_node,invisible);
      gw.set_label_color(value_node,invisible);
    }
  }

  visualize_cell_color();

  if (cell_color == invisible)
  { gw.set_color(cell_node,invisible);
    gw.set_border_color(cell_node,invisible);
   }
  if (value_color != invisible) visualize_value();
}

  

void gw_element::detach()
{ if (gwp)
  { if (value_edge) gwp->del_edge(value_edge);
    gwp->del_node(cell_node);
    gwp->del_node(value_node);
   }
  gwp = 0;
  cell_node = 0;
  value_node = 0;
  value_edge = 0;
}

bool gw_element::is_attached() const { return gwp != 0; }


double gw_element::set_value(double x) 
{ double v_old = val;
  val = x; 
  if (gwp) 
  { if (value_color != invisible) visualize_value();
    gwp->set_position(value_node,value_position(pos,val));
   }
  return v_old;
}
  

color gw_element::set_cell_color(color col) 
{ color c_old = cell_color;
  cell_color = col; 
  if (gwp) 
  { if (col == invisible)
    { gwp->set_color(cell_node,invisible);
      gwp->set_border_color(cell_node,invisible);
     }
    else visualize_cell_color();
   }
  return c_old;
}
  
color gw_element::set_value_color(color col) 
{ color c_old = value_color;
  value_color = col;
  if (gwp) 
  { if (value_edge)
       gwp->set_color(value_edge,col);
    else
    { gwp->set_color(value_node,col);
      if (col == invisible)
      { gwp->set_border_color(value_node,invisible);
        gwp->set_label_color(value_node,invisible);
      }
     }
  }
  return c_old;
 }
  
point gw_element::set_position(point p)  
{ point p_old = pos;
  pos = p;
  if (gwp) 
  { gwp->set_position(cell_node,p);
    if (!value_edge) gwp->set_position(value_node,value_position(pos,val));
  }
  return p_old;
}


double gw_element::get_value()       const { return val; }
color  gw_element::get_cell_color()  const { return cell_color; }
color  gw_element::get_value_color() const { return value_color; }
point  gw_element::get_position()    const { return pos;  }

point  gw_element::get_value_position()  const
{ return gwp->get_position(value_node); }

point  gw_element::set_value_position(point pos)
{ return gwp->set_position(value_node,pos); }

  

// animation

void gw_element::assign(double x)
{ val = x;
  if (gwp)
  { point p = gwp->get_position(value_node);
    double y = gwp->get_ymax() + (gwp->get_ymax() - gwp->get_ymin())/2;
    point q(p.xcoord(),y);
    if (value_color != invisible) visualize_value();
    gwp->set_position(value_node,q);
    gwp->move_nodes_set_position(value_node,value_position(pos,val));
   }
}


void gw_element::assign(gw_element& el, motion_base* m)
{ 
  assert(value_node);

  if (gwp && value_edge)
  { gwp->move_nodes_set_position(value_node,gwp->get_position(el.cell_node));
    return;
   }

  val = el.val;
  if (gwp)
  { if (value_color != invisible) visualize_value();
    gwp->set_position(value_node,el.value_position(el.pos,el.val));
    gwp->move_nodes_set_position(value_node,value_position(pos,val));
    gwp->set_motion(value_node,m);
   }
}



void gw_element::swap(gw_element& el, motion_base* m1, motion_base* m2)
{
 leda_swap(val,el.val);
 leda_swap(value_node,el.value_node);
 if (gwp)
 { gwp->move_nodes_set_position(value_node,value_position(pos,val));
   gwp->move_nodes_set_position(el.value_node,el.value_position(el.pos,el.val));
   gwp->set_motion(value_node,m1);
   gwp->set_motion(el.value_node,m2);
  }
}


void gw_element::move(point p, motion_base* m)
{ pos = p;
  if (gwp)
  { gwp->move_nodes_set_position(cell_node,p);
    gwp->move_nodes_set_position(value_node,value_position(p,val));
    gwp->set_motion(value_node,m);
   }
}







gw_view::gw_view(int w, int h, string label) : gw(w,h,label),steps_togo(0),do_anim(true)
{ //gw.win_init(0,w,0);
  gw.win_init(0,0.75*w,0);
  //gw.set_default_menu(M_FILE | M_HELP | M_DONE);
  gw.set_show_status(false);
  gw.set_node_label_type(user_label);
  gw.set_node_shape(rectangle_node);
  gw.set_animation_steps(64);
  gw.set_ortho_mode(true);
  gw.set_ortho_first_dir(1);
  gw.set_flush(false);
}


void gw_view::scene_begin(int steps) 
{ if (do_anim)
  { if (steps > 0) gw.set_animation_steps(steps);
    gw.move_nodes_begin(); 
   }
}

int  gw_view::scene_end() 
{ if (do_anim)
  { gw.redraw(); 
    steps_togo = gw.move_nodes_end(); 
    return steps_togo;
   }
  else return 0;
}

void  gw_view::scene_step(int n)
{ if (do_anim)
  { while (n--) 
    { steps_togo --;
      if (steps_togo > 0) gw.move_nodes_step(); 
     }
    if (steps_togo < 0) steps_togo = 0;
   }
}

void  gw_view::scene_play() 
{ if (do_anim)
  { scene_step(steps_togo); 
    scene_finish(); 
   }
}


void  gw_view::scene_finish()
{ if (do_anim)
  { gw.move_nodes_finish(); 
    steps_togo = 0;
   }
}


LEDA_END_NAMESPACE


