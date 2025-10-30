/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _rat_ray.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/




#include <LEDA/geo/rat_ray.h>

//------------------------------------------------------------------------------
// rat_rays 
//
// by S. Naeher (1995,1996)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

rat_ray::rat_ray()
{ PTR = new rat_ray_rep; }

rat_ray::rat_ray(const rat_segment& s) 
{ PTR = new rat_ray_rep(s); 
  if (s.is_trivial())
     LEDA_EXCEPTION(1,"rat_ray::rat_ray: cannot construct ray from trivial segment");
}

rat_ray::rat_ray(const rat_point& p, const rat_vector& v)
{ PTR = new rat_ray_rep(rat_segment(p, p+v)); }

rat_ray::rat_ray(const rat_point& x, const rat_point& y)    
{ PTR = new rat_ray_rep(rat_segment(x,y)); }


ray rat_ray::to_ray() const
{ return ray(ptr()->seg.to_segment()); }


bool rat_ray::contains(const rat_point& p) const
{ if (orientation(ptr()->seg,p) == 0) 
  { // p lies on line
    rat_segment s = ptr()->seg.rotate90();
    return (orientation(s,p) <= 0);
   }
  return false;
}


bool rat_ray::contains(const rat_segment& s) const
{ return contains(s.source()) && contains(s.target()); }


bool rat_ray::intersection(const rat_ray& r, rat_point& inter) const
{ if ( cmp_slopes(*this,r) == 0 )
     { if ( contains(r.source()) )
          { inter = r.source(); return true; }
       if ( r.contains(source()) )
          { inter = source(); return true; }
       return false;
     }

  if (!ptr()->seg.intersection_of_lines(r.ptr()->seg,inter)) return false;

  return orientation(ptr()->seg.rotate90(),inter)   <= 0 &&
         orientation(r.ptr()->seg.rotate90(),inter) <= 0;
}


bool rat_ray::intersection(const rat_segment& s, rat_point& inter) const
{ 
  if ( s.is_trivial() )
  { if ( ! contains(s.source()) ) return false; 
    inter = s.source(); 
    return true;
  }

  if ( cmp_slopes(*this,rat_ray(s)) == 0 )
     { if ( contains(s.source()) )
          { inter = s.source(); return true; }
       if ( contains(s.target()) )
          { inter = s.target(); return true; }
       return false;
     }

   if (!ptr()->seg.intersection_of_lines(s,inter)) return false;
  
   return orientation(ptr()->seg.rotate90(),inter) <= 0 &&
          orientation(s.rotate90(),inter)          <= 0 &&
          orientation(s.reverse().rotate90(),inter)<= 0;
}


bool rat_ray::operator==(const rat_ray& g) const 
{ rat_segment s1 = ptr()->seg;
  rat_segment s2 = g.ptr()->seg;
  return (s1.source() == s2.source()) && contains(s2); 
}


ostream& operator<<(ostream& out, const rat_ray& r) 
{ return out << r.seg(); }

istream& operator>>(istream& in, rat_ray& r)  
{ rat_segment s; 
  in >> s; 
  r = rat_ray(s); 
  return in; 
 }

LEDA_END_NAMESPACE 

