#include <LEDA/geo/rat_point_set.h>
#include <LEDA/geo/rat_point_locator.h>
#include <LEDA/geo/rat_geo_alg.h>

using namespace leda;

using std::cout;
using std::endl;
using std::flush;


int main()
{
  int n = read_int("n = ");

  float T = used_time();
   
  cout << string("Generate points ...          ") << flush;


  list<rat_point> Points;

  //random_points_in_square(n,100000,Points);
  random_points_in_disc(n,100000,Points);

/*
  { rat_point p(0,0,1);
    rat_point q(100000,rand_int(0,100000),1);
    random_points_on_segment(rat_segment(p,q),100000,Points);
   }
*/

  cout << string("%5.2f sec",used_time(T)) << endl;



  cout << string("Delaunay Triangulation ...   ") << flush;
  GRAPH<rat_point,int> G;
  DELAUNAY_TRIANG(Points,G);
  cout << string("%5.2f sec",used_time(T)) << endl;

  cout << string("Constructing Point Set ...   ") << flush;
  rat_point_set PS(G);
  cout << string("%5.2f sec",used_time(T)) << endl;

  cout << string("Constructing pl-Structure... ") << flush;
  rat_point_locator ploc(G,G.node_data());
  cout << string("%5.2f sec",used_time(T)) << endl;

  cout << string("cells: %5d", ploc.number_of_cells()) << endl;
  cout << string("empty: %5d", ploc.number_of_empty_cells()) << endl;
  cout << endl;





  list<rat_point> L;

  rat_point p(-100000,200000,1);
  rat_point q(+100000,200000,1);

  int k = n / 10;
  if (k < 1000) k = 1000;
  random_points_on_segment(rat_segment(p,q),k,L);

  for(int i = 0; i < k; i++)
  { int x = rand_int(-100000,10000);
    int y = rand_int(-100000,10000);
    L.append(rat_point(x,y,1));
   }


  k = 0;
  forall(p,Points)
  { if (k % 20 == 0) L.append(p);
    k++;
   }

  k = 0;
  edge e;
  forall_edges(e,G)
  { if (k % 400 == 0)
    { rat_point q = midpoint(G[source(e)],G[target(e)]);
      L.append(q);
     }
    k++;
  }


  L.permute();

  int m = L.length();

  cout << endl;
  cout << string("%d random points",m) << endl;

  array<edge> A(m);

  cout << string("Locate Points  ...           ") << flush;
  int i = 0;
  e = G.first_edge();
  forall(p,L)
  { e = LOCATE_IN_TRIANGULATION(G,p,e);
    A[i++] = e;
   }
  cout << string("%5.2f sec",used_time(T)) << endl;


  cout << string("Locate Points (ploc) ...       ") << flush;
  i = 0;
  forall(p,L) A[i++] = ploc.locate(p);
  cout << string("%5.2f sec",used_time(T)) << endl;

  i = 0;
  forall(p,L) CHECK_LOCATE_IN_TRIANGULATION(G,G.node_data(),p,A[i++]);



  cout << string("PointSet::locate ...         ") << flush;
  i = 0;
  e = G.first_edge();
  forall(p,L) A[i++] = PS.locate(p,e);
  cout << string("%5.2f sec", used_time(T)) << endl;
  cout << endl;

  L.sort();

  cout << endl;
  cout << string("%d sorted points",m) << endl;

  cout << string("Locate Points ...            ") << flush;
  i = 0;
  e = G.first_edge();
  forall(p,L)
  { e = LOCATE_IN_TRIANGULATION(G,p,e);
    A[i++] = e;
   }
  cout << string("%5.2f sec",used_time(T)) << endl;

  i = 0;
  forall(p,L) CHECK_LOCATE_IN_TRIANGULATION(G,G.node_data(),p,A[i++]);

  cout << string("Locate Points (ploc) ...       ") << flush;
  i = 0;
  forall(p,L) A[i++] = ploc.locate(p);
  cout << string("%5.2f sec",used_time(T)) << endl;

  i = 0;
  forall(p,L) CHECK_LOCATE_IN_TRIANGULATION(G,G.node_data(),p,A[i++]);


  cout << string("PointSet::locate ...         ") << flush;
  i = 0;
  e = G.first_edge();
  forall(p,L) A[i++] = PS.locate(p,e);
  cout << string("%5.2f sec", used_time(T)) << endl;


 return 0;
}
