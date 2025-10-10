/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  wrap_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>

using namespace leda;


using std::cout;
using std::cin;
using std::endl;
using std::flush;



template <class POINT, class SEGMENT>
list<POINT> ch_wrapping0 (const list<POINT>& points, const SEGMENT& )
{
    if (points.size() < 3) return points;

    list<POINT> ch;
    list<POINT> rest = points;

    list_item start_it = rest.first();
    POINT start_point = rest[start_it];

    // tiefsten und rechtesten Punkt suchen
    list_item it;
    forall_items ( it, rest )
    {
        POINT p = points[it];
        if (p.ycoord() < start_point.ycoord())
                {
            start_point = p;
            start_it = it;
                }
        else
            if (p.ycoord() == start_point.ycoord())
            {
                if (p.xcoord() > start_point.xcoord())
                            {
                    start_point = p;
                    start_it = it;
                            }
            }
    }
    ch.append ( start_point );
    rest.del ( start_it );

    POINT min_point = start_point;

        while ( !rest.empty() )
    {
        POINT old_point = min_point;

        list_item min_it = rest.first();
        min_point        = rest[min_it];
        SEGMENT s ( old_point, start_point );

        bool min_deleted = false;

        list_item it;
        forall_items ( it, rest )
        {
                        POINT p = rest[it];

            if ( orientation ( s, p ) > 0 )
            {  //left-turn
                if ( it == min_it )
                    min_deleted = true;
                rest.del ( it );
                continue;
            }

            int ori = orientation(old_point, min_point, p);

                    if ( ori < 0 )
            { //right-turn
                min_point = p;
                min_it = it;
                min_deleted = false;
            }
            else
            {
                if ( ori == 0 )
                            { // collinear
                                    if ( cmp_distances ( old_point, p, old_point, min_point ) )
                    {
                                            min_point = p;
                        min_it = it;
                            min_deleted = false;
                    }
                        }
            }
        }
        if ( !min_deleted )
        {
            rest.del ( min_it );
            ch.append ( min_point );
        }
    //    cout << ch << endl;
        }
        return ch;
}




template <class POINT>
list<POINT> ch_wrapping (const list<POINT>& points)
{
  if (points.empty()) return points;

  list<POINT> ch;
  list<POINT> rest = points;

  POINT start_point = rest.head();

  // tiefsten und rechtesten Punkt suchen

  POINT p;
  forall(p,rest)
  { if (p.ycoord() < start_point.ycoord() ||
       (p.ycoord() == start_point.ycoord() && 
        p.xcoord() > start_point.xcoord())) start_point = p;
   }


  POINT old_point = start_point;

  do {

    list_item min_it = 0;
    POINT min_point = start_point;

    list_item it;
    forall_items ( it, rest )
    { POINT p = rest[it];

/*
      if (orientation(old_point,start_point,p) > 0)
      { rest.del(it);
        continue;
       }
*/

      int ori = orientation(old_point, min_point, p);

      if (ori < 0  || 
         (ori == 0 && cmp_distances(old_point,p,old_point,min_point) > 0))
      { min_point = p;
        min_it = it;
        continue;
       }
      
      if (orientation(old_point,start_point,p) > 0) rest.del(it);
    }

    if (min_it) rest.del(min_it);

    ch.append(min_point);
    old_point = min_point;

  } while (old_point != start_point);

  return ch;
}



int main()
{
  int N = read_int("N = ");

  list<rat_point>      L;

  random_points_in_disc (N,10000,L);


 { cout << "CONVEX_HULL_IC(rat_point)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC(L);
   cout << string("|C| = %d   time = %.5f",C.length(),used_time(T)) << endl;
  }

 { cout << "CONVEX_HULL_IC1(rat_point) " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC1(L);
   cout << string("|C| = %d   time = %.5f",C.length(),used_time(T)) << endl;
  }

 { cout << "CONVEX_HULL_IC2(rat_point) " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC2(L);
   cout << string("|C| = %d   time = %.5f",C.length(),used_time(T)) << endl;
  }

 { cout << "CONVEX_HULL_IC3(rat_point) " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC3(L);
   cout << string("|C| = %d   time = %.5f",C.length(),used_time(T)) << endl;
  }


 { cout << "CONVEX_HULL_S(rat_point)   " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_S(L);
   cout << string("|C| = %d   time = %.5f",C.length(),used_time(T)) << endl;
  }

/*
 { cout << "ch_wrapping0(rat_point)    " << flush;
   float T = used_time();
   rat_segment s;
   list<rat_point> C = ch_wrapping0(L,s);
   cout << string("|C| = %d   time = %.5f",C.length(),used_time(T)) << endl;
  }


  { cout << "ch_wrapping(rat_point)     " << flush;
   float T = used_time();
   list<rat_point> C = ch_wrapping (L);
   cout << string("|C| = %d   time = %.5f",C.length(),used_time(T)) << endl;
  }
*/


  return 0;
}
