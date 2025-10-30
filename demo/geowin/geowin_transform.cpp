/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_transform.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>

using namespace leda;

#define FP_KERNEL
#define POLYGON    polygon
#define SEGMENT    segment

/*
#define RATIONAL_KERNEL
#define POLYGON    rat_polygon
#define SEGMENT    rat_segment
*/

int main() 
{
  GeoWin gw("Try the Affine transformations !");
  gw.add_special_help_text("geowin_transform",true);
  
  list<SEGMENT> LS;
  list<POLYGON> LP;

#if defined(RATIONAL_KERNEL)  
  geowin_gui_rat_transform<SEGMENT> trans_seg;
  geowin_gui_rat_transform<POLYGON> trans_poly;  
#else
  geowin_gui_transform<SEGMENT> trans_seg; 
  geowin_gui_transform<POLYGON> trans_poly;   
#endif   
   
  GeoEditScene<list<SEGMENT> >* sc_seg = gw.new_scene(LS);
  gw.set_transform(sc_seg, trans_seg);

  GeoEditScene<list<POLYGON> >* sc_poly = gw.new_scene(LP);
  gw.set_transform(sc_poly, trans_poly);

  gw.edit(sc_poly);
  
  return 0;
}

