/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  closest_pair.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// C. Uhrig & S. N"aher (1995)
//------------------------------------------------------------------------------

#include<LEDA/core/array.h>
#include<LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

/*
static int cmp_y(const POINT& a, const POINT& b)
{ return compare(a.ycoord(),b.ycoord()); }
*/



inline COORD square(COORD x) { return x*x; }


static void x_search(POINT* first, POINT* last, COORD& dist, POINT&  r1,
                                                             POINT&  r2)
{ for (POINT* p = first; p < last; p++)
  { POINT* q = p+1;
    while (q <= last && square(p->xdist(*q)) < dist)
    { COORD d = p->sqr_dist(*q);
      if (d < dist)
      { dist = d;
        r1 = *p;
        r2 = *q;
       }
      q++;
     }
   }
}


static void y_search(POINT* first, POINT* last, COORD& dist, POINT&  r1,
                                                             POINT&  r2)
{ for (POINT* p = first; p < last; p++)
  { POINT* q = p+1;
    while (q <= last && square(p->ydist(*q)) < dist)
    { COORD d = p->sqr_dist(*q);
      if (d < dist)
      { dist = d;
        r1 = *p;
        r2 = *q;
       }
      q++;
     }
   }
}



class point_smaller_y {
public:
bool operator()(const POINT& a, const POINT& b) const
 { return POINT::cmp_y(a,b) < 0; }
};


class point_smaller_yF {
public:
 bool operator()(const POINT& a, const POINT& b) const
 { return a.YD()*b.WD() < b.YD()*a.WD();
  }
};



static void cl_pair(POINT* start, POINT* stop, POINT& r1,
                                               POINT& r2, 
                                               COORD& dist, 
                                               array<POINT>& Ay)
{ if (dist == 0) return;

  int n = int(stop-start)+1;

  //if (n < 30) 
  if (n < 8) 
     x_search(start,stop,dist,r1,r2);
  else
     { POINT* mid = start + n/2 - 1;

       cl_pair(start, mid, r1, r2, dist, Ay); 
       cl_pair(mid+1, stop, r1, r2, dist, Ay);
   
       POINT* ystart = &Ay[0];
       POINT* pp = ystart;
       POINT* p;

       for(p = mid; p >= start; p--)
       { if (square(mid->xdist(*p)) >= dist) break;
          *pp++ = *p;
        }
   
       for(p = mid+1; p <= stop; p++)
       { if (square(mid->xdist(*p)) >= dist) break;
          *pp++ = *p;
        }
     
       int m = int(pp - ystart);

       if (m > 1) 
       { //Ay.sort(POINT::cmp_y,0,m-1);
         quicksort(&Ay[0],&Ay[m-1],point_smaller_y(),point_smaller_yF());
         y_search(ystart,pp-1,dist,r1,r2);
        }
     }
}



class point_smaller {
public:
bool operator()(const POINT& a, const POINT& b) const
 { return POINT::cmp_xy(a,b) < 0; }
};


class point_smaller_F {
public:
 bool operator()(const POINT& a, const POINT& b) const
 { double d = a.XD()*b.WD() - b.XD()*a.WD();
   if (d == 0) d = a.YD()*b.WD() - b.YD()*a.WD();
   return d < 0;
 }
};


COORD CLOSEST_PAIR(list<POINT>& L, POINT& r1, POINT& r2)
{
  int n = L.length();

  if (n < 2)
    LEDA_EXCEPTION(1, "CLOSEST PAIR: input has less than two POINTs.");

  array<POINT> Ax(n);  // array of input POINTs sorted by x-coordinates
  array<POINT> Ay(n);  // auxiliary array

  // copy input POINTs to Ax and sort

  POINT* pp = &Ax[0];
  POINT p;
  forall(p,L) *pp++ = p;

//Ax.sort();
  quicksort(&Ax[0],&Ax[n-1],point_smaller(),point_smaller_F());

  COORD dist = L.head().sqr_dist(L.tail())+1;

  cl_pair(&Ax[0],&Ax[n-1],r1,r2,dist,Ay);

  return dist;

}

LEDA_END_NAMESPACE


