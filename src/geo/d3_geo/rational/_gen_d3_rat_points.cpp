/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _gen_d3_rat_points.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_rat_point.h>

LEDA_BEGIN_NAMESPACE

d3_rat_point random_d3_rat_point_in_cube(int maxc)
{ integer x = rand_int(-maxc,maxc);
  integer y = rand_int(-maxc,maxc);
  integer z = rand_int(-maxc,maxc);
  return d3_rat_point(x,y,z,1);
}

void  random_d3_rat_points_in_cube(int n, int maxc, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_in_cube(maxc));
}

d3_rat_point random_d3_rat_point_in_square(int maxc)
{ integer x = rand_int(-maxc,maxc);
  integer y = rand_int(-maxc,maxc);
  return d3_rat_point(x,y,0,1);
}

void  random_d3_rat_points_in_square(int n, int maxc, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_in_square(maxc));
}

d3_rat_point random_d3_rat_point_in_unit_cube(int D)
{ integer x = rand_int(0,D);
  integer y = rand_int(0,D);
  integer z = rand_int(0,D);
  return d3_rat_point(x,y,z,D);
}

void  random_d3_rat_points_in_unit_cube(int n, int D, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_in_unit_cube(D));
}

void  random_d3_rat_points_in_unit_cube(int n, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_in_unit_cube());
}


void  random_d3_rat_points_on_segment(int n, int maxc, list<d3_rat_point>& L)
{ L.clear();
  while (n--)
  { integer x = rand_int(-maxc,maxc);
    L.append(d3_rat_point(x,x,x,1));
   }
}


d3_rat_point  random_d3_rat_point_in_ball(int R)
{ if ( R > 1024 * 16) 
  { LEDA_EXCEPTION(0,"random point in ball, maximal value for R is 2^{14}");
    R = 16*1024;
  }
  int x,y,z;
  do 
  { x = rand_int(-R,R);
    y = rand_int(-R,R);
    z = rand_int(-R,R);
#ifdef D2
    z = 0;
#endif
  } while (x*x + y*y + z*z > R*R );
  return d3_rat_point(x,y,z,1);
}

void  random_d3_rat_points_in_ball(int n, int R, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_in_ball(R));
}


d3_rat_point  random_d3_rat_point_in_unit_ball(int D)
{ if ( D > 1024 * 16) 
  { LEDA_EXCEPTION(0,"random point in ball, maximal value for R is 2^{14}");
    D = 16*1024;
  }
  int x,y,z;
  do
  { x = rand_int(-D,D);
    y = rand_int(-D,D);
    z = rand_int(-D,D);
#ifdef D2
    z = 0;
#endif
  } while (x*x + y*y + z*z > D*D );
  return d3_rat_point(x,y,z,D);
}

void  random_d3_rat_points_in_unit_ball(int n, int D, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_in_unit_ball(D));
}

void  random_d3_rat_points_in_unit_ball(int n, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_in_unit_ball());
}

d3_rat_point  random_d3_rat_point_in_disc(int R)
{ if ( R > 1024 * 16) 
  { LEDA_EXCEPTION(0,"random point in ball, maximal value for R is 2^{14}");
    R = 32*1024;
  }
  int x,y;
  do 
  { x = rand_int(-R,R);
    y = rand_int(-R,R);
  } while (x*x + y*y > R*R );
  return d3_rat_point(x,y,0,1);
}

void  random_d3_rat_points_in_disc(int n, int R, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_in_disc(R));
}


d3_rat_point random_d3_rat_point_on_paraboloid(int maxc)
{ double x = rand_int(-maxc,maxc);
  double y = rand_int(-maxc,maxc);
  double z = 0.004*(x*x + y*y) - 1.25*maxc;
  return d3_rat_point(int(x),int(y),int(z),1);
}

void  random_d3_rat_points_on_paraboloid(int n, int maxc, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_on_paraboloid(maxc));
}


d3_rat_point random_d3_rat_point_on_circle(int R)
{ int N = 32*1024;
  int i = rand_int(0,N-1);
  double phi = 2*i*LEDA_PI/N;

  int x = int(R*cos(phi));
  int y = int(R*sin(phi));
  return d3_rat_point(x,y,0,1);
}

void  random_d3_rat_points_on_circle(int n, int R, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_on_circle(R));
}


d3_rat_point random_d3_rat_point_on_unit_circle(int D)
{ int N = 32*1024;
  int i = rand_int(0,N-1);
  double phi = 2*i*LEDA_PI/N;

  int x = int(D*cos(phi));
  int y = int(D*sin(phi));
  return d3_rat_point(x,y,0,D);
}

void  random_d3_rat_points_on_unit_circle(int n, int D, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_on_unit_circle(D));
}

void  random_d3_rat_points_on_unit_circle(int n, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_on_unit_circle());
}




d3_rat_point random_d3_rat_point_on_sphere(int R)
{
  int D = 16*1024;
  int DD = D*D;
  int x,y,z;
  int hypot;
  do
  { x = rand_int(-D,D);
    y = rand_int(-D,D);
    z = rand_int(-D,D);
#ifdef D2
    z = 0;
#endif
    hypot = x*x + y*y + z*z;
  } while (hypot > DD || hypot < DD/64 );
  
  int w = (int) ::sqrt((double)hypot);
  return d3_rat_point(x*R,y*R,z*R,w);
}


d3_rat_point random_d3_rat_point_on_unit_sphere(int D)
{
  int DD = D*D;
  int x,y,z;
  int hypot;
  do
  { x = rand_int(-D,D);
    y = rand_int(-D,D);
    z = rand_int(-D,D);
#ifdef D2
    z = 0;
#endif
    hypot = x*x + y*y + z*z;
  } while (hypot > DD || hypot < DD/64 );
  
  int w = (int) ::sqrt((double)hypot);
  return d3_rat_point(x,y,x,w);
}



void random_d3_rat_points_on_sphere(int n, int R, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_on_sphere(R));
}

void random_d3_rat_points_on_unit_sphere(int n, int D, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_on_unit_sphere(D));
}

void random_d3_rat_points_on_unit_sphere(int n, list<d3_rat_point>& L)
{ L.clear();
  while (n--) L.append(random_d3_rat_point_on_unit_sphere());
}
 



void  lattice_d3_rat_points(int n, int maxc, list<d3_rat_point>& L)
{ 
  L.clear();

  int i = 1;
  while (i*i*i < n) i++;
  i--;

  if (i < 2) i = 2;

  int  d = 2*maxc/(i-1);

  for(int x = -maxc; x <= maxc; x += d)
     for(int y = -maxc; y <= maxc; y += d)
        for(int z = -maxc; z <= maxc; z += d)
            L.append(d3_rat_point(x,y,z,1));
}

LEDA_END_NAMESPACE






