/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _string.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/string.h>

#include <cstdlib>
#include <string.h>
#include <stdarg.h>

#include <assert.h>


LEDA_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// String
//------------------------------------------------------------------------------

string::string() 
{ PTR = new string_rep(""); }

string::string(const char* p)
{ PTR = new string_rep(p);}

string::string(char c) 
{ PTR = new string_rep(c);  }

string::string(int argc, char** argv) 
{ PTR = new string_rep(str_ncat(argc,argv)); }


string_rep::string_rep()  
{ s = 0;
  len = 0;
}

string_rep::string_rep(const char* p) 
{ s = string::str_dup(p); 
  len = strlen(s);
 }

string_rep::string_rep(const char* p, const char* q) 
{ s = string::str_cat(p,q); 
  len = strlen(s);
 }

string_rep::string_rep(char c)   
{ char p[2];
  p[0] = c;
  p[1] = '\0';
  s=string::str_dup(p); 
  len = strlen(s);
 }

char* string::str_dup(const char* p)
{ if (p==nil) LEDA_EXCEPTION(1,"string::str_dup: nil argument");
  size_t len = strlen(p);
  char* q = new char[len+1];
  //char* q = (char*)std_memory.allocate_bytes(len+1);
  if (q==nil) LEDA_EXCEPTION(1,"string::str_dup: out of memory");
  strcpy(q,p);
  return q;
}

char* string::str_cat(const char* p1, const char* p2)
{ char* q = new char[strlen(p1)+strlen(p2)+1];
  //char* q = (char*)std_memory.allocate_bytes(strlen(p1)+strlen(p2)+1);
  if (q==nil) LEDA_EXCEPTION(1,"string::str_cat: out of memory");
  strcpy(q,p1);
  strcat(q,p2);
  return q;
 }

char* string::str_ncat(int argc, char** argv)
{ size_t l=0;
  int i;
  for(i=0;i<argc;i++)  l += (strlen(argv[i])+1); 
  char* q = new char[l+1];
  //char* q = (char*)std_memory.allocate_bytes(l+1);
  if (q==nil) LEDA_EXCEPTION(1,"string::str_cat: out of memory");
  q[0] = 0;
  for(i=0;i<argc;i++)
  { strcat(q,argv[i]);
    strcat(q," ");
   }
  return q;
 }

int string::cmp(const char* s1, const char* s2) { return strcmp(s1,s2); }



void string::read(istream& s, char delim)
{ char buf[1024];
  char* q = buf+1023;
  bool go = true;

  for(int i=0;i<1024;i++) buf[i] = 0;

  operator=(""); // clear string

  while (s && go)
  { char* p;
    for(p = buf; p < q && s.get(*p); p++)
    { if (*p == delim || (delim == 0 && isspace(*p)))
      { if (delim != '\n') s.putback(*p);
        go = false;
        break;
       }
     }

    // fix for dos line delimiter ('\r\n')
    if (*(p-1) == '\r') p--;

    *p = '\0';
    operator+=(buf);
   }

}


// printf-like constructor

#define STRING_CONSTRUCTOR(type)                                   \
string::string(const char* f, type x, ...) {                       \
  char buf1[512];                                                  \
  char buf2[512];                                                  \
  char* q = buf1;                                                  \
  int first = 1;                                                   \
  while (*f != '\0')                                               \
    if ((*q++ = *f++) == '%')                                      \
    { if (!first) *q++ = '%';                                      \
      else if (*f == '*') { strcpy(q,"%%d"); q += 3; f++; }        \
      first = 0; }                                                 \
  *q = '\0';                                                       \
  sprintf(buf2,buf1,x);                                            \
  va_list arg_list;                                                \
  va_start(arg_list,x);                                            \
  vsprintf(buf1,buf2,arg_list);                                    \
  PTR = new string_rep(buf1);                                      }


STRING_CONSTRUCTOR(int)
STRING_CONSTRUCTOR(unsigned int)
STRING_CONSTRUCTOR(long)
STRING_CONSTRUCTOR(unsigned long)
STRING_CONSTRUCTOR(double)
STRING_CONSTRUCTOR(const char*)
STRING_CONSTRUCTOR(void*)


/*
string::string(const char* format, ...)
{ va_list args;

  va_start(args,format);
  int len = vsnprintf(nullptr,0,format,args);
  va_end(args);

  char* buf = new char[len+1];

  va_start(args,format);
  vsprintf(buf,format,args);
  va_end(args);

  //PTR = new string_rep((char*)0);
  PTR = new string_rep;
  ptr()->s = buf;
  ptr()->len = len;
 }
*/


string& string::operator+=(const string& x) 
{ const char* p = cstring(); 
  const char* q = x.cstring();
  *this = string(new string_rep(p,q));
  return *this; 
 }


istream& operator>>(istream& in, string& x)
{ //skip leading white space (but not eol if in = cin)
  char c;
  while (in.get(c) && isspace(c) && (&in != &cin || c != '\n')); 
  if (in && c != '\n') in.putback(c);
  x.read(in,0);
  return in;
}


ostream& operator<<(ostream& out, const string& x) 
{ return out << x.cstring(); }


string string::substring(int first, int stop) const
{
  // returns S[first ... stop-1]

  if (stop > length()) stop = length();

  if (first < 0) first = 0;

  int len = stop-first;

  if (len <= 0)  return string("");

  char* buf = new char[len+1];
  strncpy(buf,cstring()+first,len);
  buf[len] = '\0';

  string result(buf);
  delete[] buf;

  return result;
}


int string::index(string x, int i) const
{
  int s_len = length();
  int x_len = x.length();

  if (x_len == 0 || i < 0 || i >= s_len) return -1;

  char* sp = cstring();
  char* xp = x.cstring();

/*
  int result = -1;
  for(int j=i; j<=(s_len-x_len); j++){
    if (strncmp(sp+j,xp,x_len) == 0) { result = j; break; }
  }
  return result;
*/

  const char* q = strstr(sp+i,xp);
  return q ? int(q-sp) : -1;
}


int string::index(char c, int i) const
{ if (i < 0 || i >= length()) return -1;
/*
  int len = length();
  while (i<length() && char_at(i) != c) i++;
  return (i<len) ? i : -1;
*/

  const char* p = cstring();
  const char* q = strchr(p+i,c);
  return q ? int(q-p) : -1;
 }

int string::last_index(string x, int i) const
{
  char* sp = cstring();
  char* xp = x.cstring();

  int s_len = length();
  int x_len = x.length();

  if (x_len == 0 || i < 0 || i >= s_len) return -1;

  int result = -1;

  for(int j=i; j>=0; j--) {
    if (strncmp(sp+j,xp,x_len) == 0) { result = j; break; }
  }

  return result;
}


int string::last_index(char c, int i) const
{ if (i < 0 || i >= length()) return -1;
  const char* p = cstring();
  const char* q = strrchr(p+i,c);
  return q ? int(q-p) : -1;
 }



string string::insert(string s, int i) const
{ return substring(0,i) + s + substring(i,length()); }

string string::insert(int i, string s) const
{ return substring(0,i) + s + substring(i,length()); }


string string::del(int i, int j) const
{ return substring(0,i) + substring(j+1,length()); }

string string::del(const string& s, int n) const
{ return replace(s,"",n); }

string string::trim() const
{ int left = 0;
  int right = length()-1;
  while (left <= right && isspace(char_at(left))) ++left;
  while (left <= right && isspace(char_at(right))) --right;
  return substring(left, right+1);
}

string string::trim(char x) const
{ int left = 0;
  int right = length()-1;
  while (left <= right && char_at(left)  == x) ++left;
  while (left <= right && char_at(right) == x) --right;
  return substring(left, right+1);
}


string string::replace(int i, int j, const string& s) const
{ return substring(0,i) + s + substring(j+1,length()); }

string string::replace(const string& s1, const string& s2, int n) const 
{ 
  // replace n-th (all if n=0) occurrence of s1 by s2 

  if (s1.length() == 0) return *this;

  int i = 0;
  int match = 0;  

  int l1 = s1.length();

  string tmp;

  for(;;)
  { int j = index(s1,i);
    if (j < 0 ) break;
    tmp += substring(i,j);

    if (n==0 || ++match == n)
       tmp += s2;
    else
       tmp += s1;

    i = j+l1;
   }

  tmp += substring(i,length());

  return tmp;

 }


string string::format(string f) const
{ char buf[512];
  sprintf(buf,~f,cstring());
  return string(buf);
 }


string string::to_lower() const
{ int n = length();
  char* str = new char[n+1];
  for(int i=0; i<n; i++) str[i] = (char)tolower(char_at(i));
  str[n] = '\0';
  string result(str);
  delete[] str;
  return result;
}

string string::to_upper() const
{ int n = length();
  char* str = new char[n+1];
  for(int i=0; i<n; i++) str[i] = (char)toupper(char_at(i));
  str[n] = '\0';
  string result(str);
  delete[] str;
  return result;
}

string string::expand_tabs(int tab_w) const
{ 
  int k = count_words('\t');

  if (k <= 1) return *this;

  string* A = new string[k];

  int n = split(A,k,'\t');

  assert(n == k);

  string line;

  for(int i=0; i<n; i++)
  { string s = A[i] + " ";
    line += s;
    while (line.length()%tab_w) line += " ";
   }

   delete[] A;

  return line;
}




#define CHAR_MATCHES(i,c) \
((c != -1) ? (char_at(i) == c) : isspace(char_at(i)))


int string::count_words(char sep) const
{ 
  int count = 0;

  int pos1 = 0;
  int len = length();

  if (sep == -1) {
    while (pos1 < len && CHAR_MATCHES(pos1,sep)) pos1++;
  }

  while (pos1 < len)
  { int pos2 = pos1;
    while (pos2 < len && !CHAR_MATCHES(pos2,sep)) pos2++;
    count++;
    pos1 = pos2+1;
    if (sep == -1) {
      while (pos1 < len && CHAR_MATCHES(pos1,sep)) pos1++;
    }
  }

  return count;
}


int string::split(string* A, int sz, char sep) const
{ 
  int len = length();

  int pos1 = 0;

  if (sep == -1) {
    while (pos1 < len && CHAR_MATCHES(pos1,sep)) pos1++;
  }

  int i = 0;
  while (pos1 < len)
  { int pos2 = pos1;
    while (pos2 < len && !CHAR_MATCHES(pos2,sep)) pos2++;
    if (i < sz) A[i++] = substring(pos1,pos2);
    pos1 = pos2+1;
    if (sep == -1) {
      while (pos1 < len && CHAR_MATCHES(pos1,sep)) pos1++;
    }
  }

  return i;
}


string string::next_word(int& i, char sep) const
{
  // extract next word (everything until next sep-character)
  // starting at index i and move i to next position
  // sep = -1 :  white space

  int len = length();
 
  if (i < 0 || i > len) { i = -1; return ""; }

  if (sep != -1 && char_at(i) == sep) {
     i++;
     return "";
  }

  int p = i;
  if (sep == -1) {
    while (p < len && CHAR_MATCHES(p,sep)) p++;
  }
  if (p >= len) { i = -1; return ""; }

  int q = p+1;
  while (q < len && !CHAR_MATCHES(q,sep)) q++;

  i = q+1;

  string result = substring(p,q);

  if (sep == -1) {
    // white space separator
    result = result.trim();
  }

  return result;
}




string string::utf_to_iso8859() const
{ int n = length();
  char* str = new char[n+1];
  char* p = str;
  for(int i=0;i<n; i++)
  { signed char c = char_at(i);
    if (c == -61) c = char_at(++i) + 64;
    *p++ = c;
   }
  *p = '\0';
  string result(str);
  delete[] str;
  return result;
}


string string::iso8859_to_utf() const
{ int n = length();
  char* str = new char[2*n+1];
  char* p = str;
  char ae = char(0xE4);
  char oe = char(0xF6); 
  char ue = char(0xFC); 
  char ss = char(0xDF);
  for(int i=0;i<n; i++)
  { char c = char_at(i);
    if (c == ae || c == oe || c == ue || c == ss)
    { *p++ = -61;
      c -= 64;
     }
    *p++ = c;
   }
  *p = '\0';
  string result(str);
  delete[] str;
  return result;
}


// parse and return integer

int    string::atoi() const { return std::atoi(cstring()); }
double string::atof() const { return std::atof(cstring()); }



unsigned long ID_Number(const string&)
{ LEDA_EXCEPTION(1,"map<I,E>: Index type I=string not allowed."); 
  return 0;
 }

LEDA_END_NAMESPACE
