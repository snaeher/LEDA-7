/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_window.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/d3_window.h>
#include <LEDA/numbers/vector.h>
#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE

void d3_window::init(const node_array<vector>& pos) 
{ pos1 = pos;
  vector cg(0,0,0);
  node v;
  forall_nodes(v,*Hp) 
  { rotate(total_rot1,total_rot2,pos1[v]);
    cg += pos1[v];
   }
  g_center = cg/Hp->number_of_nodes();
  zoom_f = 1;
}

void d3_window::init(const node_array<rat_vector>& pos) 
{ pos1.init(*Hp,vector(3));
  vector cg(0,0,0);
  node v;
  forall_nodes(v,*Hp) 
  { pos1[v] = pos[v].to_vector();
    rotate(total_rot1,total_rot2,pos1[v]);
    cg += pos1[v];
   }
  g_center = cg/Hp->number_of_nodes();
  zoom_f = 1;
}


void d3_window::init(window& W, const graph* Gp, double rotation1,
                                                 double rotation2) 
{ draw_solid = true;
  node_labels = false;
  node_width = 8;
  elim = true;
  speed = 12; 
  draw_edges = true;
  draw_graph = false;
  total_rot1 = rotation1;
  total_rot2 = rotation2;
  rot1 = 0;
  rot2 = 0;
  moving = 1;
  mouse_x = 0;
  mouse_y = 0;
  Wp = &W;
  Hp = Gp;
  pos1.init(*Hp,vector(3));
  n_clr.init(*Hp,blue);
  e_clr.init(*Hp,black);
  draw_arrow.init(*Hp,false);
  message_str = "\\bf\\blue left:  \\rm\\black zoom up~~~~~~~~\
                 \\bf\\blue middle:\\rm\\black zoom down~~~~~~\
                 \\bf\\blue right: \\rm\\black xxxx rotation";

  g_center = vector(0,0,0);

  node_hcolor = invisible;
  edge_hcolor = invisible;
  show_bends = no_bends;
  x_rotation = true;
  y_rotation = true;
  zooming = true;
}


void d3_window::rotate(double alpha1,double alpha2, vector& p)
{ 
  // rotate 3d-point p about g_center
  // by alpha2 in yz-plane and alpha1 in xy-plane

    if (alpha1 == 0  && alpha2 == 0) return;

    p = p - g_center;

    double R  = hypot(p[1],p[2]);

    if (R > 0)
    { double phi = asin(p[1]/R);
      if (p[2] < 0) phi = LEDA_PI - phi;
      p[1]  = ((R != 0) ? R*sin(phi+alpha2) : 0);
      p[2]  = ((R != 0) ? R*cos(phi+alpha2) : 0);
     }

    R = hypot(p[0],p[2]);

    if (R > 0)
    { double phi = asin(p[0]/R);
      if (p[2] < 0) phi = LEDA_PI - phi;
      p[0]  = ((R != 0) ? R*sin(phi+alpha1) : 0);
      p[2]  = ((R != 0) ? R*cos(phi+alpha1) : 0);
    }

    p = p + g_center;
}



void d3_window::set_position(node v, double x, double y, double z)
{ pos1[v] = vector(x,y,z); 
  rotate(total_rot1,total_rot2,pos1[v]);
 }

void d3_window::set_bends(edge e, const list<vector>& L)
{ list<vector>& L1 = bends[e];
  L1.clear();
  vector vec;
  forall(vec,L)
  { rotate(total_rot1,total_rot2,vec);
    L1.append(vec);
   }
 }


void d3_window::draw_message()
{ 
  double th = Wp->text_height("H");

  double x0 = Wp->xmin();
  double x1 = Wp->xmax();
  double y  = Wp->ymax() - th/2;

  if (moving)
    Wp->text_box(x0,x1,y,message_str.replace("xxxx","stop") + "\\c");
  else
    Wp->text_box(x0,x1,y,message_str.replace("xxxx","start") + "\\c");

}
  

void d3_window::draw_poly()
{ 
  node_array<point> d2pos(*Hp);

  node v;
  forall_nodes(v,*Hp) 
  { vector vec = zoom_f*(pos1[v] - g_center) + g_center;
    d2pos[v] = point(vec[0],vec[1]);
   }

/*
  double xmin = Wp->xmin();
  double xmax = Wp->xmax();
  double ymin = Wp->ymin();
  double ymax = Wp->ymax();
*/


  Wp->start_buffering();
  Wp->clear();
  Wp->draw_point(g_center[0],g_center[1],red);
  draw_message();

  if (Hp->number_of_edges() > 0)
  {
    edge_array<double> label(*Hp,0);

    edge e;
    forall_edges(e,*Hp) 
    {
      if (e_clr[e] == grey1 /*|| e_clr[e] == invisible */) continue;

      if (source(e) == target(e) || label[e] != 0) continue;


      edge e1 = Hp->face_cycle_succ(e);
      node a = source(e);
      node b = source(e1);
      node c = target(e1);

      double d = 0;

      if (elim || draw_solid)
      { vector v1 = pos1[b] - pos1[a];
        vector v2 = pos1[c] - pos1[a];
        double cx = v1[2]*v2[1]-v1[1]*v2[2]; 
        double cy = v1[0]*v2[2]-v1[2]*v2[0];
        double cz = v1[1]*v2[0]-v1[0]*v2[1];
        vector n = vector(cx,cy,cz).norm();
        //vector r = vector(0.5,0.25,1).norm();
        vector r = vector(0,0,1);
        d = -n*r;
        if (d == 0) d = -1;
      }
      else d = 1;

/*
      edge x = e;
      do { label[x] = d;
           x = Hp->face_cycle_succ(x);
         } while (x != e);
*/

      //int i =  60 + int(170*d);  // intensity 60 ... 230 
      int i =  60 + int(180*d);  // intensity 60 ... 240 

      color fill_clr = color(i,i,i);

      list<point> pol;

int arrow_count = 0;

      edge x = e;
      do { label[x] = d;
           if (draw_arrow[x]) arrow_count++;
           //assert(e_clr[x] == e_clr[e]);
           if (draw_solid && d > 0) pol.append(d2pos[source(x)]);
           x = Hp->face_cycle_succ(x);

if (arrow_count > 0) fill_clr = red;

         } while (x != e);

      if (!pol.empty()) Wp->draw_filled_polygon(pol,fill_clr);

      if (draw_edges && d > 0)
      { edge x = e;
        do { node u = source(x);
             node v = target(x);
             color edge_clr = draw_solid ? color(60,60,60) : e_clr[x];
             if (draw_arrow[x]) 
               Wp->draw_arrow(d2pos[u],d2pos[v],edge_clr);
	     else 
               Wp->draw_segment(d2pos[u],d2pos[v],edge_clr);
             x = Hp->face_cycle_succ(x);
           } while (x != e);
       }
    }

   }

  Wp->set_node_width(node_width);

  forall_nodes(v,*Hp) 
  { 
    if (Hp->degree(v) > 0) continue;

    if (n_clr[v] == node_hcolor) 
       Wp->draw_filled_node(d2pos[v],n_clr[v]);
    else
       Wp->draw_point(d2pos[v],n_clr[v]);
   }

/*
  if (max_coord < 0)
    Wp->flush_buffer();
  else
    Wp->flush_buffer(-max_coord,-max_coord,+max_coord,+max_coord);
*/

  Wp->flush_buffer();
  Wp->stop_buffering();
}


void d3_window::draw_segments()
{
  double x0 = g_center[0];
  double y0 = g_center[1];

  Wp->start_buffering();
  Wp->clear();
  Wp->draw_point(x0,y0,red);
  draw_message();

  list_item it;
  forall_items(it,seg_list)
  { vector v = seg_list[it].first();
    vector w = seg_list[it].second();
    color  c = seg_list[it].third();
    double x1 = zoom_f*(v[0] - x0) + x0;
    double y1 = zoom_f*(v[1] - y0) + y0;
    double x2 = zoom_f*(w[0] - x0) + x0;
    double y2 = zoom_f*(w[1] - y0) + y0;
    Wp->draw_segment(x1,y1,x2,y2,c);
  }
  Wp->flush_buffer();
  Wp->stop_buffering();
}


void d3_window::draw()
{ 
  if (Hp == 0)
  { draw_segments();
    return;
   }

  if (!draw_graph)
  { draw_poly();
    return;
   }

  node_array<point> d2pos(*Hp);

  node v;
  forall_nodes(v,*Hp) 
  { //d2pos[v] = point(zoom_f*pos1[v][0],zoom_f*pos1[v][1]);
    vector vec = zoom_f*(pos1[v] - g_center) + g_center;
    d2pos[v] = point(vec[0],vec[1]);
   }

  Wp->start_buffering();
  Wp->clear();
  Wp->draw_point(g_center[0],g_center[1],red);
  draw_message();

  double x0 = g_center[0];
  double y0 = g_center[1];

  edge e;
  forall_edges(e,*Hp)
  { list<vector>& L = bends[e];
    if (show_bends == no_bends || L.empty())
         if (! draw_arrow[e]) Wp->draw_segment(d2pos[source(e)],d2pos[target(e)],e_clr[e]);
	 //new
	 else Wp->draw_arrow(d2pos[source(e)],d2pos[target(e)],e_clr[e]);
    else
    if (show_bends == poly_bends)
     { int n = L.size() + 2;
       double* xbends = new double[n];
       double* ybends = new double[n];
       xbends[0] = d2pos[source(e)].xcoord();
       ybends[0] = d2pos[source(e)].ycoord();
       int i = 1;
       vector vec;
       forall(vec,L) 
       { xbends[i] = zoom_f*(vec[0] - x0) + x0;
         ybends[i] = zoom_f*(vec[1] - y0) + y0;
         i++;
        }
       xbends[i] = d2pos[target(e)].xcoord();
       ybends[i] = d2pos[target(e)].ycoord();
       Wp->draw_polyline(n,xbends,ybends,e_clr[e]);
       delete[] xbends;
       delete[] ybends;
     }
    else 
     if (show_bends == bezier_bends)
     { list<point> poly;
       poly.append(d2pos[source(e)]);
       vector vec;
       forall(vec,L) 
       { double x = zoom_f*(vec[0] - x0) + x0;
         double y = zoom_f*(vec[1] - y0) + y0;
         poly.append(point(x,y));
        }
       poly.append(d2pos[target(e)]);
       Wp->draw_bezier(poly,64,e_clr[e]);
     }
   }
       
  
  Wp->set_node_width(node_width);

  forall_nodes(v, *Hp) 
  { Wp->set_node_width(int(node_width + pos1[v][2]/200));
    if (node_labels)
       Wp->draw_text_node(d2pos[v],string("%d",index(v)),n_clr[v]);
    else
       Wp->draw_filled_node(d2pos[v],n_clr[v]);
   }
  
  Wp->flush_buffer();
  Wp->stop_buffering();
}

double d3_window::get_zoom_factor() const
{ return zoom_f; }

double d3_window::set_zoom_factor(double new_factor)
{
  double old = zoom_f;
  zoom_f = new_factor;
  return old;
}

bool d3_window::get_move() const
{ return moving; }

bool d3_window::set_move(bool m)
{
  bool old = moving;
  moving = m;
  return old;
}

void d3_window::move_graph(double x, double y)
{ if (Hp == 0) return; 
  vector vec(x,y,0);
  node v;
  forall_nodes(v,*Hp)  pos1[v] = pos1[v] + vec; 
}


void d3_window::zoom(double f)
{ int e;
  double x,y;
  int val;
/*
  unsigned long t;
  while ((e = Wp->read_event(val,x,y,t,10)) != button_release_event)
*/
  while ((e = Wp->get_event(val,x,y)) != button_release_event)
  { 
    zoom_f *= f;

    if (moving)
    { 
      node v;
      forall_nodes(v,*Hp) rotate(rot1,rot2,pos1[v]);

      if (show_bends)
      { edge e;
        forall_edges(e,*Hp)
        { list<vector>& L = bends[e];
          list_item it;
          forall_items(it,L) rotate(rot1,rot2,L[it]);
        }
      }

      total_rot1 += rot1;
      total_rot2 += rot2;
    }
    draw();
   }
}


void d3_window::move(double dx, double dy)
{
  vector vec(dx,dy);

  double l = vec.length();
  if (l == 0) return;

  double d = Wp->width()/15.0;
  double f = 0;

#if defined(__win32__)
  if (l > d) f = speed*(l-d)/(5000*d);
#else
  if (l > d) f = speed*(l-d)/(15000*d);
#endif

  vec = vec.norm();
  rot1 = f * vec[0];
  rot2 = f * vec[1];

  node v;
  forall_nodes(v,*Hp) rotate(rot1,rot2,pos1[v]);

  if (show_bends)
  { edge e;
    forall_edges(e,*Hp)
    { list<vector>& L = bends[e];
      list_item it;
      forall_items(it,L) rotate(rot1,rot2,L[it]);
     }
   }

  total_rot1 += rot1;
  total_rot2 += rot2;

  draw();
}




int d3_window::move()
{
  int val;
  double x=0;
  double y=0;
  int e = Wp->get_event(val,x,y);

  if (e == motion_event)
  { if (y < Wp->ymax())
    { mouse_x = x;
      mouse_y = y;
     }
    return NO_BUTTON;
   }

  if (e == button_press_event) 
  { if (zooming)
    { double f = 0.0004;
      if (window::display_type() == "xx") f *= 25;
      if (val == MOUSE_BUTTON(1)) zoom(1+f);
      if (val == MOUSE_BUTTON(2)) zoom(1-f);
      if (val == MOUSE_BUTTON(3)) moving = !moving;
    }
    return val;
   }

  double dx = x_rotation ? int(mouse_x - g_center[0]) : 0; 
  double dy = y_rotation ? int(mouse_y - g_center[1]) : 0; 

  if (moving) 
    move(dx,dy);
  else
    draw();

  return NO_BUTTON;
}


void d3_window::get_d2_position(node_array<point>& d2pos)
{ node v;
  forall_nodes(v,*Hp) 
  { vector vec = zoom_f*(pos1[v] - g_center) + g_center;
    d2pos[v] = point(vec[0],vec[1]);
   }
}

void d3_window::get_d2_bends(edge_array<list<point> >& d2bends)
{ edge e;
  forall_edges(e,*Hp) 
  { d2bends[e].clear();
    vector vec;
    forall(vec, bends[e])
    { vec = zoom_f*(vec - g_center) + g_center;
      d2bends[e].append(point(vec[0],vec[1]));
     }
   }
}

color d3_window::get_color(node v) const
{ return n_clr[v]; }

color d3_window::set_color(node v, color c) 
{ color prev = n_clr[v]; n_clr[v] = c; return prev;}

color d3_window::get_color(edge e) const
{ return e_clr[e]; }

color d3_window::set_color(edge e, color c) 
{ color prev = e_clr[e]; e_clr[e] = c; return prev;}

bool d3_window::get_arrow(edge e) const
{ return draw_arrow[e]; }

bool d3_window::set_arrow(edge e, bool b) 
{ bool prev=draw_arrow[e]; draw_arrow[e] = b; return prev;}

void d3_window::set_node_color(color c) 
{ node v;
  forall_nodes(v,*Hp) n_clr[v] = c; 
}

void d3_window::set_edge_color(color c)
{ edge e;
  forall_edges(e,*Hp) e_clr[e] = c; 
}


int d3_window::get_mouse() { return move(); }

int d3_window::read_mouse() 
{ int but;
  string save_msg =  message_str;
  message_str = "";
  do but = move(); while (but == NO_BUTTON);
  message_str = save_msg;
  return but;
 }

LEDA_END_NAMESPACE
