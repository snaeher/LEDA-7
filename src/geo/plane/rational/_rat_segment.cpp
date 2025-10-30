/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _rat_segment.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


 
//------------------------------------------------------------------------------
// rat_segment : segments with rational coordinates
//
// by S. Naeher (1995,1996)
//------------------------------------------------------------------------------

#include <LEDA/geo/rat_segment.h>
#include <LEDA/numbers/fp.h>


#if __GNUC__ == 3 && __GNUC_MINOR__ == 0 && __GNUC_PATCHLEVEL__ <= 2
#define FABS(x) x = fp::abs(x)
#else
// causes internal compiler error in g++-3.0.1 (s.n. August 2001)
// and 3.0.2 (november 2001)
#define FABS(x) fp::clear_sign_bit(x)
#endif

LEDA_BEGIN_NAMESPACE 

atomic_counter rat_segment_rep::id_counter(0);

rat_segment_rep::rat_segment_rep(const rat_point& p, const rat_point& q) : start(p),end(q)
{ 
  double pwd = p.WD(); 
  double qwd = q.WD(); 

  if (pwd == 1 && qwd == 1)
  { dxd = q.XD() - p.XD();
    dyd = q.YD() - p.YD();
   }
  else
  { integer dx = q.X()*p.W() - p.X()*q.W();
    integer dy = q.Y()*p.W() - p.Y()*q.W();
    dxd = dx.to_double();
    dyd = dy.to_double();
   }

  id  = ++id_counter; 
}

rat_segment_rep::rat_segment_rep() : start(0,0,1), end(0,0,1)
{ //dx  = 0;
  //dy  = 0;
  dxd = 0;
  dyd = 0;
  id  = ++id_counter; 
 }

rat_segment::rat_segment(const rat_point& x, const rat_vector& v) 
{ PTR = new rat_segment_rep(x,x+v); }
  

int rat_segment::use_filter = 1;


segment rat_segment::to_segment() const
{ return segment(ptr()->start.to_point(),ptr()->end.to_point()); }


rat_segment rat_segment::reversal() const
{ return rat_segment(ptr()->end,ptr()->start); }


rat_segment rat_segment::translate(const integer& dx, const integer& dy, const integer& dw) const
{ rat_point p = ptr()->start.translate(dx,dy,dw);
  rat_point q = ptr()->end.translate(dx,dy,dw);
  return rat_segment(p,q);
 }

rat_segment rat_segment::translate(const rational& dx, const rational& dy) const
{ rat_point p = ptr()->start.translate(dx,dy);
  rat_point q = ptr()->end.translate(dx,dy);
  return rat_segment(p,q);
}

rat_segment rat_segment::translate(const rat_vector& v) const
{ rat_point p = ptr()->start.translate(v);
  rat_point q = ptr()->end.translate(v);
  return rat_segment(p,q);
 }


rat_segment rat_segment::rotate90(const rat_point& q, int i) const
{  return rat_segment(start().rotate90(q,i),end().rotate90(q,i)); }

rat_segment rat_segment::rotate90(int i) const
{ return rotate90(start(),i); }
 



rat_segment rat_segment::reflect(const rat_point& p, const rat_point& q) const
{ return rat_segment(start().reflect(p,q),end().reflect(p,q)); }

rat_segment rat_segment::reflect(const rat_point& p) const
{ return rat_segment(start().reflect(p),end().reflect(p)); }



rational rat_segment::sqr_length() const
{ integer x = dx();
  integer y = dy();
  integer w = W1()*W2();
  return rational(x*x+y*y,w*w);
 }


ostream& operator<<(ostream& out, const rat_segment& s) 
{ out << "[" << s.start() << "===" << s.end() << "]"; 
  return out;
 } 

istream& operator>>(istream& in, rat_segment& s) 
{ // syntax: {[} p {===} q {]}

  rat_point p,q; 
  char c;

  do in.get(c); while (isspace(c));
  if (c != '[') in.putback(c);

  in >> p;

  do in.get(c); while (isspace(c));
  while (c== '=') in.get(c);
  while (isspace(c)) in.get(c);
  in.putback(c);

  in >> q; 

  do in.get(c); while (c == ' ');
  if (c != ']') in.putback(c);

  s = rat_segment(p,q); 
  return in; 

 } 


bool rat_segment::contains(const rat_point& p) const
{ rat_point a = source();
  rat_point b = target();
  if (a == b) return a == p;
  return a.orientation(b,p) == 0 && (b-a)*(p-a) >=0 && (a-b)*(p-b) >= 0;
 }
 

bool rat_segment::intersection(const rat_segment& t) const
{ // decides whether this and |t| intersect
  if (   is_trivial() ) return t.contains(  source());
  if ( t.is_trivial() ) return   contains(t.source());
   
  int o1 = orientation(t.start()); 
  int o2 = orientation(t.end());
  int o3 = t.orientation(start());
  int o4 = t.orientation(end());

  if ( o1 == 0 && o2 == 0 )
     // same slope
     return ( t.contains(  source()) || t.contains(  target()) ||
                contains(t.source()) ||   contains(t.target())    );
  else 
     return o1 != o2 && o3 != o4;
}



bool rat_segment::intersection(const rat_segment& t, rat_point& I) const
{ rat_point a = source();
  rat_point b = target();
  rat_point c = t.source();
  rat_point d = t.target();

  if (is_trivial())
  { bool C = t.contains(a);
    if (C) I = a;
    return C;
   }

  if ( t.is_trivial() )
  { bool C = contains(c);
    if (C) I = c;
    return C;
   }

   
  int o1 = orientation(c); 
  int o2 = orientation(d);
  int o3 = t.orientation(a);
  int o4 = t.orientation(b);

  if ( o1 == 0 && o2 == 0 )
  { // same slope
    if (t.contains(a)) { I = a; return true; }
    if (t.contains(b)) { I = b; return true; }
    if (contains(c))   { I = c; return true; }
    if (contains(d))   { I = d; return true; }
    return false;
  }
  
  if ( o1 != o2 && o3 != o4 )
  { integer w  =  dy()*t.dx()-t.dy()*dx();
    integer c1 = b.X()*a.Y() - a.X()*b.Y();
    integer c2 = d.X()*c.Y() - c.X()*d.Y();
    I = rat_point(c2*dx()-c1*t.dx(), c2*dy()-c1*t.dy(), w);
    return true;
  }

  return false;
}
 

bool rat_segment::intersection(const rat_segment& t, rat_segment& I) const
{ if (   is_trivial() )
  { if ( t.contains(  source()) )
       { I = rat_segment(source(),source()); return true; }
    else
       return false;
  }
  if ( t.is_trivial() ) 
  { if ( contains(t.source()) )
       { I = rat_segment(t.source(),t.source()); return true; }
    else 
       return false;
  }
   
  int o1 = orientation(t.start()); 
  int o2 = orientation(t.end());
  int o3 = t.orientation(start());
  int o4 = t.orientation(end());

  if ( o1 == 0 && o2 == 0 )
  { rat_point sa = source(); 
    rat_point sb = target();
    if ( compare (sa,sb) > 0 )
       { rat_point h = sa; sa = sb; sb = h; }

    rat_point ta = t.source(); rat_point tb = t.target();

    if ( compare (ta,tb) > 0 )
       { rat_point h = ta; ta = tb; tb = h; }

    rat_point a = sa;
    if (compare(sa,ta) < 0) a = ta;

    rat_point b = tb; 
    if (compare(sb,tb) < 0) b = sb;

    if ( compare(a,b) <= 0 )
    { I = rat_segment(a,b);
      return true;
    }
    return false;
  }
  
  if ( o1 != o2 && o3 != o4 )
  { integer w  =   dy()*t.dx() - t.dy()*dx();
    integer c1 =   X2()*  Y1() -   X1()*  Y2();
    integer c2 = t.X2()*t.Y1() - t.X1()*t.Y2();

    rat_point p(c2*dx()-c1*t.dx(), c2*dy()-c1*t.dy(), w);
    I = rat_segment(p,p);
    return true;
  }
  return false;
}
 

bool rat_segment::intersection_of_lines(const rat_segment& t, rat_point& inter) const
{ 
  /* decides whether the lines induced by |s| and |this| segment 
     intersect and, if so, returns the intersection in |inter|. 
     It is assumed that both segments have non-zero length
   */

  integer w = dy()*t.dx() - dx()*t.dy();

  if (w == 0) //same slope
    return false;

  integer c1 =   X2()*  Y1() -   X1()*  Y2();
  integer c2 = t.X2()*t.Y1() - t.X1()*t.Y2();

  inter = rat_point(c2*dx()-c1*t.dx(), c2*dy()-c1*t.dy(), w);

  return true;
}


const double seg_eps0 = ldexp(1.0,-53);

int cmp_slopes(const rat_segment& s1, const rat_segment& s2)
{ 
  if (rat_segment::use_filter)
  { if ( s2.dxD() == 0 || s1.dxD() == 0 )
    { // one of the lines is vertical
      if ( s2.dxD() == 0 && s1.dxD() == 0 ) return 0;
      if ( s2.dxD() == 0) return -1;  // s1 has smaller slope
      return +1;  // s2 has smaller slope
     }

    double dy1dx2 = s1.dyD()*s2.dxD();
    double dy2dx1 = s2.dyD()*s1.dxD();
    double E = dy1dx2 - dy2dx1; 
    if ( s1.dxD() * s2.dxD() < 0 ) E = -E;

    //-----------------------------------------------------------------------
    //  ERROR BOUNDS
    //-----------------------------------------------------------------------
    // mes(E) = 2 * (mes(dy1*dx2) + mes(dy2*dx2))
    //        = 2 * (mes(dy1)*mes(dx2) + mes(dy2)*mes(dx2))
    //        = 2 * (4*fabs(dy1dx2) + 4*fabs(dy2dx2))
    //        = 8 * (fabs(dy1dx2) + fabs(dy2dx2))
    //
    // ind(E) = (ind(dy1*dx2) + ind(dy2*dx2) + 1)/2
    //        = (ind(dy1) + ind(dx2) + 0.5 + ind(dy2) + ind(dx2) + 0.5 + 1)/2
    //        = (0.5 + 0.5 + 0.5 + 0.5 + 0.5 + 0.5 + 1)/2
    //        = 2
    //
    // eps(E) = ind(E) * mes(E) * eps0
    //        = 16 * (fabs(dy1dx2) + fabs(dy2dx2)) * eps0
    //-----------------------------------------------------------------------
  
    FABS(dy1dx2);
    FABS(dy2dx1);
    double eps = 16 * (dy1dx2+dy2dx1) * seg_eps0;
  
#if defined(__win32__)
      if (fp::is_finite(E) && fp::is_finite(eps))
#endif
      { if (E >  eps) return  1;
        if (E < -eps) return -1;
        if (eps < 1)  return 0;
       }
  }
  
  // use big integers
  if ( s2.dx() == 0 || s1.dx() == 0 )
  { // one of the lines is vertical
    if ( s2.dx() == 0 && s1.dx() == 0 ) return 0;
    if ( s2.dx() == 0) return -1;  // s1 has smaller slope
    return +1;  // s2 has smaller slope
  }
  integer E = s1.dy() * s2.dx() - s2.dy() * s1.dx();
  int s = sign(E);
  return ( sign(s1.dx()) * sign(s2.dx()) < 0 ) ? -s : s;
}

int rat_segment::cmp_slope(const rat_segment& s1) const
{ return cmp_slopes(*this,s1); }


rational rat_segment::y_proj(rational x) const
{ return  ycoord1() - slope() * (xcoord1() - x); }

rational rat_segment::x_proj(rational y) const
{ if (is_vertical())  
       return  xcoord1();
  else
       return  xcoord1() - (ycoord1() - y)/slope(); 
}

rational rat_segment::y_abs() const 
{ return ycoord1() - slope()*xcoord1(); }


int rat_segment::orientation(const rat_point& p) const
{ 
  const rat_segment& s = *this;

  rat_point a = s.start();

  if (rat_segment::use_filter)
  { 
    double dx = s.dxD();
    double dy = s.dyD();
    double axpw = a.XD() * p.WD();
    double aypw = a.YD() * p.WD();
    double pxaw = p.XD() * a.WD();
    double pyaw = p.YD() * a.WD();
  
    double E =  dy * (axpw - pxaw) - dx * (aypw - pyaw); 


    //-----------------------------------------------------------------------
    //  ERROR BOUNDS
    //-----------------------------------------------------------------------
    //
    // mes(E) = 2 * (mes(dx) * 2*(mes(aypw)+mes(pyaw)) +
    //               mes(dy) * 2*(mes(axpw)+mes(pxaw)))
    //        = 4 * (mes(dx) * (mes(aypw)+mes(pyaw)) +
    //               mes(dy) * (mes(axpw)+mes(pxaw)))
    //        = 8 * (fabs(dx) * (2*fabs(aypw)+2*fabs(pyaw)) +
    //               fabs(dy) * (2*fabs(axpw)+2*fabs(pxaw)))
    //        =16 * (fabs(dx) * (fabs(aypw)+fabs(pyaw)) +
    //               fabs(dy) * (fabs(axpw)+fabs(pxaw)))
    //
    // ind(E) =  (ind(dx) + ind(aypw - pyaw) + 0.5 +
    //            ind(dy) + ind(axpw - pxaw) + 0.5 + 1)/2
    //        =  (ind(dx) + (ind(aypw) + ind(pyaw) + 1)/2 + 0.5 +
    //            ind(dy) + (ind(axpw) + ind(pxaw) + 1)/2 + 0.5 + 1)/2
    //        =  (0.5 + (1.5 + 1.5 + 1)/2 + 0.5
    //            0.5 + (1.5 + 1.5 + 1)/2 + 0.5 + 1)/2
    //        =  (0.5 + 2 + 0.5 + 0.5 + 2 + 0.5 + 1)/2
    //        =  3.5 
    //
    // mes(E) = ind(E) * mes(E) * eps0
    //        = 56 * (fabs(dx) * (fabs(aypw)+fabs(pyaw)) +
    //                fabs(dy) * (fabs(axpw)+fabs(pxaw))) * eps0
    //-----------------------------------------------------------------------
  
    FABS(aypw);
    FABS(pyaw);
    FABS(axpw);
    FABS(pxaw);
    FABS(dx);
    FABS(dy);
  
    double eps = 56 * ((aypw+pyaw)*dx + (axpw+pxaw)*dy) * seg_eps0;

#if defined(__win32__)
   if (fp::is_finite(E) && fp::is_finite(eps))
#endif
   { if (E >  eps) return  1;
     if (E < -eps) return -1;
     if (eps < 1)  return 0;
    }

  }

  // big integers

  return sign( s.dy() * (a.X()*p.W() - p.X()*a.W()) -
               s.dx() * (a.Y()*p.W() - p.Y()*a.W()) );

 }



int cmp_segments_at_xcoord(const rat_segment& s1, const rat_segment& s2, 
                           const rat_point& r)
{ 
  if (rat_segment::use_filter)
    {
      double s1x = s1.XD1();
      double s1y = s1.YD1();
      double s1w = s1.WD1();
      
      double s2x = s2.XD1();
      double s2y = s2.YD1();
      double s2w = s2.WD1();
      
      double rx = r.XD();
      double ry = r.YD();
      double rw = r.WD();
      
      double d1x = s1.dxD();
      double d1y = s1.dyD();
      
      double d2x = s2.dxD();
      double d2y = s2.dyD();
      
      double E = d2x * s2w * (s1y * d1x * rw + d1y * (rx * s1w - s1x * rw))
          - d1x * s1w * (s2y * d2x * rw + d2y * (rx * s2w - s2x * rw));


      /* ----------------------------------------------------------------------
       *                             ERROR BOUNDS
       * ----------------------------------------------------------------------
       *
       * mes(E) = mes(d2x*s2w *(s1y*d1x*rw+d1y*(rx*s1w-s1x*rw))
       *              -d1x*s1w*(s2y*d2x*rw+d2y*(rx*s2w-s2x*rw)))
       *
       *        = 2*(mes(d2x)*mes(s2w)*mes(s1y*d1x*rw+d1y*(rx*s1w-s1x*rw))
       *             + mes(d1x)*mes(s1w)*mes(s2y*d2x*rw+d2y*(rx*s2w-s2x*rw)))
       *
       *        = 4*(mes(d2x)*mes(s2w)*
       *               (mes(s1y)*mes(d1x)*mes(rw)+
       *                  mes(d1y)*2(mes(rx)*mes(s1w)+mes(s1x)*mes(rw)))
       *           +mes(d1x)*mes(s1w)*
       *              (mes(s2y)*mes(d2x)*mes(rw)+
       *                  mes(d2y)*2(mes(rx)*mes(s2w)+mes(s2x)*mes(rw))))
       *
       *        = 4*(fabs(d2x)*fabs(s2w)*
       *               (fabs(s1y)*2*fabs(d1x)*fabs(rw)+
       *                  fabs(d1y)*2*(fabs(rx)*fabs(s1w)+fabs(s1x)*fabs(rw)))
       *            +fabs(d1x)*fabs(s1w)*
       *               (fabs(s2y)*2*fabs(d2x)*fabs(rw)+
       *                  fabs(d2y)*2*(fabs(rx)*fabs(s1w)+fabs(s2x)*fabs(rw))))
       *
       * ----------------------------------------------------------------------
       *
       * ind(E) = ind(d2x*s2w *(s1y*d1x*rw+d1y*(rx*s1w-s1x*rw))
       *              -d1x*s1w*(s2y*d2x*rw+d2y*(rx*s2w-s2x*rw)))
       * 
       *        = (ind(d2x*s2w *(s1y*d1x*rw+d1y*(rx*s1w-s1x*rw)))
       *          +ind(d1x*s1w*(s2y*d2x*rw+d2y*(rx*s2w-s2x*rw))) + 1)/2
       *
       *        = (ind(d2x)+ind(s2w *(s1y*d1x*rw+d1y*(rx*s1w-s1x*rw)))+0.5
       *          +ind(d1x)+ind(s1w*(s2y*d2x*rw+d2y*(rx*s2w-s2x*rw)))+0.5+1)/2
       *
       *        = (1 + ind(s1y*d1x*rw+d1y*(rx*s1w-s1x*rw)) + 1
       *           + 1 ind(s2y*d2x*rw+d2y*(rx*s2w-s2x*rw)) + 1 + 1)/2
       *
       *        = (5 + (ind(s1y*d1x*rw) + ind(d1y*(rx*s1w-s1x*rw)) + 1)/2
       *          + (ind(s2y*d2x*rw) + ind(d2y*(rx*s2w-s2x*rw)) + 1)/2 )/2
       *
       *        = (5 + (ind(s1y)+ind(d1x)+ind(rw) + 1 + 
       *                ind(d1y*(rx*s1w-s1x*rw)) + 1)/2
       *          + (ind(s2y)+ind(d2x)+ind(rw) + 1 +
       *             ind(d2y*(rx*s2w-s2x*rw))+ 1)/2)/2
       *
       *        = (5 + (0.5 + 0.5 + 0.5 + 1 + ind(d1y*(rx*s1w-s1x*rw)) + 1)/2
       *          + (0.5 + 0.5 + 0.5 + 1 + ind(d2y*(rx*s2w-s2x*rw)) + 1)/2 )/2
       *
       *        = (5 + (3.5 + ind(d1y) + ind(rx*s1w-s1x*rw) + 0.5)/2
       *          + (3.5 + ind(d2y) + ind(rx*s2w-s2x*rw) + 0.5)/2 )/2
       *
       *        = (5 + (3.5 + 0.5 +(ind(rx*s1w) + ind(s1x*rw) + 1)/2 + 0.5)/2
       *          + (3.5 + 0.5 +(ind(rx*s2w) + ind(s2x*rw) + 1)/2 + 0.5)/2 )/2 
       *
       *        = (5 + (3.5 + 0.5 + (1.5 + 1.5 + 1 )/2 + 0.5 )/2
       *             + (3.5 + 0.5 + (1.5 + 1.5 + 1 )/2 + 0.5 )/2)/2
       *
       *        = (5 + (4 + 2 + 0.5)/2 + (4 + 2 + 0.5)/2)/2 
       *
       *        = (5 + 13/2)/2 = 23/4
       *
       * ----------------------------------------------------------------------
       *
       * eps(E) = ind(E) * mes(E) * eps0
       *
       * eps    = 23*(fabs(d2x)*fabs(s2w)*
       *               (fabs(s1y)*fabs(d1x)*fabs(rw)+
       *                  fabs(d1y)*2*(fabs(rx)*fabs(s1w)+fabs(s1x)*fabs(rw)))
       *            +fabs(d1x)*fabs(s1w)*
       *               (fabs(s2y)*fabs(d2x)*fabs(rw)+
       *                  fabs(d2y)*2*(fabs(rx)*fabs(s1w)+fabs(s2x)*fabs(rw))))
       *            *eps0
       *
       *
       * ------------------------------------------------------------------- */

      FABS(s1x);
      FABS(s1y);
      FABS(s1w);
      
      FABS(s2x);
      FABS(s2y);
      FABS(s2w);
      
      FABS(rx);
      FABS(ry);
      FABS(rw);
      
      FABS(d1x);
      FABS(d1y);
      
      FABS(d2x);
      FABS(d2y);
      
      double eps = 23 * 
                   (d2x * s2w * (s1y * d1x * rw + 2*d1y * 
                                 (rx * s1w + s1x * rw))
                   + d1x * s1w * (s2y * d2x * rw + 2*d2y * 
                                  (rx * s2w + s2x * rw)))
                   * seg_eps0; 

#if defined(__win32__)
      if (fp::is_finite(E) && fp::is_finite(eps))
#endif
      { if (E > eps)  return  1;
        if (E < -eps) return -1;
        if (eps < 1) return 0;
       }
    }
  
  return sign( s2.dx() * s2.W1() *
                 ( s1.Y1() * s1.dx() * r.W() + 
                   s1.dy() * (r.X() * s1.W1() - s1.X1() * r.W()))
               -
               s1.dx() * s1.W1() * 
                 ( s2.Y1() * s2.dx() * r.W() + 
                   s2.dy() * ( r.X() * s2.W1() - s2.X1() * r.W()))); 
}


rat_segment rat_segment::perpendicular(const rat_point& q) const
{ rat_point p = source();
  if (is_trivial()) return rat_segment(q,p);
  integer  dx = q.X()*p.W() - p.X()*q.W();
  integer  dy = q.Y()*p.W() - p.Y()*q.W();
  integer  dw = p.W()*q.W();
  rat_point r;
  intersection_of_lines(translate(dx,dy,dw).rotate90(), r);
  return rat_segment(q,r);
 }


rational rat_segment::sqr_dist(const rat_point& p) const
{ rat_point a = source();
  rat_point b = target();
  if (a.orientation(b.rotate90(a),p) > 0) return a.sqr_dist(p);
  if (b.orientation(a.rotate90(b),p) > 0) return b.sqr_dist(p);
  return perpendicular(p).sqr_length();
}

LEDA_END_NAMESPACE 
