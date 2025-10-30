/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  view_base.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/anim/view_base.h>
#include <LEDA/graphics/anim/view.h>

LEDA_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
// path
// ---------------------------------------------------------------------------

path::path() : P(new linear_motion), x(true) {}

path::path(const path& p) { *this = p; }
  
path& path::operator=(const path& p)
{ delete P; 
  P = p.P; 
  ((path&)p).x = false;
  return *this; 
}

path::~path() { } // if (x) delete P; }   
  
void path::get_path(point p0, point p1, int steps, list<point>& L)
{ P->get_path(p0,p1,steps,L); }
  
double path::get_path_length(point p0, point p1) 
{ return P->get_path_length(p0,p1); }
  
vector path::get_step(vector v, int steps, int i) 
{ return P->get_step(v,steps,i); }



//---------------------------------------------------------------------------
// base_element
// ---------------------------------------------------------------------------

base_element::base_element() { V = 0; item = 0; rank = 0; } 

base_element::base_element(const base_element& x) 
{ V = 0; item = 0; rank = x.rank; }

base_element& base_element::operator=(const base_element& x) 
{ V = 0; item = 0; rank = x.rank; 
  return *this;
} 

int base_element::set_rank(int x)  
{ if (x == rank) return rank;   

#if !defined(__HP_aCC)    
  swap(rank,x);  
#else
  leda_swap(rank,x);
#endif
  
  if (is_attached())
  { list<elem_item>& Elems = V->all_elements();
    Elems.sort();    
    list_item it;
    forall_items(it,Elems) Elems[it]->set_item(it);
  }
  
  return x;
}  

int base_element::get_rank() const { return rank; }  
  
void base_element::set_item(list_item it) { item = it; }

list_item base_element::get_item() const { return item; } 



//---------------------------------------------------------------------------
// view::redraw_handler
// ---------------------------------------------------------------------------

void view::redraw_handler::operator()() 
{ 
      window& W = V.get_window();
      
      W.start_buffering();
      W.clear();
      
      { elem_item it;
        forall(it,V.Elems) it->redraw();                        
      }      
      
      { list<action_item> L; 
        forall(L,V.Action) 
        { action_item it;
          forall(it,L) it->redraw();
        }
      }
      
      W.flush_buffer();
      W.stop_buffering();
 }

LEDA_END_NAMESPACE
