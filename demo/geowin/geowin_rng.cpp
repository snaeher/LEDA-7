/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_rng.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/graphics/geowin.h>
#include<LEDA/geowin/geowin_init.h>
#include<LEDA/geo/rat_geo_alg.h>
#include<LEDA/geo/float_geo_alg.h>
#include<LEDA/geo/rat_point_set.h>
#include<LEDA/geo/point_set.h>

using namespace leda;


/*
typedef rat_point     POINT;
typedef rat_segment   SEGMENT;
typedef rat_circle    CIRCLE;
typedef rat_point_set POINTSET;
*/

typedef point     POINT;
typedef segment   SEGMENT;
typedef circle    CIRCLE;
typedef point_set POINTSET;

window* out_win;
bool bval;

void diagramm(const POINT& p1,const POINT& p2)
{  out_win->set_color(grey1);(*out_win) << SEGMENT(p1,p2);}

void triang(const POINT& p1,const POINT& p2)
{  out_win->set_color(grey2);(*out_win) << SEGMENT(p1,p2);}

void hull(const POINT& p1,const POINT& p2)
{  out_win->set_color(black);(*out_win) << SEGMENT(p1,p2);}

// function for computation of the RNG ...
void compute_rng(const POINTSET& PS,GRAPH<POINT,int>& RNG)
{
  edge e;
  node v;
  node v1,v2;
  POINT p1,p2;
  
  map<node,node> link;
  edge_map<bool> EM(PS,false);
  
  // insert the new nodes in RNG ...
  forall_nodes(v,PS){
    node nv = RNG.new_node(PS[v]);
    link[v] = nv;
  }
  
  forall_edges(e,PS){
    // is e an edge of the RNG ??
    edge rev = PS.reversal(e);
    if (EM[rev] == false) { // reversal was not yet touched ...
       EM[e] = true; // mark e ...
       v1 = PS.source(e); v2 = PS.target(e);
       p1 = PS[v1]; p2 = PS[v2];
       // 2 circles for testing ...
       CIRCLE c2(p2,p1);
       
       // range search in a disk...
       list<node> LN = PS.range_search(v1,p2);
       
       bool flag=true;
       forall(v,LN) { // one of these nodes in c2 ?
         if (v!=v2 && c2.inside(PS[v])) { flag = false; break; }
       }
       if (flag){ // we found a new edge of the RNG ...
         RNG.new_edge(link[v1], link[v2]);
       }
    }
  }
  //cout << "#nodes RNG:" << RNG.number_of_nodes() << "\n";
  //cout << "#edges RNG:" << RNG.number_of_edges() << "\n";
}


class geo_delau : public geowin_redraw, public geowin_update<list<POINT>, list<POINT> >
{
public:
  POINTSET S;
  GRAPH<POINT,int> RNG;
  bool flag;
  
  geo_delau(bool b) { flag = b;} 
  
  virtual ~geo_delau() {}
  
  void draw_rng(window& W)
  {
    color cold = W.set_color(green);
    int lwold = W.set_line_width(2);
    edge e;
    forall_edges(e,RNG) {
      segment seg(RNG[RNG.source(e)].to_float(), RNG[RNG.target(e)].to_float());
      W << seg;
      if (bval) { // output lunes (forbidden regions) ...
        W.set_color(black); W.set_line_width(1);
        point p1 = seg.source(), p2 = seg.target();
	vector v = p2-p1, vh = v/2;
	vector v2 = v.rotate90() * 0.866;
	vector v3 = -v2;
	point c1 = p1 + vh + v2;
	point c2 = p1 + vh + v3;
	W.draw_arc(c2,p1,c1); W.draw_arc(c2,p2,c1);
	W.set_color(green); W.set_line_width(2);
      }
    }
    
    W.set_line_width(lwold);
    W.set_color(cold);
  }

  void draw(window& W,color c1,color c2,double x1,double y1,double x2,double y2)
  {
    out_win= &W;
    
    if (! flag) S.draw_edges(&diagramm, &triang, &hull);
    else draw_rng(W);
  }

  void update(const list<POINT>& L, list<POINT>&)
  { S= POINTSET(L); 
    RNG.clear();
    if (flag) compute_rng(S,RNG);
  }
  
};

geo_delau Delau(false);
geo_delau RelNBG(true);

int main()
{
  GeoWin gw("Relative neighborhood graph (RNG) demo");
  bval = true;
  
  list<POINT> L;
  
  GeoEditScene<list<POINT> >* sc_input =  gw.new_scene(L); 
  gw.set_color(sc_input,black);
  gw.set_point_style(sc_input,disc_point);
  gw.set_visible(sc_input, true);

  geo_scene sc1 = gw.new_scene(Delau, Delau, sc_input,"Delaunay triangulation"); 
  gw.set_color(sc1,blue);
  
  geo_scene sc2 = gw.new_scene(RelNBG, RelNBG, sc_input,"RNG"); 
  gw.set_color(sc2,blue);
  gw.set_line_width(sc2, 2);  
  gw.set_visible(sc2,true);
  
  gw.init_menu();
  gw.get_window().bool_item(" Show forbidden regions for RNG:",bval);

  gw.add_special_help_text("geowin_rng", true);
  gw.edit(sc_input);
  
  return 0;
}

