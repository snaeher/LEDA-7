/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  r_circle_polygon_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/r_circle_gen_polygon.h>
#include <LEDA/graphics/real_window.h>

#include <LEDA/graphics/window.h>
#include <LEDA/graphics/pixmaps/button32.h>

#include <LEDA/system/file.h>
#include <LEDA/graphics/file_panel.h> // for history
#include <LEDA/core/tuple.h> // for history

#include <LEDA/geo/rat_polygon.h>

#include <LEDA/system/file.h>

#include <fstream> 

#include <stdio.h> // for rename

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cerr;
using std::istream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::endl;
#endif


static window W(540,580,"LEDA Polygon With Circular Edges Demo");
static int window_width;
static int window_height;

static r_circle_gen_polygon P;

static list<r_circle_polygon> result;  static r_circle_gen_polygon R;

static color pcol1 = grey1;
static color pcol2 = grey2;

static bool approximate = false;

static int grid_mode = 0;

static void message(string s) 
{ W.set_status_string(s + "\\c "); }


static void update_msg(const r_circle_gen_polygon& P, string msg = "Polygon P")
{
	if (P.is_trivial()) { message(""); return; }

	int n = 0, h = 0;
	r_circle_polygon p;
	forall(p,P.polygons()) if ( p.orientation() > 0 ) n++; else h++;

	string p_string = "polygon";
	if ( n != 1 ) p_string += "s";
	string h_string = "hole";
	if ( h != 1 ) h_string += "s";
	string display_string = "\\bf %s\\rm ~~~%d " + p_string + " ~~~%d " + h_string + " ~~~orient %d ";
	msg = string(display_string, ~msg,n,h,P.orientation());

	double area = P.approximate_area();
	msg += string("~~~area %.2f", area);

	bool convex = P.is_convex();
	msg += "~~~conv " + string(convex ? '+' : '-') + " ";

	message(msg);
}

static void draw_poly(const r_circle_polygon& P,color pcol)
{ 
	if (P.orientation() > 0) draw_filled(W,P,pcol);
	else { 
		W.draw_box(W.xmin(),W.ymin(),W.xmax(),W.ymax(),pcol);
		draw_filled(W,P,white);
	}
	draw(W,P,black); // oriented
}
  
static bool draw_poly(const r_circle_gen_polygon& P)
{ 
	draw_filled(W,P,grey1);
	draw(W,P,black); // oriented

	return true;
}


r_circle_polygon hilbert(int n) 
{
	double x1, y1, x2, y2;

	int gmode = W.get_grid_mode();

	if (gmode > 0) {
		int d = int((W.xmax() - W.xmin())/gmode) - 3;
		x1 = W.xmin() + gmode;   
		y1 = W.ymin() + gmode;
		x2 = W.xmin() + d*gmode;
		y2 = W.ymin() + d*gmode;
	}
	else {
		x1 = W.xmin() + 50;   
		y1 = W.ymin() + 50;
		x2 = W.xmax() - 100;
		y2 = W.ymax() - 100;
	}

	message("Computing Hilbert Curve");

	return hilbert(n,x1,y1,x2,y2);
}

void point_location(const r_circle_gen_polygon& P)
{
	draw_poly(P);

	int gmode = W.get_grid_mode();

	if (gmode > 0) {
		// grid mode:
		for(double x = W.xmin(); x < W.xmax(); x += gmode)
			for(double y = W.ymin(); y < W.ymax(); y += gmode) {
				point p(x,y);
				int side = P.side_of(rat_point(p));
				if (side > 0) 
					W.draw_filled_node(p,red);
				else if (side < 0)
					W.draw_filled_node(p,blue);
				else
					W.draw_filled_node(p,green);
			}
		return;
	}

	// no grid: random points
	int i = 0;
	while (i < 4000) {
		i++;
		if (i % 10 == 0) message(string("\\bf Side Of Test:\\rm %5d points.",i));

		if (W.get_button() != NO_BUTTON) break;

		int x0 = (int)W.xmin();
		int x1 = (int)W.xmax();
		int y0 = (int)W.ymin();
		int y1 = (int)W.ymax();

		point p(rand_int(x0,x1),rand_int(y0,y1));

		int side = P.side_of(rat_point(p));
		if (side > 0) 
			W.draw_filled_node(p,red);
		else if (side < 0)
			W.draw_filled_node(p,blue);
		else
			W.draw_filled_node(p,green);
	}
	message(string("%5d  random points",i));
}


r_circle_polygon mouse_polygon(window& W)
{
	W.disable_panel();
	r_circle_polygon Q;
	bool ok = false;
	bool go_on = false;
	do {
		W >> Q; if (!W) break;
		if (Q.is_weakly_simple())
			ok = true;
		else {
			panel msg("Error");
			msg.text_item("");
			msg.text_item("Polygon is \\red not simple\\black .");
			msg.text_item("");
			msg.button("again",0);
			msg.button("cancel",1);
			go_on = msg.open(W) != 1;
			W.clear();
		}
	} while(!ok && go_on);
	W.enable_panel();

#ifdef LEDA_DEBUG
	cout << string("area: %10.2f", Q.approximate_area()) << endl;
#endif
	return ok ? Q : r_circle_polygon();
}


/// HISTORY /////////////////////////////////////////////////////////////////////////////////

enum OP_ID { 
	o_noop = 0, o_new = 1, o_intersect = 2, o_unite = 3, o_diff = 4, 
	o_symdiff = 5, o_complement = 6, o_translate = 7, o_rotate90 = 8,
	o_reflect_point = 9, o_reflect_line = 10, o_clip = 11, 
	o_intersect_line = 12, o_intersect_segment = 13, o_dist = 14
};

enum OP_ARG_TYPE { o_arg_none, o_arg_polygon, o_arg_doubles };

const char* op_long_name[] = 
	{ "(Start)", "New Polygon", "Intersection", "Union", "Difference", 
	  "Symmetric Difference", "Complement", "Translation", "Rotation by 90 Degrees",
	  "Reflection Across Point", "Reflection Across Line", "Clip Against Halfplane", 
	  "Intersection With Line", "Intersection With Segment", "Distance to Point"
	};

const char* op_member_name[] = 
	{ 0, "new", "intersect", "unite", "diff", 
	  "sym_diff", "complement", "translate", "rotate90",
	  "reflect", "reflect", "clip_against_halfplane", 
	  "intersection", "intersection", "dist"
	};


typedef four_tuple<double,double,double,double> four_doubles;

struct Operation {
	OP_ID Id;
	r_circle_polygon ArgPolygon;
	four_doubles     ArgDoubles;

	Operation(OP_ID id = o_noop) : Id(id),ArgDoubles(0,0,0,0) {}
	Operation(OP_ID id, const r_circle_polygon& Q) : Id(id), ArgPolygon(Q), ArgDoubles(0,0,0,0) {}
	Operation(OP_ID id, const four_doubles& doubles) : Id(id), ArgDoubles(doubles) {}

	OP_ARG_TYPE arg_type() const { return arg_type(Id); }

	static OP_ARG_TYPE arg_type(OP_ID id)
	{ 
		if (id == o_noop || id == o_complement) return o_arg_none;
		return id < o_complement ? o_arg_polygon : o_arg_doubles;
	}
};

ostream& operator<<(ostream& os, const Operation& op)
{
	os << op.Id << endl;
	switch (op.arg_type()) {
	case o_arg_none:	break;
	case o_arg_polygon:	os << op.ArgPolygon; break;
	case o_arg_doubles:	os << op.ArgDoubles; break;
	}
	return os;
}

istream& operator>>(istream& is, Operation& op)
{
	int id; is >> id; op.Id = OP_ID(id);
	switch (op.arg_type()) {
	case o_arg_none:	break;
	case o_arg_polygon:	is >> op.ArgPolygon; break;
	case o_arg_doubles:	is >> op.ArgDoubles; break;
	}
	return is;
}

static ofstream history; 
static unsigned hist_cnt = 0;

void record_history(const Operation& op)
{
  history << ++hist_cnt << " " << op << endl; history.flush();
}

void perform_operation(const Operation& op, r_circle_gen_polygon& P)
{
	record_history(op);

	r_circle_polygon Q = op.ArgPolygon;
	double x0 = op.ArgDoubles.first(), y0 = op.ArgDoubles.second();
	double x1 = op.ArgDoubles.third(), y1 = op.ArgDoubles.fourth();

	switch (op.Id) {

        case o_intersect_line:
        case o_intersect_segment:
        case o_dist:
	case o_noop:			break;

	case o_new:				P = Q; break;
	case o_intersect:		P = approximate ? P.intersection_approximate(Q) : P.intersection(Q); break;
	case o_unite:			P = approximate ? P.unite_approximate(Q)        : P.unite(Q); break;
	case o_diff:			P = approximate ? P.diff_approximate(Q)         : P.diff(Q); break;
	case o_symdiff:			P = approximate ? P.sym_diff_approximate(Q)     : P.sym_diff(Q); break;
	case o_complement:		P = P.complement(); break;
	case o_translate:		P = P.translate(rational(x1-x0),rational(y1-y0)); break;
	case o_rotate90:		P = P.rotate90(rat_point(x0,y0)); break;
	case o_reflect_point:	P = P.reflect(rat_point(x0,y0)); break;
	case o_reflect_line:	P = P.reflect(rat_point(x0,y0), rat_point(x1,y1)); break;
	case o_clip:
		{
			if (P.is_r_circle_polygon() && !P.is_full()) {
				r_circle_polygon Q = P.to_r_circle_polygon(); 
				rat_line l(rat_point(x0,y0), rat_point(x1,y1));
				P = Q.clip_against_halfplane(l);
			}
			else P = r_circle_polygon(); // error
			break;
		}
	}
}

void display_result(OP_ID id, const r_circle_gen_polygon& P, color fill_col = red)
{
	string msg;
	switch (id) {
	case o_noop:
		msg = "Start"; break;
	case o_new:
		msg = "New Polygon P"; break;
	default:
		msg = "P."+string(op_member_name[id]);
		if (approximate) msg += "App";
		switch (Operation::arg_type(id)) {
		case o_arg_none:		msg += "()"; break;
		case o_arg_polygon:		msg += "(Q)"; break;
		case o_arg_doubles:		msg += "(...)"; break;
		}
		break;
	}

	draw_filled(W,P,fill_col);
	draw(W,P,black); // oriented

	update_msg(P, msg);
}

void display_points(const list<r_circle_point>& L, string msg = string())
{
	r_circle_point p;
	forall(p, L) {
		W.draw_filled_node(p.to_float(), red);
	}

	update_msg(P, msg);
	W.read_mouse();
}

void do_operation(OP_ID id)
{
	string long_name(op_long_name[id]);
	string member_name(op_member_name[id]);

	draw_poly(P);

	Operation op(id);
	if (op.arg_type() == o_arg_polygon) {
		message("\\bf "+long_name+":\\rm Draw a second r_circle_polygon Q.");
		r_circle_polygon Q = mouse_polygon(W);
		draw_poly(Q,pcol2);
		op.ArgPolygon = Q;
	}

	message("Computing "+long_name);
	perform_operation(op, P);
	display_result(id, P);
}


/// history tracer >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

class history_tracer {
public:
	typedef two_tuple<Operation, r_circle_gen_polygon> op_info;

public:
	history_tracer() { init(); }

	void init();
	void init(const string& file_name);

	bool first() const { return CurrentOperation == StoredOperations.first(); }
	bool eof() const { return CurrentOperation == nil; }

	bool advance();
	bool step_back();

	void rewind() { CurrentOperation = StoredOperations.first(); }

	void go_to_end() { while(advance()); step_back(); }

	void go_to_last_evaluated() { CurrentOperation = LastEvaluatedOperation; }

	Operation get_current_operation() const 
	{ return operation(StoredOperations[CurrentOperation]); }

	r_circle_polygon get_current_polygon_operand() const 
	{ return polygon_operand(StoredOperations[CurrentOperation]); }

	r_circle_gen_polygon get_current_polygon()
	{
		if (eof()) return r_circle_gen_polygon();
		evaluate_current_operation_if_necessary();
		return result(StoredOperations[CurrentOperation]);
	}

	r_circle_gen_polygon get_previous_polygon() const 
	{
		if (first()) return r_circle_gen_polygon();
		list_item prev_oper = eof() ? StoredOperations.last() : StoredOperations.pred(CurrentOperation);
		return result(StoredOperations[prev_oper]); 
	}

private:
	const Operation&            operation(const op_info& info) const { return info.first(); }
	const OP_ID&                id(const op_info& info) const { return operation(info).Id; }
	const r_circle_polygon&     polygon_operand(const op_info& info) const { return operation(info).ArgPolygon; }
	const r_circle_gen_polygon& result(const op_info& info) const { return info.second(); }
	      r_circle_gen_polygon& result(op_info& info) { return info.second(); }

	void evaluate_current_operation_if_necessary();

private:
	list<op_info> StoredOperations;
	list_item     CurrentOperation;
	list_item     LastEvaluatedOperation;
};

void
history_tracer::init()
{
	StoredOperations.clear();
	StoredOperations.append( op_info(Operation(), r_circle_gen_polygon()) );
	LastEvaluatedOperation = StoredOperations.first();

	rewind();
}

void
history_tracer::init(const string& file_name)
{
	init();

	ifstream in(file_name);

	while (in) {
		int cnt; in >> cnt; if (!in) break;
		Operation op; in >> op; if (!in) break;

		StoredOperations.append(op_info(op, r_circle_gen_polygon()));
	}
}

bool history_tracer::step_back() 
{ 
	if (first()) return false;
	CurrentOperation = eof() ? StoredOperations.last() : StoredOperations.pred(CurrentOperation);
	return true;
}

bool 
history_tracer::advance()
{
	if (eof()) return false;

	evaluate_current_operation_if_necessary();
	CurrentOperation = StoredOperations.succ(CurrentOperation);

	return !eof();
}

void 
history_tracer::evaluate_current_operation_if_necessary()
{
	if (LastEvaluatedOperation != StoredOperations.pred(CurrentOperation)) return;

	r_circle_gen_polygon P = get_previous_polygon();
	op_info& current_info = StoredOperations[CurrentOperation];
	perform_operation(operation(current_info), P);
	result(current_info) = P;
	LastEvaluatedOperation = CurrentOperation;
}

static history_tracer tracer;

/// history tracer <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


static void read_file(string file_name)
{
	tracer.init(file_name);
}

void trace_history()
{
	static string fname("history1.txt");
	static string dname(".");
	static string filter("*.txt");

	file_panel FP(W, fname, dname);
	FP.set_load_handler(read_file);
	if (filter != "") FP.set_pattern(filter);
	FP.open();

	P = tracer.get_current_polygon(); // if there should be redraw ...

	int but = 0;
	tracer.advance();
	while(!tracer.eof()) {
		W.draw_box(W.xmin(),W.ymin(),W.xmax(),W.ymax(),white);
		draw_poly(tracer.get_previous_polygon());

		Operation op = tracer.get_current_operation();
		if (op.arg_type() == o_arg_polygon)
			draw_poly(tracer.get_current_polygon_operand(), grey2);
		message("Trace: Next Operation \\bf "+string(op_long_name[op.Id]));

		but = W.read_mouse(); message("");
		if (but == MOUSE_BUTTON(2)) break;

		P = tracer.get_current_polygon();
		display_result(op.Id, P);

		but = W.read_mouse();
		if (but == MOUSE_BUTTON(1) && W.shift_key_down()) {
			tracer.step_back(); if (tracer.first()) tracer.advance();
		}
		if (but == MOUSE_BUTTON(2)) break;
		if (but == MOUSE_BUTTON(3)) tracer.advance();
	}
	
	if (but == MOUSE_BUTTON(2) && !W.shift_key_down())
		tracer.go_to_end();
	else 
		tracer.go_to_last_evaluated(); 

	P = tracer.get_current_polygon();
	W.clear();
	draw_poly(P);
	message("Trace finished");
}


/// callback functions //////////////////////////////////////////////////////////////////////

void redraw(window* wp, double x0, double y0, double x1, double y1)
{
//	cout << "redraw " << x0 << " " << y0 << " / " << x1 << " " << y1; cout.flush();
	++x1; ++y1;
	list<r_circle_segment> clip_P_segs;
	rat_point c1(point(x0,y0)), c2(point(x1,y0)), c3(point(x1,y1)), c4(point(x0,y1));
	clip_P_segs.append(rat_segment(c1,c2));
	clip_P_segs.append(rat_segment(c2,c3));
	clip_P_segs.append(rat_segment(c3,c4));
	clip_P_segs.append(rat_segment(c4,c1));
	r_circle_gen_polygon clip_P(clip_P_segs);

	r_circle_gen_polygon Q = P.intersection(clip_P);
	draw_filled(*wp,Q,pcol1);
	draw(*wp,Q,black); // oriented

	if (wp->width() != window_width || wp->height() != window_height) { 
		window_width = wp->width();
		window_height = wp->height();
	}
//	cout << " done" << endl; cout.flush();
}

void setgrid(int d) { grid_mode = d; }

void select_poly(int i)
{
	W.clear();
	if (i == 0) return;

	r_circle_polygon Q;

	if ( i <= 6 ) Q = hilbert(i);
	else {
		double x1 = W.xmin() + 50;   
		double y1 = W.ymin() + 50;
		double x2 = W.xmax() - 100;
		double y2 = W.ymax() - 100;
		circle C((x1 + x2)/2,(y1 + y2)/2, (y2 - y1)/2);
		Q = r_circle_polygon(n_gon(i,C,0.01));
	}

	perform_operation(Operation(o_new, Q), P);
}

void move_poly(int _id)
{
	OP_ID id = OP_ID(_id);

	double x0 = (W.xmax()-W.xmin()) / 2, y0 = (W.ymax()-W.ymin()) / 2;
	double x1 = 0, y1 = 0;

	switch (id) {

        case o_noop:
        case o_new:
        case o_intersect:
        case o_unite:
        case o_diff:
        case o_symdiff:
        case o_complement: break;

	case o_translate:
		message("Specify translation vector");
		W.read_mouse_seg(x0,y0, x1,y1);
		break;
	case o_rotate90:
		message("Specify center of rotation");
		W.read_mouse(x0,y0);
		break;
	case o_reflect_point:
		message("Specify reflection point");
		W.read_mouse(x0,y0);
		break;
	case o_reflect_line:
		message("Specify reflection line");
		W.read_mouse(x0,y0); W.read_mouse_line(x0,y0, x1, y1);
		break;
	case o_clip:
		message("Specify bounding line of clip-halfplane");
		W.read_mouse(x0,y0); W.read_mouse_line(x0,y0, x1, y1);
		break;
	case o_intersect_line:
		message("Specify intersection line");
		W.read_mouse(x0,y0); W.read_mouse_line(x0,y0, x1, y1);
		display_points( P.intersection(rat_line(rat_point(x0,y0), rat_point(x1,y1))) );
		return;
	case o_intersect_segment:
		{
			message("Specify intersection segment");
			r_circle_segment s; W >> s;
			display_points( P.intersection(s) );
			return;
		}
	case o_dist:
		{
			message("Specify point for distance measurement");
			W.read_mouse(x0,y0); point cen(x0,y0);
			double dist = P.dist(real_point(x0,y0)).to_double();
			W.draw_circle(circle(cen, dist), green); W.draw_filled_node(cen, red);
			message(string("Distance to P: %.2f", dist));
			W.read_mouse();
			return;
		}
	}

	Operation op(id, four_doubles(x0,y0,x1,y1));
	perform_operation(op, P);
	display_result(op.Id, P, grey1);
}


/// main ////////////////////////////////////////////////////////////////////////////////////

int main()
{
//	W.set_show_coordinates(true);

	if (is_file("history.txt")) {
		if (is_file("history.sav")) delete_file("history.sav");
		rename("history.txt", "history.sav");
	}
	history.open("history.txt");

	int max_c = 1000;

	panel help_panel("Help Panel");
	help_panel.set_panel_bg_color(ivory);

help_panel.text_item("\
This program demonstrates generalized polygons. It manipulates a \
\\bf generalized polygon P. \\rm P is initially empty. The draw \
button allows to give P a new value and the select button allows to \
select P from a set of predefined polygons. P is updated by boolean \
operations. The binary boolean operations operate on P and a polygon \
Q. Q must be input after selecting the boolean operation. The side-of \
button performs the side-of test on P for a set of random points. \
Points in P are shown red, points outside P are blue, and points on P \
are green. ");

	help_panel.button("continue");


	menu poly_menu;
	poly_menu.button("hilbert(2) ",2,select_poly);
	poly_menu.button("hilbert(3) ",3,select_poly);
	poly_menu.button("hilbert(4) ",4,select_poly);
	poly_menu.button("hilbert(5) ",5,select_poly);
	poly_menu.button("hilbert(6) ",6,select_poly);
	poly_menu.button("n-gon(10) ",10,select_poly);
	poly_menu.button("n-gon(20) ",20,select_poly);
	poly_menu.button("n-gon(40) ",40,select_poly);
	poly_menu.button("n-gon(80) ",80,select_poly);
	poly_menu.button("current poly",0,select_poly);

	menu grid_menu;
	grid_menu.button("no grid", 0,setgrid);
	grid_menu.button("dist 10",10,setgrid);
	grid_menu.button("dist 20",20,setgrid);
	grid_menu.button("dist 30",30,setgrid);
	grid_menu.button("dist 40",40,setgrid);
	grid_menu.button("dist 50",50,setgrid);

	menu move_menu;
	move_menu.button("translate",         o_translate,         move_poly);
	move_menu.button("rotate90",          o_rotate90,          move_poly);
	move_menu.button("reflect (point)",   o_reflect_point,     move_poly);
#if defined(LEDA_DEBUG) || defined(LEDA_POLY_MOVE)
	move_menu.button("reflect (line)",    o_reflect_line,      move_poly);
#endif
	move_menu.button("clip",              o_clip,              move_poly);
	move_menu.button("intersect (line)",  o_intersect_line,    move_poly);
	move_menu.button("intersect (seg)",   o_intersect_segment, move_poly);
	move_menu.button("distance",          o_dist,              move_poly);

	menu op_menu;

	char* poly_pr       = W.create_pixrect(pen_xpm);
	char* hilb_pr       = W.create_pixrect(maze_xpm);
	char* grid_pr       = W.create_pixrect(grid_xpm);
	char* inter_pr      = W.create_pixrect(intersect_xpm);
	char* union_pr      = W.create_pixrect(puzzle_xpm);
	char* diff_pr       = W.create_pixrect(scissors_xpm);
	char* sdiff_pr      = W.create_pixrect(intersect2_xpm);
	char* compl_pr      = W.create_pixrect(complement_xpm);
	char* side_of_pr    = W.create_pixrect(watch_xpm);
	char* hist_trace_pr = W.create_pixrect(blocks_xpm);
	char* help_pr       = W.create_pixrect(help_xpm);
	char* exit_pr       = W.create_pixrect(door_exit_xpm);
	char* move_pr       = W.create_pixrect(mouse_xpm);


	W.button(grid_pr,        grid_pr,       "define a grid",    99, grid_menu);
	W.button(hilb_pr,        hilb_pr,       "list of polygons", 20, poly_menu);  
	W.button(poly_pr,        poly_pr,       "draw a polygon",   o_new);  
	W.button(inter_pr,       inter_pr,      "intersect",        o_intersect);
	W.button(union_pr,       union_pr,      "union",            o_unite);
	W.button(diff_pr,        diff_pr,       "diff",             o_diff);
	W.button(sdiff_pr,       sdiff_pr,      "symdiff",          o_symdiff);
	W.button(compl_pr,       compl_pr,      "complement",       o_complement);
	W.button(side_of_pr,     side_of_pr,    "side_of",          21);
	W.button(help_pr,        help_pr,       "help",             22);
	W.button(move_pr,        move_pr,       "move",             24, move_menu);
	W.button(hist_trace_pr,  hist_trace_pr, "trace history",    23);
	W.button(exit_pr,        exit_pr,       "exit",             0);

	W.make_menu_bar();


	W.set_redraw(redraw);
	W.set_node_width(2);  
	//W.set_line_width(2);
	W.init(0,max_c,0,grid_mode);
	W.display(window::center,window::center);
	W.open_status_window();

#ifndef LEDA_DEBUG
	help_panel.open(W);
#endif
	W.clear();

	window_width = W.width();
	window_height = W.height();

	r_circle_polygon Q;

	int but;
	while ((but = W.read_mouse()) != 0) {
		if (but > 0) W.init(0,max_c,0,grid_mode); // clears window

		switch (but)  {

		case MOUSE_BUTTON(2):
			W.clear(); W.redraw(); break;

		case MOUSE_BUTTON(3):
			{	// cocircular vertices are shown in red
				list<r_circle_point> old_verts = P.vertices();
				list<r_circle_point> new_verts = P.eliminate_cocircular_vertices().vertices();
				r_circle_point p;
				forall(p, old_verts) W.draw_filled_node(p.to_float(), red);
				forall(p, new_verts) W.draw_filled_node(p.to_float(), green);
				break;
			}

		case o_new: 
			{ 
				P = r_circle_gen_polygon(); W.clear();
				message("Please draw a \\bf simple r_circle_polygon P.");
				Q = mouse_polygon(W); message("");
				perform_operation(Operation(o_new, Q), P);
				display_result(o_new, P, grey1);
				break;
             }
 
	    case o_intersect:
		case o_unite:
		case o_diff: 
		case o_symdiff: 
		case o_complement: 
			do_operation(OP_ID(but)); break;

		case 20:
			draw_poly(P); break;

		case 21:
			point_location(P); break;

		case 22:
			help_panel.open(W); if (! P.is_trivial()) { W.clear(); draw_poly(P); } break;

		case 23:
			trace_history(); break;

		case 24:
			draw_poly(P); break;

		} // switch

	} // while

	return 0;

}

