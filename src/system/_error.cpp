/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _error.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/error.h>
#include <string.h>
#include <stdlib.h>

LEDA_BEGIN_NAMESPACE

LedaErrorHandler std_error_handler = default_error_handler;

bool leda_assert(bool cond, const char* err_msg, int err_no)
{ if (!cond) error_handler(err_no,err_msg);
  return cond;
 }

void default_error_handler(int i, const char* s)
{ 
  cerr << endl;

  if (i==0) 
     cerr << "LEDA WARNING" << endl;
  else 
     cerr << "LEDA ERROR HANDLER" <<endl;

  cerr << "\t" << s << endl; 
  cerr << endl;

  if (i > 0)
  { catch_system_errors(false);
    trace_stack(cerr);
    cerr.flush();
    abort();
   }
}


leda_exception::leda_exception(const char* s, int n)
{ num = n;
  size_t len = strlen(s);
  if (len > 255) len = 255;
  for(size_t i=0; i<len; i++) msg[i] = s[i];
  msg[len] = '\0';
}


void exception_error_handler(int i, const char* s)
{ 
  if (i > 0)
    throw leda_exception(s,i);
  else
    cerr << "LEDA WARNING:" << s << "\n";
}

//LedaErrorHandler set_error_handler(LedaErrorHandler handler)

LedaErrorHandler set_error_handler(void (*handler)(int,const char*))
{ LedaErrorHandler old = error_handler;
  std_error_handler = handler;
  return old;
}

LedaErrorHandler get_error_handler() { return std_error_handler; }

LEDA_END_NAMESPACE


#if defined(__unix__)

#include <signal.h>

/*
#include <ucontext.h>
static void handle_system_errors(int sig, siginfo_t*, void* ucp) 
*/
LEDA_BEGIN_NAMESPACE

static void handle_system_errors(int sig)
{ 
  string err_msg = "System Error: ";
  switch (sig) {
   case SIGINT:  err_msg += "Interrupt.";
                 break;
   case SIGBUS:  err_msg += "Bus error.";
                 break;
   case SIGSEGV: err_msg += "Segmentation violation.";
                 break;
   case SIGILL:  err_msg += "Illegal instruction.";
                 break;
   default:      err_msg += "Unknown error.";
                 break;
  }

 
 error_handler(1,err_msg);
/*
 makecontext((ucontext*)ucp,error_handler,3,1,"error");
 setcontext((ucontext*)ucp);
*/
}


#if defined(__DECCXX)
#define sig_pf __sigfp
#elif defined(SIG_PF) || defined(sgi) || (defined(__SUNPRO_CC) && !defined(linux))
#define sig_pf SIG_PF
#else
typedef void (*sig_pf)(int);
#endif

void catch_system_errors(bool b) 
{
 //static struct sigaction sigact;
 //sigact.sa_sigaction = handle_system_errors;
 //sigact.sa_flags = SA_SIGINFO;
 //sigaction(SIGBUS, &sigact,NULL);
 //sigaction(SIGSEGV,&sigact,NULL);
 //sigaction(SIGILL, &sigact,NULL);

 sig_pf handler = (b) ? (sig_pf) handle_system_errors : (sig_pf)SIG_DFL;
 signal(SIGBUS, handler);  // bus error
 signal(SIGSEGV,handler);  // segmentation fault
 signal(SIGILL, handler);  // illegal instruction
}

LEDA_END_NAMESPACE

#else

LEDA_BEGIN_NAMESPACE
void catch_system_errors(bool) {};
LEDA_END_NAMESPACE

#endif

