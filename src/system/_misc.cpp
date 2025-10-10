/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _basic.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/system/file.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__unix__)
#include <unistd.h>
#include <pwd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>

#if !defined(__aCC__)
#include <sys/select.h>
#endif

#if defined(_AIX)
#include<strings.h>
#include<sys/select.h>
#endif

#endif

#if defined(__win32__) || defined(__win64__)
#if defined(MAXINT)
#undef MAXINT
#endif
#include <windows.h>
#endif



LEDA_BEGIN_NAMESPACE

#if defined(__unix__) && !defined(__win32__)

void sleep(double sec)
{ int usec = int(1000000*sec);
  timeval delay;
  delay.tv_sec  = usec / 1000000;
  delay.tv_usec = usec % 1000000;
  select(0, NULL, NULL, NULL, &delay);
}

void get_command_line(int& argc, char**& argv) 
{ 
  int pid = getpid();
  string fname = string("/proc/%d/cmdline",pid);

  FILE* fp = fopen(fname,"rb");

  char buf[256];
  int n = fread(buf,1,256,fp);

  for(int i=0; i<n;i++) {
    if (buf[i] == '\0') buf[i] = ' ';
  }
  buf[n-1] = '\0';

  //break_into_words(string(buf),argc,argv);

  string A[128];
  argc = string(buf).break_into_words(A,128);
  argv = new char*[argc];
  for(int i=0; i<argc; i++) argv[i] = A[i].cstring();
}


string get_user_name()
{ 
#if !defined(__geode__)
  passwd* pwd = getpwuid(geteuid());
  return pwd ? pwd->pw_name : "unknown";
#endif
  char* p = getenv("USER");
  return p ? p : "unknown";
 }

string get_host_name()
{ char buf[64];
  strcpy(buf,"unknown");
  gethostname(buf,64);
  return buf;
}

#endif

#if defined(__win32__) || defined(__win64__)

void sleep(double sec) 
{ int msec = int(1000*sec);
  Sleep(msec); 
 }


void get_command_line(int& argc, char**& argv)
{ string cmd_line = GetCommandLine(); 
  string params[32];
  argc = cmd_line.break_into_words(params,32);
  argv = new char*[argc];
  for(int i=0; i<argc; i++) argv[i] = strdup(~params[i]);
}

string get_user_name()
{ char buf[128];
  DWORD sz = 128;
  strcpy(buf,"unknown");
  GetUserName(buf,&sz);
  return buf;
}

string get_host_name()
{ char buf[128];
  DWORD sz = 128;
  strcpy(buf,"unknown");
  GetComputerName(buf,&sz);
  return buf;
}


#endif


bool  is_space(char c)  { return isspace(c) != 0; }


void get_time(int& ye, int& mo, int& da, int& h, int& m, int& s)
{ time_t clock; 
  time(&clock);
  tm* T = localtime(&clock); 
  ye = (int)T->tm_year + 1900;
  mo = (int)T->tm_mon + 1;
  da = (int)T->tm_mday;
  h = (int)T->tm_hour;
  m = (int)T->tm_min;
  s = (int)T->tm_sec;
}

string time_string(time_t t) 
{ tm* T = localtime(&t); 
  int ye = (int)T->tm_year + 1900;
  int mo = (int)T->tm_mon + 1;
  int da = (int)T->tm_mday;
  int h = (int)T->tm_hour;
  int m = (int)T->tm_min;
  int s = (int)T->tm_sec;
  return string("%4d/%02d/%02d %02d:%02d:%02d",ye,mo,da,h,m,s);
}

string time_string() 
{ time_t clock; 
  time(&clock);
  return time_string(clock);
}


bool get_environment(string var)
{ return getenv(var) != NULL; }

bool get_environment(string var, string& val)
{ char* p = getenv(var);
  if (p) val = string(p);
  return p != NULL;
}


int Yes(string s)
{ char answer = read_char(s);
  return ((answer == 'y') || (answer == 'Y'));
}

int read_int(string s)
{ int answer;
  char c;

  for(;;)
  { cout << s;
    cin >> answer;
    if (!cin) 
      { cin.clear();
        cin.get(c);
        cout << string("read_int: illegal input \"%c\"\n",c);
        if (c != '\n') skip_line(cin);
       }
    else  
       break;
   }

  skip_line(cin);

  return answer;
}

char read_char(string s)
{ char c;
  cout << s;
  cin.get(c);
  if (c != '\n') skip_line(cin);
  return c;
}

double read_real(string s)
{ double answer;
  cout << s;
  cin >> answer;
  skip_line(cin);
  return answer;
}

string read_line(istream& in)
{ string result;
  result.read_line(in);
  return result;
 }

string read_string(string s)
{ cout << s;
  cout.flush();
  return read_line(cin); 
 }

void skip_line(istream& s)
{ //char c;
  //while (s.get(c) &&  c != '\n');
  s.ignore(256,'\n');
 }

int     Yes()              { return Yes(""); }
int     read_int()         { return read_int(""); }
char    read_char()        { return read_char(""); }
double  read_real()        { return read_real(""); }
string  read_string()      { return read_string(""); } 


string cplus_filter(string name)
{
  string tmpf = tmp_file_name();
  string filt_cmd = string("c++filt '") + name + "' > " + tmpf;

  string result = name;

  if (system(filt_cmd) == 0)
  { ifstream in(tmpf);
    result = read_line(in);
   }

  delete_file(tmpf);

  return result; 
}

LEDA_END_NAMESPACE
