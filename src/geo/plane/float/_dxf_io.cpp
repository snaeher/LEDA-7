/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _dxf_io.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/dxf_io.h>

#include <LEDA/system/file.h>
#include <LEDA/system/error.h>
#include <LEDA/core/tuple.h>
#include <LEDA/core/dictionary.h>

#include <stdlib.h>

LEDA_BEGIN_NAMESPACE

enum { 
	GR_SEP = 0, // seperates important parts of the file
	GR_SECTION = 2,
	GR_LAYER = 8,
	GR_POLYLINE_FLAGS = 70, GR_POLYLINE_VERTICES_FOLLOW = 66, 
	GR_X = 10, GR_Y = 20, GR_Z = 30, GR_VERT_ARC = 42, 
	GR_X0 = GR_X, GR_Y0 = GR_Y, GR_Z0 = GR_Z, GR_X1 = 11, GR_Y1 = 21, GR_Z1 = 31,
	GR_RADIUS = 40, GR_START_AGL = 50, GR_END_AGL = 51, 
	GR_NUMBER_VERTICES = 90,
};

struct dxf_vertex {
	double x, y, z;
	double arc_parameter;

	dxf_vertex() : x(0), y(0), z(0), arc_parameter(0) {}
};

int compare(const dxf_vertex& v1, dxf_vertex& v2)
{
	int cmp = compare(v1.x, v2.x);
	if (cmp) return cmp;
	return compare(v1.y, v2.y);
}

/// dxf_io ///////////////////////////////////////////////////////////////////////////////////////

/// global public functions

void 
dxf_io::clear()
{
	delete Input;  Input = 0;
	delete Output; Output = 0;

	CurvedPolygons.clear(); StraightPolygons.clear(); 
	Circles.clear();
	CurvedSegments.clear(); StraightSegments.clear();

	clear_error();
	clear_statistics();
}

void 
dxf_io::set_distance_for_arc_approximation(double d)
{
	const double eps = 1e-5;
	DistanceForArcApprox = d > eps ? d : eps;
}

/// global private functions

void 
dxf_io::clear_error()
{
	Error = false;   ErrorMsg = "";
	Warning = false; WarningMsg = "";
}

void 
dxf_io::error(const string& msg)
{
	if (! Error) {
		Error = true; ErrorMsg = msg;
	}
}

void 
dxf_io::warn(const string& msg)
{
	if (! Warning) {
		Warning = true; WarningMsg = msg;
	}
}

void 
dxf_io::clear_statistics()
{
	NumStraightSegments = NumArcSegments = NumCircles = NumApproxSegments = 0;
}

void 
dxf_io::get_statistics(unsigned& num_straight_segs, 
					   unsigned& num_arc_segs, 
					   unsigned& num_circles,
					   unsigned& num_approx_segs) const
{
	num_straight_segs = NumStraightSegments;
	num_arc_segs = NumArcSegments;
	num_circles = NumCircles;
	num_approx_segs = NumApproxSegments;
}

void 
dxf_io::print_statistics(ostream& out) const
{
	if (has_error()) {
		out << "Error: " << get_error_message() << endl;
	}
	else if (has_warning()) {
		out << "Warning: " << get_warning_message() << endl;
	}

	if (NumStraightSegments > 0)
		out << "NumStraigtSegments = " << NumStraightSegments << endl;
	if (NumArcSegments > 0)
		out << "NumArcSegments = " << NumArcSegments << endl;
	if (NumCircles > 0)
		out << "NumCircles = " << NumCircles << endl;
	if (NumApproxSegments > 0)
		out << "NumApproxSegments = " << NumApproxSegments << endl;
}

void 
dxf_io::compute_bounding_box(real& xmin, real& ymin, real& xmax, real& ymax) const
{
	bool has_initial_box = false;

	real _xmin, _ymin, _xmax, _ymax;
	rat_point xmin_pnt, ymin_pnt, xmax_pnt, ymax_pnt;

	if (!has_initial_box && !CurvedPolygons.empty()) {
		CurvedPolygons.head().bounding_box(xmin, ymin, xmax, ymax);
		has_initial_box = true;
	}
	r_circle_polygon curved_poly;
	forall(curved_poly, CurvedPolygons) {
		curved_poly.bounding_box(_xmin, _ymin, _xmax, _ymax);
		if (_xmin < xmin) xmin = _xmin;
		if (_ymin < ymin) ymin = _ymin;
		if (_xmax > xmax) xmax = _xmax;
		if (_ymax > ymax) ymax = _ymax;
	}

	if (!has_initial_box && !StraightPolygons.empty()) {
		StraightPolygons.head().bounding_box(xmin_pnt, ymin_pnt, xmax_pnt, ymax_pnt);
		xmin = xmin_pnt.xcoord(); ymin = ymin_pnt.ycoord();
		xmax = xmax_pnt.xcoord(); ymax = ymax_pnt.ycoord();
		has_initial_box = true;
	}
	rat_polygon straight_poly;
	forall(straight_poly, StraightPolygons) {
		straight_poly.bounding_box(xmin_pnt, ymin_pnt, xmax_pnt, ymax_pnt);
		if (xmin_pnt.xcoord() < xmin) xmin = xmin_pnt.xcoord();
		if (ymin_pnt.ycoord() < ymin) ymin = ymin_pnt.ycoord();
		if (xmax_pnt.xcoord() > xmax) xmax = xmax_pnt.xcoord();
		if (ymax_pnt.ycoord() > ymax) ymax = ymax_pnt.ycoord();
	}

	if (!has_initial_box && !Circles.empty()) {
		real_circle c = real_circle(Circles.head());
		real_point cen = c.center(); real rad = c.radius();
		xmin = cen.xcoord() - rad; ymin = cen.ycoord() - rad;
		xmax = cen.xcoord() + rad; ymax = cen.ycoord() + rad;
	}
	rat_circle circ;
	forall(circ, Circles) {
		real_circle c = real_circle(circ);
		real_point cen = c.center(); real rad = c.radius();
		_xmin = cen.xcoord() - rad; _ymin = cen.ycoord() - rad;
		_xmax = cen.xcoord() + rad; _ymax = cen.ycoord() + rad;
		if (_xmin < xmin) xmin = _xmin;
		if (_ymin < ymin) ymin = _ymin;
		if (_xmax > xmax) xmax = _xmax;
		if (_ymax > ymax) ymax = _ymax;
	}

	if (!has_initial_box && !CurvedSegments.empty()) {
		CurvedSegments.head().compute_bounding_box(xmin, ymin, xmax, ymax);
		has_initial_box = true;
	}
	r_circle_segment curved_seg;
	forall(curved_seg, CurvedSegments) {
		curved_seg.compute_bounding_box(_xmin, _ymin, _xmax, _ymax);
		if (_xmin < xmin) xmin = _xmin;
		if (_ymin < ymin) ymin = _ymin;
		if (_xmax > xmax) xmax = _xmax;
		if (_ymax > ymax) ymax = _ymax;
	}

	if (!has_initial_box && !StraightSegments.empty()) {
		rat_segment straight_seg = StraightSegments.head();
		xmin = straight_seg.source().xcoord(); ymin = straight_seg.source().ycoord();
		xmax = straight_seg.target().xcoord(); ymax = straight_seg.target().ycoord();
		if (xmax < xmin) leda_swap(xmin, xmax);
		if (ymax < ymin) leda_swap(ymin, ymax);
	}
	rat_segment straight_seg;
	forall(straight_seg, StraightSegments) {
		_xmin = straight_seg.source().xcoord(); _ymin = straight_seg.source().ycoord();
		_xmax = straight_seg.target().xcoord(); _ymax = straight_seg.target().ycoord();
		if (_xmax < _xmin) leda_swap(_xmin, _xmax);
		if (_ymax < _ymin) leda_swap(_ymin, _ymax);

		if (_xmin < xmin) xmin = _xmin;
		if (_ymin < ymin) ymin = _ymin;
		if (_xmax > xmax) xmax = _xmax;
		if (_ymax > ymax) ymax = _ymax;
	}
}

/// input functions (private)

bool 
dxf_io::input_init(const string& filename)
{
	clear();

	if (! is_file(filename)) {
		Input = 0;
		error("dxf_io::input_init: cannot open file " + filename);
		return false;
	}

	Input = new ifstream(filename);

	// skip till beginning of entities
	bool found = false;
	while (has_input()) {
		if (!input_pair()) break;
		if (Code != GR_SEP || Value != "SECTION") continue;
		if (!input_pair()) break;
		if (Code == GR_SECTION && Value == "ENTITIES") { found = true; break; }
	}
	if (!found || Error) {
		error("dxf_io::input_init: could not find ENTITIES");
		return false;
	}

	return true;
}

bool 
dxf_io::input_pair()
{
	string code_str;
	code_str.read_line(*Input); code_str = code_str.trim();
	Code = atoi(code_str);
	Value.read_line(*Input); Value = Value.trim();
	
	if (Input->bad()) error("dxf_io::input_pair: bad input");
	return !Error;
}

bool 
dxf_io::input_polyline()
{
	typedef two_tuple<dxf_vertex,r_circle_point> vertex_tuple;

	// parse header
	//bool is_open = true;
	do {
		if (!input_pair()) break;
		if (Code == GR_POLYLINE_FLAGS) {
			//int flags = atoi(Value);
			//if ((flags & 1) != 0) is_open = false;
		}
	} while (Code != GR_SEP);
	if (Error) return false;

	// parse vertex list
	list<vertex_tuple> vertices;
	bool is_curved = false;
	do {
		if (Code != GR_SEP || Value != "VERTEX") { 
			error("dxf_io::input_polyline: expected vertex"); break;
		}

		//cout << "."; cout.flush();

		// parse vertex
		dxf_vertex v;
		do {
			if (!input_pair()) break;
			switch (Code) {
			case GR_X:			v.x = atof(Value); break;
			case GR_Y:			v.y = atof(Value); break;
			case GR_Z:			v.z = atof(Value); break;
			case GR_VERT_ARC:	v.arc_parameter = atof(Value); 
								is_curved |= (v.arc_parameter != 0.0);
								break;
			}
		} while (Code != GR_SEP);
		r_circle_point v_rcp( point(v.x, v.y) );
		vertices.append( vertex_tuple(v, v_rcp) );

	} while (!Error && !(Code == GR_SEP && Value == "SEQEND"));

	if (vertices.size() < 2) { 
		error("dxf_io::input_polyline: not enough vertices"); return false;
	}

	list<r_circle_segment> segments;
	list<rat_point> rat_vertices;

	list_item it;
	forall_items(it, vertices) {
		const vertex_tuple&   src = vertices[it];
		const r_circle_point& s   = src.second();

		const vertex_tuple&   tgt = vertices[vertices.cyclic_succ(it)];
		const r_circle_point& t   = tgt.second();

		if (s == t) continue; // trivial segment

		double arc_parameter = src.first().arc_parameter;
		rat_point p = s.to_rat_point(), q = t.to_rat_point();

		if (arc_parameter == 0) {
			// regular vertex
			++NumStraightSegments;
			segments.append( r_circle_segment(s,t) );
			rat_vertices.append( p );
		}
		else {
			// arc vertex
			++NumArcSegments;
			rat_point m = midpoint(p, q);
			rat_vector vec = (p-q).rotate90();
			vec *= rational(arc_parameter / 2); // Note: division by 2 is exact!
			rat_point r = m + vec;
			r_circle_segment s(p, r, q);

			segments.append( s );
			if (InputModePolygons == IM_only_straight) {
				list<rat_point> approx_pnts = s.approximate_by_rat_points(DistanceForArcApprox);
				approx_pnts.pop_back(); // remove target from list
				NumApproxSegments += approx_pnts.size();
				rat_vertices.conc(approx_pnts);
			}
		}
	}

	if (InputModePolygons == IM_only_curved || (InputModePolygons == IM_mixed && is_curved)) {
		r_circle_polygon::RESPECT_TYPE respect = RespectOrientation ? r_circle_polygon::RESPECT_ORIENTATION : r_circle_polygon::DISREGARD_ORIENTATION;
		r_circle_polygon poly(segments, r_circle_polygon::NO_CHECK, respect);

		if (Verify) poly.verify();

		if (!Verify || poly.is_weakly_simple())
			CurvedPolygons.append(poly);
	}
	else {
		if (is_curved) warn("dxf_io::input_polyline: arc segment in straight input");
		rat_polygon::RESPECT_TYPE respect = RespectOrientation ? rat_polygon::RESPECT_ORIENTATION : rat_polygon::DISREGARD_ORIENTATION;
		rat_polygon poly(rat_vertices, rat_polygon::NO_CHECK, respect);

		if (!Verify || poly.is_weakly_simple())
			StraightPolygons.append(poly);
	}

	return !Error;
}

bool 
dxf_io::input_lwpolyline()
{
	typedef two_tuple<dxf_vertex,r_circle_point> vertex_tuple;

	int vertex_count = 0;

	// parse header
	//bool is_open = true;
	do {
		if (!input_pair()) break;

		if (Code == GR_POLYLINE_FLAGS) {
			//int flags = atoi(Value);
			//if ((flags & 1) != 0) is_open = false;
		}

		if (Code == GR_NUMBER_VERTICES) {
			vertex_count = atoi(Value);
		}
	} while (Code != GR_X);
	if (Error) return false;


	// parse vertex list
	list<vertex_tuple> vertices;
	bool is_curved = false;
	while (vertex_count-- > 0) {
		dxf_vertex v;

		v.x = atof(Value);

		do {
			if (!input_pair()) break;

			switch(Code) {
				case GR_Y:			v.y = atof(Value); break;
				case GR_Z:			v.z = atof(Value); break;
				case GR_VERT_ARC:	v.arc_parameter = atof(Value); 
									is_curved |= (v.arc_parameter != 0.0);
									break;
			}

		} while (Code != GR_X && Code != GR_SEP);
		

		if (   (Code != GR_X   && vertex_count > 0) 
		    || (Code != GR_SEP && vertex_count == 0) )
		{
			error("dxf_io::input_lwpolyline: unexpected code"); break;
		}

		r_circle_point v_rcp( point(v.x, v.y) );
		vertices.append( vertex_tuple(v, v_rcp) );
	}
	if (Error) return false;

	// the remainder is copied from input_polyline ...
	if (vertices.size() < 2) { 
		error("dxf_io::input_lwpolyline: not enough vertices"); return false;
	}

	list<r_circle_segment> segments;
	list<rat_point> rat_vertices;

	list_item it;
	forall_items(it, vertices) {
		const vertex_tuple&   src = vertices[it];
		const r_circle_point& s   = src.second();

		const vertex_tuple&   tgt = vertices[vertices.cyclic_succ(it)];
		const r_circle_point& t   = tgt.second();

		if (s == t) continue; // trivial segment

		double arc_parameter = src.first().arc_parameter;
		rat_point p = s.to_rat_point(), q = t.to_rat_point();

		if (arc_parameter == 0) {
			// regular vertex
			++NumStraightSegments;
			segments.append( r_circle_segment(s,t) );
			rat_vertices.append( p );
		}
		else {
			// arc vertex
			++NumArcSegments;
			rat_point m = midpoint(p, q);
			rat_vector vec = (p-q).rotate90();
			vec *= rational(arc_parameter / 2); // Note: division by 2 is exact!
			rat_point r = m + vec;
			r_circle_segment s(p, r, q);

			segments.append( s );
			if (InputModePolygons == IM_only_straight) {
				list<rat_point> approx_pnts = s.approximate_by_rat_points(DistanceForArcApprox);
				approx_pnts.pop_back(); // remove target from list
				NumApproxSegments += approx_pnts.size();
				rat_vertices.conc(approx_pnts);
			}
		}
	}

	if (InputModePolygons == IM_only_curved || (InputModePolygons == IM_mixed && is_curved)) {
		r_circle_polygon::RESPECT_TYPE respect = RespectOrientation ? r_circle_polygon::RESPECT_ORIENTATION : r_circle_polygon::DISREGARD_ORIENTATION;
		r_circle_polygon poly(segments, r_circle_polygon::NO_CHECK, respect);

		if (Verify) poly.verify();

		if (!Verify || poly.is_weakly_simple())
			CurvedPolygons.append(poly);
	}
	else {
		if (is_curved) warn("dxf_io::input_lwpolyline: arc segment in straight input");
		rat_polygon::RESPECT_TYPE respect = RespectOrientation ? rat_polygon::RESPECT_ORIENTATION : rat_polygon::DISREGARD_ORIENTATION;
		rat_polygon poly(rat_vertices, rat_polygon::NO_CHECK, respect);

		if (!Verify || poly.is_weakly_simple())
			StraightPolygons.append(poly);
	}

	return !Error;
}

bool 
dxf_io::input_line()
{
	++NumStraightSegments;
	double x0 = 0, y0 = 0, x1 = 0, y1 = 0;
        //double z0 = 0, z1 = 0;
	do {
		if (!input_pair()) break;
		switch (Code) {
		case GR_X0:		x0 = atof(Value); break;
		case GR_Y0:		y0 = atof(Value); break;
		case GR_Z0:		/*z0 = atof(Value);*/ break;
		case GR_X1:		x1 = atof(Value); break;
		case GR_Y1:		y1 = atof(Value); break;
		case GR_Z1:		/*z1 = atof(Value);*/ break;
		}
	} while (Code != GR_SEP);
	rat_segment s(segment(x0, y0, x1, y1));

	if (InputModeSegments != IM_only_curved) {
		StraightSegments.append(s);
	}
	else {
		CurvedSegments.append(r_circle_segment(s));
	}

	return !Error;
}

bool 
dxf_io::input_circle()
{
	++NumCircles;
	double x = 0, y = 0, radius = 0;
        //double z = 0;
	do {
		if (!input_pair()) break;
		switch (Code) {
		case GR_X:		x = atof(Value); break;
		case GR_Y:		y = atof(Value); break;
		case GR_Z:		/*z = atof(Value);*/ break;
		case GR_RADIUS:	radius = atof(Value); break;
		}
	} while (Code != GR_SEP);
	rat_circle c( circle(x, y, radius) );

	if (InputModePolygons != IM_only_straight) {
		Circles.append(c);
	}
	else {
		warn("dxf_io::input_circle: circle in straight input");
		r_circle_segment s(c);
		list<rat_point> rat_vertices = s.approximate_by_rat_points(DistanceForArcApprox);
		rat_vertices.pop_back(); // remove last point which is identical to first one
		rat_polygon poly(rat_vertices, rat_polygon::NO_CHECK, rat_polygon::DISREGARD_ORIENTATION);
		StraightPolygons.append(poly);
		NumApproxSegments += rat_vertices.size();
	}

	return !Error;
}

rat_point 
dxf_io::point_near_circle(const circle& c, double angle_in_deg, bool& on_circle)
{
	int mult_of_90 = int( angle_in_deg / 90 );
	if (angle_in_deg == double(mult_of_90 * 90)) {
		on_circle = true;
		point p(c.radius(), 0); p = p.rotate90(mult_of_90);
		rational x = rational(c.center().xcoord()) + rational(p.xcoord());
		rational y = rational(c.center().ycoord()) + rational(p.ycoord());
		return rat_point(x, y);
	}
	else {
		on_circle = false;
		angle_in_deg *= LEDA_PI / 180;
		rat_circle circ(c);
		return circ.point_on_circle(angle_in_deg, 1e-5);
	}
}

bool 
dxf_io::input_arc()
{
	++NumArcSegments;
	double x = 0, y = 0, radius = 0, start = 0, end = 0;
        //double z = 0;
	do {
		if (!input_pair()) break;
		switch (Code) {
		case GR_X:			x = atof(Value); break;
		case GR_Y:			y = atof(Value); break;
		case GR_Z:			/*z = atof(Value);*/ break;
		case GR_RADIUS:		radius = atof(Value); break;
		case GR_START_AGL:	start = atof(Value); break;
		case GR_END_AGL:	end = atof(Value); break;
		}
	} while (Code != GR_SEP);

	// create a positively oriented circle
	circle c(x, y, radius);

	bool src_on_c = false, tgt_on_c = false;
	rat_point src = point_near_circle(c, start, src_on_c);
	rat_point tgt = point_near_circle(c, end,   tgt_on_c);

	r_circle_segment s;
	if (src_on_c && tgt_on_c) {
		s = r_circle_segment(src, tgt, rat_circle(c));
	}
	else {
		// we make sure that the endpoints are rat_points, which may change the circle!
		bool dummy = false;
		if (end < start) end += 360;
		rat_point mid = point_near_circle(c, (start+end)/2, dummy);
		s = r_circle_segment(src, mid, tgt);
	}

	if (InputModeSegments == IM_only_straight) {
		list<rat_segment> approx_segs = s.approximate_by_rat_segments(DistanceForArcApprox);
		NumApproxSegments += approx_segs.size();
		StraightSegments.conc(approx_segs);
	}
	else {
		CurvedSegments.append(s);
	}

	return !Error;
}

/// read functions (public)

void 
dxf_io::read(const string& filename, input_mode mode_polys, input_mode mode_segs)
{
	InputModePolygons = mode_polys; InputModeSegments = mode_segs;

	// read till beginning of ENTITIES
	if (! input_init(filename)) return;

	bool fetch_next_pair = true;

	while (has_input()) {
		if (fetch_next_pair && !input_pair()) break;
		fetch_next_pair = true;

		if (Code == GR_SEP && Value == "ENDSEC") break;
		if (Code == GR_SEP && Value == "EOF") { 
			error("dxf_io::input_read: unexpected EOF"); break;
		}

		if (Code == GR_SEP && Value == "POLYLINE") {
			if (! input_polyline()) break;
		}

		if (Code == GR_SEP && Value == "LWPOLYLINE") {
			if (! input_lwpolyline()) break;
		}

		if (Code == GR_SEP && Value == "LINE") {
			if (! input_line()) break;
			fetch_next_pair = false;
		}

		if (Code == GR_SEP && Value == "CIRCLE") {
			if (! input_circle()) break;
			fetch_next_pair = false;
		}

		if (Code == GR_SEP && Value == "ARC") {
			if (! input_arc()) break;
			fetch_next_pair = false;
		}
	}

	if (Error) {
		error("dxf_io::input_read: error parsing entities");
	}
}

void 
dxf_io::read(const string& filename, list<r_circle_polygon>& L)
{
	read(filename, IM_only_curved);
	convert_circles_to_curved_polygons();
	list<r_circle_polygon> L1 = CurvedPolygons;
	L.conc(L1);
}

void 
dxf_io::read(const string& filename, list<rat_polygon>& L)
{
	read(filename, IM_only_straight);
	list<rat_polygon> L1 = StraightPolygons;
	L.conc(L1);
}


/// conversion functions (public)

void 
dxf_io::convert_straight_entities_to_curved_entities()
{
	convert_polygons_from_straight_to_curved(StraightPolygons, CurvedPolygons);
	StraightPolygons.clear();

	convert_segments_from_straight_to_curved(StraightSegments, CurvedSegments);
	StraightSegments.clear();
}

void 
dxf_io::convert_curved_entities_to_straight_entities()
{
	convert_circles_to_curved_polygons();

	NumApproxSegments += 
		convert_polygons_from_curved_to_straight(CurvedPolygons, StraightPolygons, DistanceForArcApprox);
	CurvedPolygons.clear();

	NumApproxSegments +=
		convert_segments_from_curved_to_straight(CurvedSegments, StraightSegments, DistanceForArcApprox);
	CurvedSegments.clear();
}

void 
dxf_io::convert_circles_to_curved_polygons()
{
	rat_circle c;
	forall(c, Circles)
		CurvedPolygons.append( r_circle_polygon( c, r_circle_polygon::DISREGARD_ORIENTATION ) );

	Circles.clear();
}

void 
dxf_io::convert_all_segments_to_polygons(real arc_eps)
{
	convert_curved_segments_to_polygons(arc_eps);
	convert_straight_segments_to_polygons();
}

void 
dxf_io::convert_curved_segments_to_polygons(real arc_eps)
{
	typedef r_circle_point   point_type;
	typedef r_circle_segment segment_type;
	typedef r_circle_polygon polygon_type;

	dictionary<point_type, list_item> sources;

	// correct sources and targets of arcs
	list_item it;
	if (arc_eps > 0)
	{  forall_items(it, CurvedSegments) 
           { segment_type seg = CurvedSegments[it];
	     if (!seg.is_proper_arc()) continue;

			point_type seg_src = seg.source(), seg_tgt = seg.target();;

			bool change = false;

			list_item pred_it = CurvedSegments.cyclic_pred(it);
			point_type pred_tgt = CurvedSegments[pred_it].target();
			if ( pred_tgt.is_rat_point() 
				 && seg_src != pred_tgt && seg_src.distance(pred_tgt) < arc_eps )
			{
				change = true; seg_src = pred_tgt;
			}

			list_item succ_it = CurvedSegments.cyclic_succ(it);
			point_type succ_src = CurvedSegments[succ_it].source();
			if ( seg_tgt.is_rat_point()
				 && seg_tgt != succ_src && seg_tgt.distance(succ_src) < arc_eps )
			{
				change = true; seg_tgt = succ_src;
			}

			if ( change && seg_src.is_rat_point() && seg_tgt.is_rat_point() ) {
				rat_point p = seg_src.to_rat_point(), q = seg_tgt.to_rat_point();
				CurvedSegments[it] = seg.trim_approximate(p, q);
			}
		}
        }

	list_item pred_it = nil, chain_start = nil;
	forall_items(it, CurvedSegments) {
		segment_type seg = CurvedSegments[it];

		if (pred_it && CurvedSegments[pred_it].target() != seg.source()) {
			// start new chain
			chain_start = pred_it = nil;
			sources.clear();
		}

		list_item sub_chain_start = nil;
		dic_item dit = sources.lookup(seg.target());
		if (dit)
		{
			sub_chain_start = sources.inf(dit);
		}
		else if ( seg.is_proper_arc() && arc_eps > 0 && chain_start 
				  && seg.source().is_rat_point() )
		{
			point_type seg_src = seg.source(), seg_tgt = seg.target();
			for(;;) {
				point_type pred_src = CurvedSegments[pred_it].source();

				if ( pred_src.is_rat_point()
					 && seg_tgt != pred_src && seg_tgt.distance(pred_src) < arc_eps ) 
				{
					// we change the tgt of seg so that it becomes equal to the source of pred
					seg_tgt = pred_src;
					rat_point p = seg_src.to_rat_point(), q = seg_tgt.to_rat_point();
					CurvedSegments[it] = seg.trim_approximate(p, q);

					sub_chain_start = pred_it; break;
				}

				if (pred_it == chain_start) break;
				
				pred_it = CurvedSegments.pred(pred_it);
			}
		}

		if (sub_chain_start) {
			pred_it = CurvedSegments.pred(sub_chain_start);

			list<segment_type> sub_chain;
			CurvedSegments.extract(sub_chain_start, it, sub_chain);

			polygon_type::RESPECT_TYPE respect = RespectOrientation ? polygon_type::RESPECT_ORIENTATION : polygon_type::DISREGARD_ORIENTATION;
			polygon_type poly(sub_chain, polygon_type::NO_CHECK, respect);
			CurvedPolygons.append(poly);

			if (chain_start == sub_chain_start) chain_start = pred_it = nil;
			segment_type s; forall(s, sub_chain) sources.del(s.source());
		}
		else {
			if (!chain_start) chain_start = it;
			pred_it = it; 
			sources.insert(seg.source(), it);
		}
	}
}

void 
dxf_io::convert_straight_segments_to_polygons()
{
	typedef rat_point   point_type;
	typedef rat_segment segment_type;
	typedef rat_polygon polygon_type;

	dictionary<point_type, list_item> sources;

	list_item pred_it = nil;
	list_item it;
	forall_items(it, StraightSegments) {
		segment_type seg = StraightSegments[it];
		if (pred_it && StraightSegments[pred_it].target() != seg.source()) {
			pred_it = nil; sources.clear(); // start new chain
		}
		dic_item dit = sources.lookup(seg.target());
		if (dit) {
			list<segment_type> sub_chain;
			StraightSegments.extract(sources.inf(dit), it, sub_chain);
			list<point_type> vertices;
			segment_type s; 
			forall(s, sub_chain) {
				vertices.append(s.source());
				sources.del(s.source());
			}

			polygon_type::RESPECT_TYPE respect = RespectOrientation ? polygon_type::RESPECT_ORIENTATION : polygon_type::DISREGARD_ORIENTATION;
			polygon_type poly(vertices, polygon_type::NO_CHECK, respect);
			StraightPolygons.append(poly);

			pred_it = nil;
		}
		else {
			pred_it = it; sources.insert(seg.source(), it);
		}
	}
}

void 
dxf_io::convert_polygons_from_straight_to_curved(const list<rat_polygon>& straight, 
												 list<r_circle_polygon>& curved)
{
	rat_polygon straight_poly;
	forall(straight_poly, straight)
		curved.append( r_circle_polygon(straight_poly) );
}

void 
dxf_io::convert_polygons_from_straight_to_curved(const list<rat_gen_polygon>& straight, 
												 list<r_circle_gen_polygon>& curved)
{
	rat_gen_polygon straight_gp;
	forall(straight_gp, straight)
		curved.append( r_circle_gen_polygon(straight_gp) );
}

void 
dxf_io::convert_segments_from_straight_to_curved(const list<rat_segment>& straight, 
												 list<r_circle_segment>& curved)
{
	rat_segment straight_seg;
	forall(straight_seg, straight)
		curved.append( r_circle_segment(straight_seg) );
}

int 
dxf_io::convert_polygons_from_curved_to_straight(const list<r_circle_polygon>& curved, 
												 list<rat_polygon>& straight,
												 double dist)
{
	int num_approx_segs = 0;

	r_circle_polygon curved_poly;
	forall(curved_poly, curved) {
		unsigned num_straight_segs = 0;
		r_circle_segment curved_seg;
		forall_segments(curved_seg, curved_poly) {
			if (curved_seg.is_straight_segment()) ++num_straight_segs;
		}
		rat_polygon straight_poly = curved_poly.approximate_by_rat_polygon(dist);
		straight.append(straight_poly);
		num_approx_segs += straight_poly.size() - num_straight_segs;
	}

	return num_approx_segs;
}

int 
dxf_io::convert_polygons_from_curved_to_straight(const list<r_circle_gen_polygon>& curved, 
												 list<rat_gen_polygon>& straight, 
												 double dist)
{
	int num_approx_segs = 0;

	r_circle_gen_polygon curved_gp;
	forall(curved_gp, curved) {
		list<rat_polygon> straight_polys;
		num_approx_segs += 
			convert_polygons_from_curved_to_straight(curved_gp.polygons(), straight_polys, dist);
		straight.append( rat_gen_polygon(straight_polys, rat_gen_polygon::NO_CHECK) );
	}

	return num_approx_segs;
}

int 
dxf_io::convert_segments_from_curved_to_straight(const list<r_circle_segment>& curved, 
												 list<rat_segment>& straight,
												 double dist)
{
	int num_approx_segs = 0;

	r_circle_segment curved_seg;
	forall(curved_seg, curved) {
		list<rat_segment> approx_segs = curved_seg.approximate_by_rat_segments(dist);
		if (!curved_seg.is_straight_segment())
			num_approx_segs += approx_segs.size();
		straight.conc(approx_segs);
	}

	return num_approx_segs;
}

/// output functions (private)

void 
dxf_io::output_pair(int code, const string& value)
{
	ostream& out = *Output;

	out << "  " << code << endl;
	out << value << endl;
}

void 
dxf_io::output_pair(int code, double value)
{
	string val_str = string("%f", value);
	output_pair(code, val_str);
}

void 
dxf_io::output_pair(int code, int value)
{
	string val_str = string("%d", value);
	output_pair(code, val_str);
}

void 
dxf_io::output_layer()
{
	output_pair(GR_LAYER, Layer);
}

void 
dxf_io::output_begin()
{
}

void 
dxf_io::output_end()
{
	output_pair(GR_SEP, "EOF");
}

void 
dxf_io::output_section_begin()
{
	output_pair(GR_SEP, "SECTION");
}

void 
dxf_io::output_section_end()
{
	output_pair(GR_SEP, "ENDSEC");
}

void 
dxf_io::output_entities_begin()
{
	output_section_begin();
	output_pair(GR_SECTION, "ENTITIES");
}

void 
dxf_io::output_entities_end()
{
	output_section_end();
}

void 
dxf_io::output_polygon(const r_circle_polygon& p)
{
	if (p.is_rat_circle())
		output_circle( p.to_rat_circle() );
	else {
		if (RespectOrientation || p.orientation() >= 0)
			output_closed_polyline(p);
		else
			output_closed_polyline(p.complement());
	}
}

void 
dxf_io::output_closed_polyline(const r_circle_polygon& poly)
{
	output_pair(GR_SEP, "POLYLINE");
	output_layer();
	output_pair(GR_POLYLINE_FLAGS, 1); // closed line
	output_pair(GR_POLYLINE_VERTICES_FOLLOW, 1); // always there

	r_circle_segment seg;
	forall_segments(seg, poly) {
		if (seg.is_trivial()) continue;

		if (seg.is_straight_segment()) {
			point p = seg.source().to_float();
			output_vertex(p);
		}
		else {
			rat_point p = seg.source().approximate_by_rat_point();
			rat_point q = seg.target().approximate_by_rat_point();
			rat_point m = midpoint(p, q);
			point r = seg.real_middle().to_float();

			double arc_parameter = 0;
			double d = p.to_float().distance(q.to_float());
			double h = m.to_float().distance(r);
			if (d != 0) arc_parameter = seg.orientation() * 2 * (h / d);

			output_vertex(p.to_float(), arc_parameter);
		}
	}

	output_pair(GR_SEP, "SEQEND");
	output_layer();
}

void 
dxf_io::output_closed_polyline(const rat_polygon& poly)
{
	output_pair(GR_SEP, "POLYLINE");
	output_layer();
	output_pair(GR_POLYLINE_FLAGS, 1); // closed line
	output_pair(GR_POLYLINE_VERTICES_FOLLOW, 1); // always there

	rat_segment seg;
	forall_segments(seg, poly) {
		if (seg.is_trivial()) continue;
		point p = seg.source().to_float();
	}

	output_pair(GR_SEP, "SEQEND");
	output_layer();
}

void 
dxf_io::output_curved_segment(const r_circle_segment& s)
{
	if (s.is_straight_segment()) {
		output_line(s.source().to_float(), s.target().to_float());
	}
	else if (s.is_full_circle()) {
		output_circle(s.circle());
	}
	else if (s.is_proper_arc()) {
		circle c = s.circle().to_float();
		point cen = c.center();
		point ref = cen.translate(c.radius(), 0);
		double start_angle = cen.angle(ref, s.source().to_float());
		double end_angle   = cen.angle(ref, s.target().to_float());
		output_arc(s.circle(), start_angle, end_angle);
	}
}

void 
dxf_io::output_vertex(const point& p, double arc_parameter)
{
	output_pair(GR_SEP, "VERTEX");
	output_layer();

	double x = p.xcoord(), y = p.ycoord(); 
	output_pair(GR_X, x);
	output_pair(GR_Y, y);

	if (arc_parameter != 0.0) {
		output_pair(GR_VERT_ARC, arc_parameter);
		++NumArcSegments;
	}
	else {
		++NumStraightSegments;
	}
}

void 
dxf_io::output_line(const point& p, const point& q)
{
	output_pair(GR_SEP, "LINE");
	output_layer();

	double x0 = p.xcoord(), y0 = p.ycoord(); 
	output_pair(GR_X0, x0);
	output_pair(GR_Y0, y0);

	double x1 = q.xcoord(), y1 = q.ycoord(); 
	output_pair(GR_X1, x1);
	output_pair(GR_Y1, y1);

	++NumStraightSegments;
}

void 
dxf_io::output_circle(const rat_circle& c)
{
	output_pair(GR_SEP, "CIRCLE");
	output_layer();

	circle cf = c.to_float();
	double x = cf.center().xcoord(), y = cf.center().ycoord(), radius = cf.radius(); 
	output_pair(GR_X, x);
	output_pair(GR_Y, y);
	output_pair(GR_RADIUS, radius);
	++NumCircles;
}

void 
dxf_io::output_arc(const rat_circle& c, double start_angle_rad, double end_angle_rad)
{
	output_pair(GR_SEP, "ARC");
	output_layer();

	circle cf = c.to_float();
	double x = cf.center().xcoord(), y = cf.center().ycoord(), radius = cf.radius();
	double start_angle_deg = start_angle_rad * 180 / LEDA_PI;
	double end_angle_deg   = end_angle_rad   * 180 / LEDA_PI;
	if (start_angle_deg < 0) start_angle_deg += 360;
	if (end_angle_deg < 0)   end_angle_deg   += 360;

	output_pair(GR_X, x);
	output_pair(GR_Y, y);
	output_pair(GR_RADIUS, radius);
	output_pair(GR_START_AGL, start_angle_deg);
	output_pair(GR_END_AGL, end_angle_deg);
	++NumArcSegments;
}

/// writing functions (public)

void 
dxf_io::writing_begin(const string& filename, string layer)
{
	clear();
	Layer = layer;

	Output = new ofstream(filename);

	if (Output->bad()) {
		error("dxf_io::write: cannot open file");
		return;
	}

	output_begin();

	output_entities_begin();
}

void 
dxf_io::writing_append(const r_circle_polygon& p, string layer)
{
	string old_layer = Layer;
	if (!layer.empty()) Layer = layer;

	output_polygon(p);

	Layer = old_layer;
}

void 
dxf_io::writing_append(const list<r_circle_polygon>& L, string layer)
{
	r_circle_polygon p;
	forall(p, L) writing_append(p, layer);
}

void 
dxf_io::writing_append(const r_circle_gen_polygon& gp, string layer)
{
	writing_append(gp.polygons(), layer);
}

void 
dxf_io::writing_append(const rat_polygon& p, string layer)
{
	string old_layer = Layer;
	if (!layer.empty()) Layer = layer;

	output_polygon(p);

	Layer = old_layer;
}

void 
dxf_io::writing_append(const list<rat_polygon>& L, string layer)
{
	r_circle_polygon p;
	forall(p, L) writing_append(p, layer);
}

void 
dxf_io::writing_append(const rat_gen_polygon& gp, string layer)
{
	writing_append(gp.polygons(), layer);
}

void 
dxf_io::writing_append(const r_circle_segment& s, string layer)
{
	string old_layer = Layer;
	if (!layer.empty()) Layer = layer;

	output_curved_segment(s);

	Layer = old_layer;
}

void 
dxf_io::writing_append(const list<r_circle_segment>& L, string layer)
{
	r_circle_segment s;
	forall(s, L) writing_append(s, layer);
}

void 
dxf_io::writing_end()
{
	output_entities_end();

	output_end();
}

/// write functions (public)

void 
dxf_io::write(const string& filename, const list<r_circle_polygon>& L, string layer)
{
	writing_begin(filename, layer);
	writing_append(L);
	writing_end();
}

void 
dxf_io::write(const string& filename, const r_circle_gen_polygon& p, string layer)
{
	write(filename, p.polygons(), layer);
}

void 
dxf_io::write(const string& filename, const list<r_circle_gen_polygon>& GPs, string layer)
{
	writing_begin(filename, layer);

	r_circle_gen_polygon GP;
	forall(GP, GPs) {
		writing_append(GP);
	}

	writing_end();
}

void 
dxf_io::write(const string& filename, const list<rat_polygon>& L, string layer)
{
	writing_begin(filename, layer);
	writing_append(L);
	writing_end();
}

void 
dxf_io::write(const string& filename, const rat_gen_polygon& p, string layer)
{
	write(filename, p.polygons(), layer);
}

void 
dxf_io::write(const string& filename, const list<rat_gen_polygon>& GPs, string layer)
{
	writing_begin(filename, layer);

	r_circle_gen_polygon GP;
	forall(GP, GPs) {
		writing_append(GP);
	}

	writing_end();
}

LEDA_END_NAMESPACE
