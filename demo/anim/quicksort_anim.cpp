/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  quicksort_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/pixmaps/button32.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/anim/TextLoader.h>

#include <LEDA/core/stack.h>
#include <LEDA/core/tuple.h>
#include <LEDA/core/array.h>

#include <stdlib.h>

using namespace leda;


enum direction_t { LEFT,RIGHT };

enum item_t {
  grid_item     = (1<<0),
  pointer_item  = (1<<1),
  pivot_item    = (1<<2),
  active_item   = (1<<3),
  inactive_item = (1<<4),
  left_item     = (1<<5),
  right_item    = (1<<6)
};

enum { MIN_SPEED=1,MAX_SPEED=100 };

enum { RANDOMIZE,UP,DOWN,CONSTANT }; 
 
enum { 
  CREATE   = 10,
  CANCEL   = 11,
  START    = 12,
  SETUP    = 13,    
  INFO     = 14,
  DONE     = 15,
  MORE     = 16,
  CONTINUE = 17,
  STOP     = 18
};

enum { MIN=3,MID=10,MAX=15 };

class view {

  stack< two_tuple<int,int> >* S; // Rekursionsintervall
  node_array<point>* P;           // Positionen der Knoten 
  GRAPH<int,int>* G;              // max Knoten + Center Knoten + Pivot Knoten
  array<node>* A;                 // Feld
  array<int>* C;                  // Zeiger auf den Client
  array<int>* C0;  
  GraphWin* GW;                 
 
  int    left;                    // Index des Knotens vom linken Zeiger
  int    right;                   // Index des Knotens vom rechten Zeiger
  int    steps;                   // Animationsschritte + Delay
  int    size;                    // aktuelle Groesse des Feldes
  int    def_rec_level;           // Rekursionstiefe des Gitters
  int    max_rec_level;           // maximale Rekursionstiefe
  int    cur_rec_level;           // aktuelle Rekursionstiefe 
  
  double node_sz;                 // Knotengroesse
  double time;                    // time := 0.017*steps;              
  double xw0;
  double yw0;
  double xw1;
  double yw1;
 
  bool   grided;                 // Gitter sichtbar?
  bool   started;
  bool   stopped;
  bool   zoomed;                 
  
  color  grid_c;                  // Farbe des Gitters 
  color  pivot_c;                 // Farbe des Pivot 
  color  active_c;                // Standardfarbe aktiver Schluessel
  color  inactive_c;              // Farber nicht betrachteter Schluessel
  color  left_c;                  // Farbe der linken Schluessel
  color  right_c;                 // Farbe der rechten Schluessel
  color  pointer_c;               // Farbe der Zeiger   

  typedef int(*F0)(window*,int,int,double,double,unsigned long);
  
  void wait();
  void line_pos();              
  void circle_pos(int,int);
  
  inline void default_edge_params(direction_t);

  static void redraw_func(window*,double,double,double,double);

  public:
    
  view(array<int>&,int); 
 ~view();
 
  void display(int,int);
  
  void  set_zooming(bool);
  bool  zooming() const { return zoomed; }
  
  void  set_animation_steps(int);
  int   get_animation_steps() const { return 100-GW->get_animation_steps(); }
 
  void  grid_visible(bool);
  bool  grid_visible() const { return grided; }

  void  set_color(color,item_t);
  color get_color(item_t) const;
  
  void  set_event_handler(F0); 
  void  get_right_upper_border(int&,int&);
   
  void run();
  void init();
  void finish();
  void stop();
  void set_pointer(int,direction_t);
  void mark_key(int,direction_t);
  void recursions_call(int,int);
  void swap_pivot(int,int);
  void return_recursion();
  void choose_pivot(int);
  void swap(int,int);
};

inline void view::default_edge_params(direction_t d)
{ edge e;
  if(d==LEFT && left>-1) {
    e=G->first_adj_edge(A->get(left));
    GW->set_color(e,grid_c);
    GW->set_width(e,grided? 0:1);
    left=-1;
    return;
  } 
  
  if(d!=RIGHT || right<0) return;
  
  e=G->first_adj_edge(A->get(right));
  GW->set_color(e,grid_c);
  GW->set_width(e,grided? 0:1);
  right=-1;     
}

view::view(array<int>&C0,int m):C(&C0)
{ S=new stack< two_tuple<int,int> >;
  A=new array<node>(C->low(),C->low()+m);
  P=new node_array<point>;
  G=new GRAPH<int,int>;
  
  GW=new GraphWin(*G,650,400,"Quicksort Animation");
  
  stopped = false;
  started = false;  
  grided  = true;
  zoomed  = true;
  
  grid_c     = grey2;   // Edge Color = Grid Color
  left_c     = green2;
  right_c    = blue2;
  pointer_c  = black;
  active_c   = ivory;
  inactive_c = grey1;
  pivot_c    = red;
  pivot_c.set_rgb(255,100,100); 
  
  def_rec_level = 7;
  max_rec_level = def_rec_level;
  cur_rec_level = def_rec_level;
  
  set_animation_steps(50);
  
  GW->set_bg_redraw(view::redraw_func);
  GW->set_show_status(false);
  GW->set_default_menu(0);
  
  GW->set_edge_direction(undirected_edge);
  GW->set_edge_color(grid_c);
  GW->set_edge_width(0);
  
  GW->set_node_label_font(fixed_font,10);
  GW->set_node_label_type(data_label);
  GW->set_zoom_labels(true);
  GW->set_node_width(22);
    
  window* W=&GW->get_window();
  W->set_client_data(this,3);
  node_sz=GW->get_node_radius1()+W->pix_to_real(2);
   
  node v=G->new_node(0); //Center Knoten 
  int i;
	for(i=A->low();i<=A->high();i++) {
    node w=G->new_node(0);
    G->new_edge(w,v);
    A->set(i,w);
  }
  node w=G->new_node(0); //Pivot Knoten
  G->new_edge(w,v);      //Kante speichert den Index des Pivot Knotens
  
  GW->update_graph();
  
  GW->set_border_color(v,invisible);
  GW->set_label_type(v,no_label);
  GW->set_color(v,invisible);
 
  GW->set_border_color(w,invisible);
  GW->set_label_type(w,no_label);
  GW->set_color(w,invisible);
  
  line_pos();    
  GW->set_position(*P);
  size=C->size(); 
}

view::~view()
{ delete S;
  delete A;
  delete P;
  delete G;
  delete GW;
}

void view::set_zooming(bool b) { zoomed=b; }

void view::set_event_handler(F0 f)
{ window& W=GW->get_window();
  W.set_event_handler(f);
}

void view::set_animation_steps(int s)
{ if(s<1) return;
  GW->set_animation_steps(steps=101-s); 
  time=0.017*steps;
}

void view::set_color(color c,item_t it)
{ if(it&grid_item) grid_c=c;
  if(it&pivot_item) pivot_c=c;
  if(it&pointer_item) pointer_c=c;
  if(it&inactive_item) inactive_c=c;
  if(it&left_item) left_c=c;
  if(it&right_item) right_c=c; 
  if(it&active_item) {
    active_c=c;
    if(!started) {
      GW->set_flush(false);
      int i;
			for(i=C->low();i<=C->high();i++)
        GW->set_color(A->get(i),active_c=c);
      GW->set_flush(true);
    }
  }
  GW->redraw();
}

color view::get_color(item_t it) const
{ if(it&inactive_item) return inactive_c;
  if(it&pointer_item) return pointer_c;
  if(it&active_item) return active_c;
  if(it&pivot_item) return pivot_c;
  if(it&right_item) return right_c; 
  if(it&grid_item) return grid_c;
  if(it&left_item) return left_c;
  return invisible;
}

void view::grid_visible(bool b)
{ grided=b;
  if(grided) 
    GW->set_bg_redraw(view::redraw_func);
  GW->redraw();
}

void view::wait()
{ window& W=GW->get_window();
  unsigned long t=0;
  double x=0,y=0;
  int val=0;
  while(W.read_event(val,x,y,t,int(1000*time))!=no_event); 
}

void view::get_right_upper_border(int&x,int&y)
{ window& W=GW->get_window();
  x=W.xpos()+W.frame_width();
  y=W.ypos();
}

//------------------------------------------------------------------------------

void view::choose_pivot(int i)
{ if(!started) return;
  node u=A->get(i);
  node v=G->last_node();
  node w=G->first_node();
 
  edge e=G->first_adj_edge(v);
  G->assign(e,i);                      // speichert den Index Pivot Knotens
  G->assign(v,G->inf(u));              // speichert die Information des Pivot
 
  GW->set_flush(false);
  if(!grided)
    GW->set_width(G->first_adj_edge(v),1);
  GW->set_position(v,(*P)[v]=(*P)[u]); // speichert alte Position Pivot Knotens
  GW->set_label_type(v,data_label);
  GW->set_border_color(v,black); 
  GW->set_color(v,active_c);
  GW->set_flush(true);
  
  GW->set_position(u,(*P)[u]=(*P)[w]); // setzt den Pivot Knoten in die Mitte
  GW->set_color(u,pivot_c);

  GW->redraw();
  wait();
}

void view::swap_pivot(int i,int j)
{ if(!started) return;
  node u=G->last_node();
  int  k=G->inf(G->first_adj_edge(u));
  node v=A->get(k);  
   
  default_edge_params(LEFT);
  default_edge_params(RIGHT);  
  
  GW->set_animation_steps(steps*3);  
  GW->set_position(v,(*P)[v]=(*P)[u]);
  GW->set_animation_steps(steps);  
  
  GW->set_label_type(u,no_label);
  GW->set_border_color(u,invisible);
  GW->set_color(u,invisible);
  if(!grided)
    GW->set_width(G->first_adj_edge(u),0);
  
  GW->redraw();

  node w=A->get(k==i?j:i);
  if(w==v) {                                // es wird nichts vertauscht
    wait();
    return;
  }
   
  point tmp((*P)[v]);
  (*P)[v]=(*P)[w];
  (*P)[w]=tmp;

  A->swap(i,j);  

  GW->set_flush(false);
  GW->set_width(G->first_node(),1);
  GW->set_flush(true);

  set_pointer(k,LEFT);
  set_pointer(k==i?j:i,RIGHT);
  
  GW->set_animation_steps(steps*3);
  GW->set_position(*P);
  GW->set_animation_steps(steps);
  
  default_edge_params(LEFT);
  default_edge_params(RIGHT);  
  
  GW->redraw();
  wait();

  GW->set_flush(false);
  GW->set_width(G->first_node(),22);
  GW->set_flush(true);
}

void view::swap(int i,int j)
{ if(!started) return;
  node v=A->get(i);
  node w=A->get(j);
 
  point tmp((*P)[v]);
  (*P)[v]=(*P)[w];
  (*P)[w]=tmp;
 
  A->swap(i,j);
 
  GW->set_animation_steps(steps*3);
  GW->set_position(*P);
  GW->set_animation_steps(steps);
  
  GW->set_flush(false);
   
  GW->set_color(A->get(left),left_c);
  GW->set_color(A->get(right),right_c);
  
  GW->set_flush(true);
  GW->redraw();
}

void view::set_pointer(int i,direction_t d)
{ if(!started) return;
  GW->set_flush(false);

  edge e;
  if(d==LEFT) {
    default_edge_params(LEFT);
    e=G->first_adj_edge(A->get(left=i));
  } else {
    default_edge_params(RIGHT);
    e=G->first_adj_edge(A->get(right=i));
  }  
  GW->set_color(e,pointer_c);
  GW->set_width(e,2);  

  GW->set_flush(true);
  GW->redraw();
}


void view::mark_key(int i,direction_t d)
{ if(!started) return;
  GW->set_flush(false);
  
  edge e;
  if(d==LEFT) {       
    default_edge_params(LEFT);
     e=G->first_adj_edge(A->get(left=i));
    GW->set_color(A->get(left),left_c);
  } else {
    default_edge_params(RIGHT);
    e=G->first_adj_edge(A->get(right=i));
    GW->set_color(A->get(right),right_c);
  }  
  GW->set_color(e,pointer_c);
  GW->set_width(e,2);  

  GW->set_flush(true);
  GW->redraw();
  wait();
}

void view::recursions_call(int lo,int hi)
{ if(!started) return;
  S->push(two_tuple<int,int>(lo,hi));
  
  if(cur_rec_level<=S->size()) {
    max_rec_level=++cur_rec_level;
    double de=node_sz*2*(def_rec_level-cur_rec_level);
    if(zoomed)
      GW->zoom_area(xw0+de,yw0,xw1-de,yw1-de);
  }
  
  GW->set_flush(false);  

  int i;
	for(i=C->low();i<lo;i++)
    GW->set_color(A->get(i),inactive_c);
  for(i=hi+1;i<=C->high();i++)
    GW->set_color(A->get(i),inactive_c);    
  for(i=lo;i<=hi;i++)
    GW->set_color(A->get(i),active_c);
  
  GW->set_flush(true);

  circle_pos(lo,hi);
  GW->set_position(*P);
  GW->redraw();
  wait();
}

void view::return_recursion()
{ if(S->empty()||!started) return;
  
  two_tuple<int,int> t=S->pop();
  circle_pos(t.first(),t.second());
  
  GW->set_flush(false);
  
  int i;
	for(i=t.first();i<=t.second();i++)
    GW->set_color(A->get(i),active_c);
 
  GW->set_flush(true);
 
  GW->set_position(*P);
  GW->redraw();
  
  if(cur_rec_level>def_rec_level) {
    cur_rec_level--;    
    double de=node_sz*2*(def_rec_level-cur_rec_level);
    if(zoomed)
      GW->zoom_area(xw0+de,yw0,xw1-de,yw1-de);
  }
  
  wait();
}

void view::circle_pos(int lo,int hi)
{ double dphi=3.141/(C->size()-1);
  double phi=(C->high()-hi)*dphi;
  double ns=node_sz;
  double r=ns/sin(dphi/2)<6*ns? 6*ns:ns/sin(dphi/2); // 6*ns minimaler Radius
 
  r+=S->size()*2*ns;
 
  point p0((*P)[G->first_node()]);
 
  int i;
	for(i=hi;i>=lo;i--) {
    double x=p0.xcoord()+r*cos(phi);
    double y=p0.ycoord()+r*sin(phi);
    (*P)[A->get(i)]=point(x,y);
    phi+=dphi;
  }
}

void view::line_pos()
{ double ns=2*node_sz;
  double xc=(GW->get_xmax()-GW->get_xmin())/2-ns/2;
  double x0=xc-C->size()*ns/2+ns/2;
  double y0=GW->get_ymin(); 
 
  P->init(*G,point(xc,y0-ns));
 
  int i;
  for(i=C->low();i<=C->high();i++)
    (*P)[A->get(i)]=point(x0+ns*i,y0+ns);
 
  (*P)[G->first_node()]=point(xc,y0+ns);
}

void view::display(int x,int y)
{ window& W=GW->get_window();
  if(W.is_open()) return;
  
  GW->display(x,y);
    
  xw0 = GW->get_xmin();
  xw1 = GW->get_xmax();
  yw0 = GW->get_ymin();
  yw1 = GW->get_ymax();

  init();
}

void view::run()
{ if(started) return;
  cur_rec_level=def_rec_level;
  max_rec_level=def_rec_level;
  left=right=-1;
  
  started=true;
  stopped=false;
  
  circle_pos(C->low(),C->high());
  GW->set_position(*P);
 
  if(!grided) {
 
    GW->set_flush(false);
    GW->set_width(G->first_node(),22);
 
    int i;
		for(i=C->low();i<=C->high();i++) 
      GW->set_width(G->first_adj_edge(A->get(i)),1);
 
    GW->set_flush(true);  
  }
 
  GW->redraw();
  wait();
}

void view::finish() 
{ int i;
  if(stopped) {
    for(int i=C->low();i<=C->high();i++)
      C->set(i,C0->get(i));
    delete C0;
  }
  
  if(!started) return;
  GW->set_flush(false);
 
  for(i=C->low();i<=C->high();i++)
    GW->set_color(A->get(i),active_c);
 
  if(!grided) 
    for(i=C->low();i<=C->high();i++) 
      GW->set_width(G->first_adj_edge(A->get(i)),0);  
  
  GW->set_flush(true);
  
  line_pos(); 
  GW->set_position(*P);  
  GW->redraw();
  
  started=false;  
}

void view::stop()
{ if(!started) return;
  
  started=false;
  stopped=true;
  
  GW->set_flush(false);
  
  int i;
	for(i=C->low();i<=C->high();i++)
    GW->set_color(A->get(i),active_c);
 
  node u=G->last_node();
  GW->set_label_type(u,no_label);
  GW->set_border_color(u,invisible);
  GW->set_color(u,invisible);
  
  default_edge_params(RIGHT);
  default_edge_params(LEFT);
 
  C0=new array<int>(C->low(),C->high());
  for(i=C->low();i<=C->high();i++)
    C0->set(i,G->inf(A->get(i)));
  
  if(!grided) 
    for(i=C->low();i<=C->high();i++) 
      GW->set_width(G->first_adj_edge(A->get(i)),0);  
  
  S->clear();
  
  GW->set_flush(true);
  
  line_pos(); 
  GW->set_position(*P);  
  GW->redraw();
}

void view::init()
{ int i;
  for(i=C->low();i<=C->high();i++) 
    G->assign(A->get(i),C->get(i));

  GW->redraw();

  if(size==C->size()) return;  // Feldgroesse bleibt gleich

  line_pos();    
  GW->set_position(*P);
  size=C->size(); 
}

void view::redraw_func(window* wp,double,double,double,double)
{ view& V=*(view*)wp->get_client_data(3); 
  
  if(!V.grided) {
    V.GW->set_bg_redraw(0);
    wp->clear();
    return;
  }
  
  point p0((*V.P)[V.G->first_node()]);
  double x0=p0.xcoord();
  double y0=p0.ycoord();
  double ns=V.node_sz;
  double dphi=3.141/(V.C->size()-1),phi=0;
  double r=ns/sin(dphi/2)<6*ns? 6*ns:ns/sin(dphi/2); // 6*ns minimaler Radius
  double dr=2*V.node_sz;        
  
  int i;
  for(i=0;i<V.max_rec_level;i++) {
    point o(x0-r,y0);
    point p(x0,y0+r);
    point q(x0+r,y0);
    wp->draw_arc(o,p,q,V.grid_c);
    r+=dr;
  }
  
  for(i=0;i<V.C->size();i++) {
    double x1=x0+(r-dr)*cos(phi);
    double y1=y0+(r-dr)*sin(phi);
     wp->draw_segment(x0,y0,x1,y1,V.grid_c);
    phi+=dphi;
  }
}

// -- end view ----------------------------------------------------------------
 
static void qsort(view& V,array<int>&A,int lo,int hi)
{ if( lo>=hi ) return;  
  int l=lo;
  int r=hi-1;  
  int p=hi;
 
  V.choose_pivot(p);       
  V.set_pointer(l,LEFT);
  V.set_pointer(r,RIGHT);

  for(;;) {
  
    while(A.get(l)<A.get(p)) 
      V.mark_key(l++,LEFT);
    V.set_pointer(l,LEFT);

    while(r>l&&A.get(r)>=A.get(p)) 
      V.mark_key(r--,RIGHT);
    
    if(A.get(r)>=A.get(p))    // rechters Element ist das Kleinste 
      V.mark_key(r,RIGHT);    // muss deshalb richtig hervorgehoben werden
    else
      V.set_pointer(r,RIGHT);
    
    if(l>=r) break; 
    V.swap(l,r);
    A.swap(l,r);
  }
  
  V.swap_pivot(p,l);
  A.swap(p,l);
  
  if(lo<l-1) {
    V.recursions_call(lo,l-1);
    qsort(V,A,lo,l-1);
    V.return_recursion();
  }
  
  if(l+1<hi) {
    V.recursions_call(l+1,hi);
    qsort(V,A,l+1,hi);
    V.return_recursion();
  }
}

static void init(array<int>& A,view& V,int n,int SortMode)
{ if(n!=A.size()) 
     A.resize(0,n-1);

  int i;
  switch(SortMode) {
    case UP  : 
      for(i=A.low();i<=A.high();i++) 
        A.set(i,i); 
      break;
    case DOWN: 
      for(i=A.low();i<=A.high();i++) 
        A.set(i,n-1-i); 
      break;
     case RANDOMIZE:
      for(i=A.low();i<=A.high();i++) 
        A[i]=rand_int(0,50);
      break;
    case CONSTANT:
      A.init(4);
      break;
  }; 
  V.init();
}

#define SET_COLOR \
  V.set_color(gi_c,grid_item); \
  V.set_color(pi_c,pivot_item); \
  V.set_color(ac_c,active_item); \
  V.set_color(ia_c,inactive_item); \
  V.set_color(le_c,left_item); \
  V.set_color(ri_c,right_item); \
  V.set_color(po_c,pointer_item); 

static void setup2(view& V,int x,int y)
{ int st=V.get_animation_steps(),st0=st;
  
  bool  zo_b=V.zooming(),zo_b0=zo_b;
  bool  gi_b=V.grid_visible(),gi_b0=gi_b;
  color gi_c=V.get_color(grid_item),gi_c0=gi_c;
  color pi_c=V.get_color(pivot_item),pi_c0=pi_c;
  color ac_c=V.get_color(active_item),ac_c0=ac_c;
  color ia_c=V.get_color(inactive_item),ia_c0=ia_c;
  color le_c=V.get_color(left_item),le_c0=le_c;
  color ri_c=V.get_color(right_item),ri_c0=ri_c;
  color po_c=V.get_color(pointer_item),po_c0=po_c;  

  panel P("Preferences");
  P.int_item("Speed",st,MIN_SPEED,MAX_SPEED);
  P.text_item("\\bf\\blue Grid");
  P.bool_item("Visible",gi_b);
  P.bool_item("Zooming",zo_b); 
  P.color_item("Color",gi_c);
  
  P.text_item("\\bf\\blue Items Color");
  P.color_item("Lesser Itmes",le_c);
  P.color_item("Graeter Itmes",ri_c);
  P.color_item("Pivot Item",pi_c);
  P.color_item("Active Itmes",ac_c);
  P.color_item("Inactive Itmes",ia_c);
  
  P.text_item("\\bf\\blue Pointer");
  P.color_item("Color",po_c);
  
   P.fbutton("&Done",DONE);
  P.button("&Create",CREATE);
  P.button("C&ancel",CANCEL);
  P.display(x,y);
  while(1)
    switch(P.read()) {
      case DONE: {
        SET_COLOR;
        V.set_animation_steps(st);
        V.set_zooming(zo_b);
        V.grid_visible(gi_b);
        P.close();
        return;
      }
      case CANCEL: {
         V.set_color(gi_c0,grid_item); 
        V.set_color(pi_c0,pivot_item); 
        V.set_color(ac_c0,active_item); 
        V.set_color(ia_c0,inactive_item); 
        V.set_color(le_c0,left_item); 
        V.set_color(ri_c0,right_item); 
        V.set_color(po_c0,pointer_item); 
        V.set_animation_steps(st0);
        V.set_zooming(zo_b0);
        V.grid_visible(gi_b0);
        P.close();
        return;
      }
      case CREATE: {
        SET_COLOR;
        V.set_animation_steps(st);
        V.grid_visible(gi_b);
        break;
       }
    };    
}

#undef SET_COLOR

static int setup3(window* wp,int e,int,double,double,unsigned long)
{ if(e!=button_press_event) return e; 
  
  panel P("Pause");
  P.buttons_per_line(3);
  P.set_item_space(2);
  P.make_menu_bar(-1);
    
  char* co=P.create_pixrect(continue_xpm);
  char* se=P.create_pixrect(pencils_xpm);
  char* st=P.create_pixrect(stop_xpm);

  P.button(co,co,"Continue",CONTINUE);
  P.button(se,se,"Setup",SETUP);
  P.button(st,st,"Abort",STOP);
  
  view& V=*(view*)wp->get_client_data(3);
  
  int x,y;
  V.get_right_upper_border(x,y);
  
  while(1) {
    switch(P.open(x+10,y)) {
      case CONTINUE: return no_event;
      case SETUP: setup2(V,x+10,y); break;
      case STOP: V.stop(); return no_event;
    } 
  }

  return no_event;
}

static void start(array<int>& A,view& V)
{ V.run();
   V.set_event_handler(setup3);
  qsort(V,A,A.low(),A.high());
  V.set_event_handler(0);
  V.finish();
}

static void setup(array<int>& A,view& V,int& SortMode,int x,int y)
{ int  ord=SortMode;
  int  sz=A.size();
   
  panel P("Preferences");
  P.int_item("Size",sz,MIN,MAX);
 
  P.choice_item("Order",ord,"random","up","down","const");
  P.fbutton("&Done",DONE);
  P.button("&Create",CREATE);
  P.button("&More...",MORE);
  P.button("C&ancel",CANCEL);
 
  array<int> A0(A);  
  P.display(x,y);
  while(1) 
    switch(P.read()) {
      case DONE:  {
        SortMode=ord;
        P.close();
        return;
      }
      case CANCEL: {
        A=A0;
        V.init();
        return;
      }
      case CREATE: { 
        init(A,V,sz,ord); 
        break;      
      }
      case MORE: {
        P.close();
        setup2(V,x,y);
        P.display(x,y);
        break;
      }
    }  
}

static void info(int x,int y)
{ panel P("About");
  P.text_item("~~~This animation demonstrates how");
  P.text_item("");
  P.text_item("~~~the algorithm works, which solves");
  P.text_item("");
  P.text_item("~~~the sort problem, by using the");
  P.text_item("");
  P.text_item("~~\\blue Quicksort Algorithm.");
  P.text_item("");
  P.text_item("~~~The idea based upon yFiles");
  P.text_item("");
  P.text_item("~~~by Roland Wiese.");
  P.text_item("~~~~~~~~~~\\tt 09/2000");  
  P.fbutton("&Done",DONE);
  P.display(x,y);
  while(P.read()!=DONE);
}

static void exit_func(int) { exit(0); }

int main()
{ array<int> A(MID);
  view V(A,MAX);
  
  int SortMode=RANDOMIZE; 
  init(A,V,A.size(),SortMode);
  
  V.display(150,window::center);

  panel P("Main Panel");
  P.buttons_per_line(4);
  P.set_item_space(2);
  P.make_menu_bar(-1);
    
  char* st=P.create_pixrect(run_xpm);
  char* se=P.create_pixrect(pencils_xpm);
  char* in=P.create_pixrect(info_xpm);
  char* ex=P.create_pixrect(exit_xpm);

  P.button(st,st,"Start",START);
  P.button(se,se,"Setup",SETUP);
  P.button(in,in,"Info",INFO);
  P.button(ex,ex,"Exit",exit_func);   
  
  int x,y;
  while(1) {
    V.get_right_upper_border(x,y);
    switch(P.open(x+10,y)) {
      case START : start(A,V); break;
      case INFO  : info(x+10,y); break;
      case SETUP : setup(A,V,SortMode,x+10,y); break;
    };
  }
  return 0;
}
