/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _curve_sweep_traits.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// S. Thiel (2004)

#include <LEDA/geo/curve_sweep.h> // we need all this test & debugging stuff
#include <LEDA/geo/curve_sweep_traits.h>

#if defined(LEDA_DEBUG)
#include <LEDA/system/assert.h>
#else
#undef assert
#define	assert(cond) ((void)0)
#endif

LEDA_BEGIN_NAMESPACE

/* Documentation of (the requirements for) a traits class

class __exportC curve_sweep_traits_X {
public:
	typedef X CURVE;
	typedef Y CRV_PNT;

//	assumption about the curves:
//	- if two curves touch at a point p, then their relative y-order locally
//	  to the left of p is the same as locally to the right of p.
//	- if two curves c1 and c2 overlap (except for common endpoints), then they
//	  overlap at every common x-coordinate, i.e. if the vertical line x=x0 
//	  intersects both of them, then it intersects them in the same point


// operations of CURVE:
	// CRV_PNT CURVE.source()
	// CRV_PNT CURVE.target()
	// bool    CURVE.is_trivial()
	// bool    CURVE.overlaps(const CURVE& c2) cosnt; // only for debugging
	// bool    CURVE.contains(const CRV_PNT& p) const; // only for debugging
	// bool    operator==(const CURVE& c1, const CURVE& c2); // only for debugging
	// window& operator<<(window&, const CURVE&); // only for visual debugging

// operations of CRV_PNT:
	// point   CRV_PNT.to_point(); // only for visual debugging

// global predicates:
	// int ID_Number(const CURVE&);
		// unique ID needed for map

	// int compare(const CRV_PNT& p, const CRV_PNT& q); 
		// lexicographical comparison (needed for CurveQueue and XStructure)

	// bool identical(const CRV_PNT& p, const CRV_PNT& q);
		// identical(p,q) implies p == q, i.e. p and q have the same coords,
		// but the converse is not necessarily true
		// (The idea is that "idendical" can usually computed much faster than "==".)
		// A possible but slow implementation would be: return compare(p,q) == 0;
		// but usually it is just:                      return ID_Number(p) == ID_number(q);
		// NOTE: could also be a member function of the traits class

// member functions:
public:
	static int compare_intersections_with_sweepline_at(const CRV_PNT& p_sweep, 
		                                               const CURVE& A, const CURVE& B);
		// precond: sweepline intersects A and B
		//          and p_sweep is identical to the source of A or B (or both)

protected:
	void reset();
		// initializes all internal data structures

	void finish();
		// clears all internal data structures

	// for sentinel construction:
	void update_sentinels(const CURVE& crv);
	CURVE get_lower_sentinel() const;
	CURVE get_upper_sentinel() const;
		// conditions: lower_sentinel <_CRV_COMPARE crv <_CRV_COMPARE upper_sentinel
		//             lower_sentinel.source() <_lex crv.source()/target() <_lex upper_sentinel.target()

	static void add_x_monotonous_parts(const CURVE& crv, list<CURVE>& x_monotonous_parts);
		// crv must be the disjoint union the curves in x_montone_parts 
		// (except for common endpoints)
		// every curve c in x_montone_parts must be x-montone and c.source() <=_lex c.target()

	static CURVE make_endpoints_of_curve_identical(const CURVE& crv, 
		                                           const CRV_PNT& src, const CRV_PNT& tgt);
		// precond: src==crv.source() and tgt==crv.target() (i.e. equal coords)
		// the returned curve res and crv must contain exactly the same points and
		// identical(src,res.source()) and identical(tgt,crv.target()) must hold

	CURVE make_trivial_curve(const CRV_PNT& p) const { return CURVE(p); }
		// returns a trivial curve that consists only of the single point p

	static bool have_same_direction(const CURVE& x_mon_part, const CURVE& orig_crv)
	{ return x_mon_part.orientation() == orig_crv.orientation(); }
		// returns true, if x_mon_part and orig_crv have the same direction
		// (and false if they have opposite directions)
		// precond: x_mon_part has been generated from orig_crv by add_x_monotonous_parts

	static bool intersect_right_of_pswp_given_A_leq_pswp_leq_B
		(const CRV_PNT& sweeppos, const CURVE& A, const CURVE& B, CRV_PNT& inter);
		// precond: A and B do not overlap, A.source()/B.source() <=_lex p_sweep
		//          and A <=_CRV_COMPARE B
		// checks A and B intersect in a point p with p >_lex p_sweep
		// (common targets of A and B also HAVE to be reported as intersection)
		// if so, p is assigned to inter

	static bool A_touches_B_at_sweeppos_given_that_A_leq_B
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B);
		// precond: A and B have previously been tested for intersection
		//			(by intersect_right_of_pswp_given_A_leq_pswp_leq_B)
		//			or been marked as touching (by mark_as_touching_at_p)

	void mark_as_touching_at_p(const CRV_PNT& p, const CURVE& A, const CURVE& B);
		// remember that A and B are touching in p

	static bool A_overlaps_B_given_that_As_source_is_on_B(const CURVE& A, const CURVE& B);
		// precond: B.contains(A.source()), A and B continue to the right of A.source()
		// checks if A and B overlap

	static bool A_overlaps_B_given_they_share_three_points(const CURVE& A, const CURVE& B);
		// precond: A and B share three distinct points
		// checks if A and B overlap

	static list<CRV_PNT> intersections(const CURVE& A, const CURVE& B);
		// only needed for brute-force and invariant checking

	static int compare_intersections_with_vertical_line_through_sweeppos
		(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B);
		// precond: the line intersects A and B
		// only needed for invariant checking
		// similar to compare_intersections_with_sweepline_at, but weaker precond.

	static void print_curve(ostream& os, const CURVE& crv);
		// only needed for trace

	static void print_point(ostream& os, const CRV_PNT& p); 
		// only needed for trace

};
*/ // eof traits class documentation

/// curve_sweep_traits_r_circle_segment /////////////////////////////////////////////////////

void 
curve_sweep_traits_r_circle_segment::reset()
{
	Infinity = 1;
	IntersectionCache.clear();

	TIME( CacheMiss = 0; TotalIntersections = 0; );
}

void 
curve_sweep_traits_r_circle_segment::finish()
{
	dic_item it;
	forall_items(it, IntersectionCache) {
		delete IntersectionCache[it];
	}
	IntersectionCache.clear();

	TIME( cout << "Cache: total: " << TotalIntersections
			   << "  /  hits: " << (TotalIntersections-CacheMiss) 
			   << "  / misses: " << CacheMiss << endl );
	TIME( cout << "IntersectionTimer: " << IntersectionTimer << "  /  " );
	TIME( cout << "TimerA: " << TimerA << "  / TimerB: " << TimerB << endl );
}

void 
curve_sweep_traits_r_circle_segment::update_sentinels(const CURVE& crv)
{
	if (! crv.is_degenerate()) {
		real radius_enclosing_sphere 
			= crv.real_center().distance() + crv._real_circle().radius();
	    while (radius_enclosing_sphere >= real(Infinity)) Infinity *= 2;
	}
	else {
		real r_src = crv.source().distance(), r_tgt = crv.target().distance();
	    while (r_src >= real(Infinity)) Infinity *= 2;		
	    while (r_tgt >= real(Infinity)) Infinity *= 2;		
	}

}

curve_sweep_traits_r_circle_segment::CURVE 
curve_sweep_traits_r_circle_segment::get_lower_sentinel() const
{
	return CURVE(rat_point(-Infinity,-Infinity), rat_point(0,-2*Infinity), rat_point(Infinity,-Infinity));
}

curve_sweep_traits_r_circle_segment::CURVE 
curve_sweep_traits_r_circle_segment::get_upper_sentinel() const
{
	return CURVE(rat_point(-Infinity, Infinity), rat_point(0, 2*Infinity), rat_point(Infinity, Infinity));
}


void 
curve_sweep_traits_r_circle_segment::add_x_monotonous_parts
	(const CURVE& crv, list<CURVE>& x_monotonous_parts)
// NOTE: One is tempted to pre-cache intersections between x-monotonous parts, but this
//       does not work because make_endpoints_of_curve_identical (see below) generates
//       new curves.
{
	if (crv.is_trivial()) { 
		// single point=> keep it
		x_monotonous_parts.append(crv); return;
	}

	if (crv.is_straight_segment()) {
		if (compare(crv.source(), crv.target()) <= 0)
			x_monotonous_parts.append(crv);
		else
			x_monotonous_parts.append(crv.reverse());
		return;
	}

	if (crv.is_full_circle()) {
		// full circle => split it in two halfs
		rat_circle circ = crv.circle();
		rat_point center = circ.center();
		rat_line horiz(center, rat_point(center.X()+center.W(), center.Y(), center.W()));
		r_circle_point src(circ, horiz, r_circle_point::first);
		r_circle_point tgt(circ, horiz, r_circle_point::second);

		x_monotonous_parts.append(CURVE(src, tgt, circ));
		x_monotonous_parts.append(CURVE(src, tgt, circ.reverse()));
		return;
	}

	r_circle_point src = crv.source(), tgt = crv.target();
	rat_circle circ = crv.circle();
	if (compare(crv.source(), crv.target()) > 0) {
		src = crv.target(); tgt = crv.source(); circ = circ.reverse();
	}

	// from now (src,tgt,circ) =_set crv; and src <_lex tgt
	assert(src != tgt);

	// compare src and tgt with horizontal through the center of circ
	real_point rc = real_point(circ.center());
	int orient = circ.orientation();
	int cmp_y_src_center = orient * compare(src.ycoord(), rc.ycoord());
	int cmp_y_tgt_center = orient * compare(tgt.ycoord(), rc.ycoord());

	// we may assume that circ is positively oriented (since we multiplied with orient)

	if (cmp_y_src_center <= 0 && cmp_y_tgt_center <= 0) {
		// segment is never above this line => keep it
		x_monotonous_parts.append( r_circle_segment(src, tgt, circ) );
	}
	else {
		rat_point center = circ.center();
		rat_line horiz(center, rat_point(center.X()+center.W(), center.Y(), center.W()));

		r_circle_point leftmost = src;
		if (cmp_y_src_center > 0) {
			leftmost = r_circle_point(circ, horiz, r_circle_point::first);
			x_monotonous_parts.append( r_circle_segment(leftmost, src, circ.reverse()) );
		}

		r_circle_point rightmost = tgt;
		if (cmp_y_tgt_center > 0) {
			rightmost = r_circle_point(circ, horiz, r_circle_point::second);
			x_monotonous_parts.append( r_circle_segment(tgt, rightmost, circ.reverse()) );
		}
		
		x_monotonous_parts.append( r_circle_segment(leftmost, rightmost, circ) );
	}
}

curve_sweep_traits_r_circle_segment::CURVE 
curve_sweep_traits_r_circle_segment::make_endpoints_of_curve_identical
	(const CURVE& crv, const CRV_PNT& src, const CRV_PNT& tgt)
{
	return CURVE(src, tgt, crv.circle());
}

int 
curve_sweep_traits_r_circle_segment::compare_intersections_with_sweepline_at
	(const CRV_PNT& p_sweep, const CURVE& c1, const CURVE& c2)
// Precondition:
// p_sweep is identical to the source of either c1 or c2. 
{
//	int id1 = ID_Number(c1), id2 = ID_Number(c2);
	if (identical(c1, c2)) return 0;

	if (identical(c1.source(), p_sweep)) {
		int cmp = c2.compare_y(p_sweep);
		if (cmp || c1.is_trivial() || c2.is_trivial()) return cmp;
		// maybe optimize if identical(c2.source(),p_sweep) ... (not needed below !!)
	}
	else if (identical(c2.source(), p_sweep)) {
		int cmp = -c1.compare_y(p_sweep);
		if (cmp || c1.is_trivial() || c2.is_trivial()) return cmp;
	}
	else {
		LEDA_EXCEPTION(1, "curve_sweep_traits_r_circle_segment: error in compare");
	}

	int cmp = compare_tangent_slopes(c1, c2, p_sweep);
	if (cmp) return cmp;

//	if (c1.orientation() != c2.orientation()) return c1.orientation();
	cmp = c1.orientation() - c2.orientation();
	if (cmp) return cmp;
	
	if (! c1.is_degenerate())
		cmp = compare(c2.sqr_radius(), c1.sqr_radius()) * c1.orientation();

	return cmp ? cmp : compare(ID_Number(c1), ID_Number(c2));
}

bool 
curve_sweep_traits_r_circle_segment::x_mon_curve_wedge_contains_p
	(const CURVE& crv, const CRV_PNT& p) const
{
	switch (crv.orientation()) {
	case -1:
		return p.ycoord() >= crv.real_center().ycoord()
			&& crv.source().xcoord() <= p.xcoord() && p.xcoord() <= crv.target().xcoord();
	case +1:
		return p.ycoord() <= crv.real_center().ycoord()
			&& crv.source().xcoord() <= p.xcoord() && p.xcoord() <= crv.target().xcoord();
	default:
		if (! crv.is_vertical_segment())
			return crv.source().xcoord() <= p.xcoord() && p.xcoord() <= crv.target().xcoord();
		else
			return crv.source().ycoord() <= p.ycoord() && p.ycoord() <= crv.target().ycoord();
	}
}

int
curve_sweep_traits_r_circle_segment::compute_intersection_curve_line
	(const CURVE& A, const rat_line& l, const CRV_PNT& p_sweep, const CURVE& B, 
	 bool not_strictly_right, CRV_PNT rcp_inter[2]) const
{
	two_tuple<real_point,real_point> inters;
	int num_circ_line_inter = intersection(A.circle(), l, inters);

	if (num_circ_line_inter == 0) return 0;

	if (!identical(A.source(), p_sweep) || !identical(B.source(), p_sweep)) 
	{
		// standard case ...
		int num_inter = 0;

		rcp_inter[0] = r_circle_point(inters.first(), A.circle(), l, r_circle_point::first);
		int cmp = compare(p_sweep.xcoord(), rcp_inter[0].xcoord());
		bool p_sweep_lex_smaller 
			= cmp < 0 || (not_strictly_right && cmp == 0 && p_sweep.ycoord() < rcp_inter[0].ycoord());

		if ( p_sweep_lex_smaller && x_mon_curve_wedges_contain_p(A,B,rcp_inter[0]) )
		{
			++num_inter;
		}

		if (num_circ_line_inter > 1) {
			rcp_inter[num_inter] = r_circle_point(inters.second(), A.circle(), l, r_circle_point::second);
			if ( !p_sweep_lex_smaller ) {
				// implies num_inter == 0
				int cmp = compare(p_sweep.xcoord(), rcp_inter[0].xcoord());
				p_sweep_lex_smaller 
					= cmp < 0 || (not_strictly_right && cmp == 0 && p_sweep.ycoord() < rcp_inter[0].ycoord());
			}

			if ( p_sweep_lex_smaller && x_mon_curve_wedges_contain_p(A,B,rcp_inter[num_inter]) )
			{
				++num_inter;
			}
		}

		// if there is exactly one intersection we check for touching
		if (num_inter == 1)
			rcp_inter[1] = (num_circ_line_inter == 2) ? rcp_inter[0] : TouchingIndicator;

		return num_inter;
	} // eof standard case
	else 
	{
		// case for optimization: p_sweep == A.source() == B.source()
		assert( real_line(l).contains(p_sweep) );
		if (num_circ_line_inter == 1) return 0;

		if (! not_strictly_right) {
			real x_middle = (inters.first().xcoord() + inters.second().xcoord()) / 2;
				// p_sweep.x == second.x >= first.x (no inter to right) => x_middle <= p_sweep.x
				// p_sweep.x == first.x  < second.x (inter to right)    => x_middle >  p_sweep.x
				// NOTE: if l is vertical, then the intersection different from p_sweep
				//       cannot lie on A or B, because they x-monotonous

			if (x_middle <= p_sweep.xcoord()) {
				assert(num_circ_line_inter == 2 && (l.is_vertical() || p_sweep == inters.second()) );
				return 0; // no intersection right of p_sweep
			}
			else {
				// -> p_sweep == inters.first() && p_sweep <_lex inters.second
				assert( p_sweep == inters.first() && compare(p_sweep, inters.second())<0 );
				rcp_inter[0] = rcp_inter[1] = r_circle_point(inters.second(), A.circle(), l, r_circle_point::second);
				return x_mon_curve_wedges_contain_p(A,B,rcp_inter[0]) ? 1 : 0;
			}
		}
		else { // not_strictly_right
			assert(l.is_vertical());

			real y_middle = (inters.first().ycoord() + inters.second().ycoord()) / 2;
			if (y_middle <= p_sweep.ycoord()) {
				return 0; // no intersection above p_sweep
			}
			else {
				// -> p_sweep == inters.first() && p_sweep <_lex inters.second
				assert( p_sweep == inters.first() && compare(p_sweep, inters.second())<0 );
				rcp_inter[0] = rcp_inter[1] = r_circle_point(inters.second(), A.circle(), l, r_circle_point::second);
				return x_mon_curve_wedges_contain_p(A,B,rcp_inter[0]) ? 1 : 0;
			}
		}
	} // eof optimization
}

bool
curve_sweep_traits_r_circle_segment::intersect_right_of_pswp_given_A_leq_pswp_leq_B
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B, CRV_PNT& inter) const
// precond: A and B do not overlap, A.source()/B.source() <=_lex p_sweep
//          and A <=_CRV_COMPARE CURVE(p_sweep) <=_CRV_COMPARE B
// Note: A and B cannot intersect properly above p_sweep
{
	TIME( timer::measure m(IntersectionTimer) );
	TIME( ++TotalIntersections );
	assert( !A.overlaps(B) );

	// it is faster not to cache intersections between straight segments ...
	if (A.is_degenerate() && B.is_degenerate()) {
		TIME( ++CacheMiss );
TIME( timer::measure _mA(TimerA) );
		// both are line segments
		if (A.orientation_of_p_wrt_supporting_line(B.target()) <= 0
			&& B.orientation_of_p_wrt_supporting_line(A.target()) >= 0)
		{
TIME( timer::measure _mB(TimerB) );
			rat_point rat_inter;
			A.supporting_line().intersection(B.supporting_line(), rat_inter);
			inter = r_circle_point(rat_inter);
			return true;
		}
		return false;
	}

	curve_pair key(make_pair(A,B));
	dic_item it = IntersectionCache.lookup(key);
	if (! it) {
		// cache miss -> compute the intersection and cache it
		TIME( ++CacheMiss );
		int num_inter = 0;
		r_circle_point rcp_inter[2];

		if (A.is_degenerate()) {
			// A is line segment, B is not degenerate
			rat_line A_support = A.supporting_line();
			num_inter = compute_intersection_curve_line(B, A_support, p_sweep, A, A_support.is_vertical(), rcp_inter);
		}
		else if (B.is_degenerate()) {
			// B is line segment, A is not degenerate
			rat_line B_support = B.supporting_line();
			num_inter = compute_intersection_curve_line(A, B_support, p_sweep, B, B_support.is_vertical(), rcp_inter);
		}
		else {
			// both A and B are not degenerate
			rat_line rad_axis;
			if (radical_axis(A.circle(), B.circle(), rad_axis)) {
				// radical axis exists -> compute the intersection points with it
				num_inter = compute_intersection_curve_line(A, rad_axis, p_sweep, B, false, rcp_inter);
			}
 			else { // A and B have the same center
 				// A and B might share an endpoint (if the radii are equal) -> CROSSING
 				// since A.source()/B.source() <=_lex p_sweep, they can only have a common
 				// target
 				if (identical(A.target(),B.target())) {
					num_inter = 1;
					rcp_inter[0] = rcp_inter[1] = A.target();
				}
			}
		}

		if (num_inter == 0) {
			IntersectionCache.insert(key, nil); return false;
		}
		IntersectionCache.insert(key, new intersect_result(rcp_inter[0], rcp_inter[1]));
		inter = rcp_inter[0]; return true;
	}

	// cache hit
	intersect_result* p_descr = IntersectionCache[it];
	if (! p_descr) return false;

	if (!identical(p_descr->first(), p_descr->second()) 
		&& !identical(p_descr->second(), TouchingIndicator)) 
	{
		inter = p_descr->first();
//		if (compare(p_sweep,inter)<0) return true;
		if (!identical(p_sweep,inter)) {
			// !identical(...) is redundant, but gives big speed-up
			if ((A.is_vertical_segment() || B.is_vertical_segment()) && p_sweep.ycoord() < inter.ycoord()) return true;
			if (p_sweep.xcoord() < inter.xcoord()) return true;
		}
 		p_descr->first() = p_descr->second();
	}

	inter = p_descr->first();
//	if (compare(p_sweep,inter)<0) return true;
	if (!identical(p_sweep,inter)) {
		// !identical(...) is redundant, but gives big speed-up
		if ((A.is_vertical_segment() || B.is_vertical_segment()) && p_sweep.ycoord() < inter.ycoord()) return true;
		if (p_sweep.xcoord() < inter.xcoord()) return true;
	}
	IntersectionCache[it] = nil;
	delete p_descr;
	return false;
}

bool curve_sweep_traits_r_circle_segment::A_touches_B_at_sweeppos_given_that_A_leq_B
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B)
{
	curve_pair key(make_pair(A,B));
	dic_item it = IntersectionCache.lookup(key);
	if (!it || !IntersectionCache[it]) return false;
	intersect_result& descr = *IntersectionCache[it];
	return identical(descr.second(), TouchingIndicator) && descr.first() == p_sweep;
}

void curve_sweep_traits_r_circle_segment::mark_as_touching_at_p
	(const CRV_PNT& p, const CURVE& A, const CURVE& B)
{
	curve_pair key(make_pair(A,B));
	IntersectionCache.insert(key, new intersect_result(p, TouchingIndicator));
}

bool 
curve_sweep_traits_r_circle_segment::A_overlaps_B_given_that_As_source_is_on_B
	(const CURVE& A, const CURVE& B)
{
	// Observe: orientations must be equal, too
	// (works for degenerate curves, too)
	return A.circle() == B.circle();
}

list<curve_sweep_traits_r_circle_segment::CRV_PNT> 
curve_sweep_traits_r_circle_segment::intersections(const CURVE& A, const CURVE& B) const
{
	TIME( ++TotalIntersections; ++CacheMiss );
	list<CRV_PNT> result = A.intersection(B);
	return result;
}

int
curve_sweep_traits_r_circle_segment::compare_intersections_with_vertical_line_through_sweeppos
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B)
{
	if (A.is_vertical_segment() || B.is_vertical_segment()) return 0;

	real_line vert_line(p_sweep, real_point(p_sweep.xcoord(), p_sweep.ycoord()+1));

	list<real_point> inters;
	inters = A.intersection(vert_line);
	if (inters.size() != 1) { 
		LEDA_EXCEPTION(1, "curve_sweep_traits_r_circle_segment: error comparing y-coords");
		return 0;
	}
	real A_y = inters.head().ycoord();

	inters = B.intersection(vert_line);
	if (inters.size() != 1) { 
		LEDA_EXCEPTION(1, "curve_sweep_traits_r_circle_segment: error comparing y-coords");
		return 0;
	}
	real B_y = inters.head().ycoord();

	return compare(A_y, B_y);
}

void 
curve_sweep_traits_r_circle_segment::print_curve(ostream& os, const CURVE& crv)
{
	os << crv.source().to_point() << "->" << crv.target().to_point();

	if (! crv.is_degenerate()) {
		circle c = crv.circle().to_circle();
		os << " r=" << (c.orientation() > 0 ? "+" : "-") << c.radius();
	}
}

void 
curve_sweep_traits_r_circle_segment::print_point(ostream& os, const CRV_PNT& p)
{
	os << p.to_point();
}

/// colour_sweep_traits_mc_r_circle_segment /////////////////////////////////////////////////

colour_sweep_traits_mc_r_circle_segment::CURVE 
colour_sweep_traits_mc_r_circle_segment::get_lower_sentinel()
{
	CURVE res = base::get_lower_sentinel();
	Colour[res] = ~(0UL); // sentinel does not intersect 
                              // any other curve by construction
	return res;
}

colour_sweep_traits_mc_r_circle_segment::CURVE 
colour_sweep_traits_mc_r_circle_segment::get_upper_sentinel()
{
	CURVE res = base::get_upper_sentinel();
	Colour[res] = ~(0UL); // sentinel does not intersect 
                              // any other curve by construction
	return res;
}

void 
colour_sweep_traits_mc_r_circle_segment::add_x_monotonous_parts
	(const CURVE& crv, list<CURVE>& x_monotonous_parts)
{
	list<CURVE> x_mon_parts;
	base::add_x_monotonous_parts(crv, x_mon_parts);


	CURVE x_mon_part;
        // work-around for a bug in ch_map  (sn 12/2007)
	//forall(x_mon_part, x_mon_parts) Colour[x_mon_part] = Colour[crv];
        unsigned long crv_clr = Colour[crv];
	forall(x_mon_part, x_mon_parts) Colour[x_mon_part] = crv_clr; 

	x_monotonous_parts.conc(x_mon_parts);
}

colour_sweep_traits_mc_r_circle_segment::CURVE 
colour_sweep_traits_mc_r_circle_segment::make_endpoints_of_curve_identical
	(const CURVE& crv, const CRV_PNT& src, const CRV_PNT& tgt)
{
	CURVE res = base::make_endpoints_of_curve_identical(crv, src, tgt);
	//Colour[res] = Colour[crv];
        unsigned long crv_clr = Colour[crv];
	Colour[res] = crv_clr;
	return res;
}

bool 
colour_sweep_traits_mc_r_circle_segment::intersect_right_of_pswp_given_A_leq_pswp_leq_B
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B, CRV_PNT& inter) const
{
	if ( (Colour[A] & Colour[B]) != 0 ) {
		if (! identical(A.target(), B.target())) return false;
		inter = A.target();
		return compare(p_sweep, inter) < 0;
	}

	return base::intersect_right_of_pswp_given_A_leq_pswp_leq_B(p_sweep, A, B, inter);
}

bool 
colour_sweep_traits_mc_r_circle_segment::A_touches_B_at_sweeppos_given_that_A_leq_B
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B)
{
	if ( (Colour[A] & Colour[B]) != 0 ) return false;

	return base::A_touches_B_at_sweeppos_given_that_A_leq_B(p_sweep, A, B);
}

bool 
colour_sweep_traits_mc_r_circle_segment::A_overlaps_B_given_that_As_source_is_on_B
	(const CURVE& A, const CURVE& B) const
{
	if ( (Colour[A] & Colour[B]) != 0 ) return false;

	return base::A_overlaps_B_given_that_As_source_is_on_B(A, B);
}

/// colour_sweep_traits_sc_r_circle_segment /////////////////////////////////////////////////

void 
colour_sweep_traits_sc_r_circle_segment::add_x_monotonous_parts
	(const CURVE& crv, list<CURVE>& x_monotonous_parts)
{
	list<CURVE> x_mon_parts;
	base::add_x_monotonous_parts(crv, x_mon_parts);

	CURVE x_mon_part;
	//forall(x_mon_part, x_mon_parts) Colour[x_mon_part] = Colour[crv];
        unsigned long crv_clr = Colour[crv];
	forall(x_mon_part, x_mon_parts) Colour[x_mon_part] = crv_clr; 

	x_monotonous_parts.conc(x_mon_parts);
}

colour_sweep_traits_sc_r_circle_segment::CURVE 
colour_sweep_traits_sc_r_circle_segment::make_endpoints_of_curve_identical
	(const CURVE& crv, const CRV_PNT& src, const CRV_PNT& tgt)
{
	CURVE res = base::make_endpoints_of_curve_identical(crv, src, tgt);
	//Colour[res] = Colour[crv];
        unsigned long crv_clr = Colour[crv];
	Colour[res] = crv_clr;
	return res;
}

bool 
colour_sweep_traits_sc_r_circle_segment::intersect_right_of_pswp_given_A_leq_pswp_leq_B
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B, CRV_PNT& inter) const
{
	if ( Colour[A] == Colour[B] ) {
		if (! identical(A.target(), B.target())) return false;
		inter = A.target();
		return compare(p_sweep, inter) < 0;
	}

	return base::intersect_right_of_pswp_given_A_leq_pswp_leq_B(p_sweep, A, B, inter);
}

bool 
colour_sweep_traits_sc_r_circle_segment::A_touches_B_at_sweeppos_given_that_A_leq_B
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B)
{
	if ( Colour[A] == Colour[B] ) return false;

	return base::A_touches_B_at_sweeppos_given_that_A_leq_B(p_sweep, A, B);
}

bool 
colour_sweep_traits_sc_r_circle_segment::A_overlaps_B_given_that_As_source_is_on_B
	(const CURVE& A, const CURVE& B) const
{
	if ( Colour[A] == Colour[B] ) return false;

	return base::A_overlaps_B_given_that_As_source_is_on_B(A, B);
}

/// curve_sweep_traits_rat_segment //////////////////////////////////////////////////////////

void 
curve_sweep_traits_rat_segment::reset()
{
	Infinity = 1;
}

void 
curve_sweep_traits_rat_segment::update_sentinels(const CURVE& crv)
{
    COORD x1 = crv.xcoord1(); 
    COORD y1 = crv.ycoord1(); 
    COORD x2 = crv.xcoord2(); 
    COORD y2 = crv.ycoord2(); 
    if (x1 < 0) x1 = -x1;
    if (y1 < 0) y1 = -y1;
    if (x2 < 0) x2 = -x2;
    if (y2 < 0) y2 = -y2;

    while (x1 >= Infinity || y1 >= Infinity || 
           x2 >= Infinity || y2 >= Infinity )   Infinity *= 2;
}

curve_sweep_traits_rat_segment::CURVE 
curve_sweep_traits_rat_segment::get_lower_sentinel() const
{
	return CURVE(-Infinity,-Infinity,Infinity,-Infinity);
}

curve_sweep_traits_rat_segment::CURVE 
curve_sweep_traits_rat_segment::get_upper_sentinel() const
{
	return CURVE(-Infinity, Infinity,Infinity, Infinity);
}

int 
curve_sweep_traits_rat_segment::compare_intersections_with_sweepline_at
	(const CRV_PNT& p_sweep, const CURVE& s1, const CURVE& s2)
{ // Precondition:
  // p_sweep is identical to the left endpoint of either s1 or s2. 
 
	if (identical(s1,s2)) return 0;

	int s = 0;

	if ( identical(p_sweep,s1.source()) )
		s = orientation(s2,p_sweep);
	else if ( identical(p_sweep,s2.source()) )
		s = -orientation(s1,p_sweep);
	else 
		LEDA_EXCEPTION(1,"compare error in sweep");

	if (s || s1.is_trivial() || s2.is_trivial()) return s;

	s = orientation(s2,s1.target());

	// overlapping segments will be ordered by their ID_numbers :

	return s ? s : (ID_Number(s1) - ID_Number(s2));
}


void 
curve_sweep_traits_rat_segment::add_x_monotonous_parts
	(const CURVE& crv, list<CURVE>& x_monotonous_parts)
{
	if (compare(crv.source(), crv.target()) <= 0)
		x_monotonous_parts.append(crv);
	else
		x_monotonous_parts.append(crv.reverse());
}

curve_sweep_traits_rat_segment::CURVE 
curve_sweep_traits_rat_segment::make_endpoints_of_curve_identical
	(const CURVE& crv, const CRV_PNT& src, const CRV_PNT& tgt)
{
	return CURVE(src, tgt);
}

bool
curve_sweep_traits_rat_segment::intersect_right_of_pswp_given_A_leq_pswp_leq_B
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B, CRV_PNT& inter)
{
	// precond: A and B do not overlap and A <=_CRV_COMAPRE B
	if ( orientation(A,B.target()) <= 0 && orientation(B,A.target()) >= 0 )
	{ 
		A.intersection_of_lines(B, inter);
		return true;
	}
	return false;
}

bool 
curve_sweep_traits_rat_segment::A_overlaps_B_given_that_As_source_is_on_B(const CURVE& A, const CURVE& B)
{
	return orientation(B, A.target()) == 0;
}

list<curve_sweep_traits_rat_segment::CRV_PNT> 
curve_sweep_traits_rat_segment::intersections(const CURVE& A, const CURVE& B)
{
	CRV_PNT inter;
	list<CRV_PNT> result;
	if (A.intersection(B, inter)) result.append(inter);
	return result;
}

int
curve_sweep_traits_rat_segment::compare_intersections_with_vertical_line_through_sweeppos
	(const CRV_PNT& p_sweep, const CURVE& A, const CURVE& B)
{
	rat_line vert_line(p_sweep, rat_point(p_sweep.xcoord(), p_sweep.ycoord()+1));

	rat_point inter;
	if (!A.is_vertical()) {
		if (!vert_line.intersection(A, inter)) { 
			LEDA_EXCEPTION(1, "curve_sweep_traits_rat_segment: error comparing y-coords");
			return 0;
		}
	}
	else if (A.source().xcoord() != p_sweep.xcoord()) {
		LEDA_EXCEPTION(1, "curve_sweep_traits_rat_segment: vert. A not on vert. line");
		return 0;
	}
	rational A_y = inter.ycoord();

	if (!B.is_vertical()) {
		if (!vert_line.intersection(B, inter)) { 
			LEDA_EXCEPTION(1, "curve_sweep_traits_rat_segment: error comparing y-coords");
			return 0;
		}
	}
	else if (B.source().xcoord() != p_sweep.xcoord()) {
		LEDA_EXCEPTION(1, "curve_sweep_traits_rat_segment: vert. B not on vert. line");
		return 0;
	}
	rational B_y = inter.ycoord();

	if (!A.is_vertical()) {
		if (!B.is_vertical()) {
			return compare(A_y, B_y);
		}
		else {
			if (A_y < B.source().ycoord()) return -1;
			if (A_y > B.target().ycoord()) return +1;
			return 0; // intersection on vert. segment
		}
	}
	else {
		if (!B.is_vertical()) {
			if (B_y < A.source().ycoord()) return +1;
			if (B_y > A.target().ycoord()) return -1;
			return 0; // intersection on vert. segment
		}
		else { // both vertical
			if (A.target().ycoord() < B.source().ycoord()) return -1;
			if (B.target().ycoord() < A.source().ycoord()) return +1;
			return 0; // they overlap
		}
	}
}

void 
curve_sweep_traits_rat_segment::print_curve(ostream& os, const CURVE& crv)
{
	os << crv.to_segment();
}

void 
curve_sweep_traits_rat_segment::print_point(ostream& os, const CRV_PNT& p)
{
	os << p.to_point();
}

LEDA_END_NAMESPACE
