/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  sweep_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/prio.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/geo/segment.h>
#include <LEDA/graphics/window.h>

#include <math.h>

using namespace leda;

using std::cout;
using std::endl;





/*
#include <LEDA/graph/graph.h>
*/



#define EPS  0.00001
#define EPS2 0.0000000001

#define pq_nil  pq_item(0)


window* Wp;

static bool trace = false;
static bool intera = false;
static color seg_color = blue;
static color node_color = red;


class SWEEP_POINT;
class SWEEP_SEGMENT;
typedef SWEEP_POINT* SWEEP_point;
typedef SWEEP_SEGMENT* SWEEP_segment;

enum SWEEP_point_type {Cross=0,Rightend=1,Leftend=2}; 


class SWEEP_POINT
{
  friend class SWEEP_SEGMENT;

  SWEEP_segment seg;
  int     kind;
  double    x,y;

  public:

  SWEEP_POINT(double a,double b)   { x=a; y=b; seg=0; kind=Cross;}

  SWEEP_POINT(point p)         { x=p.xcoord();y=p.ycoord();seg=0;kind=Cross;}

  friend double    get_x(SWEEP_point p)       { return p->x; }
  friend double    get_y(SWEEP_point p)       { return p->y; }
  friend int       get_kind(SWEEP_point p)    { return p->kind; }
  friend SWEEP_segment get_seg(SWEEP_point p) { return p->seg; }   

  friend bool intersection(SWEEP_segment, SWEEP_segment, SWEEP_point&);

  LEDA_MEMORY(SWEEP_POINT);

};



inline int compare(const SWEEP_point& p1, const SWEEP_point& p2)
{ if (p1==p2) return 0;

  double diffx = get_x(p1) - get_x(p2);
  if (fabs(diffx) > EPS2 ) return (diffx > 0.0) ? 1 : -1;

  int  diffk = get_kind(p1)-get_kind(p2);
  if (diffk != 0) return diffk;

  double diffy = get_y(p1) - get_y(p2);
  if (fabs(diffy) > EPS2 ) return (diffy > 0.0) ? 1 : -1;

  return 0;

}

void Print(SWEEP_point& p) { cout << string("(%f,%f)",get_x(p), get_y(p)); }




class SWEEP_SEGMENT
{
  SWEEP_point startpoint;
  SWEEP_point endpoint;
  double  slope;
  double  yshift;
  //node  left_node;
  int   orient;
  int   color;
  int   name;

  public:

  SWEEP_SEGMENT(SWEEP_point, SWEEP_point,int,int);     
 ~SWEEP_SEGMENT() { delete startpoint; delete endpoint; }     

  friend SWEEP_point get_startpoint(SWEEP_segment seg){ return seg->startpoint;}
  friend SWEEP_point get_endpoint(SWEEP_segment seg)  { return seg->endpoint; }
  friend double get_slope(SWEEP_segment seg)          { return seg->slope; }
  friend double get_yshift(SWEEP_segment seg)         { return seg->yshift; }
  friend int  get_orient(SWEEP_segment seg)           { return seg->orient; }
  friend int  get_color(SWEEP_segment seg)            { return seg->color; }
  friend int  get_name(SWEEP_segment seg)             { return seg->name; }

/*
  friend node get_left_node(SWEEP_segment seg)         { return seg->left_node;}
  friend void set_left_node(SWEEP_segment seg, node v) { seg->left_node = v; }
*/

  friend bool intersection(SWEEP_segment, SWEEP_segment, SWEEP_point&);

  LEDA_MEMORY(SWEEP_SEGMENT);


};



  SWEEP_SEGMENT::SWEEP_SEGMENT(SWEEP_point p1,SWEEP_point p2,int c, int n)    
  {
    //left_node  = nil;
    color      = c;
    name       = n;

    if (compare(p1,p2) < 0)
     { startpoint = p1; 
       endpoint = p2; 
       orient = 0;
      }
    else
     { startpoint = p2; 
       endpoint = p1; 
       orient = 1;
      }

    startpoint->kind = Leftend; 
    endpoint->kind = Rightend; 
    startpoint->seg = this; 
    endpoint->seg = this;

    if (endpoint->x != startpoint->x)
    {
      slope = (endpoint->y - startpoint->y)/(endpoint->x - startpoint->x);
      yshift = startpoint->y - slope * startpoint->x;

      startpoint->x -= EPS;
      startpoint->y -= EPS * slope;
      endpoint->x += EPS;
      endpoint->y += EPS * slope;
    }
    else //vertical segment
    { startpoint->y -= EPS;
      endpoint->y   += EPS;
     }
  }

void Print(SWEEP_segment& s) { cout << string("S%d",get_name(s)); }

static double x_sweep;
static double y_sweep;


#if defined(__aCC__)
template<>
#endif
int compare(const SWEEP_segment& s1,const SWEEP_segment& s2)
{
  double sl1 = get_slope(s1);
  double sl2 = get_slope(s2);
  double ys1 = get_yshift(s1);
  double ys2 = get_yshift(s2);

  double y1 = sl1*x_sweep+ys1;
  double y2 = sl2*x_sweep+ys2;

  double diff = y1-y2;

  if (fabs(diff) > EPS2) return (diff > 0.0) ? 1 : -1;

  if (sl1 == sl2) 
        return compare(get_x(get_startpoint(s1)), get_x(get_startpoint(s2)));

    if (y1 <= y_sweep+EPS2)
        return compare(sl1,sl2);
    else
        return compare(sl2,sl1);

}




static priority_queue<seq_item,SWEEP_point> X_structure;

static sortseq<SWEEP_segment,pq_item> Y_structure;


bool intersection(SWEEP_segment seg1,SWEEP_segment seg2, SWEEP_point& inter)
{
  if (seg1->slope == seg2->slope)
    return false;
  else
  {
    //x-coordinate  of the intersection
    double Cross_x = (seg2->yshift - seg1->yshift) / (seg1->slope - seg2->slope);
 
    if (Cross_x <= x_sweep) return false;

    double s1 = seg1->startpoint->x;
    double s2 = seg2->startpoint->x;
    double e1 = seg1->endpoint->x;
    double e2 = seg2->endpoint->x;

    if (s2>Cross_x || s1>Cross_x || Cross_x>e1 || Cross_x>e2) return false;

    //y-coordinate of the intersection
    double Cross_y = (seg1->slope * Cross_x + seg1->yshift);
    inter =  new SWEEP_POINT(Cross_x,Cross_y);

    return true;
  }
}

void message(string s, int line=1)
{ s += "                                                            ";
  double d = 20/Wp->scale();
  if (s.length() > 150) s = s.head(150);
  Wp->draw_text(Wp->xmin()+d, Wp->ymax()-d*(0.5+line),s);
 }

 
void draw_segment(SWEEP_segment seg)
{ double x1 = get_x(get_startpoint(seg));
  double y1 = get_y(get_startpoint(seg));
  double x2 = get_x(get_endpoint(seg));
  double y2 = get_y(get_endpoint(seg));

  double d = 10/Wp->scale();

  Wp->draw_text(x1-d,y1,string("%d",get_name(seg)));

  Wp->draw_segment(x1,y1,x2,y2,seg_color);
}
 



void draw_sweep_line(double xpos) 
{
  Wp->set_mode(xor_mode);
  Wp->draw_segment(xpos,-1150,xpos,Wp->ymax()-5*20/Wp->scale());
  Wp->set_mode(src_mode);
}


void move_sweep_line(double x, double xpos) 
{ 
  if (x == xpos) return;

  double delta = 1/Wp->scale(); 
  double y = Wp->ymax()-100*delta;

  Wp->set_mode(xor_mode);

  while (x < xpos)
  { Wp->draw_segment(x+delta,-1150,x+delta,y);
    Wp->draw_segment(x,-1150,x,y);
    x += delta;
   }
  
  Wp->draw_segment(x,-1150,x,y);
  Wp->draw_segment(xpos,-1150,xpos,y);

  Wp->set_mode(src_mode);
}



void draw_Ystructure()
{
  seq_item sit;
  string s = "Y_structure: ";

  forall_items(sit,Y_structure) 
    s+=string("%d ",get_name(Y_structure.key(sit)));

  message(s,3);

}

void draw_Xstructure()
{
  string s = "X_structure: ";

  priority_queue<seq_item,SWEEP_point> Q = X_structure;

  while (!Q.empty())
  { pq_item it = Q.find_min(); 

    SWEEP_point p = X_structure.inf(it);
    seq_item sit  = X_structure.key(it);

    Q.del_min();

     SWEEP_segment seg = get_seg(p);

     switch (get_kind(p)) {

     case Leftend:  s += string("L(%d) ",get_name(seg));
                    break;

     case Rightend: s += string("R(%d) ",get_name(seg)) ;
                    break;

     default:       s += string("X(%d) ",get_name(Y_structure.key(sit))); 
                    break;

     }

  }

  message(s,2);

}

pq_item Xinsert(seq_item i, SWEEP_point p) 
{ 

 if (trace)
 {
  SWEEP_segment s ;
  if (i!=nil) s = Y_structure.key(i);

  else s = get_seg(p);
    
     switch (get_kind(p)) {

     case Leftend:  message(string("Xinsert: L(%d) ",get_name(s)),4);
                    break;

     case Rightend: message(string("Xinsert: R(%d) ",get_name(s)),4) ;
                    break;

     default:       message(string("Xinsert: X(%d) ",get_name(s)),4); 
                    break;
      }

  Wp->set_mode(xor_mode);
  Wp->draw_filled_node(get_x(p),get_y(p),green);
  Wp->set_mode(src_mode);
 }
  return X_structure.insert(i,p);
}

SWEEP_point Xdelete(pq_item i) 
{
 if (trace)
 {SWEEP_point p = X_structure.inf(i);

  seq_item sit = X_structure.key(i);

  SWEEP_segment s ;

  if (sit!=nil) s = Y_structure.key(sit);
  else s = get_seg(p);
    
     switch (get_kind(p)) {

     case Leftend:  message(string("Xdelete: L(%d) ",get_name(s)),4);
                    break;

     case Rightend: message(string("Xdelete: R(%d) ",get_name(s)),4) ;
                    break;

     default:       message(string("Xdelete: X(%d) ",get_name(s)),4); 
                    break;
      }

  Wp->set_mode(xor_mode);
  Wp->draw_filled_node(get_x(p),get_y(p),green);
  Wp->set_mode(src_mode);
 }

  SWEEP_point p = X_structure.inf(i);

  X_structure.del_item(i);

  return p;
}



/*
node New_Node(GRAPH<point,int>& G,double x, double y )
{ return G.new_node(point(x,y)); }

void New_Edge(GRAPH<point,int>& G,node v, node w, SWEEP_segment l )
{ if (get_orient(l)==0)
       G.new_edge(v,w,get_color(l));
  else G.new_edge(w,v,get_color(l));
}
*/


void process_vertical_segment(/*GRAPH<point,int>& SUB, */ SWEEP_segment l)
{ 
  SWEEP_point p = 
              new SWEEP_POINT(get_x(get_startpoint(l)),get_y(get_startpoint(l)));
  SWEEP_point q = 
              new SWEEP_POINT(get_x(get_endpoint(l)),get_y(get_endpoint(l)));

  SWEEP_point r = new SWEEP_POINT(get_x(p)+1,get_y(p));
  SWEEP_point s = new SWEEP_POINT(get_x(q)+1,get_y(q));

  SWEEP_segment bot = new SWEEP_SEGMENT(p,r,0,0);
  SWEEP_segment top = new SWEEP_SEGMENT(q,s,0,0);

  seq_item bot_it = Y_structure.insert(bot,pq_nil);
  seq_item top_it = Y_structure.insert(top,pq_nil);
  seq_item sit;

  //node u,v,w;

  SWEEP_segment seg;
  

  for(sit=Y_structure.succ(bot_it); sit != top_it; sit=Y_structure.succ(sit))
  { seg = Y_structure.key(sit);

    double cross_y = (get_slope(seg) * get_x(p) + get_yshift(seg));

    Wp->draw_filled_node(get_x(p),cross_y,node_color);

/*
    v = get_left_node(seg);
    if (v==nil)
    { w = New_Node(SUB,get_x(p),cross_y);
      set_left_node(seg,w);
     }
    else
    { double vx = SUB[v].xcoord();
      if ( vx < get_x(p)-EPS2) 
      { w = New_Node(SUB,get_x(p),cross_y);
        New_Edge(SUB,v,w,seg);
        set_left_node(seg,w);
       }
      else w = v;
     }

    u = get_left_node(l);
    if (u!=nil && u!=w) New_Edge(SUB,u,w,l);
    set_left_node(l,w);
*/

   }
    
  delete l;
  delete bot;
  delete top;
  Y_structure.del_item(bot_it);
  Y_structure.del_item(top_it);

 }


void plane_sweep(list<segment>& L1, list<segment>& L2 
                 /*, GRAPH<point,int>& SUB */)
{
  SWEEP_point    p,inter;
  SWEEP_segment  seg, l,lsit,lpred,lsucc,lpredpred;

  pq_item  pqit,pxmin;
  seq_item sitmin,sit,sitpred,sitsucc,sitpredpred;


  int count=1;

  Wp->clear();
 
  //initialization of the X-structure

  segment s;

  forall(s,L1) 
   { SWEEP_point p = new SWEEP_POINT(s.start());
     SWEEP_point q = new SWEEP_POINT(s.end());
     seg = new SWEEP_SEGMENT(p,q,0,count++);
     draw_segment(seg);
     Xinsert(nil,get_startpoint(seg));
/*
Print(seg);
Print(get_startpoint(seg));
cout << endl;
*/
   }

  count = -1;

  forall(s,L2) 
   { SWEEP_point p = new SWEEP_POINT(s.start());
     SWEEP_point q = new SWEEP_POINT(s.end());
     seg = new SWEEP_SEGMENT(p,q,1,count--);
     draw_segment(seg);
     Xinsert(nil,get_startpoint(seg));
/*
Print(seg);
Print(get_startpoint(seg));
cout << endl;
*/
   }


  count = 0;

  x_sweep = Wp->xmin();
  y_sweep = Wp->ymin();

  if (trace)
  { draw_Xstructure();
    draw_Ystructure();
   }

  draw_sweep_line(x_sweep);

  while(!X_structure.empty())
  {
    if (trace)  
       trace = (Wp->read_mouse() != MOUSE_BUTTON(3));
    else
       trace = (Wp->get_button() != NO_BUTTON);


    pxmin = X_structure.find_min();
    p = X_structure.inf(pxmin);

    move_sweep_line(x_sweep,get_x(p));

    sitmin = X_structure.key(pxmin);

    Xdelete(pxmin);



    if (get_kind(p) == Leftend)

    //left endpoint
    { 

      l = get_seg(p); 

      x_sweep = get_x(p);
      y_sweep = get_y(p);

      if (trace)
      message(string("LEFT  point   x = %4f seg = %4d",
              get_x(p),get_name(l)),1);


      if (get_x(p) == get_x(get_endpoint(l)))
        process_vertical_segment(/*SUB,*/ l);
      else
      {

      sit = Y_structure.lookup(l);

      if (sit!=nil)  error_handler(0,"plane sweep: sorry, overlapping segments");

      sit = Y_structure.insert(l,pq_nil);

      Xinsert(sit,get_endpoint(l));

      sitpred = Y_structure.pred(sit);
      sitsucc = Y_structure.succ(sit);

      if (sitpred != nil) 
      { if ((pqit = Y_structure.inf(sitpred)) != nil)
          delete Xdelete(pqit);

        lpred = Y_structure.key(sitpred);

        Y_structure.change_inf(sitpred,pq_nil);

        if (intersection(lpred,l,inter))
            Y_structure.change_inf(sitpred,Xinsert(sitpred,inter));
      }


      if (sitsucc != nil)
      { lsucc = Y_structure.key(sitsucc);
        if (intersection(lsucc,l,inter))
           Y_structure.change_inf(sit,Xinsert(sit,inter));
      }
     } /* else if vertical */

    }
    else if (get_kind(p) == Rightend)
         //right endpoint
         { 
           x_sweep = get_x(p);
           y_sweep = get_y(p);

           if (trace)
           message(string("RIGHT point   x = %4f seg = %4d",
                   get_x(p),get_name(get_seg(p))),1);

           sit = sitmin;
 
           sitpred = Y_structure.pred(sit);
           sitsucc = Y_structure.succ(sit);

           SWEEP_segment SEG =Y_structure.key(sit);

           Y_structure.del_item(sit);

           delete SEG;

           if((sitpred != nil)&&(sitsucc != nil))
           {
             lpred = Y_structure.key(sitpred);
             lsucc = Y_structure.key(sitsucc);
             if (intersection(lsucc,lpred,inter))
                Y_structure.change_inf(sitpred,Xinsert(sitpred,inter));
           }
         }
         else /*point of intersection*/
         { 
           //node w = New_Node(SUB,get_x(p),get_y(p));

           count++;

           /* Let L = list of all lines intersecting in p 
 
              we compute sit     = L.head();
              and        sitpred = L.tail();

              by scanning the Y_structure in both directions 
              starting at sitmin;

           */

           /* search for sitpred upwards from sitmin: */

           Y_structure.change_inf(sitmin,pq_nil);

           sitpred = Y_structure.succ(sitmin);


           while ((pqit=Y_structure.inf(sitpred)) != nil)
           { SWEEP_point q = X_structure.inf(pqit);
             if (compare(p,q) != 0) break; 
             X_structure.del_item(pqit);
             Y_structure.change_inf(sitpred,pq_nil);
             sitpred = Y_structure.succ(sitpred);
            }


           /* search for sit downwards from sitmin: */

           sit = sitmin;

           seq_item sit1;
           
           while ((sit1=Y_structure.pred(sit)) != nil)
           { pqit = Y_structure.inf(sit1);
             if (pqit == nil) break;
             SWEEP_point q = X_structure.inf(pqit);
             if (compare(p,q) != 0) break; 
             X_structure.del_item(pqit);
             Y_structure.change_inf(sit1,pq_nil);
             sit = sit1;
            }


/*
           if (trace)
           { string line;
             line += string("sit = %d  ", get_name(Y_structure.key(sit)));
             line += string("sitpred = %d",get_name(Y_structure.key(sitpred)));
             message(line,5);
            }
*/

/*

           // insert edges to p for all segments in sit, ..., sitpred into SUB
           // and set left node to w 

           lsit = Y_structure.key(sitpred);

           node v = get_left_node(lsit);
           if (v!=nil) New_Edge(SUB,v,w,lsit);
           set_left_node(lsit,w);

           for(sit1=sit; sit1!=sitpred; sit1 = Y_structure.succ(sit1))
           { lsit = Y_structure.key(sit1);

             v = get_left_node(lsit);
             if (v!=nil) New_Edge(SUB,v,w,lsit);
             set_left_node(lsit,w);
            }
*/

           lsit = Y_structure.key(sit);
           lpred=Y_structure.key(sitpred);
           sitpredpred = Y_structure.pred(sit);
           sitsucc=Y_structure.succ(sitpred);

           message(string("INTERSECTION  # = %4d  x = %5f  seg = %4d ",
                        count, float(get_x(p)),get_name(lsit)),1);

           draw_sweep_line(get_x(p));
           Wp->draw_filled_node(get_x(p),get_y(p),node_color);
           draw_sweep_line(get_x(p));


           if (sitpredpred != nil)
            { 
              lpredpred=Y_structure.key(sitpredpred);

              if ((pqit = Y_structure.inf(sitpredpred)) != nil)
               delete Xdelete(pqit);
     

              Y_structure.change_inf(sitpredpred,pq_nil);


              if (intersection(lpred,lpredpred,inter))
                Y_structure.change_inf(sitpredpred,
                                       Xinsert(sitpredpred,inter));
             }


           if (sitsucc != nil)
            {
              lsucc=Y_structure.key(sitsucc);

              if ((pqit = Y_structure.inf(sitpred)) != nil)
                delete Xdelete(pqit);
                 
              Y_structure.change_inf(sitpred,pq_nil);

              if (intersection(lsucc,lsit,inter))
                  Y_structure.change_inf(sit,Xinsert(sit,inter));
             }


// reverse the subsequence sit, ... ,sitpred  in the Y_structure

           x_sweep = get_x(p);
           y_sweep = get_y(p);

           Y_structure.reverse_items(sit,sitpred);

           delete p;

         } // intersection

   if (trace) 
    { draw_Xstructure();
      draw_Ystructure();
     }

  }

  message("END OF SWEEP",1);

  if (intera) Wp->read_mouse();             //wait for mouse click

    draw_sweep_line(x_sweep);

    X_structure.clear();

} // plane_sweep



void interactive(window& W)
{
  intera= true;

  int grid_width = 0;
  int line_width = 1;
  int node_width = 4;
  int N = 80;

  panel P("PLANE SWEEP DEMO");

  P.bool_item("TRACE",trace);
  P.int_item("GRID",grid_width,0,80,20);
  P.int_item("SEGMENTS", N);
  P.int_item("line width",line_width,1,5);
  P.int_item("node width",node_width,1,10);
  P.button("MOUSE");
  P.button("RANDOM");
  P.button("QUIT");

  for(;;)
  { int input = P.open(W);
  
    if (input == 2) break;
  
    W.init(-1200,1200,-1200, grid_width);
    W.set_text_mode(opaque);
    W.set_node_width(node_width);
    W.set_line_width(line_width);
  
    list<segment> seglist1,seglist2;
  
    if (input==1)  // random 
     { double ymax = W.ymax()-4*20/W.scale()-100;
       int xmin = int(W.xmin())+100;
       int xmax = int(W.xmax())-100;
       for(int i=0; i<N; i++)
       { double x1 = rand_int(xmin,-100);
         double y1 = rand_int(-1000,int(ymax));
         double x2 = rand_int(100,xmax);
         double y2 = rand_int(-1000,int(ymax));
         segment s(x1,y1,x2,y2);
         W << s;
         seglist1.append(s);
        }
      }
    else // input == 0 (mouse)
      { segment s;
        while (W >> s)
        { W << s;
          seglist1.append(s);
         }
       }
  
  
    plane_sweep(seglist1,seglist2);
  
  //GRAPH<point,int> SUB;
  //plane_sweep(seglist1,seglist2,SUB);
  
 } // for(;;)

}



void demo(window& W, int N)
{
  trace = false;

  W.init(-1200,1200,-1200);

  for(;;)
  {

  W.clear();


  W.set_text_mode(opaque);
  W.set_node_width(3);


  list<segment> seglist1,seglist2;

     double ymax = W.ymax()-4*20/W.scale()-100;

     for(int i=0; i<N; i++)
     { double x1 = rand_int(-1100,-100);
       double y1 = rand_int(-1000,int(ymax));
       double x2 = rand_int(100,1100);
       double y2 = rand_int(-1000,int(ymax));
       segment s(x1,y1,x2,y2);
       W << s;
       seglist1.append(s);
      }

  plane_sweep(seglist1,seglist2);

/*
  GRAPH<point,int> SUB;
  edge e;

  plane_sweep(seglist1,seglist2,SUB);
 
  W.clear();
  forall_edges(e,SUB) W <<  segment(SUB[source(e)],SUB[target(e)]);
*/

  leda_wait(2);

  }

}



int main(int argc, char** argv)
{
  int N = 0;

  panel P0("PLANE SWEEP DEMO");
  P0.text_item("This program computes the intersections in a set of");
  P0.text_item("straight line segments using a plane sweep algorithm.");
  P0.text_item("You can define the set of line segments interactively");
  P0.text_item("using the left mouse button (input is terminated with");
  P0.text_item("the right button) or create a random set of segments.");
  P0.text_item("In TRACE mode the current contents of the event queue");
  P0.text_item("(X-structure) and of the sweep line (Y-structure) are");
  P0.text_item("displayed and the sweep line stops at each event point.");
  P0.button("continue");
   
  if (argc == 1)
    P0.open();
  else
    N = atoi(argv[1]);

  window W;
  W.open();

  Wp = &W;

  if (N==0) 
      interactive(W);
  else
     demo(W,N);


  return 0;
}
