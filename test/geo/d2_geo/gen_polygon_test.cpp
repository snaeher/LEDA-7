/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gen_polygon_test.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rat_gen_polygon.h>


using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::flush;


using namespace leda;

bool equal(const rat_gen_polygon& P, const rat_gen_polygon& Q)
{
	return P.sym_diff(Q).empty();
}

bool check_bool_ops(const rat_gen_polygon& P, const rat_gen_polygon& Q,
					const rat_gen_polygon& P_unite_Q,
					const rat_gen_polygon& P_intersection_Q,
					const rat_gen_polygon& P_diff_Q,
					const rat_gen_polygon& P_symdiff_Q)
{
	bool result = true;

	rat_gen_polygon R;

	R = P.unite(Q);
	if (!equal(R, P_unite_Q)) {
		cout << "  P union Q failed" << endl;
		result = false;
	}

	R = P.intersection(Q);
	if (!equal(R, P_intersection_Q)) {
		cout << "  P intersection Q failed" << endl;
		result = false;
	}

	R = P.diff(Q);
	if (!equal(R, P_diff_Q)) {
		cout << "  P diff Q failed" << endl;
		result = false;
	}

	R = P.sym_diff(Q);
	if (!equal(R, P_symdiff_Q)) {
		cout << "  P symdiff Q failed" << endl;
		result = false;
	}

	return result;
}


static rat_gen_polygon generate_test_GP()
{
	list<rat_point> vertices;

	// P
	vertices.clear();
	vertices.append( point(0, 0) );
	vertices.append( point(5, 0) );
	vertices.append( point(5, 6) );
	vertices.append( point(0, 6) );

	rat_polygon P( vertices );

	// P1
	vertices.clear();
	vertices.append( point(1, 1) );
	vertices.append( point(1, 4) );
	vertices.append( point(4, 4) );
	vertices.append( point(4, 1) );

	rat_polygon P1( vertices );

	// P2
	vertices.clear();
	vertices.append( point(3, 5.5) );
	vertices.append( point(3, 4.5) );
	vertices.append( point(2, 4.5) );
	vertices.append( point(2, 5.5) );

	rat_polygon P2( vertices );

	// Q
	vertices.clear();
	vertices.append( point(1.5, 1.5) );
	vertices.append( point(3.5, 1.5) );
	vertices.append( point(3.5, 3.5) );
	vertices.append( point(1.5, 3.5) );

	rat_polygon Q( vertices );

	// Q1
	vertices.clear();
	vertices.append( point(3, 2) );
	vertices.append( point(2, 3) );
	vertices.append( point(3, 3) );

	rat_polygon Q1( vertices );


	// GP
	list<rat_polygon> boundary_rep;
	boundary_rep.append(P);
	boundary_rep.append(P1);
	boundary_rep.append(Q);
	boundary_rep.append(P2);
	boundary_rep.append(Q1);

	rat_gen_polygon GP(boundary_rep);

	return GP;
}

bool test_basic()
{
	cout << "Test Basic ..." << endl;

	rat_gen_polygon GP1 = generate_test_GP();
	rat_gen_polygon GP2 = generate_test_GP();

	GP1.normalize();

	bool ok = equal(GP1, GP2);

	if (ok)
		cout << "Test passed." << endl;
	else
		cout << "Test FAILED!" << endl;

	cout << endl;

	return ok;
}

bool test1()
{
	cout << "Test 1 ..." << endl;

	list<rat_point> vertices;

	vertices.append( rat_point( 51400,138500 ) );
	vertices.append( rat_point( 73400,138500 ) );
	vertices.append( rat_point( 51400,160500 ) );

	rat_gen_polygon P(vertices, 
                      rat_gen_polygon::NO_CHECK, 
                      rat_gen_polygon::DISREGARD_ORIENTATION);

	vertices.clear();

	vertices.append( rat_point( 15200,161400 ) ); 
	vertices.append( rat_point( 16100,160500 ) ); 
	vertices.append( rat_point( 38100,138500 ) ); 
	vertices.append( rat_point( 73400,103200 ) );
	vertices.append( rat_point( 73400,138500 ) );
	vertices.append( rat_point( 51400,160500 ) );
	vertices.append( rat_point( 51400,161400 ) );

	rat_gen_polygon Q(vertices, 
                      rat_gen_polygon::NO_CHECK, 
                      rat_gen_polygon::DISREGARD_ORIENTATION);

	rat_gen_polygon P_unite_Q = Q;

	rat_gen_polygon P_intersection_Q = P;

	rat_gen_polygon P_diff_Q;

	vertices.clear();

	vertices.append( rat_point( 15200,161400 ) );
	vertices.append( rat_point( 16100,160500 ) );
	vertices.append( rat_point( 38100,138500 ) );
	vertices.append( rat_point( 73400,103200 ) );
	vertices.append( rat_point( 73400,138500 ) );
	vertices.append( rat_point( 51400,138500 ) );
	vertices.append( rat_point( 51400,160500 ) );
	vertices.append( rat_point( 51400,161400 ) );

	rat_gen_polygon P_symdiff_Q(vertices, 
		                        rat_gen_polygon::NO_CHECK, 
							    rat_gen_polygon::DISREGARD_ORIENTATION);

	bool ok = check_bool_ops(P, Q, 
		                     P_unite_Q, 
				    	     P_intersection_Q, 
						     P_diff_Q, 
						     P_symdiff_Q);

	if (ok)
		cout << "Test passed." << endl;
	else
		cout << "Test FAILED!" << endl;

	cout << endl;

	return ok;
}

bool test2()
{
	cout << "Test 2 ..." << endl;

	list<rat_point> vertices;

	vertices.append( rat_point( 1,1 ) );
	vertices.append( rat_point( 4,1 ) );
	vertices.append( rat_point( 4,4 ) );
	vertices.append( rat_point( 1,4 ) );

	rat_gen_polygon P(vertices);

	rat_polygon outer(vertices);

	vertices.clear();

	vertices.append( rat_point( 2,2 ) );
	vertices.append( rat_point( 3,2 ) );
	vertices.append( rat_point( 3,3 ) );
	vertices.append( rat_point( 2,3 ) );

	rat_gen_polygon Q(vertices);

	vertices.reverse();

	rat_polygon hole(vertices);


	rat_gen_polygon P_unite_Q = P;

	rat_gen_polygon P_intersection_Q = Q;


	list<rat_polygon> chains;
	chains.append( outer );
	chains.append( hole );

	rat_gen_polygon P_diff_Q(chains);

	rat_gen_polygon P_symdiff_Q = P_diff_Q;

	bool ok = check_bool_ops(P, Q, 
		                     P_unite_Q, 
				    	     P_intersection_Q, 
						     P_diff_Q, 
						     P_symdiff_Q);

	if (ok)
		cout << "Test passed." << endl;
	else
		cout << "Test FAILED!" << endl;

	cout << endl;

	return ok;
}

bool test3()
{
	cout << "Test 3 ..." << endl;

	list<rat_point> vertices;

	vertices.append( rat_point( 3,1 ) );
	vertices.append( rat_point( 7,1 ) );
	vertices.append( rat_point( 7,5 ) );
	vertices.append( rat_point( 3,5 ) );

	rat_gen_polygon P(vertices, 
                      rat_gen_polygon::NO_CHECK, 
                      rat_gen_polygon::DISREGARD_ORIENTATION);

	vertices.clear();
	vertices.append( rat_point( 1,2 ) );
	vertices.append( rat_point( 9,2 ) );
	vertices.append( rat_point( 5,6 ) );

	rat_gen_polygon Q(vertices, 
                      rat_gen_polygon::NO_CHECK, 
                      rat_gen_polygon::DISREGARD_ORIENTATION);

	vertices.clear();
	vertices.append( rat_point( 1,2 ) );
	vertices.append( rat_point( 3,2 ) );
	vertices.append( rat_point( 3,1 ) );
	vertices.append( rat_point( 7,1 ) );
	vertices.append( rat_point( 7,2 ) );
	vertices.append( rat_point( 9,2 ) );
	vertices.append( rat_point( 7,4 ) );
	vertices.append( rat_point( 7,5 ) );
	vertices.append( rat_point( 6,5 ) );
	vertices.append( rat_point( 5,6 ) );
	vertices.append( rat_point( 4,5 ) );
	vertices.append( rat_point( 3,5 ) );
	vertices.append( rat_point( 3,4 ) );

	rat_gen_polygon P_unite_Q(vertices);

	vertices.clear();
	vertices.append( rat_point( 3,2 ) );
	vertices.append( rat_point( 7,2 ) );
	vertices.append( rat_point( 7,4 ) );
	vertices.append( rat_point( 6,5 ) );
	vertices.append( rat_point( 4,5 ) );
	vertices.append( rat_point( 3,4 ) );

	rat_gen_polygon P_intersection_Q(vertices);


	list<rat_polygon> chains;

	vertices.clear();
	vertices.append( rat_point( 6,5 ) );
	vertices.append( rat_point( 7,4 ) );
	vertices.append( rat_point( 7,5 ) );
	chains.append( rat_polygon(vertices) );
	
	vertices.clear();
	vertices.append( rat_point( 3,1 ) );
	vertices.append( rat_point( 7,1 ) );
	vertices.append( rat_point( 7,2 ) );
	vertices.append( rat_point( 3,2 ) );
	chains.append( rat_polygon(vertices) );

	vertices.clear();
	vertices.append( rat_point( 3,4 ) );
	vertices.append( rat_point( 4,5 ) );
	vertices.append( rat_point( 3,5 ) );
	chains.append( rat_polygon(vertices) );

	rat_gen_polygon P_diff_Q(chains);

	
	chains.clear();

	vertices.clear();
	vertices.append( rat_point( 1,2 ) );
	vertices.append( rat_point( 3,2 ) );
	vertices.append( rat_point( 3,4 ) );
	chains.append( rat_polygon(vertices) );
 
	vertices.clear();
	vertices.append( rat_point( 3,1 ) );
	vertices.append( rat_point( 7,1 ) );
	vertices.append( rat_point( 7,2 ) );
	vertices.append( rat_point( 3,2 ) );
	chains.append( rat_polygon(vertices) );

	vertices.clear();
	vertices.append( rat_point( 3,4 ) );
	vertices.append( rat_point( 4,5 ) );
	vertices.append( rat_point( 3,5 ) );
	chains.append( rat_polygon(vertices) );

	vertices.clear();
	vertices.append( rat_point( 4,5 ) );
	vertices.append( rat_point( 6,5 ) );
	vertices.append( rat_point( 5,6 ) );
	chains.append( rat_polygon(vertices) );

	vertices.clear();
	vertices.append( rat_point( 6,5 ) );
	vertices.append( rat_point( 7,4 ) );
	vertices.append( rat_point( 7,5 ) );
	chains.append( rat_polygon(vertices) );

	vertices.clear();
	vertices.append( rat_point( 7,2 ) );
	vertices.append( rat_point( 9,2 ) );
	vertices.append( rat_point( 7,4 ) );
	chains.append( rat_polygon(vertices) );
 
 	rat_gen_polygon P_symdiff_Q(chains);

	bool ok = check_bool_ops(P, Q, 
		                     P_unite_Q, 
				    	     P_intersection_Q, 
						     P_diff_Q, 
						     P_symdiff_Q);

	if (ok)
		cout << "Test passed." << endl;
	else
		cout << "Test FAILED!" << endl;

	cout << endl;

	return ok;
}

int main()
{
	if (!test_basic()) return -1;

	if (!test1()) return -1;

	if (!test2()) return -1;

	if (!test3()) return -1;

	cout << "All tests completed successfully!" << endl;

	return 0;
}
