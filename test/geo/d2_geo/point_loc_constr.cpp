#include <LEDA/geo/rat_point_locator.h>
#include <LEDA/geo/rat_geo_alg.h>

using namespace leda;

using std::cout;
using std::endl;
using std::flush;


void generate_disjoint_segments(int n, list<rat_segment>& L)
{ list<rat_point> Points;
  random_points_in_disc(n,100000,Points);
  GRAPH<rat_point,int> G;
  TRIANGULATE_POINTS(Points,G);

  L.clear();
  edge_array<bool> handled(G,false);
  edge e;
  forall_edges(e,G)
  { if (handled[e]) continue;
    edge r = G.reversal(e);
    handled[r] = true;
    rat_point a = G[source(e)];
    rat_point b = G[target(e)];
    rat_point c = midpoint(a,b);
    a = midpoint(a,c);
    b = midpoint(c,b);
    L.append(rat_segment(a,b));
  }
}




int main()
{
  cout << endl;
  int n = read_int("n = ");

  list<rat_segment> Segments;

  generate_disjoint_segments(n,Segments);

  cout << endl;
  cout << "segments: " << Segments.length() << endl;

  float T = used_time();

  cout << string("Constrained Delaunay Triangulation ...   ") << flush;
  GRAPH<rat_point,rat_segment> G;
  DELAUNAY_TRIANG(Segments,G);
  cout << string("%5.2f sec",used_time(T)) << endl;

  cout << "nodes: " << G.number_of_nodes() << endl;
  cout << "edges: " << G.number_of_edges() << endl;
  cout << endl;


  cout << string("Constructing Locator ...                 ") << flush;
  rat_point_locator locator(G);
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
  int i;

  int d = m/10;

  cout << string("Locate Points (locator)   ") << flush;
  i = 0;
  forall(p,L) 
  { A[i] = locator.locate(p);
    if (i++ % d == 0) cout << "." << flush;
   }
  cout << string("  %5.2f sec",used_time(T)) << endl;

  cout << "checking ... " << flush;
  int hull_edges = 0;
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
  cout << endl;

  return 0;
}
