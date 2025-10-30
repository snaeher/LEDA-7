/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  dxf_io_demo.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/dxf_io.h>
#include <LEDA/geo/r_circle_gen_polygon.h>
#include <LEDA/graphics/real_window.h>
#include <LEDA/graphics/file_panel.h>

#include <iostream>

using namespace leda;

using std::cout;
using std::cerr;
using std::istream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::endl;
using std::flush;


static window W(540, 580, "LEDA DXF-File I/O Demo");

static bool approximate = true;

static string file;
static string dir;

static dxf_io* dxf_io_ptr = nil;
static const r_circle_gen_polygon* gp_ptr = nil;

static void read_file(string filename)
{
	cout << "reading " << filename << " ... " << flush;
	if (dxf_io_ptr) dxf_io_ptr->read(filename);
	cout << "done" << endl;
}

static void read_dxf_file(dxf_io& dxf_reader)
{
	dxf_io_ptr = &dxf_reader;

	file_panel fp(W, file, dir);
	fp.set_pattern("dxf files (*.dxf)", "*.dxf");
	fp.set_load_handler(read_file);
	fp.open();

	dxf_io_ptr = nil;
}

static void write_file(string filename)
{
	cout << "writing " << filename << " ... " << flush;
	if (gp_ptr) {
		dxf_io dxf_writer;
		dxf_writer.write(filename, *gp_ptr);
	}
	cout << "done" << endl;
}

static void write_dxf_file(const r_circle_gen_polygon& gp)
{
	gp_ptr = &gp;

	file_panel fp(W, file, dir);
	fp.set_pattern("dxf files (*.dxf)", "*.dxf");
	fp.set_save_handler(write_file);
	fp.open();

	gp_ptr = nil;
}

static void show(dxf_io& dxf_reader, window& W)
{
	W.clear();

	cout << "computing bounding box ... " << flush;
	real _xmin, _ymin, _xmax, _ymax;
	dxf_reader.compute_bounding_box(_xmin, _ymin, _xmax, _ymax);
	double xmin = _xmin.to_close_double(), ymin = _ymin.to_close_double();
	double xmax = _xmax.to_close_double(), ymax = _ymax.to_close_double();
	cout << "done" << endl;

	rectangle bbox(xmin, ymin, xmax, ymax);
	cout << "bbox = " << bbox << endl;

	if (xmin == xmax || ymin == ymax) return;

	W.init(xmin, xmax, ymin, ymax);
	W.set_show_coordinates(true);

	list<r_circle_polygon> curved_polys = dxf_reader.get_curved_polygons();
	cout << "# curved polygons (green) = " << curved_polys.size() << endl;
	r_circle_polygon curved_poly;
	forall(curved_poly, curved_polys) {
		draw(W, curved_poly, green);
	}

	list<rat_circle> circles = dxf_reader.get_circles();
	cout << "# circles (black) = " << circles.size() << endl;
	rat_circle circ;
	forall(circ, circles) {
		W.draw_circle(circ.to_float(), black);
	}

	list<rat_polygon> straight_polys = dxf_reader.get_straight_polygons();
	cout << "# straight polygons (blue) = " << straight_polys.size() << endl;
	rat_polygon straight_poly;
	forall(straight_poly, straight_polys) {
		draw(W, straight_poly, blue);
	}

	list<r_circle_segment> curved_segs = dxf_reader.get_curved_segments();
	cout << "# curved segments (red) = " << curved_segs.size() << endl;
	r_circle_segment curved_seg;
	forall(curved_seg, curved_segs) {
		draw(W, curved_seg, red);
	}

	list<rat_segment> straight_segs = dxf_reader.get_straight_segments();
	cout << "# straight segments (yellow) = " << straight_segs.size() << endl;
	rat_segment straight_seg;
	forall(straight_seg, straight_segs) {
		draw(W, straight_seg, yellow);
	}

	cout << "Press button in window\n"; W.await_button();
}

static r_circle_gen_polygon unite(const list<r_circle_polygon>& polys, window& W)
{
	W.clear();

	list<r_circle_gen_polygon> gps;
	r_circle_polygon poly;
	forall(poly, polys) {
		gps.append( r_circle_gen_polygon(poly) );
		draw(W, poly, black);
	}
	
	cout << "computing union";
	if (approximate) cout << " (approximate)";
	cout << " ... " << flush;
	r_circle_gen_polygon gp = approximate ? r_circle_gen_polygon::unite_approximate(gps)
	                                      : r_circle_gen_polygon::unite(gps);
	cout << endl;

	draw(W, gp, red);
	cout << "Press button in window\n"; W.await_button();

	return gp;
}

/// main ////////////////////////////////////////////////////////////////////////////////////

int main()
{
	W.display(window::center, window::center);

	dxf_io dxf_reader;

	read_dxf_file(dxf_reader);

	show(dxf_reader, W);

	dxf_reader.convert_circles_to_curved_polygons();
	dxf_reader.convert_straight_entities_to_curved_entities();

	list<r_circle_polygon> polys = dxf_reader.get_curved_polygons();

	r_circle_gen_polygon gp = unite(polys, W);

	write_dxf_file(gp);

	return 0;
}

