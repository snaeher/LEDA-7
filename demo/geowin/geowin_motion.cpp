/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_motion.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/graph/shortest_path.h>
#include <LEDA/geo/rat_geo_alg.h>

using namespace leda;


#define POLYGON     rat_polygon
#define GEN_POLYGON rat_gen_polygon
#define SEGMENT     rat_segment
#define TRIANGLE    rat_triangle
#define POINT       rat_point
#define VECTOR      rat_vector
#define NB          rational

/*
#define POLYGON     polygon
#define GEN_POLYGON gen_polygon
#define SEGMENT     segment
#define TRIANGLE    triangle
#define POINT       point
#define VECTOR      vector
#define NB          double
*/


// add handler ...
GeoEditScene<list<POLYGON> >* input;

// for BCC:
GeoBaseScene<list<POLYGON> >* input2; 

void forbidden_space(const array<POLYGON>& AP, int low, int high, const POLYGON& R, GEN_POLYGON& result)
{
  int sz = high-low;
  if (sz==0) result = MINKOWSKI_DIFF(AP[low], R);
  else {
    GEN_POLYGON gp1,gp2;
    int spl = low + sz/2;
   
    forbidden_space(AP, low, spl, R, gp1);
    forbidden_space(AP, spl+1, high, R, gp2);
    result = gp1.unite(gp2);
  }
}

void forbidden_space(const list<POLYGON>& LP, const POLYGON& R, GEN_POLYGON& result)
{
   array<POLYGON> AP(LP.size());
   int arr_index=0;
   POLYGON piter;
   forall(piter,LP){
     AP[arr_index++] = piter; 
   }
   forbidden_space(AP, 0, LP.size() -1, R, result);
}


void free_space(const list<POLYGON>& LP, const POLYGON& R, GEN_POLYGON& result)
{
   forbidden_space(LP, R, result);
   result = result.complement();
}

void free_space(const list<POLYGON>& LP, const POLYGON& R, const GEN_POLYGON& border, GEN_POLYGON& result)
{
   forbidden_space(LP, R, result);
   result = result.complement();
   result = result.intersection(border);   
}

window* win;
bool anim_val=false;

bool upd = false;
POLYGON border;
POLYGON rpoly;
GEN_POLYGON all_space;

static void free_space( const list<POLYGON>& Lin, list<GEN_POLYGON>& Lout)
{
  Lout.clear();
  if( Lin.length() < 2 ) { upd = false; return; }
  
  upd = true;
  POLYGON p1=Lin[Lin[0]];
  rpoly = p1;
  list<POLYGON> Lh = Lin; 
  Lh.pop();
  GEN_POLYGON result;
  free_space(Lh, p1, border, result);
  all_space = result;
  
  Lout.append(result);
}


void animation(window* w, const POLYGON& rpoly, const list<SEGMENT>& LSeg, const POINT& src, const POINT& trg)
{
 (*win) << src << trg;

 list<POINT> pos;
 list<POINT> act_pts;
 POINT piter;
 
 SEGMENT iter;
 
 forall(iter,LSeg){
  pos.append(midpoint(iter.source(), iter.target()));
  pos.append(iter.source());
  win->draw_segment(iter.to_float(), green);
 }
 
 char* prect = win->get_window_pixrect();
 
 color fill_before = w->set_fill_color(yellow);
  
 forall(piter,pos){
  (*win) << rpoly.translate(piter.xcoord(),piter.ycoord());
  leda_wait(0.2);
  win->put_pixrect(prect);
 }
 
 w->set_fill_color(fill_before);
 
 delete prect;
}

void path_fcn(const list<POINT>& Pl, list<SEGMENT>& Lseg)
{
 Lseg.clear();

 if (upd) {
 if (Pl.size() != 2) return;
 POINT source = Pl.head(), target = Pl.tail();
 
 list<edge> inner, outer;
 list<edge> hull, hole;
 GRAPH<POINT,SEGMENT>  G;
 TRIANGULATE_POLYGON(all_space, G, inner, outer, hull, hole); 
 
 edge e, succ;
 edge src = NULL, trg = NULL;
 
 forall(e, outer) G.del_edge(e);
 
 forall(e,inner){
   succ = G.face_cycle_succ(e);
   
   POINT p1 = G[G.source(e)];
   POINT p2 = G[G.target(e)];
   POINT p3 = G[G.target(succ)];
   TRIANGLE tr(p1,p2,p3);

   if (tr.contains(source)) src = e; 
   if (tr.contains(target)) trg = e;  
 } 
  
 if ((src == NULL) || (trg == NULL))  return;
 
 // now we found the triangles, where src and trg are in
 // now find the CLOSEST vertices of the triangles ... 
 edge src2 = G.face_cycle_succ(src);
 edge src3 = G.face_cycle_succ(src2);
 edge trg2 = G.face_cycle_succ(trg);
 edge trg3 = G.face_cycle_succ(trg2);
 
 edge best_src = src, best_trg = trg;
 
 // compute closest triangle vertex ...
 if (source.cmp_dist(G[G.source(best_src)],G[G.source(src2)]) == 1) best_src = src2;
 if (source.cmp_dist(G[G.source(best_src)],G[G.source(src3)]) == 1) best_src = src3;
 
 if (target.cmp_dist(G[G.source(best_trg)],G[G.source(trg2)]) == 1) best_trg = trg2;
 if (target.cmp_dist(G[G.source(best_trg)],G[G.source(trg3)]) == 1) best_trg = trg3;  

 node ns1 = G.source(best_src);
 node nt1 = G.source(best_trg);
 
 // compute shortest path ...
 edge_array<double> c(G);
 forall_edges(e,G) c[e] = G[G.source(e)].to_float().distance(G[G.target(e)].to_float());
 node_array<edge> pred(G,NULL);
 
 DIJKSTRA(G, ns1, nt1, c, pred);

 edge pe;
 node nact = nt1;
 
 if (pred[nt1] == NULL) {
  if (nt1==ns1) Lseg.append(SEGMENT(source,target));
  return;
 }
 
 else {
   do {
     pe = pred[nact];
     Lseg.append(SEGMENT(G[G.source(pe)], G[G.target(pe)]));
     nact = G.source(pe);
   }
   while (! (G.source(pe) == ns1));
 }
 Lseg.push(SEGMENT(G[nt1],target));
 Lseg.append(SEGMENT(source,G[ns1]));  
 
 if (anim_val) animation(win, rpoly, Lseg, source, target);
 }
}


void polygon_add_handler(GeoWin& gw, const POLYGON& pol)
{
  //cout << "polygon_add_handler !\n"; 
  // set the fill color ...
  const list<POLYGON>& LP = gw.get_objects_reference(input2);
  
  if (LP.size()==1) {
    gw.set_obj_fill_color(input2, (void*)(&pol), yellow);
    gw.set_obj_label(input2, (void*)(&pol), string("Robot"));
  }  
}

int main()
{
  GeoWin GW("Motion planning demo");
  
  //GW.set_show_grid(true);
  //GW.set_grid_dist(9);

  GW.set_bg_color(grey1);

  GW.set_update_while_changing(false);
  
  win = &(GW.get_window());
  
  POINT p1(-1000,-1000,1), p2(1000,-1000,1), p3(1000,1000,1), p4(-1000,1000,1);
  list<POINT> poly_points;
  poly_points.append(p1); poly_points.append(p2); poly_points.append(p3); poly_points.append(p4);
  border = POLYGON(poly_points);
  
  list<rat_polygon> L;
  list<POINT> Lpoints;

  input = GW.new_scene(L); 
  input2 = (GeoBaseScene<list<POLYGON> >*) input;
  
  GW.set_fill_color(input, blue2);
  GW.set_color(input, blue2);
  GW.set_line_width(input, 2);
  GW.set_short_name(input, "poly");
  GW.set_description(input,"This scene holds one robot\nand some obstacles.\n");
  GW.set_post_add_handler(input, polygon_add_handler);
  GW.set_z_order(input, 0);
  
  GeoEditScene<list<POINT> > *sc_points = GW.new_scene(Lpoints);
  GW.set_limit(sc_points, 2);
  GW.set_point_style(sc_points, disc_point);
  GW.set_short_name(sc_points, "path pt");
  GW.set_description(sc_points,"Start- and end point\nof the robot motion.\n"); 
  GW.set_z_order(sc_points, 1);  
  
  geowin_update<list<POLYGON>, list<GEN_POLYGON> > up3(free_space);  
  geowin_update<list<POINT>,   list<SEGMENT> >     up4(path_fcn);   
  
  geo_scene all_space =  GW.new_scene(up3, input, "free space");
  GW.set_color( all_space, black);
  GW.set_fill_color( all_space, ivory);
  //GW.set_line_width( all_space, 2);  
  GW.set_short_name( all_space, "all.space");
  GW.set_description( all_space, "Borders of the free space.\nThat is the space where the reference\npoint of the robot can be placed.\n");
  GW.set_z_order(all_space, 3);  

  
  geo_scene sc_path = GW.new_scene(up4, sc_points, "PATH from source to target"); 
  GW.set_color(sc_path, black);
  GW.set_short_name(sc_path, "PATH");
  GW.set_description(sc_path, "Motion path of the reference point of the robot.\n"); 
  GW.set_show_orientation(sc_path,true);
  GW.set_line_width(sc_path,2);
  GW.set_z_order(sc_path, 2);  
  
  
  GW.set_all_visible(true);
  
  GW.add_dependence(input,sc_path);

  GW.add_help_text("geowin_motion");

  GW.hide_buttons();
  
  GW.init_menu();
  GW.get_window().bool_item(" Animation ?",anim_val);

  GW.display();
  GW.get_window().close_status_window();
  GW.edit(input);

  return 0;
}
