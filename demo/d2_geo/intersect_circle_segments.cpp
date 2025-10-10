/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  intersect_circle_segments.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/file_panel.h>
#include <LEDA/system/file.h>
#include <LEDA/system/stream.h>
#include <LEDA/system/timer.h>
#include <LEDA/core/d_array.h>

// switches between r_circle_segment and rat_segment
//#define LEDA_CURVE_SWEEP_RAT_SEGMENTS

// If LEDA_CURVE_SWEEP_RAT_SEGMENTS is NOT defined but all r_circle_segments
// are straight rat_segments, we also offer an optimized algorithm for rat_segments
// called "sweep".
// The define below decides whether "sweep" is the old LEDA function SWEEP_SEGMENTS
// or the new function CURVE_SWEEP_SEGMENTS(const list<rat_segment>&, ...).
#define USE_OLD_IMPL_FOR_STRAIGHT_SEGMENTS

#include <LEDA/geo/plane_alg.h>

#include <LEDA/geo/curve_sweep.h> // for LEDA_CURVE_SWEEP_VISUAL_DEBUG

#ifdef LEDA_CURVE_SWEEP_RAT_SEGMENTS
#include <LEDA/geo/rat_window.h>

namespace leda {
	typedef rat_segment SEGMENT;
	typedef rat_point   POINT;

	void WIN_SWEEP_SEGMENTS(const list<rat_segment>& S, GRAPH<rat_point,rat_segment>& G, bool embed, window* w);
	void INTERSECT_SEGMENTS_BRUTE_FORCE(const list<SEGMENT>& S, GRAPH<POINT,SEGMENT>& G);

#ifndef LEDA_CURVE_SWEEP_VISUAL_DEBUG
	void CURVE_SWEEP_SEGMENTS(const list<rat_segment>& S, GRAPH<rat_point,rat_segment>& G, bool embed);
	void WIN_SWEEP_SEGMENTS(const list<rat_segment>& S, GRAPH<rat_point,rat_segment>& G, bool embed, window* w)
	{ CURVE_SWEEP_SEGMENTS(S, G, embed); }
#endif
}

#else
#include <LEDA/graphics/real_window.h>
#include <LEDA/geo/r_circle_segment.h>

namespace leda {
	typedef r_circle_segment SEGMENT;
	typedef r_circle_point   POINT;

	void WIN_SWEEP_SEGMENTS(const list<r_circle_segment>& S, GRAPH<r_circle_point,r_circle_segment>& G, bool embed, window* w);

#ifndef LEDA_CURVE_SWEEP_VISUAL_DEBUG
	void WIN_SWEEP_SEGMENTS(const list<r_circle_segment>& S, GRAPH<r_circle_point,r_circle_segment>& G, bool embed, window* w)
	{ SWEEP_SEGMENTS(S, G, embed); }
#endif

__exportF 
void CURVE_SWEEP_SEGMENTS(const list<rat_segment>& S, 
						  GRAPH<rat_point,rat_segment>& G, 
						  bool embed);
}
#endif // #ifdef LEDA_CURVE_SWEEP_RAT_SEGMENTS


#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif

using namespace leda;

enum { file_load, file_save, file_all, file_exit, file_test };

static list<SEGMENT> seglist;
static window* win_ptr;
static GRAPH<POINT,SEGMENT>* gp;

static bool colorful_seglist = true;

#ifdef LEDA_CURVE_SWEEP_RAT_SEGMENTS
static string  fname = "segments.dat";
#else
static string  fname = "circ_segs.dat";
#endif

static string  dname = ".";
static string  filter = "";
static int     percent_straight = 0;

static string  test_file_dir = "Data";

static void draw_seglist(window& W, const list<SEGMENT>& seglist, bool colorful = true)
{
	color c_sav = W.get_fg_color();
    int j = 0;
    SEGMENT s;
    forall(s,seglist) { 
		color c = colorful ? color(1 + (j++ % 15)) : color(black);
		W.set_fg_color(c);
		W << s;
	}
	W.set_fg_color(c_sav);
}

#ifdef LEDA_CURVE_SWEEP_RAT_SEGMENTS
void gen_random_segments(list<SEGMENT>& seglist, int N, int grid_size)
{ seglist.clear();

  int gmin = -grid_size;
  int gmax =  grid_size;

  window& W = *win_ptr;
  double d = W.get_grid_dist();

  for(int i = 0; i < N; i++)
  { double x0 = rand_int(gmin,gmax)*d;
    double y0 = rand_int(gmin,gmax)*d;
    double x1 = rand_int(gmin,gmax)*d;
    double y1 = rand_int(gmin,gmax)*d;
    seglist.append(SEGMENT(int(x0),int(y0),int(x1),int(y1)));
    W.del_messages();
    W.message(string("%4d",i));
  }
}

void gen_ngon(list<SEGMENT>& seglist, int N)
{
  seglist.clear();

  double radius = 800;
  circle C(point(radius,0), point(0,radius), point(-radius,0));
  polygon poly1 = n_gon(N, C, radius/1024);
  polygon poly2 = poly1.rotate(LEDA_PI/(N));

  segment s;
  forall_segments(s, poly1) seglist.append(rat_segment(s));
  forall_segments(s, poly2) seglist.append(rat_segment(s));
};

#else

void gen_random_segments(list<SEGMENT>& seglist, int N, int grid_size)
{
  seglist.clear();

  int gmin = -grid_size;
  int gmax =  grid_size;

  window& W = *win_ptr;
  double d = W.get_grid_dist();

  int N_straight = int( double(percent_straight) * N / 100 );
  int N_arc      = N - N_straight;

  int i;
  for(i = 0; i < N_arc; i++)
  { double x0 = rand_int(gmin,gmax)*d;
    double y0 = rand_int(gmin,gmax)*d;
    double x1 = rand_int(gmin,gmax)*d;
    double y1 = rand_int(gmin,gmax)*d;
    double x2 = rand_int(gmin,gmax)*d;
    double y2 = rand_int(gmin,gmax)*d;
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

    W.del_messages();
    W.message(string("%4d",i));
  }

  for(i = 0; i < N_straight; i++)
  { double x0 = rand_int(gmin,gmax)*d;
    double y0 = rand_int(gmin,gmax)*d;
    double x1 = rand_int(gmin,gmax)*d;
    double y1 = rand_int(gmin,gmax)*d;
    seglist.append( SEGMENT(rat_point(int(x0),int(y0)),rat_point(int(x1),int(y1))) );
    W.del_messages();
    W.message(string("%4d",i));
  }
}

void add_curved_polygon(const polygon& poly, list<SEGMENT>& seglist)
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

void gen_ngon(list<SEGMENT>& seglist, int N)
{
  seglist.clear();

  double radius = 800;
  circle C(point(radius,0), point(0,radius), point(-radius,0));
  polygon poly = n_gon(N, C, radius/1024);

  add_curved_polygon(poly, seglist);
  add_curved_polygon(poly.rotate(LEDA_PI/(2*N)), seglist);
};

bool convert(const list<r_circle_segment>& seglist, list<rat_segment>& rat_segs)
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

#endif


static void run_alg(window& W, string alg, const list<SEGMENT>& seglist, 
                    GRAPH<POINT,SEGMENT>& G)
{
#ifdef LEDA_CURVE_SWEEP_COMPACTIFICATION_VISUAL_DEBUG
	TraceWinForCompatification = &W;
#endif

	W.clear();
	draw_seglist(W, seglist, false);

    G.clear();
    G.make_directed();

    W.message(alg);

	bool embed = alg.contains("embed");

	timer run_time;
	run_time.start();

	if (alg.contains("curve sweep")) {
	  bool cseg_sav = colorful_seglist;
	  colorful_seglist = false;
	  WIN_SWEEP_SEGMENTS(seglist,G,embed,&W);
	  colorful_seglist = cseg_sav;
	}
 
#ifdef LEDA_CURVE_SWEEP_RAT_SEGMENTS
    if (alg.index("sweep") == 0)
      SWEEP_SEGMENTS(seglist,G,embed);
#else
	if (alg.index("sweep") == 0) {
	  list<rat_segment> rat_segs;
	  if (convert(seglist, rat_segs)) {
	    GRAPH<rat_point,rat_segment> G_rat;
        run_time.reset(); run_time.start();
#ifdef USE_OLD_IMPL_FOR_STRAIGHT_SEGMENTS
		SWEEP_SEGMENTS(rat_segs,G_rat,embed);
#else
		CURVE_SWEEP_SEGMENTS(rat_segs,G_rat,embed);
#endif
        run_time.stop();
		convert(G_rat, G);
	  }
	  else error_handler(0, "sweep is only applicable if all segments are straight");
	}
#endif

	if (alg.contains("trivial")) {
	  INTERSECT_SEGMENTS_BRUTE_FORCE(seglist,G);
	}

    run_time.stop();
	float t = run_time.elapsed_time();

	int n = G.number_of_nodes();
    int m = G.number_of_edges();

    W.del_messages();
    W.message(string("%s |V| = %d |E| = %d  t = %6.2f sec", alg,n,m,t));
  
	cout << string("%s |S| = %d  |V| = %d |E| = %d  t = %6.2f sec", alg,seglist.size(),n,m,t) 
		 << endl << endl;

    node v;
    forall_nodes(v,G) 
       W.draw_filled_node(G[v].to_point(),red);
}
  
static void write_file(string fn)
{ file_ostream out(fn);
  seglist.print(out);
  win_ptr->set_grid_mode(0);
 }

// randomly select some of the segments
/*
static void select(list<SEGMENT>& L)
{
  L.permute();
  int to_del = L.size() / 4;
  while (to_del-- > 0 && !L.empty()) L.pop();
  write_file("_save.dat");
}
*/

static void read_file(string fn)
{ file_istream in(fn);
  seglist.read(in);
//  select(seglist);
  win_ptr->set_grid_mode(0);
 }


static void file_handler(int what)
{
  file_panel FP(*win_ptr,fname,dname);

  switch (what) {
  case file_load: FP.set_load_handler(read_file);
                  break;
  case file_save: FP.set_save_handler(write_file);
                  break;
  case file_all:  FP.set_load_handler(read_file);
                  FP.set_save_handler(write_file);
                  break;
  }
  if (filter != "") FP.set_pattern(filter);
  FP.open();
}

struct test_info {
	int num_segs;
	int num_nodes;
	int num_edges;

	test_info(int s = 0, int n = 0, int m = 0) : num_segs(s), num_nodes(n), num_edges(m) {}
};

static void test_handler(int what)
{
	d_array<string, test_info> info_of_file;
	info_of_file["circ_segs_err.dat"] = test_info(411, 4195, 14374);
	info_of_file["circ_segs_err2.dat"] = test_info(114, 2371, 4862);
	info_of_file["circ_segs_err3.dat"] = test_info(114, 1121, 2380);
	info_of_file["circ_segs_err4.dat"] = test_info(33, 146, 238);
	info_of_file["circ_segs_err6.dat"] = test_info(234, 13419, 28702);
	info_of_file["circ_segs_err7.dat"] = test_info(234, 13419, 28702);
	info_of_file["circ_segs_p1.dat"] = test_info(30, 193, 439);
	info_of_file["circ_segs_p2.dat"] = test_info(98, 2501, 5347);
	info_of_file["circ_segs_p3.dat"] = test_info(51, 838, 1723);
	info_of_file["circ_segs_p4.dat"] = test_info(703, 47566, 100349);
	info_of_file["circ_segs_p5.dat"] = test_info(1000, 117877, 234886);
	info_of_file["circ_segs1.dat"] = test_info(15, 119, 219);
	info_of_file["circ_segs2.dat"] = test_info(15, 53, 106);
	info_of_file["circ_segs3.dat"] = test_info(9, 22, 44);
	info_of_file["circ_segs4.dat"] = test_info(4, 9, 9);
	info_of_file["circ_segs5.dat"] = test_info(4, 11, 13);
	info_of_file["circ_segs6.dat"] = test_info(51, 687, 1417);
	info_of_file["circ_segs7.dat"] = test_info(6, 21, 28);
	info_of_file["circ_segs8.dat"] = test_info(100, 2061, 3946);
	info_of_file["circ_segs9.dat"] = test_info(100, 2158, 4141);

	bool success = true;
	string file;
	forall_defined(file, info_of_file) {
		string path = test_file_dir + get_directory_delimiter() + file;
		if (! is_file(path)) continue;
		cout << "testing " << file << " ..." << endl;
		read_file(path);
		test_info& info = info_of_file[file];
		if (seglist.size() != info.num_segs) {
			cout << "ERROR: num_segs should be " << info.num_segs << endl << endl;
			success = false;
			continue;
		}

		// curve sweep
		run_alg(*win_ptr, "curve sweep", seglist, *gp);
		if (gp->number_of_nodes() != info.num_nodes) {
			cout << "ERROR: num_nodes should be " << info.num_nodes << endl << endl;
			success = false;
		}
		else if (gp->number_of_edges() != info.num_edges) {
			cout << "ERROR: num_edges should be " << info.num_edges << endl << endl;
			success = false;
		}

		// curve sweep (embed)
		run_alg(*win_ptr, "curve sweep (embed)", seglist, *gp);
		if (gp->number_of_nodes() != info.num_nodes) {
			cout << "ERROR: num_nodes should be " << info.num_nodes << endl << endl;
			success = false;
		}
		else if (gp->number_of_edges() != 2 * info.num_edges) {
			cout << "ERROR: num_edges should be " << info.num_edges << endl << endl;
			success = false;
		}

		// trivial
		run_alg(*win_ptr, "trivial", seglist, *gp);
		if (gp->number_of_nodes() != info.num_nodes) {
			cout << "ERROR: num_nodes should be " << info.num_nodes << endl << endl;
			success = false;
		}
	}

	if (success) {
		cout << "All tests passed successfully!" << endl << endl;
	}
}


static void redraw(window* wp)
{ window& W = *wp;
  GRAPH<POINT,SEGMENT>& G = *gp;
  W.start_buffering();
  W.clear();

  draw_seglist(W, seglist, colorful_seglist);

  node v;
  forall_nodes(v,G) 
     W.draw_filled_node(G[v].to_point(),red);
  W.flush_buffer();
  W.stop_buffering();
}


static void set_grid(int sz)
{ int grid_dist = int((win_ptr->xmax() - win_ptr->xmin())/(2*sz+2));
  win_ptr->set_grid_mode(grid_dist);
}

int main()
{ 
rat_point::use_filter = 0;
rat_segment::use_filter = 0;

  int N = 50;
  int grid_size = 3;
  int h = int(0.69 * window::screen_height());
  int w = int(0.82 * h);

  menu file_menu;
  file_menu.button("Load File",file_load,file_handler);
  file_menu.button("Save File",file_save,file_handler);
#ifndef LEDA_CURVE_SWEEP_RAT_SEGMENTS
  file_menu.button("Test Files",file_test,test_handler);
#endif
  //file_menu.button("Exit",file_exit);

#ifdef LEDA_CURVE_SWEEP_RAT_SEGMENTS
  window W(w,h,"Segment Intersection Algorithms");
#else
  window W(w,h,"Circle-Segment Intersection Algorithms");
#endif
  win_ptr = &W;

  list<string> alg_names;
  alg_names.append("curve sweep");
  alg_names.append("curve sweep (embed)");
  alg_names.append("sweep");
  alg_names.append("sweep (embed)");
  alg_names.append("trivial");

  string alg = alg_names.head();

  W.int_item("segments", N,0,1000);
  W.int_item("grid size",grid_size,1,32,set_grid);
#ifndef LEDA_CURVE_SWEEP_RAT_SEGMENTS
  W.int_item("% straight", percent_straight, 0, 100);
#endif
  W.string_item("algorithm",alg,alg_names,10);

  W.button("mouse", 1);
  W.button("rand",  2);
  W.button("n-gon", 5);
  W.button("file",  99,file_menu);
  W.button("run",   3);
  W.button("graph", 4);
  W.button("quit",  0);

  W.set_node_width(2);
  W.set_grid_style(line_grid);
  W.set_redraw(redraw);

  W.init(-1000,1000,-1000);
  W.display(window::center,window::center);
  GRAPH<POINT,SEGMENT> G;
  gp = &G;

  set_grid(grid_size);

  for(;;)
  {
    int but = W.read_mouse();

    if (but == 0) break;
  
    switch (but) {

    case 1: // mouse
         { seglist.clear();
		   W.set_show_coordinates(true);
           W.clear();
           SEGMENT s;
           while (W >> s)
           { W << s;
             seglist.append(s);
            }
     
		   W.set_show_coordinates(false);
           break;
          }

    case 2: // random
			  gen_random_segments(seglist, N, grid_size);
			  break;

    case 3: { // run algorithm
              run_alg(W,alg,seglist,G);
              break;
             }

    case 4: { // start GraphWin
              char* buf = W.get_window_pixrect();
              GraphWin gw(G,W);
              gw.set_node_label_type(no_label);
              gw.set_node_shape(circle_node);
              gw.set_node_width(8);
              gw.set_directed(true);
              node v;
              forall_nodes(v,G) gw.set_position(v,G[v].to_point());
			  edge e;
			  forall_edges(e,G) gw.set_user_label(e, string("%04x", ID_Number(G[e]) & 0xFFFF));
              gw.set_edge_label_type(no_label);
              W.disable_panel();
              gw.display();
              gw.edit();
              W.enable_panel();
              W.put_pixrect(buf);
              W.del_pixrect(buf);
              break;
             }

    case 5: // n-gon
			  gen_ngon(seglist, N);
			  break;

    }

    if (but == 1 || but == 2 || but == 5 || but == 99)
    { G.clear();
      W.clear();
	  draw_seglist(W, seglist);
    }

}

 seglist.clear();

 rat_point::print_statistics();

 return 0;

}
