/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _r_circle_point.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/r_circle_point.h>
#include <ctype.h> // for operator>>
/*
#include <LEDA/system/timer.h>
#include <LEDA/system/counter.h>
*/

//------------------------------------------------------------------------------
// r_circle_point
//
// S. Thiel (2004)
//------------------------------------------------------------------------------

#if defined(LEDA_DEBUG)
#include <LEDA/system/assert.h>
#else
#undef assert
#define	assert(cond) ((void)0)
#endif

LEDA_BEGIN_NAMESPACE 

/// r_circle_point ///////////////////////////////////////////////////////////////////////////

/*
representation:
  rat_point:	  !identical(RatPoint, dummy_rat_point())
		  identical(SupportCircle, circle_for_default_ctor())
		  identical(SupportLine, line_for_default_ctor())
  non rat_point:  identical(RatPoint, dummy_rat_point())
*/

#if defined(__APPLE__)
const real_point r_circle_point::real_point_for_default_ctor() { 
  return real_point(0, 0);
}

const rat_circle r_circle_point::circle_for_default_ctor()
{ // vertical line through (0,0)
  return rat_circle(rat_point(0,-1),rat_point(0,0),rat_point(0,1));
}

const rat_line r_circle_point::line_for_default_ctor()
{ // horizontal line through (0,0)
  return rat_line(rat_point(0,0), rat_point(1,0));
}

const rat_point r_circle_point::dummy_rat_point()
{ return rat_point(-11,11); }

#else

const real_point& r_circle_point::real_point_for_default_ctor()
{ static real_point p(0, 0);
  return p;
}

const rat_circle& r_circle_point::circle_for_default_ctor()
{ // vertical line through (0,0)
  static rat_circle c(rat_point(0,-1),rat_point(0,0),rat_point(0,1));
  return c;
}

const rat_line& r_circle_point::line_for_default_ctor()
{ // horizontal line through (0,0)
  static rat_line l(rat_point(0,0), rat_point(1,0));
  return l;
}

const rat_point& r_circle_point::dummy_rat_point()
{ static rat_point dummy(-11,11);
  return dummy;
}

#endif


#ifndef LEDA_R_CIRCLE_SAVE_MEM
r_circle_point::r_circle_point(const rat_point& rat_pnt)
 : base(rat_pnt), 
   SupportCircle(circle_for_default_ctor()),
   SupportLine(line_for_default_ctor()),
   RatPoint(rat_pnt)
{}

r_circle_point::r_circle_point(const point& pnt)
 : base(pnt), 
   SupportCircle(circle_for_default_ctor()),
   SupportLine(line_for_default_ctor()),
   RatPoint(rat_point(pnt, 0)) // exact conversion to rational!
{}

#else

r_circle_point::r_circle_point(const rat_point& rat_pnt) : base(rat_pnt), 
                                                           RatPoint(rat_pnt)
{}

r_circle_point::r_circle_point(const point& pnt) : base(pnt), 
                                                   RatPoint(rat_point(pnt, 0))
{}

#endif

void 
r_circle_point::normalize()
{
  if (is_rat_point()) { RatPoint.normalize(); return; }

#ifndef LEDA_R_CIRCLE_SAVE_MEM
  SupportCircle.normalize();
  SupportLine.normalize();

  two_tuple<real_point,real_point> inters;
  int num_inters = leda::intersection(supporting_circle(), 
                                           supporting_line(), inters);
  assert(num_inters > 0);
  if (num_inters == 1 || inters.first() == ((base&)*this))
    ((base&)*this) = inters.first();
  else
    ((base&)*this) = inters.second();
#endif
}

bool r_circle_point::verify(bool report_errors) const
{
  bool ok = true;
#ifndef LEDA_R_CIRCLE_SAVE_MEM
  real_point p;
  ok = intersection(supporting_circle(), supporting_line(), 
                                          which_intersection(), p);
  ok &= ((base&)*this) == p;
  if (!ok) 
     LEDA_EXCEPTION(1, "r_circle_point::verify: bad representation");
#endif
   return ok;
}

#ifndef LEDA_R_CIRCLE_SAVE_MEM
rat_circle r_circle_point::supporting_circle() const
{ if (identical(SupportCircle, circle_for_default_ctor()) && is_rat_point()) 
  { integer X = RatPoint.X();
    integer Y = RatPoint.Y();
    integer W = RatPoint.W();
    SupportCircle = rat_circle(rat_point(X,Y-W,W),RatPoint,rat_point(X,Y+W,W));
   }
  return SupportCircle;
}

rat_line r_circle_point::supporting_line() const
{ if (identical(SupportLine, line_for_default_ctor()) && is_rat_point()) 
  { integer X = RatPoint.X();
    integer Y = RatPoint.Y();
    integer W = RatPoint.W();
    SupportLine = rat_line(RatPoint, rat_point(X+W,Y,W));
   }
  return SupportLine;
}
#endif

rat_point 
r_circle_point::approximate_by_rat_point() const
{ if (is_rat_point()) 
     return to_rat_point();
   else
     return rat_point(to_float());
}

r_circle_point r_circle_point::round(int prec) const
{ return r_circle_point( rat_point(to_float(), prec) ); }

r_circle_point r_circle_point::translate(rational dx, rational dy) const
{
  if (is_rat_point())  
      return r_circle_point(RatPoint.translate(dx, dy));

  real_point new_rp   = base::translate(real(dx), real(dy));

#ifndef LEDA_R_CIRCLE_SAVE_MEM

  rat_circle new_circ = supporting_circle().translate(dx, dy);
  rat_line   new_line = supporting_line().translate(dx, dy);
  return r_circle_point(new_rp, new_circ, new_line); // special private ctor

#else

  return r_circle_point(new_rp); // special private ctor

#endif
}

r_circle_point 
r_circle_point::rotate90(const rat_point& q, int i) const
{
	if (is_rat_point()) {
		return r_circle_point(RatPoint.rotate90(q, i));
	}
	else {
		real_point new_rp   = base::rotate90(real_point(q), i);
#ifndef LEDA_R_CIRCLE_SAVE_MEM
		rat_circle new_circ = supporting_circle().rotate90(q, i);
		rat_line   new_line = supporting_line().rotate90(q, i);
		return r_circle_point(new_rp, new_circ, new_line); // special private ctor
#else
		return r_circle_point(new_rp); // special private ctor
#endif
	}
}

r_circle_point 
r_circle_point::reflect(const rat_point& p, const rat_point& q) const
{
	if (is_rat_point()) {
		return r_circle_point(RatPoint.reflect(p, q));
	}
	else {
		real_point new_rp   = base::reflect(real_point(p), real_point(q));
#ifndef LEDA_R_CIRCLE_SAVE_MEM
		rat_circle new_circ = supporting_circle().reflect(p, q);
		rat_line   new_line = supporting_line().reflect(p, q);
		return r_circle_point(new_rp, new_circ, new_line); // special private ctor
#else
		return r_circle_point(new_rp); // special private ctor
#endif
	}
}

r_circle_point 
r_circle_point::reflect(const rat_point& p) const
{
	if (is_rat_point()) {
		return r_circle_point(RatPoint.reflect(p));
	}
	else {
		real_point new_rp   = base::reflect(real_point(p));
#ifndef LEDA_R_CIRCLE_SAVE_MEM
		rat_circle new_circ = supporting_circle().reflect(p);
		rat_line   new_line = supporting_line().reflect(p);
		return r_circle_point(new_rp, new_circ, new_line); // special private ctor
#else
		return r_circle_point(new_rp); // special private ctor
#endif
	}
}


bool 
r_circle_point::intersection(const rat_circle& c, const rat_line& l, tag which, real_point& p)
{
	two_tuple<real_point,real_point> inters;
	int num_inters = leda::intersection(c, l, inters);
	if (num_inters == 0 || (which == second && num_inters < 2)) return false;
	p = (which == first) ? inters.first() : inters.second();
	return true;
}

bool 
r_circle_point::intersection(const rat_circle& c, const rat_line& l, tag which, r_circle_point& rcp)
{
	real_point p;
	bool ok = intersection(c, l, which, p);
	if (ok) rcp = r_circle_point(p, c, l, which);
	return ok;
}

int 
r_circle_point::intersection(const rat_circle& c, const rat_line& l, 
							 two_tuple<r_circle_point,r_circle_point>& inters)
{
	two_tuple<real_point,real_point> real_inters;
	int num_inters = leda::intersection(c, l, real_inters);
	if (num_inters >= 1) 
		inters.first()  = r_circle_point(real_inters.first(), c, l, first);
	if (num_inters >= 2) 
		inters.second() = r_circle_point(real_inters.second(), c, l, second);
	return num_inters;
}

r_circle_point::tag 
r_circle_point::which_intersection(const rat_circle& c, const rat_line& l, const r_circle_point& p)
{
	two_tuple<real_point,real_point> inters;
	int num_inters = leda::intersection(c, l, inters);

	return (num_inters == 2 && inters.second() == p) ? second : first;
}

void 
r_circle_point::write_point(ostream& os, const r_circle_point& p, const rat_circle& _c, bool write_circle)
{
	if (p.is_rat_point()) {
		os << "$" << p.to_rat_point() << "$";
	}
	else {
#ifndef LEDA_R_CIRCLE_SAVE_MEM
		rat_circle c(_c);
		rat_circle sc(p.supporting_circle());
		rat_line   sl(p.supporting_line());

		// if equal_as_sets(c, sl) || c is trivial, we have to write sc instead of c
		if ( !write_circle && !identical(c, sc) 
			 && ( (c.is_line() && equal_as_sets(c.to_line(), sl)) || c.is_trivial() ) )
		{
			write_circle = true; c = sc;
		}

		if (write_circle)
			os << "/" << c << " ";
		else
			os << "{";

		int which = which_intersection(c, sl, p);
		os << sl << " " << which;

		if (write_circle)
			os << "/";
		else
			os << "}";
#else
		os << "* " << p.to_float() << " *";
#endif
	}
}

void 
r_circle_point::read_point(istream& is, r_circle_point& p, const rat_circle& c)
{
	char ch;

	do is.get(ch); while (is && isspace(ch));
	if (!is) return;

	if (ch != '{' && ch != '*') { is.putback(ch); is >> p; return; }
	
	if (ch == '{') {
		rat_line l;
		int which;
		is >> l >> which;

		do is.get(ch); while (is && isspace(ch));
		if (ch != '}') is.putback(ch);

		if (! r_circle_point::intersection(c, l, r_circle_point::tag(which), p)) {
			LEDA_EXCEPTION(1, "r_circle_point::read_point: line does not intersect circle");
		}
	}
	else /* ch == '*' */ {
		// we have a point with (possibly) inexact double coords, 
		// so we compute the closest point on c
		do is.get(ch); while (is && isspace(ch));
		if (!is) return;

		point p_dbl;
		is >> p_dbl;

		do is.get(ch); while (is && isspace(ch));
		if (ch != '*') is.putback(ch);

		rat_point p_rat(p_dbl);
		if (! c.is_degenerate()) {
			rat_line l;
			if (p_rat != c.center())
				l = rat_line(p_rat, c.center());
			else
				l = rat_line(c.center(), rat_vector(rational(0), rational(1)));
			r_circle_point::intersection(c, l, first, p);
		}
		else {
			rat_line lc(c.point1(), c.point3());
			rat_segment s = lc.perpendicular(p_rat);
			p = r_circle_point(s.target());
		}
	}
}


ostream& operator<<(ostream& os, const r_circle_point& p)
{
#ifndef LEDA_R_CIRCLE_SAVE_MEM
	r_circle_point::write_point(os, p, p.supporting_circle(), true);
#else
	r_circle_point::write_point(os, p, rat_circle(), false);
#endif
	return os;
}

istream& operator>>(istream& is, r_circle_point& p)
{
	char ch;

	do is.get(ch); while (is && isspace(ch));
	if (!is) return is;

	if (ch == '/') {
		rat_circle c;
		rat_line l;
		int which;
		is >> c >> l >> which;
		if (! r_circle_point::intersection(c, l, r_circle_point::tag(which), p)) {
			LEDA_EXCEPTION(1, "operator>>(r_circle_point): line does not intersect circle");
		}
	}
	else if (ch == '$') {
		rat_point rp;
		is >> rp;
		p = r_circle_point(rp);
	}
	else {
		LEDA_EXCEPTION(1, "operator>>(r_circle_point): wrong format");
	}

	do is.get(ch); while (is && isspace(ch));
	if (ch != '/' && ch != '$') is.putback(ch);

	return is;
}

LEDA_END_NAMESPACE 
