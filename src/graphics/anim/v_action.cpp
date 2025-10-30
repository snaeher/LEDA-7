/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  v_action.c
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
// class standard_view_menu
// --------------------------------------------------------------------------

standard_view_menu::standard_view_menu(view& v) : V(v)
{ M = new window;
  M->set_border_width(0);    
  
  menu* file_menu = new menu;
  //file_menu->button("Exit",exit);
  file_menu->button("Exit",100);
  
  menu* option_menu = new menu;
  option_menu->button("Delay...",200);
  
  M->button("File",*file_menu);
  M->button("Options",*option_menu); 
  M->button("done",300);  
  M->make_menu_bar(2);
}

void standard_view_menu::redraw()
{ M->display(V.get_window(),-1,-1); 
  M->resize(0,0,V.get_window().width()-1,28);
}
  
int standard_view_menu::handle_event(int event, int but, double x, double y)
{ if (event != button_press_event) return ACT_CONTINUE;    
 
  switch (but)
  { case 300 : return ACT_RET_TRUE;
    case 200 : 
    { panel P;
      int time = V.get_delay();
      P.int_item("Delay [msec]",time,0,100);
      P.button("Ok",0);
      P.button("Cancel",1);
      if (P.open(V.get_window(),window::center,window::center)) break;
      V.set_delay(time);
      break;
    }  
      
    case 100 : return ACT_RET_FALSE;
  };   
   
  return ACT_CONTINUE;
}

//---------------------------------------------------------------------------
// class standard_view_action
// ---------------------------------------------------------------------------

int standard_view_action::handle_event(int event, int but, double x, double y)
{ if (event != button_press_event) return ACT_CONTINUE;

  window& W = V.get_window();
  
  long MASK = 0;
  switch (but) 
  { case MOUSE_BUTTON(1) : MASK |= ACT_LEFT;   break;
    case MOUSE_BUTTON(2) : MASK |= ACT_MIDDLE; break;
    case MOUSE_BUTTON(3) : MASK |= ACT_RIGHT;  break;
    default : return ACT_CONTINUE;
  } 

  if (W.shift_key_down())  MASK |= ACT_SHIFT; 
  if (W.ctrl_key_down())   MASK |= ACT_CTRL; 
  if (W.alt_key_down())    MASK |= ACT_ALT; 

  bool dclick = false;
  bool drag = false;
  
  if (but == MOUSE_BUTTON(1) || but == MOUSE_BUTTON(2) || but == MOUSE_BUTTON(3))
    W.read_mouse(x,y,300,300,dclick,drag);
  
  if (drag)   MASK |= ACT_DRAG;
  if (dclick) MASK |= ACT_DOUBLE;
  
  switch (MASK)
  { case ACT_LEFT | ACT_DRAG : 
    {
      W.scroll_window(point(x,y),0); 
      break;  
    }
    
    case ACT_RIGHT | ACT_DRAG : 
    { double dy; int val;      
      while (W.read_event(val,x,dy) != button_release_event);              
      y  = W.real_to_pix(y); 
      dy = W.real_to_pix(dy);            
      W.zoom(1 + (dy - y) / (W.height() * 2));      
      break;
    }
  };
  
  return ACT_CONTINUE;
}


//---------------------------------------------------------------------------
// class view
// --------------------------------------------------------------------------

bool view::wait()
{ while (1)
  { window* wp;
    double x,y;    
    int but = -MAXINT;       
    int event = read_event(wp,but,x,y);
  
    list<action_item>& L = Action[wp];
    
    action_item it;
    forall(it,L)
    { switch (it->handle_event(event,but,x,y))
      { case base_action::ACT_RET_TRUE  : return true;
        case base_action::ACT_RET_FALSE : return false;
      };
    }
  }  
  
  return false;
}


bool view::wait(int time)
{ int runs = int(time / listen_time);
  
  while (runs-- > 0)
    if (!listen()) return false;

  return true;
}

bool view::listen()
{ window* wp;
  double x,y;    
  int but = -MAXINT;       
  int event = get_event(wp,but,x,y);

  list<action_item>& L = Action[wp];
    
  action_item it;
  forall(it,L) 
  { switch (it->handle_event(event,but,x,y))
    { case base_action::ACT_RET_TRUE  : return true;
      case base_action::ACT_RET_FALSE : return false;
    };
  }

  return true;
}

void view::set_listen_time()
{ int i;
  
  float T = used_time();  
  for (i = 1; i < 10000; i++) listen();
  float T1 = used_time(T);  

  T = used_time();  
  for (i = 1; i < 10000; i++) listen();
  float T2 = used_time(T);  
  
  listen_time = (T1 < T2) ? T2 / 10 : T1 / 10;
  if (listen_time == 0) listen_time++;
}


void view::add_action(action_item x)
{ Action[x->get_window()].append(x); 
  set_listen_time();
}

void view::del_action(action_item x)
{ list<action_item>& L = Action[x->get_window()];
  
  list_item it;
  forall_items(it,L)
  { if (L[it] == x) 
    { L.del(it); 
      break; 
    }
  }  
  
  set_listen_time();
}

LEDA_END_NAMESPACE
