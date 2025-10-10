/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _residual.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/numbers/digit_vector.h>
#include <LEDA/numbers/residual.h>
#include <LEDA/numbers/residual_tables.h>
#include <LEDA/system/assert.h>

#include <stdlib.h>

LEDA_BEGIN_NAMESPACE

double residual::mod::inverse(double x, double p)
{

#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(0 < x && x < p);
  assert(x == floor(x));
#endif

  double u2,u3, v2,v3, t2,t3; 
  // no need for Knuth's u1, v1, and t1
  double q;

  // step X1
  u2=0; u3=p; //u3=p;
  v2=1; v3=x; //v3=x;

  // step X2
  while( v3!=0 ) {  
    // step X3 
    q=mod::rounded_to_integral(u3/v3);
    t2=u2-q*v2; t3=u3-q*v3;
    u2=v2; u3=v3;
    v2=t2; v3=t3;
  }
  u2=smod::reduce(u2,p);
  if(u2<0) u2+=p;

#ifdef RESIDUAL_CHECK_POSTCONDITIONS
  assert(mod::reduce_of_positive(u2*x,p)==1);
#endif
  return u2;
}



bool residual::mod_system::is_prime(int prime)
{
  if (prime%2 == 0) return false;
  int root = (int) ::sqrt(double(prime));
  int k;
  for (k=3; k<=root; k+=2)
    if (prime%k == 0) return false;
  return true;
}

int residual::mod_system::compute_prime_bounded_by(int a)
{
  if (a < 2)
    LEDA_EXCEPTION
      (1,"mod_system cannot compute prime smaller than 2");
  while(1) {
    if (is_prime(a))
      return a;
    a--;
  }
}

int residual::mod_system::compute_prime_with_bit_size(int b)
{
  if ((b < 2) || (b > 31))
    LEDA_EXCEPTION(1,"compute_prime: prime length must be in [2,...31]"); 
  return compute_prime_bounded_by((1<<b)-1);
}

void residual::mod_system::compute_prime_sequence_bounded_by
(int a, residual_sequence& p)
{
  int i=0;
  p[0] = compute_prime_bounded_by(a);
  for (i=1;i<p.size();i++)
    p[i] = compute_prime_bounded_by(int(p[i-1])-1);
}

void residual::mod_system::compute_prime_sequence_with_bit_size
(int b, residual_sequence& p)
{
  int i=0;
  p[0] = compute_prime_with_bit_size(b);
  for (i=1;i<p.size();i++)
    p[i] = compute_prime_bounded_by(int(p[i-1])-1);
}

void residual::mod_system::write_table_to_file
(const char* filename, residual_sequence& p, int b)
{
  ofstream table(filename);
  table << b << "\n";
  int k = p.size();
  table << k << "\n";
  int i=0;
  for (i=0;i<k;i++) {
    if (i%12==0)
      table << "\n";
    table << int(p[i]) << " ";
  }
  table << "\n";
}

void residual::mod_system::read_table_from_file (const char* fname, 
                                                 residual_sequence& table, 
                                                 int& _primelength,                                                              int& _tablesize)
{
  const char* lroot = getenv("LEDAROOT");

  if (lroot == NULL) 
      LEDA_EXCEPTION(1,"residual: Cannot read tables (LEDAROOT undefined).");

  string filename = lroot;
  filename += fname;

  ifstream tablefile(filename);

  if (!tablefile)
   LEDA_EXCEPTION(1,string("residual: Cannot read table $LEDAROOT%s.",fname));

  tablefile >> _primelength;
  tablefile >> _tablesize;
  int k = table.size();
  if (_tablesize<k)
    k=_tablesize;

  int i=0;
  while(tablefile >> table[i]) {
    i++;
    if (i==k)
      break;
  }
}


void residual::mod_system::insert_garner_constant
(int i, residual_sequence& u, const residual_sequence& p)
{
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(0<=i);
  assert(i<=u.size()&&i<=p.size());
#endif
  double M=1;
  int j;
  for (j=0;j<i;j++)
    M = mod::reduce_of_positive(M*p[j],p[i]);
  u[i] = smod::inverse(M,p[i]);
}

void residual::mod_system::compute_garner_constants
(int k, residual_sequence& u, const residual_sequence& p)
{
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(0<=k);
  assert(k<=u.size()&&k<=p.size());
#endif
  int i;
  u[0]=1;
  for (i=1;i<k;i++)
    insert_garner_constant(i,u,p);
}

void residual::mod_system::write_garner_table_to_file
(char* filename, int b, int k, residual_sequence& p)
{
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(0<=k&&k<=p.size());
#endif
  ofstream garnerfile(filename);
  garnerfile << b << "\n";
  garnerfile << k << "\n";
  residual_sequence u(k);
  int i=0;
  compute_garner_constants(k,u,p);
  for (i=0;i<k;i++) {
    if (i%12==0)
      garnerfile << "\n";
    garnerfile << int(u[i]) << " ";
  }
}


void residual::mod_system::compute_lagrange_w
(residual_sequence& w, const residual_sequence& p)
{
  int k=w.size();
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(k<=p.size());
#endif
  double mod;
  int i,j;
  for (i=0;i<k;i++)
    {
      mod=1;
      for (j=0; j<k; j++)
	if (j!=i)
	  mod=smod::reduce(mod*p[j],p[i]);
      w[i]=smod::inverse(mod,p[i]);
    }
}

void residual::mod_system::compute_lagrange_alpha
(residual_sequence& al, const residual_sequence& p)
{
  int k=al.size();
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(k<=p.size());
#endif
  int i;
  double epsilon = (3*(k-1))*ldexp(1.0,-53);
  // this is evaluated exactly
  double Alpha = floor(1/(4*epsilon))-1;
  // the error in this computation is compensated by subtracting 1
  for (i=0;i<k;i++)
    al[i] = smod::reduce(Alpha,p[i]);
}


void residual::mod_system::compute_W_matrix
(residual_matrix& W, const residual_sequence& p)
{
  int k = W.rowsize();
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(k==W.colsize());
  assert(k<=p.size());
#endif
  compute_lagrange_w(W[k-1],p);
  int i,j;
  for (i=0;i<k;i++)
    for (j=k-2;j>=i;j--)
      W[j][i]=smod::reduce(W[j+1][i]*p[j+1],p[i]);
}




void residual::mod_system::long2residual
(long a, residual_sequence& x, const residual_sequence& p)
{
  int i;
  int k = x.size();
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(k<=p.size());
#endif
  if (a >= 0) {
    if (a<ldexp(1.0,25))
      for (i=0;i<k;i++)
      x[i]=a;
    else
      for (i=0;i<k;i++)
      x[i]=mod::reduce_of_positive(a,p[i]);
  }
  else {
    if (-a<ldexp(1.0,25))
      for (i=0;i<k;i++)
      x[i]=p[i]+a;
    else 
      for (i=0;i<k;i++)
        x[i]=mod::reduce_of_negative(a,p[i]);
  }
}


void residual::mod_system::intdouble2residual
(double a, residual_sequence& x, const residual_sequence& p)
{
  int k = x.size();
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(a==floor(a));
#endif
    
  int i;
  if (a >= 0) {
    if (a<ldexp(1.0,25))
      for (i=0;i<k;i++)
        x[i]=a;
    else
      for (i=0;i<k;i++)
        x[i]=mod::reduce_of_positive(a,p[i]);
  }
  else {
    if (-a<ldexp(1.0,25))
      for (i=0;i<k;i++)
        x[i]=p[i]+a;
    else
      for (i=0;i<k;i++)
        x[i]=mod::reduce_of_negative(a,p[i]);
  }
}


double residual::mod_system::binary2mod
(double prime, int length, const digit* a, int sign)
{
  double mod, modbase;

  int j;
#ifndef WORD_LENGTH_64
  mod=residual::mod::reduce_of_positive(a[length-1],prime);
  modbase=residual::mod::reduce_of_positive(digit_base,prime);
  for (j = length-2; j>=0;j--)
    mod=residual::mod::reduce_of_positive(mod*modbase+a[j],prime);
#else
  mod=residual::mod::reduce_of_integral(a[length-1],prime);
  modbase=residual::mod::reduce_of_positive(double(HALF_BASE),prime);
  modbase=residual::mod::mul(modbase,modbase,prime);
  double ajmod; 
  for (j = length-2; j>=0;j--) {
    ajmod = residual::mod::reduce_of_integral(a[j],prime);
    mod=residual::mod::reduce_of_integral((digit)(mod*modbase+ajmod),prime);
  }
#endif
  if (sign==-1)
    mod=residual::mod::negate(mod,prime);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
  assert(0<=mod && mod<prime);
#endif
  return mod;
}

double residual::mod_system::integer2mod(const integer& a, double prime)
{
  return
  binary2mod(prime,a.used_words(),a.word_vector(),a.sign());
}

double residual::mod_system::bigbinary2mod
(double prime, int length, const residual_sequence& a48, 
int sign, double mod48)
{
  double mo;
  int j;
  mo=mod::reduce_of_positive(a48[length-1],prime);
  for (j = length-2; j>=0;j--)
    mo=mod::reduce_of_positive(mo*mod48+a48[j],prime);
  if (sign==-1)
    mo=mod::negate(mo,prime);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
  assert(0<=mo && mo<prime);
#endif
  return mo;
}


void residual::mod_system::binary2residual
(residual_sequence& x, const residual_sequence& p, 
 int a_length, const digit* a, int sign,
 residual_sequence& a48, const residual_sequence& mod48)
{
  int i;
  int k = x.size();
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(k<= p.size());
  assert(k<= a48.size());
  assert(k<= mod48.size());
#endif

#ifdef WORD_LENGTH_32
  double pow16=ldexp(1.0,16);
  int index=-1;
  digit lower_half, upper_half;
  for (i=0; i<a_length;i++) {
    switch(i%3) {
      // create 2x48 bit from 3x32
      case 0:
        index++;
        a48[index]=a[i];
        break;
      case 1:
        lower_half=a[i]<<16;
        upper_half=a[i]>>16;
        a48[index]+=lower_half*pow16;
        index++;
        a48[index]=upper_half;
        break;
      case 2:
        a48[index]+=a[i]*pow16;
    }
  }
  for (i=0; i<k; i++)
    x[i] = bigbinary2mod(p[i],index+1,a48,sign,mod48[i]);
#else
  for (i=0; i<k; i++) 
    x[i] = binary2mod(p[i],a_length,a,sign);
#endif
}



void residual::mod_system::integer2residual
(const integer& a, residual_sequence& x, const residual_sequence& p, 
 residual_sequence& a48, const residual_sequence& mod48)
{
  int k = x.size();
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(k<= p.size());
  assert(k<= a48.size());
  assert(k<= mod48.size());
#endif

  int i;
  int sgn = a.sign();
  if (sgn==0) {
    for (i=0;i<k;i++)
      x[i]=0;
    return;
  }

  int l = a.used_words();

#ifdef WORD_LENGTH_32
  if (l<=2) {
    if ( !(l==2 && (a.word_vector()[1] & ((1UL<<21)-1))) ) {
      double a_dbl=a.word_vector()[0];
      if (l==2)
      a_dbl+=ldexp(double(a.word_vector()[1]),32);
      a_dbl*=sgn;
      intdouble2residual(a_dbl,x,p);
      return;
    }  
  }
#else
  if ( l==1 && !((a.word_vector()[0] & ~((1UL<<53)-1))) ) { 
    double a_dbl=a.word_vector()[0];
    a_dbl*=sgn;
    intdouble2residual(a_dbl,x,p);
    return;
  }
#endif
  mod_system::binary2residual(x,p,l,a.word_vector(),sgn, a48, mod48);
}




double residual::mod_system::radix2mod
(double prime, int l, const residual_sequence& y, const residual_sequence& p)
{
  double prime_inv = 1/prime;
  double mod=smod::reduce(y[l-1],prime,prime_inv);
  int j;
  for (j=l-2;j>=0;j--)
    mod=smod::reduce(mod*p[j]+y[j],prime,prime_inv);
  if (mod < 0)
    mod+=prime;
  return mod;
}

unsigned long residual::mod_system::radix2modbase
(int l, const residual_sequence& y, const residual_sequence& p)
{
  digit modbase=int(y[l-1]);
  int j;
  for (j=l-2;j>=0;j--)
    modbase=modbase*digit(p[j])+digit(y[j]);
  return modbase;
}

void residual::mod_system::radix2residual
(residual_sequence& x, int l, const residual_sequence& y, const residual_sequence& p)
{
  int k = x.size();
  int i;
  x[0]=y[0];
  for (i=1;i<l;i++)
    x[i] = radix2mod(p[i],i,y,p);
  for (i=l;i<k;i++)
    x[i] = radix2mod(p[i],l,y,p);
}


void residual::mod_system::radix2integer
(integer& a, int l, const residual_sequence& y, const residual_sequence& p)
{
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(l<=p.size());
#endif
  int i;
  integer X;
  X=y[l-1];
  for (i=l-2;i>=0;i--)
    X=X*int(p[i])+int(y[i]);
  a=X;
}




void residual::mod_system::add_garner_coefficient
(int i, residual_sequence&y, const residual_sequence& x, 
 const residual_sequence& u, const residual_sequence& p)
{
  double z=0;
  double prime = p[i];
  double prime_inv = 1/prime;
  int j;
  for (j=i-1;j>0;j--)
    z=smod::reduce((z+y[j])*p[j-1],prime,prime_inv);
  y[i]=smod::safe_reduce(u[i]*(x[i]-z-y[0]),prime);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
  assert(fabs(y[i]) < prime/2);
#endif
}

void residual::mod_system::probabilistic_check_from_radix
(int l, const residual_sequence& y)
{
  int k = y.size();
  if (l == k && y[k-1] != 0)
    LEDA_EXCEPTION(1,"residual check: number overflows\n");
}

void residual::mod_system::residual2radix
(int k, int& l, residual_sequence& y, const residual_sequence& x, 
 const residual_sequence& u, const residual_sequence& p)
{
  // we first add garner coefficients until one is 0

  l=0;
  int i;
  y[0]=smod::safe_reduce(x[0],p[0]);
  for (i=1;i<k;i++) {
    add_garner_coefficient(i,y,x,u,p);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
    assert(fabs(y[i]) < p[i]/2);
#endif
    if (y[i] == 0)
      break;
  }
  l=i;

  // next we test if really all following y[i] are zero

  double mod;
  double failed = false;
  for (i=l+1;i<k;i++) {
    mod = radix2mod(p[i],l,y,p);
    if (mod != x[i]) {
      failed=true;
      break;
    }
  }

  if (failed) {
    // we proceed with our computation of y[i]
    // at the same point where we left it
    for (i=l+1;i<k;i++)
    add_garner_coefficient(i,y,x,u,p);
    l=k;
  }
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
  assert(0 < l && l <= k);
  if (l!=1)
    assert(y[l-1]!=0);
  if (l!=k)
    assert(y[l]==0); 
#endif
}




int residual::mod_system::sign_from_radix
(int l, const residual_sequence& y)
{
  int i=l-1;
  while (i >= 0) {
    if (y[i]!=0)	
      return smod::sign(y[i]);
    i--;
  }
  return 0;
}


int residual::mod_system::sign_from_residual
(const residual_sequence& x, int& count,
 bool with_check, bool& overflowed, 
 const residual_sequence& w, const residual_sequence& al, 
 residual_sequence& tmp, 
 const residual_sequence& p, const residual_sequence& p_inv
)
{
  int i=0;
  int k=x.size();
  bool zero=true;
  overflowed=false;
  while(zero && (i<k))
    if (x[i++]!=0)
      zero=false;
  if (zero)
    return 0;

  double sum=0;
  for (i=0; i<k;i++) {
    tmp[i] = smod::reduce(x[i]*w[i],p[i],p_inv[i]);
    sum = smod::lazy_frac(sum+tmp[i]*p_inv[i]);
  }
  sum = smod::frac(sum);
  double epsilon = 3*(k-1)*ldexp(1.0,-53);
  count=1;
  if (fabs(sum) > epsilon) {
    if (with_check)
      overflowed=true;
    return smod::sign(sum);
  }

  while(1) {
    // since x is nonzero, this loop always terminates
    count++;
    sum = 0;
    for (i=0; i<k;i++) {
      tmp[i] = smod::reduce(tmp[i]*al[i],p[i],p_inv[i]);
      sum = smod::lazy_frac(sum+tmp[i]*p_inv[i]);
    }
    sum = smod::frac(sum);
    if (fabs(sum) > epsilon)
      return smod::sign(sum);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
    assert(count<=k);
    // to prevent the infinite loop in case of an internal error
#endif
  }
}



const double residual::smod::round_mask=ldexp(3.0,51);
const bool residual::do_overflow_check=true;
const bool residual::no_overflow_check=false;
int residual::maximal_primetable_size=COMPILE_TABLESIZE;
int residual::used_prime_length=0; 
int residual::used_number_of_moduli=0; 
int residual::maximal_representable_bit_size=0;
bool residual::overflow_check_active=true;
residual_sequence residual::primes;           
residual_sequence residual::primes_inv;
residual_sequence residual::garner_constant; 
bool residual::lagrange_active=false;
residual_matrix residual::w_matrix;
residual_sequence residual::lagrange_w; 
residual_sequence residual::alpha; 
residual_sequence residual::mod_48;
residual_sequence residual::temporary;

int residual::used_primetable_size = -1;
//  =initialize_static_members(LAGRANGE_SIZE,COMPILE_TABLESIZE);




void residual::generate_residual_tables
(int _primelength, int _tablesize)
{
  ofstream tablefile("residual_tables.h");
  tablefile << "// The file residual_tables.h was generated by\n";
  tablefile << "// the function residual::generate_residual_tables\n\n";
  tablefile << "const int primelength=" << _primelength << ";\n";
  tablefile << "const int tablesize=" << _tablesize << ";\n";

  residual_sequence p(_tablesize);
  int i=0;

  // first step: fill primetable
  mod_system::compute_prime_sequence_with_bit_size(_primelength,p);
  tablefile << "\nconst int residual_primetable[] = {";
  for (i=0;i<_tablesize;i++) {
    if (i%10==0)
      tablefile << "\n";
    tablefile << int(p[i]);
    if (i < _tablesize-1)
      tablefile << ", ";
  }
  tablefile << "\n};\n";

  // second step: fill in garner coefficients
  residual_sequence u(_tablesize);
  mod_system::compute_garner_constants(_tablesize,u,p);
  tablefile << "\n\nconst int residual_garnertable[] = {";
  for (i=0;i<_tablesize;i++) {
    if (i%10==0)
      tablefile << "\n";
    if (u[i]>0)
      tablefile << " ";
    tablefile << int(u[i]);
    if (i < _tablesize-1)
      tablefile << ", ";
  }
  tablefile << "\n};\n\n";

}



void residual::write_residual_tablefiles
(int _primelength, int compile_size, int max_size)
{
  cout << "write residual_tables.h ...";
  cout.flush();
  generate_residual_tables(_primelength,compile_size);
  cout << "done." << endl;

  residual_sequence _primes(max_size);
  residual_sequence _garner(max_size);

  cout << "write primes ...";
  cout.flush();
  mod_system::compute_prime_sequence_with_bit_size
    (_primelength,_primes);
  mod_system::write_table_to_file("primetable",_primes,26);
  cout << "done." << endl;

  cout << "write garner ...";
  cout.flush();
  mod_system::compute_garner_constants(max_size,_garner,_primes);
  mod_system::write_table_to_file("garnertable",_garner,26);
  cout << "done" << endl;
}





void residual::resize_double_table(int k, residual_sequence& table)
{
  table.resize_rep(k);
}

void residual::fill_main_tables(int k)
{
  if (used_primetable_size >= k)
    return;

  int i;
  resize_double_table(k,primes);
  resize_double_table(k,garner_constant);

  if (k <= tablesize) {
    // tablesize is defined in the file residual_tables.h
    for (i=used_primetable_size;i<k;i++) {
      primes[i] = residual_primetable[i];
      garner_constant[i] = residual_garnertable[i];
    }
  }
  else {
    // need bigger tables
    int prime_length, table_size;
      
    // read primetable from file

    mod_system::read_table_from_file("/incl/LEDA/numbers/primetable",primes,
                                      prime_length,table_size);
    if (table_size<k) {
      // not enough primes found in residual_tables.h
      residual_sequence tmp(k-table_size);
      mod_system::compute_prime_sequence_bounded_by
       (int(primes[table_size-1]-1),tmp);
      for (i=table_size;i<k;i++)
        primes[i]=tmp[i-table_size];	
    }

    // read garnertable from file

    mod_system::read_table_from_file("/incl/LEDA/numbers/garnertable",garner_constant,
                                     prime_length,table_size);
    if (table_size<k) 
      for (i=table_size;i<k;i++)
        mod_system::insert_garner_constant(i,garner_constant,primes);
  }

  resize_double_table(k,primes_inv);
  resize_double_table(k,mod_48);
  double pow48=ldexp(1.0,48);
  for (i=0;i<k;i++) {
    primes_inv[i]=1/primes[i];
    mod_48[i]=residual::mod::reduce_of_positive(pow48,primes[i]);
  }
  
  resize_double_table(k,alpha);
  resize_double_table(k,temporary);

  used_primetable_size = k;
}


 
int residual::initialize_static_members(int lagrange_k, int max_k)
{
  // leda_fpu::set_defaults();

  if (used_primetable_size < 0)  
  {
    used_number_of_moduli = 0;
    maximal_representable_bit_size = 0;
    overflow_check_active=true;
  
    fill_main_tables(max_k);
    used_primetable_size = max_k;
    used_prime_length = 26;
  
    w_matrix.resize(lagrange_k,lagrange_k);
    residual::mod_system::compute_W_matrix
      (w_matrix,primes);
  
    lagrange_active=false;
  }

  return used_primetable_size;
}



int residual::required_primetable_size(int b)
{
  int num=(b+2)/25+1;
  if (num+2 > maximal_primetable_size)
    fill_main_tables(num+2);
  if (b <= 600)
    return num;
  int i=0;
  double bcur=0;
  double correction = 1+ldexp(4.0,-53);
  double log2=log(2.0);
  while(bcur<b) {
    bcur+=log(primes[i++])/log2;
    bcur*=correction;
  }
  return i;
}


int residual::set_maximal_bit_length(int b, bool with_check)
{
  int num=required_primetable_size(b);
  overflow_check_active=with_check;
  if (overflow_check_active)
    num+=2;
  if (num>used_primetable_size)
    LEDA_EXCEPTION(1,"residual: internal error");
  used_number_of_moduli = num;
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
  assert(num>0);
#endif
  int old_length = get_maximal_bit_length();
  maximal_representable_bit_size = b;
  lagrange_active=false;
  return old_length;
}


void residual::activate_lagrange()
{
  int num=used_number_of_moduli;
  int i;
  mod_system::compute_lagrange_alpha(alpha,primes);
  resize_double_table(num,lagrange_w);
  if (w_matrix.rowsize() < num) {
    // matrix does not suffice
    mod_system::compute_lagrange_w(lagrange_w,primes);
  }
  else {
    // matrix suffices
    for (i=0;i<num;i++)
      lagrange_w[i]=w_matrix[num-1][i];
  }
  lagrange_active=true;
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
  double check;
  for (i=0;i<num;i++) {
    check = lagrange_w[i];
    assert(check==floor(check));
  }
#endif
}

 

void residual::add_moduli_above(int old_size) const
{
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(used_number_of_moduli==PTR->size);
  assert(old_size<=PTR->size);
#endif
  int l;
  residual_sequence& y=temporary;
  mod_system::residual2radix
    (old_size,l,y,*this,garner_constant,primes);
  // this is far from optimal!!
  if (overflow_check_active)
    mod_system::probabilistic_check_from_radix(l,y);
  int i;
  for(i=old_size;i<used_number_of_moduli;i++)
    PTR->mod[i]=mod_system::radix2mod(primes[i],l,y,primes);
}

void residual::actualize() const
{
  if (PTR->size>=used_number_of_moduli)
    return;
  int old_size=PTR->size;
  residual& this_residual=(residual&) *this;
  this_residual.PTR=upgrade_rep(PTR);
  add_moduli_above(old_size);
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(used_number_of_moduli==PTR->size);
#endif
}

inline void residual::refresh() const
{
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  int i;
  for (i=0;i<PTR->size;i++)
    assert(0<=PTR->mod[i] && PTR->mod[i]<primes[i]);
#endif
  if (PTR->size<used_number_of_moduli)
    actualize();
}



residual::residual(const residual_sequence& a)
  : residual_sequence(used_number_of_moduli)
{
  if (a.PTR->size != used_number_of_moduli)
    LEDA_EXCEPTION (1,"error: wrong array size, cannot construct residual");
  copy_rep(a.PTR,PTR);
}

residual::residual(const residual& a)
{
  int asize=a.PTR->size;
  if (asize == used_number_of_moduli) {
    PTR = a.PTR;
    a.PTR->count++;
  }
  else {
    if (asize < used_number_of_moduli)
      a.actualize();
    PTR=new_rep(used_number_of_moduli);
    copy_rep(a.PTR,PTR);
  }
}

residual& residual::operator=(const residual& a)
{
  if (this==&a)
    return *this;
  int asize = a.PTR->size;
  if (asize == used_number_of_moduli) {
    if (PTR)
      delete_rep(PTR);
    PTR = a.PTR;
    a.PTR->count++;
  }
  else {
    if (asize < used_number_of_moduli)
      a.actualize();
    PTR=new_rep(used_number_of_moduli);
    copy_rep(a.PTR,PTR);
  }
  return *this;
}


string residual::to_string() const 
{
  return to_integer().to_string();
}
ostream& operator << (ostream& O, const residual& a)
{
  return (O<<a.to_integer());
}
istream& operator >> (istream& I, residual& a)
{
  integer ai;
  (I>>ai);
  a=ai;
  return I;
}


residual::residual() : residual_sequence(used_number_of_moduli)
{ initialize_static_members(LAGRANGE_SIZE,COMPILE_TABLESIZE);
  mod_system::long2residual(0,*this,primes);
}

residual::residual(long a) : residual_sequence(used_number_of_moduli)
{ initialize_static_members(LAGRANGE_SIZE,COMPILE_TABLESIZE);
  mod_system::long2residual(a,*this,primes);
}

residual::residual(int a) : residual_sequence(used_number_of_moduli)
{ initialize_static_members(LAGRANGE_SIZE,COMPILE_TABLESIZE);
  mod_system::long2residual(a,*this,primes);
}

 
residual::residual(const integer& a) 
: residual_sequence(used_number_of_moduli)
{
  initialize_static_members(LAGRANGE_SIZE,COMPILE_TABLESIZE);
  if (overflow_check_active)
    if (a.length() > maximal_representable_bit_size)
      LEDA_EXCEPTION(1,"Overflow in residual(integer)");
  mod_system::integer2residual
    (a,*this,primes,temporary,mod_48);
}

integer residual::to_integer() const
{
  residual_sequence& y=temporary;
  int l;
  int k=PTR->size;

  integer a;
  mod_system::residual2radix
    (k,l,y,*this,garner_constant,primes);
  mod_system::radix2integer(a,l,y,primes);
  if (overflow_check_active)
    if (a.length() > maximal_representable_bit_size)
      LEDA_EXCEPTION(1,"residual to_integer(): number overflows");
  return a;
}


residual::residual(double x) : residual_sequence(used_number_of_moduli)
{
  initialize_static_members(LAGRANGE_SIZE,COMPILE_TABLESIZE);
  if (x==floor(x)) {
    integer a(x);
    if (overflow_check_active) {
      if (a.length() > maximal_representable_bit_size)
        LEDA_EXCEPTION(1,"residual to_integer(): number overflows");
    }
    mod_system::integer2residual(a,*this,primes,temporary,mod_48);
  } 
  else
    LEDA_EXCEPTION(1,"residual(double): argument is not integral");
}



long   residual::length() const { return to_integer().length(); }
bool   residual::is_long() const { return to_integer().is_long(); }
long   residual::to_long() const { return to_integer().to_long(); }
double residual::to_double() const { return to_integer().to_double(); }



void residual::rewrite(residual& z)
{
  if (z.PTR->size!=used_number_of_moduli || z.PTR->count>1) {
    if (z.PTR->size==used_number_of_moduli) {
      rep* old_ptr=z.PTR;
      z.PTR->count--;
      z.PTR=new_rep(used_number_of_moduli);
      copy_rep(old_ptr,z.PTR);
      return;
    }
    if (z.PTR->size>used_number_of_moduli)
      z.PTR=restrict_rep(z.PTR);
    else {
      delete_rep(z.PTR);
      z.PTR=new_rep(used_number_of_moduli);
    }
  }
}



void residual::add
(const residual& x, const residual& y)
{
  residual& z=*this;
  x.refresh(); 
  y.refresh();
  rewrite(z);

#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert(size()==used_number_of_moduli);
#endif
  int i;
  for (i=0;i<used_number_of_moduli;i++) {
    z[i]=mod::add(x[i],y[i],primes[i]);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
    assert(0<=z[i]);
    assert(z[i]<primes[i]);
#endif
  }
}

void residual::sub
(const residual& x, const residual& y)
{
  residual& z=*this;
  x.refresh(); 
  y.refresh();
  rewrite(z);
  
  int i;
  for (i=0;i<used_number_of_moduli;i++) {
    z[i]=mod::sub(x[i],y[i],primes[i]);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
    assert(0<=z[i] && z[i]<primes[i]);
#endif
  }
}

void residual::mul
(const residual& x, const residual& y)
{
  residual& z=*this;
  x.refresh(); 
  y.refresh();
  rewrite(z);

  int i;
  for (i=0;i<used_number_of_moduli;i++) {
    z[i]=mod::mul(x[i],y[i],primes[i]);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
    assert(0<=z[i] && z[i]<primes[i]);
#endif
  }
}

void residual::det2x2
(const residual& a, const residual& b,
 const residual& c, const residual& d)
{
  residual& z=*this;
  a.refresh(); 
  b.refresh();
  c.refresh(); 
  d.refresh();
  rewrite(z);

  int i;
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
  for (i=0;i<used_number_of_moduli;i++) {
    z[i]=mod::reduce(a[i]*d[i]-b[i]*c[i],primes[i],primes_inv[i]);
    assert(0<=z[i] && z[i]<primes[i]);
  }
#else
  for (i=0;i<used_number_of_moduli;i++)
    z[i]=mod::reduce(a[i]*d[i]-b[i]*c[i],primes[i],primes_inv[i]);
#endif
}


void residual::div(const residual& x, const residual& y) {
  residual& z=*this;
  x.refresh(); 
  y.refresh();
  rewrite(z);

  if (y.is_invertible()) {
    int i;
    for (i=0;i<used_number_of_moduli;i++) {
      z[i]=mod::div(x[i],y[i],primes[i]);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
      assert(0<=z[i] && z[i]<primes[i]);
#endif
    }
  }
  else {
    if (y.is_zero())
      LEDA_EXCEPTION(1,"residual: division by zero");
    integer A(x.to_integer()), B(y.to_integer());
    integer C = A/B;
    mod_system::integer2residual
     (C,z,primes,temporary,mod_48);
  }
}

void residual::inverse(const residual& x) {
  residual& z=*this;
  x.refresh(); 
  rewrite(z);

  int i;
  for (i=0;i<used_number_of_moduli;i++) {
    if (x[i]==0)
      LEDA_EXCEPTION(1,"residual::inverse: object not invertible");
    z[i]=mod::inverse(x[i],primes[i]);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
    assert(0<=z[i] && z[i]<primes[i]);
#endif
  }
}

void residual::negate(const residual& x) {
  residual& z=*this;
  x.refresh();
  rewrite(z);

  int i;
  for (i=0;i<used_number_of_moduli;i++) {
    z[i]=mod::negate(x[i],primes[i]);
#ifdef RESIDUAL_CHECK_POSTCONDITIONS
    assert(0<=z[i] && z[i]<primes[i]);
#endif
  }
}


residual operator+(const residual& a, const residual& b)
{
  residual z;
  z.add(a,b);
  return z;
}

residual operator-(const residual& a, const residual& b)
{
  residual z;
  z.sub(a,b);
  return z;
}

residual operator*(const residual& a, const residual& b)
{
  residual z;
  z.mul(a,b);
  return z;
}

residual operator/(const residual& a, const residual& b)
{
  residual z;
  z.div(a,b);
  return z;
}

residual residual::operator-() const
{
  residual z;
  z.negate(*this);
  return z;
}

residual det2x2(const residual& a, const residual& b, 
		const residual& c, const residual& d)
{
  residual z;
  z.det2x2(a,b,c,d);
  return z;
}



residual& residual::operator+=(const residual& a)
{
  add(*this,a);
  return *this;
}

residual& residual::operator-=(const residual& a)
{
  sub(*this,a);
  return *this;
}

residual& residual::operator*=(const residual& a)
{
  mul(*this,a);
  return *this;
}

residual& residual::operator/=(const residual& a)
{
  div(*this,a);
  return *this;
}

residual sqr(const residual& a)
{
  return a*a;
}




bool residual::is_invertible() const
{
  int i;
  for (i=0;i<PTR->size;i++)
    if (PTR->mod[i] == 0)
      return false;
  return true;
}

bool residual::is_zero() const
{
  int i;
  for (i=0;i<PTR->size;i++)
    if (PTR->mod[i] != 0)
      return false;
  return true;
}

void residual::absolute(const residual& a) 
{
  int sgn=a.sign();
  if (sgn==-1)
    negate(a);
}

residual abs(const residual& a)
{
  if (a.is_zero())
    return a;
  if (a.sign()>0)
    return a;
  else
    return -a;
}



int residual::lagrange_sign() const
{
  if (!lagrange_active)
    activate_lagrange();
  bool overflowed;
  int count;
  int sgn = 
    mod_system::sign_from_residual
    (*this,count,overflow_check_active,overflowed,
     lagrange_w,alpha,temporary,
     primes,primes_inv);
  if (overflow_check_active)
    if (overflowed)
      LEDA_EXCEPTION(1,"residual sign(): number overflows\n");
  return sgn;
}

int residual::garner_sign() const
{
  residual_sequence& y=temporary;
  int l;
  int k=size();
  mod_system::residual2radix(k,l,y,*this,garner_constant,primes);
  if (overflow_check_active)
    if (l == k && y[k-1] == 0) 
      LEDA_EXCEPTION(1,"residual sign(): number overflows\n");
  return mod_system::sign_from_radix(l,y);
}

int residual::sign() const
{
  if (lagrange_active || w_matrix.rowsize()<=used_number_of_moduli)
    return lagrange_sign();
  else
    return garner_sign();
}



bool operator==(const residual& a, const residual& b)
{
  return ((a-b).sign()==0);
}
bool operator!=(const residual& a, const residual& b)
{
  return ((a-b).sign()!=0);
}
bool operator<(const residual& a, const residual& b)
{
  return ((a-b).sign()<0);
}
bool operator>(const residual& a, const residual& b)
{
  return ((a-b).sign()>0);
}
bool operator<=(const residual& a, const residual& b)
{
  return ((a-b).sign()<=0);
}
bool operator>=(const residual& a, const residual& b)
{
  return ((a-b).sign()>=0);
}




residual_sequence& residual_sequence::operator=(const residual_sequence& x)
{
  if (this==&x)
    return *this;
  if (PTR)
    delete_rep(PTR);
  PTR=new_rep(x.size());
  copy_rep(x.PTR,PTR);
  return *this;
}

residual_sequence::residual_sequence(const residual_sequence& x)
{
  PTR=new_rep(x.size());
  copy_rep(x.PTR,PTR);
}


void residual_matrix::create_me(int _m, int _n)
{
  row = new residual_sequence[_m];
  for (int i=0; i<_m;i++) 
    row[i].PTR = residual_sequence::new_rep(_n);
  m=_m; n = _n;
}

void residual_matrix::delete_me()
{
  delete[] row;
}

residual_matrix::residual_matrix(int _m, int _n)
{
   create_me(_m,_n);
}

void residual_matrix::resize(int _m, int _n)
{
  delete_me();
  create_me(_m,_n);
}

residual_matrix::~residual_matrix()
{
  delete_me();
}

residual_sequence& residual_matrix::operator[](int i)
{
#ifdef RESIDUAL_CHECK_PRECONDITIONS
  assert (0<=i&&i<m);
#endif
  return row[i];
}

LEDA_END_NAMESPACE


