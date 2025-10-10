/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  element_view.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>

using namespace leda;


typedef int value_t;


class element_struct {

 node cell_node;
 node value_node;
 list_item lpos;
 value_t  val;

public:

 element_struct(node u, node v) : cell_node(u), value_node(v) {}

 friend class element_view;
};


typedef element_struct* element;


class element_view
{

  GraphWin gw;

  list<element>      all_elements;
  node_array<point>  new_pos;
  node_array<vector> trans;

  int value_dx; 
  int value_dy;

  point value_position(double x, double y, value_t val)
  { double d = gw.get_window().pix_to_real(3+val/2);
    return point(x,y+d);
  }


public:

 element_view(string label);
 element_view(int width, int height, string label);
~element_view(); 


void win_init(double x1, double x2, double y1);
void display(int x, int y);
void display();
void draw();
bool edit();

void message(string msg) { gw.message(msg); }

// global attributes

void set_color(color col);
void set_width(int w);
void set_height(int h);
void set_value_offset(int dx, int dy);

void set_bg_redraw(void (*f)(window*,double,double,double,double))
{ gw.set_bg_redraw(f); }


element new_element(double x, double y, value_t val);
void    del_element(element el);

void    get_position(element el, double& x, double& y);
value_t get_value(element el);

color get_cell_color(element el);
color get_value_color(element el);
void  set_cell_color(element el, color col);
void  set_value_color(element el, color col);

void animation_start();
int  animation_end();
void animation_step(int n=1);
void animation_finish();

void set_position(element el, double x, double y);
void translate(element el, double dx, double dy);

void assign(element el, value_t val);
void assign(element el1, element el2);
void swap(element el1, element el2);

};



element_view::element_view(string label) : gw(label)
{ gw.set_default_menu(M_FILE | M_HELP | M_DONE);
  gw.set_show_status(false);
  gw.set_node_label_type(user_label);
  gw.set_node_shape(rectangle_node);
  gw.set_animation_steps(48);
  gw.set_flush(false);
}

element_view::element_view(int w, int h, string label) : gw(w,h,label)
{ gw.set_default_menu(M_FILE | M_HELP | M_DONE);
  gw.set_show_status(false);
  gw.set_node_label_type(user_label);
  gw.set_node_shape(rectangle_node);
  gw.set_animation_steps(48);
  gw.set_flush(false);
}

element_view::~element_view() 
{ while (!all_elements.empty()) 
      delete all_elements.pop(); 
}


void element_view::win_init(double x1, double x2, double y1) 
                                         { gw.win_init(x1,x2,y1); }

void element_view::display(int x, int y) { gw.display(x,y); }
void element_view::display() { gw.display(window::center,window::center); }

void element_view::draw()    { gw.redraw(); }
bool element_view::edit()    { return gw.edit(); }


void element_view::set_color(color col) { gw.set_node_color(col); }
void element_view::set_width(int w)  { gw.set_node_width(w); }
void element_view::set_height(int h) { gw.set_node_height(h); }

void element_view::set_value_offset(int dx, int dy) 
{ value_dx = dx;
  value_dy = dy;
}


element element_view::new_element(double x, double y, value_t val)
{ 
  if (val == 0) val = 1;

  node u = gw.new_node(point(x,y));
  gw.set_shape(u,rectangle_node);
  gw.set_width(u,16);
  gw.set_height(u,3);
  gw.set_color(u,blue2);

  node v = gw.new_node(value_position(x,y,val));
  gw.set_shape(u,rectangle_node);
  gw.set_width(v,14);
  gw.set_height(v,val);
  gw.set_color(v,ivory);

  element el = new element_struct(u,v);
  el->lpos = all_elements.append(el);
  el->val = val;
  return el;
}


void element_view::del_element(element el) 
{ gw.del_node(el->cell_node);
  if (el->value_node) gw.del_node(el->value_node);
  all_elements.del_item(el->lpos); 
  delete el;
}

void element_view::get_position(element el, double& x, double& y)
{ point p = gw.get_position(el->cell_node);
  x = p.xcoord();
  y = p.ycoord();
 }


value_t element_view::get_value(element el) { return el->val; }


void element_view::animation_start()
{ graph& G = gw.get_graph();
  new_pos.init(G);
  trans.init(G);
  node v;
  forall_nodes(v,G) new_pos[v] = gw.get_position(v);
}

int  element_view::animation_end()
{ gw.redraw();
  gw.move_nodes_start(new_pos,trans);
  return gw.get_animation_steps();
}

void  element_view::animation_step(int n)
{ while (n--) gw.move_nodes_step(trans); }

void  element_view::animation_finish()
{ gw.move_nodes_end(new_pos); }


void element_view::set_position(element el, double x, double y)
{ node v = el->cell_node;
  node w = el->value_node;
  new_pos[v] = point(x,y);
  new_pos[w] = value_position(x,y,el->val);
}

void element_view::translate(element el, double dx, double dy)
{ node v = el->cell_node;
  node w = el->value_node;
  double x,y;
  get_position(el,x,y);
  x += dx;
  y += dy;
  new_pos[v] = point(x,y);
  new_pos[w] = value_position(x,y,el->val);
}


void element_view::set_cell_color(element el, color col) 
{  gw.set_color(el->cell_node,col); }

void element_view::set_value_color(element el, color col) 
{  gw.set_color(el->value_node,col); }

color element_view::get_cell_color(element el) 
{ return gw.get_color(el->cell_node); }

color element_view::get_value_color(element el) 
{ return gw.get_color(el->value_node); }



void element_view::assign(element el, value_t val)
{ node v = el->value_node;
  double x,y;
  get_position(el,x,y);
  gw.set_height(v,val);
  el->val = val;
  gw.set_position(v,point(x,80));
  new_pos[v] = value_position(x,y,val);
}


void element_view::assign(element el1, element el2)
{ node v1 = el1->value_node;
  double x1,y1;
  get_position(el1,x1,y1);
  double x2,y2;
  get_position(el2,x2,y2);
  value_t val = el2->val;
  gw.set_height(v1,val);
  el1->val = val;
  gw.set_position(v1,value_position(x2,y2,val));
  new_pos[v1] = value_position(x1,y1,val);
}


void element_view::swap(element el1, element el2)
{ double x1,y1;
  get_position(el1,x1,y1);
  double x2,y2;
  get_position(el2,x2,y2);
  new_pos[el1->value_node] = value_position(x2,y2,el1->val);
  new_pos[el2->value_node] = value_position(x1,y1,el2->val);
  leda::swap(el1->value_node,el2->value_node);
  leda::swap(el1->val,el2->val);
}


static element_view* evp;
static element* elements;


static int color_table[] = 
{blue2, yellow, orange, brown, red, green, green2,cyan, blue, violet};

void swap(int* A,int i, int j)
{
  leda::swap(A[i],A[j]);
  evp->animation_start();
  evp->swap(elements[i],elements[j]);
  int n = evp->animation_end();
  evp->animation_step(n);
  evp->animation_finish();
}

void recursion_start(int l, int r, int level)
{ for(int i=l; i<=r; i++) evp->set_cell_color(elements[i],color_table[level]);
  evp->animation_start();
  for(int i=l; i<=r; i++) evp->translate(elements[i],-1.5,2);
  int n = evp->animation_end();
  evp->animation_step(n);
  evp->animation_finish();
}

void recursion_finish(int l, int r, int level)
{ evp->animation_start();
  for(int i=l; i<=r; i++) evp->translate(elements[i],1.5,-2);
  int n = evp->animation_end();
  evp->animation_step(n);
  evp->animation_finish();
  if (level > 0)
  { for(int i=l; i<=r; i++) 
      evp->set_cell_color(elements[i],color_table[level-1]);
    evp->draw();
   }
}


template<class T>
void quicksort(T* A, int l, int r, int depth)
{ 
  recursion_start(l,r,depth);

  int i = l;
  int j = r+1;

  evp->set_value_color(elements[l],grey2);

  T s = A[l];

  for(;;)
  { while (A[++i] < s);
    while (s < A[--j]);
    if (i<j) swap(A,i,j);
    else break;
   }

  swap(A,l,j); 

  if (l <= j-1) quicksort(A,l,j-1,depth+1);
  if (r >= j+1) quicksort(A,j+1,r,depth+1);

  recursion_finish(l,r,depth);
}


void bg_draw(window* wp, double, double, double, double)
{ int i;
  for(i=-8; i<=12; i++) 
  { if (i>0 && i<=10)
      wp->draw_hline(2*i,color_table[i-1]);
    else
      wp->draw_hline(2*i,grey1);
  }

  wp->draw_hline(0,grey3);

  for(i=-8; i<32; i++) 
  { point p(5*i,0);
    point q = p.translate(-18,24);
    wp->draw_ray(q,p,grey1);
  }
}


int main()
{
  int n = 16;

  element_view ev(600,450,"Stick View");
  ev.win_init(-10,5*n,-5);
  ev.set_bg_redraw(bg_draw);
  ev.display();

  evp = &ev;

  int* A = new int[n+1];
  elements = new element[n];

  for(int i=0;i<n;i++) 
  { A[i] = 0;
    elements[i] = ev.new_element(5*i,0,0);
   }

  A[n] = MAXINT;

  ev.draw();

/*
  for(int i=0;i<n;i++) 
  { int r = rand_int(1,50);
    A[i] = r;
    ev.animation_start();
    ev.assign(elements[i],5*r);
    int m = ev.animation_end();
    ev.animation_step(m);
    ev.animation_finish();
  }
*/

  ev.animation_start();
  for(int i=0;i<n;i++) 
  { int r = rand_int(1,50);
    A[i] = r;
    ev.assign(elements[i],5*r);
   }
  int m = ev.animation_end();
  ev.animation_step(m);
  ev.animation_finish();

  ev.message("click done to start animation");
  ev.edit();

  ev.message("");

  quicksort(A,0,n-1,0);

  ev.edit();

  delete[] A;
  delete[] elements;

  return 0;
}






