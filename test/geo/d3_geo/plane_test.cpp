/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  plane_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/d3_window.h>
#include <LEDA/geo/d3_rat_plane.h>
#include <assert.h>

using namespace leda;

using namespace std;


int ran() { return rand_int(-100,100); }

int main()
{
  // determine a new seed and report it!
  int seed = rand_int.reinit_seed();
  cout << "seed = " << seed << endl << endl; cout.flush();

  int N = read_int("N = ");

  d3_rat_plane P;

  d3_plane FP = P.to_float();

  int i;
  for(i=0; i<N; i++)
  { int w = ran();
    d3_rat_point p(ran(),ran(),ran(),(w == 0) ? 1 : w);
    w = ran();
    d3_rat_point q(ran(),ran(),ran(),(w == 0) ? 1 : w);
    d3_rat_point i;

    d3_point fp = p.to_float();
    d3_point fq = q.to_float();
    d3_point fi;

    int res = P.intersection(p,q,i);
    cout << res << ": " << i.to_float() << endl;

    if (res == 0) 
    { assert(P.sqr_dist(p) == P.sqr_dist(q));
      assert(P.side_of(p) == P.side_of(q));
     }

    if (res == 1)
    { assert (collinear(p,q,i));
      assert (P.contains(i));
     }

    if (res == 2) 
    { assert(P.contains(p));
      assert(P.contains(q));
     }

    res = FP.intersection(fp,fq,fi);
    cout << res << ": " << fi << endl;
    cout << endl;
   }
  cout << endl;


  for(i=0; i<N; i++)
  { d3_rat_point p(ran(),ran(),ran(),ran());
    d3_rat_point q(ran(),ran(),ran(),ran());
    d3_rat_point r(ran(),ran(),ran(),ran());

    if (collinear(p,q,r)) continue;

    d3_rat_plane Q(p,q,r);
    d3_rat_point i1,i2;

    int res = P.intersection(Q,i1,i2);
    cout << res << ": " << i1.to_float() << " " << i2.to_float() << endl;

    if (res == 1)
    { assert(P.contains(i1));
      assert(P.contains(i2));
      assert(Q.contains(i1));
      assert(Q.contains(i2));
     }

    d3_point fp = p.to_float();
    d3_point fq = q.to_float();
    d3_point fr = r.to_float();
    if (collinear(fp,fq,fr)) continue; // might be true, even if the rat-version returns false

    d3_plane FQ(fp,fq,fr);
    d3_point fi1,fi2;

    res = FP.intersection(FQ,fi1,fi2);
    cout << res << ": " << fi1 << " " << fi2 << endl;
    cout << endl;
   }

   return 0;
}

