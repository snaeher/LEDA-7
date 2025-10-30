/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _real_window.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/real_window.h>
#include <LEDA/geo/float_kernel.h>
#include <LEDA/geo/real_kernel.h>
#include <LEDA/geo/r_circle_gen_polygon.h>
#include <LEDA/numbers/fp.h>

LEDA_BEGIN_NAMESPACE

#if defined(__hpuxcc__)

#define def_input_op(type)\
window& operator>>(window& W, real_##type &r)\
{ type q; W >> q; r = real_##type(q); return W; }

def_input_op(point)
def_input_op(segment)
def_input_op(ray)
def_input_op(line)
def_input_op(circle)
def_input_op(polygon)
def_input_op(gen_polygon)
def_input_op(rectangle)
def_input_op(triangle)

#else

template <class T>
inline window& read_object(window& W, T& p)
{ typename T::float_type q;
  W >> q;
  p = T(q);
  return W;
}

window& operator>>(window& W, real_point& p)
{ return read_object(W,p); }

window& operator>>(window& W, real_segment& s)
{ return read_object(W,s); }

window& operator>>(window& W, real_ray& r)
{ return read_object(W,r); }

window& operator>>(window& W, real_line& l)
{ return read_object(W,l); }

window& operator>>(window& W, real_circle& c)
{ return read_object(W,c); }

window& operator>>(window& W, real_polygon& p)
{ return read_object(W,p); }

window& operator>>(window& W, real_gen_polygon& p)
{ return read_object(W,p); }

window& operator>>(window& W, real_rectangle& r)
{ return read_object(W,r); }

window& operator>>(window& W, real_triangle& t)
{ return read_object(W,t); }

#endif

// ------------------------------------------------------------------------------------------

template <class T>
inline window& draw_object(window& W, const T& p) 
{ 
  W << p.to_float();
  return W;
}


window& operator<<(window& W, const real_point& p)
{ return draw_object(W,p); }

window& operator<<(window& W, const real_segment& s)
{ return draw_object(W,s); }

window& operator<<(window& W, const real_ray& r)
{ return draw_object(W,r); }

window& operator<<(window& W, const real_line& l)
{ return draw_object(W,l); }

window& operator<<(window& W, const real_circle& c)
{ return draw_object(W,c); }

window& operator<<(window& W, const real_polygon& p)
{ return draw_object(W,p); }

window& operator<<(window& W, const real_gen_polygon& p)
{ return draw_object(W,p); }

window& operator<<(window& W, const real_rectangle& r)
{ return draw_object(W,r); }

window& operator<<(window& W, const real_triangle& t)
{ return draw_object(W,t); }


/// r_circle_segment //////////////////////////////////////////

window& read(window& W, r_circle_segment& cs)
{
        point src, tgt, middle;
        W.read_arc(src, tgt, middle);

        rat_point rat_src(src); rat_src.normalize();
        rat_point rat_tgt(tgt); rat_tgt.normalize();
        rat_point rat_middle(middle); rat_middle.normalize();

        if (W) {
                if (rat_src != rat_tgt && rat_src != rat_middle && rat_tgt != rat_middle) 
                        cs = r_circle_segment(rat_src, rat_middle, rat_tgt);
                else
                        cs = r_circle_segment(rat_src, rat_tgt);
        }
        return W;
}

window& operator>>(window& W, r_circle_segment& cs)
{
        W.set_tmp_label(">> r_circle_segment");
        read(W, cs);
        W.reset_frame_label();
        return W;
}

window& operator<<(window& W, const r_circle_segment& cs)
{
        return draw(W, cs);
}

window& draw(window& W, const r_circle_segment& cs, color c)
{
/*
        real xmin, ymin, xmax, ymax;
        cs.compute_bounding_box(xmin, ymin, xmax, ymax);
        real_rectangle bbox(xmin, ymin, xmax, ymax);
        W.draw(bbox.to_float(), green);
*/

        if (cs.is_trivial()) {
                W.draw_point(cs.source().to_point(), c); return W;
        }
        if (cs.is_full_circle()) {
                W.draw_circle(cs.circle().to_circle(), c); return W;
        }

        point p1 = cs.source().to_point();
        point p3 = cs.target().to_point();

        // NOTE: Even if !cs.is_trivial() we might have that p1 == p3.

        if (cs.is_straight_segment() || p1 == p3) {
                W.draw_segment(p1, p3, c); return W;
        }

        point p2 = cs.circle().point2().to_point();
        if (orientation(real_point(p1), real_point(p2), real_point(p3)) != cs.orientation()) {
                p2 = cs.real_middle().to_point();
                if (orientation(real_point(p1), real_point(p2), real_point(p3)) != cs.orientation()) {
                        W.draw_segment(p1, p3, c); return W;
                }
        }

        if (W.get_show_orientation())
                W.draw_arc_arrow(p1, p2, p3, c);
        else
                W.draw_arc(p1, p2, p3, c);

        return W;
}

/// r_circle_polygon //////////////////////////////////////////

window& read(window& W, r_circle_polygon& p)
{
        bool buf = W.is_buffering();
        if (buf) W.stop_buffering();
        drawing_mode save = W.set_mode(xor_mode);

        list<r_circle_segment> segs;

        rat_point start, next_src;
        int but = 0;
        bool has_src = false;
        do {
                if (! has_src) {
                        // read the very first point ...
                        point src;
            but = W.read_mouse(src);
                        has_src = !W.shift_key_down();
                        next_src = src; next_src.normalize(); start = next_src;
                
                        if (has_src && W.ctrl_key_down() && but != MOUSE_BUTTON(1)) {
                                // read a circle with center next_src ...
                                point cen = next_src.to_point(), pnt_on_circle;
                                but = W.read_mouse_circle(cen, pnt_on_circle);
                                if (!W.shift_key_down() && cen != pnt_on_circle) {
                                        rat_circle c(next_src, pnt_on_circle); c.normalize();
                                        if (!W.ctrl_key_down()) c = c.reverse();
                                        p = r_circle_polygon(c);
                                        but = 999; break;
                                }
                                else has_src = false;
                        }
                }
                else {
                        // read a (possibly degenerate) segment starting in next_src ...
                        point src = next_src.to_point(), middle, tgt;
                        but = W.read_mouse_arc_with_fixed_src(src, tgt, middle);
                        if (but == MOUSE_BUTTON(3)) break;

                        if (! W.shift_key_down()) {
                                rat_point rat_middle(middle); rat_middle.normalize();
                                rat_point rat_tgt(tgt);       rat_tgt.normalize();

                                r_circle_segment new_edge;
                                if (next_src != rat_tgt && next_src != rat_middle && rat_middle != rat_tgt)
                                        new_edge = r_circle_segment(next_src, rat_middle, rat_tgt);
                                else
                                        new_edge = r_circle_segment(next_src, rat_tgt);
                                segs.append(new_edge);

                                W << new_edge;
                                next_src = rat_tgt;
                                if (next_src == start) break;
                        }
                        else {
                                if (segs.empty()) has_src = false;
                                else {
                                        r_circle_segment last_edge = segs.pop_back();
                                        W << last_edge;
                                        next_src = last_edge.source().to_rat_point();
                                }
                        }
                }
        } while (but == MOUSE_BUTTON(1));

        r_circle_segment cs;
        forall(cs, segs) W << cs;

        if (but == MOUSE_BUTTON(3) && !segs.empty() && start != next_src) {
                segs.append( r_circle_segment(next_src, start) );
                but = MOUSE_BUTTON(1);
        }

        if (but == MOUSE_BUTTON(1)) {
                // normal termination
                W.set_state(1);
                if (! r_circle_polygon::clean_chain(segs)) { W.set_state(0); segs.clear(); }
                p = r_circle_polygon(segs, r_circle_polygon::NO_CHECK);
        }
        else if (but == 999) {
                // normal termination, but polygon has already been constructed
                W.set_state(1);
        }
        else if (but == MOUSE_BUTTON(2)) {
                // break button
                W.set_state(2); p = r_circle_polygon();
        }
        else {
                // error
                W.set_state(0); p = r_circle_polygon();
        }

        W.set_mode(save);
        if (buf) W.start_buffering();

        return W;
}

window& operator>>(window& W, r_circle_polygon& p)
{
        W.set_tmp_label(">> r_circle_polygon");
        read(W, p);
        W.reset_frame_label();
        return W;
}


window& operator<<(window& W, const r_circle_polygon& gp)
{
        return draw(W, gp);
}

window& draw(window& W, const r_circle_polygon& p, color c)
{
        r_circle_segment cs;
        forall(cs, p.segments()) draw(W, cs, c);
        return W;
}


// Okay, the constant 10 is a hack, but we do not want to compute the area of large polygons!
#define AREA_TOO_SMALL_FOR_WIN(P,W) \
        ( (P).size() <= 10 && fp::abs((P).approximate_area()) * (W).scale() * (W).scale() < 2*2 )


void xor_bounded_reg_into_clip_mask(window& W, const r_circle_polygon& P)
{
        if (AREA_TOO_SMALL_FOR_WIN(P,W)) return;

#ifdef __unix__
        // slow drawing via approximation
        double dist = 0.5 / W.scale(); // error: at most half a pixel
        list<point> approx;
        r_circle_segment s;
        forall_segments(s, P) {
                list<point> approx_s = s.approximate(dist);
                approx_s.pop_back(); // remove target
                approx.conc( approx_s );
        }

        int n = approx.size();
        if (n == 0) return;
        double* X = new double[n];
        double* Y = new double[n];
        n = 0;
        point p;
        forall(p, approx) {
                X[n] = p.xcoord();
                Y[n] = p.ycoord();
                n++;
        }
        W.clip_mask_polygon(n,X,Y,3);
        delete[] X;
        delete[] Y;
#else
        // fast drawing with chord xors
        int n = P.size();
        if (n == 0) return;
        double* X = new double[n];
        double* Y = new double[n];
        n = 0;
        r_circle_point v;
        forall_vertices(v, P) {
                point p = v.to_float();
                X[n] = p.xcoord();
                Y[n] = p.ycoord();
                n++;
        }
        W.clip_mask_polygon(n,X,Y,3);
        delete[] X;
        delete[] Y;

        r_circle_segment s;
        forall_segments(s, P) {
                if (s.is_degenerate()) continue;
                point src = s.source().to_float(), tgt = s.target().to_float();
                point middle = s.real_middle().to_point();
                if ((src.distance(middle)+middle.distance(tgt))*W.scale() < 10) continue;
                point cen = s.center().to_float(); double rad = s.radius().to_close_double();
                if (s.orientation() < 0) leda_swap(src,tgt);
                W.clip_mask_chord(src.xcoord(), src.ycoord(), tgt.xcoord(), tgt.ycoord(),
                                                  cen.xcoord(), cen.ycoord(), rad, s.orientation()*P.orientation() < 0 ? 3 : 4);
        }
#endif
}

window& draw_filled(window& W, const r_circle_polygon& P, color c)
{
        if (AREA_TOO_SMALL_FOR_WIN(P,W)) return draw(W, P, c);

        double x0 = W.xmin();
        double y0 = W.ymin();
        double x1 = W.xmax();
        double y1 = W.ymax();

        if (W.is_buffering()) {
                x0 = W.xreal(0);
                y0 = W.yreal(W.height());
                x1 = W.xreal(W.width());
                y1 = W.yreal(0);
        }
        W.clip_mask_rectangle(x0, y0, x1, y1, P.orientation()<0 || P.is_full());

        xor_bounded_reg_into_clip_mask(W, P);

        W.draw_box(x0,y0, x1,y1, c);

        W.clip_mask_window(1);

        return W;
}


/// r_circle_gen_polygon //////////////////////////////////////

window& read(window& W, r_circle_gen_polygon& gp)
{
        bool buf = W.is_buffering();
        if (buf) W.stop_buffering();
        drawing_mode save = W.set_mode(xor_mode);

        list<r_circle_polygon> polys;

        gp = r_circle_gen_polygon();

        bool first_run = true;
        for(;;) {
                r_circle_polygon p;
                read(W, p);
                if (!W || W.get_state() == 2) break;
                if (!p.is_weakly_simple()) { W.set_state(0); break; }
                W << p; polys.append(p);
                if (p.orientation() == 1) gp = gp.unite(p);
                else {
                        if (first_run) gp = r_circle_gen_polygon(r_circle_gen_polygon::FULL);
                        gp = gp.diff(p.complement());
                }
                first_run = false;
        }

        r_circle_polygon p;
        forall(p, polys) W << p;

        W.set_mode(save);
        if (buf) W.start_buffering();

        return W;
}

window& operator>>(window& W, r_circle_gen_polygon& gp)
{
        W.set_tmp_label(">> r_circle_gen_polygon");
        read(W, gp);
        W.reset_frame_label();
        return W;
}


window& operator<<(window& W, const r_circle_gen_polygon& gp)
{
        return draw(W, gp);
}

window& draw(window& W, const r_circle_gen_polygon& gp, color c)
{
        r_circle_polygon p;
        forall(p, gp.polygons()) draw(W, p, c);
        return W;
}

window& draw_filled(window& W, const r_circle_gen_polygon& GP, color c)
{
        if (GP.is_empty()) return W;

        double x0 = W.xmin();
        double y0 = W.ymin();
        double x1 = W.xmax();
        double y1 = W.ymax();

        if (W.is_buffering()) {
                x0 = W.xreal(0);
                y0 = W.yreal(W.height());
                x1 = W.xreal(W.width());
                y1 = W.yreal(0);
        }
        W.clip_mask_rectangle(x0, y0, x1, y1, GP.orientation()<0 || GP.is_full());

        r_circle_polygon P;
        forall_polygons(P, GP) {
                xor_bounded_reg_into_clip_mask(W, P);
        }

        W.draw_box(x0,y0, x1,y1, c);

        W.clip_mask_window(1);

        forall_polygons(P, GP) {
                if (AREA_TOO_SMALL_FOR_WIN(P,W) && P.orientation()>0) draw(W, P, c);
        }

        return W;
}


LEDA_END_NAMESPACE
