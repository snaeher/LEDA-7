/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_pointset.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include<LEDA/graphics/geowin.h>
#include<LEDA/geo/rat_geo_alg.h>
#include<LEDA/geo/rat_point_set.h>
#include<LEDA/graphics/bitmaps/button21.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


typedef rat_point   POINT;
typedef rat_segment SEGMENT;
typedef rat_circle  CIRCLE;

window* out_win;

void diagramm(const POINT& p1,const POINT& p2)
{  out_win->set_color(grey1);(*out_win) << SEGMENT(p1,p2);}

void triang(const POINT& p1,const POINT& p2)
{  out_win->set_color(grey2);(*out_win) << SEGMENT(p1,p2);}

void hull(const POINT& p1,const POINT& p2)
{  out_win->set_color(blue);(*out_win) << SEGMENT(p1,p2);}

class geo_delau : public geowin_redraw, public geowin_update<list<POINT>, list<POINT> >
{
public:
  rat_point_set S;
  
  virtual ~geo_delau() {}

  void draw(window& W,color c1,color c2,double x1,double y1,double x2,double y2)
  {
    out_win= &W;
    S.draw_edges(&diagramm, &triang, &hull);
  }

  void update(const list<POINT>& L, list<POINT>&)
  { S= rat_point_set(L); }
  
  bool insert(const POINT& new_obj)
  { cout << "insert!\n"; 
    S.insert(new_obj); return true; }
  
  bool del(const POINT& del_obj)
  { cout << "del\n"; 
    S.del(del_obj); return true; }

  bool change(const POINT& old_obj, const POINT& new_obj)
  {
    S.del(old_obj); S.insert(new_obj);
    return true;
  }
};

geo_delau Delau;

// circular range search ...

class geo_rangesearch : public geowin_redraw, public geowin_update<list<CIRCLE>, list<POINT> >
{
public:
  list<POINT> found;
  
  virtual ~geo_rangesearch() {}

  virtual void draw(window& W,color c1,color c2,double x1,double y1,double x2,double y2)
  {
    POINT p;
    forall(p,found){ W.draw_disc(p.to_point(),3,c1); }
  }
  
  virtual void update(const list<CIRCLE>& L, list<POINT>&)
  {
    found.clear();
    CIRCLE c;
    forall(c,L){
      list<node> nl = Delau.S.range_search(c);
      node nd;
      forall(nd,nl){
        found.append(Delau.S.pos(nd));
      }
    }
  }
};

// k nearest neighbors ...

class geo_knn : public geowin_redraw, public geowin_update<list<POINT>, list<POINT> >
{
public:
  list<POINT>  found;
  
  virtual ~geo_knn() {}

  virtual void draw(window& W,color c1,color c2,double x1,double y1,double x2,double y2)
  {
    POINT p;
    forall(p,found){ W.draw_disc(p.to_point(),3,c1); }
  }

  virtual void update(const list<POINT>& L, list<POINT>&)
  {
    found.clear();
    POINT p;
    forall(p,L){
      list<node> nl = Delau.S.nearest_neighbors(p,5);
      node nd;
      forall(nd,nl){
        found.append(Delau.S.pos(nd));
      }
    }
  }   
};

class geo_locate :  public geowin_redraw, public geowin_update<list<POINT>, list<POINT> >
{
public:
  list<SEGMENT>  nearest;
  list<POINT>    p1_list;
  list<POINT>    p2_list;
  list<POINT>    p3_list;

  virtual ~geo_locate() {}

  virtual void draw(window& W,color c1,color c2,double x1,double y1,double x2,double y2)
  {
   POINT p1,p2,p3;
   list_item l2=p2_list.first(), l3= p3_list.first();
   forall(p1,p1_list){
        p2 = p2_list[l2]; p3=p3_list[l3];
        W.draw_filled_triangle(p1.to_point(),p2.to_point(),p3.to_point(),green);
        W.draw_triangle(p1.to_point(),p2.to_point(),p3.to_point(),black);
        l2=p2_list.succ(l2); l3=p3_list.succ(l3);
   }

   SEGMENT s;
   forall(s, nearest) {
     W.draw_disc(s.target().to_point(),3,red);
     W.draw_edge_arrow(s.to_segment(),c1);
   }
  }

  virtual void update(const list<POINT>& L, list<POINT>&)
  { 
    nearest.clear();
    edge e,e2;
    p1_list.clear(); p2_list.clear(); p3_list.clear();
    POINT p;
    SEGMENT s;
    forall(p,L){
      node nd = Delau.S.nearest_neighbor(p);
      if (nd != NULL){
       nearest.append(SEGMENT(p,Delau.S.pos(nd)));
      }
      e=Delau.S.locate(p);
      if (e!=NULL && Delau.S.is_diagram_edge(e)  && !Delau.S.is_hull_edge(e)){
       s=Delau.S.seg(e);
       p1_list.append(s.source()); p2_list.append(s.target());
       e2 = Delau.S.face_cycle_succ(e);
       if (e2 !=NULL) p3_list.append(Delau.S.seg(e2).target());
      
      }
    }
  } 
};

int main()
{
  GeoWin gw("Advanced Delaunay Triangulation Data Type");
  
  gw.set_button_width(21);
  gw.set_button_height(21);
  
  list<POINT> L;
  list<POINT> LOC_PT;
  list<CIRCLE> LCIRC;
  list<POINT> LNN;
  
  GeoEditScene<list<POINT> >* sc_input =  gw.new_scene(L, "Input point set"); 
  gw.set_color(sc_input,black);
  gw.set_activate_bitmap(sc_input, point_21_bits);
  gw.set_description(sc_input, "Input points for the Delaunay triangulation.\n");

  geo_scene sc_loc   =  gw.new_scene(LOC_PT, "Input points for locate and nearest neighbor search"); 
  gw.set_color(sc_loc,black);
  gw.set_activate_bitmap(sc_loc, point_21_bits);
  gw.set_description(sc_loc, "Points for locate and nearest neighbor operations.\n");
  
  geo_scene sc_circles = gw.new_scene(LCIRC, "Input circles for circular range searches");
  gw.set_color(sc_circles,blue2);
  gw.set_fill_color(sc_circles,invisible);
  gw.set_activate_bitmap(sc_circles, circle_21_bits);
  gw.set_description(sc_circles, "Circles for range searches on the input points.\n");
  
  geo_scene sc_nn  = gw.new_scene(LNN, "Input points for k (5) nearest neighbor search");
  gw.set_color(sc_nn, brown);
  gw.set_activate_bitmap(sc_nn, point_21_bits);
  gw.set_description(sc_nn, "Points for k nearest neighbors operation.\n");  

  geo_scene sc1 = gw.new_scene( Delau, Delau, sc_input,"Delaunay triangulation"); 
  gw.set_color(sc1,blue);
  gw.set_description(sc_loc, "This scene holds the Delaunay triangulation of the input point set.\n");

  geo_locate LOC;
  geo_scene sc2 = gw.new_scene( LOC, LOC, sc_loc, "Nearest neighbor");
  gw.set_color(sc2,blue2);
  
  geo_rangesearch RS;
  geo_scene sc3 = gw.new_scene( RS, RS, sc_circles, "Circular range search");
  gw.set_color(sc3,blue); 
  
  geo_knn NN;
  geo_scene sc4 = gw.new_scene( NN, NN, sc_nn, "k nearest neighbors");
  gw.set_color(sc4,blue2);   
 
  gw.set_all_visible(true);
 
  gw.add_dependence(sc_input,sc2);
  gw.add_dependence(sc_input,sc3);
  gw.add_dependence(sc_input,sc4);
  
  gw.add_special_help_text("geowin_pointset", true);
  gw.display(window::center,window::center);
  gw.edit(sc_input);
  
  return 0;
}

