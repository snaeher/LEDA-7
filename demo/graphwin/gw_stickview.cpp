/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_stickview.c
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



class gw_element {

  GraphWin* gwp;

  value_t  val;

  double pos_x;
  double pos_y;

  node  cell_node;
  node  value_node;

  int width;
  int height;

  color cell_color;
  color value_color;

protected:

  node get_value_node() { return value_node; }
  node get_cell_node() { return cell_node; }

  double pix_to_real(double pix)
  { if (gwp) 
       return gwp->get_window().pix_to_real((int)pix);
    else
       return pix/40;
  }

  virtual point value_position(point p, value_t val)= 0;

  virtual void  show_value()= 0;
  virtual void  show_cell_color()= 0;

  point value_position() 
  { return value_position(point(pos_x,pos_y),val); }

public:

  GraphWin* get_graphwin() { return gwp; }

  gw_element(value_t x, int w, int h,color c_color, color v_color) 
  { gwp = 0;
    width = w;
    height = h;
    cell_color = c_color;
    value_color = v_color;
    cell_node = nil;
    value_node = nil;
    val = x;
    pos_x = 0;
    pos_y = 0;
  }

virtual ~gw_element() {} 
  

  // attach to a view

  void attach(GraphWin& gw)
  { gwp = &gw;
  
    cell_node = gw.new_node(get_position());
    gw.set_shape(cell_node,rectangle_node);
    gw.set_width(cell_node,width);
    gw.set_height(cell_node,height);
    show_cell_color();
  
    value_node = gw.new_node(value_position());
    gw.set_shape(value_node,rectangle_node);
    gw.set_width(value_node,width-2);
    gw.set_height(value_node,height-2);
    gw.set_color(value_node,value_color);
    show_value();
  
  }
  

  // remove from view

  void detach()
  { gwp->del_node(cell_node);
    gwp->del_node(value_node);
    gwp = 0;
    cell_node = 0;
    value_node = 0;
  }

  bool is_attached() { return gwp != 0; }

  // get
  
  value_t get_value()       const { return val; }
  color   get_cell_color()  const { return cell_color; }
  color   get_value_color() const { return value_color; }
  point   get_position()    const { return point(pos_x,pos_y);  }
  

  // set

  void set_value(value_t x) 
  { val = x; 
    if (gwp) 
    { show_value();
      gwp->set_position(value_node,value_position());
     }
  }
  
  void set_cell_color(color col) 
  { cell_color = col; 
    if (gwp) show_cell_color();
  }
  
  void set_value_color(color col) 
  { value_color = col;
    if (gwp) gwp->set_color(value_node,col);
   }
  
  void  set_position(double x, double y)  
  { pos_x = x; 
    pos_y = y; 
    if (gwp) 
    { gwp->set_position(cell_node,point(x,y));
      gwp->set_position(value_node,value_position());
     }
  }
  
  

  // animation

  void assign(value_t x)
  { val = x;
    if (gwp)
    { point p = gwp->get_position(value_node);
      double y = gwp->get_ymax() + (gwp->get_ymax() - gwp->get_ymin())/2;
      point q(p.xcoord(),y);
      show_value();
      gwp->set_position(value_node,q);
      gwp->move_nodes_set_position(value_node,value_position());
     }
  }
  
  
  void assign(gw_element& st)
  { val = st.val;
    if (gwp)
    { show_value();
      gwp->set_position(value_node,st.value_position());
      gwp->move_nodes_set_position(value_node,value_position());
     }
  }
  
  
  void swap(gw_element& st)
  { leda_swap(val,st.val);
    leda_swap(value_node,st.value_node);
    if (gwp)
    { gwp->move_nodes_set_position(value_node,value_position());
      gwp->move_nodes_set_position(st.value_node,st.value_position());
     }
  }
  
  
  void change_position(double x, double y)
  { pos_x = x;
    pos_y = y;
    if (gwp)
    { point p(x,y);
      gwp->move_nodes_set_position(cell_node,p);
      gwp->move_nodes_set_position(value_node,value_position(p,val));
     }
  }
  
  void translate(double dx, double dy)
  { pos_x += dx;
    pos_y += dy;
    if (gwp)
    { point p(pos_x,pos_y);
      gwp->move_nodes_set_position(cell_node,p);
      gwp->move_nodes_set_position(value_node,value_position(p,val));
     }
  }

};


class gw_stick : public gw_element {

point value_position(point p, value_t val)
{ double d = double(get_value())/2 + pix_to_real(4);
  return p.translate(0,d);
}

void show_value() 
{ get_graphwin()->set_radius2(get_value_node(),double(get_value())/2); }

void show_cell_color() 
{ get_graphwin()->set_color(get_cell_node(),get_cell_color()); }


public:
  gw_stick() : gw_element(0,16,3,grey2,ivory) {}
  

};


class gw_number_cell : public gw_element {

point value_position(point p, value_t val) { return p; }

void show_value() 
{ get_graphwin()->set_label(get_value_node(),string("%d",get_value())); }

void show_cell_color() 
{ get_graphwin()->set_color(get_cell_node(),invisible);
  get_graphwin()->set_border_color(get_cell_node(),get_cell_color()); 
}

public:
  gw_number_cell() : gw_element(0,24,24,black,ivory) {}
  

};




class gw_view
{

  GraphWin gw;

  int steps_togo;

public:

  gw_view(string label);
  gw_view(int w, int h, string label);

  void set_bg_redraw(void (*f)(window*,double,double,double,double))
  { gw.set_bg_redraw(f); }

  void win_init(double x1, double x2, double y1) { gw.win_init(x1,x2,y1); }
  void display(int x, int y) { gw.display(x,y); }
  void display()             { gw.display(); }
  void message(string msg)   { gw.message(msg); }
  void draw()                { gw.redraw(); }
  bool edit()                { return gw.edit(); }


  void add_element(gw_element& st) { st.attach(gw); }
  void del_element(gw_element& st) { st.detach(); }


  void scene_begin();
  int  scene_end();

  void scene_step(int=1);
  void scene_play();
  void scene_finish();

};


gw_view::gw_view(string label) : gw(label),steps_togo(0)
{ gw.set_default_menu(M_FILE | M_HELP | M_DONE);
  gw.set_show_status(false);
  gw.set_node_label_type(user_label);
  gw.set_node_shape(rectangle_node);
  gw.set_animation_steps(64);
  gw.set_flush(false);
}

gw_view::gw_view(int w, int h, string label) : gw(w,h,label),steps_togo(0)
{ gw.set_default_menu(M_FILE | M_HELP | M_DONE);
  gw.set_show_status(false);
  gw.set_node_label_type(user_label);
  gw.set_node_shape(rectangle_node);
  gw.set_animation_steps(64);
  gw.set_flush(false);
}


void gw_view::scene_begin() { gw.move_nodes_begin(); }

int  gw_view::scene_end() 
{ gw.redraw(); 
  steps_togo = gw.move_nodes_end(); 
  return steps_togo;
}

void  gw_view::scene_step(int n)
{ while (n--) gw.move_nodes_step(); 
  steps_togo -= n;
  if (steps_togo < 0) steps_togo = 0;
}

void  gw_view::scene_play() { scene_step(steps_togo); scene_finish(); }


void  gw_view::scene_finish()
{ gw.move_nodes_finish(); 
  steps_togo = 0;
}





// quicksort


static gw_view*    svp;
static gw_element* elem1;
static gw_element* elem2;



static int color_table[] = 
{blue2, yellow, orange, brown, red, green, green2,cyan, blue, violet};

void swap(int* A,int i, int j)
{
  leda_swap(A[i],A[j]);
  svp->scene_begin();
  elem1[i].swap(elem1[j]);
  elem2[i].swap(elem2[j]);
  int n = svp->scene_end();
  svp->scene_step(n);
  svp->scene_finish();
}

void recursion_start(int l, int r, int level)
{ for(int i=l; i<=r; i++)
  { elem1[i].set_cell_color(color_table[level]);
    elem2[i].set_cell_color(color_table[level]);
   }
  svp->draw();
/*
  svp->scene_begin();
  for(int i=l; i<=r; i++) 
  { elem1[i].translate(-1.5,2);
    elem2[i].translate(-1.5,2);
   }
  int n = svp->scene_end();
  svp->scene_step(n);
  svp->scene_finish();
*/
}

void recursion_finish(int l, int r, int level)
{ 
/*
  svp->scene_begin();
  for(int i=l; i<=r; i++) 
  { elem1[i].translate(1.5,-2);
    elem1[i].translate(1.5,-2);
   }
  int n = svp->scene_end();
  svp->scene_step(n);
  svp->scene_finish();
*/
  if (level > 0)
  { for(int i=l; i<=r; i++) 
    { elem2[i].set_cell_color(color_table[level-1]);
      elem2[i].set_cell_color(color_table[level-1]);
     }
    svp->draw();
   }
}


template<class T>
void quicksort(T* A, int l, int r, int depth)
{ 
  recursion_start(l,r,depth);

  int i = l;
  int j = r+1;

  elem1[l].set_value_color(grey2);
  elem2[l].set_value_color(grey2);

  T s = A[l];

  for(;;)
  { while (A[++i] < s) {}
    while (s < A[--j]) {}
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

  gw_view sv(600,500,"Stick View");
  sv.win_init(-10,6*n+10,-5);

  //sv.set_bg_redraw(bg_draw);

  sv.display();

  svp = &sv;

  int* A = new int[n+1];

  elem1 = new gw_stick[n];
  elem2 = new gw_number_cell[n];

  int i;
  for(i=0;i<n;i++) 
  { A[i] = 1;
    elem1[i].set_value(1);
    elem1[i].set_position(5*i,-2);
    sv.add_element(elem1[i]);
    elem2[i].set_value(1);
    elem2[i].set_position(5*(i+2),60);
    sv.add_element(elem2[i]);

   }

  A[n] = MAXINT;

  sv.draw();

  sv.scene_begin();
  for(i=0;i<n;i++) 
  { int r = rand_int(1,50);
    A[i] = r;
    elem1[i].assign(r);
    elem2[i].assign(r);
   }
  sv.scene_end();
  sv.scene_play();
/*
  int m = sv.scene_end();
  sv.scene_step(m);
  sv.scene_finish();
*/

  sv.message("click done to start animation");
  sv.edit();

  sv.message("");

  quicksort(A,0,n-1,0);

  sv.edit();

  delete[] A;

  delete[] elem1;
  delete[] elem2;

  return 0;
}






