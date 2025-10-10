/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _r_circle_segment.c
+
+
+  Copyright (c) 1995-2016
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/r_circle_segment.h>
#include <LEDA/geo/real_circle.h>
#include <LEDA/geo/real_line.h>
/*
#include <LEDA/system/timer.h>
#include <LEDA/system/counter.h>
*/

//------------------------------------------------------------------------------
// r_circle_segment
//
// S. Thiel (2004)
//------------------------------------------------------------------------------

#if defined(LEDA_DEBUG)
#include <LEDA/system/assert.h>
#else
#undef assert
#define assert(cond) ((void)0)
#endif

LEDA_BEGIN_NAMESPACE 

/// r_circle_segment /////////////////////////////////////////////////////////////////////////

/*
representation:
 - proper non-degenerate arc:  source() != target(), !circle().is_degenerate()
 - trivial curve:              identical(source(),target()), circle() == rat_circle()
 - non-degenerate full circle: identical(source(),target()), !circle().is_degenerate()
 - non-triv. straight line segment: source() != target(), circle().is_line()
   compare(source(),target()) == compare(circle().point1(),circle().point3())
*/



// changed by sn 12/2007 
// constructors create a legal representation even if errors occur
// (important when throwing exceptions)


r_circle_segment::r_circle_segment(const r_circle_point& src, 
                                   const r_circle_point& tgt, 
                                   const rat_circle& c)
{
  //assert(src != tgt); 
  //assert(!c.is_trivial());

  if (src == tgt || c.is_trivial())
  { init_as_trivial(src);
    LEDA_EXCEPTION(1,"r_circle_segment::r_circle_segment: illegal arguments");
   }

  if ( !c.is_line() || compare(src,tgt) == compare(c.point1(),c.point3()) )
    PTR = new rep(src, tgt, c);
  else
    PTR = new rep(src, tgt, c.reverse());

//  if(!_real_circle().contains(src) || !_real_circle().contains(tgt))
//	  LEDA_EXCEPTION(1,"r_circle_segment::r_circle_segment: illegal arguments");

  assert(_real_circle().contains(src) && _real_circle().contains(tgt));

}


r_circle_segment::r_circle_segment(const r_circle_point& src, 
                                   const r_circle_point& tgt, const rat_line& l)
{
  //assert(src != tgt); 
  //assert(real_line(l).contains(src) && real_line(l).contains(tgt));

  if (src == tgt || !real_line(l).contains(src) || !real_line(l).contains(tgt))
  { init_as_trivial(src);
    LEDA_EXCEPTION(1,"r_circle_segment::r_circle_segment: illegal arguments");
   }

  rat_point p = l.point1(), q = l.point2();
  rat_point middle = midpoint(p, q);

  if ( compare(src,tgt) == compare(p,q) )
     PTR = new rep(src, tgt, rat_circle(p, middle, q));
  else
     PTR = new rep(src, tgt, rat_circle(q, middle, p));
}



r_circle_segment::r_circle_segment(const rat_point& src, const rat_point& mid, 
                                                         const rat_point& tgt)
{ //assert(src != tgt && src != mid && tgt != mid);
  if (leda::orientation(src,mid,tgt) == 0)
  { init_as_trivial(src);
    LEDA_EXCEPTION(1,"r_circle_segment::r_circle_segment: illegal arguments");
   }
  rat_circle circ(src, mid, tgt);
  PTR = new rep(r_circle_point(src), r_circle_point(tgt), circ);
}


r_circle_segment::r_circle_segment(const rat_circle& c)
{
  //assert(c.orientation() != 0);
  if (c.orientation() == 0)
  { init_as_trivial(c.point1());
    LEDA_EXCEPTION(1,"r_circle_segment::r_circle_segment: degenerate circle");
   }

   rat_circle circ = (c.orientation() > 0) ? c : c.reverse();
   r_circle_point p(circ.point1());
   PTR = new rep(p, p, circ);
}

void 
r_circle_segment::init_as_segment(const rat_point& src, const rat_point& tgt)
{
   if (src != tgt) 
     { rat_point middle = midpoint(src, tgt);
       rat_circle c(src, middle, tgt);
       PTR = new rep(r_circle_point(src), r_circle_point(tgt), c);
      }
    else 
      init_as_trivial(r_circle_point(src));
}

// The only difference to the function above is that we avoid the construction 
// of new r_circle_points for s and t!

r_circle_segment::r_circle_segment(const r_circle_point& s, 
                                   const r_circle_point& t)
{
  assert(s.is_rat_point() && t.is_rat_point());

  rat_point src = s.to_rat_point(); 
  rat_point tgt = t.to_rat_point();

  if (src != tgt) 
  { rat_point middle = midpoint(src, tgt);
    rat_circle c(src, middle, tgt);
    PTR = new rep(s, t, c);
   }
   else init_as_trivial(s);
}

void r_circle_segment::normalize()
{ ptr()->Source.normalize();
  ptr()->Target.normalize();
  ptr()->Circle.normalize();
}

bool r_circle_segment::verify(bool report_errors) const
{
  r_circle_point src = source();
  r_circle_point tgt = target();

  bool ok = src.verify(report_errors) && tgt.verify(report_errors);

  // TODO: check segment specific invariants (cf. above)

  if (! is_trivial()) 
  { ok &= _real_circle().contains(src);
    ok &= _real_circle().contains(tgt);
    if (!ok) 
      LEDA_EXCEPTION(1, "r_circle_segment::verify: src or tgt not on circle");
   }
  return ok;
}


real_point r_circle_segment::real_middle() const
{
  if (is_trivial()) return source();
  if (is_full_circle()) 
  { // return the point on the circle that is opposite to source()
    real_point cen(_real_circle().center());
    return cen + (cen - source());
   }

  real_point p = source(), q = target();
  real_point m = midpoint(p, q);
  if (is_straight_segment()) return m;

  real_vector vec = (q-p).rotate90(orientation());
  real_line l(m, vec);
  list<real_point> inter = _real_circle().intersection(l);
  if (!inter.empty()) return inter.head();

  // should never be reached!
  LEDA_EXCEPTION(1, "r_circle_segment::real_middle: internal error");
  return real_point();
}

r_circle_point r_circle_segment::middle() const
{
  if (is_trivial()) return source();

    if (is_full_circle()) 
	{ // return the point on the circle that is opposite to source()
    real_point cen(_real_circle().center());
	real_point md = cen + (cen - source());
	point cend (md.xcoord().to_double(), md.ycoord().to_double());
    return r_circle_point(cend);
    }

	// CU: begin work around, middle did not work for vertical bisector
	if (source().ycoordD() == target().ycoordD())
	{

	rat_vector vec = (source().to_rat_point() - target().to_rat_point()).rotate90(orientation());

	double d1 = vec.sqr_length().todouble();
	double d2 = circle().sqr_radius().todouble();
	rat_line rl(center(), vec);

	if (d1 < d2)
		vec *= int(d2/d1) + 1;
	rat_segment rs(center(), vec);

	list<real_point> rpl = ((real_circle)circle()).intersection(rs);
	r_circle_point result(rpl.head(), circle(), rl, r_circle_point::second);
	return result;
	}
	// CU: end fix

  rat_line l;

  if (source().is_rat_point() && target().is_rat_point()) 
  { rat_point p = source().to_rat_point();
    rat_point q = target().to_rat_point();

    if (is_full_circle()) return r_circle_point( center() + (center() - p) );
    if (is_straight_segment()) return midpoint(p, q);

    rat_vector vec = (q-p).rotate90(orientation());
    l = rat_line(center(), vec);
   }
   else 
   { if (is_full_circle()) 
       l = rat_line(center(), source().approximate_by_rat_point());
     else 
       { real_point p = source();
         real_point q = target();
         real_point a = is_straight_segment() ? midpoint(p,q) : _real_circle().center();
         real_point b = a + (q-p).rotate90(orientation());

         rat_segment s(rat_point(a.to_float()), rat_point(b.to_float()));

         // new (s.n.)
         if (s.is_trivial()) return source();

         l = rat_line(s);
        }
     }

    r_circle_point result;
    if (! r_circle_point::intersection(circle(), l, r_circle_point::first, result))
       LEDA_EXCEPTION(1, "r_circle_segment::middle: internal error"); 
                // should never be reached!
    return result;
}

bool r_circle_segment::contains(const r_circle_point& p) const
{
  if ( p == source() || p == target() ) return true;
  bool circle_contains_p = p.is_rat_point() ? circle().contains(p.to_rat_point()) : _real_circle().contains(p);
  return circle_contains_p && wedge_contains(p);
}

bool r_circle_segment::overlaps(const r_circle_segment& cs) const
{
  if (is_trivial() || cs.is_trivial() || !equal_as_sets(circle(), cs.circle()))
     return false;
        
  if (! is_degenerate()) 
  { if (identical(source(),target())) return true; // full circle

    int o_circ = orientation();
                
    // assume underlying circle is ccw-oriented

    int o_cs_src = leda::orientation(target(), source(), cs.source()) * o_circ;
    if (o_cs_src > 0) return true;

    // cs.source() lies to left of the line (tgt,src),
    // hence in the interior of this segment

    int o_cs_tgt = leda::orientation(target(), source(), cs.target()) * o_circ;
    if (o_cs_tgt > 0) return true;
    // ... same for cs.target()

    // both endpoints of cs do not lie in the interior of *this, so check the 
    // orientations of the underlying circles ...
    if (o_cs_src == 0) 
      return (source() == cs.source()) == (o_circ == cs.circle().orientation());
    else 
      if (o_cs_tgt == 0)
        return (target() == cs.target()) == (o_circ == cs.circle().orientation());
      else // source() != cs.source() && source() != cs.target()
        return leda::orientation(cs.source(), source(), cs.target()) == cs.circle().orientation();
    }
    else 
     { assert(cs.is_degenerate()); // NOTE: equal_as_sets is true

       // supporting line for *this has the equation: p = src + lambda * (dx,dy)
       // where (dx,dy) = tgt - src
       // we compute the lambda-values
       //    for cs.source() (called alpha) and 
       //    for cs.target() (called beta)
       // NOTE: 0 <= lambda <= 1  ->  p lies between src and tgt 
       r_circle_point src = source(), tgt = target();
       real dx = tgt.xcoord() - src.xcoord();
       real alpha, beta;
       if (sign(dx) != 0) 
       { alpha = (cs.source().xcoord() - src.xcoord()) / dx;
         beta  = (cs.target().xcoord() - src.xcoord()) / dx;
        }
       else 
       { real dy = tgt.ycoord() - src.ycoord();
         alpha = (cs.source().ycoord() - src.ycoord()) / dy;
         beta  = (cs.target().ycoord() - src.ycoord()) / dy;
        }
       return (alpha > 0 || beta > 0) && (alpha < 1 || beta < 1);
       // one endpnt of cs comes after src, and one endpnt comes before tgt
     }
}

bool 
r_circle_segment::overlaps(const r_circle_segment& cs2, 
                           const r_circle_point& common1, 
                           const r_circle_point& common2) const
{
  r_circle_segment cs1 = *this;

  assert(!cs1.is_trivial() && !cs2.is_trivial() && common1 != common2);

  if (cs1.is_straight_segment() && cs2.is_straight_segment()) 
    return true;



// Problem in the original version: comparing the radius is not
// sufficient for testing if underlying circles are equal
// I use a full equality test (operator==) for both orientations
// of the first circle
//
// fixed by sn (Nov. 2011)

  if (!cs1.is_degenerate() && !cs2.is_degenerate()
       //&& cs1.sqr_radius() == cs2.sqr_radius()) /*not sufficient (see above)*/
        && (cs1.circle() == cs2.circle() || 
            cs1.circle().reverse() == cs2.circle()))
  {
    // the two underlying circles are equal (as sets), now check orientation ...
    int ori1 = 0;
    if (cs1.source() == common1) ori1 = cs1.orientation();
    else if (cs1.target() == common1) ori1 = -cs1.orientation();
    else ori1 = leda::orientation(cs1.source(), common1, cs1.target());

    int ori2 = 0;
    if (cs2.source() == common1) ori2 = cs2.orientation();
    else if (cs2.target() == common1) ori2 = -cs2.orientation();
    else ori2 = leda::orientation(cs2.source(), common1, cs2.target());
    return ori1 == ori2;
  }

 return false;
}


bool 
r_circle_segment::wedge_contains(const real_point& p) const
{
        if(! is_degenerate()) {
                if (is_full_circle()) return true;

                // (should also work if *this is_full_circle)
                real_point cen = real_center(), src = source(), tgt = target();
                int o_circ = orientation();
                int o_src  = leda::orientation(cen, src, p);
                int o_tgt  = leda::orientation(cen, tgt, p);

                if (leda::orientation(src, cen, tgt) * o_circ < 0)
                        // less than half a circle
                        return (o_circ * o_src >= 0) && (o_circ * o_tgt) <= 0;
                else
                        // more than half a circle
                        return (o_circ * o_src >= 0) || (o_circ * o_tgt) <= 0;
        }
        else { // suppose p is on the line
                real_point src = source(), tgt = target();

                if (is_trivial()) return p == src;

                int cmp_src_x = real_point::cmp_x(p, src);
                int cmp_tgt_x = real_point::cmp_x(p, tgt);

                if (cmp_src_x != 0 || cmp_tgt_x != 0) {
                        return (cmp_src_x != cmp_tgt_x) || (cmp_src_x == 0);
                }
                else {
                        // *this is vertical
                        int cmp_src_y = real_point::cmp_y(p, src);
                        int cmp_tgt_y = real_point::cmp_y(p, tgt);
                        return (cmp_src_y != cmp_tgt_y) || (cmp_src_y == 0);
                }
        }
}

/*
        representation:
        - proper non-degenerate arc:       source() != target(), !circle().is_degenerate()
        - trivial curve:                   identical(source(),target()), circle() == rat_circle()
        - non-degenerate full circle:      identical(source(),target()), !circle().is_degenerate()
        - non-triv. straight line segment: source() != target(), circle().is_line()
                                                                           compare(source(),target()) == compare(circle().point1(),circle().point3())
*/

r_circle_segment 
r_circle_segment::round(int prec) const
{
        if (is_full_circle()) {
                rat_circle c(circle().to_float(), prec);
                return r_circle_segment(c);
        }

        r_circle_point src = source().round(prec), tgt = target().round(prec);
        if (src == tgt) return r_circle_segment(src);

        if (!is_straight_segment()) {
                point mid = middle().to_float();
                rat_point p = src.to_rat_point(), r(mid, prec), q = tgt.to_rat_point();
                if (!collinear(p, r, q)) return r_circle_segment(p, r, q);
                // otherwise we fall through and return a straight segment!
        }

        return r_circle_segment(src, tgt);
}

r_circle_segment 
r_circle_segment::translate(rational dx, rational dy) const
{
        rat_circle     new_circ = circle().translate(dx, dy);
        r_circle_point new_src  = source().translate(dx, dy);
        r_circle_point new_tgt  = target().translate(dx, dy);

        r_circle_segment result(new_circ, new_src, new_tgt); // special private ctor
        result.normalize();
        return result;
}

r_circle_segment 
r_circle_segment::rotate90(const rat_point& q, int i) const
{
        rat_circle     new_circ = circle().rotate90(q, i);
        r_circle_point new_src  = source().rotate90(q, i);
        r_circle_point new_tgt  = target().rotate90(q, i);

        r_circle_segment result(new_circ, new_src, new_tgt); // special private ctor
        result.normalize();
        return result;
}

r_circle_segment 
r_circle_segment::reflect(const rat_point& p, const rat_point& q) const
{
        rat_circle     new_circ = circle().reflect(p, q);
        r_circle_point new_src  = source().reflect(p, q);
        r_circle_point new_tgt  = target().reflect(p, q);

        r_circle_segment result(new_circ, new_src, new_tgt); // special private ctor
        result.normalize();
        return result;
}

r_circle_segment 
r_circle_segment::reflect(const rat_point& p) const
{
        rat_circle     new_circ = circle().reflect(p);
        r_circle_point new_src  = source().reflect(p);
        r_circle_point new_tgt  = target().reflect(p);

        r_circle_segment result(new_circ, new_src, new_tgt); // special private ctor
        result.normalize();
        return result;
}


list<r_circle_point> 
r_circle_segment::intersection(const rat_line& l) const
{
        list<r_circle_point> result;

        if (! is_degenerate()) {
                list<real_point> inters = leda::intersection(circle(), l);
                real_point inter;
                int which = r_circle_point::first;
                forall(inter, inters) {
                        r_circle_point icp(inter, circle(), l, r_circle_point::tag(which));
                        if (wedge_contains((const real_point&)icp)) result.append(icp);
                        ++which;
                }
        }
        else {
                if (is_trivial()) {
                        if (real_line(l).contains(source())) result.append(source());
                }
                else {
                        assert(is_straight_segment());
                        rat_point inter;
                        if (supporting_line().intersection(l, inter)) {
                                r_circle_point icp(inter);
                                if (wedge_contains(icp)) result.append(icp);
                        }
                }
        }

        return result;
}

list<r_circle_point> 
r_circle_segment::intersection(const r_circle_segment& cs) const
{
        list<r_circle_point> result;

        if (! is_degenerate() && !cs.is_degenerate()) {
                rat_line rad_axis;
                if (radical_axis(circle(), cs.circle(), rad_axis)) {
                        list<real_point> inters = leda::intersection(circle(), rad_axis);
                        real_point inter;
                        int which = r_circle_point::first;
                        forall(inter, inters) {
                                r_circle_point icp(inter, circle(), rad_axis, r_circle_point::tag(which));
                                if ( wedge_contains((const real_point&)icp) 
                                         && cs.wedge_contains((const real_point&)icp) ) 
                                {
                                        result.append(icp);
                                }
                                ++which;
                        }
                }
                else {
                        // both circles have the same center
                        if (!overlaps(cs)) {
                                // if they do not overlap properly, they might share an endpoint
                                if (orientation() == cs.orientation()) {
                                        if (source() == cs.target()) result.append(source());
                                        if (target() == cs.source()) result.append(target());
                                }
                                else {
                                        if (source() == cs.source()) result.append(source());
                                        if (target() == cs.target()) result.append(target());
                                }
                        }
                }
        }
        else { // at least one is degenerate
                if (is_trivial()) {
                        if ( cs.contains(source()) ) result.append(source()); 
                        return result;
                }
                if (cs.is_trivial()) { 
                        if ( contains(cs.source()) ) result.append(cs.source());
                        return result;
                }

                r_circle_segment cs1 = *this, cs2 = cs;
                if (cs1.is_straight_segment()) {
                        cs1 = cs; cs2 = *this;
                }

                assert(cs2.is_straight_segment());
                result = cs1.intersection(cs2.supporting_line());
                list_item it;
                forall_items(it, result) 
                        if (! cs2.wedge_contains(result[it])) result.del_item(it);
        }

        return result;
}

list<real_point>
r_circle_segment::intersection(const real_line& l) const
{
        list<real_point> inters = _real_circle().intersection(l);
        list_item it;
        forall_items(it, inters) {
                real_point inter = inters[it];
                if (!wedge_contains(inter)) inters.del_item(it);
        }

        return inters;
}

real 
r_circle_segment::sqr_dist(const real_point& p) const
{
        if (is_trivial()) return source().sqr_dist(p);
        if (is_straight_segment()) return real_segment(source(),target()).sqr_dist(p);
        if (is_full_circle()) return _real_circle().sqr_dist(p);

        assert(is_proper_arc());
/*      Consider the voronoi-diagram of *this:
        It consists of the wedge induced by *this (see wedge_contains(...)) 
        and the ray r which starts in the center of the supporting circle and is
        supported by the bisector of source() and target().
        This means:
        -       Inside the wedge, the distance to the supporting circle equals the 
                distance to *this.
        -       Outside the wedge, the distance to *this is the minimum of the 
                distances to source() and target().
*/
        if (wedge_contains(p)) return _real_circle().sqr_dist(p);
        real sd = source().sqr_dist(p), td = target().sqr_dist(p);
        return sd < td ? sd : td;
}

real_line 
r_circle_segment::tangent_at(const r_circle_point& p) const
// NOTE: If you change this, check that tangent_at_src (in r_circle_segment.h) is still ok!
{
        if (is_straight_segment()) {
                return real_line(source(), target());
        }
        else {
                assert(orientation() != 0);
                // c1_p_dx/dy refers to line l1(cs1.real_center(), p),
                // tangent t1 is perpendicular to l1
                real c1_p_dx = p.xcoord() - real_center().xcoord();
                real c1_p_dy = p.ycoord() - real_center().ycoord();
                real_vector dir(orientation() * -c1_p_dy, orientation() * c1_p_dx);
                return real_line(p, dir);
        }
}

int 
r_circle_segment::orientation_of_p_wrt_supporting_line(const r_circle_point& p) const
{
        if (identical(p, source()) || identical(p, target())) return 0;

        if (p.is_rat_point()) {
                return leda::orientation(circle().point1(), circle().point3(), p.to_rat_point());
        }
        else {
                return leda::orientation((const real_point&)source(), (const real_point&)target(), (const real_point&)p);
        }
}

r_circle_segment 
r_circle_segment::trim(const r_circle_point& new_src, const r_circle_point& new_tgt) const
{
        if (identical(new_src, source()) && identical(new_tgt,target())) return *this;

        if (is_degenerate() || is_full_circle()) 
                return r_circle_segment(new_src, new_tgt, circle());

        if (new_src == source())
                return r_circle_segment(new_src, new_tgt, circle());
        if (new_src == target())
                return r_circle_segment(new_src, new_tgt, circle().reverse());

        if (leda::orientation(source(), new_src, new_tgt) == orientation())
                return r_circle_segment(new_src, new_tgt, circle());
        else
                return r_circle_segment(new_src, new_tgt, circle().reverse());
}

r_circle_segment 
r_circle_segment::trim(const r_circle_point& new_src, const r_circle_point& new_tgt,
                                           bool& same_direction) const
{
        r_circle_segment result = trim(new_src, new_tgt);
        same_direction = identical(circle(), result.circle());
        return result;
}



// Fix by sn (12/2007)
// approximate area broken for zero (or very small ?) angles

/*
double r_circle_segment::approximate_area() const
{
  if (is_degenerate()) return 0;
  if (is_full_circle()) 
      return sqr_radius().to_double() * LEDA_PI * orientation();

   double phi = center().to_float().angle(source().to_float(), 
                                          target().to_float()); //[0,pi[
   double ori_phi = (orientation() > 0) ? phi : (phi - 2*LEDA_PI);
   return (sqr_radius().to_double() / 2) * (ori_phi - sin(phi));
}
*/


double r_circle_segment::approximate_area() const
{
   if (is_degenerate()) return 0;

   double r2 = sqr_radius().to_float();

   if (is_full_circle()) return r2 * LEDA_PI * orientation();

   point src = source().to_float();
   point tgt = target().to_float();
   point cen = center().to_float();

   double phi = cen.angle(src,tgt); // [0,pi[

 //if (phi == 0)
   if (fabs(phi) < 1e-20) return 0;

   if (orientation() < 0) phi -= 2* LEDA_PI; 

   return r2*(phi - sin(phi))/2;
}


void 
r_circle_segment::compute_bounding_box(real& xmin, real& ymin, real& xmax, real& ymax) const
{
        // trivial
        if (is_trivial()) {
                xmin = xmax = source().xcoord(); ymin = ymax = source().ycoord();
                return;
        }

        // straight segment
        if (is_straight_segment()) {
                if (r_circle_point::cmp_x(source(), target()) <= 0) {
                        xmin = source().xcoord(); xmax = target().xcoord();
                }
                else {
                        xmax = source().xcoord(); xmin = target().xcoord();
                }
                if (r_circle_point::cmp_y(source(), target()) <= 0) {
                        ymin = source().ycoord(); ymax = target().ycoord();
                }
                else {
                        ymax = source().ycoord(); ymin = target().ycoord();
                }
                return;
        }

        // full circle
        real rad = radius();
        real_point cen = real_center();
        if (is_full_circle()) {
                xmin = cen.xcoord() - rad; xmax = cen.xcoord() + rad;
                ymin = cen.ycoord() - rad; ymax = cen.ycoord() + rad;
                return;
        }

        // proper arc
        r_circle_point start = orientation() > 0 ? source() : target();
        r_circle_point end   = orientation() > 0 ? target() : source();

        // we may assume that circle goes from start to end and is pos. oriented

        int cmp_x_start_cen = real_point::cmp_x(start, cen);
        int cmp_y_start_cen = real_point::cmp_y(start, cen);
        int cmp_x_end_cen   = real_point::cmp_x(end, cen);
        int cmp_y_end_cen   = real_point::cmp_y(end, cen);

        unsigned quadrant_start = 0;
        if (cmp_y_start_cen >= 0)
                quadrant_start = cmp_x_start_cen >= 0 ? 1 : 2;
        else
                quadrant_start = cmp_x_start_cen < 0  ? 3 : 4;

        unsigned quadrant_end = 0;
        if (cmp_y_end_cen >= 0)
                quadrant_end = cmp_x_end_cen >= 0 ? 1 : 2;
        else
                quadrant_end = cmp_x_end_cen < 0  ? 3 : 4;

        switch (quadrant_start) {
        case 1:
                switch (quadrant_end) {
                case 1: // 1 -> 1 (x: mon. decreasing, y: mon increasing)
                        if (r_circle_point::cmp_x(start,end) <= 0) {
                                // same bbox as full circle
                                xmin = cen.xcoord() - rad; xmax = cen.xcoord() + rad;
                                ymin = cen.ycoord() - rad; ymax = cen.ycoord() + rad;
                        }
                        else {
                                // same bbox as line
                                xmin = end.xcoord();   xmax = start.xcoord();
                                ymin = start.ycoord(); ymax = end.ycoord();
                        }
                        return;
                case 2: // 1 -> 2
                        ymax = cen.ycoord() + rad;
                        xmin = end.xcoord();
                        ymin = r_circle_point::cmp_y(start,end) <= 0 ? start.ycoord() : end.ycoord();
                        xmax = start.xcoord();
                        return;
                case 3: // 1 -> 3
                        ymax = cen.ycoord() + rad;
                        xmin = cen.xcoord() - rad;
                        ymin = end.ycoord();
                        xmax = start.xcoord();
                        return;
                case 4: // 1 -> 4
                        ymax = cen.ycoord() + rad;
                        xmin = cen.xcoord() - rad;
                        ymin = cen.ycoord() - rad;
                        xmax = r_circle_point::cmp_x(start,end) >= 0 ? start.xcoord() : end.xcoord();
                        return;
                } // eof 1 -> ?

        case 2:
                switch (quadrant_end) {
                case 2: // 2 -> 2 (x: mon. decreasing, y: mon decreasing)
                        if (r_circle_point::cmp_x(start,end) <= 0) {
                                // same bbox as full circle
                                xmin = cen.xcoord() - rad; xmax = cen.xcoord() + rad;
                                ymin = cen.ycoord() - rad; ymax = cen.ycoord() + rad;
                        }
                        else {
                                // same bbox as line
                                xmin = end.xcoord(); xmax = start.xcoord();
                                ymin = end.ycoord(); ymax = start.ycoord();
                        }
                        return;
                case 3: // 2 -> 3
                        xmin = cen.xcoord() - rad;
                        ymin = end.ycoord();
                        xmax = r_circle_point::cmp_x(start,end) >= 0 ? start.xcoord() : end.xcoord();
                        ymax = start.ycoord();
                        return;
                case 4: // 2 -> 4
                        xmin = cen.xcoord() - rad;
                        ymin = cen.ycoord() - rad;
                        xmax = end.xcoord();
                        ymax = start.ycoord();
                        return;
                case 1: // 2 -> 1
                        xmin = cen.xcoord() - rad;
                        ymin = cen.ycoord() - rad;
                        xmax = cen.xcoord() + rad;
                        ymax = r_circle_point::cmp_y(start,end) >= 0 ? start.ycoord() : end.ycoord();
                        return;
                } // eof 2 -> ?

        case 3:
                switch (quadrant_end) {
                case 3: // 3 -> 3 (x: mon. increasing, y: mon decreasing)
                        if (r_circle_point::cmp_x(start,end) >= 0) {
                                // same bbox as full circle
                                xmin = cen.xcoord() - rad; xmax = cen.xcoord() + rad;
                                ymin = cen.ycoord() - rad; ymax = cen.ycoord() + rad;
                        }
                        else {
                                // same bbox as line
                                xmin = start.xcoord(); xmax = end.xcoord();
                                ymin = end.ycoord();   ymax = start.ycoord();
                        }
                        return;
                case 4: // 3 -> 4
                        ymin = cen.ycoord() - rad;
                        xmax = end.xcoord();
                        ymax = r_circle_point::cmp_y(start,end) >= 0 ? start.ycoord() : end.ycoord();
                        xmin = start.xcoord();
                        return;
                case 1: // 3 -> 1
                        ymin = cen.ycoord() - rad;
                        xmax = cen.xcoord() + rad;
                        ymax = end.ycoord();
                        xmin = start.xcoord();
                        return;
                case 2: // 3 -> 2
                        ymin = cen.ycoord() - rad;
                        xmax = cen.xcoord() + rad;
                        ymax = cen.ycoord() + rad;
                        xmin = r_circle_point::cmp_x(start,end) <= 0 ? start.xcoord() : end.xcoord();
                        return;
                } // eof 3 -> ?

        case 4:
                switch (quadrant_end) {
                case 4: // 4 -> 4 (x: mon. increasing, y: mon increasing)
                        if (r_circle_point::cmp_x(start,end) >= 0) {
                                // same bbox as full circle
                                xmin = cen.xcoord() - rad; xmax = cen.xcoord() + rad;
                                ymin = cen.ycoord() - rad; ymax = cen.ycoord() + rad;
                        }
                        else {
                                // same bbox as line
                                xmin = start.xcoord(); xmax = end.xcoord();
                                ymin = end.ycoord();   ymax = start.ycoord();
                        }
                        return;
                case 1: // 4 -> 1
                        xmax = cen.xcoord() + rad;
                        ymax = end.ycoord();
                        xmin = r_circle_point::cmp_x(start,end) <= 0 ? start.xcoord() : end.xcoord();
                        ymin = start.ycoord();
                        return;
                case 2: // 4 -> 2
                        xmax = cen.xcoord() + rad;
                        ymax = cen.ycoord() + rad;
                        xmin = end.xcoord();
                        ymin = start.ycoord();
                        return;
                case 3: // 4 -> 3
                        xmax = cen.xcoord() + rad;
                        ymax = cen.ycoord() + rad;
                        xmin = cen.xcoord() - rad;
                        ymin = r_circle_point::cmp_y(start,end) <= 0 ? start.ycoord() : end.ycoord();
                        return;
                } // eof 4 -> ?
        } // eof outer switch
}

r_circle_point 
r_circle_segment::lex_smallest_point() const
{
        if (is_trivial()) return source();
        if (is_straight_segment()) {
                if (is_vertical_segment()) 
                        return r_circle_point::cmp_y(source(), target()) < 0 ? source() : target();
                else
                        return r_circle_point::cmp_x(source(), target()) < 0 ? source() : target();
        }

        rat_point cnt = center();
        rat_line horiz_line(cnt, rat_point(cnt.xcoord()+1, cnt.ycoord()));

        if (is_full_circle()) return r_circle_point(circle(), horiz_line, r_circle_point::first);

        // proper arc:
        r_circle_point smaller_end_pnt = source(); int orient = orientation();
        if (compare(target(), source()) < 0) {
                smaller_end_pnt = target(); orient *= -1;
        }

        int cmp = real_point::cmp_y(smaller_end_pnt, real_center()) * orient;
        // cmp > 0 <=> a) smaller_end_pnt above horiz_line and circle starting there pos. oriented
        //          or b) smaller_end_pnt below horiz_line and circle starting there neg. oriented

        return cmp > 0 ? r_circle_point(circle(),horiz_line,r_circle_point::first) : smaller_end_pnt;
}

int 
r_circle_segment::compare_y(const r_circle_point& p) const
{
        if (p == source() || p == target()) return 0;
        if (is_trivial()) return compare(p.ycoord(), source().ycoord());

        if (is_vertical_segment()) {
                int cmp_src_y = compare(p.ycoord(), source().ycoord());
                int cmp_tgt_y = compare(p.ycoord(), target().ycoord());
                if (cmp_src_y != cmp_tgt_y || cmp_src_y == 0 /*|| cmp_tgt_y == 0*/) return 0;
                return cmp_src_y;
        }

        int o_src_tgt = leda::orientation(source(), target(), p);

        if (is_degenerate()) return o_src_tgt;

        // non-degenerate and x-monotonous (hence at most half a circle)
        int o_circ = orientation();
        if (o_src_tgt * o_circ >= 0) return o_circ; 
                // p lies above (below) line through source and target if circ is pos. (neg.) oriented
                // => p lies strictly above (below) the segment (obsv: p != source() && p != target())

        return compare(real(sqr_radius()), p.sqr_dist(real_center())) * o_circ;
}

/*
        We want to approximate the segment as follows.
        If we connect each point in the returned list by straight segment with its 
        successor, then we obtain a curve C.
        We want to make sure that the distance of any point on the r_circle_segment
        to C is at most "dist".
*/
list<point> 
r_circle_segment::approximate(double dist) const
{
        const double eps = 1e-5;
        if (dist < eps) dist = eps;

        list<point> result;

        r_circle_point src = source(), tgt = target();
        point src_f = src.to_float(), tgt_f = tgt.to_float();

        result.append(src_f);
        if (is_trivial()) return result;

        if (!is_straight_segment()) {
                // leda::circle circ_f = circle().to_float(); // this may fail
                point c1_f = circle().point1().to_float();
                point c2_f = circle().point2().to_float();
                point c3_f = circle().point3().to_float();
                if (!collinear(c1_f, c2_f, c3_f)) {
                        leda::circle circ_f(c1_f, c2_f, c3_f);
                        // phi is the angle between two successive approximation points
                        if (circ_f.radius() > 0 && circ_f.orientation() == circle().orientation()) {
                                double rad = circ_f.radius();
                                double cos_phi_2 = (rad - dist) / rad;
                                if (cos_phi_2 > 1) cos_phi_2 = 1; else if (cos_phi_2 < 0) cos_phi_2 = 0;
                                double phi = 2 * acos(cos_phi_2);
                                if (phi < eps) phi = eps;
                                double ori_phi = phi * orientation();
                                point cen_f = circ_f.center(); point horiz_f(cen_f.xcoord()+1,cen_f.ycoord());
                                double angle_span;
                                if (is_full_circle()) angle_span = 2 * LEDA_PI;
                                else {
                                        angle_span = cen_f.angle(src_f, tgt_f);
                                        if (orientation() < 0) angle_span = 2*LEDA_PI - angle_span;
                                }
                                double angle = cen_f.angle(horiz_f, src_f);
                                while (angle_span > phi) {
                                        angle_span -= phi; angle += ori_phi;
                                        point next = circ_f.point_on_circle(angle);
                                        result.append(next);
                                }
                        }
                }
        }

        result.append(tgt_f);
        return result;
}

list<rat_point> 
r_circle_segment::approximate_by_rat_points(double dist) const
{
        list<rat_point> result;

        list<point> approx_pnts = approximate(dist);
        point p;
        forall(p, approx_pnts) result.append(rat_point(p));

        return result;
}

list<rat_segment> 
r_circle_segment::approximate_by_rat_segments(double dist) const
{
        list<rat_segment> result;

        list<point> approx_pnts = approximate(dist);
        rat_point src = rat_point( approx_pnts.pop_front() );
        point p;
        forall(p, approx_pnts) {
                rat_point tgt = rat_point(p);
                result.append( rat_segment(src, tgt) );
                src = tgt;
        }

        return result;
}

list<r_circle_point> 
r_circle_segment::compute_split_points(double dist) const
{
        list<r_circle_point> result;

        result.append(source());
        if (is_trivial()) return result;

        if (!is_straight_segment()) {
                list<point> pnts_f = approximate(dist);
                if (!pnts_f.empty()) pnts_f.pop_front(); // remove source
                if (!pnts_f.empty()) pnts_f.pop_back();  // remove target
                point next_f;
                forall(next_f, pnts_f) {
                        rat_line l((rat_point)next_f, center());
                        r_circle_point next(circle(), l, r_circle_point::first);
                        result.append(next);
                }
        }

        result.append(target());
        return result;
}

r_circle_segment 
r_circle_segment::trim_approximate(const rat_point& new_src, const rat_point& new_tgt) const
// NOTE: The original and the trimmed segment may have opposite orientations!
{
        r_circle_point src = source(), tgt = target();
        if ( src.is_rat_point() && tgt.is_rat_point() 
                 && src.to_rat_point() == new_src && tgt.to_rat_point() == new_tgt )
        {
                return *this;
        }

        if (is_full_circle()) {
                LEDA_EXCEPTION(1, "r_circle_segment::trim_approximate: cannot trim full circle");
                return *this;
        }

        if (is_degenerate()) return r_circle_segment(new_src, new_tgt);

        rat_line l = p_bisector(new_src, new_tgt);
        list<r_circle_point> inters = intersection(l);
        if (inters.empty()) return r_circle_segment(new_src, new_tgt);
        real_point middle_rp = inters.head();
        if (inters.size() == 2) {
                // src and tgt on same side of line l (rare case)
                // We consider line h = line(new_src, new_tgt).
                // (h is perpendicular to l and intersects l in midpoint(new_src, new_tgt).)
                // let m1 denote inters.head() and m2 denote inters.tail().
                // m1 lies to the right of h and m2 lies to the left of h.
                // If src and tgt lie both on the same side of h, the decision is easy;
                // otherwise we choose the mi that is closest to new_src (and hence to new_tgt).
                r_circle_point new_src_rcp(new_src), new_tgt_rcp(new_tgt);
                int ori_h_src = leda::orientation(new_src_rcp, new_tgt_rcp, src);
                int ori_h_tgt = leda::orientation(new_src_rcp, new_tgt_rcp, tgt);
                if (ori_h_src < 0 && ori_h_tgt < 0)      middle_rp = inters.tail();
                else if (ori_h_src > 0 && ori_h_tgt > 0) middle_rp = inters.head();
                else {
                        real_point m1 = inters.head(), m2 = inters.tail();
                        middle_rp = m1.sqr_dist(new_src_rcp) < m2.sqr_dist(new_src_rcp) ? m1 : m2;
                }
        }

        middle_rp.xcoord().guarantee_relative_error(DOUBLE_PREC);
        middle_rp.ycoord().guarantee_relative_error(DOUBLE_PREC);
        rat_point middle = rat_point( middle_rp.to_float() );

        if (collinear(new_src, middle, new_tgt)) return r_circle_segment(new_src, new_tgt);

        //if (new_src.sqr_dist(middle) < 1e-6) {
        //      return r_circle_segment(new_src, new_tgt);
        //}

        return r_circle_segment(new_src, middle, new_tgt);
}

list<r_circle_segment> 
r_circle_segment::split_circle(const rat_circle& circ)
{
        if (circ.is_degenerate()) 
          LEDA_EXCEPTION(1, "circle must not be degenerate");

        rat_point center = circ.center();
        rat_line horiz(center, rat_point(center.xcoord()+1, center.ycoord()));
        r_circle_point p(circ, horiz, r_circle_point::first);
        r_circle_point q(circ, horiz, r_circle_point::second);

        list<r_circle_segment> segs;
        segs.append( r_circle_segment(p, q, circ) );
        segs.append( r_circle_segment(q, p, circ) );
        return segs;
}

bool equal_as_sets(const r_circle_segment& cs1, const r_circle_segment& cs2)
{
        rat_circle c1 = cs1.circle(), c2 = cs2.circle();
        r_circle_point src1 = cs1.source(), tgt1 = cs1.target();
        r_circle_point src2 = cs2.source(), tgt2 = cs2.target();

        if (identical(src1, tgt1)) {
                if (!identical(src2, tgt2)) return false;
                if (c1.is_trivial()) return c2.is_trivial();
                return equal_as_sets(c1, c2);
        }
        else {
                if (identical(src2, tgt2)) return false;
                if (!equal_as_sets(c1, c2)) return false;
                if (src1 == src2) return (tgt1 == tgt2) && (c1.orientation() ==  c2.orientation()); 
                if (src1 == tgt2) return (tgt1 == src2) && (c1.orientation() == -c2.orientation());
                return false;
        }
}

int compare_tangent_slopes(const r_circle_segment& cs1, const r_circle_segment& cs2, 
                                                   const r_circle_point& p)
/*
        We compare the slopes of the tangents to cs1/cs2 in the point p
        (which must be on cs1 and cs2).
        If the slope is infinity, we distinguish between +infty and -infty
*/
{
        if (cs1.is_trivial() || cs2.is_trivial()) return 0;

        if (cs1.is_degenerate() && cs2.is_degenerate()) {
                assert(cs1.is_straight_segment() && cs2.is_straight_segment());
                return cmp_slopes(cs1.supporting_line(), cs2.supporting_line());
        }

        real t1_dx, t1_dy; int cs1_orient;
        if (cs1.is_degenerate()) {
                t1_dx = cs1.target().xcoord() - cs1.source().xcoord();
                t1_dy = cs1.target().ycoord() - cs1.source().ycoord();
                cs1_orient = 1;
        }
        else {
                // c1_p_dx/dy refers to line l1(cs1.real_center(), p),
                // tangent t1 is perpendicular to l1
                real c1_p_dx = p.xcoord() - cs1.real_center().xcoord();
                real c1_p_dy = p.ycoord() - cs1.real_center().ycoord();
                t1_dx = -c1_p_dy; t1_dy = c1_p_dx;
                cs1_orient = cs1.orientation();
        }

        real t2_dx, t2_dy; int cs2_orient;
        if (cs2.is_degenerate()) {
                t2_dx = cs2.target().xcoord() - cs2.source().xcoord();
                t2_dy = cs2.target().ycoord() - cs2.source().ycoord();
                cs2_orient = 1;
        }
        else {
                real c2_p_dx = p.xcoord() - cs2.real_center().xcoord();
                real c2_p_dy = p.ycoord() - cs2.real_center().ycoord();
                t2_dx = -c2_p_dy; t2_dy = c2_p_dx;
                cs2_orient = cs2.orientation();
        }

        if (sign(t1_dx) != 0) {
                if (sign(t2_dx) != 0) 
                        // both not vertical
                        return compare(t1_dy/t1_dx, t2_dy/t2_dx);
                else
                        // t1 not vertical, t2 vertical
                        return sign(t2_dy)*cs2_orient > 0 ? -1 : +1; 
                                // determines if slope(t2) is +/- infty
        }
        else {
                int s1 = sign(t1_dy)*cs1_orient;
                        // determines if slope(t1) is +/- infty
                if (sign(t2_dx) != 0) 
                        // t1 vertical, t2 not vertical
                        return s1 > 0 ? +1 : -1;                                
                else {
                        // both vertical
                        int s2 = sign(t2_dy)*cs2_orient;
                        return s1-s2; // == compare(s1,s2);
                }
        }
}

ostream& operator<<(ostream& os, const r_circle_segment& cs)
{
        os << cs.circle() << " ";
//      os << endl;
        r_circle_point::write_point(os, cs.source(), cs.circle(), false);
        os << " ";
//      os << endl;
        r_circle_point::write_point(os, cs.target(), cs.circle(), false);
//      os << endl << endl;
        return os;
}

istream& operator>>(istream& is, r_circle_segment& cs)
{
        r_circle_point src, tgt;
        rat_circle c;
        is >> c;
        r_circle_point::read_point(is, src, c);
        r_circle_point::read_point(is, tgt, c);

        if (src != tgt) 
                cs = r_circle_segment(src, tgt, c);
        else if (c.is_trivial()) 
                cs = r_circle_segment(src);
        else
                cs = r_circle_segment(c);

        return is;
}

/*
__exportF void SWEEP_SEGMENTS(const list<r_circle_segment>& S, 
                                                          GRAPH<r_circle_point,r_circle_segment>& G, 
                                                          bool embed = true);

__exportF void INTERSECT_SEGMENTS_BRUTE_FORCE(const list<r_circle_segment>& S, 
                                                                                          GRAPH<r_circle_point,r_circle_segment>& G);

see _curve_sweep.c
*/

LEDA_END_NAMESPACE 
