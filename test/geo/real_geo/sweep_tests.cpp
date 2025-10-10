/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  sweep_tests.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/stream.h>
#include <LEDA/system/timer.h>
#include <LEDA/system/file.h>

#include <stdlib.h>

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif

#include <LEDA/geo/real_geo_alg.h>

#include <LEDA/geo/r_circle_segment.h>

namespace leda {
	typedef r_circle_segment SEGMENT;
	typedef r_circle_point   POINT;
}

using namespace leda;

static bool exit_on_failure = true;

static list<SEGMENT> seglist;

static
void gen_random_segments(list<SEGMENT>& seglist, int N, 
						 int percent_straight = 50, 
						 int grid_size = 10, double dist = 10)
{
	seglist.clear();

	int gmin = -grid_size;
	int gmax =  grid_size;

	int N_straight = int( double(percent_straight) * N / 100 );
	int N_arc      = N - N_straight;

	int i;
	for(i = 0; i < N_arc; i++)
	{
		double x0 = rand_int(gmin,gmax)*dist;
		double y0 = rand_int(gmin,gmax)*dist;
		double x1 = rand_int(gmin,gmax)*dist;
		double y1 = rand_int(gmin,gmax)*dist;
		double x2 = rand_int(gmin,gmax)*dist;
		double y2 = rand_int(gmin,gmax)*dist;
		rat_point p0(x0,y0), p1(x1,y1), p2(x2,y2);
		if (p0 == p1 && p1 == p2 && p2 == p0) {
			seglist.append(SEGMENT(p0));
		}
		else if (p0 == p1 || p1 == p2 || p2 == p0 || rat_circle(p0,p1,p2).orientation()==0) {
			--i; continue;
		}
		else {
			seglist.append(SEGMENT(p0,p1,p2));
		}
	}

	for(i = 0; i < N_straight; i++)
	{
		double x0 = rand_int(gmin,gmax)*dist;
		double y0 = rand_int(gmin,gmax)*dist;
		double x1 = rand_int(gmin,gmax)*dist;
		double y1 = rand_int(gmin,gmax)*dist;
		seglist.append( SEGMENT(rat_point(int(x0),int(y0)),rat_point(int(x1),int(y1))) );
	}
}

/*
static void add_curved_polygon(const polygon& poly, list<SEGMENT>& seglist)
{
	segment _s;
	forall_segments(_s, poly) {
		rat_segment s(_s);
		rat_point middle = s.source() + (s.target() - s.source()) / 2;
		rat_segment rot = s.rotate90(middle);
		r_circle_segment cs(s.source(), rot.source(), s.target());
		seglist.append(cs);
	}
}

static void gen_ngon(list<SEGMENT>& seglist, int N)
{
	seglist.clear();

	double radius = 800;
	circle C(point(radius,0), point(0,radius), point(-radius,0));
	polygon poly = n_gon(N, C, radius/1024);

	add_curved_polygon(poly, seglist);
	add_curved_polygon(poly.rotate(LEDA_PI/(2*N)), seglist);
};
*/


template <class S>
bool convert(const list<r_circle_segment>& seglist, list<S>& rat_segs)
{
	rat_segs.clear();

	r_circle_segment seg;
	forall(seg, seglist) {
		if (seg.is_rat_segment())
		rat_segs.append(seg.to_rat_segment());
		else return false;
	}

	return true;
}

/*
void convert(const GRAPH<rat_point,rat_segment>& G_rat, GRAPH<POINT,SEGMENT>& G)
{
  G.clear();

  node_array<node> node_in_G(G_rat);

  node n;
  forall_nodes(n, G_rat) {
	node_in_G[n] = G.new_node(POINT(G_rat[n]));
  }

  edge e;
  forall_edges(e, G_rat) {
    SEGMENT seg(G_rat[e]);
	G.new_edge(node_in_G[source(e)], node_in_G[target(e)], seg);
  }
}
*/

static int run_alg(const list<SEGMENT>& seglist, GRAPH<POINT,SEGMENT>& G, bool embed)
{
    G.clear();
    G.make_directed();

	timer run_time;
	float t;
	string alg;

	alg = "sweep r_circle_segment";
	run_time.start();
	SWEEP_SEGMENTS(seglist, G, embed);
	run_time.stop();

	t = run_time.elapsed_time();
	int n1 = G.number_of_nodes();
    int m1 = G.number_of_edges();

	cout << string("%s |S| = %d  |V| = %d |E| = %d  t = %6.2f sec", 
                        alg, seglist.size(),n1,m1,t)  << endl;

	G.clear();

/*	list<rat_segment> rat_segs;
	if (convert(seglist, rat_segs)) {
		alg = "sweep rat_segment";
		GRAPH<rat_point,rat_segment> _G;
		run_time.reset(); run_time.start();
		SWEEP_SEGMENTS(rat_segs,_G,embed);
		run_time.stop();
		convert(_G, G);
	}
	else {
*/	    alg = "brute force";
		run_time.reset(); run_time.start();
		INTERSECT_SEGMENTS_BRUTE_FORCE(seglist,G);
		run_time.stop();
//	}

	t = run_time.elapsed_time();
	int n2 = G.number_of_nodes();
    int m2 = G.number_of_edges();

	cout << string("%s |S| = %d  |V| = %d |E| = %d  t = %6.2f sec", alg, seglist.size(),n2,m2,t) 
		 << endl << endl;

/*
	cout << "(Note that brute force never computes any edges, that's ok ...)" << endl;
*/

	if (n1 != n2) {
		cout << "ERROR: sweep and brute force differ!" << endl;
		if (exit_on_failure) exit(1);
	}

	return n1 == n2 ? 0 : 1;
}
  
static void write_file(string fn)
{
	file_ostream out(fn);
	//seglist.print(out);

  SEGMENT seg;
  forall(seg,seglist)
     out << seg << endl;
}

// randomly select some of the segments
static void select(list<SEGMENT>& L)
{
	L.permute();
	int to_del = L.size() / 4;
	if (to_del == 0) to_del = 1;
	while (to_del-- > 0 && !L.empty()) L.pop();
	write_file("_save.dat");
}

static void read_file(string fn)
{ 
	file_istream in(fn);
	seglist.read(in);
//	select(seglist);
}

//static 
void find_error()
{
	exit_on_failure = false;

	int i = 0;

	string old_file = "segs.dat";

	while (++i < 300) {
		string cur_file = string("segs%d.dat", i);

		read_file(old_file);
		select(seglist);
		write_file(cur_file);

		cout << "cur_file: " << cur_file << "  /  old_file: " << old_file << endl;
		GRAPH<POINT,SEGMENT> G;
		bool ok = run_alg(seglist, G, false) == 0;
		cout << endl;

		if (ok) {
			delete_file(cur_file);
		}
		else {
			old_file = cur_file;
		}
	}

	cout << endl << "final_file: " << old_file << endl;
	read_file(old_file);
	GRAPH<POINT,SEGMENT> G;
	bool ok = run_alg(seglist, G, false) == 0;
	cout << "ok = " << ok << endl;

	exit(1);
}

int run_test()
{
	// determine a new seed and report it!
	int seed = rand_int.reinit_seed();
	cout << "seed = " << seed << endl; cout.flush();
	rand_int.set_seed(seed);

	int N = 100;

	gen_random_segments(seglist, N);
	write_file("segs.dat");

	GRAPH<POINT,SEGMENT> G;

	return run_alg(seglist, G, false);
}


int run_test(string fn, int a, int b)
{
	file_istream in(fn);
        SEGMENT seg;
        seglist.clear();
        int i = 0;

        while (in)
        { in >> seg;
          i++;
          if (i >= a && i <= b) seglist.append(seg);
         }

	GRAPH<POINT,SEGMENT> G;
	return run_alg(seglist, G, false);
}



int run_tests(int runs = 100)
{
  while (--runs >= 0) 
  { int res = run_test();
    if (res != 0) return res;
   }
  return 0;
}

int main(int argc, char** argv)
{ 
  rat_point::use_filter = 0;
  rat_segment::use_filter = 0;

  //find_error(); return 0;


  if (argc > 1)
    { int a = 1;
      int b = MAXINT;
      if (argc > 2) a = atoi(argv[2]);
      if (argc > 3) b = atoi(argv[3]);
      return run_test(argv[1],a,b);
     }
  else
    return run_tests();
}
