/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  embed.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/plane_graph_alg.h>
#include <LEDA/graphics/motion.h>
#include "local.h"

LEDA_BEGIN_NAMESPACE

static linear_motion linear_move;

inline bool IsVertical(const line& l)
{ const double eps = 0.000001;
  double delta = l.point1().xcoord() - l.point2().xcoord();
  return (delta < eps && delta > -eps);
 }

inline bool IsHorizontal(const line& l)
{ const double eps = 0.000001;
  double delta = l.point1().ycoord() - l.point2().ycoord();
  return (delta < eps && delta > -eps);
 }


point GraphWin::first_hit(node v, const line& l, 
                          bool from_left, bool from_above)
{
  point  v_pos = get_position(v);

  double x0 = v_pos.xcoord();
  double y0 = v_pos.ycoord();
  double rx = get_radius1(v);
  double ry = get_radius2(v);

  switch(get_shape(v)) {

    case circle_node: 
      //ry=rx;
    case ellipse_node: {

      if (IsVertical(l)) 
      { double x  = l.x_proj(0);
        double dx = x-x0;
        double s  = ry/rx*sqrt(rx*rx-dx*dx);
        if (from_above) s = -s;
        return point(x,y0+s);
       }

      double m  = l.slope();
      double t0 = y0-l.y_proj(0);
      double t1 = m*rx*rx;
      double t2 = ry*ry;
      double t3 = x0*t2+t0*t1;
      t2 = t1*m+t2;
      t1 = x0*m-t0;

      if (t2 >= t1*t1)
        t1 = rx*ry*sqrt(t2-t1*t1);
      else // should never happen ?
        t1 = 0;

      double x = (from_left) ? (t3-t1)/t2 : (t3+t1)/t2;

      return point(x,l.y_proj(x));
    }

    case square_node:
      //ry=rx;
    case roundrect_node: 
    case ovalrect_node: 
    case rectangle_node: {

      if (IsVertical(l)) 
      { double x  = l.x_proj(0);
        if (from_above)
          return point(x,y0-ry);
        else
          return point(x,y0+ry);
       }

      if (IsHorizontal(l)) 
      { double y  = l.y_proj(0);
        if (from_left)
          return point(x0-rx,y);
        else
          return point(x0+rx,y);
       }

      x0 += (from_left  ? -rx : +rx);
      y0 += (from_above ? -ry : +ry);

      double x = l.x_proj(y0);
      double y = l.y_proj(x0);
        
      if (from_above) 
         if (y < y0) 
            return point(x,y0);
         else
            return point(x0,y);
      else
         if (y > y0) 
            return point(x,y0);
         else
            return point(x0,y);
     }

   case rhombus_node:
        { point a = v_pos.translate(0,-ry);
          point b = v_pos.translate(+rx,0);
          point c = v_pos.translate(0,+ry);
          point d = v_pos.translate(-rx,0);
          list<point> I;
          point p;
          if (l.intersection(segment(a,b),p)) I.append(p);
          if (l.intersection(segment(b,c),p)) I.append(p);
          if (l.intersection(segment(c,d),p)) I.append(p);
          if (l.intersection(segment(d,a),p)) I.append(p);

          I.sort();

          if (IsVertical(l))
             return (from_above) ? I.head() : I.tail();

          return (from_left) ? I.head() : I.tail();
        }

    case triang0_node:
    case triang1_node:
    case triang2_node:
    case triang3_node:
        { 
          point a = v_pos.translate(0,ry);
          //point b = v_pos.translate(-sqrt(3.0)*rx/2, -0.5*rx);
          //point c = v_pos.translate( sqrt(3.0)*rx/2, -0.5*rx);
          point b = v_pos.translate(-rx,-sqrt(ry*ry-rx*rx));
          point c = b.translate(2*rx,0);

          switch (get_shape(v)) {
            case triang3_node : a = a.rotate90(v_pos);
                                b = b.rotate90(v_pos);
                                c = c.rotate90(v_pos);
                               
            case triang2_node : a = a.rotate90(v_pos);
                                b = b.rotate90(v_pos);
                                c = c.rotate90(v_pos);
                               
            case triang1_node : a = a.rotate90(v_pos);
                                b = b.rotate90(v_pos);
                                c = c.rotate90(v_pos);
            default:
                    break;
          }

          list<point> I;
          point p;
          if (l.intersection(segment(a,b),p)) I.append(p);
          if (l.intersection(segment(b,c),p)) I.append(p);
          if (l.intersection(segment(c,a),p)) I.append(p);

          if (I.empty()) return l.point2();

          I.sort();

          if (IsVertical(l))
             return (from_above) ? I.head() : I.tail();

          return (from_left) ? I.head() : I.tail();
        }

  }

  return point(0,0); // never reached
}  


//----------------------------------------------------------------------------


point GraphWin::compute_leaving_point(node v, point v_pos, point p) 
{
  if (p.distance(v_pos) < win_p->pix_to_real(1)) return p;

  double x_v  = v_pos.xcoord();
  double y_v  = v_pos.ycoord();

  double x    = p.xcoord();
  double y    = p.ycoord();

  if (ortho_mode)
  { double rx_v = get_radius1(v);
    double ry_v = get_radius2(v);
    if (x >= x_v-rx_v && x <= x_v+rx_v)
       return first_hit(v, line(p,vector(0.0,1.0)), false, y<y_v);
    if (y >= y_v-ry_v && y <= y_v+ry_v)
       return first_hit(v, line(p,vector(1.0,0.0)), x<x_v, false);
  }

  //return first_hit(v, line(p,v_pos), x<x_v, y<y_v);
  return first_hit(v, line(v_pos,p), x<x_v, y<y_v);
}


point GraphWin::compute_leaving_circle_point(node v, point pv, point p, point q) 
{
  if (orientation(pv,p,q) == 0) 
       return compute_leaving_point(v,pv,p);

  double r1  = get_radius1(v);
  double r2  = get_radius2(v);
  double x   = pv.xcoord();
  double y   = pv.ycoord();

  circle C(pv,p,q);
  list<point> I;


  switch(get_shape(v)) {

    case ellipse_node:
    case circle_node:    { I = C.intersection(circle(pv,r1));
                           break;
                          }

    case rhombus_node:   { point a(x,y-r2);
                           point b(x+r1,y);
                           point c(x,y+r2);
                           point d(x-r1,y);
                           list<point> L;
                           L = C.intersection(segment(a,b));
                           I.conc(L);
                           L = C.intersection(segment(b,c));
                           I.conc(L);
                           L = C.intersection(segment(c,d));
                           I.conc(L);
                           L = C.intersection(segment(d,a));
                           I.conc(L);
                           break;
                         }

    case triang0_node:
    case triang1_node:
    case triang2_node:
    case triang3_node:   { point a = pv.translate(0,r2);
                           //point b = pv.translate(-sqrt(3.0)*r1/2, -0.5*r1);
                           //point c = pv.translate( sqrt(3.0)*r1/2, -0.5*r1);
                           point b = pv.translate(-r1,-sqrt(r2*r2-r1*r1));
                           point c = b.translate(2*r1,0);
                 
                           switch (get_shape(v)) {
                             case triang3_node : a = a.rotate90(pv);
                                                 b = b.rotate90(pv);
                                                 c = c.rotate90(pv);
                                                
                             case triang2_node : a = a.rotate90(pv);
                                                 b = b.rotate90(pv);
                                                 c = c.rotate90(pv);
                                                
                             case triang1_node : a = a.rotate90(pv);
                                                 b = b.rotate90(pv);
                                                 c = c.rotate90(pv);
                             default:
                                     break;
                           }


                           list<point> L;
                           L = C.intersection(segment(a,b));
                           I.conc(L);
                           L = C.intersection(segment(b,c));
                           I.conc(L);
                           L = C.intersection(segment(c,a));
                           I.conc(L);
                           break;
                         }


    case square_node:
    case roundrect_node: 
    case ovalrect_node: 
    case rectangle_node: { point a(x-r1,y-r2);
                           point b(x+r1,y-r2);
                           point c(x+r1,y+r2);
                           point d(x-r1,y+r2);
                           list<point> L;
                           L = C.intersection(segment(a,b));
                           I.conc(L);
                           L = C.intersection(segment(b,c));
                           I.conc(L);
                           L = C.intersection(segment(c,d));
                           I.conc(L);
                           L = C.intersection(segment(d,a));
                           I.conc(L);
                           break;
                         }
   }

  point i = pv;

  forall(i,I)
    if (orientation(pv,q,p) == orientation(pv,q,i)) break;

  return i;
}





//----------------------------------------------------------------------------

double GraphWin::compute_ortho_leaving_points(node v,node w,point& pv,point& pw)
{
  point p = get_position(v);
  point q = get_position(w);

  array<double> x(4);

  x[0] = p.xcoord() - get_radius1(v);
  x[1] = p.xcoord() + get_radius1(v);
  x[2] = q.xcoord() - get_radius1(w);
  x[3] = q.xcoord() + get_radius1(w);

  if (x[2] <= x[1] && x[3] >= x[0])
  { x.sort();
    point p1((x[1]+x[2])/2,p.ycoord());
    line l(p1,vector(0.0,1.0));
    pv = first_hit(v,l,false,q.ycoord() < p.ycoord());
    pw = first_hit(w,l,false,q.ycoord() > p.ycoord());
    return x[2]-x[1];
   }

  x[0] = p.ycoord() - get_radius2(v);
  x[1] = p.ycoord() + get_radius2(v);
  x[2] = q.ycoord() - get_radius2(w);
  x[3] = q.ycoord() + get_radius2(w);

  if (x[2] <= x[1] && x[3] >= x[0])
  { x.sort();
    point p1(p.xcoord(),(x[1]+x[2])/2);
    line l(p1,vector(1.0,0.0));
    pv = first_hit(v,l,q.xcoord() < p.xcoord(),false);
    pw = first_hit(w,l,q.xcoord() > p.xcoord(),false);
    return x[2]-x[1];
   }

  return -1;
}


//----------------------------------------------------------------------------

static void parallel_tangents_to_ellipse(double rx, double ry,
                                         const line& l, line& l1, line& l2) 
{ rx *= rx;
  ry *= ry;
  double m = l.slope();
  double n = m*rx;
  double s = sqrt(ry+m*n);
  rx = n/s;
  ry = -ry/s;

  l1 = l.translate(vector(rx,ry));
  l2 = l.translate(vector(-rx,-ry));
}


//----------------------------------------------------------------------------

static void parallel_tangents_to_rectangle(double rx, double ry,
                                           const line& l, line& l1, line& l2) 
{ if (l.slope() > 0) rx = -rx;
  l1 = l.translate(vector(rx,ry));
  l2 = l.translate(vector(-rx,-ry));
}

//----------------------------------------------------------------------------

double GraphWin::max_edge_distance(node v, node w) 
{
  point v_pos=get_position(v);
  point w_pos=get_position(w);

  line l(v_pos,w_pos);

  if (IsVertical(l))
    return  0.9*2*leda_min(get_radius1(v),get_radius1(w)); 

  if (IsHorizontal(l))
    return  0.9*2*leda_min(get_radius2(v),get_radius2(w));

  line v_l1;
  line v_l2;
  line w_l1;
  line w_l2;
  
  double rx=get_radius1(v);
  double ry=get_radius2(v);

  switch (get_shape(v)) {
    case circle_node :
      ry=rx;
    case ellipse_node:
      parallel_tangents_to_ellipse(rx,ry,l,v_l1,v_l2);
      break;

    case triang0_node:
    case triang1_node:
    case triang2_node:
    case triang3_node:

    case square_node:
      ry=rx;
    case rhombus_node:
    case roundrect_node: 
    case ovalrect_node: 
    case rectangle_node:
      parallel_tangents_to_rectangle(rx,ry,l,v_l1,v_l2);
      break;
  }

  rx=get_radius1(w);
  ry=get_radius2(w);

  switch (get_shape(w)) {
    case circle_node :
      ry=rx;
    case ellipse_node:
      parallel_tangents_to_ellipse(rx,ry,l,w_l1,w_l2);
      break;

    case triang0_node:
    case triang1_node:
    case triang2_node:
    case triang3_node:

    case square_node: 
      ry=rx;
    case rhombus_node:
    case roundrect_node: 
    case ovalrect_node: 
    case rectangle_node:
      parallel_tangents_to_rectangle(rx,ry,l,w_l1,w_l2);
      break;
  }

 // compute distance of inner parallels

  double m = v_l1.slope();
  double a = v_l1.y_proj(0);
  double b = v_l2.y_proj(0);
  double c = w_l1.y_proj(0);
  double d = w_l2.y_proj(0);

  if (a > b) { double tmp=a; a=b; b=tmp; }
  if (c > d) { double tmp=c; c=d; d=tmp; }
  
  double f = fabs((b < d ? b : d) - (a < c ? c : a));

  return 0.9*f/sqrt(m*m+1);
}

//----------------------------------------------------------------------------

void GraphWin::embed_edges(node v, node w) 
{
  if (gr_p->index(v) > gr_p->index(w)) { node tmp=v; v=w; w=tmp; }

  edge e; 

  if (v == w) // selfloops
  { point p0 = get_position(v);
    forall_inc_edges(e,v,w)
    { list<point>& P = get_poly(e);
      if (P.size() > 3) continue;
      double d1 = 2.0 * get_radius1(v);
      double d2 = 2.0 * get_radius2(v);
      P.pop();
      point q; 
      if (P.size() > 1) q = P.pop();
      if (q.xcoord() < p0.xcoord()) d1 = -d1;
      if (q.ycoord() < p0.ycoord()) d2 = -d2;
      P.clear();
      P.append(p0);
      P.append(p0.translate( 0,d2));
      P.append(p0.translate(d1,d2));
      P.append(p0.translate(d1, 0));
      P.append(p0);
      //e_info[e].shape = circle_edge;
    }
  }
      
/*
{
 point v_pos = get_position(v);
 point w_pos = get_position(w);

 forall_inc_edges(e,v,w)
 { list<point>& P=get_poly(e);
   list_item it1 = P.first();
   list_item it2 = P.last();
   P[it1] = v_pos;
   P[it2] = w_pos;
  }

 forall_inc_edges(e,w,v)
 { list<point>& P=get_poly(e);
   list_item it1 = P.first();
   list_item it2 = P.last();
   P[it1] = w_pos;
   P[it2] = v_pos;
  }
 return;
}
*/

//double pix = win_p->pix_to_real(1);

  double v_r1 = get_radius1(v); 
  double v_r2 = get_radius2(v); 
  double w_r1 = get_radius1(w); 
  double w_r2 = get_radius2(w); 


  list<list<point>*> auto_list_vw;
  list<list<point>*> auto_list_wv;
  
  forall_inc_edges(e,v,w)
  { 
    list<point>& P=get_poly(e);

    if (ortho_mode) 
    { auto_list_vw.append(&P);
      continue;
     }

    point sa = e_info[e].s_anchor;
    point ta = e_info[e].t_anchor;

    if (sa == point(0,0) && ta == point(0,0) && P.size() <= 2)
    { auto_list_vw.append(&P);
      continue;
     }


    double xs = sa.xcoord();
    double ys = sa.ycoord();
    if (xs >= +1) xs = +0.99;
    if (xs <= -1) xs = -0.99;
    if (ys >= +1) ys = +0.99;
    if (ys <= -1) ys = -0.99;
    sa = point(xs,ys);

    double xt = ta.xcoord();
    double yt = ta.ycoord();
    if (xt >= +1) xt = +0.99;
    if (xt <= -1) xt = -0.99;
    if (yt >= +1) yt = +0.99;
    if (yt <= -1) yt = -0.99;
    ta = point(xt,yt);

    point v_pos = get_position(v).translate(sa.xcoord()*v_r1,sa.ycoord()*v_r2);
    point w_pos = get_position(w).translate(ta.xcoord()*w_r1,ta.ycoord()*w_r2);

    list_item it1 = P.first();
    list_item it4 = P.last();

    if (P.size() > 2) // polygon edge
    { if (get_shape(e) == circle_edge)
      { list_item it2 = P.succ(it1);
        list_item it3 = P.succ(it2);
        P[it1] = compute_leaving_circle_point(v,v_pos,P[it2],P[it3]);
        P[it4] = compute_leaving_circle_point(w,w_pos,P[it2],P[it1]);
        while (P.succ(it2) != it4) P.del_item(P.succ(it2));
       }
      else
      { list_item it2 = P.succ(it1);
        list_item it3 = P.pred(it4);
        P[it1] = compute_leaving_point(v,v_pos,P[it2]);
        P[it4] = compute_leaving_point(w,w_pos,P[it3]);
       }
     }
    else // edge anchors , no bends
    { P[it1] = compute_leaving_point(v,v_pos,w_pos);
      P[it4] = compute_leaving_point(w,w_pos,v_pos);
     }
   }


  if (v != w)
  forall_inc_edges(e,w,v)
  { 
    list<point>& P=get_poly(e);

    if (ortho_mode) 
    { auto_list_wv.append(&P);
      continue;
     }

    point sa = e_info[e].s_anchor;
    point ta = e_info[e].t_anchor;

    if (sa == point(0,0) && ta == point(0,0) && P.size() <= 2)
    { auto_list_wv.append(&P);
      continue;
     }

    double xs = sa.xcoord();
    double ys = sa.ycoord();
    if (xs >= +1) xs = +0.99;
    if (xs <= -1) xs = -0.99;
    if (ys >= +1) ys = +0.99;
    if (ys <= -1) ys = -0.99;
    sa = point(xs,ys);

    double xt = ta.xcoord();
    double yt = ta.ycoord();
    if (xt >= +1) xt = +0.99;
    if (xt <= -1) xt = -0.99;
    if (yt >= +1) yt = +0.99;
    if (yt <= -1) yt = -0.99;
    ta = point(xt,yt);

    point w_pos = get_position(w).translate(sa.xcoord()*w_r1,sa.ycoord()*w_r2);
    point v_pos = get_position(v).translate(ta.xcoord()*v_r1,ta.ycoord()*v_r2);

/*

    point w_pos = get_position(w).translate(sa.xcoord()*get_radius1(w),
                                            sa.ycoord()*get_radius2(w));

    point v_pos = get_position(v).translate(ta.xcoord()*get_radius1(v),
                                            ta.ycoord()*get_radius2(v));
*/

    list_item it1 = P.first();
    list_item it2 = P.last();

    if (P.size() > 2) // polygon edge
    { list_item it3 = P.succ(it1);
      list_item it4 = P.pred(it2);
      if (get_shape(e) == circle_edge)
      { P[it1] = compute_leaving_circle_point(w,w_pos,P[it3],P[it2]);
        P[it2] = compute_leaving_circle_point(v,v_pos,P[it4],P[it1]);
       }
      else
      { P[it1] = compute_leaving_point(w,w_pos,P[it3]);
        P[it2] = compute_leaving_point(v,v_pos,P[it4]);
       }
     }
    else // edge anchors , no bends
    { P[it1] = compute_leaving_point(w,w_pos,v_pos);
      P[it2] = compute_leaving_point(v,v_pos,w_pos);
     }

   }


  point v_pos = get_position(v);
  point w_pos = get_position(w);

  unsigned s_vw = auto_list_vw.size();
  unsigned s_wv = auto_list_wv.size();
  unsigned s = s_vw + s_wv;

  if (s == 0) return;

  point p1,p2;

  bool orth_overlap = ortho_mode && compute_ortho_leaving_points(v,w,p1,p2) > 0;

  if (s == 1 || (ortho_mode && !orth_overlap && s_vw == 1 && s_wv == 1) ) 
  {
    while (s_vw+s_wv > 0)
    { node x,y; 
      point xp,yp;
      list<point>* P;
  
      if (s_vw > 0) 
        { P = auto_list_vw.pop();
          x = v; xp = v_pos;
          y = w; yp = w_pos;
          s_vw--;
         }
      else
        { P = auto_list_wv.pop();
          x = w; xp = w_pos;
          y = v; yp = v_pos;
          s_wv--;
         }
  
      double dx  = yp.xcoord() - xp.xcoord();
      double dy  = yp.ycoord() - xp.ycoord();
  
      P->clear();
      if (ortho_mode) 
        if (compute_ortho_leaving_points(x,y,xp,yp) > 0)
          { P->push(yp);
            P->push(xp);
           }
        else
          { point bend;
            if (ortho_first_dir != 0)
              { if (ortho_first_dir == 1) bend = xp.translate(dx,0);
                if (ortho_first_dir == 2) bend = xp.translate(dy,0);
               }
            else
              if (fabs(dx) > fabs(dy))
                 bend = xp.translate(dx,0);
              else
                 bend = xp.translate(0,dy);
            P->push(compute_leaving_point(y,yp,bend));
            P->push(bend);
            P->push(compute_leaving_point(x,xp,bend));
           }
      else
      { P->push(compute_leaving_point(y,yp,xp));
        P->push(compute_leaving_point(x,xp,yp));
       }
  
    }
    return;
  }

  double dx = v_pos.xcoord() - w_pos.xcoord();
  double dy = v_pos.ycoord() - w_pos.ycoord();

  double max_dist=max_edge_distance(v,w);

  if (ortho_mode)
  { double d = compute_ortho_leaving_points(v,w,v_pos,w_pos);
    if (d > 0) max_dist = d;
   }

  line l0(v_pos,w_pos);	// embed all edges around l0


  if (win_p->real_to_pix(fabs(dx)) < 1) l0 = line(v_pos,vector(0.0,1.0));


  double h = (edge_distance*(s-1) > max_dist) ? max_dist/s : edge_distance;

  vector vec = (w_pos - v_pos).rotate90();
  double v_len = vec.length();

  if (win_p->real_to_pix(v_len) >= 1) vec = (h/v_len) * vec;


  double t = 0.5*(s-1);
  line l = l0.translate(t*vec);

  while( !auto_list_vw.empty() ) 
  { list<point>* P = auto_list_vw.pop();
    P->clear();
    P->append(first_hit(v,l, dx>0, dy>0));
    P->append(first_hit(w,l, dx<=0, dy<=0));
    l = l.translate(-vec);
  }

  while( !auto_list_wv.empty() ) 
  { list<point>* P = auto_list_wv.pop();
    P->clear();
    P->push(first_hit(v,l, dx>0, dy>0));
    P->push(first_hit(w,l, dx<=0, dy<=0));
    l = l.translate(-vec);
  }
}

//----------------------------------------------------------------------------

void GraphWin::embed_node_with_edges(node u) 
{ graph& G = *gr_p;
  map<node,bool> visited(false);
  edge e;
  forall_adj_edges(e,u) 
  { node w = G.opposite(u,e);
    if (visited[w]) continue;
    embed_edges(u,w);
    visited[w] = true;
  }
  forall_in_edges(e,u) 
  { node w = G.opposite(u,e);
    if (visited[w]) continue;
    embed_edges(u,w);
    visited[w] = true;
  }
}

//----------------------------------------------------------------------------

void GraphWin::embed_edges() 
{ node_array<bool> considered(*gr_p,false);
  node u;
  forall_nodes(u,*gr_p) 
  { if (considered[u]) continue;
    considered[u]=true;
    embed_node_with_edges(u);
  }
  edges_embedded=true;
}

//----------------------------------------------------------------------------

void GraphWin::move_edge(edge e, const vector& trans) 
{ list<point>& P = get_poly(e);
  list_item it;
  forall_items(it,P) P[it] = P[it].translate(trans);
}

void GraphWin::move_edge(edge e, const list<vector>& trans) 
{ list<point>& P = get_poly(e);
  list_item itv = trans.first();
  list_item it;
  forall_items(it,P) 
  { if (it == P.first() || it == P.last()) continue;
    P[it] = P[it].translate(trans[itv]);
    itv = trans.succ(itv);
   }
}

//----------------------------------------------------------------------------

void GraphWin::move_node(node v, const vector& trans) {
  point& p = n_info[v].pos;
  p = p.translate(trans);
  // move loops
  edge e;
  forall_adj_edges(e,v)
     if (target(e) == v) move_edge(e,trans);
}

//----------------------------------------------------------------------------

void GraphWin::move_nodes_with_edges(const list<node>& L, const vector& trans) {
  node v;
  forall(v,L) 
  { move_node(v,trans);
    edge e;
    forall_adj_edges(e,v) move_edge(e,trans);
  }
}

//----------------------------------------------------------------------------

void GraphWin::move_nodes_begin()
{ if (move_nodes_dest) delete move_nodes_dest;
  move_nodes_dest = new node_array<point>(*gr_p); 
  node v;
  forall_nodes(v,*gr_p) (*move_nodes_dest)[v] = get_position(v);
}

int GraphWin::move_nodes_end()
{
  move_nodes_trans = new node_array<vector>(*gr_p);

  list<node> L;

  node v;
  forall_nodes(v,*gr_p) 
  { if ((*move_nodes_dest)[v] == get_position(v)) continue; 
    L.append(v);
    (*move_nodes_trans)[v] = (*move_nodes_dest)[v] - get_position(v);
   }

  n_animation_start(L);

  move_nodes_current_step = 0;

  return animation_steps;
}


void GraphWin::move_nodes_step()
{ //double f = 1.0/(animation_steps+1);
  
  node v;
  forall(v,n_anim_list) 
  { motion_base* motion = node_motion[v];
    if (motion == 0) motion = node_default_motion;
    if (motion == 0) motion = &linear_move;
    vector vec = (*move_nodes_trans)[v];
    vector trans = motion->get_step(vec,animation_steps,
                                        move_nodes_current_step);
    move_node(v,trans);
   }
  n_animation_step();
  move_nodes_current_step++;
}


void GraphWin::move_nodes_finish()
{ node v;
  forall_nodes(v,*gr_p) n_info[v].pos=(*move_nodes_dest)[v];
  n_animation_step();
  n_animation_end();
  delete move_nodes_dest;
  delete move_nodes_trans;
  move_nodes_dest = 0;
  move_nodes_trans = 0;
}



void GraphWin::move_nodes(const node_array<point>& dest) 
{
  unsigned anim = animation_steps;

  if (node_move == move_single_node)  
  { node v;
    forall_nodes(v,*gr_p) move_node_with_edges(v,dest[v],anim);
    edges_embedded=true;
    return;
   }    

  node_array<vector> p_trans(*gr_p);
  list<node> L;

  node v;
  forall_nodes(v,*gr_p) 
  { if (dest[v] == get_position(v)) continue; 
    L.append(v);
    p_trans[v] = (dest[v]-get_position(v))*(1.0/(anim+1));
   }


  if (L.length() > gr_p->number_of_nodes()/2)
  { // move entire graph
    window& W = get_window();

    W.start_buffering();

    do { forall(v,L) move_node(v,p_trans[v]);
         embed_edges();
         redraw();
         W.flush_buffer();
        } while (anim-- > 0);

    forall(v,L) n_info[v].pos=dest[v];
    embed_edges();
    redraw();
    W.flush_buffer();
    W.stop_buffering();
    return;
   }

   move_nodes_begin(); 
   forall_nodes(v,*gr_p) move_nodes_set_position(v,dest[v]);
   int n = move_nodes_end();
   while (n-- > 0) move_nodes_step();
   move_nodes_finish();
}



void GraphWin::set_layout_start(const node_array<point>& n_dest, 
                                const node_array<double>& n_rad1, 
                                const node_array<double>& n_rad2, 
                                const edge_array<list<point> >& e_dest, 
                                const edge_array<point>& e_sanch, 
                                const edge_array<point>& e_tanch, 
                                list<node>& L, list<edge>& E,
                                node_array<vector>& p_trans,
                                node_array<double>& r1_trans,
                                node_array<double>& r2_trans,
                                edge_array<list<vector> >& e_trans,
                                edge_array<vector>& sa_trans,
                                edge_array<vector>& ta_trans)
{
   window& W = get_window();
   W.start_buffering();

   unsigned anim = animation_steps;

   if (anim <= 0) anim = 1;

   L.clear();
   E.clear();

   p_trans.init(*gr_p);
   r1_trans.init(*gr_p);
   r2_trans.init(*gr_p);
   e_trans.init(*gr_p);
   sa_trans.init(*gr_p);
   ta_trans.init(*gr_p);


   node v;
   forall_nodes(v,*gr_p) 
   { if (n_dest[v] == get_position(v) && n_rad1[v] == get_radius1(v) 
                                      && n_rad2[v] == get_radius2(v)) continue; 
     L.append(v);
     p_trans[v] = (n_dest[v]-get_position(v))*(1.0/anim);
     r1_trans[v] = (n_rad1[v] - get_radius1(v))*(1.0/anim);
     r2_trans[v] = (n_rad2[v] - get_radius2(v))*(1.0/anim);
    }


   edge e;
   forall_edges(e,*gr_p) 
   { list<point> old_bends = get_bends(e);
     list<point> new_bends = e_dest[e];

     node a = source(e);
     node b = target(e);

     if (old_bends.empty() && new_bends.empty() 
                           && get_source_anchor(e) == e_sanch[e]
                           && get_target_anchor(e) == e_tanch[e]) continue;

     sa_trans[e] = (e_sanch[e]-get_source_anchor(e))*(1.0/anim);
     ta_trans[e] = (e_tanch[e]-get_target_anchor(e))*(1.0/anim);

     E.append(e);

     int diff = new_bends.length() - old_bends.length();

     if (diff < 0) 
        { // fewer bends: append dummys to list of new bends
          int n = -diff;
          point A = n_dest[a];
          point B = n_dest[b];
          if (!new_bends.empty()) A = new_bends.tail();
          vector vec = (B-A)/(n+1);
          while (n-- > 0) 
          { A = A + vec;
            new_bends.append(A);
           }
         }
     else
        if (diff > 0) 
        { // more bends: append dummys to list of old bends
          int n = diff;
          point A = get_position(a);
          point B = get_position(b);
          if (!old_bends.empty()) A = old_bends.tail();
          vector vec = (B-A)/(n+1);
          while (n-- > 0) 
          { A = A + vec;
            old_bends.append(A);
           }
          list<point>& L=get_poly(e);
          L = old_bends;
          L.push(get_position(a));
          L.append(get_position(b));
         }

     list_item it = new_bends.first();
     point p;
     forall(p,old_bends)
     { point q = new_bends[it];
       it = new_bends.succ(it);
       e_trans[e].append((q-p)*(1.0/anim));
      }
    }

   W.get_button(); // allow interaction
}




void GraphWin::set_layout_step(const list<node>& L, const list<edge>& E,
                               const node_array<vector>& p_trans,
                               const node_array<double>& r1_trans,
                               const node_array<double>& r2_trans,
                               const edge_array<list<vector> >& e_trans,
                               const edge_array<vector>&        sa_trans,
                               const edge_array<vector>&        ta_trans)
{
   window& W = get_window();
   node v;
   forall(v,L) 
   { // move_node(v,p_trans[v],move_loops=false);
     point& pos_v = n_info[v].pos;
     float& r1_v = n_info[v].r1;
     float& r2_v = n_info[v].r2;
     pos_v = pos_v + p_trans[v];
     r1_v = float(r1_v + r1_trans[v]);
     r2_v = float(r2_v + r2_trans[v]);
    }

   edge e;
   forall(e,E) 
   { point& sa = e_info[e].s_anchor; 
     point& ta = e_info[e].t_anchor; 
     sa = sa + sa_trans[e];
     ta = ta + ta_trans[e];
     move_edge(e,e_trans[e]);
   }
   embed_edges();
   redraw();
   W.flush_buffer();

   W.get_button(); // allow interaction
 }


void GraphWin::set_layout_end(const list<node>& L,
                              const list<edge>& E,
                              const node_array<point>& n_dest, 
                              const node_array<double>& n_rad1, 
                              const node_array<double>& n_rad2, 
                              const edge_array<list<point> >& e_dest, 
                              const edge_array<point>& e_sanch, 
                              const edge_array<point>& e_tanch)
{
   window& W = get_window();
   node v;
   forall(v,L) 
   { n_info[v].pos= n_dest[v];
     n_info[v].r1 = float(n_rad1[v]);
     n_info[v].r2 = float(n_rad2[v]);
    }

   edge e;
   forall(e,E) 
   { set_source_anchor(e,e_sanch[e]);
     set_target_anchor(e,e_tanch[e]);
     set_bends(e,e_dest[e]);
    }

   embed_edges();
   redraw();
   W.flush_buffer();
   W.stop_buffering();

   W.get_button(); // allow interaction
}




//----------------------------------------------------------------------------

void GraphWin::move_node_with_edges(node v, const point& new_p, unsigned anim) 
{
  if (anim == 0) anim = 1;

  point& p = n_info[v].pos;

  if (p == new_p) return;

  vector trans((new_p-p)*(1.0/anim));

  list<edge> loops;
  edge e;
  forall_adj_edges(e,v)
      if (target(e) == v) loops.append(e); 

  n_animation_start(v);
  while (anim-- > 0)
  { p = p + trans;
    forall(e,loops) move_edge(e,trans);
    embed_edges(v,v);
    n_animation_step();
   }
  p = new_p;
  n_animation_step();
  n_animation_end();
}


//----------------------------------------------------------------------------


void GraphWin::fill_win_params(double wx0,double wy0, double wx1, double wy1,
                               double x0,double y0,double x1, double y1,
                               double& dx, double& dy, double& fx, double& fy)
{ double DX = x1-x0;
  double DY = y1-y0;
  fx = (DX > 0) ? (wx1 - wx0)/DX : 1.0;
  fy = (DY > 0) ? (wy1 - wy0)/DY : 1.0;
  dx = (wx0+wx1 - (x0+x1)*fx)/2;
  dy = (wy0+wy1 - (y0+y1)*fy)/2;
 }

void GraphWin::fill_win_params(double wx0,double wy0, double wx1, double wy1,
                               node_array<double>& xpos,
                               node_array<double>& ypos,
                               edge_array<list<double> >& xbends,
                               edge_array<list<double> >& ybends,
                               double& dx, double& dy, double& fx, double& fy)
{ 
  graph& G = get_graph();
  node_array<double> xrad(G,0);
  node_array<double> yrad(G,0);
  fill_win_params(wx0,wy0,wx1,wy1,xpos,ypos,xrad,yrad,xbends,ybends,
                                                             dx,dy,fx,fy);
}


void GraphWin::fill_win_params(double wx0,double wy0, double wx1, double wy1,
                               node_array<double>& xpos,
                               node_array<double>& ypos,
                               node_array<double>& xrad,
                               node_array<double>& yrad,
                               edge_array<list<double> >& xbends,
                               edge_array<list<double> >& ybends,
                               double& dx, double& dy, double& fx, double& fy)
{ 
  graph& G = get_graph();

  node v = G.first_node();

  if (v == nil) return;

  double xmin = xpos[v] - xrad[v];
  double xmax = xpos[v] + xrad[v];
  double ymin = ypos[v] - yrad[v];
  double ymax = ypos[v] + yrad[v];

  forall_nodes(v,G) 
  { double x0 = xpos[v] - xrad[v];
    double x1 = xpos[v] + xrad[v];
    double y0 = ypos[v] - yrad[v];
    double y1 = ypos[v] + yrad[v];
    if (x0 < xmin) xmin  = x0; 
    if (y0 < ymin) ymin  = y0; 
    if (x1 > xmax) xmax  = x1; 
    if (y1 > ymax) ymax  = y1; 
   }

  edge e;
  forall_edges(e,G) 
  { list<double>& X = xbends[e];
    list<double>& Y = ybends[e];
    double x;
    forall(x,X) 
    { if (x < xmin) xmin = x;
      if (x > xmax) xmax = x;
     }
    double y;
    forall(y,Y)
    { if (y < ymin) ymin = y;
      if (y > ymax) ymax = y;
     }
  }

  double f1,f2;
  fill_win_params(wx0,wy0,wx1,wy1,xmin,ymin,xmax,ymax,dx,dy,f1,f2);

/*
  forall_nodes(v,G) { xrad[v] *= f1; yrad[v] *= f2; }
*/

  if (&fx != &fy)
  { fx = f1;
    fy = f2;
    return;
   }

  // &fx == &fy

  if (f1 > f2)
  { fx = f2;
    //dx += (f1*xmax -f2*xmax)/2;
    dx += (f1-f2)*xmin;
   }
  else
  { fx = f1;
    //dy += (f2*ymax -f1*ymax)/2;
    dy += (f2-f1)*ymin;
   }
}


void GraphWin::fill_win_params(node_array<double>& xpos,
                               node_array<double>& ypos,
                               edge_array<list<double> >& xbends,
                               edge_array<list<double> >& ybends,
                               double& dx, double& dy, double& fx, double& fy)
{ 
  graph& G = get_graph();
  node_array<double> xrad(G,0);
  node_array<double> yrad(G,0);

/*
  node v;
  forall_nodes(v,G)
  { xrad[v] = get_radius1(v);
    yrad[v] = get_radius2(v);
   }
*/

  fill_win_params(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);
}


void GraphWin::fill_win_params(node_array<double>& xpos,
                               node_array<double>& ypos,
                               node_array<double>& xrad,
                               node_array<double>& yrad,
                               edge_array<list<double> >& xbends,
                               edge_array<list<double> >& ybends,
                               double& dx, double& dy, double& fx, double& fy)
{ 
  window& W = get_window();

/*
  double xmargin = W.pix_to_real(W.width()/12);
  double ymargin = W.pix_to_real(W.height()/12);
*/

  double xmargin = W.pix_to_real(W.width()/13);
  double ymargin = W.pix_to_real(W.height()/13);


  double margin = max(xmargin,ymargin);

  double wx0 = get_xmin() + margin;
  double wx1 = get_xmax() - margin;
  double wy0 = get_ymin() + margin;
  double wy1 = get_ymax() - margin;

  wy0 += W.pix_to_real(status_win_height);

  while (wx0 >= wx1 || wy0 >= wy1)
  { wx0 -= xmargin;
    wx1 += xmargin;
    wy0 -= ymargin;
    wy1 += ymargin;
   }

 fill_win_params(wx0,wy0,wx1,wy1,xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);

}





void GraphWin::transform_layout(node_array<double>& xpos,
                                node_array<double>& ypos,
                                node_array<double>& xrad,
                                node_array<double>& yrad,
                                edge_array<list<double> >& xbends,
                                edge_array<list<double> >& ybends,
                                double dx, double dy, double fx, double fy)
{ graph& G = get_graph();

  node v;
  forall_nodes(v,G) 
  { xpos[v]=dx+xpos[v]*fx;
    ypos[v]=dy+ypos[v]*fy;
    xrad[v]*=fx;
    yrad[v]*=fy;
   }

  edge e;
  forall_edges(e,G) 
  { list<double>&  X = xbends[e];
    list<double>&  Y = ybends[e];
    list_item itx = X.first();
    list_item ity = Y.first();
    while (itx) 
    { double& xx = X[itx];
      double& yy = Y[ity];
      xx = fx*xx+dx;
      yy = fy*yy+dy;
      itx = X.succ(itx);
      ity = Y.succ(ity);
     }
  }
}


void GraphWin::transform_layout(node_array<double>& xpos,
                                node_array<double>& ypos,
                                edge_array<list<double> >& xbends,
                                edge_array<list<double> >& ybends,
                                double dx, double dy, double fx, double fy)
{ graph& G = get_graph();
  node_array<double> xrad(G);
  node_array<double> yrad(G);
  transform_layout(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy); 
 }


void GraphWin::transform_coords(node_array<double>& xpos, 
                                node_array<double>& ypos,
                                edge_array<list<double> >& xbends,
                                edge_array<list<double> >& ybends,
                                double dx, double dy, double fx, double fy)
{ transform_layout(xpos,ypos,xbends,ybends,dx,dy,fx,fy); }



void GraphWin::adjust_coords_to_box(node_array<double>& xpos,
                                    node_array<double>& ypos,
                                    edge_array<list<double> >& xbends,
                                    edge_array<list<double> >& ybends,
                                    double wx0, double wy0, 
                                    double wx1, double wy1)
{ double dx,dy,fx,fy;
  fill_win_params(wx0,wy0,wx1,wy1,xpos,ypos,xbends,ybends,dx,dy,fx,fy);
  transform_coords(xpos,ypos,xbends,ybends,dx,dy,fx,fy);
}


void GraphWin::adjust_coords_to_box(node_array<double>& xpos,
                                    node_array<double>& ypos,
                                    double wx0, double wy0, 
                                    double wx1, double wy1)
{ graph& G = get_graph();
  edge_array<list<double> > xbends(G);
  edge_array<list<double> > ybends(G);
  adjust_coords_to_box(xpos,ypos,xbends,ybends,wx0,wy0,wx1,wy1);
 }



void GraphWin::adjust_coords_to_win(node_array<double>& xpos,
                                    node_array<double>& ypos,
                                    edge_array<list<double> >& xbends,
                                    edge_array<list<double> >& ybends)
{ double dx,dy,fx,fy;
  fill_win_params(xpos,ypos,xbends,ybends,dx,dy,fx,fy);
  transform_layout(xpos,ypos,xbends,ybends,dx,dy,fx,fy);
}


void GraphWin::adjust_coords_to_win(node_array<double>& xpos,
                                    node_array<double>& ypos)
{ graph& G = get_graph();
  edge_array<list<double> > xbends(G);
  edge_array<list<double> > ybends(G);
  adjust_coords_to_win(xpos,ypos,xbends,ybends);
 }



void GraphWin::place_into_box(double bx0, double by0, double bx1, double by1)
{  
   graph& G = get_graph();

   double x0,y0,x1,y1,r1,r2; 
   get_extreme_coords(x0,y0,x1,y1,r1,r2);

   bx0 += r1;
   bx1 -= r1;
   by0 += r2;
   by1 -= r2;

   double dx,dy,fx,fy;
   fill_win_params(bx0,by0,bx1,by1,x0,y0,x1,y1,dx,dy,fx,fy);

   node_array<point> pos(G);

   bool snap = (grid_dist != 0);

   node v;
   forall_nodes(v,G)
   { point p = get_position(v);
     p = point(dx+fx*p.xcoord(),dy+fy*p.ycoord());
     if (snap) p = snap_point(p);
     pos[v] = p;
    }

   edge_array<list<point> > bends(G);

   edge e;
   forall_edges(e,G)
   { bends[e] = get_bends(e);
     list<point>& L = bends[e];
     list_item it;
     forall_items(it,L)
     { point p = point(dx+fx*L[it].xcoord(),dy+fy*L[it].ycoord());
       if (snap) p = snap_point(p);
       L[it] = p;
      }
   }


   list_item it;
   forall_items(it,shape_list)
   { shape& cb = shape_list[it];

     if (cb.poly.empty())
     { //circle
       point c = cb.circ.center().translate(dx,dy);
       double r = dx*cb.circ.radius();
       cb.circ = circle(c,r);
       continue;
       }

     list<point> L;
     point p;
     forall(p,cb.poly.vertices())
        L.append(point(dx+fx*p.xcoord(), dy+fy*p.ycoord()));
     cb.poly = polygon(L);
    }

   set_layout(pos,bends,false);
}


void GraphWin::place_into_win()
{ double d = get_window().pix_to_real(50);
  place_into_box(get_xmin()+d,get_ymin()+d,get_xmax()-d,get_ymax()-d); 
 }
   


void GraphWin::remove_bends(edge e) 
{ if (get_edge_points(e).size() != 2)
  { list<point> empty_list;
    set_bends(e,empty_list);
   }
}


void GraphWin::remove_bends() 
{ bool b = set_flush(false);
  set_layout();
  set_flush(b);
  if (b) redraw();
}


void GraphWin::reset_edge_anchors() 
{ bool b = set_flush(false);
  edge e;
  forall_edges(e,*gr_p) 
  { set_source_anchor(e,point(0,0));
    set_target_anchor(e,point(0,0));
   }
  set_flush(b);
  if (b) redraw();
}


// planar map support


void GraphWin::sort_edges_by_angle()
{ graph& G  = get_graph();
  edge_array<double> dx(G);
  edge_array<double> dy(G);
  edge e;
  forall_edges(e,G) 
  { list<point>& pol = get_poly(e);
    list_item it = pol.last();
    if (pol.length() > 2) it = pol.succ(pol.first());
    point ps = get_position(source(e));
    point pt = pol[it];
    dx[e] = pt.xcoord() - ps.xcoord();
    dy[e] = pt.ycoord() - ps.ycoord();
  }
  SORT_EDGES(G, dx, dy); 
}

LEDA_END_NAMESPACE
