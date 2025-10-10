#include <LEDA/geo/rat_point_set.h>
#include <LEDA/geo/rat_point_locator.h>
#include <LEDA/geo/rat_geo_alg.h>

#include <LEDA/core/array2.h>

using namespace leda;

using std::cout;
using std::endl;
using std::flush;


bool IsHullEdge(const GRAPH<rat_point,int>& G, edge e)
{ if (e == 0) return false;
  edge x = G.face_cycle_succ(e);
  return (orientation(G[source(e)],G[target(e)],G[target(x)]) <= 0);
 }



int main()
{
  cout << endl;

  int n = read_int("number of points: n = ");
  cout << endl;

  list<rat_point> Points;

//random_points_in_square(n,100000,Points);
  random_points_in_disc(n,100000,Points);


  float T = used_time();

  cout << string("Delaunay Triangulation ...   ") << flush;
  GRAPH<rat_point,int> G;
  DELAUNAY_TRIANG(Points,G);
  cout << string("%5.2f sec",used_time(T)) << endl;


  cout << string("Constructing Locator ...     ") << flush;
  rat_point_locator locator(G);
  cout << string("%5.2f sec",used_time(T)) << endl;

  cout << string("Constructing Point Set ...   ") << flush;
  rat_point_set PS(G);
  cout << string("%5.2f sec",used_time(T)) << endl;

  // generate query points

  cout << endl;
  int m = read_int("number of queries: m = ");
  cout << endl;

  list<rat_point> L;
  for(int i = 0; i < m; i++)
  { int x = rand_int(-100000,10000);
    int y = rand_int(-100000,10000);
    L.append(rat_point(x,y,1));
   }


  T = used_time();

  array<edge> A(m);
  rat_point p;
  edge e = G.first_edge();
  int hull_edges = 0;
  int i = 0;

  int d = (m < 10) ? 1 : m/10;

  cout << string("Locate Points (locator)   ") << flush;
  i = 0;
  forall(p,L) 
  { A[i] = locator.locate(p);
    if (i++ % d == 0) cout << "." << flush;
   }
  cout << string("  %5.2f sec",used_time(T)) << endl;

  cout << "checking ... " << flush;
  hull_edges = 0;
  i = 0;
  forall(p,L) 
  { edge e = A[i++];
    if (locator.is_hull_edge(e)) hull_edges++;
    assert(locator.check_locate(p,e));
   }
  cout << "ok." << endl;
  cout << "hull edges: " << hull_edges << endl;
  cout << endl;


  cout << string("Locate Points (function)  ") << flush;
  i = 0;
  forall(p,L)
  { e = LOCATE_IN_TRIANGULATION(G,p,e);
    A[i] = e;
    if (i++ % d == 0) cout << "." << flush;
   }
  cout << string("  %5.2f sec",used_time(T)) << endl;

  hull_edges = 0;
  i = 0;
  forall(p,L) 
  { edge e = A[i++];
    if (IsHullEdge(G,e)) hull_edges++;
   }
  cout << "hull edges: " << hull_edges << endl;
  cout << endl;



  cout << string("PointSet::locate          ") << flush;
  i = 0;
  e = G.first_edge();
  forall(p,L) 
  { A[i] = PS.locate(p,0);
    if (i++ % d == 0) cout << "." << flush;
   }
  cout << string("  %5.2f sec", used_time(T)) << endl;

  hull_edges = 0;
  i = 0;
  forall(p,L)
  { edge e = A[i++];
    if (e && PS.is_hull_edge(e)) hull_edges++;
   }
  cout << "hull edges: " << hull_edges << endl;
  cout << endl;


  cout << endl;

  return 0;
}
