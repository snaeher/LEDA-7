#include<LEDA/graphics/rat_window.h>
#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/graphics/bitmaps/button21.h>

#include <assert.h>

using namespace leda;


rat_point  MaxAreaTriangle(const list<rat_point>& L, const rat_point& a, 
                                                     const rat_point& b)
{
  assert(!L.empty());

  double DX = a.XD()*b.WD() - b.XD()*a.WD();
  double DY = a.YD()*b.WD() - b.YD()*a.WD();

  rat_point p_max = L.head();
  double A_max = DY * p_max.XD() - DX * p_max.YD();

  rat_point p;
  forall(p,L)
  { if (identical(p,p_max)) continue;
    double A = DY * p.XD() - DX * p.YD();
    if (A*p_max.WD() > A_max*p.WD())
    { p_max = p;
      A_max = A;
     }
   }
  return p_max;
}
   

void max_triang(const list<rat_point>& L, list<rat_segment>& T)
{ 
  if (L.length() < 2) return;
  rat_point a = L[L[0]];
  rat_point b = L[L[1]];
  rat_point c =  MaxAreaTriangle(L,a,b);

  T.clear();
  T.append(rat_segment(a,b));
  T.append(rat_segment(b,c));
  T.append(rat_segment(c,a));
}


void max_triang1(const list<rat_point>& L, list<rat_triangle>& T)
{ 
  if (L.length() < 2) return;
  rat_point a = L[L[0]];
  rat_point b = L[L[1]];
  rat_point c =  MaxAreaTriangle(L,a,b);

  T.clear();
  T.append(rat_triangle(a,b,c));
}



int main()
{
  GeoWin GW("Max Triangle Demo");
  
  list<rat_point> L;
  geo_scene sc_input = GW.new_scene(L); 
  
  geowin_update<list<rat_point>, list<rat_segment> >  MAX_TRIANG(max_triang);
  geowin_update<list<rat_point>, list<rat_triangle> > MAX_TRIANG1(max_triang1);
  
  geo_scene sc1 = GW.new_scene(MAX_TRIANG, sc_input, "MAX_TRIANG");
  GW.set_color(sc1,red);

  geo_scene sc2 = GW.new_scene(MAX_TRIANG1, sc_input, "MAX_TRIANG");
  GW.set_color(sc1,blue);
  GW.set_fill_color(sc1,ivory);

  GW.set_z_order(sc_input,1);
  GW.set_z_order(sc1,2);
  GW.set_z_order(sc2,3);


  GW.set_all_visible(true);
  
  GW.set_button_width(21); 
  GW.set_button_height(21);
  GW.set_bitmap(sc1, triang_21_bits);
  GW.set_bitmap(sc2, triang_21_bits);

  GW.edit(sc_input);
  
  return 0;
}

