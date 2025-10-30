/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _leda.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/system/file.h>
#include <LEDA/system/stream.h>

#include <string.h>
#include <stdlib.h>
#include <time.h>

#if defined(__unix__)

#include <unistd.h>
#include <pwd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>

#endif

#if defined(__win32__) || defined(__win64__)
#if defined(MAXINT)
#undef MAXINT
#endif
#include <windows.h>
#endif



LEDA_BEGIN_NAMESPACE

void compiler_string(char* ccname) {

#if defined(__GNUC__)
  sprintf(ccname,"GNU gcc-%d.%d", __GNUC__, __GNUC_MINOR__);

#elif defined(__SUNPRO_CC)
  sprintf(ccname,"SUN C++ %x", __SUNPRO_CC);

#elif defined(__KCC)
  sprintf(ccname,"KAI C++ %.1f%c", __KCC_VERSION/1000.0,
                                  'a' + __KCC_VERSION%100 - 1);

#elif defined(__mipspro__)
  sprintf(ccname,"SGI C++ %d", _COMPILER_VERSION);

#elif defined(__DECCXX)
  sprintf(ccname,"DEC C++ %d", __DECCXX_VER);

#elif defined(__hpuxcc__)
  sprintf(ccname,"HP  C++ ");

#elif defined(__xlC__)
  sprintf(ccname,"IBM C++ ");

#elif defined(_MSC_VER)
  sprintf(ccname,"Visual C++ (MSC_VER=%d)", _MSC_VER);

#elif defined(__BORLANDC__)
  sprintf(ccname,"Borland C++ %x", __BORLANDC__);

#elif defined(__WATCOMC__)
  sprintf(ccname,"Watcom C++ %x", __WATCOMC__);

#else
  sprintf(ccname,"???????");

#endif
}


#if defined(LEDAROOT)

#if defined(__STDC__)
const char* rootdir_name = #LEDAROOT;
#else
const char* rootdir_name = "LEDAROOT";
#endif

#else
const char* rootdir_name = "/LEDA/INSTALL";
#endif


char* option_list     = 0;
int   node_data_slots = 0;
int   edge_data_slots = 0;
int   face_data_slots = 0;


// logging


void write_log(const char* s)  
{ 
  const char* fname = "/LEDA/LOGFILE";

  if (getenv("LEDA_NO_LOG") == NULL && is_file(fname))
  { ofstream log(fname,ios::app);
    if (log.good()) log << s << endl;
   }
 }

void user_log()  
{ 
  const char* fname = "/LEDA/LOGFILE";

  if (getenv("LEDA_NO_LOG") != NULL || !is_file(fname)) return;

  ofstream log(fname,ios::app);

  if (!log.good()) return; 

  time_t clock; 
  time(&clock);

  string usr_id = get_user_name();

  char host_name[64];

/*
  list<unsigned long> idlist;
  get_host_info(host_name,64,idlist);
  unsigned long host_id = idlist.contents(idlist.first());
*/
  unsigned long idlist[256];
  get_host_info(host_name,64,idlist,256);
  unsigned long host_id = idlist[0];

  char cc_date[8];
  strncpy(cc_date,compile_date,6);
  cc_date[6] = '\0';

  char cc_name[256];
  compiler_string(cc_name);

  log << endl;
  log << time_string() << endl;
  log << string("%s@%s (%d)  ", ~usr_id,host_name,host_id);
  log << string("%s %s %s ",version_string,cc_name,cc_date);
  log << endl;
}


void read_options()  
{ 
  char* rdn = getenv("LEDAROOT");
  if (rdn) rootdir_name = rdn;

  //option_list = getenv("LEDAOPTS"); 
  option_list = getenv("LEDA_INIT"); 

  if (getenv("LEDA_CATCH_SYSTEM_ERRORS")) catch_system_errors();

  char* s1 = getenv("LEDA_NODE_DATA");
  char* s2 = getenv("LEDA_EDGE_DATA");

  if (s1) node_data_slots = atoi(s1);
  if (s2) edge_data_slots = atoi(s2);

  if (s1 || s2)
  { fprintf(stderr,"\n");
    fprintf(stderr,"node_data_slots = %d\n",node_data_slots);
    fprintf(stderr,"edge_data_slots = %d\n",edge_data_slots);
    fprintf(stderr,"\n");
   }

}


LEDA_END_NAMESPACE





#if defined(linux) || \
(defined(sparc) && defined(__GNUC__) && !defined(__OPTIMIZE__))

LEDA_BEGIN_NAMESPACE

static string get_exec_name()
{ int pid = getpid();

#if defined(sparc)
  char cmd[128];
  FILE* in;
  in = popen("/bin/ps -o pid,comm","r");
  int p;
  fgets(cmd,128,in);
  do fscanf(in,"%d %s",&p,cmd);
  while (p != pid);
  fclose(in);
  return first_file_in_path(cmd,getenv("PATH"),':');
#endif

#if defined(linux)
  return string("/proc/%d/exe",pid);
#endif

  return "";
}



static void get_name_list(list<string>& nm_list)
{ 
  string tmpf = tmp_file_name();

  string ename = get_exec_name();

  if (ename == "") return;

#if defined(linux)
  string cmd = "nm -p -t d " + ename + " > " + tmpf;
#else
  string cmd = "nm -h -p -t d " + ename + " > " + tmpf;
#endif
  if (system(cmd) & 0xFF)
	  LEDA_EXCEPTION(1, string("get_name_list: executing cmd '%s' failed.", ~cmd));

  file_istream in(tmpf);

  while (in) 
  { string s = read_line(in);
    if (s.index(" T ") > -1 || s.index(" U ") > -1) 
          nm_list.append(s);
   }

  nm_list.sort();

  delete_file(tmpf);
}



static void find_function(void* A, char* func_name)
{ 

return;

  static list<string> nm_list;

  unsigned long addr = 0;
  string c_str;
  string fname = "???";
  string s;


  if (nm_list.empty()) get_name_list(nm_list);

  list_item it;
  forall_items(it,nm_list)
  { 
    string_istream in(nm_list[it]);
    in >> addr >> c_str;
    if (addr > (unsigned long)A) 
    { fname = s;
      break;
     }
    in >> s;
    read_line(in);
   } 
#if defined(linux)
  if (addr <= (unsigned long)A) fname = s;
#endif

  strcpy(func_name,fname.cstring());

}

static char func_name[512];

LEDA_END_NAMESPACE




#if defined(linux)

#if !defined(__MIPSEL__) && !defined(__ANDROID__)
#include <execinfo.h>
#else
inline int    backtrace(void**, int ) { return 0; }
inline char** backtrace_symbols(void**, int ) { return 0; }
#endif

LEDA_BEGIN_NAMESPACE

int trace_stack(char** trace_lines)
{
  void* A[256];

  int n = backtrace(A,256);
  char** name = backtrace_symbols(A,n);

  int j = 0;
  for(int i=0; i<n; i++)
  { 
    char* p = name[i];
    while (*p!= '\0' && *p != '(') p++;
    if (*p == '(') p++;
    char* q = p;
    while (*q != '\0' && *q != ')' && *q != '+') q++;
    strncpy(func_name,p,q-p);
    func_name[q-p] = '\0';

    if (func_name[0] == '\0') find_function(A[i],func_name);

    if (string(func_name).index("trace_stack") >= 0) continue;

    string str("#%02d at 0x%08x in %s",j,A[i],func_name);
    trace_lines[j] = new char[str.length()+1];
    strcpy(trace_lines[j],str.cstring());
    j++;

    if (string(func_name).tail(4) == "main") break;
  }

 return j;
}

LEDA_END_NAMESPACE

#else

LEDA_BEGIN_NAMESPACE

int trace_stack(char** trace_lines)
{
 volatile int    i=0;
 volatile void** stack_ptr;

 asm("ta 3");

 stack_ptr = (volatile void**)&trace_lines - 35;

 func_name[0] = 0;

 for(i = 0; stack_ptr[1] != 0 && strcmp(func_name,"main") != 0; i++)
 { find_function((void**)stack_ptr[1],func_name);
   string str("#%02d at 0x%08x in %s",i,stack_ptr[1],func_name);
   trace_lines[i] = new char[str.length()+1];
   strcpy(trace_lines[i],str.cstring());
   stack_ptr = (volatile void**)stack_ptr[0] + 14;
  }
  return i;
}

LEDA_END_NAMESPACE

#endif


#else

LEDA_BEGIN_NAMESPACE
int trace_stack(char**) { return 0; }
inline string get_exec_name() { return ""; }
LEDA_END_NAMESPACE

#endif

LEDA_BEGIN_NAMESPACE

static string addr2line(string line)
{
  string ename = get_exec_name();
  int pos = line.index("0x");
  string addr = line(pos,pos+10);

  string tmpf = tmp_file_name();
  string filt_cmd = "addr2line -e " + ename + " " + addr + " > " + tmpf;

  string result;
  if (system(filt_cmd) == 0)
  { ifstream in(tmpf);
    result = read_line(in);
   }

  delete_file(tmpf);
  return result; 
}


void  trace_stack(ostream& out) 
{ char* trace_lines[256];
  int trace_count = trace_stack(trace_lines);
  for(int i=0; i< trace_count; i++) 
  { char* p = trace_lines[i];
    char* q = p + strlen(p);
    while (*q != ' ') q--;
    cerr.write(trace_lines[i],int(q-p+1));
    cerr << cplus_filter(q+1) << endl;
    cerr << "    " << addr2line(trace_lines[i]) << endl;
    cerr << endl;
    delete[] trace_lines[i];
   }
  out << endl;
 }

LEDA_END_NAMESPACE
