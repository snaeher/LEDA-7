/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  v_base.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/anim/view.h>

LEDA_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
// class view
// ---------------------------------------------------------------------------

view::view(string label) : redraw_h(*this), W(label)
{ W.set_redraw(redraw_h); 
  W.init(0,W.width(),0);
    
  scale_factor = 1.0;  
  first_step   = false;
  max_steps    = 0;   
  sleep_time   = 3;
  
  Menu  = new standard_view_menu(*this);
  StdAction = new standard_view_action(*this);
  
  add_action(Menu);
  add_action(StdAction);
}

view::view(int w, int h, string label) : redraw_h(*this), W(w,h,label)
{ W.set_redraw(redraw_h); 
  W.init(0,w,0);
  
  scale_factor = 1.0;  
  first_step   = false;
  max_steps    = 0;   
  sleep_time   = 3;
  
  Menu = new standard_view_menu(*this);
  StdAction = new standard_view_action(*this);
  
  add_action(Menu);
  add_action(StdAction);
}

view::~view() 
{ while (!Elems.empty()) Elems.pop()->detach(*this);         
  if (Menu) delete Menu;
  if (StdAction) delete StdAction;
}

void view::win_init(double x0, double x1, double y0) 
{ scale_factor = W.xmax() / (x1 - x0);
  W.init(x0,x1,y0); 
}

void view::display() 
{ display(window::center,window::center); }

void view::display(int x, int y) 
{ W.display(x,y);   
  W.redraw();  
  set_listen_time();
}

void view::close() { W.close(); }

double view::xmin() const { return W.xmin(); }
double view::ymin() const { return W.ymin(); } 
double view::xmax() const { return W.xmax(); }
double view::ymax() const { return W.ymax(); }   

double view::get_scale_factor() const { return scale_factor; }

int view::set_delay(int time) 
{ leda_swap(sleep_time,time);
  return time;
}

int view::get_delay() const { return sleep_time; }

void view::add_element(base_element& e)
{ if (e.is_attached(*this)) return;  
  Elems.append(&e);    
  Elems.sort();      
  list_item x;
  forall_items(x,Elems) Elems[x]->set_item(x);     
  e.attach(*this);    

  if (W.is_open()) W.redraw();
}

void view::del_element(base_element& e)
{ if (!e.is_attached(*this)) return;  
  e.detach(*this);  
  Elems.del(e.get_item());   
  Elems.sort();      
  list_item x;
  forall_items(x,Elems) Elems[x]->set_item(x); 
  
  if (W.is_open()) W.redraw();
}

bool view::is_member(base_element& e) 
{ return e.is_attached(*this); }

LEDA_END_NAMESPACE
