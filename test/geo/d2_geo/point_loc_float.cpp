#include <LEDA/geo/point_set.h>
#include <LEDA/geo/point_locator.h>
#include <LEDA/geo/geo_alg.h>

using namespace leda;

using std::cout;
using std::endl;
using std::flush;




int main()
{
  cout << endl;

  int n = read_int("number of points: n = ");
  cout << endl;

  list<point> Points;

//random_points_in_square(n,100000,Points);
  random_points_in_disc(n,100000,Points);


  float T = used_time();

  cout << string("Delaunay Triangulation ...   ") << flush;
  GRAPH<point,int> G;
  DELAUNAY_TRIANG(Points,G);
  cout << string("%5.2f sec",used_time(T)) << endl;


  cout << string("Constructing Locator ...     ") << flush;
  point_locator loc(G);
  cout << string("%5.2f sec",used_time(T)) << endl;


  cout << string("Constructing Point Set ...   ") << flush;
  point_set PS(G);
  cout << string("%5.2f sec",used_time(T)) << endl;
  cout << endl;


  // generate query points

  int m = read_int("number of queries: m = ");
  cout << endl;

  list<point> L;
  for(int i = 0; i < m; i++)
  { int x = rand_int(-100000,10000);
    int y = rand_int(-100000,10000);
    L.append(point(x,y,1));
   }


  T = used_time();

  array<edge> A(m);
  point p;
  edge e = G.first_edge();
  int i;

  int d = m/10;

  cout << string("Locate Points (locator)   ") << flush;
  i = 0;
  forall(p,L) 
  { A[i] = loc.locate(p);
    if (i++ % d == 0) cout << "." << flush;
   }
  cout << string("  %5.2f sec",used_time(T)) << endl;

  cout << "checking ... " << flush;
  i = 0;
  forall(p,L) assert(loc.check_locate(p,A[i++]));
  cout << "ok." << endl;


  cout << string("Locate Points (function)  ") << flush;
  i = 0;
  forall(p,L)
  { e = LOCATE_IN_TRIANGULATION(G,p,e);
    A[i] = e;
    if (i++ % d == 0) cout << "." << flush;
   }
  cout << string("  %5.2f sec",used_time(T)) << endl;


  cout << string("PointSet::locate          ") << flush;
  i = 0;
  e = G.first_edge();
  forall(p,L) 
  { A[i] = PS.locate(p,e);
    if (i++ % d == 0) cout << "." << flush;
   }
  cout << string("  %5.2f sec", used_time(T)) << endl;
  cout << endl;

  return 0;
}
