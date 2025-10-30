/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _fpu.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/system/basic.h>
#include <LEDA/numbers/fpu.h>
#include <LEDA/numbers/fp.h>
#include <LEDA/system/assert.h>

#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#ifdef mips
#include <LEDA/internal/std/stdlib.h>
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4723)
#endif


#if defined(IEEE_FPU_SUPPORTED)

LEDA_BEGIN_NAMESPACE

inline double dbl_add(double x, double y) { return x + y; }
inline double dbl_sub(double x, double y) { return x - y; }
inline double dbl_mul(double x, double y) { return x * y; }
inline double dbl_div(double x, double y) { return x / y; }

#if defined(i386) && !defined(sun) && !defined(__geode__)
extern "C" fcr_type i386_clear_all_exceptions();
#endif


void ieee_round_nearest(void)
{
       	ieee_set_fcr(ieee_nearest  | (ieee_default_mask & ~ieee_round_mask));
} 

void ieee_round_up(void)
{
	ieee_set_fcr(ieee_positive | (ieee_default_mask & ~ieee_round_mask));
}

void ieee_round_down(void)
{ 
	ieee_set_fcr(ieee_negative | (ieee_default_mask & ~ieee_round_mask));
}

void ieee_round_tozero(void)
{ 
	ieee_set_fcr(ieee_tozero   | (ieee_default_mask & ~ieee_round_mask)); 
}



fcr_type set_rounding(fcr_type rmode)
{ fcr_type cw_old = ieee_get_fcr();
  ieee_set_fcr((cw_old & ~ieee_round_mask) | rmode);
  return cw_old;
}
fcr_type get_rounding()
{
    return ieee_get_fcr();
}


fcr_type ieee_clear_all_exceptions(void)
{
#if defined(i386) && !defined(sun) && !defined(__APPLE__) && !defined(__geode__)
  return i386_clear_all_exceptions();
#else
  fcr_type cw_old = ieee_get_fsr();
  ieee_set_fsr(cw_old & ~ieee_all_exceptions);
  return cw_old;
#endif
}

fcr_type ieee_clear_current_exceptions(void)
{
#ifdef IEEE_CAUSING_BITS
  fcr_type sw = ieee_get_fsr();
  ieee_set_fsr(sw & ~ieee_all_current_exceptions);
  return sw & ieee_all_current_exceptions;
#else
  return 0;
#endif
}

BOOLEAN ieee_any_exceptions_raised(void)
{ return ieee_get_fsr() & ieee_all_exceptions; }

BOOLEAN ieee_nontrivial_exceptions_raised(void)
{ return ieee_get_fsr() & ieee_nontrivial_exceptions; }

BOOLEAN exceptions_raised(fcr_type exception)
{ return ieee_get_fsr() & exception; }

void ieee_set_defaults()
{ ieee_set_fcr(ieee_default_mask);
#ifdef IEEE_SEPARATE_SW
  ieee_clear_all_exceptions();
#endif
// caution: for separate status/control words  we need an extra command here
}









fcr_type leda_fpu::recoverable_exceptions=0;
fcr_type leda_fpu::avoidable_traps=0;
fcr_type leda_fpu::catchable_exceptions=0;
fcr_type leda_fpu::recoverable_traps=0;
fcr_type leda_fpu::detectable_trap_types=0;

bool leda_fpu::all_exceptions_recoverable=false;
bool leda_fpu::all_traps_avoidable=false;
bool leda_fpu::all_exceptions_catchable=false;
bool leda_fpu::all_traps_recoverable=false;
bool leda_fpu::all_trap_types_detectable=false;

bool leda_fpu::sigfpe_signal_okay=false;

fcr_type leda_fpu::available_rounding_modes=0;
bool leda_fpu::all_rounding_modes_available=false;
bool leda_fpu::sqrt_rounding_okay=false;

fcr_type volatile leda_fpu::pending_exceptions=0;
fcr_type volatile leda_fpu::retrospective_exceptions=0;
fcr_type volatile leda_fpu::ignored_exceptions=0;
fcr_type volatile leda_fpu::silenced_exceptions=0;
fcr_type volatile leda_fpu::trapping_exceptions=0;
fcr_type volatile leda_fpu::observed_exceptions=0; 




void leda_fpu::round_nearest(void) { ieee_round_nearest(); }
void leda_fpu::round_up(void) { ieee_round_up(); }
void leda_fpu::round_down(void) { ieee_round_down(); }
void leda_fpu::round_tozero(void) { ieee_round_tozero(); }


static bool check_rounding(fcr_type rmode, double x)
{
  volatile double z=1;
  volatile double w=1;
  set_rounding(rmode);

  z = dbl_div(z,x);
  if (x>0) z=dbl_mul(z,x); else z=dbl_mul(z,-x);
  if (z<0) z=-z;

  if (rmode==ieee_positive)
    ieee_round_up(); 
  if (rmode==ieee_negative)
    ieee_round_down(); 
  if (rmode==ieee_nearest)
    ieee_round_nearest();
  if (rmode==ieee_tozero)
    ieee_round_tozero(); 
 
  w = dbl_div(w,x);
  if (x>0) w=dbl_mul(w,x); else w=dbl_mul(w,-x);
  if (w<0) w=-w;
  if (rmode==ieee_positive)
    {
      if (x>0)
        return (z > 1 && w > 1); 
      else
        return (z < 1 && w < 1);
    }
  if (rmode==ieee_negative)
    {
      if (x > 0) 
        return (z < 1 && w < 1); 
      else
        return (z > 1 && w > 1);
    }
  if (rmode==ieee_nearest)
    return (z == 1 && w == 1); 
  if (rmode==ieee_tozero)
    return (z < 1 && w < 1);

  return 0;
}


fcr_type leda_fpu::check_available_rounding_modes(void) 
{
    // save old rounding mode
    fcr_type oldm = leda::get_rounding();
    
   fcr_type approved_modes = 0;
   bool approve_neg=false, approve_pos=false;

   approve_pos = check_rounding(ieee_nearest, 3.0);
   approve_neg = check_rounding(ieee_nearest,-3.0);
   if (approve_pos&&approve_neg)
     approved_modes |= 1;

   approve_pos = check_rounding(ieee_tozero, 3.0);
   approve_neg = check_rounding(ieee_tozero,-3.0);
   if (approve_pos&&approve_neg)
     approved_modes |= 2;

   approve_pos = check_rounding(ieee_positive, 3.0);
   approve_neg = check_rounding(ieee_positive,-3.0);
   if (approve_pos&&approve_neg)
     approved_modes |= 4;

   approve_pos = check_rounding(ieee_negative, 3.0);
   approve_neg = check_rounding(ieee_negative,-3.0);
   if (approve_pos&&approve_neg)
     approved_modes |= 8;

   // restore old rounding mode
   leda::set_rounding(oldm);
   return approved_modes;
}


bool leda_fpu::check_sqrt_rounding(void)
{
    // save old rounding mode
    fcr_type oldm = leda::get_rounding();
    
  volatile double z;
  leda_fpu::round_up();
  z=sqrt(2.0);
  z*=z;
  if (z<=2) return 0;
  leda_fpu::round_down();
  z=sqrt(2.0);
  z*=z;
  // restore old rounding mode before return
  leda::set_rounding(oldm);
  
  if (z>=2) 
    return 0;
  return 1;
}



static fcr_type trap_to_exception(fcr_type traps)
{
  fcr_type exceptions=0;
  if (traps&ieee_trap_bit.inexact)
     exceptions|=ieee_inexact;
  if (traps&ieee_trap_bit.underflow)
     exceptions|=ieee_underflow;
  if (traps&ieee_trap_bit.overflow)
     exceptions|=ieee_overflow;
  if (traps&ieee_trap_bit.divbyzero)
     exceptions|=ieee_divbyzero;
  if (traps&ieee_trap_bit.invalid)
     exceptions|=ieee_invalid;
  return exceptions;
}

#ifdef IEEE_CAUSING_BITS
static fcr_type cause_to_exception(fcr_type current_exceptions)
{
  fcr_type exceptions=0;
  if (current_exceptions&ieee_current_exception_bit.inexact)
     exceptions|=ieee_inexact;
  if (current_exceptions&ieee_current_exception_bit.underflow)
     exceptions|=ieee_underflow;
  if (current_exceptions&ieee_current_exception_bit.overflow)
     exceptions|=ieee_overflow;
  if (current_exceptions&ieee_current_exception_bit.divbyzero)
     exceptions|=ieee_divbyzero;
  if (current_exceptions&ieee_current_exception_bit.invalid)
     exceptions|=ieee_invalid;
  return exceptions;
}
#endif

static fcr_type exception_to_trap(fcr_type exceptions)
{
  fcr_type traps=0;
  if (exceptions&ieee_exception_bit.inexact)
     traps|=ieee_trap_bit.inexact;
  if (exceptions&ieee_exception_bit.underflow)
     traps|=ieee_trap_bit.underflow;
  if (exceptions&ieee_exception_bit.overflow)
     traps|=ieee_trap_bit.overflow;
  if (exceptions&ieee_exception_bit.divbyzero)
     traps|=ieee_trap_bit.divbyzero;
  if (exceptions&ieee_exception_bit.invalid)
     traps|=ieee_trap_bit.invalid;
  return traps;
}


void leda_fpu::display_exceptions(fcr_type exceptions)
{
  if ((exceptions&ieee_all_exceptions)==0)
  {
    cerr << "Sorry, exception type could not be determined\n";
    return;
  }
  if (exceptions == ieee_inexact)
  {
    cerr << "  Inexact operation\n";
    return;
  }

  cerr << "Serious exceptions: \n";

  if (exceptions & ieee_invalid)
    cerr << "  Invalid operation\n";

  if (exceptions & ieee_divbyzero)
    cerr << "  Division by zero\n";

  if (exceptions & ieee_overflow)
    cerr << "  Floating point overflow\n";

  if (exceptions & ieee_underflow)
    cerr << "  Floating point underflow\n";

  if (exceptions & ieee_inexact)
   { cerr << "Other exceptions:\n";
     cerr << "  Inexact operation (usually harmless)\n";
   }
}

void leda_fpu::display_exceptions_short(fcr_type exceptions)
{
  if (exceptions & ieee_invalid)
    cerr << "Invalid ";
  if (exceptions & ieee_divbyzero)
    cerr << "DivBy0 ";
  if (exceptions & ieee_overflow)
    cerr << "Overflow ";
  if (exceptions & ieee_underflow)
    cerr << "Underflow ";
  if (exceptions & ieee_inexact)
    cerr << "Inexact";
  cerr << "\n";
}


/*

void print_binary(fcr_type mask)
{
  int limit = 8*sizeof(fcr_type);
  int x[128],i;
  for (i=0;i<limit;i++)
    x[i]=-1;
  i=0;
  while (mask) 
    {
      if (mask&1)
	x[i++]=1;
      else
	x[i++]=0;
      mask>>=1;
    }
  while (i<limit)
    x[i++]=0;
  while (--i>=0)
    cout << x[i];
  cout << "\n";
}

*/




fcr_type leda_fpu::exception_recently_raised(fcr_type exceptions)
{
  return ieee_get_fsr() & exceptions;
}

fcr_type leda_fpu::exceptions_raised(fcr_type exceptions)
{
  leda_fpu::process_all_exceptions();
  return leda_fpu::pending_exceptions & exceptions;
}

fcr_type leda_fpu::unraise_all_exceptions(void)
{
  return ieee_clear_all_exceptions();
}

fcr_type leda_fpu::ignore_all_exceptions(void)
{
  fcr_type new_pending = ieee_get_fsr() & ieee_all_exceptions;
  fcr_type old_pending = leda_fpu::pending_exceptions;
  leda_fpu::pending_exceptions=0;
  ieee_clear_all_exceptions();
  return new_pending | old_pending;
}

fcr_type leda_fpu::store_all_recent_exceptions(void)
{
  fcr_type pending = ieee_get_fsr() & ieee_all_exceptions;
  ieee_clear_all_exceptions();
  leda_fpu::pending_exceptions |= pending;
  return leda_fpu::pending_exceptions;
}

fcr_type leda_fpu::unraise_exceptions(fcr_type exceptions)
{
  fcr_type pending = store_all_recent_exceptions();
  leda_fpu::pending_exceptions = pending & (~exceptions);
  return (pending & exceptions);
}

void leda_fpu::process_all_exceptions(void)
{
  leda_fpu::store_all_recent_exceptions();
  fcr_type trapping
    = leda_fpu::pending_exceptions & leda_fpu::trapping_exceptions;
  if (trapping)
    (*leda_fpu::sigfpe_immediate)(trapping);
  leda_fpu::pending_exceptions &= ~trapping;
  leda_fpu::observed_exceptions |= 
    (leda_fpu::pending_exceptions & leda_fpu::retrospective_exceptions);
  leda_fpu::pending_exceptions &= (~leda_fpu::ignored_exceptions);
}

// ---------------------------------------------------

void leda_fpu::raise_inexact_exception(void) {
  volatile double z=fp::power_two(-56);
  z=dbl_add(z,1.0);
}
void leda_fpu::raise_underflow_exception(void) {
  volatile double z;
  z=fp::power_two(-1000);
  z=dbl_mul(z,z);
}
void leda_fpu::raise_overflow_exception(void) {
  volatile double z;
  z=fp::power_two(1000);
  z=dbl_mul(z,z);
}
void leda_fpu::raise_divbyzero_exception(void) {
  volatile double z=1.0;
  z=dbl_div(z,0.0);
}
void leda_fpu::raise_invalid_exception(void) {
  volatile double z;
  z=dbl_sub(fp::pInf,fp::pInf);
  z = z;
}



fcr_type leda_fpu::get_trapping_exceptions(void)
{
  fcr_type trapping_except=0;
  fcr_type sw = ieee_get_fsr();
  if (sw==0) return 0;
  fcr_type cw = ieee_get_fcr();
#ifdef IEEE_INVERSE_TRAPPING
  cw=~cw;
#endif
#ifdef IEEE_CAUSING_BITS
  trapping_except= 
    cause_to_exception(sw&ieee_all_current_exceptions);
#else
  trapping_except=sw&ieee_all_exceptions;
#endif
  trapping_except &= trap_to_exception(cw);
  return trapping_except;
}


static jmp_buf env;

static volatile int sigfpe_flag=0;
// this is set whenever a signal handler is taken

static volatile fcr_type sigfpe_type=0;
// here the type of the exception is saved, if available

extern "C" void trap_recovery(int sig)
{
  assert(sig==SIGFPE);
  longjmp(env,1);
}


#if defined(__DECCXX)
#define sig_pf __sigfp
#elif defined(SIG_PF) || defined(sgi) || (defined(__SUNPRO_CC) && ! defined(linux))
#define sig_pf SIG_PF
#else
typedef void (*sig_pf)(int);
#endif



extern "C" void trap_return_checker(int sig)
{
  assert(sig==SIGFPE);
  signal(sig,(sig_pf)trap_recovery);
  sigfpe_flag=1;
  sigfpe_type=leda_fpu::get_trapping_exceptions();
}


bool leda_fpu::check_sigfpe_signal(void)
{
  bool sigfpe_okay = false;
  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0)
  {
    raise(SIGFPE);
    assert(sigfpe_flag==1);
    signal(SIGFPE,(sig_pf)trap_return_checker);
    sigfpe_okay=true;
  }
  else
    sigfpe_okay=false;
  leda_fpu::sigfpe_signal_okay = sigfpe_okay;
  return sigfpe_okay;
}



void leda_fpu::do_check_recoverable_exceptions(void) {
  /* static fcr_type dummy = */ check_recoverable_exceptions();
}

fcr_type leda_fpu::get_recoverable_exceptions(void) {
  leda_fpu::do_check_recoverable_exceptions();
  return leda_fpu::recoverable_exceptions;
}

fcr_type leda_fpu::get_avoidable_traps(void) {
  leda_fpu::do_check_recoverable_exceptions();
  return leda_fpu::avoidable_traps;
}

fcr_type leda_fpu::check_recoverable_exceptions(void)
{
  leda_fpu::set_exception_policy(ieee_all_exceptions,leda_fpu::ignore);
  volatile fcr_type recoverable=0;
  volatile fcr_type avoidable=0;

  signal(SIGFPE,(sig_pf)trap_return_checker);
  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0) {
    if (check_inexact_exception())
      recoverable |= ieee_inexact;
    if (sigfpe_flag==0)
      avoidable |= ieee_inexact;
    sigfpe_flag=0; 
  }
  
  signal(SIGFPE,(sig_pf)trap_return_checker);
  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0) {
    if (check_underflow_exception())
      recoverable |= ieee_underflow;
    if (sigfpe_flag==0)
      avoidable |= ieee_underflow;
    sigfpe_flag=0; 
  }
  signal(SIGFPE,(sig_pf)trap_return_checker);
  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0) {
    if (check_overflow_exception())
      recoverable |= ieee_overflow;
    if (sigfpe_flag==0)
      avoidable |= ieee_overflow;
    sigfpe_flag=0; 
  }
  signal(SIGFPE,(sig_pf)trap_return_checker);
  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0) {
    if (check_divbyzero_exception())
      recoverable |= ieee_divbyzero;
    if (sigfpe_flag==0)
      avoidable |= ieee_divbyzero;
    sigfpe_flag=0; 
  }
  signal(SIGFPE,(sig_pf)trap_return_checker);
  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0) {
    if (check_invalid_exception())
      recoverable |= ieee_invalid;
    if (sigfpe_flag==0)
      avoidable |= ieee_invalid;
    sigfpe_flag=0; 
  }
  leda_fpu::unraise_all_exceptions();

  leda_fpu::recoverable_exceptions = recoverable;
  leda_fpu::avoidable_traps = avoidable;
  return recoverable;
}



void leda_fpu::do_check_traps(void) {
  /* static fcr_type dummy = */ leda_fpu::check_traps();
}


fcr_type leda_fpu::check_traps(void)
{
#if defined(__win32__) || defined(__amd64)
  leda_fpu::catchable_exceptions = 0;
  leda_fpu::recoverable_traps = 0;
  leda_fpu::detectable_trap_types = 0;
  return 0;
#else
  volatile fcr_type available_traps=0;
  volatile fcr_type recoverable=0;
  leda_fpu::detectable_trap_types=0;
  
  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0)
  {
    signal(SIGFPE,(sig_pf)trap_return_checker);
    activate_trap(ieee_invalid);
    sigfpe_flag=0;
    sigfpe_type=0;
    check_invalid_exception();
    if (sigfpe_flag==1)
      recoverable |= ieee_trap_bit.invalid;
  }
  if (sigfpe_flag==1)
    available_traps |= ieee_trap_bit.invalid;
  leda_fpu::detectable_trap_types |= (sigfpe_type & ieee_invalid);
  
  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0)
  {
    signal(SIGFPE,(sig_pf)trap_return_checker);
    activate_trap(ieee_overflow);
    sigfpe_flag=0;
    sigfpe_type=0;
    check_overflow_exception();
    if (sigfpe_flag==1)
      recoverable |= ieee_trap_bit.overflow;
  }
  if (sigfpe_flag==1)
    available_traps |= ieee_trap_bit.overflow;
  leda_fpu::detectable_trap_types |= (sigfpe_type & ieee_overflow);

  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0)
  {
    signal(SIGFPE,(sig_pf)trap_return_checker);
    activate_trap(ieee_underflow);
    sigfpe_flag=0;
    sigfpe_type=0;
    check_underflow_exception();
    if (sigfpe_flag==1)
      recoverable |= ieee_trap_bit.underflow;
  }
  if (sigfpe_flag==1)
    available_traps |= ieee_trap_bit.underflow;
  leda_fpu::detectable_trap_types |= (sigfpe_type & ieee_underflow);

  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0)
  {
    signal(SIGFPE,(sig_pf)trap_return_checker);
    activate_trap(ieee_divbyzero);
    sigfpe_flag=0;
    sigfpe_type=0;
    check_divbyzero_exception();
    if (sigfpe_flag==1)
      recoverable |= ieee_trap_bit.divbyzero;
  }
  if (sigfpe_flag==1)
    available_traps |= ieee_trap_bit.divbyzero;
  leda_fpu::detectable_trap_types |= (sigfpe_type & ieee_divbyzero);

  leda_fpu::unraise_all_exceptions();
  if (setjmp(env)==0)
  {
    signal(SIGFPE,(sig_pf)trap_return_checker);
    activate_trap(ieee_inexact);
    sigfpe_flag=0;
    sigfpe_type=0;
    check_inexact_exception();
    if (sigfpe_flag==1)
      recoverable |= ieee_trap_bit.inexact;
  }
  if (sigfpe_flag==1)
    available_traps |= ieee_trap_bit.inexact;
  leda_fpu::detectable_trap_types |= (sigfpe_type & ieee_inexact);

  inactivate_all_traps();
  leda_fpu::unraise_all_exceptions();
  signal(SIGFPE,(sig_pf)leda_fpu::sigfpe_action);

  leda_fpu::catchable_exceptions = trap_to_exception(available_traps);
  leda_fpu::recoverable_traps    = trap_to_exception(recoverable);
  return leda_fpu::catchable_exceptions;
#endif
}

fcr_type leda_fpu::get_recoverable_traps(void)
{
  /* static const fcr_type dummy = */ check_traps();
  return leda_fpu::recoverable_traps;
}
 






void leda_fpu::inactivate_all_traps(void)
{
  fcr_type cw=ieee_get_fcr();
#ifndef IEEE_INVERSE_TRAPPING
  cw &= ~ieee_trap_mask;
#else
  cw |= ieee_trap_mask;
#endif
  ieee_set_fcr(cw);
  leda_fpu::trapping_exceptions = 0;
}


void leda_fpu::activate_trap(fcr_type exceptions)
{
  fcr_type cw = ieee_get_fcr();
  fcr_type traps = exception_to_trap(exceptions);
#ifndef IEEE_INVERSE_TRAPPING
  cw |= traps;
#else
  cw &= ~traps;
#endif
  ieee_set_fcr(cw);
  leda_fpu::trapping_exceptions |= exceptions;
}

void leda_fpu::inactivate_trap(fcr_type exceptions)
{
  fcr_type cw = ieee_get_fcr();
  fcr_type traps = exception_to_trap(exceptions);
#ifndef IEEE_INVERSE_TRAPPING
  cw &= ~traps;
#else
  cw |= traps;
#endif
  ieee_set_fcr(cw);
  leda_fpu::trapping_exceptions &= ~exceptions;
}



void (*leda_fpu::sigfpe_immediate)(fcr_type)      = &default_sigfpe_immediate;
void (*leda_fpu::sigfpe_retrospective)(fcr_type)  = &default_sigfpe_retrospective;

void leda_fpu::set_immediate_handler(void (*my_handler)(fcr_type)) {
  leda_fpu::sigfpe_immediate = my_handler;
}

void leda_fpu::set_retrospective_handler(void (*my_handler)(fcr_type)) {
  leda_fpu::sigfpe_retrospective = my_handler;
}


void leda_fpu::default_sigfpe_immediate(fcr_type exceptions) {
  cerr << "\n------------------------------------------------------\n";
  cerr << "leda_fpu::sigfpe_immediate:\n";
  cerr << "An arithmetic exception occured:\n";
  display_exceptions(exceptions);
  cerr << "------------------------------------------------------\n";
  leda_fpu::unraise_exceptions(exceptions);

  // cerr << "\nSignal handlers are not allowed to return!\n";
  // cerr << "Abort now.\n";
  // abort();
  // TODO
}

void leda_fpu::default_sigfpe_retrospective(fcr_type exceptions) {
  signal(SIGFPE,(sig_pf)leda_fpu::sigfpe_action);
  cerr << "\n------------------------------------------------------\n";
  cerr << "Class leda_fpu retrospective diagnostics: \n";
  display_exceptions(exceptions);
  cerr << "------------------------------------------------------\n";
}


void leda_fpu::sigfpe_ignore() {
  if (!(leda_fpu::recoverable_traps==leda_fpu::catchable_exceptions))
    {
      cerr << "sigfpe_ignore:\n";
      cerr << "\nSignal handlers are not allowed to return!\n";
      cerr << "Abort now.\n";
      abort();
    }
}

void leda_fpu::sigfpe_action(int sig)
{
  assert (sig==SIGFPE);
  sigfpe_flag=1;
  sigfpe_type=0;
  signal(SIGFPE,(sig_pf)leda_fpu::sigfpe_action);
  fcr_type causing_exceptions = leda_fpu::get_trapping_exceptions();
  if (leda_fpu::trapping_exceptions & causing_exceptions)
    (*leda_fpu::sigfpe_immediate)(causing_exceptions);
  (*leda_fpu::sigfpe_ignore)();
}


void leda_fpu::set_exception_policy
(fcr_type exceptions,exception_policy policy)
{
  sigfpe_flag=0;
  sigfpe_type=0;
  signal(SIGFPE,(sig_pf)leda_fpu::sigfpe_action);
  // note: one handler for all policies 
  leda_fpu::process_all_exceptions();
  retrospective_exceptions &= ~exceptions;
  ignored_exceptions       &= ~exceptions;
  inactivate_trap(exceptions);
  switch (policy)
  {
    case ignore:
      ignored_exceptions |= exceptions;
      break;
    case retrospective:
      retrospective_exceptions |= exceptions;
      break; 
    case immediate:
      activate_trap(exceptions);
      break;
    case silence:
      silenced_exceptions |= exceptions;
      break;
  }
}





fcr_type leda_fpu::check_inexact_exception(void)
{ 
  leda_fpu::raise_inexact_exception();
  return leda_fpu::exception_recently_raised(ieee_inexact);
}

fcr_type leda_fpu::check_underflow_exception(void)
{
  leda_fpu::raise_underflow_exception();
  return leda_fpu::exception_recently_raised(ieee_underflow);
}

fcr_type leda_fpu::check_overflow_exception(void)
{
  leda_fpu::raise_overflow_exception();
  return leda_fpu::exception_recently_raised(ieee_overflow);
}

fcr_type leda_fpu::check_divbyzero_exception(void)
{
  leda_fpu::raise_divbyzero_exception();
  return leda_fpu::exception_recently_raised(ieee_divbyzero);
}

fcr_type leda_fpu::check_invalid_exception(void)
{
  leda_fpu::raise_invalid_exception();
  return leda_fpu::exception_recently_raised(ieee_invalid);
}



unsigned int inexact_exception_computable(void) {

  ieee_set_defaults();

  // precondition: 
  // -- rounding to nearest
  // -- rounding precision = 53 bits (double)
 

  volatile double x = 16383; // 2^14 - 1 
  volatile double y = 8191;  // 2^13 - 1 

  volatile double z = dbl_div(x,y);   

  // for some compilers on i386 nachines, z is wrongly 
  // computed, e.g. i386/linux and i386/MSVC++;
  // this is because internal rounding is 
  // by default to extended precision ...

  volatile double w = z-2;
  for (int i = 1; i <= 4; i++)
    w = w*(y+1)-1;
   
  unsigned int check;
  check = (w==1);

  x = 5244795;
  y = 3932159;
  z = dbl_div(x,y);
  w = x - z*y;
  // the Pentium fdiv bug: on some chips we get z==256 
  
  check = check && (w==0);
 
  return check;
}


static unsigned int divbyzero_exception_computable(void) {
  
  volatile double z;

  ieee_set_defaults();

  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0) {
    z=dbl_div(-3.0,0.0);
  }

  unsigned int check = (z==fp::nInf);

  return check;
}

static unsigned int underflow_exception_computable(void) {
  
  volatile double z,w;

  ieee_set_defaults();

  signal(SIGFPE,(sig_pf)trap_return_checker);
  z=fp::power_two(-1000);
  if (setjmp(env)==0) {
    z=dbl_mul(z,z);
  }
  volatile unsigned int check = (z==0);

  signal(SIGFPE,(sig_pf)trap_return_checker);
  z=fp::power_two(-1000);
  w=fp::power_two(-53);
  if (setjmp(env)==0) {
    z=dbl_mul(z,w);
  }
  check = check && (z>0);
  return check;
}


static unsigned int overflow_exception_computable(void) {
  
  volatile double z;

  ieee_set_defaults();
  signal(SIGFPE,(sig_pf)trap_return_checker);
  z=fp::power_two(1000);
  if (setjmp(env)==0) {
    z*=z; // z=dbl_mul(z,z);
  }
  volatile unsigned int check = (z==fp::pInf);

  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0) {
    z=dbl_div(-fp::pInf,-3.0);
  }
  check = check && (z==fp::pInf);

  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0) {
    z=dbl_div(fp::pInf,-3.0);
  }
  check = check && (z==fp::nInf);

  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0) {
    z=dbl_add(fp::nInf,3.0);
  }
  check = check && (z==fp::nInf);

  return check;
}


static unsigned int invalid_exception_computable(void) {
  
  volatile double z;

  ieee_set_defaults();

  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0) {
    z=dbl_sub(fp::pInf,fp::pInf);
  }
  volatile unsigned int check = (fp::is_nan(z));

  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0) {
    z=dbl_div(0.0,0.0);
  }
  check = check && (fp::is_nan(z));

  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0) {
    z=dbl_add(fp::NaN,3.0);
  }
  check = check && (fp::is_nan(z));

  signal(SIGFPE,(sig_pf)trap_return_checker);
  if (setjmp(env)==0) {
    z=dbl_mul(fp::pInf,0.0);
  }
  check = check && (fp::is_nan(z));

  return 1;
}

static fcr_type do_get_computable_exceptions(void) 
{
  fcr_type computable=0;
  if (inexact_exception_computable())
    computable |= ieee_inexact;
  if (underflow_exception_computable())
    computable |= ieee_underflow;
  if (overflow_exception_computable())
    computable |= ieee_overflow;
  if (divbyzero_exception_computable())
    computable |= ieee_divbyzero;
  if (invalid_exception_computable())
    computable |= ieee_invalid;
  return computable;
}

fcr_type leda_fpu::get_computable_exceptions(void)
{
  static const fcr_type computable=do_get_computable_exceptions();
  return computable;
}




int leda_fpu::set_defaults(void)
{
  leda_fpu::unraise_all_exceptions();
  ieee_set_fcr(ieee_default_mask);
  set_exception_policy(ieee_all_exceptions,leda_fpu::silence);

  static leda_fpu singleton;

  return 1;
}

leda_fpu::leda_fpu()
{
//   leda_fpu::check_sigfpe_signal();
//   check_recoverable_exceptions();
//   check_traps();

  leda_fpu::available_rounding_modes=check_available_rounding_modes();
  leda_fpu::sqrt_rounding_okay=leda_fpu::check_sqrt_rounding();
}

leda_fpu::~leda_fpu()
{
  process_all_exceptions();  
  if (leda_fpu::observed_exceptions)
    (*sigfpe_retrospective)(leda_fpu::observed_exceptions);
}

void leda_fpu::diagnosis() 
{
  leda_fpu::check_sigfpe_signal();
  check_recoverable_exceptions();
  check_traps();

  cerr << "\n------------------------------------------------------\n";
  cerr << "leda_fpu::diagnosis: \n";
  cout << "sigfpe signal: ";
  if (leda_fpu::sigfpe_signal_okay) cout << "okay\n";
  else cout << "wrong\n";

  cout << "computable exceptions: ";
  display_exceptions_short(leda_fpu::get_computable_exceptions());
  
  cout << "recoverable exception: ";
  display_exceptions_short(leda_fpu::recoverable_exceptions);
  cout << "avoidable trap       : ";
  display_exceptions_short(leda_fpu::avoidable_traps);

  cout << "catchable   exception: ";
  display_exceptions_short(leda_fpu::catchable_exceptions);
  cout << "recoverable trap     : ";
  display_exceptions_short(leda_fpu::recoverable_traps);
  cout << "detectable types     : ";
  display_exceptions_short(leda_fpu::detectable_trap_types);

  cout << "rounding to nearest: ";
  if (available_rounding_modes&1)
    cout << "okay\n";
  cout << "directed rounding:   ";
  if ((available_rounding_modes&14U) == 14)
    cout << "okay\n";

  cout << "sqrt rounding: ";
  if (leda_fpu::sqrt_rounding_okay==true) cout << "okay"; 
  else cout << "wrong";
  cout << "\n";
  cerr << "------------------------------------------------------\n";
}



int dummy = leda_fpu::set_defaults();
// important: This does a lot of initialization

LEDA_END_NAMESPACE

// NOTE: This is a global function!
int leda_set_fpu_defaults() { return leda::leda_fpu::set_defaults(); }

#endif

