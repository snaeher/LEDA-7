/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _file.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/internal/system.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <sys/stat.h>

#if defined(__win32__) || defined(__win64__)
/*
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN
*/

#if defined(MAXINT)
#undef MAXINT
#endif

#include <windows.h>
#include <conio.h>
#endif


#include <LEDA/core/string.h>

#include <LEDA/system/file.h>
#include <LEDA/system/socket.h>
#include <LEDA/system/http.h>


#include <assert.h>


#if defined(__unix__)

#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__APPLE__)
/*
#include <sys/param.h>
#include <sys/ucred.h>
*/
#include <sys/mount.h>
#else
#include <mntent.h>
#endif


#if defined(__svr4__) || defined(__linux__) || defined(__hpux)
#include <fnmatch.h>
#else
inline int fnmatch(const char*,const char*,int) { return 0; }
#endif

#endif



LEDA_BEGIN_NAMESPACE



string get_directory_delimiter()
{
#if defined(__unix__) || !defined(__win32__) || !defined(__win64__)
  return string("/");
#else
  return string("\\");
#endif
}

void append_directory_delimiter(string& dir)
{
  string delim = get_directory_delimiter();
  if (dir.tail(delim.length()) != delim) dir += delim;
}

void remove_trailing_directory_delimiter(string& dir)
{
  string delim = get_directory_delimiter();
  if (dir.tail(delim.length()) == delim) dir = dir(0, dir.length()-1-delim.length());
}



#if defined(__unix__) && !defined(__win32__)

#ifndef P_tmpdir
#define P_tmpdir "/tmp"
#endif

int get_number_of_cores() {  return sysconf(_SC_NPROCESSORS_ONLN); }


list<string> get_disk_drives()
{ 
  list<string> drives;

#if defined(__APPLE__)

/*
  list<string> L = get_directories("/Volumes");
  string s;
  forall(s,L) {
    if (s != "" && s[0] == '.') continue;
    drives.append(string("/Volumes/") + s);
  }
*/

 struct statfs* mounts;
 int num_mounts = getmntinfo(&mounts,MNT_WAIT);

 if (num_mounts < 0) { /* error */ }

 for(int i=0; i<num_mounts; i++)
 {
   string dev_name = mounts[i].f_fstypename;
   string dir_name = mounts[i].f_mntonname;
/*
   if (dev_name.starts_with("/dev") && dir_name != "/") 
*/
      drives.append(dir_name);
}

#else
  FILE* file = setmntent("/proc/mounts", "r");

  if (file == NULL)  
    perror("setmntent");
  else
  {
    mntent* mntent;

    while ((mntent = getmntent(file)))
    { string dev_name = mntent->mnt_fsname;
      string dir_name = mntent->mnt_dir;
/*
cout << "name = " << dir_name << endl;
cout << "type = " << mntent->mnt_type << endl;
cout << "opts = " << mntent->mnt_opts << endl;
*/
      if (dev_name.starts_with("/dev") && dir_name != "/") 
        drives.append(dir_name);
     }

    endmntent(file);
  }

#endif

  drives.sort();
  drives.unique();

  return drives;
}


/*
list<string> get_disk_drives_0() 
{ 
  list<string> drives;

  ifstream in("/proc/mounts");

  while (in)
  { string line = read_line(in).trim();
    list<string> L = break_into_words(line);
    if (L.length() < 2) continue;
    string device = L.pop();
    string target = L.pop();
    if (device(0,3) == "/dev" && target != "/") drives.append(target);
   }

  drives.sort();
  drives.unique();

  return drives;
}
*/




string get_home_directory()
{ string dname;
  if (get_environment("HOME",dname)) 
    return dname;
  else
    return P_tmpdir;
}


bool create_directory(string fname)
{ string fn = fname.replace_all(" ","\\ ");
  struct stat stat_buf;
  if (stat(fname,&stat_buf) == 0) return false;
/*
  if (system("mkdir " + fn) & 0xFF) return false;
  return is_directory(fname);
*/
  return mkdir(fname,0755) == 0;
}

bool is_directory(string fname)
{ struct stat stat_buf;
  if (stat(fname,&stat_buf) != 0) return false;
  return (stat_buf.st_mode & S_IFMT) == S_IFDIR;
}

bool is_file(string fname)
{ struct stat stat_buf;
  if (stat(fname,&stat_buf) != 0) return false;
  return (stat_buf.st_mode & S_IFMT) == S_IFREG;
}


bool create_link(string path, string link)
{ //system("ln -s " + path + " " + link);
  return symlink(path,link) == 0; 
 }


bool is_link(string fname)
{ struct stat stat_buf;
  if (lstat(fname,&stat_buf) != 0) return false;
  return (stat_buf.st_mode & S_IFMT) == S_IFLNK;
}


/*
size_t size_of_file(string fname)
{ struct stat stat_buf;
  if (stat(fname,&stat_buf) != 0) 
     LEDA_EXCEPTION(1,"size_of_file: file does no exist");
  return stat_buf.st_size);
}
*/


/*
time_t time_of_file(string fname)
{ struct stat stat_buf;
  if (stat(fname,&stat_buf) != 0) 
     LEDA_EXCEPTION(1,"time_of_file: cannot access file.");
  // time of last modification
  return stat_buf.st_mtime;
}
*/



string get_directory()
{ char dir_buf[256];
  if (getcwd(dir_buf,256) == NULL)
	  LEDA_EXCEPTION(1, "getcwd failed");
  return dir_buf;
}


string set_directory(string new_dir)
{ string old_dir = get_directory();
  if (is_directory(new_dir)) {
    if (chdir(new_dir))
		LEDA_EXCEPTION(1, string("set_directory: chdir(%s) failed.", ~new_dir));
  } else
    LEDA_EXCEPTION(1,string("set_directory: %s is not a directory.",~new_dir));
  return old_dir;
}

static void read_directory(string dir_name, int what, list<string>& L, 
                                                           char* pat=0)
{ // what == 0: all files
  // what == 1: regular files 
  // what == 2: sub-directories

 L.clear();

 if (!is_directory(dir_name))
 { LEDA_EXCEPTION(1,string("read_directory: %s is not a directory.",~dir_name));
   return;
  }

 DIR* dir_p = opendir(dir_name);

 dirent* dir_e;
 while ( (dir_e = readdir(dir_p)) != NULL )
 { string fname = dir_e->d_name;
/*
   if (what == 2 && fname[0] == '.') continue;
*/
   if (fname == "." || fname == "..") continue;
   if (pat != 0 && fnmatch(pat,fname,0) != 0) continue;
   if (what != 0)
   { string full_name = dir_name + "/" + fname;
     if (what == 1 && !is_file(full_name)) continue;
     if (what == 2 && !is_directory(full_name)) continue;
    }
   L.append(fname);
  }
 closedir(dir_p);
}


string tmp_dir_name()
{ char* td = getenv("TMPDIR");
  return (td) ? td : P_tmpdir;
}
  


string tmp_file_name() 
{ 
 char* td = getenv("TMPDIR");

 char  buf[128];

 random_source rand;

 if (td) 
   sprintf(buf,"%s",td);
 else
   sprintf(buf,"%s",P_tmpdir);

 int n = strlen(buf);
 if (buf[n-1] == '/') n--;

 for(int i = 0; i < 16; i++)
 { unsigned long r = rand.get() | 0x10000000;
   sprintf(buf+n,"/L%lx",r);
   if (!is_file(buf)) break;
  } 

 return buf;
}


bool delete_file(string fname) 
{ string fn = fname.replace_all(" ","\\ ");
  return system("rm -f " + fn) == 0; 
}

bool copy_file(string src, string dest) 
{ string s = src.replace_all(" ","\\ ");
  string d = dest.replace_all(" ","\\ ");
  return system("cp " + s + " " + d) == 0; 
}

bool move_file(string src, string dest) 
{ string s = src.replace_all(" ","\\ ");
  string d = dest.replace_all(" ","\\ ");
   return system("mv " + s + " " + d) == 0; 
}

bool chmod_file(string fname, string option) 
{ string fn = fname.replace_all(" ","\\ ");
  return system("chmod " + option + " " + fn) == 0; 
}


bool open_url(string url)
{ 
   string path = getenv("PATH");

   string cmd;

   if (first_file_in_path("google-chrome",path,':') != "")
     cmd = "google-chrome --app=";
   else
     if (first_file_in_path("firefox",path,':') != "")
        cmd = "firefox ";

   if (cmd == "") return false;

   cmd += url;

 //cout << "cmd = " << cmd << endl;

   return create_process(cmd);
}



bool open_file(string fname) 
{ 
/*
  int pid = fork();
  if (pid == 0) {
    execl("/usr/bin/xdg-open","xdg-open",~fname,(char*)0);
    exit(1);
  }
*/
 return system("/usr/bin/xdg-open " + fname + "&");
}

#endif


#if defined(__win32__) || defined(__win64__)

static string LastErrorMessage(const char* function) 
{ 
    // Retrieve the system error message for the last-error code

    char MsgBuf[1024];
    DWORD dw = GetLastError(); 

    FormatMessage(// FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  dw,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  MsgBuf,
                  1024, NULL );

    // Display the error message and exit the process

    //char buf[2048];

    char* buf = new char[2048];

    //StringCchPrintf((LPTSTR)lpDisplayBuf, 
    sprintf(buf, "%s failed with error %ld: %s", function, dw, MsgBuf); 

    //MessageBox(NULL, buf, TEXT("Error"), MB_OK); 

    return buf;
}


string get_home_directory()
{ string dname;
  if (get_environment("HOME",dname)) 
    return dname;
  else
    return "C:";
}


int get_number_of_cores() 
{ SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
}


list<string> get_disk_drives()
{ char buf[128];
  int sz = GetLogicalDriveStrings(128,buf);
  list<string> drives;
  for(int i = 0; i < sz; i += 4)
  { unsigned t = GetDriveType(buf+i);
    if (t != DRIVE_FIXED && t != DRIVE_REMOVABLE) continue;
/*
    unsigned long sectors_per_cluster = 0;
    unsigned long bytes_per_sector = 0;
    unsigned long free_clusters = 0;
    unsigned long total_clusters = 0;
    if (GetDiskFreeSpace(buf+i,&sectors_per_cluster,
                               &bytes_per_sector,
                               &free_clusters,
                               &total_clusters))
*/
    drives.append(string("%c:",buf[i]));
   }
  return drives;
}


bool create_directory(string dname)
{ //LEDA_EXCEPTION(0,"create_directory: not implemented.");
  return CreateDirectory(dname,NULL) != 0;
}

bool is_directory(string name)
{ DWORD att = GetFileAttributes(name);
  if (att == 0xFFFFFFFF) return false;
  return (att & FILE_ATTRIBUTE_DIRECTORY) != 0; 
 }


bool is_file(string name)
{ DWORD att = GetFileAttributes(name);
  if (att == 0xFFFFFFFF) return false;
  return !(att & FILE_ATTRIBUTE_DIRECTORY); 
  //WIN32_FIND_DATA fd;
  //HANDLE ha = FindFirstFile(name.cstring(),&fd);
  //return (ha && (ha != (HANDLE)0xffffffff);
 }


bool create_link(string path, string link)
{ 
#if !defined(__GNUC__) 
  int flags = 0;
#if _MSC_VER >= 1920
  // >= vs studio 2019
  flags |= SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
#endif
  if (is_directory(path)) flags |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  path = path.replace_all("/","\\");
  // link --> path
  //bool b =  CreateSymbolicLinkA(link,path,flags);
  bool b =  CreateSymbolicLinkA(link,path,flags) == TRUE;
  if (!b) {
    LEDA_EXCEPTION(0,LastErrorMessage("create_link"));
  }
  return b;
#endif
  return false;
}

bool is_link(string) { return false; }

/*
int size_of_file(string fname)
{ 
  WIN32_FIND_DATA fd;
  HANDLE ha = FindFirstFile(fname.cstring(),&fd);
  //if (ha == NULL || ha == (HANDLE)0xffffffff)
  if (ha == NULL)
     LEDA_EXCEPTION(1,"size_of_file: file does no exist");
  return (int)fd.nFileSizeLow;
}
*/

/*
time_t time_of_file(string fname)
{
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;
    DWORD dwRet;

    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
      LEDA_EXCEPTION(1,"time_of_file: cannot access file.");

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

}
*/





string get_directory()
{ char dir_buf[256];
  int len = GetCurrentDirectory(256,dir_buf);
  if (dir_buf[len-1] == '\\') dir_buf[len-1] = '\0';
  return string(dir_buf);
}


string set_directory(string new_dir)
{ string old_dir = get_directory();
  if (is_directory(new_dir))
    SetCurrentDirectory(new_dir.cstring());
  else
    LEDA_EXCEPTION(1,string("set_directory: %s is not a directory.",~new_dir));
  return old_dir;
}



static void read_directory(string dir_name, int what, list<string>& L, 
                                                             char* pat = 0)
{ 
  L.clear();

 if (!is_directory(dir_name))
 { LEDA_EXCEPTION(1,string("read_directory: %s is not a directory.",~dir_name));
   return;
  }

  string cwd = set_directory(dir_name);

  if (pat == 0) pat = (char*)"*";

  WIN32_FIND_DATA fd;
  HANDLE ha = FindFirstFile(pat,&fd);

  //if (ha && ha != (HANDLE)0xffffffff)
  if (ha && ha != INVALID_HANDLE_VALUE)
  { do { string fname = fd.cFileName;
         if (what == 2 && fname[0] == '.') continue;
         bool isdir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
         if (what == 0 || (what == 1 && !isdir) || (what == 2 && isdir))
            L.append(fname);
     } while (FindNextFile(ha,&fd));
    FindClose(ha);
   }

  set_directory(cwd);
}


string tmp_dir_name()
{ char path[128];
  random_source rand;
  GetTempPath(128,path);
  return string(path);
 }


string tmp_file_name()
{ char name[MAX_PATH];
  char path[128];
  random_source rand;
  GetTempPath(128,path);
  time_t clock; 
  time(&clock);
  int r = ((int)clock + rand()) & 0xFFFF;
  GetTempFileName(path,"LEDA",r,name);
  return string(name);
 }


bool delete_file(string fname) 
{ if (!is_file(fname)) return false;
  return (DeleteFile(fname) != 0);
}

bool copy_file(string src, string dest) 
{  return CopyFile(src,dest,FALSE) != 0; }


bool move_file(string src, string dest) 
{  if (MoveFileEx(src,dest,
          MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == 0)
   { //cout << LastErrorMessage("Move " + src) << endl;
     return false;
    }
   return true;
}

bool chmod_file(string fname, string) 
{  return SetFileAttributes(fname,FILE_ATTRIBUTE_NORMAL) != 0; }


string get_open_cmd(const char* suffix)
{ 
  if (string(suffix) == ".html" || string(suffix) == ".htm") 
  { string path1 = 
        "C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe";
    string path2 = 
        "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";
    if (is_file(path1)) return path1;
    if (is_file(path2)) return path2;
   }

  char buf[256];
  unsigned long buf_sz = 256;
  LONG result;
  HKEY hKey;

  RegOpenKeyEx(HKEY_CLASSES_ROOT, suffix, 0, KEY_QUERY_VALUE, &hKey);
  result = RegQueryValueEx(hKey,NULL, NULL,NULL, (unsigned char*)buf, &buf_sz);
  RegCloseKey(hKey);

  if (result != ERROR_SUCCESS) return "";

  strcat(buf,"\\shell\\open\\command");
  buf_sz = 256;

  RegOpenKeyEx(HKEY_CLASSES_ROOT, buf,    0, KEY_QUERY_VALUE, &hKey);
/*
  RegOpenKeyEx(HKEY_CLASSES_ROOT, key,    0, KEY_QUERY_VALUE, &hKey);
  RegOpenKeyEx(hKey,             "shell", 0, KEY_QUERY_VALUE, &hKey);
  RegOpenKeyEx(hKey,              "open", 0, KEY_QUERY_VALUE, &hKey);
  RegOpenKeyEx(hKey,           "command", 0, KEY_QUERY_VALUE, &hKey);
*/
  result = RegQueryValueEx(hKey,NULL, NULL,NULL, (unsigned char*)buf,&buf_sz);
  RegCloseKey(hKey);

  if (result != ERROR_SUCCESS) strcpy(buf,"");

  return buf;
}



bool open_url(string url)
{ 
  string cmd = get_open_cmd(".html");
  if (cmd == "") cmd = get_open_cmd(".htm");

  if (cmd == "") return false;

  int p = cmd.index("chrome.exe");

  if (p != -1)
  { // chrome
    cmd = cmd + " --app=" + url;
    return create_process(cmd);
   }

  return ShellExecute(0,0,url,0,0,SW_SHOW) != NULL;
}



bool open_file(string fname)
{
  return ShellExecute(0,0,fname,0,0,SW_SHOW) != NULL;
}


#endif




list<string> get_entries(string dir) 
{ list<string> L;
  read_directory(dir,0,L);
  return L;
 }

list<string> get_files(string dir)
{ list<string> L;
  read_directory(dir,1,L);
  return L;
 }


list<string> get_files(string dir, string pattern)
{ list<string> L;
  read_directory(dir,1,L,pattern.cstring());
  return L;
 }

list<string> get_directories(string dir)
{ list<string> L;
  read_directory(dir,2,L);
  return L;
 }

list<string> get_directories(string dir, string pattern)
{ list<string> L;
  read_directory(dir,2,L,pattern.cstring());
  return L;
 }




string first_file_in_path(string fname, string path, char sep)
{
  path = path.replace_all(string(sep)," ");
  path += " ";

  while (path.length() > 0)
  { int p = path.index(" ");
    string s = path(0,p-1);
    path = path.del(0,p);
#if defined(__unix__)
    s += "/" + fname;
#else
    s += "\\" + fname;
#endif
    if (is_file(s)) return s; 
   }

  return "";
}




#if defined(__win32__) || defined(__win64__)


/*
bool create_process(string cmd, bool show)
{ int flag = show ? SW_SHOW : SW_HIDE;
  return WinExec(cmd.cstring(),flag) > 31; 
}
*/

bool create_process(string cmd, bool show)
{ 
  STARTUPINFO si;
  memset(&si,0,sizeof(STARTUPINFO));
  
  si.cb=sizeof(STARTUPINFO);
  si.dwFlags=STARTF_USESHOWWINDOW;
  si.wShowWindow= show ? SW_SHOW : SW_HIDE;
  
  PROCESS_INFORMATION pi;
  if (!CreateProcess(NULL,(LPTSTR)~cmd,NULL,NULL,false,0,0,0,&si,&pi)) 
    return false;
    
  WaitForSingleObject(pi.hProcess,0);

/*
  DWORD res;
  GetExitCodeProcess(pi.hProcess,&res);
*/

  CloseHandle(pi.hThread);

  return true;
}


bool create_process_wait(string cmd, bool show)
{ 
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&si,0,sizeof(STARTUPINFO));
  
  si.cb=sizeof(STARTUPINFO);
  si.dwFlags=STARTF_USESHOWWINDOW;
  si.wShowWindow= show ? SW_SHOW : SW_HIDE;
  
  bool result = false;

  if (CreateProcess(NULL,(LPTSTR)~cmd,NULL,NULL,false,0,0,0,&si,&pi)) 
    result = true;
    
  
  WaitForSingleObject(pi.hProcess,INFINITE);

/*
  DWORD res;
  GetExitCodeProcess(pi.hProcess,&res);
  cout << "res = " << res << endl;
*/

  CloseHandle(pi.hThread);

  return result;
}


bool create_process_timeout(string cmd, float sec)
{
  STARTUPINFO si;
  memset(&si,0,sizeof(STARTUPINFO));
  si.cb=sizeof(STARTUPINFO);
  si.dwFlags=STARTF_USESHOWWINDOW;
  si.wShowWindow=SW_HIDE;

  PROCESS_INFORMATION pi;
  if (CreateProcess(NULL,(LPTSTR)~cmd,NULL,NULL,false,0,0,0,&si,&pi))
  { int result = WaitForSingleObject(pi.hProcess,int(1000*sec));
    CloseHandle(pi.hThread);
    return result == 0;
  }

  return false;
}



/*
void message_box(const char* msg, const char* label) { 
  MessageBox(NULL,msg,label,MB_OK);  
}
*/



bool battery_status(int& ac_status, int& percent, int& minutes)
{ 
#if defined(__GNUC__)
  return false;
#else
  SYSTEM_POWER_STATUS s;
  if (!GetSystemPowerStatus(&s)) return false;
  ac_status = s.ACLineStatus;
  percent = s.BatteryLifePercent;
  minutes = s.BatteryLifeTime;
  return true;
#endif
}

void play_sound(string fname)
{
/*
  PlaySound(fname.cstring(),NULL,SND_FILENAME|SND_ASYNC);
*/
}

// console functions


void init_console()
{ HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode_old;
  GetConsoleMode(hstdin,&mode_old);
  DWORD mode = mode_old ^ (ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
  SetConsoleMode(hstdin,mode);
}


char read_console(int msec)
{
  char c = 0;

  HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD rc = WaitForSingleObject(hstdin, msec);

  if (rc == WAIT_OBJECT_0)
  { INPUT_RECORD record[512];
    DWORD n;
    ReadConsoleInput(hstdin, record, 512, &n);
    for(DWORD i = 0; i<n; i++)
    { if (record[i].EventType != KEY_EVENT) continue;
      KEY_EVENT_RECORD key_event = record[i].Event.KeyEvent;
      if (key_event.bKeyDown)
      { c = key_event.uChar.AsciiChar;
        break;
      }
    }
  }
  return c;
}


bool peek_console(int msec)
{ bool result = false;

  HANDLE h_stdin = GetStdHandle(STD_INPUT_HANDLE);

  DWORD rc = WaitForSingleObject(h_stdin, msec);

  if (rc == WAIT_OBJECT_0)
  { INPUT_RECORD record[512];
    DWORD n;
    PeekConsoleInput(h_stdin, record, 512, &n);
    for(DWORD i = 0; i<n; i++)
    { if (record[i].EventType != KEY_EVENT) continue;
      KEY_EVENT_RECORD key_event = record[i].Event.KeyEvent;
      if (key_event.bKeyDown)
      { result = true;
        break;
      }
    }
  }

  return result;
}

void flush_console() {
  HANDLE h_stdin = GetStdHandle(STD_INPUT_HANDLE);
  FlushConsoleInputBuffer(h_stdin);
}



bool fd_poll(int fd, int msec)
{ HANDLE std_input = GetStdHandle(STD_INPUT_HANDLE);
  DWORD result = WaitForSingleObject(std_input,msec);
  return result == WAIT_OBJECT_0;
}

#else

bool create_process(string cmd, bool show)
{ return system(cmd + "&") == 0; }

bool create_process_wait(string cmd, bool show)
{ return system(cmd) == 0; }

bool create_process_timeout(string cmd, float sec)
{ cmd = string("timeout -s KILL %.1f ",sec) + cmd;

  return system(cmd) == 0;
}



bool battery_status(int& ac_status, int& percent, int& minutes)
{
   ifstream apm_str("/proc/apm");

   if (!apm_str) return false;

   char buffer[100];
   apm_str.get(buffer,sizeof(buffer)-1);

   char driver_version[10];
   char apm_version[10];
   char units[10];
   int  apm_flags;
   int  bat_status;
   int  bat_flags;

   sscanf(buffer, "%s %s %x %x %x %x %d%% %d %s",
	  driver_version,
	  apm_version,
	  &apm_flags,  // 0x02: 32bit 0x10: disabled  0x20 disengaged
	  &ac_status,  // 0: off-line 1: on-line 2: backup power
	  &bat_status, // 0: high     1: low     2: critical     3: charging
	  &bat_flags,
	  &percent,
	  &minutes,
	  units);

   if (string(units) == "sec") minutes /= 60;

  return true;
}


void play_sound(string fname) {
   open_file(fname);
}



// console functions

char read_console(int msec)
{ char c = 0;
  if (fd_poll(0,msec)) read(0,&c,1);
  return c;
}

bool peek_console(int msec) { return fd_poll(0,msec); }

void flush_console() { }

bool fd_poll(int fd, int msec)
{
  timeval polltime;
  polltime.tv_sec  = msec / 1000;
  polltime.tv_usec = 1000 * (msec % 1000);

  fd_set rdset,wrset,xset;
  FD_ZERO(&rdset);
  FD_ZERO(&wrset);
  FD_ZERO(&xset);
  FD_SET(fd,&rdset);
  FD_SET(fd,&xset);

  return select(fd+1,&rdset,&wrset,&xset,&polltime) > 0;
}



#endif

/*
bool open_url(string url)
{ char buf[256];
  //string cmd = get_open_cmd(".htm");
  string cmd = get_open_cmd(".html");

  string quoted_url = "\"" + url + "\"";

  if (cmd.pos("%1") != -1)
     cmd = cmd.replace("%1",quoted_url);
  else
     cmd = cmd + " " + quoted_url;

  return create_process(cmd);
 }
*/





/*
size_t size_of_file(string fname)
{ ifstream str(fname, ios::binary);
  if (!str.good()) LEDA_EXCEPTION(1,"size_of_file: file does no exist");
  str.seekg(0,ios::end);
  size_t bytes = (size_t)str.tellg();
  str.seekg(0,ios::beg);
  return bytes;
}
*/

size_t size_of_file(string fname)
{ struct stat stat_buf;
  if (stat(fname,&stat_buf) != 0) 
  { perror("STAT ERROR: ");
    return 0;
   }
  //off_t sz = stat_buf.st_size;
  return (size_t)stat_buf.st_size;
}


#if defined(__linux__)
unsigned long long size_of_file64(string fname)
{ struct stat64 stat_buf;
  if (stat64(fname,&stat_buf) != 0) 
  { perror("STAT ERROR: ");
    return 0;
   }
  return stat_buf.st_size;
}
#else
unsigned long long size_of_file64(string fname)
{ struct __stat64 stat_buf;
  if (_stat64(fname,&stat_buf) != 0) 
  { perror("STAT ERROR: ");
    return 0;
   }
  return stat_buf.st_size;
}
#endif



time_t time_of_file(string fname)
{ struct stat stat_buf;
  if (stat(fname,&stat_buf) != 0) 
     return 0;
  else
     return stat_buf.st_mtime;
}


int compare_files(string fname1, string fname2)
{
  size_t sz1 = size_of_file(fname1);
  size_t sz2 = size_of_file(fname2);

  if (sz1 != sz2) return 1;

  ifstream istr1(fname1, ios::binary);
  ifstream istr2(fname1, ios::binary);

  while (istr1 && istr2)
    if (istr1.get() != istr2.get()) return 1;

  return 0;
}


LEDA_END_NAMESPACE


