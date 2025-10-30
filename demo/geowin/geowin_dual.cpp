/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_dual.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/geo_alg.h>


using namespace leda;


void dual_points(const list<line>& L, list<point>& D)
{ 
#if !defined(__KCC)
  GeoWin* gw_ptr = GeoWin::get_call_geowin();

  GeoEditScene<list<line> >* sc_input = 
    (GeoEditScene<list<line> >*) GeoWin::get_call_input_scene();

  GeoResultScene<list<line>,list<point> >* sc_dual = 
    (GeoResultScene<list<line>,list<point> >*)GeoWin::get_call_scene();
#endif

  D.clear();
  list_item it;
  forall_items(it,L){
    const line& x =  L[it];
    if (!x.is_vertical()) {
       point p = x.dual();
       list_item it2 = D.append(point(2*p.xcoord(),0.5*p.ycoord()));
       
#if !defined(__KCC)       
       color c = gw_ptr->get_obj_color(sc_input,(void*)&x);
       gw_ptr->set_obj_fill_color(sc_dual,D[it2],c);
#endif
       
    }
  }  

}

polygon upper_hull(const list<point>& L)
{ list<point> U = UPPER_CONVEX_HULL(L);
  if (!U.empty())
  { point a = U.head();
    point b = U.tail();
    U.push(a.translate(0,-100));
    U.append(b.translate(0,-100));
   }
  return polygon(U);
}

polygon intersect_halfplanes(const list<line>& L)
{ list<point> pol;
  if (!L.empty())
  { pol.append(point(-100,-100));
    pol.append(point(+100,-100));
    pol.append(point(+100,+100));
    pol.append(point(-100,+100));
   }
  polygon P(pol);
  line x;
  forall(x,L) 
  { if (compare(x.point1(),x.point2()) > 0) x = x.reverse();
    P = P.intersect_halfplane(x);
  }
  return P;
}


geowin_update<list<line>, list<point> >    DualPoints(dual_points); 
geowin_update<list<point>,list<polygon> >  UpperHull(upper_hull);
geowin_update<list<line>, list<polygon> >  IntersectHP(intersect_halfplanes);


int main()
{
  GeoWin gw("Duality Demo");
  gw.init(-10,10,-10); 

  // grid lines

  list_item itx=0,ity=0;
  list<line> coord_lines;
  for (int i = -50; i<=50; i++)
    { coord_lines.append(line(point(i,i),point(i,i+1)));
      if (i == 0) itx = coord_lines.last();
      coord_lines.append(line(point(i,i),point(i+1,i)));
      if (i == 0) ity = coord_lines.last();
     }
  GeoEditScene<list<line> >* sc_grid = gw.new_scene(coord_lines); 
  gw.set_name(sc_grid,"Grid Lines");
  gw.set_color(sc_grid,grey1);
  gw.set_visible(sc_grid,true);
  gw.set_obj_color(sc_grid,coord_lines[itx],grey3);
  gw.set_obj_color(sc_grid,coord_lines[ity],grey3);
  gw.set_z_order(sc_grid,4);
  gw.set_handle_defining_points(sc_grid,geowin_hide);
  
  list<line> L;
  geo_scene sc_input = gw.new_scene(L); 
  gw.set_name(sc_input, "Input Lines");
  gw.set_color(sc_input,blue);
  gw.set_cyclic_colors(sc_input,true);
  gw.set_cyclic_colors_counter(sc_input,yellow);
  gw.set_z_order(sc_input,0);
  gw.set_description(sc_input, "This scene holds the input lines. We want to compute the \
  intersection of the halfspaces lying above these lines.");

  geo_scene sc_inter = gw.new_scene(IntersectHP, sc_input, "HP-Intersect");
  gw.set_color(sc_inter,invisible); 
  gw.set_fill_color(sc_inter,grey2); 
  gw.set_visible(sc_inter,true);
  gw.set_z_order(sc_inter,3);
  gw.set_description(sc_inter, "Intersection of the upper halfplanes.\n");

  geo_scene sc_dual = gw.new_scene(DualPoints, sc_input, "Dual Points");
  gw.set_color(sc_dual,black); 
  gw.set_fill_color(sc_dual,red); 
  gw.set_point_style(sc_dual,rect_point); 
  gw.set_visible(sc_dual,true);
  gw.set_z_order(sc_dual,1);
  gw.set_description(sc_dual, "The scene holds the dual points of the input lines.");  

  geo_scene sc_hull = gw.new_scene(UpperHull, sc_dual, "Upper Hull");
  gw.set_fill_color(sc_hull,grey1); 
  gw.set_color(sc_hull,grey2); 
  gw.set_selection_color(sc_hull,grey2);
  gw.set_line_width(sc_hull,1); 
  gw.set_visible(sc_hull,false);
  gw.set_z_order(sc_hull,2);
  gw.set_description(sc_hull, "The scene holds the upper convex hull of the dual points.\n");    

  gw.add_special_help_text("geowin_dual", true);

  gw.edit(sc_input);
  
  return 0;  
}
