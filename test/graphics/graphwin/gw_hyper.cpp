/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_hyper.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


/*
#include <LEDA/point.h>
#include "mygraphwin.h"
*/

#include <LEDA/graphwin.h>

using namespace leda;


class hp_point  : public point
{

public:

hp_point() : point() {}
hp_point(const point& p): point(p) {}
hp_point(double x, double y): point(x,y) {}
hp_point(const hp_point& p) : point(p) {}
~hp_point() {}
hp_point& operator=(const hp_point& p) { point::operator=(p); return *this; }


double  distance_hp(const hp_point& q) const 
{ double a1=xcoord();
  double a2=ycoord();
  double b1=q.xcoord();
  double b2=q.ycoord();
  double n=(1-a1*b1-a2*b2);
  double z1=sqrt(1-a1*a1-a2*a2);
  double z2=sqrt(1-b1*b1-b2*b2);
  double e=(double)cosh((float)(n/(z1*z2)));
  return e;
}

int operator==(const hp_point& q) const { return point::operator==(q); }

int operator!=(const hp_point& q)  const { return !operator==(q);}



public:
	
double re() const { return xcoord(); }
double im() const { return ycoord(); }


static hp_point add( hp_point c1, hp_point c2 ) 
{ return hp_point (c1.re() + c2.re(), c1.im() + c2.im()); }

static hp_point sub( hp_point c1, hp_point c2 ) 
{ return hp_point(c1.re() - c2.re(), c1.im() - c2.im()); }

static hp_point mul( hp_point c1, hp_point c2 ) 
{ return hp_point(c1.re()*c2.re() - c1.im()*c2.im(),
		  c1.im()*c2.re() + c1.re()*c2.im()); }

static hp_point div( hp_point a, hp_point b ) 
{ double divisor = b.re()*b.re() + b.im()*b.im();
  return hp_point((a.re()*b.re() + a.im()*b.im())/divisor,
		 (-a.re()*b.im() + a.im()*b.re())/divisor);
 }

static hp_point cnj( hp_point c1) 
{ return hp_point( c1.re(),-c1.im()); }


hp_point focus(hp_point p) 
{ hp_point q(xcoord(),ycoord());
  hp_point r(1,0); //cos(angle),sin(angle));
  hp_point f1 = add(mul(r,q),p);
  hp_point f2 = add(hp_point(1,0),mul(cnj(p),q));
  return div(f1,f2);
 }


friend __exportF ostream& operator<<(ostream& O, const hp_point& s) 
{ return O << "[r "<<s.re()<< "/i "<< s.im()<<"]"; }
	
friend __exportF istream& operator>>(istream& I, hp_point& s) 
{ return I; }


};



static double hyper_x0 = 0;
static double hyper_y0 = 0;
static double hyper_x1 = 0;
static double hyper_y1 = 0;

static point int2ext(hp_point p) 
{ double x = p.xcoord();
  double y = p.ycoord();
  double w2 = (hyper_x1-hyper_x0)/2;
  double h2 = (hyper_y1-hyper_y0)/2;
  return point(w2*x+w2+hyper_x0, h2*y+h2+hyper_y0);     
}


static hp_point ext2int(point p) 
{ double w2 = (hyper_x1-hyper_x0)/2;
  double h2 = (hyper_y1-hyper_y0)/2;
  double x = (p.xcoord()-hyper_x0-w2)/w2;
  double y = (p.ycoord()-hyper_y0-h2)/h2;
  double fx=1; if (x<0) { fx=-1; x=-x; }
  double fy=1; if (y<0) { fy=-1; y=-y; }
  return hp_point(fx*x*x,fy*y*y); 
}


static double hp_x0 = 0;
static double hp_y0 = 0;

static void hp_bg_draw(window* wp, double, double, double, double)
{ 
  double r1 = (hyper_x1-hyper_x0)/2;
  double r2 = (hyper_y1-hyper_y0)/2;
  double x = hyper_x0 + r1;
  double y = hyper_y0 + r2;
  if (r1 > 0 && r2 > 0) 
  { r1 -= wp->pix_to_real(2);
    r2 -= wp->pix_to_real(2);
    wp->draw_ellipse(x,y,r1,r2,blue);
    wp->draw_vline(hp_x0,grey2);
    wp->draw_hline(hp_y0,grey2);
   }
}

void hyperbolic_view(GraphWin& gw, double x, double y,
                     const node_array<point>&  AP,
                     const node_array<double>& R1,
                     const node_array<double>& R2,
                     const edge_array<list<point> >& EB)
{
  hp_point pf = hp_point(ext2int(point(x,y))); 

  hp_x0 = x;
  hp_y0 = y;

  graph& G = gw.get_graph();

  node_array<point>  pos(G);
  node_array<double> rad1(G);
  node_array<double> rad2(G);
  edge_array<list<point> > bends(G);
  edge_array<point> sanch(G);
  edge_array<point> tanch(G);

  edge e;
  forall_edges(e,G) 
  { list<point>& LN = bends[e];
    point dp;
    forall(dp,EB[e]) 
    { hp_point hp2(ext2int(dp));
      hp2=hp2.focus(pf);
      LN.append(int2ext(hp2));
     }

    sanch[e] = gw.get_source_anchor(e);
    tanch[e] = gw.get_target_anchor(e);

/*
    hp_point p(ext2int(gw.get_source_anchor_pos(e)));
    p=p.focus(pf);
    gw.set_source_anchor_pos(e,int2ext(p));

    hp_point q(ext2int(gw.get_target_anchor_pos(e)));
    q=q.focus(pf);
    gw.set_target_anchor_pos(e,int2ext(q));
*/
  }

  node v;
  forall_nodes(v,G) 
  { hp_point p(ext2int(AP[v])); 
    pos[v] = int2ext(p.focus(pf));
    double f = 2/p.distance_hp(hp_point(0,0));
    rad1[v] = f*R1[v];
    rad2[v] = f*R2[v];
   }

  gw.set_layout(pos,rad1,rad2,bends,sanch,tanch);

}
    


void gw_hyperbol_embed(GraphWin& gw)
{
  gw.set_bg_redraw(hp_bg_draw);
  gw.zoom_graph();

  graph&  G = gw.get_graph();
  window& W = gw.get_window();


  node_array<point>  AP(G);
  node_array<double> R1(G);
  node_array<double> R2(G);
  edge_array<list<point> > EB(G);

  node v; 
  forall_nodes(v,G) 
  { AP[v]=gw.get_position(v);
    R1[v]=gw.get_radius1(v);
    R2[v]=gw.get_radius2(v);
   }

  edge e; 
  forall_edges(e,G) EB[e]=gw.get_bends(e);

  hyper_x0 = gw.get_xmin();
  hyper_x1 = gw.get_xmax();
  hyper_y0 = gw.get_ymin();
  hyper_y1 = gw.get_ymax();


  double x = (hyper_x0 + hyper_x1) / 2;
  double y = (hyper_y0 + hyper_y1) / 2;

  hyperbolic_view(gw,x,y,AP,R1,R2,EB);

  gw.set_flush(false);

  bool button_down=false; 
  double x0 = 0;
  double y0 = 0;

  W.disable_panel();

  gw.message("Hyperbolic View  (exit: right button)");
        
  while (true) 
  { 
    int key=0; 
    double xx,yy;
    int val = W.read_event(key,xx,yy);

    if (val==motion_event && button_down==true) 
    { double tx = x + (xx-x0);
      double ty = y + (yy-y0);
      hyperbolic_view(gw,tx,ty,AP,R1,R2,EB);
    }
    
    if (val==button_press_event) 
    { if (key == MOUSE_BUTTON(3)) break;
      x0 = xx;
      y0 = yy;
      //gw.set_bg_redraw(hp_bg_draw);
      button_down=true; 
     }

    if (val==button_release_event) 
    { x = x + (xx-x0);
      y = y + (yy-y0);
      //gw.set_bg_redraw(0);
      button_down=false; 
     }

    gw.redraw();
  }

  W.enable_panel();
  gw.message("");
  gw.set_bg_redraw(0);
  gw.redraw();
}



int main() 
{ 
  GraphWin gw;
  gw.display(window::center,window::center);

  while (gw.edit()) gw_hyperbol_embed(gw);

  return 0;
}

