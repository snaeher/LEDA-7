/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  locate.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

LEDA_BEGIN_NAMESPACE

static double distance(const point& p, const point& a, const point& b, 
                                       const point& c, const point& d,
                                       const list<point>& P) 
{ double d_min = MAXDOUBLE;
  list_item it = P.first();
  point     p1 = P[it];
  
  while ( (it=P.succ(it)) != nil ) 
  { point p2 = P[it];
    segment s(p1,p2);
    if (orientation(s,a) != orientation(s,b) ||
        orientation(s,c) != orientation(s,d))
    { double d = s.sqr_dist(p);
      if (d < d_min) d_min=d;
     }
    p1 = p2;
  }
  return d_min;
}


/*
static bool intersection(const point& p, const segment& a, const segment& b, 
                                       const segment& c, const segment& d,
                                       const list<point>& P) 
{ 
  list_item it = P.first();
  point     p1 = P[it];
  while ( (it=P.succ(it)) != nil ) 
  { point p2 = P[it];
    segment s(p1,p2);
    if (s.intersection(a) || s.intersection(b) ||
        s.intersection(c) || s.intersection(d)) return true;
    p1 = p2;
  }
  return false;
}
*/


list_item GraphWin::find_bend(const point& p, edge x) 
{ 
  list<point>& P = get_poly(x);

  double eps  = win_p->pix_to_real(8);

  list_item min_it = 0;

  list_item it;
  forall_items(it,P)
  { if (it == P.first() || it == P.last()) continue;
    double d = p.distance(P[it]);
    if (d < eps) {
      min_it = it; 
      break;
    }
   }

  return min_it;
}



edge GraphWin::find_edge(const point& p, edge from) 
{ 
  edge   e_min = nil;
  double d_min = MAXDOUBLE;

  found_edge = nil;

  edge e = from == nil ? gr_p->first_edge() : gr_p->succ_edge(from);

  //double eps  = win_p->pix_to_real(6);
  double eps  = win_p->pix_to_real(8);

  double eps2 = eps*eps;

  point p1  = p.translate(-eps,0);
  point p2  = p.translate(+eps,0);
  point p3  = p.translate(0,-eps);
  point p4  = p.translate(0,+eps);

/*
  segment a = segment(p1,p2);
  segment b = segment(p2,p3);
  segment c = segment(p3,p4);
  segment d = segment(p4,p1);
*/

  while (e != nil && found_edge == nil) 
  { 
    // if (intersection(p,a,b,c,d,get_poly(e))) found_edge = e;
    list<point>& pol = get_poly(e);

    if (e_info[e].shape == circle_edge && pol.length() > 2)
    { // circular arc
      list_item it1 = pol.first();
      list_item it2 = pol.succ(it1);
      list_item it3 = pol.succ(it2);
      point p1 = pol[it1];
      point p2 = pol[it2];
      point p3 = pol[it3];
      circle C(p1,p2,p3);

      if (!C.is_degenerate())
      { point  cent = C.center();
        double  rad = C.radius();
        if (orientation(p1,p3,p2) == orientation(p1,p3,p))
        { double dist = rad - cent.distance(p);
          dist *= dist;
          if (dist < d_min) 
          { e_min = e;
            d_min = dist;
          }      
         }
        e = gr_p->succ_edge(e);
        continue;
       }
     }

    double dist = distance(p,p1,p2,p3,p4,pol);

    if (dist < d_min) 
    { e_min = e;
      d_min = dist;
     }      
    e = gr_p->succ_edge(e);
  }

  if (d_min < eps2) found_edge = e_min;

  return found_edge;
}
 

int GraphWin::find_slider(const point& p, edge e) 
{ list<point>& P = get_poly(e);
  point p1 = P.head();
  point p2 = P.tail();

  double  e_len = p1.distance(p2);

  line L(p1,p2.rotate90(p1));

//double delta;

  int i;
  for(i=MAX_SLIDERS-1; i>=0; i--)
  { if (edge_slider_handler[i] == NULL && 
        start_edge_slider_handler[i] == NULL &&
        end_edge_slider_handler[i] == NULL) continue;
    double dist = L.distance(p);
    double D = e_info[e].slider_f[i]*e_len;

    //double d = win_p->pix_to_real(6); 
    double d = win_p->pix_to_real(8); 

    if (dist < D+d && dist > D-d)
    { //delta = e_info[e].slider_f[i] - dist/e_len;
      break;
     }
   }

  found_slider = (i == MAX_SLIDERS) ? -1 : i;
  return found_slider;
}



node GraphWin::find_node(const point& p, node from) 
{
  // search from last node to first node

  node v = (from == nil) ? gr_p->last_node() : gr_p->pred_node(from);

  node found_node_save = found_node;
  node found_frame_save = found_frame;

  found_node = nil;
  found_frame = nil;
  
  while (v != nil && found_node == nil)
  { 
    point  vp = get_position(v);

    double dx = p.xcoord() - vp.xcoord(); 
    double dy = p.ycoord() - vp.ycoord();
    double r1 = get_radius1(v); 
    double r2 = get_radius2(v);

/*
    double d  = get_border_thickness(v)/2 + win_p->pix_to_real(1);
    if (d < win_p->pix_to_real(2)) d = win_p->pix_to_real(2);
*/

    double d  = get_border_thickness(v);
    if (d < win_p->pix_to_real(3)) d = win_p->pix_to_real(3);
    
    switch (get_shape(v)) {
    
      case circle_node:
        { double dist = sqrt(dx*dx + dy*dy);
          if (dist < r1+d)
          { found_node = v;
            if (frame_actions_enabled && dist >= r1-d) found_frame = v;
           }
          break;
         }
        

      case ellipse_node: 
        { double d = dx*dx/(r1*r1) + dy*dy/(r2*r2);
          if (d < 1.15)
          { found_node = v;
            if (frame_actions_enabled && d > 0.85) found_frame = v;
           }
          break;
         }
      
      case triang0_node:
      case triang1_node:
      case triang2_node:
      case triang3_node:

      case square_node: 
      case roundrect_node: 
      case ovalrect_node: 
      case rectangle_node: 
        { double DX = fabs(dx);
          double DY = fabs(dy);
          if ( DX < r1+d && DY < r2+d) 
          { found_node = v;
            if (frame_actions_enabled && (DX >= r1-d || DY >= r2-d)) 
               found_frame = v;
           }
          break;
        }

      case rhombus_node: 
	{ point A = vp.translate(0,-r2-d);
	  point B = vp.translate(+r1+d,0);
	  point C = vp.translate(0,+r2+d);
	  point D = vp.translate(-r1-d,0);
          if (orientation(A,B,p) > 0 && orientation(B,C,p) > 0 &&
              orientation(C,D,p) > 0 && orientation(D,A,p) > 0)
          { found_node = v;
            if (frame_actions_enabled)
	    { A = vp.translate(0,-r2+d);
  	      B = vp.translate(+r1-d,0);
              C = vp.translate(0,+r2-d);
              D = vp.translate(-r1+d,0);
              if (orientation(A,B,p) < 0 || orientation(B,C,p) < 0 ||
                  orientation(C,D,p) < 0 || orientation(D,A,p) < 0)
                found_frame = v;
             }
           }
          break;
        }
    }

    v = gr_p->pred_node(v);  
  }

// sn 2023/04

  if (found_node != found_node_save || found_frame != found_frame_save)
  { if (found_node) redraw_node(found_node);
    if (found_node_save) redraw_node(found_node_save);
   }

/*
  if (found_node) cout << p << " v = " << found_node << endl;
*/
  
  return found_node;
}

LEDA_END_NAMESPACE
