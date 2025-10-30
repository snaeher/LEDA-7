/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_paths.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/float_geo_alg.h>
// motions ...
#include <LEDA/graphics/motion.h>

using namespace leda;

using std::cout;


class my_animation : public geowin_animation { 
public:
 bool flag;
 list<point> path;
 list_item act;
 
 my_animation(motion_base& motion, const point& start, const point& end, int steps) 
 {  
  flag = true;  
  motion.get_path(start, end, steps, path);
  if (! path.empty()) act = path.first();
  else act = NULL;
 }
 
 void set_path(motion_base& motion, const point& start, const point& end, int steps) 
 {
  path.clear();
  flag = true;  
  motion.get_path(start, end, steps, path);
  if (! path.empty()) act = path.first();
  else act = NULL;  
 }
 
 virtual ~my_animation() { }

 bool is_running(const GeoWin& gw) { return flag; }
 
 point get_next_point(const GeoWin& gw) {
  point pact = path[act];
  
  act = path.succ(act);
  if (act == NULL) flag = false; // path finished ...

  return pact;
 }
 
 long  get_next_action(const GeoWin& gw) { return GEOWIN_MOVE_SELECTED; }
};

typedef list<point>                        point_list;
typedef list<polygon>                      polygon_list;
geowin_update<point_list, polygon_list>    hull(CONVEX_HULL_POLY);

int main(){
 GeoWin gw("Motion demo");
 
 list<point> LP;
 LP.append(point(10,10)); LP.append(point(210,60)); LP.append(point(90,10)); 
 LP.append(point(100,220)); LP.append(point(99,50)); LP.append(point(170,140)); 
 
 GeoEditScene<list<point> >* sc1 = gw.new_scene(LP);
 gw.set_point_style(sc1, disc_point);
 
 linear_motion lin_motion;
 my_animation anim(lin_motion, point(10,10), point(200,200), 100); 

 // select some points ...
 list<list<point>::iterator> LIT;
 list<point>::iterator it = LP.begin();
 LIT.append(it); it++;
 LIT.append(it);
 gw.set_selected_objects(sc1,LIT);
 
 geo_scene sc_hull = gw.new_scene(hull, sc1, "CONVEX HULL");
 gw.set_color(sc_hull,grey1); 
 gw.set_fill_color(sc_hull,grey1);
 gw.set_visible(sc_hull,true);
 gw.set_z_order(sc_hull,1); 
 
 gw.message("Linear motion");
 gw.animate(sc1, anim);
  
 gw.message("Press Done to continue"); 
 gw.edit(sc1);
 
 gw.message("Circular motion");
 
 circular_motion circ_motion(- 1.5);
 anim.set_path(circ_motion, point(200,200), point(10,10), 450);
 
 gw.animate(sc1, anim); 
 
 gw.message("Press Done to continue");   
 gw.edit(sc1);
 
 bezier_motion p_motion;
 p_motion.append(rel_coord(0.0), abs_coord(40.0)); 
 p_motion.append(rel_coord(0.5), abs_coord(100.0));  
 p_motion.append(rel_coord(1.0), abs_coord(40.0));
 
 anim.set_path(p_motion, point(10,10), point(200,200), 400); 
 
 gw.message("Bezier motion");
 gw.animate(sc1, anim);  
 gw.message("Press Done to end program");   
 
 gw.edit(sc1); 

 return 0;
}
