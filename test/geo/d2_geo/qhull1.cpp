#include <LEDA/geo/plane_alg.h>
#include <LEDA/geo/rat_point.h>
#include <LEDA/core/array.h>
#include <LEDA/numbers/fp.h>

using namespace leda;

using std::cout;
using std::cin;
using std::endl;
using std::flush;

class rp_smaller {
public:
bool operator()(const rat_point& p, const rat_point& q) const
{ return rat_point::cmp_xy(p,q) < 0; }
};

class rp_smaller_F {
public:
 bool operator()(const rat_point& a, const rat_point& b) const
 { double d = a.XD()*b.WD() - b.XD()*a.WD();
   if (d == 0) d = a.YD()*b.WD() - b.YD()*a.WD();
   return d < 0;
 }
};


class rp_greater: public rp_smaller {
public:
bool operator()(const rat_point& p, const rat_point& q) const
{ return rp_smaller::operator()(q,p); }
};

class rp_greater_F: public rp_smaller_F {
public:
bool operator()(const rat_point& p, const rat_point& q) const
{ return rp_smaller_F::operator()(q,p); }
};



int orientation2(const rat_point& a, const rat_point& b, 
                 const rat_point& c, const rat_point& d)
{  
  const double eps0 = ldexp(1.0,-53);

  double cx =  c.XD(); 
  double cy =  c.YD();
  double cw =  c.WD();

  double dx =  d.XD(); 
  double dy =  d.YD();
  double dw =  d.WD();

  double aybx = a.YD()*b.XD();
  double axby = a.XD()*b.YD();
  double axbw = a.XD()*b.WD();
  double bxaw = b.XD()*a.WD();
  double aybw = a.YD()*b.WD();
  double byaw = b.YD()*a.WD();    
    
  double E1 = cx*(byaw-aybw) + cy*(axbw-bxaw) + cw*(aybx-axby); 
  double E2 = dx*(byaw-aybw) + dy*(axbw-bxaw) + dw*(aybx-axby); 


  if (rat_point::use_static_filter) 
  { if (rat_point::ori_eps_static == -1)
        rat_point::set_static_orientation_error_bound(rat_point::x_abs_max,
                                                      rat_point::y_abs_max,
                                                      rat_point::w_abs_max);
    int ori1 = MAXINT; 
    int ori2 = MAXINT; 

    double eps = rat_point::ori_eps_static;
    
    if (E1 >  eps) ori1 = -1;
    else if (E1 < -eps) ori1 = 1;
 
    if (E2 >  eps) ori2 = -1;
    else if (E2 < -eps) ori2 = 1;

    if (ori1 != MAXINT && ori2 != MAXINT)
      return (ori1 == ori2) ? ori1 : 0;
   }    
    
   int ori1 = MAXINT; 
   int ori2 = MAXINT; 
    
   fp::clear_sign_bit(cx); fp::clear_sign_bit(cy);
   fp::clear_sign_bit(dx); fp::clear_sign_bit(dy);
   fp::clear_sign_bit(aybx); fp::clear_sign_bit(axby);
   fp::clear_sign_bit(axbw); fp::clear_sign_bit(bxaw);
   fp::clear_sign_bit(aybw); fp::clear_sign_bit(byaw);  
    
   double eps1 = 11*(cx*(byaw+aybw) + cy*(axbw+bxaw) + cw*(aybx+axby))*eps0;
   double eps2 = 11*(dx*(byaw+aybw) + dy*(axbw+bxaw) + dw*(aybx+axby))*eps0;


   if (E1 >  eps1) ori1 = -1;
   else if (E1 < -eps1) ori1 = 1;
        else if (eps1 < 1) ori1 = 0; 

   if (E2 >  eps2) ori2 = -1;
   else if (E2 < -eps2) ori2 = 1;
        else if (eps2 < 1) ori2 = 0; 

   if (ori1 == MAXINT) ori1 = orientation(a,b,c);
   if (ori2 == MAXINT) ori2 = orientation(a,b,d);

   return (ori1 == ori2) ? ori1 : 0;
}



class orientation_predicate {

  rat_point a;
  rat_point b;

  double axbw_bxaw; double axbw_bxaw_abs;
  double byaw_aybw; double byaw_aybw_abs;
  double aybx_axby; double aybx_axby_abs; 

public:

orientation_predicate(const rat_point& aa, const rat_point& bb) : a(aa),b(bb)
{  
  double aybx = a.YD()*b.XD();
  double axby = a.XD()*b.YD();
  double axbw = a.XD()*b.WD();
  double bxaw = b.XD()*a.WD();
  double aybw = a.YD()*b.WD();
  double byaw = b.YD()*a.WD();    

  axbw_bxaw = axbw-bxaw; 
  byaw_aybw = byaw-aybw; 
  aybx_axby = aybx-axby; 

  axbw_bxaw_abs = fabs(axbw)+fabs(bxaw); 
  byaw_aybw_abs = fabs(byaw)+fabs(aybw); 
  aybx_axby_abs = fabs(aybx)+fabs(axby); 
};

double get_xf() const { return byaw_aybw; }
double get_yf() const { return axbw_bxaw; }
double get_wf() const { return aybx_axby; }
    

int operator()(const rat_point& c) const
{
  const double eps0 = ldexp(1.0,-53);

  double cx = c.XD();
  double cy = c.YD();
  double cw = c.WD();

  double E = cx*(byaw_aybw) + cy*(axbw_bxaw) + cw*(aybx_axby); 

  if (rat_point::use_static_filter) 
  { if (rat_point::ori_eps_static == -1)
        rat_point::set_static_orientation_error_bound(rat_point::x_abs_max,
                                                      rat_point::y_abs_max,
                                                      rat_point::w_abs_max);
    double eps = rat_point::ori_eps_static;
    
    if (E >  eps) return -1;
    if (E < -eps) return  1;
   }    


   fp::clear_sign_bit(cx); 
   fp::clear_sign_bit(cy);

   double eps = 11*(cx*(byaw_aybw_abs) + cy*(axbw_bxaw_abs) 
                                       + cw*(aybx_axby_abs))*eps0;

   if (E >  eps) return -1;
   if (E < -eps) return  1;
   if (eps < 1)  return  0; 

   integer AX = a.X(); integer AY = a.Y(); integer AW = a.W();
   integer BX = b.X(); integer BY = b.Y(); integer BW = b.W();
   integer CX = c.X(); integer CY = c.Y(); integer CW = c.W();

   integer D =  BX*(CY*AW-AY*CW)+BY*(AX*CW-CX*AW)+BW*(AY*CX-AX*CY);

   return sign(D);
}

};




void quick_hull1(rat_point* l, rat_point* r, list<rat_point>& result)
{
  if (r <= l+1) return;

  rat_point a = *l;
  rat_point b = *r;

  if (r == l+2) 
  { if (orientation(a,*(l+1),b) < 0) result.append(*(l+1));
    return;
   }

  if (r == l+3) 
  { if (orientation(a,l[1],l[2]) < 0 && orientation(l[1],l[2],b) < 0) 
    { result.append(l[1]);
      result.append(l[2]);
      return;
     }
   }

  rat_vector vec = b-a;
  rat_point* k = l;
  rat_point c = a;
  rat_point d = b;

  for(rat_point* p=l+1; p<r; p++)
  {  int orient = orientation(c,d,*p);
     //if (orient > 0 || (orient == 0 && side_of_halfspace(c,d,*p) > 0))
     if (orient > 0 || (orient == 0 && compare(c,*p) > 0))
     { k = p; 
       c = *p; 
       d = c+vec; 
      } 
   }

  //if (k == l) return;
  if (orientation(a,b,c) == 0) return;

  swap(*(l+1),*k);

  rat_point* i = l+1;
  rat_point* j = r;

  rat_point* p = i+1;
  while (p < j)
  { if (orientation(a,c,*p) > 0)
      swap(*++i,*p++);
    else
      if (orientation(c,b,*p) > 0)
        swap(*--j,*p);
      else
        p++;
   }

  swap(*(l+1),*i);
  *--j = c;

  quick_hull1(l,i,result);
  result.append(c);
  quick_hull1(j,r,result);
}


list<rat_point> quick_hull1(const list<rat_point>& L)
{ int n = L.length();
  rat_point* A = new rat_point[n+1];

  list<rat_point> result;

  rat_point a = L.front();
  rat_point b = L.front();

  rat_point p;
  forall(p,L)
  { if (rat_point::cmp_xy(p,a) < 0) a = p;
    if (rat_point::cmp_xy(p,b) > 0) b = p;
   }

  rat_point* l = A;
  rat_point* r = A+n;

  *l = a;
  *r = a;
  forall(p,L) 
  { if (identical(p,a) || identical(p,b)) continue;
    if (orientation(a,b,p) > 0) 
      *++l = p;
    else
      *--r = p;
   }
  *++l = b;

  result.append(A[0]);
  quick_hull1(A,l,result);
  result.append(*l);
  quick_hull1(l,A+n,result);
  delete[] A;
  return result;
}




void quick_hull2(rat_point* l, rat_point* r, list<rat_point>& result)
{
  if (r <= l+1) return;

  rat_point a = *l;
  rat_point b = *r;

  if (r == l+2) 
  { if (orientation(a,*(l+1),b) < 0) result.append(*(l+1));
    return;
   }

  if (r == l+3) 
  { if (orientation(a,l[1],l[2]) < 0 && orientation(l[1],l[2],b) < 0) 
    { result.append(l[1]);
      result.append(l[2]);
      return;
     }
   }

  rat_vector vec = b-a; 
  rat_point* k = l;
  rat_point c = a;
  rat_point d = b;

  if (identical(a,b)) 
  { vec = rat_vector(integer(0),integer(-1),integer(1));
    d = c+vec;
   }

  for(rat_point* p=l+1; p<r; p++)
  {  int orient = orientation(c,d,*p);
     //if (orient > 0 || (orient == 0 && side_of_halfspace(c,d,*p) > 0)) 
     if (orient > 0 || (orient == 0 && compare(c,*p) > 0)) 
     { k = p; 
       c = *p; 
       d = c+vec; 
      } 
   }

  if (k == l) return;
  //if (orientation(a,b,c) == 0) return;

  swap(*(l+1),*k);

  rat_point* i = l+1;
  rat_point* j = r;

  rat_point* p = i+1;
  while (p < j)
  { if (orientation(a,c,*p) > 0)
      swap(*++i,*p++);
    else
      if (orientation(c,b,*p) > 0)
        swap(*--j,*p);
      else
        p++;
   }

  swap(*(l+1),*i);
  *--j = c;

  quick_hull2(l,i,result);
  result.append(c);
  quick_hull2(j,r,result);
}







list<rat_point> quick_hull2(const list<rat_point>& L)
{ int n = L.length();
  if (n < 1) return L;

  rat_point* A = new rat_point[n+1];
  rat_point* q = A-1;
  rat_point* a = A;

  rat_point p;
  forall(p,L)
  { *++q = p;
    if (rat_point::cmp_xy(p,*a) < 0) a = q;
   }
  *++q = *a;
  swap(*A,*a);

  assert(q == A+n);

  list<rat_point> result;
  result.append(*a);
  quick_hull2(A,q,result);


  delete[] A;
  return result;
}



void quick_hull3(rat_point* l, rat_point* r, list<rat_point>& result)
{
  if (r <= l+1) return;

  if (r <= l+1) return;

  rat_point a = *l;
  rat_point b = *r;

  if (r == l+2) 
  { if (orientation(a,*(l+1),b) < 0) result.append(*(l+1));
    return;
   }

  if (r == l+3) 
  { rat_point p = l[1];
    rat_point q = l[2];
    int ori1 = orientation(p,q,a);
    int ori2 = orientation(p,q,b);
    if (ori1 < 0 && ori2 < 0) 
    { result.append(p);
      result.append(q);
      return;
     }
    if (ori1 > 0 && ori2 > 0) 
    { result.append(q);
      result.append(p);
      return;
     }
   }


  if (r == l+4) 
  { rat_point p1 = l[1];
    rat_point p2 = l[2];
    rat_point p3 = l[3];
    if (rat_point::cmp_xy(p1,p2) > 0) swap(p1,p2);
    if (rat_point::cmp_xy(p2,p3) > 0) 
    { swap(p2,p3);
      if (rat_point::cmp_xy(p1,p2) > 0) swap(p1,p2);
     }
    if (orientation(p1,p2,a) < 0 && orientation(p1,p2,b) < 0 &&
        orientation(p2,p3,a) < 0 && orientation(p2,p3,b) < 0) 
    { result.append(p1); 
      result.append(p2); 
      result.append(p3); 
      return;
     }
   }

  if (r == l+5) 
  { rat_point p1 = l[1];
    rat_point p2 = l[2];
    rat_point p3 = l[3];
    rat_point p4 = l[4];
    if (rat_point::cmp_xy(p1,p2) > 0) swap(p1,p2);
    if (rat_point::cmp_xy(p2,p3) > 0) swap(p2,p3);
    if (rat_point::cmp_xy(p3,p4) > 0) swap(p3,p4);
    if (rat_point::cmp_xy(p1,p2) > 0) swap(p1,p2);
    if (rat_point::cmp_xy(p2,p3) > 0) swap(p2,p3);
    if (rat_point::cmp_xy(p1,p2) > 0) swap(p1,p2);
    if (orientation(p1,p2,a) < 0 && orientation(p1,p2,b) < 0 &&
        orientation(p2,p3,a) < 0 && orientation(p2,p3,b) < 0 && 
        orientation(p3,p4,a) < 0 && orientation(p3,p4,b) < 0) 
    { result.append(p1); 
      result.append(p2); 
      result.append(p3); 
      result.append(p4); 
      return;
     }
   }


  rat_vector vec = b-a;
  rat_point* k = l+1;

  rat_point c = *k;
  rat_point d = c+vec;

  for(rat_point* p=l+2; p<r; p++)
  {  int orient = orientation(c,d,*p);
     //if (orient > 0 || (orient == 0 && side_of_halfspace(c,d,*p) > 0)) 
     if (orient > 0 || (orient == 0 && compare(c,*p) > 0)) 
     { k = p; 
       c = *k; 
       d = c+vec; 
      } 
   }

  if (orientation(a,b,c) == 0) return; 


  swap(*(l+1),*k);
  rat_point* i = l+1;
  rat_point* j = r;

  rat_point* p = i+1;
  while (p < j)
  { if (orientation(a,c,*p) > 0)
      swap(*++i,*p++);
    else
      if (orientation(c,b,*p) > 0)
        swap(*--j,*p);
      else
        p++;
   }

  swap(*(l+1),*i);
  *--j = c;

  quick_hull3(l,i,result);
  result.append(c);
  quick_hull3(j,r,result);
}


list<rat_point> quick_hull3(const list<rat_point>& L)
{ int n = L.length();
  rat_point* A = new rat_point[n+1];

  list<rat_point> result;

  rat_point a = L.front();
  rat_point b = L.front();

  rat_point p;
  forall(p,L)
  { if (rat_point::cmp_xy(p,a) < 0) a = p;
    if (rat_point::cmp_xy(p,b) > 0) b = p;
   }

  rat_point* l = A;
  rat_point* r = A+n;

  *l = a;
  *r = a;
  forall(p,L) 
  { if (identical(p,a) || identical(p,b)) continue;
    if (orientation(a,b,p) > 0) 
      *++l = p;
    else
      *--r = p;
   }
  *++l = b;

  result.append(A[0]);
  quick_hull3(A,l,result);
  result.append(*l);
  quick_hull3(l,A+n,result);
  delete[] A;
  return result;
}



void quick_hull4(rat_point* l, rat_point* r, list<rat_point>& result, int delta)
{
  if (r <= l+1) return;

  rat_point a = *l;
  rat_point b = *r;

  if (r == l+2) 
  { //if (orientation(a,*(l+1),b) < 0) 
    result.append(*(l+1));
    return;
   }



/*
  if (r == l+3) 
  { rat_point p = l[1];
    rat_point q = l[2];

    //int ori1 = orientation(p,q,a);
    //int ori2 = orientation(p,q,b);
    //int orient2 =  (ori1 == ori2) ? ori1 : 0; 

    int orient2 = orientation2(p,q,a,b);

    if (orient2 < 0)
    { result.append(p);
      result.append(q);
      return;
     }

    if (orient2 > 0) 
    { result.append(q);
      result.append(p);
      return;
     }
   }
*/



  if (r <= l+delta) 
  {  // graham scan
    if (rat_point::cmp_xy(a,b) < 0)
       quicksort(l,r,rp_smaller());
    else
       quicksort(l,r,rp_greater());

    rat_point* j = l+1;
    for(rat_point* p = l+2; p<=r; p++)
    { while(j >= l+1 && orientation(*(j-1),*j,*p) >= 0) j--;
      *++j = *p;
     }
    for(rat_point*p = l+1; p<j; p++) result.append(*p);
    return;
   }



  rat_vector vec = b-a;
  rat_point* k = l+1;

  rat_point c = *k;
  rat_point d = c+vec;

  for(rat_point* p=l+2; p<r; p++)
  {  int orient = orientation(c,d,*p);
     //if (orient > 0 || (orient == 0 && side_of_halfspace(c,d,*p) > 0)) 
     if (orient > 0 || (orient == 0 && compare(c,*p) > 0))
     { k = p; 
       c = *k; 
       d = c+vec; 
      } 
   }

  if (orientation(a,b,c) == 0) return; 


  swap(*(l+1),*k);
  rat_point* i = l+1;
  rat_point* j = r;

/*
  orientation_predicate oac(a,c);
  orientation_predicate ocb(c,b);
*/

  rat_point* p = i+1;
  while (p < j)
  { if (orientation(a,c,*p) > 0)   //if (oac(*p) > 0)
      swap(*++i,*p++);
    else
      if (orientation(c,b,*p) > 0) //if (ocb(*p) > 0)
        swap(*--j,*p);
      else
        p++;
   }

  swap(*(l+1),*i);
  *--j = c;


  quick_hull4(l,i,result,delta);
  result.append(c);
  quick_hull4(j,r,result,delta);
}


list<rat_point> quick_hull4(const list<rat_point>& L, int d = 256)
{ int n = L.length();
  rat_point* A = new rat_point[n+1];

  list<rat_point> result;

  rat_point a = L.front();
  rat_point b = L.front();

  rat_point p;
  forall(p,L)
  { if (rat_point::cmp_xy(p,a) < 0) a = p;
    if (rat_point::cmp_xy(p,b) > 0) b = p;
   }

  rat_point* l = A;
  rat_point* r = A+n;

  *l = a;
  *r = a;
  forall(p,L) 
  { if (identical(p,a) || identical(p,b)) continue;
    if (orientation(a,b,p) > 0) 
      *++l = p;
    else
      *--r = p;
   }
  *++l = b;

  result.append(A[0]);
  quick_hull4(A,l,result,d);
  result.append(*l);
  quick_hull4(l,A+n,result,d);
  delete[] A;
  return result;
}

void quick_hull5(rat_point* l, rat_point* r, list<rat_point>& result, int delta)
{
  if (r <= l+1) return;

  rat_point a = *l;
  rat_point b = *r;

  if (r == l+2) 
  { //if (orientation(a,*(l+1),b) < 0) 
    result.append(*(l+1));
    return;
   }

  if (r <= l+delta) 
  {  // graham scan
    if (rat_point::cmp_xy(a,b) < 0)
       quicksort(l,r,rp_smaller(),rp_smaller_F());
    else
       quicksort(l,r,rp_greater(),rp_greater_F());

    rat_point* j = l+1;
    for(rat_point* p = l+2; p<=r; p++)
    { rat_point pp = *p;
      while(orientation(*(j-1),*j,pp) >= 0) j--;
      *++j = pp;
     }
    for(rat_point*p = l+1; p<j; p++) result.append(*p);
    return;
   }


  rat_vector vec = b-a;
  rat_point* k = l+1;
  rat_point c = *k;
  rat_point d = c+vec;

  for(rat_point* p=l+2; p<r; p++)
  {  int orient = orientation(c,d,*p);
     //if (orient > 0 || (orient == 0 && side_of_halfspace(c,d,*p) > 0))
     if (orient > 0 || (orient == 0 && compare(c,*p) > 0))
     { k = p; 
       c = *k; 
       d = c+vec; 
      } 
   }

  if (orientation(a,b,c) == 0) return; 


  swap(*(l+1),*k);
  rat_point* i = l+1;
  rat_point* j = r;

  rat_point* p = i+1;
  while (p < j)
  { if (orientation(a,c,*p) > 0)
      swap(*++i,*p++);
    else
      if (orientation(c,b,*p) > 0)
        swap(*--j,*p);
      else
        p++;
   }

  swap(*(l+1),*i);
  *--j = c;

  quick_hull5(l,i,result,delta);
  result.append(c);
  quick_hull5(j,r,result,delta);
}


list<rat_point> quick_hull5(const list<rat_point>& L, int d = 256)
{ int n = L.length();
  rat_point* A = new rat_point[n+2];

  list<rat_point> result;

  rat_point a = L.front();
  rat_point b = L.front();

  rat_point p;
  forall(p,L)
  { if (rat_point::cmp_xy(p,a) < 0) a = p;
    if (rat_point::cmp_xy(p,b) > 0) b = p;
   }

  rat_point* l = A+1;
  rat_point* r = A+n+1;

  A[0] = b;

  *l = a;
  *r = a;
  forall(p,L) 
  { if (identical(p,a) || identical(p,b)) continue;
    if (orientation(a,b,p) > 0) 
      *++l = p;
    else
      *--r = p;
   }
  *++l = b;

  result.append(A[1]);
  quick_hull5(A+1,l,result,d);
  result.append(*l);
  quick_hull5(l,A+n+1,result,d);
  delete[] A;
  return result;
}




class qh_elem {
public:
rat_point pt;
bool flag;
};

class qh_smaller {
public:
bool operator()(const qh_elem& p, const qh_elem& q) const
{ return rat_point::cmp_xy(p.pt,q.pt) < 0; }
};


class qh_greater: public qh_smaller {
public:
bool operator()(const qh_elem& p, const qh_elem& q) const
{ return qh_smaller::operator()(q,p); }
};





void quick_hull6(qh_elem* l, qh_elem* r, int delta)
{
  if (r <= l+1) return;

  rat_point a = l->pt;
  rat_point b = r->pt;

  if (r == l+2) 
  { //if (orientation(a,*(l+1),b) < 0) 
    l[1].flag = true;
    return;
   }

  if (r <= l+delta) 
  {  // graham scan
    if (rat_point::cmp_xy(a,b) < 0)
       quicksort(l,r,qh_smaller());
    else
       quicksort(l,r,qh_greater());

    qh_elem* j = l+1;
    for(qh_elem* p = l+2; p<=r; p++)
    { rat_point pp = p->pt;
      while(orientation((j-1)->pt,j->pt,pp) >= 0) j--;
      (++j)->pt = pp;
     }
    for(qh_elem* p = l+1; p<j; p++) p->flag = true;
    return;
   }


  rat_vector vec = b-a;
  qh_elem* k = l+1;

  rat_point c = k->pt;
  rat_point d = c+vec;

  for(qh_elem* p=l+2; p<r; p++)
  {  int orient = orientation(c,d,p->pt);
     //if (orient > 0 || (orient == 0 && side_of_halfspace(c,d,p->pt) > 0))
     if (orient > 0 || (orient == 0 && compare(c,p->pt) > 0))
     { k = p; 
       c = k->pt; 
       d = c+vec; 
      } 
   }

  if (orientation(a,b,c) == 0) return; 


  swap(*(l+1),*k);
  qh_elem* i = l+1;
  qh_elem* j = r;

  qh_elem* p = i+1;
  while (p < j)
  { if (orientation(a,c,p->pt) > 0)
      swap((++i)->pt,(p++)->pt);
    else
      if (orientation(c,b,p->pt) > 0)
        swap((--j)->pt,p->pt);
      else
        p++;
   }

  swap(*(l+1),*i);
  (--j)->pt = c;

  i->flag = true;

  quick_hull6(l,i,delta);
  quick_hull6(j,r,delta);
}


list<rat_point> quick_hull6(const list<rat_point>& L, int d = 256)
{ int n = L.length();

  qh_elem* A = new qh_elem[n+2];

  for(int k=0; k<n+2; k++) A[k].flag = false;


  list<rat_point> result;

  rat_point a = L.front();
  rat_point b = L.front();

  rat_point p;
  forall(p,L)
  { if (rat_point::cmp_xy(p,a) < 0) a = p;
    if (rat_point::cmp_xy(p,b) > 0) b = p;
   }

  int i = 1;
  int j = n+1;

  A[0].pt = b;
  A[1].pt = a;
  A[n+1].pt = a;

  forall(p,L) 
  { if (identical(p,a) || identical(p,b)) continue;
    if (orientation(a,b,p) > 0) 
      A[++i].pt = p;
    else
      if (orientation(a,b,p) < 0) 
         A[--j].pt = p;
   }
  A[++i].pt = b;
  A[--j].pt = b;


  A[1].flag = true;
  quick_hull6(A+1,A+i,d);

  A[j].flag = true;
  quick_hull6(A+j,A+n+1,d);

  for(int k=0; k<n+2; k++) 
     if (A[k].flag) result.append(A[k].pt);
  
  delete[] A;
  return result;
}




template<class rat_point>
bool check_hull(const list<rat_point>& H)
{ int err = 0;
  list_item i;
  forall_items(i,H)
  { list_item j = H.cyclic_succ(i);
    list_item k = H.cyclic_succ(j);
    rat_point a = H[i];
    rat_point b = H[j];
    rat_point c = H[k];
    if (orientation(a,b,c) <= 0) err++;
  }
  return err == 0;
}



//------------------------------------------------------------------------------


void quickhull_list_rec(list<rat_point>& ch, list_item right_it, 
                                             list<rat_point>& out) 
{
  if (out.empty()) return;

  rat_point right = ch[right_it];
  rat_point left  = ch[ch.cyclic_succ(right_it)];

  rat_vector vec = right - left;
  list_item p = out.first();
  rat_point c = left;
  rat_point d = right;

  list_item r;
  forall_items(r,out) 
  { int orient = orientation(c,d,out[r]);
    if (orient == 1 || (orient == 0 && compare(out[p],out[r]) == 1))
    { p = r;
      c = out[p];
      d = c + vec;
     }
   }

  rat_point pivot = out[p];
  out.move_to_back(p);

  list_item it = out.first();

  while (it != p)
  { list_item itm = out.succ(it);
    if (orientation(right,pivot,out[it]) == -1)
        out.move_to_rear(it);
    else if (orientation(pivot,left,out[it]) != -1)
        out.del(it);
    it = itm;
   } 

  list<rat_point> l_out, r_out;
  out.split(p,l_out,out,leda::before);
  out.split(p,out,r_out,leda::after);

  list_item left_it = ch.insert(pivot,right_it);
  quickhull_list_rec (ch, left_it, l_out);
  quickhull_list_rec (ch, right_it, r_out);

}


list<rat_point> quickhull_list(const list<rat_point>& M)
{
  list<rat_point> ch; 
  if (M.empty()) return ch;

  list<rat_point> L = M;

  list<rat_point> upper;
  list<rat_point> lower;

  rat_point x_min = L.front(); 
  rat_point x_max = L.front();

  rat_point s;
  forall(s,L)
  { if (compare(s,x_max) > 0) x_max = s; 
    if (compare(s,x_min) < 0) x_min = s; 
  }

  if ( x_min == x_max ) 
  { ch.append(x_min);
    return ch;
  }

  rat_point p; 
  L.append(p);

  list_item middle = L.last();

  list_item it = L.first();
  while (it != middle)
  { list_item itm = L.succ(it);
    int ori = orientation(x_min,x_max,L[it]);
    if (ori == 1)
        L.move_to_rear(it);   
    else if (ori == 0)
        L.del(it);
    it = itm;
   } 

  L.split(middle,lower,upper,leda::after);
  lower.pop_back();

  ch.append(x_max);
  ch.append(x_min);

  quickhull_list_rec (ch, ch.first(), upper);
  quickhull_list_rec (ch, ch.last(), lower);

  return ch;
}

//------------------------------------------------------------------------------




int main()
{
  int n = read_int("n = ");

  list<rat_point>  L;
  string gen;

  while (gen == "")
  { gen  = read_string("generator d(isk)/s(quare)/c(ircle) : ");
    if (gen[0] == 'd')
       random_points_in_disc(n,10000,L);
    else if (gen[0] == 's')
       random_points_in_square(n,10000,L);
    else if (gen[0] == 'c')
       random_points_on_circle(n,10000,L);
    else gen = "";
  }


  cout << endl;

  list<rat_point> H;

  float T = used_time();

/*
  cout << "CONVEX_HULL      " << flush;
  H = CONVEX_HULL(L);
  cout << string("|C| = %d  time = %.2f",H.size(),used_time(T)) << endl;

  cout << "CONVEX_HULL_IC   " << flush;
  H = CONVEX_HULL_IC(L);
  cout << string("|C| = %d  time = %.2f",H.size(),used_time(T)) << endl;
*/

/*
  cout << "CONVEX_HULL_S    " << flush;
  H = CONVEX_HULL_S(L);
  cout << string("|C| = %d  time = %.2f",H.size(),used_time(T)) << endl;

  cout << "CONVEX_HULL_S3F  " << flush;
  H = CONVEX_HULL_S3F(L);
  cout << string("|C| = %d  time = %.2f",H.size(),used_time(T)) << endl;

  cout << "quick_hull1      " << flush;
  H =  quick_hull1(L);
  cout << string("|C| = %d  time = %.2f",H.length(),used_time(T)) << endl;

  cout << "quick_hull2      " << flush;
  H =  quick_hull2(L);
  cout << string("|C| = %d  time = %.2f",H.length(),used_time(T)) << endl;

  cout << "quick_hull3      " << flush;
  H =  quick_hull3(L);
  cout << string("|C| = %d  time = %.2f",H.length(),used_time(T)) << endl;

  cout << "qh_list          " << flush;
  H =  quickhull_list(L);
  cout << string("|C| = %d  time = %.2f",H.length(),used_time(T)) << endl;
*/

  cout << "quick_hull4      " << flush;
  H =  quick_hull4(L);
  cout << string("|C| = %d  time = %.2f",H.length(),used_time(T)) << endl;

  cout << "quick_hull5      " << flush;
  H =  quick_hull5(L);
  cout << string("|C| = %d  time = %.2f",H.length(),used_time(T)) << endl;
  cout << endl;

  cout << "quick_hull6      " << flush;
  H =  quick_hull6(L);
  cout << string("|C| = %d  time = %.2f",H.length(),used_time(T)) << endl;
  cout << endl;


  return 0;
}
