/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _geo_rep.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/numbers/geo_rep.h>

LEDA_BEGIN_NAMESPACE

integer* geo_rep::allocate(int d)
{ integer* v = (integer*)std_memory.allocate_bytes(d*sizeof(integer));
  integer*p = v+d;
  while (p > v) new(--p) integer(0);
  return v;
}

void geo_rep::deallocate(integer* v, int d)
{ integer* p = v+d;
  while(p > v) (--p)->~integer();
  std_memory.deallocate_bytes(v,d*sizeof(integer));
}

geo_rep::geo_rep()
{ dim = 0;
  v = allocate(dim+1);
  v[dim] = 0;
 }



geo_rep::geo_rep(int d)
{ dim = d;
  v = allocate(dim+1);
}

geo_rep::geo_rep(integer a,integer b,integer D)
{ dim = 2;
  v = allocate(dim+1);
  init3(a,b,D);
}

geo_rep::geo_rep(integer a,integer b,integer c,integer D)
{ dim = 3;
  v = allocate(dim+1);
  init4(a,b,c,D);
}



integer_vector geo_rep::vec()const
{ integer_vector res(dim+1);
  for(int i= 0;i<=dim;i++) res[i]= v[i];
  return res;
 }

geo_rep::geo_rep(const integer_vector&c,integer D)
{ dim= c.dim();
  v = allocate(dim+1);
  v[dim]= D;
  for(int i= 0;i<dim;i++)v[i]= c[i];
}


geo_rep::geo_rep(const integer_vector&c)
{ dim= c.dim()-1;
  v = allocate(dim+1);
  for(int i= 0;i<=dim;i++) v[i]= c[i];
}



geo_rep::~geo_rep() { deallocate(v,dim+1); }




void geo_rep::init3(integer x0,integer x1,integer x2)
{ v[0]= x0;
  v[1]= x1;
  v[2]= x2;
 }

void geo_rep::init4(integer x0,integer x1,integer x2,integer x3)
{ v[0]= x0;
  v[1]= x1;
  v[2]= x2;
  v[3]= x3;
 }



void geo_rep::copy(geo_rep*g1)
{ for(int i= 0;i<=dim;i++) v[i]= g1->v[i]; }

void geo_rep::negate(int d)
{ for(int i= 0;i<d;i++) v[i]= -v[i]; }

int geo_rep::cmp_rat_coords(geo_rep* a, geo_rep* b)
{
  if(a->dim != b->dim)
     LEDA_EXCEPTION(1,"geo_rep::cmp: dimensions disagree.");

  integer aw= a->v[a->dim];
  integer bw= b->v[b->dim];
  int signadim= (aw>0?1:-1);
  int signbdim= (bw>0?1:-1);

  int s = signadim*signbdim;
  
  for(int i= 0;i<a->dim;i++)
  { integer aibw= a->v[i]*bw;
    integer biaw= b->v[i]*aw;
    int c= compare(aibw,biaw);
    if (c != 0) return c*s;
  }
 return 0;
}



int geo_rep::cmp_hom_coords(geo_rep* a, geo_rep* b)
{ if(a->dim!=b->dim)
     LEDA_EXCEPTION(1,"geo_rep::cmp: dimensions disagree.");

  for(int i= 0;i<=a->dim;i++)
  { int c = compare(a->v[i],b->v[i]);
    if (c != 0) return c;
  }
  return 0;
 }


ostream& operator<<(ostream&out,geo_rep*p)
{ out<<"(";
  for(int i= 0;i<p->dim;i++)
  out<<p->v[i]<<",";
  out<<p->v[p->dim]<<")";
  return out;
}

istream&operator>>(istream&in,geo_rep*p)
{ // syntax: $(x_0, x_1, \ldots , x_d)$ 

  int d= p->dim;
  integer x,y,w;
  char c;
  
  do in.get(c);while(in&&isspace(c));

  if (!in) return in;

  if (c != '(' )
  { in.putback(c);
    return in;
   }
  
  long inputnum;
  for(int i= 0;i<d;i++)
  { in>>inputnum;
    p->v[i]= inputnum;
    do in.get(c);while(isspace(c));
    if(c!=',')
    { in.putback(c);
      return in;
    }
  }
 in>>inputnum;
 p->v[d]= inputnum;
 do in.get(c);while(isspace(c));
 if(c!=')')in.putback(c);
  
 return in;
}


void c_add(geo_rep* res, geo_rep* a,geo_rep* b)
{ // We expect |res| to have the same dim as |a| 
  int d = a->dim;
  if(d!=b->dim) LEDA_EXCEPTION(1,"cartesian+: dimensions disagree.");
  integer aw = a->v[d];
  integer bw = b->v[d];
  for(int i= 0; i<d; i++) res->v[i]= a->v[i]*bw+b->v[i]*aw;
  res->v[d]= aw*bw;
}

void c_sub(geo_rep* res, geo_rep* a, geo_rep* b)
{ // We expect |res| to have the same dim as |a| 
  int d = a->dim;
  if(d != b->dim) LEDA_EXCEPTION(1,"cartesian-: dimensions disagree.");
  integer aw= a->v[d];
  integer bw= b->v[d];
  for(int i = 0;i<d;i++) res->v[i] = a->v[i]*bw-b->v[i]*aw;
  res->v[d] = aw*bw;
}

LEDA_END_NAMESPACE
