/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gen_points.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#if (KERNEL == RAT_KERNEL)
#include <LEDA/geo/rat_circle.h>
#endif


LEDA_BEGIN_NAMESPACE 

static random_source Rand_Source;

static double random_double_with_sign()
{ double x;
  Rand_Source >> x;
  return 2*(x - 0.5);
}

#ifdef D2
inline POINT make_point(INT_TYPE x, INT_TYPE y, INT_TYPE, INT_TYPE w)
{ return POINT(x,y,w); }
#endif

#ifdef D3
inline POINT make_point(INT_TYPE x, INT_TYPE y, INT_TYPE z, INT_TYPE w)
{ return POINT(x,y,z,w); }
#endif

inline void add_point(INT_TYPE x, INT_TYPE y, INT_TYPE z, list<POINT>& L)
{ L.append(make_point(x,y,z,1));}

void random_point_in_cube(POINT& p, int maxc)
{ if ( maxc >= (1 << 30) ) 
  { LEDA_EXCEPTION(0,"point generators: decreased maxc to (1 << 30) -1");
    maxc = (1 << 30) - 1;
  }
  INT_TYPE x = rand_int(-maxc,maxc);
  INT_TYPE y = rand_int(-maxc,maxc);
  INT_TYPE z = rand_int(-maxc,maxc);
  p = make_point(x,y,z,1);
}

void  random_points_in_cube(int n, int maxc, list<POINT>& L)
{ L.clear(); 
  POINT p;
  while (n--) 
  { random_point_in_cube(p,maxc);
    L.append(p);
  }
}

void random_point_in_square(POINT& p, int maxc)
{ 
  if ( maxc >= (1 << 30) ) 
  { LEDA_EXCEPTION(0,"point generators: decreased maxc to (1 << 30) -1");
    maxc = (1 << 30) - 1;
  }
  INT_TYPE x = rand_int(-maxc,maxc);
  INT_TYPE y = rand_int(-maxc,maxc);
  p = make_point(x,y,0,1);
}

void  random_points_in_square(int n, int maxc, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_square(p,maxc);
    L.append(p);
  }
}

void random_point_in_unit_cube(POINT& p, int D)
{ INT_TYPE x = rand_int(0,D);
  INT_TYPE y = rand_int(0,D);
  INT_TYPE z = rand_int(0,D);
  p = make_point(x,y,z,D);
}

void random_point_in_unit_square(POINT& p, int D)
{ INT_TYPE x = rand_int(0,D);
  INT_TYPE y = rand_int(0,D);
  p = make_point(x,y,0,D);
}

void  random_points_in_unit_cube(int n, int D, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_unit_cube(p,D);
    L.append(p);
  }
}

void  random_points_in_unit_square(int n, int D, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_unit_square(p,D);
    L.append(p);
  }
}

void  random_points_in_unit_cube(int n, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_unit_cube(p);
    L.append(p);
  }
}

void  random_points_in_unit_square(int n, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_unit_square(p);
    L.append(p);
  }
}


void  random_points_on_diagonal(int n, int maxc, list<POINT>& L)
{ if ( maxc >= (1 << 30) ) 
  { LEDA_EXCEPTION(0,"point generators: decreased maxc to (1 << 30) -1");
    maxc = (1 << 30) - 1;
  }
  L.clear(); POINT p;
  while (n--)
  { INT_TYPE x = rand_int(-maxc,maxc);
    add_point(x,x,x,L);
   }
}

void random_point_on_segment(POINT& p, SEGMENT s)
{ 
  int n = (1 << 30) - 1;
  int i = rand_int(0,n);

  
D2_POINT p1 = s.source();
D2_POINT p2 = s.target();

p = make_point(i*p1.X()*p2.W() + (n-i)*p2.X()*p1.W(),
               i*p1.Y()*p2.W() + (n-i)*p2.Y()*p1.W(),
               0,
               n*p1.W()*p2.W());  

}
                


void random_points_on_segment(SEGMENT s, int n, list<POINT>& L)
{ 
  L.clear(); POINT p;
  while (n--)
  { random_point_on_segment(p,s);
    L.append(p);
  }
}

void points_on_segment(SEGMENT s, int n, list<POINT>& L)
{ 
  L.clear(); POINT p;
  n--;
  for (int i = 0; i <= n - 1; i++)
  { 
D2_POINT p1 = s.source();
D2_POINT p2 = s.target();

p = make_point(i*p1.X()*p2.W() + (n-i)*p2.X()*p1.W(),
               i*p1.Y()*p2.W() + (n-i)*p2.Y()*p1.W(),
               0,
               n*p1.W()*p2.W());  

    L.append(p);
  }
}




void  random_point_in_ball(POINT& p, int R)
{ 
  double x,y,z;
  do 
  { x = random_double_with_sign();
    y = random_double_with_sign();
#ifdef D3
    z = random_double_with_sign();
#endif
#ifdef D2
    z = 0;
#endif
  } while (x*x + y*y + z*z > 1 );
  p = make_point((int)(x*R),(int)(y*R),(int)(z*R),1);
}



void  random_points_in_ball(int n, int R, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_ball(p,R);
    L.append(p);
  }
}



void  random_point_in_unit_ball(POINT& p, int D)
{ 
  double x,y,z;
  do 
  { x = random_double_with_sign();
    y = random_double_with_sign();
#ifdef D3
    z = random_double_with_sign();
#endif
#ifdef D2
    z = 0;
#endif
  } while (x*x + y*y + z*z > 1 );
  p = make_point((int)(x*D),(int)(y*D),(int)(z*D),D);
}

void  random_point_in_unit_disc(POINT& p, int D)
{ random_point_in_unit_ball(p,D);
}

void  random_points_in_unit_ball(int n, int D, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_unit_ball(p,D);
    L.append(p);
  }
}

void  random_points_in_unit_ball(int n, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_unit_ball(p);
    L.append(p);
  }
}

void  random_points_in_unit_disc(int n, int D, list<POINT>& L)
{ random_points_in_unit_ball(n,D,L);
}

void  random_points_in_unit_disc(int n, list<POINT>& L)
{ random_points_in_unit_ball(n,L);

}



void  random_point_in_disc(POINT& p, int R)
{ 
  double x,y;
  do 
  { x = random_double_with_sign();
    y = random_double_with_sign();
  } while (x*x + y*y > 1 );
  p = make_point((int)(x*R),(int)(y*R),0,1);
}



void  random_points_in_disc(int n, int R, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_in_disc(p,R);
    L.append(p);
  }
}

void random_point_on_paraboloid(POINT& p, int maxc)
{ double x = rand_int(-maxc,maxc);
  double y = rand_int(-maxc,maxc);
  double z = 0.004*(x*x + y*y) - 1.25*maxc;
  p = make_point(int(x),int(y),int(z),1);
}

void  random_points_on_paraboloid(int n, int maxc, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_on_paraboloid(p,maxc);
    L.append(p);
  }
}

void  lattice_points(int n, int maxc, list<POINT>& L)
{ 
  L.clear(); POINT p;

#ifdef D3
  int i = 1;
  while (i*i*i < n) i++;
  i--;

  if (i < 2) i = 2;

  int  d = 2*maxc/(i-1);  

  for(int x = -maxc; x <= maxc; x += d)
     for(int y = -maxc; y <= maxc; y += d)
        for(int z = -maxc; z <= maxc; z += d)
            add_point(x,y,z,L);
#endif

#ifdef D2
  int i = 1;
  while (i*i < n) i++;
  i--;

  if (i < 2) i = 2;

  int  d = 2*maxc/(i-1);  

  for(int x = -maxc; x <= maxc; x += d)
     for(int y = -maxc; y <= maxc; y += d)
          add_point(x,y,0,L);
#endif
}




void random_point_near_circle(POINT& p, int R)
{ int N = 1 << 30;;
  int i = rand_int(0,N-1);
  double phi = 2.0*i*LEDA_PI/N;

  int x = int(R*cos(phi));
  int y = int(R*sin(phi));
  p = make_point(x,y,0,1);
}

void  random_points_near_circle(int n, int R, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_near_circle(p,R);
    L.append(p);
  }
}


void random_point_near_unit_circle(POINT& p, int D)
{ double a;
  Rand_Source >> a;
  double phi = 2*a*LEDA_PI;

  int x = int(D*cos(phi));
  int y = int(D*sin(phi));
  p = make_point(x,y,0,D);
}

void  random_points_near_unit_circle(int n, int D, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_near_unit_circle(p,D);
    L.append(p);
  }
}

void  random_points_near_unit_circle(int n, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_near_unit_circle(p);
    L.append(p);
  }
}





void random_point_near_sphere(POINT& p, int R)
{
  

double x,y,z; int k = 10; int N = ( 1 << k ) - 1; double r;
  int i;
#ifdef D3
    double a;
    Rand_Source >> a;
    int l = 0; int u = N; double alpha;
    while ( u - l > 1 )
    { // u = l + 2^k - 1
      i = (u + l)/2;  // i = l + 2^{k - 1} - 1
      alpha = i * LEDA_PI / ( 2 * N);
      if ( cos( alpha ) >= a ) 
        l = i + 1;
      else
        u = i;
    } 
    z = sin(alpha);
    i = rand_int(0,N);
    if ( i > N/2 ) z = -z;
    r = cos(alpha);
#endif
#ifdef D2
    z = 0; r = 1;
#endif
  N = 1 << 30;
  i = rand_int(0,N-1);
  double phi = 2*i*LEDA_PI/N;

  x = r * cos(phi);
  y = r * sin(phi);



  p = make_point((int)(x*R),(int)(y*R),(int)(z*R),1);
}

void random_point_near_unit_sphere(POINT& p, int D)
{
  
double x,y,z; int k = 10; int N = ( 1 << k ) - 1; double r;
  int i;
#ifdef D3
    double a;
    Rand_Source >> a;
    int l = 0; int u = N; double alpha;
    while ( u - l > 1 )
    { // u = l + 2^k - 1
      i = (u + l)/2;  // i = l + 2^{k - 1} - 1
      alpha = i * LEDA_PI / ( 2 * N);
      if ( cos( alpha ) >= a ) 
        l = i + 1;
      else
        u = i;
    } 
    z = sin(alpha);
    i = rand_int(0,N);
    if ( i > N/2 ) z = -z;
    r = cos(alpha);
#endif
#ifdef D2
    z = 0; r = 1;
#endif
  N = 1 << 30;
  i = rand_int(0,N-1);
  double phi = 2*i*LEDA_PI/N;

  x = r * cos(phi);
  y = r * sin(phi);



  p = make_point((int)(x*D),(int)(y*D),(int)(z*D),D);
}


void random_points_near_sphere(int n, int R, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_near_sphere(p,R);
    L.append(p);
  }
}

void random_points_near_unit_sphere(int n, int D, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_near_unit_sphere(p,D);
    L.append(p);
  }
}

void random_points_near_unit_sphere(int n, list<POINT>& L)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_near_unit_sphere(p);
    L.append(p);
  }
}



void random_point_on_circle(POINT& p, int R, int C)
{ 
#if (KERNEL == RAT_KERNEL)
rat_point origin(0,0);
rat_circle Circ(origin,origin + R*rat_vector::unit(1));
double a; Rand_Source >> a;
double eps = 1.0/(2*C);
rat_point p2 = Circ.point_on_circle(2*LEDA_PI*a,eps);
#ifdef D3
p = d3_rat_point(p2.X(),p2.Y(),0,p2.W());
#else
p = p2;
#endif
#else
  random_point_near_circle(p,R);
#endif
 
}

void  random_points_on_circle(int n, int R, list<POINT>& L, int C)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_on_circle(p,R,C);
    L.append(p);
  }
}


void random_point_on_unit_circle(POINT& p, int C)
{ 
#if (KERNEL == RAT_KERNEL)
rat_point origin(0,0);
rat_circle Circ(origin,origin + rat_vector::unit(1));
double a; Rand_Source >> a;
double eps = 1.0/(2*C);
rat_point p2 = Circ.point_on_circle(2*LEDA_PI*a,eps);
#ifdef D3
p = d3_rat_point(p2.X(),p2.Y(),0,p2.W());
#else
p = p2;
#endif
#else
  random_point_near_unit_circle(p,C);
#endif
}

void  random_points_on_unit_circle(int n, list<POINT>& L, int C)
{ L.clear(); POINT p;
  while (n--) 
  { random_point_on_unit_circle(p,C);
    L.append(p);
  }
}


void random_point_on_sphere(POINT& p, int R)
{
  random_point_near_sphere(p,R);
}

void random_point_on_unit_sphere(POINT& p, int D)
{
  random_point_near_unit_sphere(p,D);
}


void random_points_on_sphere(int n, int R, list<POINT>& L)
{ 
  random_points_near_sphere(n, R, L);
}

void random_points_on_unit_sphere(int n, int D, list<POINT>& L)
{ 
  random_points_near_unit_sphere(n, D, L);
}

void random_points_on_unit_sphere(int n, list<POINT>& L)
{ 
  random_points_near_unit_sphere(n, L);
}

LEDA_END_NAMESPACE 
