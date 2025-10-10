#include <LEDA/geo/geo_alg.h>

using namespace leda;
using std::cout;
using std::endl;

int main()
{
  int seg_num = read_int("#segments = ");

  for(;;)
  { list<rat_segment> seglist;

    for (int i = 0; i < seg_num; i++)
    { leda::rat_point p, q;
      leda::random_point_in_unit_square(p);
      leda::random_point_in_unit_square(q);
      if (p != q) seglist.push_back(leda::rat_segment(p, q));
     }

    float T = used_time();
    float t = elapsed_time();

    GRAPH<rat_point,rat_segment> G;
    SWEEP_SEGMENTS(seglist,G,true);

    list<rat_point> L;
    node v;
    forall_nodes(v,G) L.append(G[v]);
    GRAPH<rat_circle,rat_point> V;
    VORONOI(L,V);

    cout << string("|V|= %d  |E| = %d  cpu: %.2f  real: %.2f",
                       V.number_of_nodes(), V.number_of_edges(), 
                       used_time(T), elapsed_time(t));
    cout << endl;
    rat_point::print_statistics();
   }

  return 0;
}
