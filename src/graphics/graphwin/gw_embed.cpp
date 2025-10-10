/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_embed.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/graph_draw.h>
#include <LEDA/graphics/d3_window.h>

#include "local.h"

LEDA_BEGIN_NAMESPACE

// simple embeddings

void gw_circle_embed(GraphWin& gw)
{ graph& G = gw.get_graph();

  if (G.empty()) return;

  node_array<double> xcoord(G);
  node_array<double> ycoord(G);

  point  m = point(0.5,0.5);
  double alpha = 0;
  double step  = 2*LEDA_PI/G.number_of_nodes();
  node v;
  forall_nodes(v,G) 
  { point p = m.translate_by_angle(alpha,0.5);
    xcoord[v] = p.xcoord();
    ycoord[v] = p.ycoord();
    alpha+=step;
  }

/*
  gw.adjust_coords_to_win(xcoord,ycoord);
*/

  double width  = 0.8*(gw.get_xmax() - gw.get_xmin());
  double height = 0.8*(gw.get_ymax() - gw.get_ymin());

  if (height < width) width=height;

  double x_center = (gw.get_xmax() - gw.get_xmin())/2;
  double y_center = (gw.get_ymax() - gw.get_ymin())/2;

  gw.adjust_coords_to_box(xcoord,ycoord,x_center-width/2,y_center-width/2,
                                        x_center+width/2,y_center+width/2);

  gw.set_layout(xcoord,ycoord); // removes bends
  gw.call_init_graph_handler();

  if (!gw.get_flush()) gw.redraw();
}


void gw_random_embed(GraphWin& gw)
{ graph& G = gw.get_graph();

  if (G.empty()) return;

  node_array<double> xcoord(G);
  node_array<double> ycoord(G);
  random_source ran;
  node v;
  forall_nodes(v,G)  ran >> xcoord[v] >> ycoord[v];
  gw.adjust_coords_to_win(xcoord,ycoord);
  gw.set_layout(xcoord,ycoord); // removes bends
  gw.call_init_graph_handler();

  if (!gw.get_flush()) gw.redraw();
}


void gw_straight_embed1( GraphWin& gw)
{ 
  graph& G=gw.get_graph();

  int n=G.number_of_nodes();
  if (n == 0) return;

  if (!PLANAR(G)) 
  { gw_test_planarity(gw);
    return;
   }

  // if (!gw.check_init_graph_handler()) return;

  list<edge> inserted_edges;
  Make_Biconnected(G,inserted_edges);
  Make_Bidirected(G,inserted_edges);
  PLANAR(G,true);

  node_array<double> xcoord(G);
  node_array<double> ycoord(G);

  STRAIGHT_LINE_EMBEDDING(G,xcoord,ycoord);

  // restore original graph
  edge e;
  forall(e,inserted_edges) G.del_edge(e);
  gw.update_graph();

  double rad = gw.get_window().pix_to_real(8);
  node_array<double> r(G,rad);
  edge_array<list<double> > b(G);
  edge_array<double> a(G,0); 

  gw.adjust_coords_to_win(xcoord,ycoord);
  gw.set_layout(xcoord,ycoord,r,r,b,b,a,a,a,a);
  gw.call_init_graph_handler();

  if (!gw.get_flush()) gw.redraw();
}


void gw_straight_embed2( GraphWin& gw)
{ 
  graph& G=gw.get_graph();

  int n=G.number_of_nodes();
  if (n == 0) return;

  if (!PLANAR(G)) 
  { gw_test_planarity(gw);
    return;
   }

  // if (!gw.check_init_graph_handler()) return;

  list<edge> inserted_edges;
  Make_Biconnected(G,inserted_edges);
  Make_Bidirected(G,inserted_edges);
  PLANAR(G,true);

  node_array<double> xcoord(G);
  node_array<double> ycoord(G);

  STRAIGHT_LINE_EMBEDDING2(G,xcoord,ycoord);

  // restore original graph
  edge e;
  forall(e,inserted_edges) G.del_edge(e);
  gw.update_graph();

  double rad = gw.get_window().pix_to_real(8);
  node_array<double> r(G,rad);
  edge_array<list<double> > b(G);
  edge_array<double> a(G,0); 

  gw.adjust_coords_to_win(xcoord,ycoord);
  gw.set_layout(xcoord,ycoord,r,r,b,b,a,a,a,a);
  gw.call_init_graph_handler();

  if (!gw.get_flush()) gw.redraw();
}



void gw_quod_embed(GraphWin& gw) 
{
  graph& G=gw.get_graph();

  if (G.empty()) return;

  if (!PLANAR(G)) 
  { gw_test_planarity(gw);
    return; 
   }

  bool save_flush = gw.get_flush();

  // reversal and parallel edges (will get the same polygons)

  Make_Simple(G);

  edge_array<edge> rev(G);
  Is_Bidirected(G,rev);

  edge e;
  forall_edges(e,G)
  { edge r = rev[e];
    if (r != nil) 
    { rev[r] = nil;
      G.del_edge(e);
     }
   }

  // Prepare for calling ORTHO_EMBEDDING

  list<edge> dummy_edges;
  Make_Connected(G,dummy_edges);
  Make_Bidirected(G,dummy_edges);
  G.make_planar_map();

  node_array<double> x;
  node_array<double> y;
  edge_array<list<double> > ex;
  edge_array<list<double> > ey;

  if (ORTHO_EMBEDDING(G,x,y,ex,ey) < 0) // something went wrong
  { gw.set_flush(save_flush);
    gw.redraw();
    return;
   }

  G.del_edges(dummy_edges);
  gw.update_edges();

  gw.adjust_coords_to_win(x,y,ex,ey);

  if (G.number_of_nodes() + G.number_of_edges() > 2*gw.get_max_move_items()) 
     gw.set_flush(false);

  gw.set_layout(x,y,ex,ey);

  if (!gw.get_flush()) gw.redraw();

  gw.set_flush(save_flush);

}


void gw_ortho_embed(GraphWin& gw) 
{
  graph& G = gw.get_graph();

  if (G.empty()) 
  { gw.redraw();
    return; 
   }

  if (!Is_Connected(G)) {
    gw_test_connectivity(gw);
    return;
  }

  if (!PLANAR(G)) 
  { gw_test_planarity(gw);
    return; 
   }

  node_array<double>        xpos(G);
  node_array<double>        ypos(G);
  node_array<double>        xrad(G);
  node_array<double>        yrad(G);
  edge_array<list<double> > xbends(G);
  edge_array<list<double> > ybends(G);
  edge_array<double>        xsan(G);
  edge_array<double>        ysan(G);
  edge_array<double>        xtan(G);
  edge_array<double>        ytan(G);

  bool ok = ORTHO_DRAW(G,xpos,ypos,xrad,yrad,xbends,ybends,xsan,ysan,xtan,ytan);

  if (!ok) return;

  gw.set_node_shape(rectangle_node);

  bool save_flush = gw.set_flush(false);

  gw.set_flush(save_flush);

  double dx,dy,fx,fy;
//gw.fill_win_params(xpos,ypos,xbends,ybends,dx,dy,fx,fy);
  gw.fill_win_params(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);

  gw.transform_layout(xpos,ypos,xbends,ybends,dx,dy,fx,fy);

  node v;
  forall_nodes(v,G)
  { xrad[v] *= fx;
    yrad[v] *= fy;
   }

  gw.set_layout(xpos,ypos,xrad,yrad,xbends,ybends,xsan,ysan,xtan,ytan);
  gw.set_flush(save_flush);
}



void gw_spring_embed(GraphWin& gw) { gw_spring_embed(gw,false); }

void gw_spring_embed(GraphWin& gw, bool rand)
{
  graph&  G = gw.get_graph();

  if (G.empty()) 
  { gw.redraw();
    return;
   }

  bool save_flush = gw.get_flush();
  gw.set_flush(false);

  double xleft  = gw.get_xmin();
  double xright = gw.get_xmax();
  double ybottom= gw.get_ymin();
  double ytop   = gw.get_ymax();


  edge_array<list<double> > b(G);
  edge_array<double> a(G,0); 
  node_array<double> xpos(G);
  node_array<double> ypos(G);

  double rad = DefNodeRadius1;
  node_array<double> r(G,rad);

  node v;
  forall_nodes(v,G) gw.set_radius(v,rad); 
  //gw.redraw();

  if (rand)
  { random_source ran;
    forall_nodes(v,G)  ran >> xpos[v] >> ypos[v];
    gw.adjust_coords_to_win(xpos,ypos);
   }
  else
  { forall_nodes(v,G) 
    { //r[v] = gw.get_radius1(v);
      point p = gw.get_position(v);
      xpos[v] = p.xcoord();
      ypos[v] = p.ycoord();
    }
  }


/*
  // problem: undefined positions e.g. when starting with grid layout
  D2_SPRING_EMBEDDING(G,xpos,ypos,xleft,xright,ybottom,ytop,250);
*/

  SPRING_EMBEDDING(G,xpos,ypos,xleft,xright,ybottom,ytop,500);
  gw.adjust_coords_to_win(xpos,ypos);

  gw.set_flush(save_flush);
  gw.set_layout(xpos,ypos,r,r,b,b,a,a,a,a);
  if (!gw.get_flush()) gw.redraw();
}



void gw_d2_spring_embed(GraphWin& gw, bool rand)
{
  graph&  G = gw.get_graph();

  double xleft  = gw.get_xmin();
  double xright = gw.get_xmax();
  double ybottom= gw.get_ymin();
  double ytop   = gw.get_ymax();

  node_array<double>    xpos(G);
  node_array<double>    ypos(G);
 
  node v;

  if (rand)
  { random_source ran;
    forall_nodes(v,G)  ran >> xpos[v] >> ypos[v];
   }
  else
   forall_nodes(v,G) 
   { point p = gw.get_position(v);
     xpos[v] = p.xcoord();
     ypos[v] = p.ycoord();
    }
   
 
  D2_SPRING_EMBEDDING(G,xpos,ypos,xleft,xright,ybottom,ytop,500);
  gw.set_layout(xpos,ypos); // removes bends
  if (!gw.get_flush()) gw.redraw();
}


void gw_d3_spring_embed(GraphWin& gw)
{
  graph&  G = gw.get_graph();
  window& W = gw.get_window();

  double xmin = gw.get_xmin(); 
  double xmax = gw.get_xmax(); 
  double ymin = gw.get_ymin(); 
  double ymax = gw.get_ymax(); 
  double zmin = xmin;
  double zmax = xmax;

  gw.save_node_attributes();
  gw.save_edge_attributes();

  gw.remove_bends();

  node_array<double> xpos(G,0);
  node_array<double> ypos(G,0);
  node_array<double> zpos(G,0);

  node v;
  forall_nodes(v,G)
  { point p = gw.get_position(v);
    xpos[v] = p.xcoord();
    ypos[v] = p.ycoord();
    zpos[v] = (xmax - xmin) * rand_int(-1000,1000)/5000.0;
   }
 
  D3_SPRING_EMBEDDING(G,xpos,ypos,zpos,xmin,xmax,ymin,ymax,zmin,zmax,300);
 
  leda_swap(xmin,xmax);
  leda_swap(ymin,ymax);
  leda_swap(zmin,zmax);

  forall_nodes(v,G)
  { double x = xpos[v];
    double y = ypos[v];
    double z = zpos[v];
    if (x < xmin) xmin = x; else if (x > xmax) xmax = x;
    if (y < ymin) ymin = y; else if (y > ymax) ymax = y;
    if (z < zmin) zmin = z; else if (z > zmax) zmax = z;
  }


  double wx0 = gw.get_xmin();
  double wx1 = gw.get_xmax();
  double wy0 = gw.get_ymin();
  double wy1 = gw.get_ymax();

  double space = 0.15 * (wx1-wx0);
  wx0 += space;
  wx1 -= space;
  wy0 += space;
  wy0 -= space;

  double d = (wy1-wy0) - (wx1-wx0)*(ymax-ymin)/(xmax-xmin);
  wy0 += d/2;
  wy1 -= d/2;

  double dx,dy,fx,fy;
  gw.fill_win_params(wx0,wy0,wx1,wy1,xmin,ymin,xmax,ymax,dx,dy,fx,fy);


  node_array<vector> pos(G);

  forall_nodes(v,G)
  { xpos[v] = dx + fx*xpos[v];
    ypos[v] = dy + fy*ypos[v];
    zpos[v] =  0 + fx*zpos[v];
    pos[v] = vector(xpos[v],ypos[v],zpos[v]);
   }

  W.disable_panel();

  d3_window d3_win(W,G,pos);

  d3_win.set_draw_graph(true);
  d3_win.set_node_width(12);

  d3_win.set_message("");

  string msg("\\bf\\blue ~~~left:  \\black zoom in \
              \\bf\\blue ~~~~middle:\\black zoom out \
              \\bf\\blue ~~~~right: \\black return");

  W.set_show_coord_handler(nil);
  W.set_status_string(msg);

  node u;
  forall_nodes(u,G) d3_win.set_color(u,gw.get_color(u));
  edge e;
  forall_edges(e,G) d3_win.set_color(e,gw.get_color(e));

  d3_win.draw();
 
  if (window::display_type() == "xx")
    d3_win.set_speed(300);
  else
    d3_win.set_speed(5);

  int but = 0;
 
  while (but != MOUSE_BUTTON(3)) but = d3_win.move();

  node_array<point> d2pos(G);
  d3_win.get_d2_position(d2pos);
 
  bool fl = gw.set_flush(false);
  gw.restore_node_attributes();
  gw.restore_edge_attributes();
  gw.set_layout(d2pos);
  gw.redraw();
  gw.set_flush(fl);

  W.enable_panel();
  W.set_show_coord_handler(GraphWin::coord_handler);
  gw.update_status_line();

}

void gw_visrep_embed(GraphWin& gw)
{
  graph&  G = gw.get_graph();

  if (!PLANAR(G))
  { gw_test_planarity(gw);
    return;
   }

  node_array<double> xpos(G);
  node_array<double> ypos(G);
  node_array<double> xrad(G);
  node_array<double> yrad(G);
  edge_array<list<double> > xbends(G);
  edge_array<list<double> > ybends(G);
  edge_array<double> xsan(G);
  edge_array<double> ysan(G);
  edge_array<double> xtan(G);
  edge_array<double> ytan(G);

  if (VISIBILITY_REPRESENTATION(G,xpos,ypos,xrad,yrad,xsan,ysan,xtan,ytan))
  { gw.set_node_shape(rectangle_node);
    double dx,dy,fx,fy;
    gw.fill_win_params(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);
    gw.transform_layout(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);
    gw.set_layout(xpos,ypos,xrad,yrad,xbends,ybends,xsan,ysan,xtan,ytan);
   }

  if (!gw.get_flush()) gw.redraw();
}



void gw_sp_embed(GraphWin& gw)
{
  graph&  G = gw.get_graph();

  if (!Is_Series_Parallel(G))
  { LEDA_EXCEPTION(0,"gw_sp_embed: graph must be series-parallel.");
    return;
   }

  node_array<double> xpos(G);
  node_array<double> ypos(G);
  node_array<double> xrad(G);
  node_array<double> yrad(G);
  edge_array<list<double> > xbends(G);
  edge_array<list<double> > ybends(G);
  edge_array<double> xsan(G);
  edge_array<double> ysan(G);
  edge_array<double> xtan(G);
  edge_array<double> ytan(G);

  if (SP_EMBEDDING(G,xpos,ypos,xrad,yrad,xbends,ybends,xsan,ysan,xtan,ytan))
  { gw.set_node_shape(rectangle_node);
    double dx,dy,fx,fy;
    gw.fill_win_params(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);
    gw.transform_layout(xpos,ypos,xrad,yrad,xbends,ybends,dx,dy,fx,fy);
    gw.set_layout(xpos,ypos,xrad,yrad,xbends,ybends,xsan,ysan,xtan,ytan);
   }

  if (!gw.get_flush()) gw.redraw();
}



void gw_d3_sp_embed(GraphWin& gw)
{
  graph&  G = gw.get_graph();
  window& W = gw.get_window();

  if (!Is_Series_Parallel(G))
  { LEDA_EXCEPTION(0,"gw_d3_sp_embed: graph must be series-parallel.");
    return;
   }

  node_array<vector>        pos(G,vector(3));
  edge_array<list<vector> > bends(G);

  D3_SP_EMBEDDING(G,0.2,pos,bends,100,100,0,0);

  node_array<double> xpos(G);
  node_array<double> ypos(G);
  edge_array<list<double> > xbends(G);
  edge_array<list<double> > ybends(G);

  node v;
  forall_nodes(v,G)
  { xpos[v] = pos[v][0];
    ypos[v] = pos[v][1];
   }

  edge e;
  forall_edges(e,G)
  { list<vector>& L = bends[e];
    vector vec;
    forall(vec,L)
    { xbends[e].append(vec[0]);
      ybends[e].append(vec[1]);
     }
   }

  double dx,dy,fx,fy;
  gw.fill_win_params(xpos,ypos,xbends,ybends,dx,dy,fx,fy);
  gw.transform_layout(xpos,ypos,xbends,ybends,dx,dy,fx,fy);
  gw.set_layout(xpos,ypos,xbends,ybends);

  forall_nodes(v,G)
  { double x = dx + fx*pos[v][0];
    double y = dy + fy*pos[v][1];
    double z =  0 + fx*pos[v][2];
    pos[v] = vector(x,y,z);
   }

  forall_edges(e,G)
  { list<vector>& L = bends[e];
    list_item it;
    forall_items(it,L)
    { double x = dx + fx*L[it][0];
      double y = dy + fy*L[it][1];
      double z =  0 + fx*L[it][2];
      L[it] = vector(x,y,z);
    }
   }

  W.disable_panel();

  d3_window d3_win(W,G,pos);

  if (gw.get_edge_shape() == poly_edge)
     d3_win.set_show_bends(d3_window::poly_bends);
  else 
     if (gw.get_edge_shape() == bezier_edge)
        d3_win.set_show_bends(d3_window::bezier_bends);
     else
        d3_win.set_show_bends(d3_window::no_bends);

  d3_win.set_draw_graph(true);
  d3_win.set_node_width(8);

  //d3_win.set_y_rotation(false);
  //d3_win.set_node_labels(true);

  d3_win.set_message("");

  string msg("\\bf\\blue ~~~left:  \\black zoom in \
              \\bf\\blue ~~~~middle:\\black zoom out \
              \\bf\\blue ~~~~right: \\black return");

  //d3_win.set_message(msg);
  d3_win.set_message("");

  W.set_show_coord_handler(nil);
  W.set_status_string(msg);

  forall_edges(e,G) d3_win.set_bends(e,bends[e]);

  d3_win.draw();

  node u;
  forall_nodes(u,G) d3_win.set_color(u,gw.get_color(u));

  forall_edges(e,G) d3_win.set_color(e,gw.get_color(e));

 
  if (window::display_type() == "xx")
   d3_win.set_speed(300);
  else
   d3_win.set_speed(5);
  
  d3_win.draw();
 
  int but = 0;
 
  while (but != MOUSE_BUTTON(3)) but = d3_win.move();

  W.enable_panel();

  W.set_show_coord_handler(GraphWin::coord_handler);
  gw.update_status_line();
 
  node_array<point> d2pos(G);
  d3_win.get_d2_position(d2pos);

  edge_array<list<point> > d2bends(G);
  d3_win.get_d2_bends(d2bends);
 
  bool fl = gw.set_flush(false);
  gw.set_layout(d2pos,d2bends);
  gw.redraw();
  gw.set_flush(fl);
}



// hyperbolic view


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
  double z1=::sqrt(1-a1*a1-a2*a2);
  double z2=::sqrt(1-b1*b1-b2*b2);
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
static bool button_down=false; 

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
    if (button_down)
    { wp->draw_vline(hp_x0,grey2);
      wp->draw_hline(hp_y0,grey2);
     }
   }
}

static void hyperbolic_view(GraphWin& gw, double x, double y,
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

  double x0 = 0;
  double y0 = 0;

  W.disable_panel();

  gw.message("Hyperbolic View  (move: click & drag left   exit: right)");
        
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

LEDA_END_NAMESPACE
