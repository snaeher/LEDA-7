/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _time.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>

#include <time.h>

#if defined(__unix__)
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#endif

#if defined(__win32__) || defined(__win64__)
#if defined(MAXINT)
#undef MAXINT
#endif
#include <windows.h>
#include <chrono>
#endif



LEDA_BEGIN_NAMESPACE

#if defined(__unix__)

unsigned long epoche_time()
{ // unix epoche time (milliseconds since 01.01.1970)
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return 1000*(unsigned long)(tv.tv_sec)+(unsigned long)(tv.tv_usec)/1000;
}

double cpu_time() 
{ struct timespec tspec;
  clock_gettime(CLOCK_REALTIME,&tspec);
  return double(tspec.tv_sec) + 0.000000001 * double(tspec.tv_nsec);
}
  
float used_time()
{ 
#if defined(CLK_TCK)
  long clk_tck = CLK_TCK;
#elif defined(HZ)
  long clk_tck = HZ;
#else
  long clk_tck = sysconf(_SC_CLK_TCK);
#endif

  tms x;
  times(&x);
  return  float(x.tms_utime)/clk_tck;
}

float elapsed_time()
{ 
  // elapsed time since midnight

  timeval tv;
  gettimeofday(&tv,NULL);

  time_t now = tv.tv_sec;
  
  tm* tptr = localtime(&now);
  tptr->tm_hour = 0;
  tptr->tm_min = 0;
  tptr->tm_sec = 0;

  time_t t0 = mktime(tptr);

  double real_t  = (tv.tv_sec - t0) + double(tv.tv_usec)/1000000;

  return float(real_t);
}


#elif defined(__win32__) || defined(__win64__)

unsigned long epoche_time()
{ 
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);

  __int64 high = ft.dwHighDateTime;
  __int64 low  = ft.dwLowDateTime;

  return (unsigned long)(((high << 32) + low)/10000);
 
/*

  //Get the number of seconds since January 1, 1970 12:00am UTC
  //Code released into public domain; no attribution required.

  //January 1, 1970 (start of Unix epoch) in "ticks"
  const __int64 UNIX_TIME_START = 0x019DB1DED53E8000; 
  const __int64 TICKS_PER_SECOND = 10000000; //a tick is 100ns

  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);

  //Copy the low and high parts of FILETIME into a LARGE_INTEGER
  //This is so we can access the full 64-bits as an Int64 without 
  // causing an alignment fault

   //Convert ticks since 1/1/1970 into seconds
   LARGE_INTEGER li;
   li.LowPart  = ft.dwLowDateTime;
   li.HighPart = ft.dwHighDateTime;
   return (unsigned long)((li.QuadPart - UNIX_TIME_START)/TICKS_PER_SECOND);
*/

}


double get_process_time()
{ HANDLE hProcess = GetCurrentProcess();


  FILETIME ftCreate, ftExit, ftKernel, ftUser;
  GetProcessTimes(hProcess,&ftCreate,&ftExit,&ftKernel,&ftUser);

  double msec = (ldexp(ftKernel.dwHighDateTime,32) + 
                 ftKernel.dwLowDateTime)/10000;

  msec += ((ldexp(ftUser.dwHighDateTime,32) + ftUser.dwLowDateTime)/10000);

  return msec/1000;
}


typedef std::chrono::time_point<std::chrono::system_clock> sys_time;

double cpu_time()
{ static sys_time t_start = std::chrono::system_clock::now();
  sys_time t = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = t - t_start;
  return elapsed_seconds.count();
  return 0;
}


#if 0
// seems not to work
double cpu_time()
{ 
  // elapsed time since sytem start

/*
  // 16 msec resolution

  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  double msec = (ldexp(ft.dwHighDateTime,32)+ft.dwLowDateTime)/10000.0;
  return msec/1000;
*/

  HANDLE hProcess = GetCurrentProcess();

  FILETIME ftCreate, ftExit, ftKernel, ftUser;
  GetProcessTimes(hProcess,&ftCreate,&ftExit,&ftKernel,&ftUser);

  double t1=(ldexp(ftUser.dwHighDateTime,32)+ftUser.dwLowDateTime)/10000.0;
  double t2=(ldexp(ftKernel.dwHighDateTime,32)+ftKernel.dwLowDateTime)/10000.0;

  return (t1 + t2)/1000;
}
#endif


float used_time() { return (float)cpu_time(); }

float elapsed_time() { 
  return  float(clock())/CLOCKS_PER_SEC;
}

#else

double cpu_time()    { return  double(clock())/CLOCKS_PER_SEC; }
float used_time()    { return  float(clock())/CLOCKS_PER_SEC; }
float elapsed_time() { return  float(clock())/CLOCKS_PER_SEC; }

#endif

double cpu_time(double& T)
{ double t = cpu_time() - T;
  T += t;
  return t;
}

float used_time(float& T)
{ float t = used_time() - T;
  T += t;
  return t;
}

float elapsed_time(float& T)
{ float t = elapsed_time() - T;
  T += t;
  return t;
}

LEDA_END_NAMESPACE
