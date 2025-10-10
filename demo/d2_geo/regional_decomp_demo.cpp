#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/graphics/rat_window.h>

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::streambuf;
using std::ostream;
using std::istream;
using std::ifstream;
#endif

using namespace leda;

int main()
{
	list<rat_point> vertices;

	// P
	vertices.clear();
	vertices.append( point(0, 0) );
	vertices.append( point(5, 0) );
	vertices.append( point(5, 6) );
	vertices.append( point(0, 6) );

	rat_polygon P( vertices );
	P.normalize();

	// P1
	vertices.clear();
	vertices.append( point(1, 1) );
	vertices.append( point(1, 4) );
	vertices.append( point(4, 4) );
	vertices.append( point(4, 1) );

	rat_polygon P1( vertices );
	P1.normalize();

	// P2
	vertices.clear();
	vertices.append( point(3, 5.5) );
	vertices.append( point(3, 4.5) );
	vertices.append( point(2, 4.5) );
	vertices.append( point(2, 5.5) );

	rat_polygon P2( vertices );
	P2.normalize();

	// Q
	vertices.clear();
	vertices.append( point(1.5, 1.5) );
	vertices.append( point(3.5, 1.5) );
	vertices.append( point(3.5, 3.5) );
	vertices.append( point(1.5, 3.5) );

	rat_polygon Q( vertices );
	Q.normalize();

	// Q1
	vertices.clear();
	vertices.append( point(3, 2) );
	vertices.append( point(2, 3) );
	vertices.append( point(3, 3) );

	rat_polygon Q1( vertices );
	Q1.normalize();

	// R
	vertices.clear();
	vertices.append( point(6, 4) );
	vertices.append( point(7, 4) );
	vertices.append( point(8, 4) );
	vertices.append( point(8, 5) );
	vertices.append( point(8, 8) );
	vertices.append( point(7, 8) );
	vertices.append( point(6, 8) );
	vertices.append( point(6, 5) );

	rat_polygon R( vertices );

	// R1
	vertices.clear();
	vertices.append( point(7, 4) );
	vertices.append( point(6, 5) );
	vertices.append( point(7, 6) );
	vertices.append( point(8, 5) );

	rat_polygon R1( vertices );

	// R2
	vertices.clear();
	vertices.append( point(6.5, 6.5) );
	vertices.append( point(6.5, 7.5) );
	vertices.append( point(7.5, 7.5) );
	vertices.append( point(7.5, 6.5) );

	rat_polygon R2( vertices );

	// S
	vertices.clear();
	vertices.append( point(6, 0) );
	vertices.append( point(9, 0) );
	vertices.append( point(9, 3) );
	vertices.append( point(6, 3) );

	rat_polygon S( vertices );

	// S1
	vertices.clear();
	vertices.append( point(6.5, 0.5) );
	vertices.append( point(6.5, 2.5) );
	vertices.append( point(8.5, 2.5) );
	vertices.append( point(8.5, 0.5) );

	rat_polygon S1( vertices );

	// T
	vertices.clear();
	vertices.append( point(7, 1) );
	vertices.append( point(8, 1) );
	vertices.append( point(8, 2) );
	vertices.append( point(7, 2) );

	rat_polygon T( vertices );

	// U (two touching spikes)
	vertices.clear();
	vertices.append( point(1, 7) );
	vertices.append( point(3, 7) );
	vertices.append( point(5, 7) );
	vertices.append( point(4, 8) );
	vertices.append( point(3, 7) );
	vertices.append( point(2, 8) );

	rat_polygon U( vertices );


	// GP
	list<rat_polygon> boundary_rep;
	boundary_rep.append(P);
	boundary_rep.append(P1);
	boundary_rep.append(Q);
	boundary_rep.append(P2);
	boundary_rep.append(Q1);
	boundary_rep.append(S);
	boundary_rep.append(S1);
	boundary_rep.append(T);
	boundary_rep.append(R);
	boundary_rep.append(R1);
	boundary_rep.append(U);
	boundary_rep.append(R2);

	rat_gen_polygon GP(boundary_rep, rat_gen_polygon::NO_CHECK);
	GP.normalize();

	GRAPH<rat_polygon, int> Tree;
	//node root = 
          Nesting_Tree(GP, Tree);

	cout << "Nesting tree:" << endl;
	cout << Tree << endl;

	list<rat_gen_polygon> regions = GP.regional_decomposition();

	cout << regions << endl;

	// display
	window W("Regional Decomposition");

	W.init_rectangle( -1, 10, -1, 10 );

	//W.set_show_coordinates(true);

	W.display();

	W.draw_filled_polygon(GP.to_float());

	W.message("Input gen_polygon - press mouse button to continue");
	W.await_mouse_button();
	W.del_message();

	W.clear();

	int col = 1;
	rat_gen_polygon region;
	forall(region, regions)
		W.draw_filled_polygon(region.to_float(), ++col);

	W.message("Regional decomposition - press mouse button to finish");
	W.await_mouse_button();

	return 0;
}
