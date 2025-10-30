/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_flip_delaunay.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/geo_global_enums.h>
#include <LEDA/geo/rat_geo_alg.h>

using namespace leda;


#define POINT    rat_point
#define SEGMENT  rat_segment
#define CIRCLE   rat_circle

bool bval;

double wait_time;

void output_graph(GeoWin* gw, const GRAPH<POINT,int>& G, edge e1, edge e2, edge e3, edge e4, edge fl1, edge fl2, 
                  color cl1, color cl2, string msg)
{ 
  window& w = gw->get_window();
  w.clear();
  gw->message(msg);
  
  edge e;
  forall_edges(e,G) {
    if (e==e1 || e==e2 || e==e3 || e== e4 || e==G.reversal(e1) || \
         e==G.reversal(e2) || e==G.reversal(e3) || e== G.reversal(e4))
    { 
      int lold = w.set_line_width(3);
      w.draw_segment(segment(G[G.source(e)].to_float(), G[G.target(e)].to_float()),cl1); 
      w.set_line_width(lold);
    }
    else {
      // we have to flip an edge ...
      if (e==fl1 || e==fl2) {
        int lold = w.set_line_width(3);
        w.draw_segment(segment(G[G.source(e)].to_float(), G[G.target(e)].to_float()),cl2); 
	
	// output of circumcircle ...
	edge esucc = G.face_cycle_succ(e);
	w.set_line_width(1);
	w.draw_circle(CIRCLE(G[G.source(e)], G[G.target(e)], G[G.target(esucc)]).to_float(), orange);
	
        w.set_line_width(lold);        
      }
      w << SEGMENT(G[G.source(e)], G[G.target(e)]);
    }
  }
  if (bval) {
    int but = w.read_mouse();
    if (but == MOUSE_BUTTON(3)) bval= !bval;
  }
  else {
    int but = w.get_mouse();
    switch(but){ 
     case MOUSE_BUTTON(2): {
       if (wait_time>0.1) wait_time = wait_time - 0.1;
       break;
     }
     case MOUSE_BUTTON(3): {
       wait_time = wait_time + 0.1;
       break;
     }
     default: { break; } 
    }
    leda_wait(wait_time);    
  }
  
  gw->message(string(""));  
}

int GW_DELAUNAY_FLIPPING(GRAPH<POINT,int>& G, 
                      delaunay_voronoi_kind k)
{
  GeoWin* gw = GeoWin::get_call_geowin();

  if (G.number_of_nodes() <= 3) return 0;
  int f = ( k == NEAREST ? +1 : -1);

  list<edge> S = G.all_edges();
  S.permute();
  int flip_count = 0;

  while ( !S.empty() )
  { edge e = S.pop();
    edge r = G.reversal(e);
    
    if (G[e] == HULL_DART || G[r] == HULL_DART) continue;

    G[e] = DIAGRAM_DART;
    G[r] = DIAGRAM_DART;

    // e1,e2,e3,e4: edges of quadriliteral with diagonal e
    edge e1 = G.face_cycle_succ(r);
    edge e3 = G.face_cycle_succ(e);

    // flip test
    POINT a = G[source(e1)];
    POINT b = G[target(e1)];
    POINT c = G[source(e3)];
    POINT d = G[target(e3)];
    

    if ( left_turn(d,a,b) && left_turn(b,c,d) )
    { // the quadrilateral is convex
      
      int soc = f * side_of_circle(a,b,c,d);

      if (soc == 0) // co-circular quadriliteral(a,b,c,d) 
      { G[e] = NON_DIAGRAM_DART;
        G[r] = NON_DIAGRAM_DART;
      }

      if (soc > 0) // flip
      { edge e2 = G.face_cycle_succ(e1);
        edge e4 = G.face_cycle_succ(e3);
	
        S.push(e1); 
        S.push(e2); 
        S.push(e3); 
        S.push(e4); 
		
	output_graph(gw,G,e1,e2,e3,e4,e,r, red, blue, string("before edge flip"));
        
        // flip diagonal
        G.move_edge(e,e2,source(e4));
        G.move_edge(r,e4,source(e2));
	
	output_graph(gw,G,e1,e2,e3,e4,e,r, red, blue, string("after edge flip"));
	
        flip_count++;
      }
      
      if (soc < 0) // don't flip
      {
        edge e2 = G.face_cycle_succ(e1);
        edge e4 = G.face_cycle_succ(e3);          
        output_graph(gw,G,e1,e2,e3,e4,e,r, yellow, blue, string("no flip needed"));
      }
    }
    else { // quadrilateral is not convex, so we cannot flip
        edge e2 = G.face_cycle_succ(e1);
        edge e4 = G.face_cycle_succ(e3);    
	output_graph(gw,G,e1,e2,e3,e4,e,r, green, blue, string("quadrilateral not convex, flip not possible"));
    }

  }

  return flip_count;
}

void GW_DELAUNAY_FLIP(const list<POINT>& L, GRAPH<POINT,int>& G)
{ TRIANGULATE_POINTS(L,G);
  if (G.number_of_edges() == 0) return;
  GW_DELAUNAY_FLIPPING(G,NEAREST);
}

class del_update : public geowin_update<list<POINT>, list<SEGMENT> >
{
public:
 void update(const list<POINT>& L, list<SEGMENT>& LS)
 {
   LS.clear();
   GRAPH<POINT,int> G;
   wait_time = 1.3;
   GW_DELAUNAY_FLIP(L,G);
   edge e;
   forall_edges(e,G) LS.append(SEGMENT(G[G.source(e)], G[G.target(e)]));
 }
};


int main()
{
  GeoWin gw("Delaunay flipping");
  gw.message("Delaunay flipping animation. See help text in help menu.");
  list<POINT> LP;
 
  geo_scene sc1 = gw.new_scene(LP);
  
  del_update delaunay;
  geo_scene sc2 = gw.new_scene(delaunay, sc1, "Delaunay Flipping");
  gw.set_color(sc2,blue2);
  gw.set_line_width(sc2,2);
  
  gw.set_all_visible(true);
  
  gw.add_special_help_text("geowin_flip_delaunay", true); //add demo help text, display it at start ...  
  gw.init_menu();
  gw.get_window().bool_item(" Mouse input in animation:",bval);
  
  gw.edit(sc1);
  
  return 0;
}




