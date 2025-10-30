/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _real.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define DONOT_SEARCH_FOR_COMMON_EXPRESSIONS

#include <LEDA/numbers/real.h>
#include <LEDA/numbers/fp.h>
#include <LEDA/numbers/interval.h>

LEDA_BEGIN_NAMESPACE
typedef real_rep* real_rep_ref;
int compare(const real_rep_ref& r1, const real_rep_ref& r2)
{
  if (r1 < r2) return -1;
  if (r1 == r2) return 0;
  return 1;
}
LEDA_END_NAMESPACE

#include <LEDA/core/d_array.h>



#include <LEDA/numbers/polynomial.h>
#include <LEDA/numbers/isolating.h>
#include <LEDA/numbers/approximating.h>


#if (defined(_MSC_VER) && _MSC_VER < 1300) || defined(__mipspro__) || defined(__DMC__)
#define MATH_SCOPE ::
#else
#define MATH_SCOPE std::
#endif

#define FP_SCOPE fp::
#define PACKAGE_SCOPE leda::

#undef LEDA_VECTOR
#undef REAL_CONTAINER
#undef REAL_CONTAINER_APPEND
#define LEDA_VECTOR growing_array
#define REAL_CONTAINER list<real>&
#define REAL_CONTAINER_APPEND(L, OBJ) (L).append(OBJ)

#define HANDLE_ERROR LEDA_EXCEPTION

LEDA_BEGIN_NAMESPACE

typedef interval_bound_absolute real_interval;

inline rational to_rational(const bigfloat& x) { return x.to_rational(); }
inline string to_string(const bigfloat& x, sz_t dec_prec) { return x.to_string(dec_prec); }
inline bigfloat input_bf(istream& in) { bigfloat x; in >> x; return x; }

static real_interval to_interval(const rational& x, const rational& y)
{
  bigfloat xnum = x.numerator();
  bigfloat xden = x.denominator();
  bigfloat ynum = y.numerator();
  bigfloat yden = y.denominator();
  bigfloat xlow = div(xnum,xden,53,TO_N_INF);
  bigfloat yupp = div(ynum,yden,53,TO_P_INF);

#if __LEDA__ < 501
  double low = to_double(xlow);
  double upp = to_double(yupp);
#else
  double low = xlow.to_double(TO_N_INF);
  double upp = yupp.to_double(TO_P_INF);
#endif

  real_interval I;
  I.set_range(low, upp);
  return I;
}


typedef polynomial<real_interval> Poly_interval;
typedef polynomial<bigfloat> Poly_bigfloat;


real::io_mode real::output_mode=real::IO_interval;
algorithm_type real::isolating_algorithm=USPENSKY;

#if __LEDA__ >= 500
  inline double convert_to_dbl(const bigfloat& x, bool& is_exact)
  { return x.to_double(is_exact); }
  
  inline double convert_to_dbl(const integer& x, bool& is_exact)
  { return x.to_double(is_exact); }  
#else
  static double convert_to_dbl(const bigfloat& x, bool& is_exact)
  {
     double xdbl = to_double(x);
	 // Error found by Michael Hemmer: Call of bigfloat(xdbl) for infinite xdbl crashes
     //is_exact = (FP_SCOPE is_infinite(xdbl)) && (x == bigfloat(xdbl));
	 if( FP_SCOPE is_infinite(xdbl)){
         is_exact = false;
     }else{
         is_exact = (x == bigfloat(xdbl));
     }
     return xdbl;
  }

  static double convert_to_dbl(const integer& x, bool& is_exact)
  {
     double xdbl = to_double(x);
	 // Error found by Michael Hemmer: Call of integer(xdbl) for infinite xdbl crashes
     //is_exact = (FP_SCOPE is_infinite(xdbl)) && (x == integer(xdbl));
	 if( FP_SCOPE is_infinite(xdbl)){
          is_exact = false;
     }else{
          is_exact = (x == integer(xdbl));
     }
     return xdbl;
  }

#endif  

#if __LEDA__ > 501
  #define convert_to_bf bigfloat::from_rational
#else
  static bigfloat convert_to_bf(const rational& r, sz_t prec, 
                                rounding_modes rmode, bool& is_exact)
  {
          bigfloat num(r.numerator()), den(r.denominator());
          return div(num, den, prec, rmode, is_exact);
  }
#endif  


  const integer zero_integer(0);


inline bigfloat ldexp_bf(const bigfloat& x, const integer& k) {  
  return x*ipow2(k);
}

inline long v2(const integer& x, integer & r){
  int v = x.zeros();
  r = x >> v;
  return v;
}

inline long v2(const rational& x, rational &r){
  integer num,den;
  long vn = v2(x.numerator(), num);
  long vd = v2(x.denominator(), den);
  r = rational(num,den);
  return(vn-vd);
}

#ifndef v_IS_LONG
inline integer v2(const bigfloat& x, integer& r){
  integer s = x.get_significant();
  integer es = v2(s,r);
  return (es+x.get_exponent());
}
#else
inline long v2(const bigfloat& x, integer& r){
  integer s = x.get_significant();
  integer es = v2(s,r);
  return (es+x.get_exponent()).to_long();
}
#endif

inline integer Maximum(const integer& x, const integer& y) { return (x >= y ? x : y); }
inline integer Minimum(const integer& x, const integer& y) { return (x <= y ? x : y); }
inline long    Maximum(long x, long y) { return (x >= y ? x : y); }
inline long    Minimum(long x, long y) { return (x <= y ? x : y); }


// const int K = 2;

bool refine(Polynomial& P, rational& a, rational& b, bool& squarefree)
{

   // P should have only simple roots -> change P, Ps
   Polynomial Ps = diff(P);
   if(!squarefree)
   {
      if(refine_diff(P,Ps)) return true;
      squarefree = true;
   }

   // the interval (a,b) should contain no root of P' -> change a,b
   if(refine_interval(P,Ps,a,b)) return true;

   return false; // root not found exactly
}




/*bool possible_common_roots(Polynomial& P,Polynomial& Q)
{
   real r = resultant(P,Q);
   return (r==0);
}
*/


bool refine_diff(Polynomial& P, Polynomial& Q)
{
   if(!possible_common_roots(P,Q)) return false;
   Polynomial G = poly_gcd(P,Q);
   if(G.degree() > 0) 
   {
      Polynomial q,r;
      P.euclidean_division(G,q,r);
      P = q;
      Q = diff(P);
      if(P.degree() <=2) return true;
   }
   return false;
}



bool refine_interval(Polynomial P, Polynomial Q, rational& a, rational& b)
{

   if(Q.degree()==0) return false;

   // R(x) = Q((b-a)*x + a)
   Polynomial R = scale_up(translate(Q,real(a)),real(b-a));
   R.translate(real(-1),real(2)); // Rnew(x) = 2^dR(x-1/2) = 2^dQ((b-a)(x-1/2)+a)
   R.scale_up(real(2));           // Rnew(x) = R(2x) = 2^dQ((b-a)(2x-1/2)+a)

   if(sign_variations(variation_transformation(R)) > 0)
   {
      int s0 = sign(P.evaluate(real(a)));
      do
      {
        // R has possible roots in (0,1)
        // Q has possible roots in [a,b]

        rational m = (a+b)/rational(2,1);
        int s = sign(P.evaluate(real(m)));
        if(s==0)
        {
           a=m;
           b=m;
           return true;
        }
        if(s0 == s) a=m;
        else b=m;
        R = scale_up(translate(Q,real(a)),real(b-a));
        R.translate(real(-1),real(2)); 
        R.scale_up(real(2));  
      } while(sign_variations(variation_transformation(R)) > 0);
   }

   return false;
}


enum constructortype { DOUBLE = 0, BIGFLOAT = 1, NEGATION = 2,
SQUAREROOT = 3, ADDITION = 4, SUBTRACTION = 5,  MULTIPLICATION = 6, 
DIVISION = 7, ROOT = 8, DIAMOND = 9, RATIONAL = 10 };

#ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS

class pair_ref
{
public:

  real_rep_ref ref1;
  real_rep_ref ref2;

  pair_ref() {}
  pair_ref(real_rep_ref r1, real_rep_ref r2) : ref1(r1) , ref2(r2) {}

  friend int compare(const pair_ref&, const pair_ref&);
};

int compare(const pair_ref& p1, const pair_ref& p2)
{
  int c=compare(p1.ref1, p2.ref1);
  if(c==0) return(compare(p1.ref2, p2.ref2)); // p1.ref1 == p2.ref1
  return c;
}

class deg_ref
{
  public:
  
  real_rep_ref ref;
  int deg;

  deg_ref() {}
  deg_ref(real_rep_ref r, int d) : ref(r) , deg(d) {}

  friend int compare(const deg_ref&, const deg_ref&);
};

int compare(const deg_ref& dr1, const deg_ref& dr2)
{
  int c=compare(dr1.ref, dr2.ref);
  if(c==0) 
  {
    if(dr1.deg < dr2.deg) return -1;
    if(dr1.deg == dr2.deg) return 0;
    return 1;
  }
  return c;
}

int compare(const double& d1, const double& d2)
{
  if(d1 < d2) return -1;
  if(d1 == d2) return 0;
  return 1;
}

class pol_ref
{
  public:
  
  int j_;
  real_interval I_;
  bool interval_known;
  Polynomial pol;

  pol_ref() {}
  pol_ref(Polynomial p, real_interval I, int j) 
     : j_(j), pol(p), I_(I), interval_known(true) {}
  pol_ref(Polynomial p, int j) : j_(j), pol(p), interval_known(false) {}

  friend int compare(const pol_ref&, const pol_ref&);
};

int compare(const pol_ref& pr1, const pol_ref& pr2)
{
  if(pr1.pol < pr2.pol) return -1;
  if(pr1.pol > pr2.pol) return  1;

  // pr1.pol == pr2.pol
  if(pr1.j_ != -1 && pr2.j_ != -1)
  {

     if(pr1.j_ < pr2.j_) return -1;
     if(pr1.j_ > pr2.j_) return  1;
     return 0; // equal root
  }


  // pr1.pol == pr2.pol
  // check intervals
  if(pr1.interval_known && pr2.interval_known)
  {
     if(intersect_open(pr1.I_, pr2.I_)) return 0;
     if(pr1.I_ <= pr2.I_) return -1;
     return 1;
  }
  return 1;
}
#endif // not DONOT_SEARCH_FOR...


#ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS

class current_real_reps
{
 protected:

  leda_d_array<double, real_rep* >       current_doubles;
  leda_d_array<bigfloat, real_rep* >     current_bigfloats;
  leda_d_array<real_rep_ref, real_rep* > current_negs;
  leda_d_array<real_rep_ref, real_rep* > current_sqrts;
  leda_d_array<deg_ref, real_rep* >      current_roots; 
  leda_d_array<pair_ref, real_rep* >     current_adds;
  leda_d_array<pair_ref, real_rep* >     current_subs;
  leda_d_array<pair_ref, real_rep* >     current_mults;
  leda_d_array<pair_ref, real_rep* >     current_divs;
  leda_d_array<pol_ref, real_rep* >      current_diamonds;

 public:
  current_real_reps();

  real_rep* find(double); 
  real_rep* find(bigfloat); 
  real_rep* find(constructortype, real_rep_ref);
  real_rep* find(real_rep_ref, int);
  real_rep* find(constructortype, real_rep_ref, real_rep_ref);
  real_rep* find(Polynomial, int);
  std::vector<real_rep*> find(Polynomial, real_interval);

  void insert(real_rep*, double);
  void insert(real_rep*, bigfloat);
  void insert(constructortype, real_rep*, real_rep_ref);
  void insert(real_rep*, real_rep_ref, int);
  void insert(constructortype, real_rep*, real_rep_ref, real_rep_ref);
  void insert(real_rep*, Polynomial, real_interval, int);

  void delete_element(double);
  void delete_element(bigfloat);
  void delete_element(constructortype, real_rep_ref);
  void delete_element(real_rep_ref, int);
  void delete_element(constructortype, real_rep_ref, real_rep_ref);
  void delete_element(Polynomial, real_interval, int);
};

current_real_reps::current_real_reps() :
               current_doubles((real_rep*) NULL),
               current_bigfloats((real_rep*) NULL),
               current_negs((real_rep*) NULL),
               current_sqrts((real_rep*) NULL),
               current_roots((real_rep*) NULL),
               current_adds((real_rep*) NULL),
               current_subs((real_rep*) NULL),
               current_mults((real_rep*) NULL),
               current_divs((real_rep*) NULL),
               current_diamonds((real_rep*) NULL) {};

real_rep* current_real_reps::find(constructortype t, real_rep_ref r)
{
  switch(t){
    case NEGATION:   return current_negs[r];
    case SQUAREROOT: return current_sqrts[r];
  }
}

real_rep* current_real_reps::find(double d) 
{
  return current_doubles[d];
}

real_rep* current_real_reps::find(bigfloat b) 
{
  return current_bigfloats[b];
}

real_rep* current_real_reps::find(real_rep_ref r, int d) 
{ 
  return current_roots[deg_ref(r,d)];
}

real_rep* current_real_reps::find(constructortype t, real_rep_ref r1, real_rep_ref r2)
{
  switch(t){
    case ADDITION:       return current_adds[pair_ref(r1,r2)];
    case SUBTRACTION:    return current_subs[pair_ref(r1,r2)];
    case MULTIPLICATION: return current_mults[pair_ref(r1,r2)];
    case DIVISION:       return current_divs[pair_ref(r1,r2)];
  }
}

real_rep* current_real_reps::find(Polynomial P, int j)
{
  return current_diamonds[pol_ref(P, j)];
}

std::vector<real_rep*> current_real_reps::find(Polynomial P, real_interval I)
{
  std::vector<real_rep*> VL(3);
  pol_ref pr;
  pol_ref pi(P,I,-1);
  int i=0;
  forall_defined(pr,current_diamonds)
  {
     if(compare(pr,pi)==0)
     {
        VL[i] = current_diamonds[pr];
        i++;
        if(i==3) return VL;
     }
  }
  return VL;
}

void current_real_reps::insert(real_rep* e, double d) 
{
  current_doubles[d]=e;
}

void current_real_reps::insert(real_rep* e, bigfloat b) 
{
  current_bigfloats[b]=e;
}

void current_real_reps::insert(constructortype t, real_rep* e, real_rep_ref r)
{
  switch(t){
  case NEGATION: current_negs[r]=e;
    break;
  case SQUAREROOT: current_sqrts[r]=e;
    break;
  }
}

void current_real_reps::insert(real_rep* e, real_rep_ref r, int d) 
{
  current_roots[deg_ref(r,d)]=e;
}

void current_real_reps::insert(constructortype t, real_rep* e, real_rep_ref r1, real_rep_ref r2)
{
  switch(t){
  case ADDITION: current_adds[pair_ref(r1,r2)]=e;
    break;
  case SUBTRACTION: current_subs[pair_ref(r1,r2)]=e;
    break;
  case MULTIPLICATION: current_mults[pair_ref(r1,r2)]=e;
    break;
  case DIVISION: current_divs[pair_ref(r1,r2)]=e;
    break;
  }
}

void current_real_reps::insert(real_rep* e, Polynomial P, real_interval I, int j)
{
  current_diamonds[pol_ref(P,I,j)]=e;

/*
  cout << " *** nach insert: \n";
  pol_ref s;
  forall_defined(s,current_diamonds)
  {
     cout << " s.P = " << s.pol << endl;
     cout << " s.I = " << s.I_ << endl;
     cout << " s.j = " << s.j_ << endl;
  }
*/

}

void current_real_reps::delete_element(double d) 
{  
  current_doubles.undefine(d);
}

void current_real_reps::delete_element(bigfloat b) 
{  
  current_bigfloats.undefine(b);
}

void current_real_reps::delete_element(constructortype t, real_rep_ref r)
{
  switch(t){
  case NEGATION: current_negs.undefine(r);
    break;
  case SQUAREROOT: current_sqrts.undefine(r);
    break;
  }
}

void current_real_reps::delete_element(real_rep_ref r, int d) 
{
  current_roots.undefine(deg_ref(r,d));
}

void current_real_reps::delete_element(constructortype t, real_rep_ref r1, real_rep_ref r2)
{
  switch(t){
  case ADDITION: current_adds.undefine(pair_ref(r1,r2));
    break;
  case SUBTRACTION: current_subs.undefine(pair_ref(r1,r2));
    break;
  case MULTIPLICATION: current_mults.undefine(pair_ref(r1,r2));
    break;
  case DIVISION: current_divs.undefine(pair_ref(r1,r2));
    break;
  }
}

void current_real_reps::delete_element(Polynomial P, real_interval I, int j=-1)
{
  current_diamonds.undefine(pol_ref(P,I,j));
}

static current_real_reps* CURRENT_REAL_REPS = new current_real_reps();


#endif // not DONOT_SEARCH_FOR...


struct sep_bound;
class real_rep 
{ 
protected:
  friend class real;
  struct sep_bound
  {
  #ifdef v_IS_LONG
    long v;
  #else
    integer v;
  #endif    
    integer U, L;
  #ifndef BFMSS_ONLY
    integer M;
    long D;
    integer lc, tc;
    integer mu, nu;
  #endif

    sep_bound()  {}
  };

  struct app_bf  
  { bigfloat NUM_BF;
    bigfloat ERROR_BF; 
    app_bf()  {}
    app_bf(const bigfloat& x, const bigfloat& error_x = bigfloat::pZero)
    {  NUM_BF = x;  ERROR_BF = error_x;  }
    LEDA_MEMORY(app_bf)
  };

  enum statustype  { CLEARED = 0, VISITED = 1 };

  real_interval  I; 

  app_bf* APP_BF_PTR; 
  constructortype CON;
  real_rep *OP1, *OP2; 

  atomic_counter count;
  unsigned long ref_plus()  { return ++count; }
  unsigned long ref_minus() { return --count; }

  int d;
  long status;
  sep_bound* sep_bound_ptr;
#ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
  Polynomial OPOL;
  int j_;
#endif

  Polynomial RPOL;
  rational left, right;
  bool first, pol_squarefree;

#ifndef BFMSS_ONLY
  bool is_integral_pol;
  bool degree_measure_possible;
  bool system_bound_computed;
#endif  
  bool is_general_;
  bool is_rational_;

  real_rep();
  real_rep(double); 
  real_rep(const bigfloat&);
  real_rep(const rational&);

  ~real_rep(); 

  LEDA_MEMORY(real_rep)

  void init_app_bf();
  friend double to_double(const real&);
  friend bigfloat to_bigfloat(const real&);
  void adjust_interval();
  void compute_op(long); 
  void compute_approximation(long); 
  int sign_with_separation_bound(const integer&,bool=false);
  void guarantee_bound_two_to(const integer&);
  inline bigfloat& num_bf()  {  return APP_BF_PTR->NUM_BF;  }
  inline bigfloat& error_bf()  {  return APP_BF_PTR->ERROR_BF;  }
  inline bool exact()  {  return isZero(error_bf());  }
  inline integer num_exp()  {  return ilog2(num_bf());  } 
  inline integer err_exp()  {  return ilog2(error_bf());  } 
  inline real_rep_ref ref() { return this; }
  inline rational& rat() { return left; }
  void delete_current_real_rep();

    bool is_general() const;
    bool is_rational() const;
    rational to_rational();
    
    void set_polynomial(const Polynomial& P);
    rat_Polynomial get_polynomial();
  
  void compute_concrete_parameters();

    integer sep_bfmss();
  #ifndef BFMSS_ONLY
    integer sep_degree_measure();
    integer sep_li_yap();
  #endif



  void compute_parameters();


  void estimate_degree(integer& D);

  void clear_visited_marks();

  #ifndef BFMSS_ONLY
  void compute_concrete_system_bound(integer& concrete_system_bound);
  #else
  void compute_concrete_system_bound(integer& concrete_system_bound)
  { concrete_system_bound = sep_bfmss(); }
  #endif

  void compute_mul_error(bigfloat&);
  void compute_div_error(bigfloat&);
  void compute_sqrt_error(bigfloat&);
  void compute_root_error(bigfloat&);


  void improve_add(const integer&);
  void improve_sub(const integer&);
  void improve_mul(const integer&);
  void improve_div(const integer&);
  void improve_sqrt(const integer&);
  void improve_root(const integer&);
  void improve_rational(const integer&);
  void improve_diamond(const integer&);


  void output_as_dag(ostream& out, int, bool, bool);
  int output_as_expression(ostream& out, int& idx_counter, d_array<real_rep_ref, int>* sub_exp);
  friend ostream& operator<<(ostream&,const real&);

  friend int compare(const real&, const real&);

  friend real operator+(const real&,const real&);
  friend real operator-(const real&,const real&);
  friend real operator*(const real&,const real&);
  friend real operator/(const real&,const real&);
  friend real operator+(const real&);
  friend real operator-(const real&);
  friend real sqrt(const real&);
  friend real sqrtt(const real&);
  friend real root(const real&, int);
  friend real roott(const real&, int);


  friend int  real_roots(const Polynomial&, REAL_CONTAINER, algorithm_type, bool);
  friend real diamond(int, const Polynomial&, algorithm_type, bool );
  friend real diamond(rational, rational, const Polynomial&, algorithm_type,
                        bool);
  friend real diamond_short(rational, rational, const Polynomial&, algorithm_type,
                        bool);

};


  real::real()  {  is_double = true;  value = 0;   PTR = NULL;   }

  real::real(int x)  {  is_double = true;  value = x;  PTR = NULL;  }

  real::real(double x) {  is_double = true;  value = x;  PTR = NULL;  }
  
  real::real(const bigfloat& x) 
  { 
    value = convert_to_dbl(x, is_double);
    if (is_double) 
    {  
       PTR = NULL;
    }
    else
    {
       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
              if(!(PTR = CURRENT_REAL_REPS->find(x))) PTR = new real_rep(x);
              else PTR->ref_plus();
       #else
              PTR = new real_rep(x);
       #endif

    }
  }

  real::real(const integer& x) 
  { 
    value = convert_to_dbl(x, is_double);
    if (is_double) 
    {  
       PTR = NULL;
    }
    else
    {
       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
              if(!(PTR = CURRENT_REAL_REPS->find(x))) PTR = new real_rep(bigfloat(x));
              else PTR->ref_plus();
       #else
              PTR = new real_rep(bigfloat(x));
       #endif

    }
  }

  real::real(const rational& x)  
  {
    is_double = false;
    PTR = new real_rep(x);
/*
    real OP1 = real(x.numerator());
    real OP2 = real(x.denominator());
    PTR = NULL;
    *this = OP1 / OP2;
*/
  }
  
  real::real(real_rep& x_rep)  {  is_double = false;  PTR = &x_rep;  }

  real::real(const real& x) 
  {
     value = x.value;
     is_double = x.is_double;
     PTR = x.PTR;
     if (PTR) PTR->ref_plus();
  }

  real& real::operator=(const real& x)
  { 
     /* we avoid problems by ruling out the assignment |x = x;| */
     //if (this == &x) return (*this); // not necessary

     /* first the actions that must anyway be performed, in the respective cases
        of |is_double| */
     value = x.value; is_double = x.is_double;
     if (x.PTR) x.PTR->ref_plus();

     /* second, an existing reference in PTR must always be deleted;
        if |x.PTR == PTR != NULL| and |is_double == false|, this leaves the
        number of references to |*PTR| unchanged, because we increased
        |x.PTR| before, and for |is_double == true| we in fact lose one 
        reference because |PTR| is set to |NULL| at the end 
     */
     if (PTR && (PTR->ref_minus() ==  0)) delete PTR;

     /* last, we set |this->PTR| */
     PTR = x.PTR;

     return (*this);
  }

  real_rep::real_rep() : count(1) {  
    APP_BF_PTR = NULL;  sep_bound_ptr = NULL;
    status = CLEARED; is_general_ = false; is_rational_ = true;

#ifndef BFMSS_ONLY
    system_bound_computed = false;
#endif
  }
  
  real_rep::real_rep(double x) : count(1) {  
    APP_BF_PTR = NULL;  sep_bound_ptr = NULL;
    OP1 = OP2 = NULL;  CON = DOUBLE;
    I = x;
    status = CLEARED; is_general_ = false; is_rational_ = true;

#ifndef BFMSS_ONLY
    is_integral = (FP_SCOPE get_unbiased_exponent(x) >= 0);
    degree_measure_possible = true;
    system_bound_computed = false;
#endif

    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        CURRENT_REAL_REPS->insert(this, x);
    #endif
  }
  
  real_rep::real_rep(const bigfloat& x) : count(1) { 
    APP_BF_PTR = new app_bf(x,0);  sep_bound_ptr = NULL;
    OP1 = OP2 = NULL;  CON = BIGFLOAT; 
    adjust_interval();
    status = CLEARED; is_general_ = false; is_rational_ = true;

#ifndef BFMSS_ONLY
    is_integral = (PACKAGE_SCOPE sign(x.get_exponent()) >= 0);
    degree_measure_possible = true;
    system_bound_computed = false;
#endif

    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        CURRENT_REAL_REPS->insert(this, x);
    #endif
  }
  
  real_rep::real_rep(const rational& x) : count(1) { 
    APP_BF_PTR = NULL;  sep_bound_ptr = NULL;
    OP1 = OP2 = NULL;  CON = RATIONAL;
    rat() = x; I = x;
    status = CLEARED; is_general_ = false; is_rational_ = true;

#ifndef BFMSS_ONLY
    is_integral = (x.denominator() == 1);
    degree_measure_possible = true;
    system_bound_computed = false;
#endif

    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        CURRENT_REAL_REPS->insert(this, x);
    #endif
  }

  real::~real() 
  {   if (PTR && (PTR->ref_minus() ==  0))  delete PTR; 
  } 

    void real_rep::delete_current_real_rep()
    {
  #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
      switch(CON)
      {
        case DOUBLE: 
          init_app_bf();
          CURRENT_REAL_REPS->delete_element(to_double(num_bf()));
          break;
        case BIGFLOAT: 
          init_app_bf();
          CURRENT_REAL_REPS->delete_element(num_bf());
          break;
        case NEGATION:
        case SQUAREROOT: 
          CURRENT_REAL_REPS->delete_element(CON, OP1->ref());
          break;
        case ROOT: 
          CURRENT_REAL_REPS->delete_element(OP1->ref(), d);
          break;
        case ADDITION:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION:
          CURRENT_REAL_REPS->delete_element(CON, OP1->ref(), OP2->ref());
          break;
        case DIAMOND:
          CURRENT_REAL_REPS->delete_element(OPOL,I,j_);
          break;
      }
  #endif // not DONOT_SEARCH_FOR ...
  }

  real_rep::~real_rep() 
  { 
    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        delete_current_real_rep();
    #endif
    if (APP_BF_PTR) 
      delete APP_BF_PTR;
    if (sep_bound_ptr)
      delete sep_bound_ptr;
    if ( OP1 && (OP1->ref_minus() == 0)) 
      delete OP1;
    if ( OP2 && (OP2->ref_minus() == 0)) 
      delete OP2;

    if(CON == DIAMOND)
    {
       int n;
       n = RPOL.degree();
       for(int i=0; i<=n; i++) 
       {
          if ((RPOL[i].PTR)->ref_minus() == 0) delete RPOL[i].PTR;
       }
    }

  }




void real_rep::init_app_bf() {  
  if (APP_BF_PTR) 
    return;
  if (OP1) 
    OP1->init_app_bf();
  if (OP2) 
    OP2->init_app_bf();

  if(CON == DIAMOND)
  {
     int n;
     n = RPOL.degree();
     for(int i=0; i<=n; i++) (RPOL[i].PTR)->init_app_bf();
  }

  if (I.is_finite()) { 
      bigfloat approximation = to_double(I);
      bigfloat error = I.get_double_error();
      APP_BF_PTR = new app_bf(approximation,error);
      return;
  }
  APP_BF_PTR = new app_bf();
  compute_op(DOUBLE_PREC);
}


void real_rep::adjust_interval() {
#if __LEDA__ < 501
  double n_head = to_double(num_bf());
  double e_head = eps*(FP_SCOPE abs(n_head))+(1+eps)*to_double(error_bf());
#else
  double e_head;
  double n_head = num_bf().to_double(e_head, TO_NEAREST);
  e_head += error_bf().to_double(TO_INF);
  e_head *= roundup_factor;
    // the previous addition above may not be exact, roundup_factor should account for this
#endif

  I.set_midpoint(n_head,e_head);
}


void real::adjust() const {

  if (is_double) 
    return;
  if (PTR->exact())
  {
    real& me = ((real&) *this); // cast away const
    me.value = convert_to_dbl(PTR->num_bf(), me.is_double);
    return;
  }
  PTR->adjust_interval(); 

}



  void real_rep::output_as_dag(ostream& out, int depth, bool _first, bool nl)
  {
     if(!_first)
     {
        for(int i=0; i<depth; i++) out << " ";
     }
     switch(CON)
     {
        case DOUBLE:
               out << "D(" << I << ")";
               break;
        case BIGFLOAT: {
               init_app_bf();
               bigfloat b = num_bf();
               bigfloat i = to_integer(b);
               if (b == i) 
               {
                   out << "B(" << i << ")";
                   break;
               }
               bigfloat low_bound, upp_bound;
               long prec = DOUBLE_PREC;
               low_bound = sub(num_bf(),error_bf(),prec,TO_N_INF);
               upp_bound = add(num_bf(),error_bf(),prec,TO_P_INF);
               long dec_prec = (long) MATH_SCOPE floor(MATH_SCOPE log10(2.0)*prec);
               out << "B([" << to_string(low_bound, dec_prec);
               out << "," << to_string(upp_bound, dec_prec);
               out << "])";
               break; }
        case ADDITION:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION:
               if(CON == ADDITION) out << "+(";
               if(CON == SUBTRACTION) out << "-(";
               if(CON == MULTIPLICATION) out << "*(";
               if(CON == DIVISION) out << "/(";
               OP1->output_as_dag(out,depth+2, true,false);
               out << ",\n";
               OP2->output_as_dag(out,depth+2, false,false);
               out << ")";
               break;
        case NEGATION:
               out << "N(";
               OP1->output_as_dag(out,depth+2,true,false);
               out << ")";
               break;
        case SQUAREROOT:
               out << "sqrt(";
               OP1->output_as_dag(out,depth+5,true,false);
               out << ")";
               break;
        case ROOT:
               out << "root[" << d << "](";
               OP1->output_as_dag(out,depth+8+(int)log10((double)d),true,false);
               out << ")";
               break;

        case RATIONAL:
               out << "R(" << rat() << ")";
               break;

        case DIAMOND:{
               out << "diamond[" << left << "," << right  << "]\n       ";
               int i;
               for(i=0; i<depth; i++) out << " ";
               out << "(";
               (RPOL[0].PTR)->output_as_dag(out,depth+8,true,false);
               for(i=1; i<= RPOL.degree(); i++) 
               {
                  out << ",\n";
                  (RPOL[i].PTR)->output_as_dag(out,depth+8,true,false);
               }
               out << ")";
               break;}

     }
     if(status == VISITED) out << "<V>";
     if(nl) out << endl;
     status = VISITED;

  }

  real::io_mode real::get_output_mode() { return output_mode; }

  real::io_mode real::set_output_mode(io_mode m)
  { io_mode old = output_mode; output_mode = m; return old;}

  void real::output_as_dag(ostream& out, const real& x)
  {
     if(x.is_double) 
     {
        out << x.value << endl;
     }
     else
     {
       x.PTR->clear_visited_marks();
       x.PTR->output_as_dag(out, 0, true, true);
       x.PTR->clear_visited_marks();
     }
  }

  void real::output_as_interval(ostream& out, const real& x)
  {
    if (x.get_double_error() == 0) {
      out << "[" << x.to_double() << "]";
      return;
    }
    x.PTR->init_app_bf();
    if (x.get_bigfloat_error() == 0)
    {
      bigfloat b = x.to_bigfloat();
      bigfloat I = to_integer(b);
      if (b == I) {
        out << "[" << I << "]";
        return;
      }
    }

    bigfloat low_bound, upp_bound;
    long prec = 0;
    if (!isZero(x.PTR->num_bf()) && !isZero(x.PTR->error_bf()))
      prec = (ilog2(x.PTR->num_bf())-ilog2(x.PTR->error_bf())).to_long();
    if (prec < DOUBLE_PREC) 
      prec = DOUBLE_PREC;
    low_bound = sub(x.to_bigfloat(),x.PTR->error_bf(),prec,TO_N_INF);
    upp_bound = add(x.to_bigfloat(),x.PTR->error_bf(),prec,TO_P_INF);
    long dec_prec = (long) MATH_SCOPE floor(MATH_SCOPE log10(2.0)*prec);
    out << "[" << to_string(low_bound, dec_prec);
    out << "," << to_string(upp_bound, dec_prec);
    out << "]";
    return;
  }

/// added for debugging

  inline void output_integer_expression(ostream& out, const integer& i)
  {
          if (i.is_long()) out << i.to_long();
          else out << "integer(\"" << i << "\")";
  }

  int real_rep::output_as_expression(ostream& out, int& idx_counter, 
                                         d_array<real_rep_ref, int>* sub_exp)
  {
     if (sub_exp != nil && sub_exp->defined(this)) return (*sub_exp)[this];
     int var_idx = idx_counter++;
     if (sub_exp != nil) (*sub_exp)[this] = var_idx;

     switch(CON)
     {
        case DOUBLE:
        case BIGFLOAT: {
               init_app_bf();
               bigfloat b = num_bf();
               integer sig = b.get_significant();
               integer exp = b.get_exponent();
               if (exp == 0) {
                 out << string("real x_%d = ", var_idx);
                 output_integer_expression(out, sig);
                 out << ";" << endl;
               }
               else {
                 out << string("integer x_%d_sig = ", var_idx); 
                 output_integer_expression(out, sig);
                 out << ";" << endl;
                 out << string("integer x_%d_exp = ", var_idx);
                 output_integer_expression(out, exp);
                 out << ";" << endl;
                 out << string("real x_%d = bigfloat(x_%d_sig, x_%d_exp);", 
                               var_idx, var_idx, var_idx) << endl;
               }
               break; }
        case RATIONAL: {
               integer num = rat().numerator();
               integer den = rat().denominator();
               if (den == 1) {
                 out << string("real x_%d = ", var_idx);
                 output_integer_expression(out, num);
                 out << ";" << endl;
               }
               else {
                 out << string("integer x_%d_num = ", var_idx); 
                 output_integer_expression(out, num);
                 out << ";" << endl;
                 out << string("integer x_%d_den = ", var_idx);
                 output_integer_expression(out, den);
                 out << ";" << endl;
                 out << string("real x_%d = rational(x_%d_num, x_%d_den);", 
                               var_idx, var_idx, var_idx) << endl;
               }
               break; }
        case ADDITION:
        case SUBTRACTION:
        case MULTIPLICATION:
        case DIVISION: {
               int idx_op1 = OP1->output_as_expression(out, idx_counter, sub_exp);
               int idx_op2 = OP2->output_as_expression(out, idx_counter, sub_exp);
               out << string("real x_%d = ", var_idx);
               out << string("x_%d", idx_op1);
               if(CON == ADDITION) out << " + ";
               if(CON == SUBTRACTION) out << " - ";
               if(CON == MULTIPLICATION) out << " * ";
               if(CON == DIVISION) out << " / ";
               out << string("x_%d", idx_op2);
               out << ";" << endl;
               break; }
        case NEGATION: {
               int idx_op1 = OP1->output_as_expression(out, idx_counter, sub_exp);
               out << string("real x_%d = - x_%d;", var_idx, idx_op1) << endl;
               break; }
        case SQUAREROOT: {
               int idx_op1 = OP1->output_as_expression(out, idx_counter, sub_exp);
               out << string("real x_%d = sqrt(x_%d);", var_idx, idx_op1) << endl;
               break; }
        case ROOT: {
               int idx_op1 = OP1->output_as_expression(out, idx_counter, sub_exp);
               out << string("real x_%d = root(x_%d, %d);", var_idx, idx_op1, d) << endl;
               break; }
        case DIAMOND:{
               LEDA_VECTOR<int> indices_op(RPOL.degree()+1);
               int i;
               for(i = 0; i <= RPOL.degree(); ++i) 
               {
                  indices_op[i] = (RPOL[i].PTR)->output_as_expression(out, idx_counter, sub_exp);
               }
               out << string("real x_%d; // DIAMOND not implemented ", var_idx) << endl;
               break;}

     }
     status = VISITED;
     return var_idx;
  }
     
  void real::output_as_expression(ostream& out, const real& x, bool use_sub_expr)
  {
     if(x.is_double) 
     {
        out << "real x_0 = " << x.value << ";" << endl;
     }
     else
     {
       x.PTR->clear_visited_marks();
       int idx_counter = 0;
       d_array<real_rep_ref, int> sub_exp(-1);
       x.PTR->output_as_expression(out, idx_counter, use_sub_expr ? &sub_exp : 0);
       x.PTR->clear_visited_marks();
     }
  }

  ostream& operator<<(ostream& out, const real& x)
  {   
    switch (real::output_mode) {
    case real::IO_dag:      real::output_as_dag(out,x); break;
    case real::IO_interval: real::output_as_interval(out,x); break;
    default:                out << x.to_close_double(); break;
    }
    return out;
  }

  istream& operator>>(istream& in, real& x)
  { 
    char c;
    do c = (char)in.get(); while (in && isspace(c));
    in.putback(c);
    if (c != '[')
    {
       x = real::input_as_dag(in); // also handles a simple double ...
       return in;
    }
    HANDLE_ERROR(1, "operator>>(real): cannot read interval format.");
    return in;
  } 

  inline static void swallow(istream &is, char d) {
    char c;
    do c = (char)is.get(); while (is && isspace(c));
    assert (c == d);
  }


  real real::input_as_dag(istream& in)
  {
    char t;
    do t=(char)in.get(); while (in && isspace(t));
    in.putback(t);
    real x;
    switch(t)
    {
      case 'D': {
                swallow(in, 'D');
                swallow(in, '(');
                double x_num;
                in >> x_num;
                x = real(x_num);  
                swallow(in, ')');
                break;
                }
      case 'B': {
                swallow(in, 'B');
                swallow(in, '(');
                bigfloat x_num = input_bf(in);
                x = real(x_num);
                swallow(in, ')');
                break;
                }
      case '+':  
      case '*':  
      case '/': {
                swallow(in, t);
                swallow(in, '(');
                real a = input_as_dag(in);
                swallow(in, ',');
                real b = input_as_dag(in);
                swallow(in, ')');
                if(t=='+') x = a+b;
                else if(t=='-') x = a-b;
                else if(t=='*') x = a*b;
                else x = a/b;
                break;
                }
      case '-': {
                swallow(in, '-');
                // input can be a double number
                do t=(char)in.get(); while (isspace(t));
                if(t!='(')
                {
                   in.putback(t);
                   double x_num;  
                   in >> x_num; 
                   x = real(-x_num);  
                   return x;
                }
                real a = input_as_dag(in);
                swallow(in, ',');
                real b = input_as_dag(in);
                swallow(in, ')');
                x = a-b;
                break;
                }
      case 'N': {
                swallow(in, t);
                swallow(in, '(');
                real a = input_as_dag(in);
                swallow(in, ')');
                x = -a;
                break;
                }
      case 's': {
                swallow(in,'s');
                swallow(in,'q');
                swallow(in,'r');
                swallow(in,'t');
                swallow(in,'(');
                real a = input_as_dag(in);
                swallow(in, ')');
                x = sqrt(a);
                break;
                }
      case 'r': {
                swallow(in,'r');
                swallow(in,'o');
                swallow(in,'o');
                swallow(in,'t');
                swallow(in,'[');
                int d;
                in >> d;
                swallow(in,']');
                swallow(in,'(');
                real a = input_as_dag(in);
                swallow(in, ')');
                x = root(a,d);
                break;
                }
      case 'R': {
                swallow(in, 'R');
                swallow(in, '(');
                rational x_num;
                in >> x_num;
                x = real(x_num);
                swallow(in, ')');
                break;
                }


      case 'd': {
        swallow(in,'d');
        swallow(in,'i');
        swallow(in,'a');
        swallow(in,'m');
        swallow(in,'o');
        swallow(in,'n');
        swallow(in,'d');
                swallow(in,'[');
                int j=-1;
                rational left, right;
        do t=(char)in.get(); while (isspace(t));
                if(t=='[') // interval input
                {
                   in >> left;
                   swallow(in,',');
                   in >> right;
                   swallow(in,']');
                }
                else
                {
                   in.putback(t);
                   in >> j;
                }
                swallow(in,']');
        swallow(in,'(');
        LEDA_VECTOR<real> pol;
                do
                {
                   pol.push_back(input_as_dag(in));
           do t=(char)in.get(); while (isspace(t));
                } while (t==',');
        in.putback(t);
                swallow(in, ')');
                if(j==-1) x = diamond(left, right, pol);
                else x = diamond(j,pol);
                break;
      }

      default: {
               double x_num;  
               in >> x_num; 
               x = real(x_num);  
               return x;
               }
    }
    do t=(char)in.get(); while (isspace(t)&& t!='\n');
    if(t=='<') // don't read "<V>" - this makes it compatible with output_as_dag()
    {
        swallow(in,'V');
        swallow(in,'>');
    }
    else in.putback(t);
    return x; 
  } 


  double to_double(const real& x) {
    return x.to_double();
  }

  double real::to_double() const {  
    if (is_double) return value;
    if (PTR->APP_BF_PTR) PTR->adjust_interval();
    return PTR->I.to_double();  
  }

  double real::to_double(double& error) const {  
    if (is_double) { error = 0; return value; }
    if (PTR->APP_BF_PTR) PTR->adjust_interval();
    error = PTR->I.get_double_error();
    return PTR->I.to_double();  
  }

  double real::get_double_error() const {  
    if (is_double) return 0;
    if (PTR->APP_BF_PTR) PTR->adjust_interval();
    return PTR->I.get_double_error();
  }

  bigfloat to_bigfloat(const real& x) {
    return x.to_bigfloat();
  }

  bigfloat real::to_bigfloat() const {  
    if (is_double) return value;
    PTR->init_app_bf(); 
    return PTR->num_bf(); 
  }

  bigfloat real::get_bigfloat_error() const {  
    if (is_double)
      return 0;
    else {  
      PTR->init_app_bf(); 
      return PTR->error_bf(); 
    }
  }

  bigfloat real::get_lower_bound() const {
    return to_bigfloat() - get_bigfloat_error();
  }

  bigfloat real::get_upper_bound() const {
    return to_bigfloat() + get_bigfloat_error();
  }

  rational real::low() const {
     if(is_double && (*this) == 0) return rational(0,1);
     return PACKAGE_SCOPE to_rational(get_lower_bound());
  }

  rational real::high() const {
     if(is_double && (*this) == 0) return rational(0,1);
     return PACKAGE_SCOPE to_rational(get_upper_bound());
  }

  double real::get_double_lower_bound() const {  
    if (is_double) return value;
    if (PTR->APP_BF_PTR) PTR->adjust_interval();
    return PTR->I.lower_bound();  
  }

  double real::get_double_upper_bound() const {  
    if (is_double) return value;
    if (PTR->APP_BF_PTR) PTR->adjust_interval();
    return PTR->I.upper_bound();  
  }

  bool real::possible_zero() const {
    if(is_double) return (value == 0);
    else return (PTR->I).contains(0);
  }

  


  real operator+(const real& x,const real& y)
  { 
    
      if (x.is_double && y.is_double) {
        real_interval I = real_interval(x.value)+real_interval(y.value);
        if (I.is_a_point())
          return x.value+y.value;
      }



    if(x.is_double && x == 0) return y;
    if(y.is_double && y == 0) return x;

      if (!x.PTR) 
      {
        #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
            if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
              ((real&) x).PTR = new real_rep(x.value);
            else ((real&) x).PTR->ref_plus();
        #else
            ((real&) x).PTR = new real_rep(x.value);
        #endif
      }
      if (!y.PTR) 
      {
        #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
            if(!(((real&) y).PTR = CURRENT_REAL_REPS->find(y.value))) 
              ((real&) y).PTR = new real_rep(y.value);
            else ((real&) y).PTR->ref_plus();
        #else
            ((real&) y).PTR = new real_rep(y.value);
        #endif
      }


    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        real_rep* z_rep = CURRENT_REAL_REPS->find(ADDITION,(x.PTR)->ref(), (y.PTR)->ref());
    #else
        real_rep* z_rep = NULL;
    #endif

    if(z_rep != NULL)
    {
       z_rep->ref_plus();
       return *z_rep;
    }

        z_rep = new real_rep();
        z_rep->OP1 = x.PTR;  z_rep->OP2 = y.PTR;
        (z_rep->OP1)->ref_plus();  (z_rep->OP2)->ref_plus();
        z_rep->is_general_ = (z_rep->OP1)->is_general_ || (z_rep->OP2)->is_general_;
        z_rep->is_rational_ = (z_rep->OP1)->is_rational_ && (z_rep->OP2)->is_rational_;
 

    z_rep->CON = ADDITION;
    z_rep->I = x.PTR->I+y.PTR->I;
#ifndef BFMSS_ONLY
    z_rep->is_integral_pol = x.PTR->is_integral_pol && y.PTR->is_integral_pol;
    z_rep->degree_measure_possible = x.PTR->degree_measure_possible && y.PTR->degree_measure_possible;
#endif

    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        CURRENT_REAL_REPS->insert(z_rep->CON, z_rep, (x.PTR)->ref(), (y.PTR)->ref());
    #endif


    return *z_rep;
  }

   real operator-(const real& x,const real& y)
   { 
     
       if (x.is_double && y.is_double) {
         real_interval I = real_interval(x.value)-real_interval(y.value);
         if (I.is_a_point())
           return x.value-y.value;
       }


    if(x.is_double && x == 0) return (-y);
    if(y.is_double && y == 0) return x;

       if (!x.PTR) 
       {
         #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
               ((real&) x).PTR = new real_rep(x.value);
             else ((real&) x).PTR->ref_plus();
         #else
             ((real&) x).PTR = new real_rep(x.value);
         #endif
       }
       if (!y.PTR) 
       {
         #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             if(!(((real&) y).PTR = CURRENT_REAL_REPS->find(y.value))) 
               ((real&) y).PTR = new real_rep(y.value);
             else ((real&) y).PTR->ref_plus();
         #else
             ((real&) y).PTR = new real_rep(y.value);
         #endif
       }


     if(x.PTR == y.PTR) return real(0);

     #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
          real_rep* z_rep = CURRENT_REAL_REPS->find(SUBTRACTION,(x.PTR)->ref(), (y.PTR)->ref());
     #else
          real_rep* z_rep = NULL;
     #endif

     if(z_rep != NULL)
     {
        z_rep->ref_plus();
        return *z_rep;
     }

         z_rep = new real_rep();
         z_rep->OP1 = x.PTR;  z_rep->OP2 = y.PTR;
         (z_rep->OP1)->ref_plus();  (z_rep->OP2)->ref_plus();
         z_rep->is_general_ = (z_rep->OP1)->is_general_ || (z_rep->OP2)->is_general_;
         z_rep->is_rational_ = (z_rep->OP1)->is_rational_ && (z_rep->OP2)->is_rational_;


     z_rep->CON = SUBTRACTION;
     z_rep->I = x.PTR->I-y.PTR->I;
#ifndef BFMSS_ONLY
     z_rep->is_integral_pol = x.PTR->is_integral_pol && y.PTR->is_integral_pol;
     z_rep->degree_measure_possible = x.PTR->degree_measure_possible && y.PTR->degree_measure_possible;
#endif

     #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
         CURRENT_REAL_REPS->insert(z_rep->CON, z_rep, (x.PTR)->ref(), (y.PTR)->ref());
     #endif

     return *z_rep;
   }

   real operator*(const real& x,const real& y)
   {
     
       if (x.is_double && y.is_double) {
         real_interval I = real_interval(x.value)*real_interval(y.value);
         if (I.is_a_point())
           return x.value*y.value;
       }



     if(x.is_double && x == 0) return real(0);
     if(y.is_double && y == 0) return real(0);
     if(x.is_double && x == 1) return y;
     if(y.is_double && y == 1) return x;

       if (!x.PTR) 
       {
         #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
               ((real&) x).PTR = new real_rep(x.value);
             else ((real&) x).PTR->ref_plus();
         #else
             ((real&) x).PTR = new real_rep(x.value);
         #endif
       }
       if (!y.PTR) 
       {
         #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             if(!(((real&) y).PTR = CURRENT_REAL_REPS->find(y.value))) 
               ((real&) y).PTR = new real_rep(y.value);
             else ((real&) y).PTR->ref_plus();
         #else
             ((real&) y).PTR = new real_rep(y.value);
         #endif
       }


     #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
          real_rep* z_rep = CURRENT_REAL_REPS->find(MULTIPLICATION,(x.PTR)->ref(), (y.PTR)->ref());
     #else
          real_rep* z_rep = NULL;
     #endif

     if(z_rep != NULL)
     {
        z_rep->ref_plus();
        return *z_rep;
     }

         z_rep = new real_rep();
         z_rep->OP1 = x.PTR;  z_rep->OP2 = y.PTR;
         (z_rep->OP1)->ref_plus();  (z_rep->OP2)->ref_plus();
         z_rep->is_general_ = (z_rep->OP1)->is_general_ || (z_rep->OP2)->is_general_;
         z_rep->is_rational_ = (z_rep->OP1)->is_rational_ && (z_rep->OP2)->is_rational_;
 

     z_rep->CON = MULTIPLICATION;
     z_rep->I = x.PTR->I*y.PTR->I;
#ifndef BFMSS_ONLY
     z_rep->is_integral_pol = x.PTR->is_integral_pol && y.PTR->is_integral_pol;
     z_rep->degree_measure_possible = x.PTR->degree_measure_possible && y.PTR->degree_measure_possible;
#endif

     #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
         CURRENT_REAL_REPS->insert(z_rep->CON, z_rep, (x.PTR)->ref(), (y.PTR)->ref());
     #endif

     return *z_rep;
   }

 real operator/(const real& x,const real& y)
   { 
     
        if (y.is_double) 
        { 
          if (y.value == 0)
            HANDLE_ERROR(1,"real::operator/:Division by zero");
        }
        else {
          if (y.PTR->I.is_a_point() && y.PTR->I.contains(0))
             HANDLE_ERROR(1,"real::operator/:Division by zero");
        }
        
     
       if (x.is_double && y.is_double) {
         real_interval I = real_interval(x.value)/real_interval(y.value);
         if (I.is_a_point())
           return x.value/y.value;
       }

  
     if(x.is_double && x == 0) return real(0);
     if(y.is_double && y == 1) return x;

       if (!x.PTR) 
       {
         #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
               ((real&) x).PTR = new real_rep(x.value);
             else ((real&) x).PTR->ref_plus();
         #else
             ((real&) x).PTR = new real_rep(x.value);
         #endif
       }
       if (!y.PTR) 
       {
         #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             if(!(((real&) y).PTR = CURRENT_REAL_REPS->find(y.value))) 
               ((real&) y).PTR = new real_rep(y.value);
             else ((real&) y).PTR->ref_plus();
         #else
             ((real&) y).PTR = new real_rep(y.value);
         #endif
       }


     if(x.PTR == y.PTR) return real(1);

     #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
          real_rep* z_rep = CURRENT_REAL_REPS->find(DIVISION,(x.PTR)->ref(),(y.PTR)->ref());
     #else
          real_rep* z_rep = NULL;
     #endif

     if(z_rep != NULL)
     {
        z_rep->ref_plus();
        return *z_rep;
     }

         z_rep = new real_rep();
         z_rep->OP1 = x.PTR;  z_rep->OP2 = y.PTR;
         (z_rep->OP1)->ref_plus();  (z_rep->OP2)->ref_plus();
         z_rep->is_general_ = (z_rep->OP1)->is_general_ || (z_rep->OP2)->is_general_;
         z_rep->is_rational_ = (z_rep->OP1)->is_rational_ && (z_rep->OP2)->is_rational_;


     z_rep->CON = DIVISION;
     z_rep->I = x.PTR->I/y.PTR->I;
#ifndef BFMSS_ONLY
     z_rep->is_integral_pol = false;
     z_rep->degree_measure_possible = x.PTR->degree_measure_possible && y.PTR->degree_measure_possible;
#endif

     #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
         CURRENT_REAL_REPS->insert(z_rep->CON, z_rep, (x.PTR)->ref(), (y.PTR)->ref());
     #endif

     return *z_rep;
   }

  real sqrt(const real& x)
  {
    if ( (x.is_double) && (x.value < 0) )
      HANDLE_ERROR(1,"real::operator sqrt:negative argument");
    
    if (x.is_double) {
        real_interval I = sqrt(real_interval(x.value));
        if (I.is_a_point())
          return MATH_SCOPE sqrt(x.value);
     }

    if (!x.PTR) 
    {
        #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
            if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
              ((real&) x).PTR = new real_rep(x.value);
            else ((real&) x).PTR->ref_plus();
        #else
            ((real&) x).PTR = new real_rep(x.value);
        #endif
     }

    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
         real_rep* z_rep = CURRENT_REAL_REPS->find(SQUAREROOT,(x.PTR)->ref());
    #else
         real_rep* z_rep = NULL;
    #endif

    if(z_rep != NULL)
    {
       z_rep->ref_plus();
       return *z_rep;
    }

    z_rep = new real_rep();
    z_rep->OP1 = x.PTR; (z_rep->OP1)->ref_plus();
    z_rep->OP2 = NULL;
    z_rep->is_general_ = (z_rep->OP1)->is_general_;
      
    z_rep->CON = SQUAREROOT;
    z_rep->I = sqrt(x.PTR->I);  
    z_rep->is_rational_ = false; 
#ifndef BFMSS_ONLY
    z_rep->is_integral_pol = false;
    z_rep->degree_measure_possible = x.PTR->degree_measure_possible;
#endif
    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        CURRENT_REAL_REPS->insert(z_rep->CON, z_rep, (x.PTR)->ref());
    #endif

    return *z_rep;
  }


  real root(const real& x, int d)
  {
    if ( (x.is_double) && (x.value < 0) )
      HANDLE_ERROR(1,"real::operator root:negative argument");

    if (!x.PTR) 
      {
        #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
            if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
              ((real&) x).PTR = new real_rep(x.value);
            else ((real&) x).PTR->ref_plus();
        #else
            ((real&) x).PTR = new real_rep(x.value);
        #endif
      }

    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
         real_rep* z_rep = CURRENT_REAL_REPS->find((x.PTR)->ref(),d);
    #else
         real_rep* z_rep = NULL;
    #endif

    if(z_rep != NULL)
    {
       z_rep->ref_plus();
       return *z_rep;
    }


    z_rep = new real_rep();
    z_rep->OP1 = x.PTR; (z_rep->OP1)->ref_plus();
    z_rep->OP2 = NULL;
    z_rep->is_general_ = (z_rep->OP1)->is_general_;
     
    z_rep->CON = ROOT;
    z_rep->d = d;
    z_rep->I = root(x.PTR->I,d); 
    z_rep->is_rational_ = false; 
#ifndef BFMSS_ONLY
    z_rep->is_integral_pol = false;
    z_rep->degree_measure_possible = x.PTR->degree_measure_possible;
#endif
    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        CURRENT_REAL_REPS->insert(z_rep, (x.PTR)->ref(), d);
    #endif

    return *z_rep;
  }


  real operator+(const real& x)
  {
     return x;
  }

  real operator-(const real& x)
  { 
    if (x.is_double) return -x.value;
    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
         real_rep* z_rep = CURRENT_REAL_REPS->find(NEGATION,(x.PTR)->ref());
    #else 
         real_rep* z_rep = NULL;
    #endif

    if(z_rep != NULL)
    {
       z_rep->ref_plus();
       return *z_rep;
    }

    z_rep = new real_rep();
    z_rep->OP1 = x.PTR; (z_rep->OP1)->ref_plus();
    z_rep->OP2 = NULL;
    z_rep->is_general_ = (z_rep->OP1)->is_general_;
     
    z_rep->CON = NEGATION;
    z_rep->I = -x.PTR->I;
    z_rep->is_rational_ = (x.PTR)->is_rational_;
#ifndef BFMSS_ONLY
    z_rep->is_integral_pol = x.PTR->is_integral_pol;
    z_rep->degree_measure_possible = x.PTR->degree_measure_possible;
#endif
    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        CURRENT_REAL_REPS->insert(z_rep->CON, z_rep, (x.PTR)->ref());
    #endif

    return *z_rep;
  }


real diamond(int j, const Polynomial& P, algorithm_type algorithm, bool is_squarefree)
{
  j -= 1;  // Isolating counts the roots from 0
  
  #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
       real_rep* z_rep = CURRENT_REAL_REPS->find(P,j);
       Polynomial original_POL;
       if(z_rep == NULL) 
       { // during search the element gets inserted somehow
          CURRENT_REAL_REPS->delete_element(P,real_interval(0),j);
          original_POL = P;
       }
  #else 
       real_rep* z_rep = NULL;
  #endif
  if(z_rep != NULL)
  {
     z_rep->ref_plus();
     return *z_rep;
  }

  if (P.degree()==1) return (-P[0])/P[1];
  if (P.degree()==2)
  {
    real disc = real(P[1]*P[1] - 4 * P[2]*P[0]);
    real z;
    if(j==0) z=real((-P[1]-sqrt(disc))/(2*P[2]));
    else z = real((-P[1]+sqrt(disc))/(2*P[2])); // j=1

    if(!z.PTR)
      {
         #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
              if(!(z.PTR = CURRENT_REAL_REPS->find(z.value))) z.PTR = new real_rep(z.value);
              else z.PTR->ref_plus();
         #else
              z.PTR = new real_rep(z.value);
         #endif

      }
      z.set_polynomial(P);

    return z;
  }

  
       z_rep = new real_rep();
       z_rep->OP1 = NULL;
       z_rep->OP2 = NULL;

        
       #ifdef TRY_USP_WITH_INTERVAL 

            bool found = false;
            if(is_squarefree && algorithm == USPENSKY)
            {
                    LEDA_VECTOR<real_interval> AI(P.degree()+1);
                    int nI = P.degree();
                 if(!(P[nI]).PTR) AI[nI] = (P[nI]).value;
                 else AI[nI] = ((P[nI]).PTR)->I;
                 for(int i=0; i<nI; i++)
                 {
                   if(!(P[i]).PTR) AI[i] = (P[i]).value;
                   else AI[i] = ((P[i]).PTR)->I;
                   AI[i] = AI[i] / AI[nI];
                 }
                 AI[nI] = real_interval(1);
                    Poly_interval PI(AI);

               isolating<real_interval> IsoI(PI,j,USPENSKY,true);
               if(IsoI.can_be_isolated())
               {
                  assert(j<=IsoI.number_of_real_roots());
                  z_rep->left = IsoI.left_boundary(j);
                  z_rep->right = IsoI.right_boundary(j);
                  int s = has_root(P,z_rep->left, z_rep->right);
                  if(s < 0) found = true;
                  else if(s==0) // P has an exact root at an endpoint
                  {
                     if((P.evaluate(z_rep->right)) == 0)
                     {
                        delete z_rep;
                        return IsoI.right_boundary(j);
                     }
                     delete z_rep;
                     return IsoI.left_boundary(j);
                  }
               }
            }
            if(!found)
            {
       #endif //TRY_USP_WITH_INTERVAL

               isolating<real> Iso(P,j,algorithm);
               assert(j<=Iso.number_of_real_roots());
          
               
               if(Iso.is_exact_root(j))
               {
                  delete z_rep;
                  return Iso.left_boundary(j);
               }


               z_rep->left = Iso.left_boundary(j);
               z_rep->right = Iso.right_boundary(j);

       #ifdef TRY_USP_WITH_INTERVAL 
            }
       #endif


            z_rep->first = true;
            z_rep->pol_squarefree = is_squarefree;
            z_rep->CON = DIAMOND;
       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
            z_rep->OPOL = P;
       #endif // DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                 for(int c=0; c<=P.degree(); c++)
                 {
                    if (!(P[c]).PTR) 
                    {
                      #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                if(!(((real&) (P[c])).PTR = CURRENT_REAL_REPS->find((P[c]).value))) 
                                  ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                                else ((real&) (P[c])).PTR->ref_plus();
                      #else
                              ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                      #endif

                    }
                    (P[c].PTR)->ref_plus();
                 }

            z_rep->RPOL = P;
       #ifndef BFMSS_ONLY
            z_rep->is_integral_pol = true;
            for(int i=0; i<=P.degree(); i++)
            {
               if(!(P[i].PTR)->is_integral_pol) 
               {
                 z_rep->is_integral_pol = false;
                 break;
               }
            }
            z_rep->degree_measure_possible = z_rep->is_integral_pol;
       #endif
            z_rep->I = PACKAGE_SCOPE to_interval(z_rep->left, z_rep->right);
            z_rep->is_general_ = true;
            z_rep->is_rational_ = false;

       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
            z_rep->j_ = j;
            rational l = z_rep->left;
            rational u = z_rep->right;
                 std::vector<real_rep*> Vrep = CURRENT_REAL_REPS->find(P,PACKAGE_SCOPE to_interval(l,u));
                 real_rep* h_rep = NULL;
                 int root_number = -1;
                 bool easy_test = (Vrep[2]!=NULL);
                 for(int i=0; i<3; i++)
                 {
                   h_rep = Vrep[i]; 
                   if(h_rep == NULL) break; 

                   Polynomial Ptest = h_rep->OPOL;

                   double du = (h_rep->I).upper_bound();
                   double dl = (h_rep->I).lower_bound();

                   // It might happen that I does not intersect (l,u). In this case
                   // update the interval in CURRENT_DIAMONDS.
                   if(du < l || dl > u)
                   {
                      CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                      CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                      continue;
                   }
                   if(dl < l) dl = l.to_double();
                   if(du > u) du = u.to_double();

                   if((easy_test && i!=2) || ( !easy_test &&
                                             sign(Ptest.evaluate(du)) == sign(Ptest.evaluate(dl))
                                             && sign(Ptest.evaluate(du))!=0) )
                   {
                      // h_rep represents not the root in (l,u)
                      // make the interval of h_rep smaller 
                      // As real_interval has double bounds (instead of rational bounds)
                      // the interval has to be smaller, such that the corresponding bound
                      // is far away from l / u.

                      CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                      if((h_rep->I).lower_bound() < l) 
                      {
                         int s = sign(Ptest.evaluate((h_rep->I).lower_bound()));
                         rational m = (h_rep->left +l)/rational(2,1);
                         while(s == sign(Ptest.evaluate(m))) m = (m+l)/rational(2,1);
                         h_rep->right = m;
                      }
                      else 
                      {
                         int s = sign(Ptest.evaluate((h_rep->I).upper_bound()));
                         rational m = (u+h_rep->right)/rational(2,1);
                         while(s == sign(Ptest.evaluate(m))) m = (u+m)/rational(2,1);
                         h_rep->left = m;
                      }
                      h_rep->I = PACKAGE_SCOPE to_interval(h_rep->left, h_rep->right);
                                   CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                                   if(h_rep->APP_BF_PTR != NULL) h_rep->APP_BF_PTR = NULL;
                                   h_rep->init_app_bf();
                   }
                   else // h_rep represents the root in (l,u)
                     root_number = i;
                 }
                 if(root_number != (-1)) h_rep = Vrep[root_number];
             

            // update the information (the new interval might be smaller, number 
            // of the root)
            if(h_rep != NULL)
            {
                     CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                     if((h_rep->I).lower_bound() < l) h_rep->left = l;
                     if((h_rep->I).upper_bound() > u) h_rep->right = u;
                     h_rep->I = PACKAGE_SCOPE to_interval(h_rep->left, h_rep->right);
                h_rep->j_ = z_rep->j_;
                             CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                             if(h_rep->APP_BF_PTR != NULL) h_rep->APP_BF_PTR = NULL;
                             h_rep->init_app_bf();
                h_rep->ref_plus();
                return *h_rep; 
            }
            else CURRENT_REAL_REPS->insert(z_rep,P,z_rep->I,j);
       #endif // DONOT...

       return *z_rep;
}

real diamond_short(rational l, rational u, const Polynomial& P, algorithm_type algorithm, bool is_squarefree)
{
   
   #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        int j = -1; // the number of the root is not known
        Polynomial original_POL = P;
             std::vector<real_rep*> Vrep = CURRENT_REAL_REPS->find(P,PACKAGE_SCOPE to_interval(l,u));
             real_rep* h_rep = NULL;
             int root_number = -1;
             bool easy_test = (Vrep[2]!=NULL);
             for(int i=0; i<3; i++)
             {
               h_rep = Vrep[i]; 
               if(h_rep == NULL) break; 

               Polynomial Ptest = h_rep->OPOL;

               double du = (h_rep->I).upper_bound();
               double dl = (h_rep->I).lower_bound();

               // It might happen that I does not intersect (l,u). In this case
               // update the interval in CURRENT_DIAMONDS.
               if(du < l || dl > u)
               {
                  CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                  CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                  continue;
               }
               if(dl < l) dl = l.to_double();
               if(du > u) du = u.to_double();

               if((easy_test && i!=2) || ( !easy_test &&
                                         sign(Ptest.evaluate(du)) == sign(Ptest.evaluate(dl))
                                         && sign(Ptest.evaluate(du))!=0) )
               {
                  // h_rep represents not the root in (l,u)
                  // make the interval of h_rep smaller 
                  // As real_interval has double bounds (instead of rational bounds)
                  // the interval has to be smaller, such that the corresponding bound
                  // is far away from l / u.

                  CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                  if((h_rep->I).lower_bound() < l) 
                  {
                     int s = sign(Ptest.evaluate((h_rep->I).lower_bound()));
                     rational m = (h_rep->left +l)/rational(2,1);
                     while(s == sign(Ptest.evaluate(m))) m = (m+l)/rational(2,1);
                     h_rep->right = m;
                  }
                  else 
                  {
                     int s = sign(Ptest.evaluate((h_rep->I).upper_bound()));
                     rational m = (u+h_rep->right)/rational(2,1);
                     while(s == sign(Ptest.evaluate(m))) m = (u+m)/rational(2,1);
                     h_rep->left = m;
                  }
                  h_rep->I = PACKAGE_SCOPE to_interval(h_rep->left, h_rep->right);
                               CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                               if(h_rep->APP_BF_PTR != NULL) h_rep->APP_BF_PTR = NULL;
                               h_rep->init_app_bf();
               }
               else // h_rep represents the root in (l,u)
                 root_number = i;
             }
             if(root_number != (-1)) h_rep = Vrep[root_number];
         

        // update the information (the new interval might be smaller)
        if(h_rep != NULL)
        {
                CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                if((h_rep->I).lower_bound() < l) h_rep->left = l;
                if((h_rep->I).upper_bound() > u) h_rep->right = u;
                h_rep->I = PACKAGE_SCOPE to_interval(h_rep->left, h_rep->right);
                        CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                        if(h_rep->APP_BF_PTR != NULL) h_rep->APP_BF_PTR = NULL;
                        h_rep->init_app_bf();
        }
        real_rep* z_rep = h_rep;
   #else
        real_rep* z_rep = NULL;
   #endif // DONOT_...

   if(z_rep != NULL)
   {
      z_rep->ref_plus();
      return *z_rep;
   }

   if (P.degree()==1) return (-P[0])/P[1];
   if (P.degree()==2)
   {
     real disc = real(P[1]*P[1] - 4 * P[2]*P[0]);
     real x0 = real((-P[1]-sqrt(disc))/(2*P[2]));
     real z;
     if(l <= x0 && x0 <= u) z = x0;
     else z = real((-P[1]+sqrt(disc))/(2*P[2])); // j=1
       if(!z.PTR)
       {
          #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
               if(!(z.PTR = CURRENT_REAL_REPS->find(z.value))) z.PTR = new real_rep(z.value);
               else z.PTR->ref_plus();
          #else
               z.PTR = new real_rep(z.value);
          #endif

       }
       z.set_polynomial(P);

     return z;
   }


        z_rep = new real_rep();
        z_rep->OP1 = NULL;
        z_rep->OP2 = NULL;

        z_rep->left = l;
        z_rep->right = u;

             z_rep->first = true;
             z_rep->pol_squarefree = is_squarefree;
             z_rep->CON = DIAMOND;
        #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             z_rep->OPOL = P;
        #endif // DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                  for(int c=0; c<=P.degree(); c++)
                  {
                     if (!(P[c]).PTR) 
                     {
                       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                 if(!(((real&) (P[c])).PTR = CURRENT_REAL_REPS->find((P[c]).value))) 
                                   ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                                 else ((real&) (P[c])).PTR->ref_plus();
                       #else
                               ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                       #endif

                     }
                     (P[c].PTR)->ref_plus();
                  }

             z_rep->RPOL = P;
        #ifndef BFMSS_ONLY
             z_rep->is_integral_pol = true;
             for(int i=0; i<=P.degree(); i++)
             {
                if(!(P[i].PTR)->is_integral_pol) 
                {
                  z_rep->is_integral_pol = false;
                  break;
                }
             }
             z_rep->degree_measure_possible = z_rep->is_integral_pol;
        #endif
             z_rep->I = PACKAGE_SCOPE to_interval(z_rep->left, z_rep->right);
             z_rep->is_general_ = true;
             z_rep->is_rational_ = false;

        #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             z_rep->j_ = j;
             CURRENT_REAL_REPS->insert(z_rep,original_POL,z_rep->I,j);
        #endif


        return *z_rep;
}

real diamond(rational l, rational u, const Polynomial& P, algorithm_type algorithm, bool is_squarefree)
{
   
   #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        int j = -1; // the number of the root is not known
        Polynomial original_POL = P;
             std::vector<real_rep*> Vrep = CURRENT_REAL_REPS->find(P,PACKAGE_SCOPE to_interval(l,u));
             real_rep* h_rep = NULL;
             int root_number = -1;
             bool easy_test = (Vrep[2]!=NULL);
             for(int i=0; i<3; i++)
             {
               h_rep = Vrep[i]; 
               if(h_rep == NULL) break; 

               Polynomial Ptest = h_rep->OPOL;

               double du = (h_rep->I).upper_bound();
               double dl = (h_rep->I).lower_bound();

               // It might happen that I does not intersect (l,u). In this case
               // update the interval in CURRENT_DIAMONDS.
               if(du < l || dl > u)
               {
                  CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                  CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                  continue;
               }
               if(dl < l) dl = l.to_double();
               if(du > u) du = u.to_double();

               if((easy_test && i!=2) || ( !easy_test &&
                                         sign(Ptest.evaluate(du)) == sign(Ptest.evaluate(dl))
                                         && sign(Ptest.evaluate(du))!=0) )
               {
                  // h_rep represents not the root in (l,u)
                  // make the interval of h_rep smaller 
                  // As real_interval has double bounds (instead of rational bounds)
                  // the interval has to be smaller, such that the corresponding bound
                  // is far away from l / u.

                  CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                  if((h_rep->I).lower_bound() < l) 
                  {
                     int s = sign(Ptest.evaluate((h_rep->I).lower_bound()));
                     rational m = (h_rep->left +l)/rational(2,1);
                     while(s == sign(Ptest.evaluate(m))) m = (m+l)/rational(2,1);
                     h_rep->right = m;
                  }
                  else 
                  {
                     int s = sign(Ptest.evaluate((h_rep->I).upper_bound()));
                     rational m = (u+h_rep->right)/rational(2,1);
                     while(s == sign(Ptest.evaluate(m))) m = (u+m)/rational(2,1);
                     h_rep->left = m;
                  }
                  h_rep->I = PACKAGE_SCOPE to_interval(h_rep->left, h_rep->right);
                               CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                               if(h_rep->APP_BF_PTR != NULL) h_rep->APP_BF_PTR = NULL;
                               h_rep->init_app_bf();
               }
               else // h_rep represents the root in (l,u)
                 root_number = i;
             }
             if(root_number != (-1)) h_rep = Vrep[root_number];
         

        // update the information (the new interval might be smaller)
        if(h_rep != NULL)
        {
                CURRENT_REAL_REPS->delete_element(P,h_rep->I,h_rep->j_);
                if((h_rep->I).lower_bound() < l) h_rep->left = l;
                if((h_rep->I).upper_bound() > u) h_rep->right = u;
                h_rep->I = PACKAGE_SCOPE to_interval(h_rep->left, h_rep->right);
                        CURRENT_REAL_REPS->insert(h_rep,P,h_rep->I,h_rep->j_);
                        if(h_rep->APP_BF_PTR != NULL) h_rep->APP_BF_PTR = NULL;
                        h_rep->init_app_bf();
        }
        real_rep* z_rep = h_rep;
   #else
        real_rep* z_rep = NULL;
   #endif // DONOT_...

   if(z_rep != NULL)
   {
      z_rep->ref_plus();
      return *z_rep;
   }

   if (P.degree()==1) return (-P[0])/P[1];
   if (P.degree()==2)
   {
     real disc = real(P[1]*P[1] - 4 * P[2]*P[0]);
     real x0 = real((-P[1]-sqrt(disc))/(2*P[2]));
     real z;
     if(l <= x0 && x0 <= u) z = x0;
     else z = real((-P[1]+sqrt(disc))/(2*P[2])); // j=1
       if(!z.PTR)
       {
          #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
               if(!(z.PTR = CURRENT_REAL_REPS->find(z.value))) z.PTR = new real_rep(z.value);
               else z.PTR->ref_plus();
          #else
               z.PTR = new real_rep(z.value);
          #endif

       }
       z.set_polynomial(P);

     return z;
   }


        z_rep = new real_rep();
        z_rep->OP1 = NULL;
        z_rep->OP2 = NULL;

        if(4*(u-l)> 1)
        {
           isolating<real> Iso(P,l,u,algorithm);
           if(Iso.is_exact_root())
           {
              delete z_rep;
              return Iso.left_boundary();
           }
           l = Iso.left_boundary();
           u = Iso.right_boundary(); 
        }


        z_rep->left = l;
        z_rep->right = u;

             z_rep->first = true;
             z_rep->pol_squarefree = is_squarefree;
             z_rep->CON = DIAMOND;
        #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             z_rep->OPOL = P;
        #endif // DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                  for(int c=0; c<=P.degree(); c++)
                  {
                     if (!(P[c]).PTR) 
                     {
                       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                 if(!(((real&) (P[c])).PTR = CURRENT_REAL_REPS->find((P[c]).value))) 
                                   ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                                 else ((real&) (P[c])).PTR->ref_plus();
                       #else
                               ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                       #endif

                     }
                     (P[c].PTR)->ref_plus();
                  }

             z_rep->RPOL = P;
        #ifndef BFMSS_ONLY
             z_rep->is_integral_pol = true;
             for(int i=0; i<=P.degree(); i++)
             {
                if(!(P[i].PTR)->is_integral_pol) 
                {
                  z_rep->is_integral_pol = false;
                  break;
                }
             }
             z_rep->degree_measure_possible = z_rep->is_integral_pol;
        #endif
             z_rep->I = PACKAGE_SCOPE to_interval(z_rep->left, z_rep->right);
             z_rep->is_general_ = true;
             z_rep->is_rational_ = false;

        #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
             z_rep->j_ = j;
             CURRENT_REAL_REPS->insert(z_rep,original_POL,z_rep->I,j);
        #endif


        return *z_rep;
}


void real::convert_int_poly_to_real_poly(const int_Polynomial& iP, Polynomial& P)
{
  LEDA_VECTOR<real> VP(iP.degree()+1);
  for(int i=0; i<=iP.degree(); i++) VP[i] = real(iP[i]);
  P = VP;
}




  int real_roots(const Polynomial& P, 
                 REAL_CONTAINER roots, 
                 algorithm_type alg, bool is_squarefree)
  {
     if(P.degree() == 1)
     {
        REAL_CONTAINER_APPEND(roots, -P[0]/P[1]);
        return 1;
     }

#ifdef TRY_USP_WITH_INTERVAL 

     int interval_problem = -1;
     if(is_squarefree && alg == USPENSKY)
     {
             LEDA_VECTOR<real_interval> AI(P.degree()+1);
             int nI = P.degree();
          if(!(P[nI]).PTR) AI[nI] = (P[nI]).value;
          else AI[nI] = ((P[nI]).PTR)->I;
          for(int i=0; i<nI; i++)
          {
            if(!(P[i]).PTR) AI[i] = (P[i]).value;
            else AI[i] = ((P[i]).PTR)->I;
            AI[i] = AI[i] / AI[nI];
          }
          AI[nI] = real_interval(1);
             Poly_interval PI(AI);

        isolating<real_interval> IsoI(PI,USPENSKY,true);
        if(IsoI.can_be_isolated())
        {
           int n=IsoI.number_of_real_roots();
           bool interval_ok = true;

           for(int i=0; i<n; i++) 
           {
              int s = has_root(P,IsoI.left_boundary(i), IsoI.right_boundary(i));
              if(s<0)
                 REAL_CONTAINER_APPEND(roots, diamond_short(IsoI.left_boundary(i), IsoI.right_boundary(i), P, alg));
              else if(s==0) // P has an exact root at the boundary.
              {
                 if(P.evaluate(IsoI.right_boundary(i)) == 0)
                    REAL_CONTAINER_APPEND(roots, IsoI.right_boundary(i));
                 else if(P.evaluate(IsoI.left_boundary(i)) == 0)
                    REAL_CONTAINER_APPEND(roots, IsoI.left_boundary(i));
              }
              else
              {
                 interval_ok = false;
                 interval_problem = i;
                 break;
              }
           }
           if(interval_ok == true) return n;
        }
     }
#endif //TRY_USP_WITH_INTERVAL

     isolating<real> Is(P,alg);
     int n=Is.number_of_real_roots();

     for(int i=0; i<n; i++) 
     {
#ifdef TRY_USP_WITH_INTERVAL
        if(i >= interval_problem)
        {
#endif
           if(Is.is_exact_root(i)) REAL_CONTAINER_APPEND(roots, real(Is.left_boundary(i)));
           else REAL_CONTAINER_APPEND(roots, diamond(Is.left_boundary(i), Is.right_boundary(i), P, alg));
#ifdef TRY_USP_WITH_INTERVAL
        }
#endif

     }
     return n;
  }



int real_roots(const Polynomial& P, 
               LEDA_VECTOR<real>& roots, 
               algorithm_type alg, bool is_squarefree)
{
  list<real> root_list;
  int n_roots = real_roots(P, root_list, alg, is_squarefree);
  roots.resize(root_list.size());
  {
    int i = 0;
    real rt;
    forall(rt, root_list) roots[i++] = rt;
  }
  return n_roots;
}



 real operator +=(real& x,const real& y) 
 {  x = x+y;  return x;  }
 real operator -=(real& x,const real& y) 
 {  x = x-y;  return x;  }  
 real operator *=(real& x,const real& y) 
 {  x = x*y;  return x;  }
 real operator /=(real& x,const real& y) 
 {  x = x/y;  return x;  }



int real::compare(const real& y)
{ return leda::compare(*this, y); }

int compare_all(const LEDA_VECTOR<real>& R, const real& x)
{
  const int n = R.size();
  int intersect;
  int prec = DOUBLE_PREC;
  bool* isolated = new bool[n];
  bigfloat* L = new bigfloat[n];
  bigfloat* U = new bigfloat[n];

  for(int i=0; i<n; i++) isolated[i] = false;

  do
  {
     intersect = 0;
     int i;

     // get interval bounds
     for(i=0; i<n; i++)
     {
        if(!isolated[i])
        {
           L[i] = R[i].get_lower_bound();
           U[i] = R[i].get_upper_bound();
        }
     }
     bigfloat xl = x.get_lower_bound();
     bigfloat xu = x.get_upper_bound();

     int I = 0;
     for(i=0; i<n; i++)
     {
        if(!isolated[i])
        {
           // check if the interval for x intersects [L[i], U[i]]
           if(   (xl >= L[i] && xl <= U[i])
              || (xu >= L[i] && xu <= U[i])
              || (xl <= L[i] && xu >= U[i]))
           {
                 I=i;
                 intersect++;
           }
           else isolated[i] = true;
        }
     }
     if(intersect == 1) {
       delete[] isolated; delete[] L; delete[] U;
       return I;
     }
     prec *= 2;
     for(i=0; i<n; i++) 
     {
        if(!isolated[i]) (R[i]).improve_approximation_to(prec);
        x.improve_approximation_to(prec);
     }
  } while(intersect > 1);
  
  // this should never be reached (?)
  return -1;
}


int compare_all(const LEDA_VECTOR<real>& R, int& J)
{
  const int n = R.size();
  int intersect;
  int prec = DOUBLE_PREC;
  bool* isolated = new bool[n];
  bool* possible_intersect = new bool[n];
  bigfloat* L = new bigfloat[n];
  bigfloat* U = new bigfloat[n];

  for(int i=0; i<n; i++) isolated[i] = false;

  do
  {
     intersect = 0;
     int i;
     for(i=0; i<n; i++) possible_intersect[i] = false;

     // get interval bounds
     for(i=0; i<n; i++)
     {
        if(!isolated[i])
        {
           L[i] = R[i].get_lower_bound();
           U[i] = R[i].get_upper_bound();
        }
     }

     int I = 0;
     for(i=0; i<n; i++)
     {
        if(!isolated[i])
        {
           // check if an interval intersects [L[i], U[i]]
           bool inter=false;
           for(int j=i+1; j<n; j++)
           { 
              if(!isolated[j] &&
                (    (L[j] >= L[i] && L[j] <= U[i]) 
                  || (U[j] >= L[i] && U[j] <= U[i])
                  || (L[j] <= L[i] && U[i] <= U[j])
                  || (L[i] <= L[j] && U[j] <= U[i])
                ))
              {
                 J=j; I=i;
                 intersect++;
                 inter = true;
                 possible_intersect[j] = true;
              }
           }
           if(!inter && !possible_intersect[i]) isolated[i] = true;
        }
     }
     if(intersect == 1) {
       delete[] isolated; delete[] possible_intersect; delete[] L; delete[] U;
       return I;
     }
     prec *= 2;
     for(i=0; i<n; i++) 
     {
        if(!isolated[i]) (R[i]).improve_approximation_to(prec);
     }
  } while(intersect > 1);
  
  // this should never be reached (?)
  return -1;
}


bool real::sign_is_known() const
{ return true;}

int real::sign() const {  
  if (is_double)
    return FP_SCOPE sign(value);
  else
    return PTR->sign_with_separation_bound(zero_integer,false); 
}

int real::sign(const integer& q) const{  
  if (is_double)
   return FP_SCOPE sign(value);
  else
   return PTR->sign_with_separation_bound(q,true); 
}

int real::sign(long p) const {  
  if (is_double)
    return FP_SCOPE sign(value);
  else
    return PTR->sign_with_separation_bound(p,true); 
}


void real_rep::compute_concrete_parameters() 
{
  sep_bound* ptr = sep_bound_ptr;
  sep_bound* ptr1=NULL; if (OP1) ptr1 = OP1->sep_bound_ptr;
  sep_bound* ptr2=NULL; if (OP2) ptr2 = OP2->sep_bound_ptr;

  switch(CON) { 
    case DOUBLE:  
    case BIGFLOAT: {
      bigfloat& num = APP_BF_PTR->NUM_BF;

      ptr->L = zero_integer;
      if(isZero(num))
      {
        ptr->v = 0;
        ptr->U = zero_integer; 
#ifndef BFMSS_ONLY
        ptr->M = 0;
        ptr->D = 1;

        ptr->lc = 1;
        ptr->tc = 0;
        ptr->mu = 0;
        ptr->nu = 0;
#endif
      }
      else
      {
        integer r = zero_integer;
        ptr->v = v2(num,r);
        // if (sign(bigfloat(r).get_exponent()) >= 0) is true in this case
        ptr->U = log2_abs(r);
#ifndef BFMSS_ONLY
        if (PACKAGE_SCOPE sign(num.get_exponent()) >= 0)
        {
          ptr->M = ilog2(num);

          ptr->lc = 0;
          ptr->tc = ptr->M;
        }
        else {
          ptr->M =  num.get_significant_length() + 1;
          integer Md = -num.get_exponent();
          if(Md > ptr->M) ptr->M = Md;

          ptr->lc = -num.get_exponent();
          ptr->tc =  num.get_significant_length()+1;
        }
        ptr->D = 1;

        ptr->mu = ilog2(num);
        ptr->nu = ptr->mu-1;
        if(ptr->nu <0) ptr->nu=0;
#endif
      }

      break;
    }
    case NEGATION:  
      ptr->v = ptr1->v;
      ptr->U = ptr1->U; 
      ptr->L = ptr1->L;
#ifndef BFMSS_ONLY
      ptr->M = ptr1->M;
      ptr->D = ptr1->D;

      ptr->lc = ptr1->lc;
      ptr->tc = ptr1->tc;
      ptr->mu = ptr1->mu;
      ptr->nu = ptr1->nu;
#endif
      break;
    case MULTIPLICATION:
      ptr->v = ptr1->v + ptr2->v;
      ptr->U = ptr1->U + ptr2->U; 
      ptr->L = ptr1->L + ptr2->L;
#ifndef BFMSS_ONLY
      ptr->M = ptr2->D * ptr1->M + ptr1->D * ptr2->M;
      ptr->D = ptr1->D * ptr2->D;

      ptr->lc = ptr2->D * ptr1->lc + ptr1->D * ptr2->lc;
      ptr->tc = ptr2->D * ptr1->tc + ptr1->D * ptr2->tc;
      ptr->mu = ptr1->mu + ptr2->mu; 
      ptr->nu = ptr1->nu + ptr2->nu; 
#endif
      break;
    case DIVISION:
      ptr->v = ptr1->v - ptr2->v;
      ptr->U = ptr1->U + ptr2->L;
      ptr->L = ptr1->L + ptr2->U;
#ifndef BFMSS_ONLY
      ptr->M = ptr2->D * ptr1->M + ptr1->D * ptr2->M;
      ptr->D = ptr1->D * ptr2->D;

      ptr->lc = ptr2->D * ptr1->lc + ptr1->D * ptr2->tc;
      ptr->tc = ptr2->D * ptr1->tc + ptr1->D * ptr2->lc;
      ptr->mu = ptr1->mu - ptr2->nu; 
      ptr->nu = ptr1->nu - ptr2->mu; 
#endif
      break;
    case ADDITION:
    case SUBTRACTION:
      ptr->v = Minimum(ptr1->v, ptr2->v);
      ptr->U = Maximum((ptr1->v - ptr->v) + ptr1->U + ptr2->L, 
                               (ptr2->v - ptr->v) + ptr2->U + ptr1->L ) + 1;
      ptr->L = ptr1->L + ptr2->L;
#ifndef BFMSS_ONLY
      ptr->D = ptr1->D * ptr2->D;
      ptr->M = ptr->D + ptr2->D * ptr1->M + ptr1->D * ptr2->M;

      ptr->lc = ptr2->D * ptr1->lc + ptr1->D * ptr2->lc;
      ptr->tc = ptr->M;
      ptr->mu = Maximum(ptr1->mu, ptr2->mu) + 1; 
      ptr->nu = -Minimum(ptr->M, (ptr->D-1)*ptr->mu + ptr->lc);
#endif
      break;
    case SQUAREROOT:
      ptr->v = (ptr1->v)/2;
      ptr->U = ptr1->U;
      ptr->L = ptr1->L;

      if((ptr1->v + ptr->U) >= ptr->L)
      {
        ptr->U += ptr1->L + (ptr1->v - (2*(ptr->v)));
        ptr->U = (ptr->U >> 1) + 1;
      }
      else
      {
        ptr->L += ptr1->U - (ptr1->v + (2*(ptr->v)));
        ptr->L = (ptr->L >> 1) + 1;
      }
#ifndef BFMSS_ONLY
      ptr->D = ptr1->D * 2;
      ptr->M = ptr1->M;

      ptr->lc = ptr1->lc;
      ptr->tc = ptr1->tc;
      ptr->mu = (ptr->mu >> 1) +1;
      ptr->nu = (ptr->nu >> 1);
#endif
      break;
    case ROOT:
      ptr->v = (ptr1->v)/d;
      ptr->U = ptr1->U;
      ptr->L = ptr1->L;

      if (ptr1->v + ptr->U >= ptr->L)
      {
        ptr->U += (d-1)*ptr1->L + (ptr1->v - d*ptr->v);
        ptr->U = ptr->U/d + 1;
      }
      else
      {
        ptr->L += (d-1)*ptr1->U - (ptr1->v + d*ptr->v);
        ptr->L = ptr->L/d + 1;
      }
#ifndef BFMSS_ONLY
      ptr->D = ptr1->D * d;
      ptr->M = ptr1->M;

      ptr->lc = ptr1->lc;
      ptr->tc = ptr1->tc;
      ptr->mu = (ptr->mu/d) +1;
      ptr->nu = (ptr->nu/d);
#endif
      break;

    case RATIONAL:
      if (rat() == 0)
      {
        ptr->v = 0;
        ptr->L = ptr->U = zero_integer; 
#ifndef BFMSS_ONLY
        ptr->M = 0;
        ptr->D = 1;

        ptr->lc = 1;
        ptr->tc = 0;
        ptr->mu = 0;
        ptr->nu = 0;
#endif
      }
      else 
      { // simulate bigfloat(numerator) / bigfloat(denominator)
        integer n1 = rat().numerator(), n2 = rat().denominator();
        integer r1 = zero_integer, r2 = zero_integer;
        ptr->v = v2(n1, r1) - v2(n2, r2);
        ptr->U = log2_abs(r1) /* + 0 */;
        ptr->L = /* 0 + */ log2_abs(r2);
#ifndef BFMSS_ONLY
        integer M1 = log2_abs(n1), M2 = log2_abs(n2);
        integer nu1 = log(n1), nu2 = log(n2);

        ptr->M = M1 + M2;
        ptr->D = 1;
        ptr->lc = M2;
        ptr->tc = M1;
        ptr->mu = M1 - nu2; 
        ptr->nu = nu1 - M2; 
#endif
      }
      break;

    case DIAMOND:
      int deg = RPOL.degree();

      // get the vi's
#ifdef v_IS_LONG
      long* vi = new long[deg+1];
#else
      integer* vi = new integer[deg+1];
#endif
      int i;
      for(i=deg; i>=0; i--)
      {
        vi[i] = ((RPOL[i].PTR)->sep_bound_ptr)->v;
      }

      // computation of m:
      if(deg == 0) ptr->v = 0;
      else
      {
#ifdef v_IS_LONG
        ptr->v = (vi[0]-vi[deg])/deg;
        for(i=1; i<=deg-1; i++)
        {
          long k=(vi[i]-vi[deg])/(deg-i);
          if(k<ptr->v) ptr->v=k;
        }
#else
        ptr->v = (vi[0]-vi[deg])/deg;
        for(i=1; i<=deg-1; i++)
        {
          integer k=(vi[i]-vi[deg])/(deg-i);
          if(k<ptr->v) ptr->v=k;
        }
#endif
      }

      //computation of L 
      ptr->L = ((RPOL[deg].PTR)->sep_bound_ptr)->U;
      for(int k = 0; k<deg; k++)
        ptr->L += ((RPOL[k].PTR)->sep_bound_ptr)->L;

      //computation of U  
      // with the Lagrange-Zassenhaus bound.

      integer Mk = 0; // Mk = max{(1/(deg-i)) b_i, i=0,...,deg}
      int Mik = 0;    // Mik = the index i from Mk

      for (i=0; i<deg; i++ )
      {
        integer bik =((RPOL[i].PTR)->sep_bound_ptr)->U
                    +((RPOL[deg].PTR)->sep_bound_ptr)->L
                    -((RPOL[i].PTR)->sep_bound_ptr)->L
                    -((RPOL[deg].PTR)->sep_bound_ptr)->U
                    + vi[i] - vi[deg];
        // bi = 2^{v_i - v_n) (u_2(E_i)l_2(E_n))/(l_2(E_i)u_2(E_n))
      
        if((deg-Mik)*bik>(deg-i)*Mk) 
        {
          Mk=bik;
          Mik = i;
        }
      }  

      ptr->U = Mk/(deg-Mik) + 2 + ptr->L - ptr->v;
      
      delete[] vi; vi = 0;
#ifndef BFMSS_ONLY
      if(!is_integral_pol)
      { 
         ptr->D = 1;
         ptr->M = 1;
         ptr->lc = 1;
         ptr->tc = 1;
         ptr->mu = 1;
         ptr->nu = 1;
      }
      else
      {
         ptr->D = deg;
         bigfloat sum = 0;
         for(int i=0; i<=deg; i++)
         {
           bigfloat s = (abs(RPOL[i])).get_upper_bound();
           sum += s * s;
         }
         ptr->M = ((ilog2(sum)) >> 1) + 1;
/*
#ifndef NO_EXACUS
               ptr->M = measure(RPOL);
#endif
*/
         bigfloat c = (abs(RPOL[deg])).get_upper_bound();
         ptr->lc = ilog2(c)+1;
         int i=0;
         do
         {
           c = (abs(RPOL[i])).get_upper_bound();
         }
         while(c==0);
         ptr->tc = ilog2(c)+1;

         for(int j=i+1; j<=deg; j++)
         {
           if((abs(RPOL[j])).get_upper_bound() > c)
           c = (abs(RPOL[j])).get_upper_bound();
         }
         ptr->mu = ilog2(c+1)+1;
         ptr->nu = -ptr->mu;
      }
#endif

      break;

  }
}




integer real::separation_bound() {
   integer b = sep_bfmss();
#ifndef BFMSS_ONLY
   if(!PTR || PTR->degree_measure_possible)
   {
      integer d = sep_degree_measure();
      if(d > b) b = d;
      d = sep_li_yap();
      if(d > b) b = d;
   }
#endif
   return b;
}

void real::print_separation_bounds(){

  cout << endl;
  cout << "KARY BFMSS: " << sep_bfmss() << endl;
#ifndef BFMSS_ONLY
  if(!PTR || PTR->degree_measure_possible)
     cout << "DEGREE MEASURE: " << sep_degree_measure() << endl;
  if(!PTR || PTR->degree_measure_possible)
     cout << "LI YAP: " << sep_li_yap() << endl;
#endif

  cout << endl;
}

integer real::sep_bfmss() {
  if(is_double) // bound = v + U(1-D) - L with D=1 and L=0.
  {
    bigfloat num(value);
    if(isZero(num)) return 0;
    integer r;
    return (v2(num,r));
  }
  return PTR->sep_bfmss();
}

integer real_rep::sep_bfmss()
{
  if (!sep_bound_ptr) 
    sep_bound_ptr = new sep_bound();
  init_app_bf();
  clear_visited_marks();
  compute_parameters();
  clear_visited_marks();
  integer degree = 1;
  estimate_degree(degree);
  clear_visited_marks();
  return (sep_bound_ptr->v+sep_bound_ptr->U*(1-degree)-sep_bound_ptr->L);
}

#ifndef BFMSS_ONLY
integer real::sep_degree_measure() {
  if(is_double)
  {
      bigfloat num(value);
      if(isZero(num)) return 0;
      if (PACKAGE_SCOPE sign(num.get_exponent()) >= 0) return ilog2(num);
      integer M =  num.get_significant_length() + 1;
      integer Md = -num.get_exponent();
      if(Md > M) return(Md);
      return M;
  }
  return PTR->sep_degree_measure();
}

integer real_rep::sep_degree_measure()
{
  if (!sep_bound_ptr) 
    sep_bound_ptr = new sep_bound();
  init_app_bf();
  clear_visited_marks();
  compute_parameters();
  clear_visited_marks();
  return (-sep_bound_ptr->M);
}

integer real::sep_li_yap() 
{
  if(is_double)
  {
      bigfloat num(value);
      if(isZero(num)) return 0;
      if (PACKAGE_SCOPE sign(num.get_exponent()) >= 0) return 0;
      return (num.get_exponent());
  }
  return PTR->sep_li_yap();
}

integer real_rep::sep_li_yap()
{
  if (!sep_bound_ptr) 
    sep_bound_ptr = new sep_bound();
  init_app_bf();
  clear_visited_marks();
  compute_parameters();
  clear_visited_marks();
  integer degree = 1;
  estimate_degree(degree);
  clear_visited_marks();
  return (sep_bound_ptr->mu*(1-degree)-sep_bound_ptr->lc);
}

#endif


void real_rep::compute_parameters()
{
  if (status == VISITED) 
    return;
  if (OP1) 
    OP1->compute_parameters();
  if (OP2) 
    OP2->compute_parameters();

  if(CON == DIAMOND)
  {
     int n;
     n = RPOL.degree();
     for(int i=0; i<=n; i++)
       (RPOL[i].PTR)->compute_parameters();
  }


  if (!sep_bound_ptr) 
    sep_bound_ptr = new sep_bound();
  compute_concrete_parameters();
  status = VISITED;
}


void real_rep::estimate_degree(integer& D)
{
  if (status == VISITED) return;
  if (OP1) 
    OP1->estimate_degree(D);
  if (OP2) 
    OP2->estimate_degree(D);
  if (CON == SQUAREROOT) D*=2;
  if (CON == ROOT) D*=d;

  if (CON == DIAMOND)
  {
    int Pd;
    Pd = RPOL.degree();
    for(int i=0; i<=Pd; i++) (RPOL[i].PTR)->estimate_degree(D);
    D*=Pd;
  }


  status = VISITED;
}

void real_rep::clear_visited_marks()
{
  if (status == CLEARED) return;
  if (OP1) OP1->clear_visited_marks();
  if (OP2) OP2->clear_visited_marks();

  if (CON == DIAMOND)
  {
    int Pd;
    Pd = RPOL.degree();
    for(int i=0; i<=Pd; i++) (RPOL[i].PTR)->clear_visited_marks();
  }

  status = CLEARED;
}

void real::clear_visited_marks()
{
  if(!is_double) PTR->clear_visited_marks();
}

#ifndef BFMSS_ONLY
void real_rep::compute_concrete_system_bound(integer& system_bound)
{
  system_bound = sep_bfmss();
  if(degree_measure_possible)
  {
     integer degree_measure_bound = sep_degree_measure();
     integer li_yap_bound = sep_li_yap();
     if(!real::counting_best || system_bound_computed)
     {
        if(degree_measure_bound > system_bound) 
           system_bound=degree_measure_bound;
        if(li_yap_bound > system_bound)
           system_bound = li_yap_bound;
     }
     else
     {
        if(degree_measure_bound > system_bound) 
        {
           if(li_yap_bound > degree_measure_bound)
           {
              system_bound = li_yap_bound;
              real::li_yap_best++;
           }
           else if(degree_measure_bound == li_yap_bound)
           {
              system_bound=degree_measure_bound;
              real::li_yap_degree_measure_best++;
           }
           else 
           {
              system_bound=degree_measure_bound;
              real::degree_measure_best++;
           }
        }
        else if(degree_measure_bound == system_bound) 
        {
           if(li_yap_bound > system_bound)
           {
              system_bound = li_yap_bound;
              real::li_yap_best++;
           }
           else if(system_bound == li_yap_bound)
              real::bfmss_li_yap_degree_measure_best++;
           else
              real::bfmss_degree_measure_best++;
        }
        else if(li_yap_bound > system_bound)
        {
           system_bound = li_yap_bound;
           real::li_yap_best++;
        }
        else if(system_bound == li_yap_bound)
           real::bfmss_li_yap_best++;
        else 
           real::bfmss_best++;
     }
  }
  else if(real::counting_best && !system_bound_computed) real::bfmss_best++;
}
#endif


int real_rep::sign_with_separation_bound(const integer& user_bound, bool supported)
{ 
  
    if (I.sign_is_known())
      return I.sign();
    init_app_bf();
    if   ( exact() || (!isZero(num_bf()) && (num_exp() > err_exp())) ) 

      return PACKAGE_SCOPE sign(num_bf());

  bool user_bound_is_reached = false;
  integer relative_precision = 26;
  integer absolute_precision;
  do
  {  
    relative_precision = relative_precision*2;
	absolute_precision = err_exp() - relative_precision;
    guarantee_bound_two_to(absolute_precision);
    user_bound_is_reached = supported && (1 + absolute_precision < -user_bound); 
  }
  while 
  (!(user_bound_is_reached ||   ( exact() || (!isZero(num_bf()) && (num_exp() > err_exp())) ) 

));


  if   ( exact() || (!isZero(num_bf()) && (num_exp() > err_exp())) ) 

 
//	  if(to_double(I) != 0) return PACKAGE_SCOPE sign(num_bf());
  return PACKAGE_SCOPE sign(num_bf());
  
    I = 0;
    num_bf() = error_bf() = bigfloat::pZero;
    {
      delete_current_real_rep();
      if (OP1)  if (OP1->ref_minus() == 0)  delete OP1;
      if (OP2)  if (OP2->ref_minus() == 0)  delete OP2;
      OP1 = OP2 = NULL;
      CON = BIGFLOAT;
    #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
      CURRENT_REAL_REPS->insert(this, num_bf());
    #endif

    }




  return 0;
}

int compare(const real& x,const real& y)
{
  if (x.is_double && y.is_double) {
    double xd = x.value, yd = y.value;
                return xd < yd ? -1 : (xd > yd ? +1 : 0);
  }
  
  real_interval X,Y;
  if (x.is_double) X=x.value; else X=x.PTR->I;
  if (y.is_double) Y=y.value; else Y=y.PTR->I;
  real_interval Z = X-Y;
  if (Z.sign_is_known()) return Z.sign();
  
  return (x-y).sign();  
}

void real::improve_approximation_to(const integer& p) const { 
  if (is_double) return;
  PTR->init_app_bf();
  PTR->guarantee_bound_two_to(-p); 
  adjust();
}

void real::guarantee_relative_error(long p) const
{  if (is_double) return;
   if (PTR->sign_with_separation_bound(zero_integer) == 0) return;
   PTR->init_app_bf();
   bigfloat num_low = abs(PTR->num_bf()) - PTR->error_bf(); 
   PTR->guarantee_bound_two_to(ilog2(num_low) - p);
   adjust(); 
}


void real::compute_with_precision(long p) const
{  if (is_double) return;
   PTR->init_app_bf();
   PTR->clear_visited_marks();
   PTR->compute_approximation(p);
   PTR->clear_visited_marks();
   adjust();
}



 void real_rep::compute_approximation(long p)
  {  
    if (status == VISITED) return;
    if (OP1) OP1->compute_approximation(p);
    if (OP2) OP2->compute_approximation(p);
    compute_op(p);
    status = VISITED;
  }

void real_rep::compute_op(long p)
{
 bool isexact=true;
 bigfloat operation_error, operand_error;
 switch(CON) 
 {
    case DOUBLE: 
          num_bf() = to_double(I); return;
    case BIGFLOAT: 
          return;
    case NEGATION: 
          num_bf() = -OP1->num_bf();
          error_bf() = OP1->error_bf();
          break;
    case ADDITION:
          num_bf() = add(OP1->num_bf(),OP2->num_bf(),p,TO_NEAREST,isexact);
          operand_error = add(OP1->error_bf(),OP2->error_bf(),ERROR_PREC,TO_INF);


          break;
    case SUBTRACTION: 
          num_bf() = sub(OP1->num_bf(),OP2->num_bf(),p,TO_NEAREST,isexact);
          operand_error = add(OP1->error_bf(),OP2->error_bf(),ERROR_PREC,TO_INF);


          break;
    case MULTIPLICATION:
          num_bf() = mul(OP1->num_bf(),OP2->num_bf(),p,TO_NEAREST,isexact);
          compute_mul_error(operand_error);
          break;
    case DIVISION: 
          if (OP2->sign_with_separation_bound(zero_integer,false) == 0)
          HANDLE_ERROR(1,"compute_op:division by zero");
          num_bf() = div(OP1->num_bf(),OP2->num_bf(),p,TO_NEAREST,isexact);
          compute_div_error(operand_error);
          break;
    case SQUAREROOT: 
          if (OP1->sign_with_separation_bound(zero_integer,false) < 0)
             HANDLE_ERROR(1,"compute_op:sqrt of negative number");
		  num_bf() = sqrt_bf(OP1->num_bf(),p,TO_NEAREST,isexact);
		  compute_sqrt_error(operand_error);
		  break;
    case ROOT: 
          if (OP1->sign_with_separation_bound(zero_integer,false) < 0)
             HANDLE_ERROR(1,"compute_op:root of negative number");
          num_bf() = sqrt_d(OP1->num_bf(),p,d);
          isexact = false;
          compute_root_error(operand_error);
          break;

        case RATIONAL:
          num_bf() = convert_to_bf(rat(),p,TO_NEAREST,isexact);
          break;

    case DIAMOND: 
         {
          
          if(first)
          {
                  Polynomial P = RPOL;
                  if(refine(P,left, right, pol_squarefree)) // root found exactly
                  {
                     if(P.degree() <= 2 && left != right) 
                     {
                        if(P.degree() == 1)
                        {
                           CON = DIVISION;
                           real x(-P[0]);
                           real y(P[1]);

                             if (!x.PTR) 
                             {
                               #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                   if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
                                     ((real&) x).PTR = new real_rep(x.value);
                                   else ((real&) x).PTR->ref_plus();
                               #else
                                   ((real&) x).PTR = new real_rep(x.value);
                               #endif
                             }
                             if (!y.PTR) 
                             {
                               #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                   if(!(((real&) y).PTR = CURRENT_REAL_REPS->find(y.value))) 
                                     ((real&) y).PTR = new real_rep(y.value);
                                   else ((real&) y).PTR->ref_plus();
                               #else
                                   ((real&) y).PTR = new real_rep(y.value);
                               #endif
                             }


                           #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                            real_rep* z_rep = CURRENT_REAL_REPS->find(CON,(x.PTR)->ref(), (y.PTR)->ref());
                                            if(z_rep != NULL)
                                            {
                                               z_rep->OPOL = OPOL;
                                               z_rep->ref_plus();
                                               ((real_rep&) *this) = *z_rep;
                                               compute_op(p);
                                               return;
                                            }
                           #endif

                           OP1 = x.PTR;
                           OP2 = y.PTR;
                           OP1->ref_plus();  
                           OP2->ref_plus();
                           is_general_ = false;
                           is_rational_ = true;
                           compute_op(p);

                           #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                            CURRENT_REAL_REPS->insert(CON, this, (x.PTR)->ref(), (y.PTR)->ref());
                           #endif

                           return;
                        }
                        else // P.degree()==2
                        {
                           real disc = real(P[1]*P[1] - 4 * P[2]*P[0]);
                           real x = sqrt(disc)/real(2*P[2]);
                           real y = real((-P[1])/(2*P[2]));

                             if (!x.PTR) 
                             {
                               #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                   if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
                                     ((real&) x).PTR = new real_rep(x.value);
                                   else ((real&) x).PTR->ref_plus();
                               #else
                                   ((real&) x).PTR = new real_rep(x.value);
                               #endif
                             }
                             if (!y.PTR) 
                             {
                               #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                   if(!(((real&) y).PTR = CURRENT_REAL_REPS->find(y.value))) 
                                     ((real&) y).PTR = new real_rep(y.value);
                                   else ((real&) y).PTR->ref_plus();
                               #else
                                   ((real&) y).PTR = new real_rep(y.value);
                               #endif
                             }



                           if(left <= x-y && x-y <= right) CON = SUBTRACTION;
                           else CON = ADDITION;

                           #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                            real_rep* z_rep = CURRENT_REAL_REPS->find(CON,(x.PTR)->ref(), (y.PTR)->ref());
                                            if(z_rep != NULL)
                                            {
                                               z_rep->OPOL = OPOL;
                                               z_rep->ref_plus();
                                               ((real_rep&) *this) = *z_rep;
                                               compute_op(p);
                                               return;
                                            }
                           #endif
                           
                           OP1 = x.PTR;
                           OP2 = y.PTR;
                           OP1->ref_plus();  
                           OP2->ref_plus();
                           is_general_ = false;
                           is_rational_ = false;
                           compute_op(p);

                           #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                            CURRENT_REAL_REPS->insert(CON, this, (x.PTR)->ref(), (y.PTR)->ref());
                           #endif
                           return;
                        }
                     }
                     else
                     {
                        num_bf() = to_bigfloat(real(left));
                        error_bf() = 0;
                        return;
                     }
                  }
                  if(RPOL.degree()>P.degree())
                  {
                     // refine changes the coefficients
                     int n = RPOL.degree();
                     for(int i=0; i<=n; i++) 
                     {
                       if ((RPOL[i].PTR)->ref_minus() == 0) delete RPOL[i].PTR;
                     }
                          for(int c=0; c<=P.degree(); c++)
                          {
                             if (!(P[c]).PTR) 
                             {
                               #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                         if(!(((real&) (P[c])).PTR = CURRENT_REAL_REPS->find((P[c]).value))) 
                                           ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                                         else ((real&) (P[c])).PTR->ref_plus();
                               #else
                                       ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                               #endif

                             }
                             (P[c].PTR)->ref_plus();
                          }

                     RPOL = P;
                  }
          }



          rounding_modes old_mode = bigfloat::get_rounding_mode();
          sz_t old_prec = bigfloat::get_precision();

          
            bigfloat b,a;
            if(first)
            {
              rational b_a = right - left;

              integer pnum = ilog2(b_a.numerator());
              integer pden = ilog2(b_a.denominator());
              long int_prec;
              int_prec = (2*(pden - pnum + 1)).to_long();
              if(int_prec < DOUBLE_PREC) int_prec = DOUBLE_PREC;


              a = div(bigfloat(left.numerator()), bigfloat(left.denominator()),
                      int_prec, TO_N_INF);
              b = div(bigfloat(right.numerator()), bigfloat(right.denominator()),
                      int_prec, TO_P_INF);

              first = false;
            }
            else
            {
              long int_prec = (-ilog2(error_bf())+2).to_long();
              b = add(num_bf(),error_bf(),int_prec,TO_P_INF);
              a = sub(num_bf(),error_bf(),int_prec,TO_N_INF);
            }

          
            bigfloat m, ma, mb;
            bigfloat minP, minPs;
            int deg;
            bool bisection;
            do {
              bisection = false;
              bigfloat::set_rounding_mode(TO_P_INF);
              if((-4*ilog2(b-a)).to_long() > long(bigfloat::get_precision()))
                    bigfloat::set_precision((-4*ilog2(b-a)).to_long());
              
                m = abs(a);
                if(m < abs(b)) m = abs(b);

              
                    bigfloat mi = m;  // m^i

                    ma = 1+m;
                    mb = 1;

                    deg = RPOL.degree();

                    for(int i=2; i<=deg; i++)
                    {
                      mb += i*mi;  
                      mi  = mi*m;
                      ma += mi;
                    } 
                    // ma = \sum_{i=0}^d m^i
                    // mb = \sum_{i=1}^d i*m^{i-1}

              bigfloat::set_rounding_mode(TO_N_INF);
              
                // min of P is at the endpoints:

                Polynomial RP= RPOL;

                int nRP = RP.degree();
                LEDA_VECTOR<real_interval> IV(nRP+1);
                for(int j=0; j<nRP; j++) 
                {
                   real hj = RP[j]/RP[nRP];
                   if(hj.is_double) IV[j] = real_interval(hj.value);
                   else IV[j] = (hj.PTR)->I;
                }
                IV[nRP] = real_interval(1);
                Poly_interval IRP(IV);


                real_interval fak;
                if(RP[nRP].is_double) fak = real_interval((RP[nRP]).value);
                else fak = (RP[nRP].PTR)->I;

                real_interval Ja = abs(fak) * abs(IRP.evaluate(real_interval(a)));
                real_interval Jb = abs(fak) * abs(IRP.evaluate(real_interval(b)));

                bigfloat bpa, bpb;

                if(Ja.is_finite() && Jb.is_finite())
                {
                   bpa = Ja.lower_bound();
                   bpb = Jb.lower_bound();
                }
                else
                {
                   real Pa = abs(RP.evaluate(real(a)));
                   real Pb = abs(RP.evaluate(real(b)));
                   bpa = Pa.get_lower_bound();
                   bpb = Pb.get_lower_bound();
                }

                minP = bpb;
                if(bpb > bpa) minP = bpa;

                // min of P'
                IRP.diff();

                // evaluate polynomial at the interval

                double da = (round(a,53,TO_N_INF)).to_double();
                double db = (round(b,53,TO_P_INF)).to_double();
                real_interval K;
                //K.set_range(real_interval(a).lower_bound(), real_interval(b).upper_bound());
                K.set_range(da,db);
                real_interval J = fak * IRP.evaluate(K);

                if(J.is_finite())
                {
                   // take the minimum
                   if(J.lower_bound() > 0) minPs = J.lower_bound();
                   else if(J.upper_bound() < 0 ) minPs = -J.upper_bound();
                   else // minPs = 0
                   {
                     // bisection
                     bigfloat h = (a+b)/2;
                     if(RPOL.evaluate(real(h))*RPOL.evaluate(real(a)) <0) b = h;
                     else a=h;
                     bisection = true;
                   }
                }
                else
                {
                   Polynomial RPs = diff(RP); 
                   real Psa = abs(RPs.evaluate(real(a)));
                   real Psb = abs(RPs.evaluate(real(b)));
                   bigfloat bpsa = Psa.get_lower_bound();
                   bigfloat bpsb = Psb.get_lower_bound();
                   if(bpsa > bpsb) minPs = bpsb;
                   else minPs = bpsa;

                   // get the derivative of RPs
                   Polynomial RPss = diff(RPs);

                   // make RPss squarefree
                   Polynomial RPsss = diff(RPss);
                   if(possible_common_roots(RPss,RPsss))
                   {
                      Polynomial G = poly_gcd(RPss,RPsss);
                      if(G.degree() > 0) 
                      {
                          polynomial<real> q,r;
                          RPss.euclidean_division(G,q,r);
                          RPss = q;
                      }
                   }

                   // get the exact roots of the derivative
                   LEDA_VECTOR<real> roots;
                   int n_roots = real_roots(RPss, roots);
                   
                   // check if there is a root in [a,b]
                   for(int i=0; i<n_roots; i++)
                   {
                       if(roots[i] >= real(a) && roots[i] <= real(b))
                       {
                          real e = abs(RPs.evaluate(roots[i]));
                          if(e.get_lower_bound()<minPs) minPs = e.get_lower_bound();
                       }
                   }
                }


            } while(bisection);

            integer q = p+2;
            if(ma >1) q += ilog2(ma);  // Condition c)

              integer q1;
              if(ma>0)
              {
                 q1 = ilog2(ma);
                 if(minP>0) q1 -= ilog2(minP);
                 if(q < q1) q = q1;
              }

              if(mb>0)
              {
                 q1 = ilog2(mb); 
                 if(minPs>0) q1 -= ilog2(minPs);
                 if(q < q1) q = q1;
              }
 

          
            LEDA_VECTOR<bigfloat> B(deg+1);
            for(int i=0; i<=deg; i++) 
            {
              (RPOL[i].PTR)->init_app_bf();
              (RPOL[i].PTR)->guarantee_bound_two_to(-q);
              B[i] = (RPOL[i].PTR)->num_bf();
            }
            Poly_bigfloat P(B);

          
            bigfloat en = ipow2(-p-1); 
            en = en/minPs;

            int r = (-ilog2(ipow2(-p)-en)+2).to_long();

          if(q.to_long() > long(bigfloat::get_precision())) 
                  bigfloat::set_precision(q.to_long());


               //long t1=0, t2=0;
               //t1 = clock();

            approximating N(P,a,b,r);
               //t2 = clock();
              //real::total += (t2 - t1);
              //cout << " t2-t1 = " << (float) (t2-t1) / CLOCKS_PER_SEC << " seconds" << std::endl << std::endl;
            num_bf() = N.approximation();
            error_bf() = add(ipow2(-r),en,ERROR_PREC, TO_INF);

          bigfloat::set_rounding_mode(old_mode);
          bigfloat::set_precision(old_prec);


          adjust_interval();
          return;
          break;
         }

   }
   if(CON != NEGATION) // ERROR MICHAEL 2
   {
		if (!isexact) operation_error = ldexp_bf(abs(num_bf()),-p);
		error_bf() = operation_error + operand_error;
   }
  if (exact()) {
                 delete_current_real_rep();
                 if (OP1)  if (OP1->ref_minus() == 0)  delete OP1;
                 if (OP2)  if (OP2->ref_minus() == 0)  delete OP2;
                 OP1 = OP2 = NULL;
                 CON = BIGFLOAT;
               #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                 CURRENT_REAL_REPS->insert(this, num_bf());
               #endif
               }



  adjust_interval(); 
}

  void real_rep::guarantee_bound_two_to(const integer& e)
  {  
    bool current_precision_is_sufficient =
      isZero(error_bf()) || (err_exp() <= e);
    if (current_precision_is_sufficient) 
      return;

    switch(CON) { 
      case DOUBLE:
      case BIGFLOAT: 
        return;
      case NEGATION: 
        OP1->guarantee_bound_two_to(e); 
        num_bf() = -OP1->num_bf();
        error_bf() = OP1->error_bf(); 
        break;
      case ADDITION:       improve_add(e);      break;
      case SUBTRACTION:    improve_sub(e);      break;
      case MULTIPLICATION: improve_mul(e);      break;
      case DIVISION:       improve_div(e);      break;
      case SQUAREROOT:     improve_sqrt(e);     break;
      case ROOT:           improve_root(e);     break;
      case RATIONAL:       improve_rational(e); break;

      case DIAMOND:        improve_diamond(e);  break;

    }

    if (exact()) 
      {
        delete_current_real_rep();
        if (OP1)  if (OP1->ref_minus() == 0)  delete OP1;
        if (OP2)  if (OP2->ref_minus() == 0)  delete OP2;
        OP1 = OP2 = NULL;
        CON = BIGFLOAT;
      #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
        CURRENT_REAL_REPS->insert(this, num_bf());
      #endif
      }



    if (!  ( exact() || (!isZero(num_bf()) && (num_exp() > err_exp())) ) 

) {
      integer system_bound;
	  compute_concrete_system_bound(system_bound);
#ifndef BFMSS_ONLY
      system_bound_computed = true;
#endif
      bool system_bound_is_reached = (e + 1 < system_bound );
      if (system_bound_is_reached) {
        
          I = 0;
          num_bf() = error_bf() = bigfloat::pZero;
          {
            delete_current_real_rep();
            if (OP1)  if (OP1->ref_minus() == 0)  delete OP1;
            if (OP2)  if (OP2->ref_minus() == 0)  delete OP2;
            OP1 = OP2 = NULL;
            CON = BIGFLOAT;
          #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
            CURRENT_REAL_REPS->insert(this, num_bf());
          #endif
	    }




#ifdef v_IS_LONG        
        sep_bound_ptr->U = sep_bound_ptr->L = zero_integer; sep_bound_ptr->v = 0;
#else        
        sep_bound_ptr->U = sep_bound_ptr->L = sep_bound_ptr->v = zero_integer; 
#endif        
      }
    }
    adjust_interval();

  }

void real_rep::improve_add(const integer& e) {
  long p;
  
    OP1->guarantee_bound_two_to(e-2);
    OP2->guarantee_bound_two_to(e-2);
    integer log_epsilon = -1;
    if ( !isZero(OP1->num_bf()) && !isZero(OP2->num_bf()) )
      log_epsilon = e - Maximum(OP1->num_exp(),OP2->num_exp()) - 2;
       if (!log_epsilon.is_long())
         HANDLE_ERROR(1,"sorry:improve:computation to expensive");
       p = -log_epsilon.to_long();
       if (p < 2)
         p = 2;

    
  bool isexact=false;
  num_bf() = add(OP1->num_bf(),OP2->num_bf(),p,TO_NEAREST,isexact);
     
  if ( isexact && OP1->exact() && OP2->exact() )
    error_bf() = bigfloat::pZero;   
  else
    error_bf() = ipow2(e);
    

}

void real_rep::improve_sub(const integer& e) {
  long p;
  
    OP1->guarantee_bound_two_to(e-2);
    OP2->guarantee_bound_two_to(e-2);
    integer log_epsilon = -1;
    if ( !isZero(OP1->num_bf()) && !isZero(OP2->num_bf()) )
      log_epsilon = e - Maximum(OP1->num_exp(),OP2->num_exp()) - 2;
       if (!log_epsilon.is_long())
         HANDLE_ERROR(1,"sorry:improve:computation to expensive");
       p = -log_epsilon.to_long();
       if (p < 2)
         p = 2;

    
  bool isexact=false;
  num_bf() = sub(OP1->num_bf(),OP2->num_bf(),p,TO_NEAREST,isexact);
     
  if ( isexact && OP1->exact() && OP2->exact() )
    error_bf() = bigfloat::pZero;   
  else
    error_bf() = ipow2(e);
    

}
  
void real_rep::compute_mul_error(bigfloat& operand_error)
{
  bigfloat x = OP1->num_bf();
  bigfloat y = OP2->num_bf();
  bigfloat error_x 
    = mul(abs(round(x,ERROR_PREC,TO_INF)),OP2->error_bf(),ERROR_PREC,TO_INF);
  bigfloat y_high  
    = add(abs(round(y,ERROR_PREC,TO_INF)),OP2->error_bf(),ERROR_PREC,TO_INF);
  bigfloat error_y = mul(y_high,OP1->error_bf(),ERROR_PREC,TO_INF);
  operand_error = add(error_x,error_y,ERROR_PREC,TO_INF);
}

void real_rep::improve_mul(const integer& e)
{
  bigfloat y_high = add(abs(OP2->num_bf()),OP2->error_bf(),ERROR_PREC,TO_P_INF);
  if (!isZero(y_high)) {
    integer ex = e - ilog2(y_high) - 2;
    OP1->guarantee_bound_two_to(ex);
  }
  integer log_epsilon = -1;
  if (!isZero(OP1->num_bf()))
  {
    integer ey = e - OP1->num_exp() - 2;
    OP2->guarantee_bound_two_to(ey);
    if (!isZero((OP2->num_bf())))
      log_epsilon = e - (OP1->num_exp()+OP2->num_exp()) - 1;
  }
  bool isexact=false;
  long p;
     if (!log_epsilon.is_long())
       HANDLE_ERROR(1,"sorry:improve:computation to expensive");
     p = -log_epsilon.to_long();
     if (p < 2)
       p = 2;

  num_bf() = mul(OP1->num_bf(),OP2->num_bf(),p,TO_NEAREST,isexact);
     
  if ( isexact && OP1->exact() && OP2->exact() )
    error_bf() = bigfloat::pZero;   
  else
    error_bf() = ipow2(e);
    

}


void real_rep::compute_div_error(bigfloat& operand_error)
{
  bigfloat y_low = sub(abs(OP2->num_bf()),OP2->error_bf(),ERROR_PREC,TO_N_INF);
  bigfloat x = OP1->num_bf();
  bigfloat t = mul(abs(round(x,ERROR_PREC)),OP2->error_bf(),ERROR_PREC,TO_INF);
           t = ldexp_bf(t,1-OP2->num_exp());
  operand_error = add(t,abs(OP1->error_bf()),ERROR_PREC,TO_INF);
  operand_error = ldexp_bf(operand_error,1-ilog2(y_low));
}

void real_rep::improve_div(const integer& e)
{
  bigfloat y_low = sub(abs(OP2->num_bf()),OP2->error_bf(),ERROR_PREC,TO_N_INF);
  integer ex = e + ilog2(y_low) - 3;
  OP1->guarantee_bound_two_to(ex);
  integer log_epsilon = -1;
  if (!isZero(OP1->num_bf()))
  {
    integer ey = ex  + OP2->num_exp() - OP1->num_exp() - 2;
    OP2->guarantee_bound_two_to(ey);
    log_epsilon = e + OP2->num_exp() - OP1->num_exp() - 2;
  }
  bool isexact=false;
  long p;
     if (!log_epsilon.is_long())
       HANDLE_ERROR(1,"sorry:improve:computation to expensive");
     p = -log_epsilon.to_long();
     if (p < 2)
       p = 2;

  num_bf() = div(OP1->num_bf(),OP2->num_bf(),p,TO_NEAREST,isexact);
     
  if ( isexact && OP1->exact() && OP2->exact() )
    error_bf() = bigfloat::pZero;   
  else
    error_bf() = ipow2(e);
    

}

void real_rep::compute_sqrt_error(bigfloat& operand_error)
{  
	
  integer log_sqrt=0;
  if (!isZero(num_bf()))
	  log_sqrt = num_exp() -1;
    //log_sqrt = (num_exp()>>1)-1;
  if (!OP1->exact())
    operand_error = ldexp_bf(OP1->error_bf(),-log_sqrt); 

	// Michael
}


void real_rep::improve_sqrt(const integer& e)
{
  bool isexact=false; 
  bigfloat x_low  = sub(OP1->num_bf(),OP1->error_bf(),ERROR_PREC,TO_N_INF);
  integer ex = e + (ilog2(x_low)-1)/2 - 1;
  OP1->guarantee_bound_two_to(ex);
  bigfloat x_high = add(OP1->num_bf(),OP1->error_bf(),ERROR_PREC,TO_P_INF);
  integer log_epsilon = e - (ilog2(x_high)+1)/2 - 1;
  long p=1;
     if (!log_epsilon.is_long())
       HANDLE_ERROR(1,"sorry:improve:computation to expensive");
     p = -log_epsilon.to_long();
     if (p < 2)
       p = 2;

  num_bf() = sqrt_bf(OP1->num_bf(),p,TO_NEAREST,isexact,num_bf());
  if ( isexact && OP1->exact() )  
    error_bf() = bigfloat::pZero; 
  else  
    error_bf() = ipow2(e);
}

void real_rep::compute_root_error(bigfloat& operand_error)
{  
  bigfloat x_low   = OP1->num_bf()-OP1->error_bf();
  integer log_root = 1+(ilog2(x_low)*(1-d))/d;
  operand_error    = ldexp_bf(OP1->error_bf(),log_root-ilog2(integer(d))+1); 
}


void real_rep::improve_root(const integer& e)
{
  bigfloat x_low  = sub(OP1->num_bf(),OP1->error_bf(),ERROR_PREC,TO_N_INF);
  integer ex = e + ilog2(integer(d)) + ((ilog2(x_low)-1)*(d-1))/d - 2;
  OP1->guarantee_bound_two_to(ex);
  bigfloat x_high = add(OP1->num_bf(),OP1->error_bf(),ERROR_PREC,TO_P_INF);
  integer log_epsilon = e - ilog2(x_high)/d - 2;
  long p;
     if (!log_epsilon.is_long())
       HANDLE_ERROR(1,"sorry:improve:computation to expensive");
     p = -log_epsilon.to_long();
     if (p < 2)
       p = 2;

  num_bf() = sqrt_d(OP1->num_bf(),p,d);
  error_bf() = ipow2(e);
}

void real_rep::improve_rational(const integer& e) {
  long p;
  integer log_epsilon = -1;
  if ( rat() != 0 )
    log_epsilon = e + (- log2_abs(rat().numerator()) + log(rat().denominator()));
  if (!log_epsilon.is_long())
    HANDLE_ERROR(1,"sorry:improve:computation to expensive");
  p = -log_epsilon.to_long();
  if (p < 2)
    p = 2;

  bool isexact = false;
  num_bf() = convert_to_bf(rat(),p,TO_NEAREST,isexact);
     
  if ( isexact )
    error_bf() = bigfloat::pZero;   
  else
    error_bf() = ipow2(e);
}



void real_rep::improve_diamond(const integer& e)
{
  integer log_epsilon = e;
  long p;
     if (!log_epsilon.is_long())
       HANDLE_ERROR(1,"sorry:improve:computation to expensive");
     p = -log_epsilon.to_long();
     if (p < 2)
       p = 2;


  
  if(first)
  {
          Polynomial P = RPOL;
          if(refine(P,left, right, pol_squarefree)) // root found exactly
          {
             if(P.degree() <= 2 && left != right) 
             {
                if(P.degree() == 1)
                {
                   CON = DIVISION;
                   real x(-P[0]);
                   real y(P[1]);

                     if (!x.PTR) 
                     {
                       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                           if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
                             ((real&) x).PTR = new real_rep(x.value);
                           else ((real&) x).PTR->ref_plus();
                       #else
                           ((real&) x).PTR = new real_rep(x.value);
                       #endif
                     }
                     if (!y.PTR) 
                     {
                       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                           if(!(((real&) y).PTR = CURRENT_REAL_REPS->find(y.value))) 
                             ((real&) y).PTR = new real_rep(y.value);
                           else ((real&) y).PTR->ref_plus();
                       #else
                           ((real&) y).PTR = new real_rep(y.value);
                       #endif
                     }


                   #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                    real_rep* z_rep = CURRENT_REAL_REPS->find(CON,(x.PTR)->ref(), (y.PTR)->ref());
                                    if(z_rep != NULL)
                                    {
                                       z_rep->OPOL = OPOL;
                                       z_rep->ref_plus();
                                       ((real_rep&) *this) = *z_rep;
                                       compute_op(p);
                                       return;
                                    }
                   #endif

                   OP1 = x.PTR;
                   OP2 = y.PTR;
                   OP1->ref_plus();  
                   OP2->ref_plus();
                   is_general_ = false;
                   is_rational_ = true;
                   compute_op(p);

                   #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                    CURRENT_REAL_REPS->insert(CON, this, (x.PTR)->ref(), (y.PTR)->ref());
                   #endif

                   return;
                }
                else // P.degree()==2
                {
                   real disc = real(P[1]*P[1] - 4 * P[2]*P[0]);
                   real x = sqrt(disc)/real(2*P[2]);
                   real y = real((-P[1])/(2*P[2]));

                     if (!x.PTR) 
                     {
                       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                           if(!(((real&) x).PTR = CURRENT_REAL_REPS->find(x.value))) 
                             ((real&) x).PTR = new real_rep(x.value);
                           else ((real&) x).PTR->ref_plus();
                       #else
                           ((real&) x).PTR = new real_rep(x.value);
                       #endif
                     }
                     if (!y.PTR) 
                     {
                       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                           if(!(((real&) y).PTR = CURRENT_REAL_REPS->find(y.value))) 
                             ((real&) y).PTR = new real_rep(y.value);
                           else ((real&) y).PTR->ref_plus();
                       #else
                           ((real&) y).PTR = new real_rep(y.value);
                       #endif
                     }



                   if(left <= x-y && x-y <= right) CON = SUBTRACTION;
                   else CON = ADDITION;

                   #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                    real_rep* z_rep = CURRENT_REAL_REPS->find(CON,(x.PTR)->ref(), (y.PTR)->ref());
                                    if(z_rep != NULL)
                                    {
                                       z_rep->OPOL = OPOL;
                                       z_rep->ref_plus();
                                       ((real_rep&) *this) = *z_rep;
                                       compute_op(p);
                                       return;
                                    }
                   #endif
                   
                   OP1 = x.PTR;
                   OP2 = y.PTR;
                   OP1->ref_plus();  
                   OP2->ref_plus();
                   is_general_ = false;
                   is_rational_ = false;
                   compute_op(p);

                   #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                    CURRENT_REAL_REPS->insert(CON, this, (x.PTR)->ref(), (y.PTR)->ref());
                   #endif
                   return;
                }
             }
             else
             {
                num_bf() = to_bigfloat(real(left));
                error_bf() = 0;
                return;
             }
          }
          if(RPOL.degree()>P.degree())
          {
             // refine changes the coefficients
             int n = RPOL.degree();
             for(int i=0; i<=n; i++) 
             {
               if ((RPOL[i].PTR)->ref_minus() == 0) delete RPOL[i].PTR;
             }
                  for(int c=0; c<=P.degree(); c++)
                  {
                     if (!(P[c]).PTR) 
                     {
                       #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                                 if(!(((real&) (P[c])).PTR = CURRENT_REAL_REPS->find((P[c]).value))) 
                                   ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                                 else ((real&) (P[c])).PTR->ref_plus();
                       #else
                               ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                       #endif

                     }
                     (P[c].PTR)->ref_plus();
                  }

             RPOL = P;
          }
  }



  rounding_modes old_mode = bigfloat::get_rounding_mode();
  sz_t old_prec = bigfloat::get_precision();

  
    bigfloat b,a;
    if(first)
    {
      rational b_a = right - left;

      integer pnum = ilog2(b_a.numerator());
      integer pden = ilog2(b_a.denominator());
      long int_prec;
      int_prec = (2*(pden - pnum + 1)).to_long();
      if(int_prec < DOUBLE_PREC) int_prec = DOUBLE_PREC;


      a = div(bigfloat(left.numerator()), bigfloat(left.denominator()),
              int_prec, TO_N_INF);
      b = div(bigfloat(right.numerator()), bigfloat(right.denominator()),
              int_prec, TO_P_INF);

      first = false;
    }
    else
    {
      long int_prec = (-ilog2(error_bf())+2).to_long();
      b = add(num_bf(),error_bf(),int_prec,TO_P_INF);
      a = sub(num_bf(),error_bf(),int_prec,TO_N_INF);
    }

  
    bigfloat m, ma, mb;
    bigfloat minP, minPs;
    int deg;
    bool bisection;
    do {
      bisection = false;
      bigfloat::set_rounding_mode(TO_P_INF);
      if((-4*ilog2(b-a)).to_long() > long(bigfloat::get_precision()))
            bigfloat::set_precision((-4*ilog2(b-a)).to_long());
      
        m = abs(a);
        if(m < abs(b)) m = abs(b);

      
            bigfloat mi = m;  // m^i

            ma = 1+m;
            mb = 1;

            deg = RPOL.degree();

            for(int i=2; i<=deg; i++)
            {
              mb += i*mi;  
              mi  = mi*m;
              ma += mi;
            } 
            // ma = \sum_{i=0}^d m^i
            // mb = \sum_{i=1}^d i*m^{i-1}

      bigfloat::set_rounding_mode(TO_N_INF);
      
        // min of P is at the endpoints:

        Polynomial RP= RPOL;

        int nRP = RP.degree();
        LEDA_VECTOR<real_interval> IV(nRP+1);
        for(int j=0; j<nRP; j++) 
        {
           real hj = RP[j]/RP[nRP];
           if(hj.is_double) IV[j] = real_interval(hj.value);
           else IV[j] = (hj.PTR)->I;
        }
        IV[nRP] = real_interval(1);
        Poly_interval IRP(IV);


        real_interval fak;
        if(RP[nRP].is_double) fak = real_interval((RP[nRP]).value);
        else fak = (RP[nRP].PTR)->I;

        real_interval Ja = abs(fak) * abs(IRP.evaluate(real_interval(a)));
        real_interval Jb = abs(fak) * abs(IRP.evaluate(real_interval(b)));

        bigfloat bpa, bpb;

        if(Ja.is_finite() && Jb.is_finite())
        {
           bpa = Ja.lower_bound();
           bpb = Jb.lower_bound();
        }
        else
        {
           real Pa = abs(RP.evaluate(real(a)));
           real Pb = abs(RP.evaluate(real(b)));
           bpa = Pa.get_lower_bound();
           bpb = Pb.get_lower_bound();
        }

        minP = bpb;
        if(bpb > bpa) minP = bpa;

        // min of P'
        IRP.diff();

        // evaluate polynomial at the interval

        double da = (round(a,53,TO_N_INF)).to_double();
        double db = (round(b,53,TO_P_INF)).to_double();
        real_interval K;
        //K.set_range(real_interval(a).lower_bound(), real_interval(b).upper_bound());
        K.set_range(da,db);
        real_interval J = fak * IRP.evaluate(K);

        if(J.is_finite())
        {
           // take the minimum
           if(J.lower_bound() > 0) minPs = J.lower_bound();
           else if(J.upper_bound() < 0 ) minPs = -J.upper_bound();
           else // minPs = 0
           {
             // bisection
             bigfloat h = (a+b)/2;
             if(RPOL.evaluate(real(h))*RPOL.evaluate(real(a)) <0) b = h;
             else a=h;
             bisection = true;
           }
        }
        else
        {
           Polynomial RPs = diff(RP); 
           real Psa = abs(RPs.evaluate(real(a)));
           real Psb = abs(RPs.evaluate(real(b)));
           bigfloat bpsa = Psa.get_lower_bound();
           bigfloat bpsb = Psb.get_lower_bound();
           if(bpsa > bpsb) minPs = bpsb;
           else minPs = bpsa;

           // get the derivative of RPs
           Polynomial RPss = diff(RPs);

           // make RPss squarefree
           Polynomial RPsss = diff(RPss);
           if(possible_common_roots(RPss,RPsss))
           {
              Polynomial G = poly_gcd(RPss,RPsss);
              if(G.degree() > 0) 
              {
                  polynomial<real> q,r;
                  RPss.euclidean_division(G,q,r);
                  RPss = q;
              }
           }

           // get the exact roots of the derivative
           LEDA_VECTOR<real> roots;
           int n_roots = real_roots(RPss, roots);
           
           // check if there is a root in [a,b]
           for(int i=0; i<n_roots; i++)
           {
               if(roots[i] >= real(a) && roots[i] <= real(b))
               {
                  real e = abs(RPs.evaluate(roots[i]));
                  if(e.get_lower_bound()<minPs) minPs = e.get_lower_bound();
               }
           }
        }


    } while(bisection);

    integer q = p+2;
    if(ma >1) q += ilog2(ma);  // Condition c)

      integer q1;
      if(ma>0)
      {
         q1 = ilog2(ma);
         if(minP>0) q1 -= ilog2(minP);
         if(q < q1) q = q1;
      }

      if(mb>0)
      {
         q1 = ilog2(mb); 
         if(minPs>0) q1 -= ilog2(minPs);
         if(q < q1) q = q1;
      }
 

  
    LEDA_VECTOR<bigfloat> B(deg+1);
    for(int i=0; i<=deg; i++) 
    {
      (RPOL[i].PTR)->init_app_bf();
      (RPOL[i].PTR)->guarantee_bound_two_to(-q);
      B[i] = (RPOL[i].PTR)->num_bf();
    }
    Poly_bigfloat P(B);

  
    bigfloat en = ipow2(-p-1); 
    en = en/minPs;

    int r = (-ilog2(ipow2(-p)-en)+2).to_long();

  if(q.to_long() > long(bigfloat::get_precision())) 
          bigfloat::set_precision(q.to_long());


       //long t1=0, t2=0;
       //t1 = clock();

    approximating N(P,a,b,r);
       //t2 = clock();
      //real::total += (t2 - t1);
      //cout << " t2-t1 = " << (float) (t2-t1) / CLOCKS_PER_SEC << " seconds" << std::endl << std::endl;
    num_bf() = N.approximation();
    error_bf() = add(ipow2(-r),en,ERROR_PREC, TO_INF);

  bigfloat::set_rounding_mode(old_mode);
  bigfloat::set_precision(old_prec);



}



bool real::is_general() const {
  if(is_double) return false;
  else return PTR->is_general();
}

bool real_rep::is_general() const {
  return is_general_;
}

bool real::is_rational() const {
  if(is_double) return true;
  else return PTR->is_rational();
}

bool real_rep::is_rational() const {
  return is_rational_;
}

rational real::to_rational() const {
  if(is_double) return (rational(value)).normalize();
  else return (PTR->to_rational()).normalize();
}

// we assume here that is_rational() is true and that we have no roots
rational real_rep::to_rational() {
   switch(CON){
      case DOUBLE: 
      case BIGFLOAT:
        init_app_bf();
        return PACKAGE_SCOPE to_rational(num_bf());
      case NEGATION:
        return -(OP1->to_rational());
      case ADDITION:
        return ((OP1->to_rational()) + (OP2->to_rational()));
      case SUBTRACTION:
        return ((OP1->to_rational()) - (OP2->to_rational()));
      case MULTIPLICATION:
        return ((OP1->to_rational()) * (OP2->to_rational()));
      case DIVISION:
        return ((OP1->to_rational()) / (OP2->to_rational()));
      case RATIONAL:
        return rat();
      default:
        HANDLE_ERROR(1, "real_rep::to_rational: forbidden CON");
        return rational();
   }
}


rat_Polynomial real::get_polynomial() const
{
  if(!PTR) 
  {
     return rat_Polynomial(rational(1,1), -(rational(value)).normalize());
  }
  return PTR->get_polynomial(); 
}

rat_Polynomial real_rep::get_polynomial()
{
  if(RPOL.degree() == 0)
  {
     if(is_rational())
     {
                return rat_Polynomial(rational(1,1), -to_rational());
     }
  }
  int n = RPOL.degree();
  LEDA_VECTOR<rational> V(n+1);
  for(int i=0; i<=n; i++) 
  {
     if(!(RPOL[i].PTR)->is_rational()) return rat_Polynomial();
     V[i] = ((RPOL[i].PTR)->to_rational()).normalize();
  }
  return rat_Polynomial(V);
}

void real::set_polynomial(const Polynomial& P)
{
  if(!PTR)
  { 
#ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
     if(!(PTR = CURRENT_REAL_REPS->find(value))) PTR = new real_rep(value);
     else PTR->ref_plus();
#else
     PTR = new real_rep(value);
#endif
  }
  PTR->set_polynomial(P); 
}

void real_rep::set_polynomial(const Polynomial& P)
{
#ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
  int n = RPOL.degree();
  if(n>0 || !RPOL.is_zero())
  {
     for(int i=0; i<=n; i++) 
     {
        if ((RPOL[i].PTR)->ref_minus() == 0) delete RPOL[i].PTR; 
     }
  }
#endif

       for(int c=0; c<=P.degree(); c++)
       {
          if (!(P[c]).PTR) 
          {
            #ifndef DONOT_SEARCH_FOR_COMMON_EXPRESSIONS
                      if(!(((real&) (P[c])).PTR = CURRENT_REAL_REPS->find((P[c]).value))) 
                        ((real&) (P[c])).PTR = new real_rep((P[c]).value);
                      else ((real&) (P[c])).PTR->ref_plus();
            #else
                    ((real&) (P[c])).PTR = new real_rep((P[c]).value);
            #endif

          }
      // (P[c].PTR)->ref_plus(); Fehler Maerz 2006: erzeugt memory leak. 
      // Brauche ich das? Ich glaube nicht. RPOL=P macht eine Zuweisung der
      // Koeffizienten. Dabei wird der operator= von real aufgerufen. Der macht
      // ein ref_plus(), falls der PTR da ist, was wir ja hier garantieren.

       }

  RPOL = P;


#ifndef BFMSS_ONLY
  // integral polynomial?
  int m = RPOL.degree();
  is_integral_pol = true;
  for(int i=0; i<=m; i++)
  { 
     if(!(RPOL[i].PTR)->is_integral_pol)
     {
        is_integral_pol = false;
        break;
     }
  }
  degree_measure_possible = is_integral_pol;
#endif        
     
}


int sign(const real& x) {  return x.sign();  }
bool sign_is_known(const real& x) { return true; }
real abs(const real& x) { if (x.sign() < 0) return -x; else return x;  }
real sqr(const real& x)  {  return x*x;  } 
real dist(const real& x, const real& y)  {  return sqrt(x*x+y*y);  } 
real powi(const real& x, int n) { 
  real y = x, z = 1;
  int n_prefix = n;
  while (n_prefix > 0) {
    if (n_prefix % 2) z = z*y;
    n_prefix = n_prefix / 2;
    y = y*y;
  }
  return z;
}

integer floor(const real& x)
{
   bigfloat bx = to_bigfloat(x) - x.get_bigfloat_error();
   integer fx = floor(bx);
   while(x-fx >= 1) fx++;
   return fx;
}

integer ceil(const real& x)
{
   bigfloat bx = to_bigfloat(x) + x.get_bigfloat_error();
   integer cx = ceil(bx);
   while(cx-x >= 1) cx--;
   return cx;
}

rational small_rational_between(const real& X, const real& Y)
{  
  // This procedure is similar to Figures 6, 7 in the paper by 
  // Canny et al:
  // "A Rational Rotation Method for Robust Geometric Algorithms"
  // (Proc. of the 8th ACM Symposium on Computational Geometry, 
  //  pages 251-260, 1992)

  if (Y <= X) 
  {
    HANDLE_ERROR(1, "real::small_rational_between: precondition violated");
    return floor(Y);
  }

  rational floor_y = floor(Y);

  if (floor_y >= X) return floor_y;
 
  integer p0 = 0;
  integer q0 = 1;
  integer p1 = 1;
  integer q1 = 1;
  integer r;

  real x = X-floor_y;
  real y = Y-floor_y;
  rational result;

  for(;;) {  

    // invariant (I):    0 <= p0/q0 < x <= y < p1/q1 <= 1

    r = floor((real(p1)-real(q1)*x)/(real(q0)*x-real(p0)));
    p1 = r*p0+p1; 
    q1 = r*q0+q1;
    if ( rational(p1,q1) <= y)
    { result = rational(p1+q1*floor(Y),q1); 
      break;
    }

    // Invariant (I) from above holds again 

    r = floor((real(q0)*y-real(p0))/(real(p1)-real(q1)*y));
    p0 = r*p1+p0; 
    q0 = r*q1+q0;
    if (rational(p0,q0) >= x)
    { result = rational(p0+q0*floor(Y),q0);
      break;
    }
  } 
  
  if ((result > Y) || (result < X))
         HANDLE_ERROR(1, "real::small_rational_between: internal error.");

  return result;

}   

rational small_rational_near(const real& X, double epsilon)
{  
  // returns a small rational between X-epsilon and X+epsilon
  // precondition: epsilon > 0

  if (epsilon <= 0)
     HANDLE_ERROR(1, "real::small_rational_near: precondition violated");

  return small_rational_between(X-epsilon,X+epsilon);
}





LEDA_END_NAMESPACE


