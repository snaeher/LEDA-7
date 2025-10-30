/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _r_circle_polygon.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/r_circle_polygon.h>
#include <LEDA/geo/r_circle_gen_polygon.h>
#include <LEDA/geo/rat_polygon.h>
#include <LEDA/geo/polygon.h>
#include <LEDA/core/stack.h>
#include <LEDA/numbers/fp.h>

//------------------------------------------------------------------------------
// r_circle_polygon
//------------------------------------------------------------------------------


#if defined(LEDA_DEBUG)
#include <LEDA/system/assert.h>
#else
#undef assert
#define assert(cond) ((void)0)
#endif

LEDA_BEGIN_NAMESPACE 

r_circle_polygon::CHECK_TYPE r_circle_polygon::input_check_type = WEAKLY_SIMPLE;

r_circle_polygon::r_circle_polygon(KIND k)
{
        if (k == NON_TRIVIAL)  
        PTR = new rep;
        if (k == FULL) ptr()->Orientation = 1;
}

void r_circle_polygon::init(const list<r_circle_segment>& segs, 
                            CHECK_TYPE check, RESPECT_TYPE respect_orient)
{
  list<r_circle_segment> clean_segs = segs;

  bool closed_chain = clean_chain(clean_segs);

  int orient = orientation_of_weakly_simple_chain(clean_segs);

  if (respect_orient == RESPECT_ORIENTATION || orient >= 0)
     PTR = new rep(clean_segs, orient);
  else
     PTR = new rep(reverse_chain(clean_segs), +1);
  
  if (check != NO_CHECK) 
  { if (!closed_chain) 
       LEDA_EXCEPTION(1, "r_circle_polygon::init: not a closed chain");

    CHECK_TYPE check_res = check_chain_for_simplicity(clean_segs);

    if (check != check_res && check_res != SIMPLE) 
       LEDA_EXCEPTION(1,"simplicity check failed");
   }
}

/*
void r_circle_polygon::init(const list<r_circle_segment>& segs, 
                            CHECK_TYPE check, RESPECT_TYPE respect_orient)
{
  if (check == NO_CHECK) 
    { int orient = orientation_of_weakly_simple_chain(segs);
      if (respect_orient == RESPECT_ORIENTATION || orient >= 0)
         PTR = new rep(segs, orient);
      else
         PTR = new rep(reverse_chain(segs), +1);
     }
   else 
    { list<r_circle_segment> clean_segs = segs;
      if (!clean_chain(clean_segs)) 
      { clean_segs.clear();
        LEDA_EXCEPTION(1, "r_circle_polygon::init: not a closed chain");
       }

      CHECK_TYPE check_res = check_chain_for_simplicity(clean_segs);
      if (check != check_res && check_res != SIMPLE) 
      { clean_segs.clear();
        LEDA_EXCEPTION(1, "simplicity check failed");
       }

      int orient = orientation_of_weakly_simple_chain(clean_segs);
      if (orient < 0 && respect_orient == DISREGARD_ORIENTATION) 
      { clean_segs = reverse_chain(clean_segs); 
        orient = +1;
       }

      PTR = new rep(clean_segs, orient);
     }
}
*/


void r_circle_polygon::init(const list<rat_point>& L, 
                            CHECK_TYPE check, RESPECT_TYPE respect_orient)
{
  list<r_circle_segment> segs;

  if (L.size() >= 2) 
  { r_circle_point src = L.head();
    list_item it;
    forall_items(it, L) 
    { //list_item succ_it = L.succ(it);
      //r_circle_point tgt = succ_it ? r_circle_point(L[succ_it]) : 
                                 (segs.empty() ? src : segs.head().source());

      list_item succ_it = L.cyclic_succ(it);
      r_circle_point tgt = L[succ_it];
      if (src != tgt) segs.append(r_circle_segment(src,tgt));
      src = tgt;
     }
   }

  // we could optimize here ...
  init(segs, check, respect_orient);
}


r_circle_polygon::r_circle_polygon(const rat_polygon& Q,
                                   CHECK_TYPE check, 
                                   RESPECT_TYPE respect_orient)
{ init(Q.vertices(), check, respect_orient); }


r_circle_polygon::r_circle_polygon(const polygon& Q, 
                                   CHECK_TYPE check, 
                                   RESPECT_TYPE respect_orient, int prec)
{ list<rat_point> rat_vertices;
  point p;
  forall_vertices(p, Q) {
    rat_point rat_p(p, prec); rat_p.normalize();
    rat_vertices.append(rat_p);
  }
  init(rat_vertices, check, respect_orient);
}

r_circle_polygon::r_circle_polygon(const rat_circle& circ, 
                                   RESPECT_TYPE respect_orient)
{ rat_circle copy = circ;
  if (respect_orient == DISREGARD_ORIENTATION && circ.orientation() < 0)
     copy = copy.reverse();
  PTR = new rep(r_circle_segment::split_circle(copy), copy.orientation());
}



void r_circle_polygon::normalize()
{
        r_circle_segment s;
        forall(s, segments()) s.normalize();
}

bool r_circle_polygon::verify(bool report_errors) const
{
        bool ok = is_closed_chain();
        if (!ok && report_errors) {
                LEDA_EXCEPTION(1, "r_circle_polygon::verify: not a closed chain");
        }

        r_circle_segment s;
        forall(s, segments()) ok &= s.verify(report_errors);

        // TODO: check polygon specific invariants
        return ok;
}



list<r_circle_point> r_circle_polygon::vertices() const
{
        list<r_circle_point> result;
        r_circle_segment s;
        forall(s, segments()) result.append(s.source());        
        return result;
}


bool r_circle_polygon::is_convex() const
{
        if (is_trivial()) return true;

        if (orientation() == -1) return false;

        if (! is_simple()) return false;

        r_circle_segment prev_seg = segments().tail();

        r_circle_segment s;
        forall(s, segments()) {
                if (s.orientation() < 0) return false;

                // the tangent of prev_seg and s at s.source() must form a left_turn
                real_point pnt_on_tangent_at_src = s.target();
                if (! s.is_straight_segment()) {
                        pnt_on_tangent_at_src = s.tangent_at_src().point2();
                }
                real_line prev_tangent = prev_seg.tangent_at(s.source());
                if (leda::orientation(prev_tangent, pnt_on_tangent_at_src) < 0) return false;

                prev_seg = s;
        }
        
        return true;
}

list<r_circle_point> 
r_circle_polygon::intersection(const r_circle_segment& s) const
{
        list<r_circle_point> result;

        list<r_circle_point> inters;
        r_circle_segment poly_seg;
        forall(poly_seg, segments()) {
                inters = poly_seg.intersection(s);
                while (! inters.empty()) {
                        r_circle_point p = inters.pop();
                        if (p != poly_seg.target()) result.append(p);
                }
        }
        return result;
}

list<r_circle_point> 
r_circle_polygon::intersection(const rat_line& l) const
{
        list<r_circle_point> result;

        list<r_circle_point> inters;
        r_circle_segment poly_seg;
        forall(poly_seg, segments()) {
                inters = poly_seg.intersection(l);
                while (! inters.empty()) {
                        r_circle_point p = inters.pop();
                        if (p != poly_seg.target()) result.append(p);
                }
        }
        return result;
}


static void clip_handle_segment(const r_circle_segment& s, const rat_line& l,
                                                                int old_side, int new_side,
                                                                list<r_circle_segment>& result, bool& close,
                                                                const r_circle_point& crossing = r_circle_point())
/* 
        s crosses the line l at most once (in crossing)
        old_side: side of the previous segment near s.source
        new_side: side of the points in an environment of s.target
        PRECOND: At least one of the two values is non-zero!
        old_side == 0 => s.source() is on l and s does not cross l (except for maybe in s.source)
        new_side == 0 => s.target() is on l and s does not cross l (except for maybe in s.target)
*/ 
{
        assert(old_side != 0 || new_side != 0);

        if (old_side >= 0 && new_side >= 0) {
                // + => +  or  0 => +  or  + => 0  (stay on positive side)
                result.append(s);
                if (old_side == 0) close = true;
        }
        else if (old_side > 0 && new_side < 0) {
                // + => - (proper crossing)
                if (s.source() != crossing)
                        result.append(r_circle_segment(s.source(), crossing, s.circle())); // portion on pos. side
        }
        else if (old_side < 0 && new_side > 0) {
                // - => + (proper crossing)
                if (!result.empty()) {
                        // connect to last + => - crossing
                        r_circle_segment connector(result.tail().target(), crossing, l);
                        result.append(connector);
                }
                else close = true; // has to be closed later ...

                result.append(r_circle_segment(crossing, s.target(), s.circle())); // portion on pos. side
        }
}


r_circle_polygon 
r_circle_polygon::clip_against_halfplane(const rat_line& l) const
{
        if (is_full()) LEDA_EXCEPTION(1, "cannot clip the full polygon");
        if (is_trivial()) return *this;

        list<r_circle_segment> result;

        real_line rl(l);
        int side_src = rl.orientation(segments().head().source());
        int old_side = side_src;
/*      meaning of old_side:
        It is the side w.r.t. l, where inner points of previous (non-vertical) 
        segment lie.
        If the source of the first segment is on l, then old_side is initially 
        zero, i.e. unknown. It remains zero until we encounter the first 
        non-vertical segment. Once it is non-zero, it can never become zero again. 
        It changes its value whenever the chain crosses l properly.
*/

        bool close = false; // maybe we need to close the chain in the end

        r_circle_segment s; // half-open: source belongs to s, target does not
        forall(s, segments()) {
                int side_tgt = rl.orientation(s.target());
                if (s.is_straight_segment()) {
                        r_circle_point crossing = s.source();
                                // NOTE: crossing at src is possible if side_src == 0 and old_side != 0

                     if ((side_src > 0 && side_tgt < 0) || 
                         (side_src < 0 && side_tgt > 0)) 
                      {
                                // inner crossing
                                rat_point i;
                                l.intersection(s.supporting_line(), i);
                                crossing = r_circle_point(i);
                        }
                        if (old_side != 0 || side_tgt != 0)
                                clip_handle_segment(s, l, old_side, side_tgt, result, close, crossing);
                        // ow: only vertical segments on l so far
                }
                else {
                        assert(s.is_proper_arc());
                        two_tuple<r_circle_point, r_circle_point> inters;
                        if (r_circle_point::intersection(s.circle(), l, inters) == 2) { 
                                // the supporting circle crosses the line l twice
                                // check if these crossings points belong to the half-open s
                                int num_on_half_open = 2;

                                if ( !s.wedge_contains(inters.second()) || inters.second() == s.target() )
                                {
                                        --num_on_half_open;
                                }

                                if ( !s.wedge_contains(inters.first()) || inters.first() == s.target() )
                                {
                                        --num_on_half_open;
                                        inters.first() = inters.second();
                                }

                                if (num_on_half_open == 1) {
                                        int new_side = side_tgt != 0 ? side_tgt : old_side;
                                        if (new_side == 0) {
                                                assert(inters.first() == s.source() && old_side == 0);
                                                bool same_order = compare(s.source(),s.target()) == compare(l.point1(),l.point2());
                                                        // same_order == true <=> l passes first through s.src and then through s.tgt
                                                new_side = same_order ? -s.orientation() : s.orientation();
                                                inters.first() = s.source(); // speedup
                                        }

                                        clip_handle_segment(s, l, old_side, new_side, result, close, inters.first());

                                        side_src = side_tgt; old_side = new_side; continue;
                                }
                                else if (num_on_half_open == 2) {
                                        // two intersections -> order them along s
                                        if (side_src != 0) {
                                                if (leda::orientation(s.source(), inters.first(), inters.second()) != s.orientation())
                                                        leda_swap(inters.first(), inters.second());
                                        }
                                        else {
                                                if (s.source() == inters.second()) {
                                                        inters.second() = inters.first(); inters.first() = s.source(); // swap
                                                }
                                                else inters.first() = s.source(); // make identical for speedup
                                        }

                                        assert(side_tgt != 0);
                                        r_circle_segment part_one(s.source(), inters.second(), s.circle());
                                        r_circle_segment part_two(inters.second(), s.target(), s.circle());
                                        clip_handle_segment(part_one, l,  old_side, -side_tgt, result, close, inters.first());
                                        clip_handle_segment(part_two, l, -side_tgt,  side_tgt, result, close, inters.second());

                                        side_src = old_side = side_tgt; continue;
                                }
                        }

                        // no intersections (tangential intersections can BASICALLY be ignored)
                        clip_handle_segment(s, l, old_side, side_tgt, result, close, s.source());
                }

                side_src = side_tgt; if (side_tgt != 0) old_side = side_tgt;
        }

        if (close) {
                r_circle_point first_src = result.head().source(), last_tgt = result.tail().target();
                if (first_src != last_tgt) {
                        r_circle_segment connector(last_tgt, first_src, l);
                        result.append(connector);
                }
        }

        return r_circle_polygon(result, NO_CHECK);
        // NOTE: result does not have to be weakly simple
}

r_circle_polygon 
r_circle_polygon::translate(rational dx, rational dy) const
{
        if (is_trivial()) return *this;

        list<r_circle_segment> segs;
        r_circle_segment s;
        forall(s, segments()) segs.append(s.translate(dx, dy));
        return r_circle_polygon(segs, orientation());
}

r_circle_polygon 
r_circle_polygon::rotate90(const rat_point& q, int i) const
{
        if (is_trivial()) return *this;

        list<r_circle_segment> segs;
        r_circle_segment s;
        forall(s, segments()) segs.append(s.rotate90(q, i));
        return r_circle_polygon(segs, orientation());
}

r_circle_polygon 
r_circle_polygon::reflect(const rat_point& p, const rat_point& q) const
{
        if (is_trivial()) return *this;

        list<r_circle_segment> segs;
        r_circle_segment s;
        forall(s, segments()) segs.append(s.reflect(p, q));
        return r_circle_polygon(segs, r_circle_polygon::NO_CHECK); // orientation may change!
}

r_circle_polygon 
r_circle_polygon::reflect(const rat_point& p) const
{
        if (is_trivial()) return *this;

        list<r_circle_segment> segs;
        r_circle_segment s;
        forall(s, segments()) segs.append(s.reflect(p));
        return r_circle_polygon(segs, orientation());
}


real 
r_circle_polygon::sqr_dist(const real_point& p) const
{
        if (is_trivial()) return 0;

        list_item it = segments().first();
        real min_sd = segments()[it].sqr_dist(p);
        while ( (it = segments().succ(it)) ) {
                real sd = segments()[it].sqr_dist(p);
                if (sd < min_sd) {
                        min_sd = sd;
                        if (sign(min_sd)==0) break;
                }
        }
        return min_sd;
}


// r_circle_polygon::split_into_weakly_simple_parts() has been moved to _r_circle_gen_polygon.cpp
// r_circle_polygon::make_weakly_simple() has been moved to _r_circle_gen_polygon.cpp

r_circle_polygon 
r_circle_polygon::complement() const
{
        if (! is_trivial()) {
                return r_circle_polygon(reverse_chain(segments()), -orientation());
        }
        else if (is_empty()) return r_circle_polygon(FULL);
        else return r_circle_polygon(EMPTY);
}

r_circle_polygon 
r_circle_polygon::eliminate_cocircular_vertices() const
{
        if (is_trivial()) return *this;
        if (is_rat_circle()) {
                return r_circle_polygon(segments().head().circle());
        }

        list<r_circle_segment> segs = segments();
        list_item it;
        forall_items(it, segs) {
                list_item it_succ = segs.cyclic_succ(it);
                r_circle_segment s = segs[it], s_succ = segs[it_succ];
                if (s.circle() == s_succ.circle()) {
                        segs.del_item(it);
                        segs[it_succ] = r_circle_segment(s.source(), s_succ.target(), s.circle());
                }
        }
        return r_circle_polygon(segs, NO_CHECK);
}

r_circle_polygon 
r_circle_polygon::round(int prec) const
{
        list<r_circle_segment> segs;

        r_circle_segment s;
        forall(s, segments()) 
                segs.append( s.round(prec) );

        return r_circle_polygon(segs, NO_CHECK);
}

bool 
r_circle_polygon::is_rat_polygon() const
{
        if (is_full()) return false;

        r_circle_segment s;
        forall_segments(s, *this) 
                if (! s.is_rat_segment()) return false;
        return true;
}

rat_polygon 
r_circle_polygon::to_rat_polygon() const
{
        if (is_full()) LEDA_EXCEPTION(1, "r_circle_polygon is not a rat_polygon");

        list<rat_point> rat_verts;
        r_circle_segment s;
        forall_segments(s, *this) {
                if (! s.is_rat_segment()) {
                        LEDA_EXCEPTION(1, "r_circle_polygon is not a rat_polygon");
                }
                rat_verts.append(s.source().to_rat_point());
        }
        return rat_polygon(rat_verts);
}

rat_polygon r_circle_polygon::approximate_by_rat_polygon(double dist) const
{
  if (is_full()) LEDA_EXCEPTION(1, "r_circle_polygon is not a rat_polygon");

  list<rat_point> vertices;
  r_circle_segment s;
  forall_segments(s, *this) 
  { list<point> approx_pnts = s.approximate(dist);
    approx_pnts.pop_back(); // remove target (source of the next seg)
    point p; 
    forall(p, approx_pnts) vertices.append( rat_point(p) );
   }

  return rat_polygon(vertices, rat_polygon::NO_CHECK);
}

polygon 
r_circle_polygon::to_float() const
{
        list<point> float_verts;
        r_circle_segment s;
        forall(s, segments()) {
                if (! s.is_straight_segment()) {
                        LEDA_EXCEPTION(1, "r_circle_polygon is not a polygon");
                }
                float_verts.append(s.source().to_float());
        }
        return polygon(float_verts);
}

bool 
r_circle_polygon::is_rat_circle() const
{
        if (is_trivial()) return false;

        list_item it = segments().first();
        rat_circle circ = segments()[it].circle();
        if (circ.is_degenerate()) return false;

        while ( (it = segments().succ(it)) ) {
                if (circ != segments()[it].circle()) return false;
        }

        return true;
}

rat_circle 
r_circle_polygon::to_rat_circle() const
{
        if (! is_rat_circle()) {
                LEDA_EXCEPTION(1, "r_circle_polygon is not a circle");
                return rat_circle();
        }
        return segments().head().circle();
}


int 
r_circle_polygon::side_of(const r_circle_point& pnt) const
{
        switch (region_of(pnt)) {
        case ON_REGION:                 return 0;
        case BOUNDED_REGION:    return orientation();
        case UNBOUNDED_REGION:  if (is_empty()) return -1;
                                                        if (is_full()) return +1;
                                                        return -orientation();
        default:                                assert( 0 == 1 ); return 0;
        }
}

region_kind 
r_circle_polygon::region_of(const r_circle_point& pnt) const
{
/*
        As described in the LEDA book (pg. 769 ff) we consider the point q which
        is obtained by pertubing pnt slightly to the right and the vertical upward
        ray r that starts at q. We count the intersections of r with the boundary 
        of *this.
*/
        if (is_trivial()) return UNBOUNDED_REGION;

        // first we check the bounding box
        real xmin, ymin, xmax, ymax;
        bounding_box(xmin, ymin, xmax, ymax);
        if (pnt.xcoord()<xmin || pnt.xcoord()>xmax || pnt.ycoord()<ymin || pnt.ycoord()>ymax)
                return UNBOUNDED_REGION;

        real_line vert_line(pnt, real_point(pnt.xcoord(), pnt.ycoord()+1));
        rat_line vert_line_rat;
        bool pnt_is_rational = pnt.is_rat_point();
        if (pnt_is_rational) {
                rat_point pnt_rat = pnt.to_rat_point();
                integer X = pnt_rat.X(), Y = pnt_rat.Y(), W = pnt_rat.W();
                vert_line_rat = rat_line(pnt_rat, rat_point(X,Y+W,W));
        }

        two_tuple<real_point, real_point> inters;

        unsigned count = 0;

        r_circle_segment s;
        forall(s, segments()) {
                if (s.is_straight_segment()) {
                        // straight segment
                        // NOTE: we need no optimization for rat_point below, the 
                        //       predicates of r_circle_point will take care of that!

                        if (s.is_vertical_segment()) {
                                if (s.contains(pnt)) return ON_REGION; else continue;
                        }
                        int cmp_x_src_pnt = r_circle_point::cmp_x(s.source(), pnt);
                        int cmp_x_tgt_pnt = r_circle_point::cmp_x(s.target(), pnt);

                        if (cmp_x_src_pnt <= 0 && cmp_x_tgt_pnt > 0) {
                                int cmp_y = leda::orientation(s.source(), s.target(), pnt);
                                if (cmp_y == 0) return ON_REGION;
                                if (cmp_y < 0) ++count;
                        }
                        else if (cmp_x_tgt_pnt <= 0 && cmp_x_src_pnt > 0) {
                                int cmp_y = leda::orientation(s.target(), s.source(), pnt);
                                if (cmp_y == 0) return ON_REGION;
                                if (cmp_y < 0) ++count;
                        }
                        else if (cmp_x_src_pnt == 0 && r_circle_point::cmp_y(s.source(), pnt) == 0) return ON_REGION;
                        else if (cmp_x_tgt_pnt == 0 && r_circle_point::cmp_y(s.target(), pnt) == 0) return ON_REGION;
                }
                else {
                        // non-degenerate segment
                        assert(!s.is_degenerate() && !s.is_full_circle());

                        int num_inters;
                        if (pnt_is_rational)
                                num_inters = leda::intersection(s.circle(), vert_line_rat, inters);
                        else
                                num_inters = s._real_circle().intersection(vert_line, inters);

                        if (num_inters == 0) continue; // no intersection

                        if (num_inters == 1) {
                                // vert_line through pnt is a tangent to the supporting circle
                                real_point i_pnt = inters.first();
                                if (!s.wedge_contains(i_pnt)) continue;

                                int cmp_y_pnt_inter = real_point::cmp_y(pnt, i_pnt);
                                if (cmp_y_pnt_inter == 0) return ON_REGION;
                                if (cmp_y_pnt_inter > 0) continue; 
                                        // i_pnt below pnt => r does not intersect s
                                if (real_point::cmp_x(s.real_center(), i_pnt) < 0) continue; 
                                        // right tangent => r does not intersect s

                                // Now we know that vert_line is a left tangent to s.circle()
                                if (real_point::cmp_x(i_pnt, s.source()) == 0 || 
                                        real_point::cmp_x(i_pnt, s.target()) == 0 ) 
                                {
                                        assert(i_pnt == s.source() || i_pnt == s.target());
                                         // if i_pnt happens to be an endpoint of s, then r intersects
                                        ++count;
                                }
                                else count += 2; // i_pnt is an inner point of s => r intersects s twice
                        }
                        else {
                                // vert_line through pnt intersects the supporting circle twice
                                real_point i1 = inters.first(), i2 = inters.second();
                                assert( real_point::cmp_y(i1,i2) < 0 );
                                int cmp_y_pnt_i2 = real_point::cmp_y(pnt, i2);
                                if (cmp_y_pnt_i2 > 0) continue; // pnt lies above i1 and i2
                                bool i2_on_s = s.wedge_contains(i2);
                                if (cmp_y_pnt_i2 == 0 && i2_on_s) return ON_REGION;
                                int cmp_y_pnt_i1 = real_point::cmp_y(pnt, i1);
                                bool i1_on_s = s.wedge_contains(i1);
                                if (cmp_y_pnt_i1 == 0 && i1_on_s) return ON_REGION;

                                bool s_inters_unpertubed_ray_in_i1 = i1_on_s && cmp_y_pnt_i1 <= 0;
                                bool s_inters_unpertubed_ray_in_i2 = i2_on_s && cmp_y_pnt_i2 <= 0;
                                if (!s_inters_unpertubed_ray_in_i1 && !s_inters_unpertubed_ray_in_i2) continue;

                                bool src_on_vert_line = real_point::cmp_x(pnt, s.source()) == 0;
                                bool tgt_on_vert_line = real_point::cmp_x(pnt, s.target()) == 0;

                                if (s_inters_unpertubed_ray_in_i1) {
                                        if (src_on_vert_line && real_point::cmp_y(i1, s.source()) == 0) {
                                                if (s.orientation() == +1) ++count;
                                        }
                                        else if (tgt_on_vert_line && real_point::cmp_y(i1, s.target()) == 0) {
                                                if (s.orientation() == -1) ++count;
                                        }
                                        else ++count;
                                }

                                if (s_inters_unpertubed_ray_in_i2) {
                                        if (src_on_vert_line && real_point::cmp_y(i2, s.source()) == 0) {
                                                if (s.orientation() == -1) ++count;
                                        }
                                        else if (tgt_on_vert_line && real_point::cmp_y(i2, s.target()) == 0) {
                                                if (s.orientation() == +1) ++count;
                                        }
                                        else ++count;
                                }
                        } // eof two inters
                } // eof non-degenerate
        } // eof forall

        return (count % 2 == 0) ? UNBOUNDED_REGION : BOUNDED_REGION;
}

double 
r_circle_polygon::approximate_area() const
{
  if (is_full()) LEDA_EXCEPTION(1,"area: cannot compute area of full polygon");

  real area_straight = 0; 

  // area of polygon obtained by making all edges straight
  // fix for autodesk
  double areax = 0;
  if (segments().size() >= 3) 
  { list_item it  = segments().get_item(1);
     r_circle_point p = segments()[it].source();
     for(it = segments().succ(it); it; it = segments().succ(it)) 
     { r_circle_segment s = segments()[it];
	   // fix for autodesk
       // area_straight += area(p, s.source(), s.target());
	   area_straight = area(p, s.source(), s.target());
	   areax += area_straight.to_close_double();
      }
   }

   // fix for autodesk  
   // double area = area_straight.to_close_double();

   r_circle_segment s;
   forall(s, segments())  
   { double a = s.approximate_area();
     areax += a;
    }


   if (fp::sign(areax) != orientation()) areax = 0;

   return areax;
}

void 
r_circle_polygon::bounding_box(real& xmin, real& ymin, real& xmax, real& ymax) const
{
        rep::BBox* ptr_box = ptr()->pBoundingBox;
        if (ptr_box) {
                xmin = ptr_box->xmin; ymin = ptr_box->ymin; xmax = ptr_box->xmax; ymax = ptr_box->ymax;
        }
        else {
                if (is_trivial()) {
                        LEDA_EXCEPTION(1, "cannot compute bounding box of trivial polygon");
                        return;
                }

                list_item it = segments().first();
                segments()[it].compute_bounding_box(xmin, ymin, xmax, ymax);
                while ((it = segments().succ(it))) {
                        real _xmin, _ymin, _xmax, _ymax;
                        segments()[it].compute_bounding_box(_xmin, _ymin, _xmax, _ymax);
                        if (_xmin < xmin) xmin = _xmin;
                        if (_ymin < ymin) ymin = _ymin;
                        if (_xmax > xmax) xmax = _xmax;
                        if (_ymax > ymax) ymax = _ymax;
                }

                ptr()->pBoundingBox = ptr_box = new rep::BBox;
                ptr_box->xmin = xmin; 
                ptr_box->ymin = ymin; 
                ptr_box->xmax = xmax; 
                ptr_box->ymax = ymax;
        }
}

void 
r_circle_polygon::bounding_box(double& xmin, double& ymin, 
                                                           double& xmax, double& ymax) const
{
        real xmin_r, ymin_r, xmax_r, ymax_r;

        bounding_box(xmin_r, ymin_r, xmax_r, ymax_r);

        xmin = xmin_r.get_double_lower_bound();
        ymin = ymin_r.get_double_lower_bound();
        xmax = xmax_r.get_double_upper_bound();
        ymax = ymax_r.get_double_upper_bound();
}

bool
r_circle_polygon::is_closed_chain(const list<r_circle_segment>& segs)
/*
        Checks whether the segments form a closed chain (possibly self-intersecting)
*/
{
        if (segs.empty()) return true;
        if (segs.size() == 1) {
                r_circle_segment s = segs.head();
                return s.is_full_circle();
        }

        r_circle_point prev_tgt = segs.tail().target();
        r_circle_segment s;
        forall(s, segs) {
                if (s.is_full_circle()) return false;
                if (prev_tgt != s.source()) return false;
                prev_tgt = s.target();
        }
        return true;
}

bool
r_circle_polygon::clean_chain(list<r_circle_segment>& segs)
{
 /* - If the chain is a single full circle, it is split in two segments.
    - Removes trivial segments from the chain
    - Checks that the segments form a closed chain (possibly self-intersecting)
  */

  if (segs.empty()) return true;

  if (segs.size() == 1) 
  { r_circle_segment s = segs.head();
    if (! s.is_full_circle()) return false;
    rat_circle circ = s.circle();
    segs = r_circle_segment::split_circle(circ);
    return true;
   }

  r_circle_point prev_tgt = segs.tail().target();
  list_item it;
  forall_items(it, segs) 
  { r_circle_segment s = segs[it];
    if (s.is_full_circle()) return false;
    if (prev_tgt != s.source()) return false;
    if (s.is_trivial()) { segs.del_item(it); }
    prev_tgt = s.target();
  }

  return segs.size() >= 2;
}


static r_circle_polygon::CHECK_TYPE
create_and_check_map_for_simplicity(const list<r_circle_segment>& segs,
                                    GRAPH<r_circle_point,r_circle_segment>& G)
{
  // we assume that segs consists of one or more closed(!) chains.

  if (segs.empty()) return r_circle_polygon::SIMPLE;

  SWEEP_SEGMENTS(segs, G, true);

  if (G.number_of_edges() != 2*segs.size()) 
    return r_circle_polygon::NOT_WEAKLY_SIMPLE;

  // check for overlapping segments
  edge e;
  forall_edges(e, G) 
  { edge f = G.cyclic_adj_succ(e);
    if (target(e) == target(f) && G[e].overlaps(G[f],G[source(e)],G[target(e)]))
         return r_circle_polygon::NOT_WEAKLY_SIMPLE;
   }

  // short-cut for SIMPLE if all nodes have outdegree 2 ...
  if (G.number_of_nodes()*2 == G.number_of_edges()) 
    return r_circle_polygon::SIMPLE;

   return r_circle_polygon::NO_CHECK;
}



r_circle_polygon::CHECK_TYPE r_circle_polygon::check_chain_for_simplicity(
                                   const list<r_circle_segment>& chain, 
                                   list<r_circle_point>* crossings)
{

  GRAPH<r_circle_point,r_circle_segment> G;
  CHECK_TYPE check = create_and_check_map_for_simplicity(chain, G);

  if (check == SIMPLE || (check != NO_CHECK && crossings == nil)) 
     return check;


  if (crossings) crossings->clear();

  // Note: If the same segment (i.e. identical id) appears twice in the 
  // chain the crossings are not guaranteed to be correct!

  map<r_circle_segment, list_item> pos_in_chain(0);

  list_item it;
  forall_items(it, chain) pos_in_chain[ chain[it] ] = it;

  check = SIMPLE;
  stack<list_item> S;
  node n;
  forall_nodes(n, G) 
  { 
    if (G.outdeg(n) == 2) continue; 

    if (check == SIMPLE) check = WEAKLY_SIMPLE;

     edge e;
     forall_out_edges(e,n) 
     { r_circle_segment e_seg = G[e];
       list_item e_pos = pos_in_chain[e_seg];
       list_item corresp_e_pos 
                         = (e_seg.target() == G[n]) ? chain.cyclic_succ(e_pos)
                                                    : chain.cyclic_pred(e_pos);
       if (S.empty() || S.top() != corresp_e_pos)
          S.push(e_pos);
       else 
          S.pop();
      }

      if (!S.empty()) 
      { // crossing
        if (!crossings) return NOT_WEAKLY_SIMPLE;
        check = NOT_WEAKLY_SIMPLE;
        crossings->append(G[n]); 
        S.clear();
       }
   }

  return check;
}

r_circle_polygon::CHECK_TYPE r_circle_polygon::check_chains_for_simplicity(
                             const list<r_circle_polygon>& polys, 
                             list<r_circle_point>* crossings)
{
  if (crossings) crossings->clear();

  if (polys.size() == 0) return SIMPLE;

  if (polys.size() == 1) 
      return check_chain_for_simplicity(polys.head().segments(), crossings);

  list<r_circle_segment> all_segs;
  r_circle_polygon P;
  forall(P, polys) 
  { list<r_circle_segment> segs = P.segments();
    all_segs.conc(segs);
  }

  GRAPH<r_circle_point,r_circle_segment> G;
  CHECK_TYPE check = create_and_check_map_for_simplicity(all_segs, G);
  if (check == SIMPLE || (check != NO_CHECK && crossings == nil)) return check;

  // Note: If the same segment (i.e. identical id) appears twice
  //       the crossings are not guaranteed to be correct!

  map<r_circle_segment, int> pos_in_chain;
  array<int> pred_in_chain(all_segs.size()); 
  array<int> succ_in_chain(all_segs.size());

  int pos = 0;
  forall(P, polys) 
  { int first_pos = pos;
    int last_pos  = first_pos + P.size() - 1;
    r_circle_segment s;
    forall_segments(s, P) 
    { pos_in_chain[s] = pos;
      pred_in_chain[pos] = (pos != first_pos) ? (pos - 1) : last_pos;
      succ_in_chain[pos] = (pos != last_pos)  ? (pos + 1) : first_pos;
      ++pos;
     }
   }

  check = SIMPLE;
  stack<int> S;
  node n;
  forall_nodes(n, G) 
  { 
    if (G.outdeg(n) == 2) continue; 

    if (check == SIMPLE) check = WEAKLY_SIMPLE;

    edge e;
    forall_out_edges(e, n) 
    { r_circle_segment e_seg = G[e];
      int e_pos = pos_in_chain[e_seg];
      int corresp_e_pos = e_seg.target() == G[n] ? succ_in_chain[e_pos]
                                                 : pred_in_chain[e_pos];
      if (S.empty() || S.top() != corresp_e_pos)
        S.push(e_pos);
      else 
        S.pop();
     }

    if (!S.empty()) 
    { // crossing
      if (!crossings) return NOT_WEAKLY_SIMPLE;
      check = NOT_WEAKLY_SIMPLE;
      crossings->append(G[n]); 
      S.clear();
     }
   }

  return check;
}


list<r_circle_segment> 
r_circle_polygon::reverse_chain(const list<r_circle_segment>& segs)
{
        list<r_circle_segment> reversed_segs;
        r_circle_segment s;
        forall(s, segs) reversed_segs.push_front(s.reverse());
        return reversed_segs;
}

int 
r_circle_polygon::orientation_of_weakly_simple_chain(const list<r_circle_segment>& segs)
{
        if (segs.empty()) return 0;

        // We look for the lex. smallest point p on the chain segs.
        // (Note that this point does not have to be an endpoint of a segment!)
        // We cannot use the lex. smallest vertex: 
        //   Consider Fig. 10.66 on pg. 769 in the LEDA book and assume that there
        //   is a circular segment that starts in r, has center r and ends in p.
        //   Then the polygon is negatively oriented!!!
        // NOTE:
        //   If the lex. smallest point p is the source of some segment, it may the
        //   source of several segments in segs, because the polygon is weakly 
        //   simple, but not necessarily simple. However, any bend at p has the
        //   same orientation, so we can choose an arbitrary one!

        list_item lex_smallest_it = segs.first();
        r_circle_point lex_smallest = segs[lex_smallest_it].source();

        list_item it;
        forall_items(it, segs) {
                r_circle_segment cur = segs[it];
                r_circle_point lex_smallest_cur = cur.lex_smallest_point();
                if (lex_smallest_cur == cur.target()) continue; // ignore targets (for speed-up)
                if (compare(lex_smallest_cur, lex_smallest) < 0) {
                        lex_smallest = lex_smallest_cur; lex_smallest_it = it;
                }
        }

        r_circle_segment s = segs[lex_smallest_it];
        r_circle_segment s_pred = segs[segs.cyclic_pred(lex_smallest_it)];

        return orientation_at_lex_smallest_bend(s_pred, lex_smallest, s);
}

int 
r_circle_polygon::orientation_at_lex_smallest_bend(const r_circle_segment& s_pred, 
                                                                                                   const r_circle_point& lex_smallest, 
                                                                                                   const r_circle_segment& s)
{
        // check first if both are straight
        if (s.is_straight_segment() && s_pred.is_straight_segment()) {
                int orient = leda::orientation(s_pred.source(), lex_smallest, s.target());
                return orient;
        }

        // now at least one segment is curved
        if (lex_smallest != s.source() /* && lex_smallest != s.target() */) {
                // lex_smallest is an inner point (of a non-degenerate segment)
                assert(s.orientation() != 0);
                return s.orientation();
        }

        // otherwise we compare tangent slopes ...
        real_line t_s      = s.tangent_at_src(); assert(t_s.point1() == s.source());
        real_line t_s_pred = s_pred.tangent_at(s.source());
        real_point p = t_s.point2();

        int orient = leda::orientation(t_s_pred, p);
        if (orient) return orient;

        // tangents are identical
        assert(s_pred.orientation() != 0 || s.orientation() != 0);
        if (s_pred.orientation() != - s.orientation()) {
                // the segements lie on different sides of the normal
                // (observe that s_pred ends in s.source() while s starts there!)
                if (s_pred.orientation()) return s_pred.orientation();
                else                      return s.orientation();
        }
        else {
                // both segments lie on the same side of the normal
                int ori = compare(s.sqr_radius(), s_pred.sqr_radius()) * s.orientation();
                return ori;
        }
}

void 
r_circle_polygon::write(ostream& out, const r_circle_polygon& p)
{
        out << p.kind();
        if (! p.is_trivial()) {
                out << " " << p.segments().size() << " ";
                p.segments().print(out);
        }
        out << endl;
}

void 
r_circle_polygon::read(istream& in, r_circle_polygon& p, 
                                    CHECK_TYPE chk, RESPECT_TYPE resp_or)
{
        int _k;
        in >> _k;
        r_circle_polygon::KIND k = r_circle_polygon::KIND(_k);

        if (k != r_circle_polygon::NON_TRIVIAL) {
                p = r_circle_polygon(k);
        }
        else {
                char c;
                while (in.get(c) && is_space(c));
                in.putback(c);
                int sz; in >> sz; if (!in) return;

                list<r_circle_segment> L;
                for(int i = 0; i < sz; ++i) { 
                        while (in.get(c) && is_space(c));
                        if (!in) break;
                        in.putback(c);

                        r_circle_segment s;
                        in >> s;
                        L.append(s);
                }

                p = r_circle_polygon(L, chk, resp_or);
        }
}

ostream& operator<<(ostream& out, const r_circle_polygon& p)
{ r_circle_polygon::write(out, p); 
  return out;
} 

istream& operator>>(istream& in,  r_circle_polygon& p)
{ r_circle_polygon::read(in, p, r_circle_polygon::input_check_type); 
  return in;
}

//------------------------------------------------------------------------------
// buffering
//------------------------------------------------------------------------------

#define PRINT(x)
#define DRAW(x,y)

static const int prec = 64;

static double approx_area(const r_circle_polygon& P)
{
  assert(!P.is_full());

  real area_straight = 0;
  // fix for Autodesk
  double As = 0;

  list<r_circle_segment> seglist = P.segments();
  if (seglist.size() >= 3) 
  { list_item it = seglist.get_item(1);
    r_circle_point p = seglist[it].source();
    for(it = seglist.succ(it); it; it = seglist.succ(it)) 
    { r_circle_segment s = seglist[it];
	  // fix for Autodesk
      // area_straight += area(p,s.source(),s.target());
	  area_straight = area(p,s.source(),s.target());
	  As += area_straight.to_close_double();
     }
  }
  // fix for Autodesk
  // double As = area_straight.to_close_double();

  r_circle_segment s;
  forall(s,seglist) As += s.approximate_area();

  //assert(fp::sign(A) == P.orientation());

  if (fp::sign(As) != P.orientation()) 
  { 
    //A = -A;
    As = 0;
/*
    //if (seglist.size() > 10)
    { file_ostream os(string("poly_err.%d",seglist.size()));
      cout << string("A = %.5f  As = %.5f orient = %d   size = %d",
                      A,As,P.orientation(),seglist.size()) << endl;
      os << r_circle_gen_polygon(P.complement()) << endl;
     }
*/
   }

  return As;
}


/*
static double approx_area(const r_circle_gen_polygon& P)
{ assert(!P.is_full());
  double area = 0;
  r_circle_polygon poly;
  forall(poly,P.polygons()) area += approx_area(poly);
  return area;
}
*/



/*
static r_circle_segment construct_arc(const rat_point& a, const rat_point& b,
                                                          const rat_point& c,
                                                          double delta)
{ // arc from a to b with center c
  rat_point p = midpoint(a,b);
  vector vec = delta*((p-c).to_float().norm());
  rat_point q = rat_point(c.to_float().translate(vec),prec);
  return r_circle_segment(a,q,b); 
}
*/

static bool construct_arc(r_circle_segment& rcs, const rat_point& a, const rat_point& b,
                                                          const rat_point& c,
                                                          double delta)
{ // arc from a to b with center c
  rat_point p = midpoint(a,b);

  vector vec = delta*((p-c).to_float().norm()); 
  rat_point q = rat_point(c.to_float().translate(vec),prec);
  if((a == q)||(b == q))
	  return false;
  /*
  cout << "bca a " << a << endl;
  cout << "bca b " << b << endl;
  cout << "bca c " << c << endl;
  cout << "bca q " << q << endl;
  */

  rcs = r_circle_segment(a,q,b);
  return true;
}
static void construct_arc_poly2(list<r_circle_polygon>& parts,
                                           const rat_point& a, 
                                           const rat_point& b,
                                           const rat_point& c,
                                           double delta, double eps)
{ 
  if (a.sqr_dist(b) < eps) return;

  list<r_circle_segment> L;
  L.append(r_circle_segment(a,b,c));
  L.append(r_circle_segment(c,a));
  
  r_circle_polygon P(L);
  double A = approx_area(P);

  if (A < 0) P = P.complement(); 

  if (fabs(A) > eps) 
  { parts.append(P);
    DRAW(P,orange);
   }
}



/*
static void construct_arc_poly(list<r_circle_polygon>& parts,
                                          const rat_point& a, 
                                          const rat_point& b,
                                          const rat_point& c,
                                          double delta, double eps)
{ 
  if (a.sqr_dist(b) < eps) return;

  list<r_circle_segment> L;
  L.append(construct_arc(a,b,c,fabs(delta)));
  L.append(r_circle_segment(b,a));
  
  r_circle_polygon P(L);
  double A = approx_area(P);

  if (A < 0) P = P.complement(); 

  if (fabs(A) > eps) 
  { parts.append(P);
    DRAW(P,orange);
   }
}
*/

static void construct_safe_arc_poly(list<r_circle_polygon>& parts,
                                          const rat_point& a, 
                                          const rat_point& b,
                                          const rat_point& c,
                                          double delta, double eps)
{ 
  if (a.sqr_dist(b) < eps) return;

  list<r_circle_segment> L;
  r_circle_segment rcs;
  if(construct_arc(rcs,a,b,c,fabs(delta)))
	  L.append(rcs);
  else
	  return;
  L.append(r_circle_segment(b,a));
  
  r_circle_polygon P(L);
  double A = approx_area(P);

  if (A < 0) P = P.complement(); 

  if (fabs(A) > eps) 
  { parts.append(P);
    DRAW(P,orange);
   }
}

static void construct_rectangle(list<r_circle_polygon>& parts,
                                           const rat_point& a,
                                           const rat_point& b,
                                           const rat_point& c, 
                                           const rat_point& d,
                                           double delta, double eps)
{ 
  list<r_circle_segment> L;
  L.append(r_circle_segment(a,b));
  L.append(r_circle_segment(b,c));
  L.append(r_circle_segment(c,d));
  L.append(r_circle_segment(d,a));
  r_circle_polygon P(L,r_circle_polygon::NO_CHECK);
  DRAW(P,green2);
  assert(P.is_weakly_simple());
  if (P.orientation() < 0) P = P.complement();
  parts.append(P);
}



static void construct_triangle(list<r_circle_polygon>& parts,
                               const rat_point& a, const rat_point& b,
                               const rat_point& c, double delta, double eps)
{ 
  if (orientation(a,b,c) == 0) return;

  list<r_circle_segment> L;
  L.append(r_circle_segment(a,b));
  L.append(r_circle_segment(b,c));
  L.append(r_circle_segment(c,a));
  r_circle_polygon P(L,r_circle_polygon::NO_CHECK);
  DRAW(P,green2);
  assert(P.is_weakly_simple());
  if (P.orientation() < 0) P = P.complement();
  parts.append(P);
}


static void construct_arc_rectangle(list<r_circle_polygon>& parts,
                const rat_point& a1, const rat_point& b1, const rat_point& c1, 
                const rat_point& a2, const rat_point& b2, const rat_point& c2, 
                double delta, double eps)
{ 
  list<r_circle_segment> L;
  L.append(r_circle_segment(a1,b1,c1));
  L.append(r_circle_segment(c1,c2));
  L.append(r_circle_segment(c2,b2,a2));
  L.append(r_circle_segment(a2,a1));
  r_circle_polygon P(L,r_circle_polygon::NO_CHECK);
  DRAW(P,orange);
  assert(P.is_weakly_simple());
  if (P.orientation() < 0) P = P.complement();
  parts.append(P);
}



inline vector normalize_vector(const vector& vec, double eps)
{ if (vec.length() > eps)
    return vec.norm();
  else
    return vector(0,0);
}


r_circle_gen_polygon r_circle_polygon::buffer(double delta) const
{
  const r_circle_polygon& P = *this;
  
  if (P.empty()) return P;

  if (delta < 0)
  { double xmin,ymin,xmax,ymax;
    P.bounding_box(xmin,ymin,xmax,ymax);
    double diag = hypot(xmax-xmin,ymax-ymin);
    if (delta < -0.5*diag) return r_circle_gen_polygon();
  }

  double area_P = approx_area(P);

#if defined(__DMC__)
  double eps = sqrt(fabs(area_P))/1e6;
#else
  double eps = std::sqrt(fabs(area_P))/1e6;
#endif

  if (fabs(delta) < eps) return P;

  list<r_circle_segment> S = P.segments();

  assert(S.length() > 1);

  list<r_circle_polygon> parts;
  list<r_circle_polygon> arc_parts;

  rat_point last_point;
  rat_segment first_seg;
  int count = 0;

  r_circle_segment s;
  forall(s,S) 
  { 
    rat_point a = rat_point(s.source().to_float(),prec);
    rat_point c = rat_point(s.target().to_float(),prec);

    // construct buffer around each segment

    list<r_circle_segment> L;

    if (s.is_degenerate()) 
    { 
      PRINT("degenerate");
/*
      vector vec = delta*((a-c).to_float().rotate90().norm());
*/

      vector vec = delta*normalize_vector((a-c).to_float().rotate90(),eps);

      rat_point a1 = a.translate(rational(vec[0]),rational(vec[1])); 
      rat_point c1 = c.translate(rational(vec[0]),rational(vec[1])); 
      rat_point a0 = a1.rotate90(a, (delta >0) ? -1 : +1);
      rat_point c0 = c1.rotate90(c, (delta >0) ? +1 : -1);

      construct_rectangle(parts,a,a1,c1,c,delta,eps);

      int orient = 0;
      //if (count > 0) orient = orientation(a,a1,last_point);
      if (count > 0) orient = a.orientation(a1,last_point);

      //if (orient >= 0 && delta > 0 || orient <=0 && delta < 0)

      if ((orient < 0 && delta > 0) || (orient > 0 && delta < 0))
      { construct_triangle(parts,last_point,a1,a,delta,eps);
        construct_safe_arc_poly(arc_parts,last_point,a1,a,delta,eps);
       }

      last_point = c1;
      if (count == 0) first_seg = rat_segment(a,a1);
    }
  else
    { double d = delta;
      if (s.circle().orientation() < 0) d = -delta;

      PRINT("non degenerate");

      rat_point b = rat_point(s.real_middle().to_float(),prec);
      rat_point cen = s.center();

/*
      vector a_vec = d*((a - cen).to_float().norm());
      vector b_vec = d*((b - cen).to_float().norm());
      vector c_vec = d*((c - cen).to_float().norm());
      vector ac_vec= d*((a - c).to_float().norm());
*/
      vector a_vec = d * normalize_vector((a - cen).to_float(),eps);
      vector b_vec = d * normalize_vector((b - cen).to_float(),eps);
      vector c_vec = d * normalize_vector((c - cen).to_float(),eps);
      vector ac_vec= d * normalize_vector((a - c).to_float(),eps);

      rat_point a1 = a.translate(rational(a_vec[0]),rational(a_vec[1])); 
      rat_point b1 = b.translate(rational(b_vec[0]),rational(b_vec[1])); 
      rat_point c1 = c.translate(rational(c_vec[0]),rational(c_vec[1])); 

      rat_segment s1(c1,c);
      rat_segment s2(a1,a);

      double d1 = a1.sqr_dist(c1).to_float();
      //double d2 = a.sqr_dist(c).to_float();

      bool inter = s1.intersection(s2);

      if (inter || d1 < eps /* || d2 < eps */)
      { 

        PRINT("wedge");

        //construct_rectangle(parts,c,a,c1,a1,delta,eps);

        rat_point mid = midpoint(a1,c1);

        rat_point p = (inter) ? mid : cen;
        construct_triangle(parts,p,c,a,delta,eps);
        construct_arc_poly2(arc_parts,c,b,a,delta,eps);

        if (count > 0)
        { 
			construct_triangle(parts,p,a,last_point,delta,eps);
			construct_safe_arc_poly(arc_parts,last_point,p,a,delta,eps);
        }

        last_point = p;

        if (count == 0) first_seg = rat_segment(a,p);
       }
      else
       { 
         PRINT("arc");

         construct_arc_rectangle(arc_parts,a1,b1,c1,a,b,c,delta,eps);

         int orient = 0;
         //if (count > 0) orient = orientation(a,a1,last_point);
         if (count > 0) orient = a.orientation(a1,last_point);

         if (/*a1.sqr_dist(last_point) > fabs(eps) && */
            ((orient < 0 && delta > 0) || (orient > 0 && delta < 0)))
         { construct_triangle(parts,a1,a,last_point,delta,eps);
           construct_safe_arc_poly(arc_parts,last_point,a1,a,delta,eps);
          }

         last_point = c1;
         if (count == 0) first_seg = rat_segment(a,a1);
        }

      }

    count++;
  }


  PRINT("close chain");

  int orient = first_seg.orientation(last_point);

  if ((orient < 0 && delta > 0) || (orient > 0 && delta < 0))
  { list<r_circle_segment> L;
    rat_point a = last_point;
    rat_point b = first_seg.target();
    rat_point c = first_seg.source();
    construct_triangle(parts,a,b,c,delta,eps);
    construct_safe_arc_poly(arc_parts,a,b,c,delta,eps);
   }
 

  list<r_circle_gen_polygon> gparts;
  r_circle_polygon rcp;
  forall(rcp,parts) gparts.append(r_circle_gen_polygon(rcp.make_weakly_simple()));

/*
cout << string("union of %d rational parts", parts.size()) << endl; 
float t = used_time();
*/

  r_circle_gen_polygon buf = r_circle_gen_polygon::unite(gparts);

/*
cout << string("buf = %.2f  (%.2f sec)",approx_area(buf),used_time(t)) << endl;
*/

  gparts.clear();
  gparts.append(buf);
  forall(rcp,arc_parts) gparts.append(r_circle_gen_polygon(rcp));

/*
cout << string("unite with %d real parts", arc_parts.size()) << endl; 
*/

  buf = r_circle_gen_polygon::unite(gparts);

/*
cout << string("buf = %.2f  parts = %d (%.2f sec)", 
                approx_area(buf),buf.polygons().size(),used_time(t));
cout << endl;
*/

  list_item it_min = 0;
  double A_min = 0;
  list<r_circle_polygon> result;

  r_circle_polygon pol;
  forall(pol,buf.polygons())
  { assert(pol.is_weakly_simple());
    if (delta < 0) pol = pol.complement();
    double A = approx_area(pol);
    if (fabs(A) < eps) continue;
     result.append(pol);
     if (A < A_min) { it_min = result.last(); A_min = A; }
   }

  if (it_min) result.del_item(it_min);

  r_circle_gen_polygon Q(result);

  if (delta < 0) 
     Q = Q.intersection(P);
  else
     Q = Q.unite(P);

  return Q;
}


LEDA_END_NAMESPACE 
