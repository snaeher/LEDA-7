/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _motion.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/motion.h>

LEDA_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
// simple linear motion
//---------------------------------------------------------------------------

void linear_motion::get_path(point p0, point p1, int steps, list<point>& L)
{ for (int i = 1; i <= steps; i++)
    L.append(p0.translate(i * vector(p1 - p0) / steps));
}

double linear_motion::get_path_length(point p0, point p1)
{ return p1.distance(p0); }

vector linear_motion::get_step(vector v, int steps, int i) 
{ return v  / steps; }


//---------------------------------------------------------------------------
// circular motion
//---------------------------------------------------------------------------

static const double eps = 0.001;

point circular_motion::center(point p0, point p1)
{
  if (angle == LEDA_PI || angle == -LEDA_PI) 
    return (p0.translate(vector(p1 - p0) / 2));  
  
  double beta = (LEDA_PI - angle) / 2;    
  line l1(p0,vector(p1 - p0).rotate(-beta));
  line l2(p1,vector(p0 - p1).rotate(beta));     
  
  point p;
  l1.intersection(l2,p);
   
  return p;
}

void circular_motion::get_path(point p0, point p1, int steps, list<point>& L)
{ 
  if (fabs(angle) < eps || fabs(angle) > 2*LEDA_PI-eps) 
  { linear_motion P;
    P.get_path(p0,p1,steps,L);
    return;
  }
  
  point  pc = center(p0,p1);
  double da = -angle / steps;    
  vector v  = p0 - pc;

  for (int i = 1; i < steps; i++)
  { v = v.rotate(da);
    L.append(pc.translate(v));
  }

  L.append(p1);  
}


double circular_motion::get_path_length(point p0, point p1)
{
  if (p0 == p1) return 0;
  
  if (fabs(angle) < eps || fabs(angle) > 2*LEDA_PI-eps) 
    return p0.distance(p1);

  return fabs(center(p0,p1).distance(p0) * angle);          
}


vector circular_motion::get_step(vector v, int steps, int i) 
{ 
  if (fabs(angle) < eps || fabs(angle) > 2*LEDA_PI-eps) 
  { linear_motion P;
    return P.get_step(v,steps,i);
  }
      
  point p0 = point(0,0);  
  point pc = center(p0,p0.translate(v));
  
  double da;
 
  da = -angle / steps * (i - 1);              
  point pi = pc.translate(vector(p0 - pc).rotate(da));
  
  da = -angle / steps * i;                
  point pj = pc.translate(vector(p0 - pc).rotate(da));  
  
  return  pj - pi;  
}


//---------------------------------------------------------------------------
// poly motion
//---------------------------------------------------------------------------

void poly_motion::translate(point p0, point p1, list<point>& L)
{  
  vector v    = p1 - p0;        
  vector v_x  = v.norm();
  vector v_y  = v.rotate90().norm();      
  double dist = p0.distance(p1);

  poly_point P;
  forall(P,Path)
  { if (P.xcoord_type() == ABSOLUTE && P.ycoord_type() == ABSOLUTE)
    { double x = p0.xcoord(), y = p0.ycoord(); 
      x += P.xcoord() * v_x.xcoord() + P.ycoord() * v_y.xcoord();
      y += P.xcoord() * v_x.ycoord() + P.ycoord() * v_y.ycoord();
      L.append(point(x,y));        
      continue;
    }

    if (P.xcoord_type() == RELATIVE && P.ycoord_type() == RELATIVE)
    { L.append(p0 + dist * (P.xcoord() * v_x + P.ycoord() * v_y));
      continue;
    }      
  
    if (P.xcoord_type() == RELATIVE && P.ycoord_type() == ABSOLUTE)
    { point p = p0 + dist * (P.xcoord() * v_x);      
      L.append(p.translate(vector(v_y * P.ycoord())));
      continue;
    }

    if (P.xcoord_type() == ABSOLUTE && P.ycoord_type() == RELATIVE)
    { point p = p0 + dist * (P.ycoord() * v_y);      
      L.append(p.translate(vector(v_x * P.xcoord())));
      continue;
    }
  }

  L.append(p1);
}

void poly_motion::append(base_coord x, base_coord y)
{ Path.append(poly_point(x,y)); }

void poly_motion::clear()       { Path.clear(); }  
bool poly_motion::empty() const { return Path.empty(); }
int  poly_motion::size()  const { return Path.size(); }

vector poly_motion::get_step(vector v, int steps, int i) 
{ 
  if (Path.empty()) 
  { linear_motion P;
    P.get_step(v,steps,i);    
  }

  list<point> L;    
  point p0 = point(0,0);    
  translate(p0,p0.translate(v),L);
  
  double length = 0;
  point p1, q = p0;
  forall(p1,L) { length += q.distance(p1); q = p1; }
  
  int points = 0;
  forall(p1,L)
  { double dist = p1.distance(p0);   
    double num  = ceil(dist * (steps - L.size()) / length);    
    if (p1 == L[L.last()]) num = steps - points;    
    double delta = dist / num;
    
    if (points + int(num) >= i)    
    { vector vec = vector(p1 - p0).norm(); 
      p1 = p0.translate(vec * delta * i);
      p0 = p0.translate(vec * delta * (i - 1));
      break;      
    }
    
    points += int(num);        
    p0 = p1;
  }

  return vector(p1 - p0);
}  

void poly_motion::get_path(point p0, point p1, int steps, list<point>& L) 
{ 
  if (Path.empty()) 
  { linear_motion P;
    P.get_path(p0,p1,steps,L);    
  }
      
  list<point> TL;
  translate(p0,p1,TL);
  if (steps == TL.size()) 
  { L = TL; 
    return; 
  }
  
  double length = 0;
  point p, q = p0;
  forall(p,TL) { length += q.distance(p); q = p; }
  
  forall(p1,TL)
  { double dist = p1.distance(p0);   
    double num  = ceil(dist * (steps - TL.size()) / length);    
    if (p1 == TL[TL.last()]) num = steps - L.size();    
    double delta = dist / num;
        
    vector v = vector(p1 - p0).norm();
          
    for (int j = 1; j <= num; j++)
      L.append(p0.translate(v * delta * j));
        
    p0 = p1;
  }
}

double poly_motion::get_path_length(point p0, point p1)
{ if (p0 == p1) return 0;
  if (Path.empty()) return p0.distance(p1);
    
  list<point> L;
  translate(p0,p1,L);
  
  double length = 0;
  forall(p1,L) { length += p1.distance(p0); p0 = p1; }

  return length;
}

//---------------------------------------------------------------------------
// bezier motion
//---------------------------------------------------------------------------

point bezier_motion::casteljau(double t, int n, double* X, double* Y)
{ 
  double* x = new double[n];
  double* y = new double[n];

  for (int j = 0; j < n; j++)
  { x[j] = X[j];
    y[j] = Y[j];
  }

  for (int k = 1; k < n; k++)
  { for (int i = n - 1; i >= k; i--)
    { x[i] = (1-t) * x[i-1] + t * x[i];
      y[i] = (1-t) * y[i-1] + t * y[i];
    }
  }

  point p(x[n-1],y[n-1]);

  delete[] x;
  delete[] y;

  return p;
}

vector bezier_motion::get_step(vector v, int steps, int i) 
{ 
  list<point> L;
  point p0 = point(0,0);
  point p1 = p0.translate(v);
  get_path(p0,p1,steps,L);

  forall(p1,L)
  { if (i-- == 0) break;
    p0 = p1;
  }
  
  return vector(p1 - p0); 
}  

void bezier_motion::get_path(point p0, point p1, int steps, list<point>& L) 
{ 
  if (Path.empty() || steps < 2) 
  { linear_motion P;
    P.get_path(p0,p1,steps,L);    
  }
      
  list<point> TL;
  translate(p0,p1,TL);
  TL.push(p0);
  
  int n = TL.length();

  double* x = new double[n];
  double* y = new double[n];

  int i = 0;
  point p;
  forall(p,TL)
  { x[i] = p.xcoord();
    y[i] = p.ycoord();
    i++;
  }
  
  double d = 1.0 / (steps - 1);
  
  L.append(point(x[0],y[0]));
  
  for (int j = 1; j < steps - 1; j++) 
	 L.append(casteljau(j*d,n,x,y));

  L.append(point(x[n-1],y[n-1]));

  delete [] x;
  delete [] y;
}

double bezier_motion::get_path_length(point p0, point p1)
{  
  list<point> L;
  get_path(p0,p1,200,L);
  
  double length = 0;
  forall(p1,L) { length += p1.distance(p0); p0 = p1; }

  return length;
}

LEDA_END_NAMESPACE

