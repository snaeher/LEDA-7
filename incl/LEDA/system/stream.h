/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  stream.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_STREAM_H
#define LEDA_STREAM_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721166
#include <LEDA/internal/PREAMBLE.h>
#endif


#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

typedef ifstream file_istream;
typedef ofstream file_ostream;


/*
typedef istringstream isstream;
typedef ostringstream osstream;
*/



/*{\Manpage {file_istream} {} {File Input Streams} }*/
/*{\Mdefinition
The data type |file_istream| is equivalent to the |ifstream| type of \CC. }*/


/*{\Manpage {file_ostream} {} {File Output Streams} }*/
/*{\Mdefinition
The data type |file_istream| is equivalent to the |ofstream| type of \CC. }*/


/*{\Manpage {string_istream} {} {String Input Streams} }*/

struct string_istream : public istringstream {

/*{\Mdefinition
An instance $I$ of the data type $string\_istream$ is an \CC istream
connected to a string $s$, i.e., all input operations or operators
applied to $I$ read from $s$. }*/

/*{\Mcreation I }*/


/*{\Moptions nextwarning=no}*/
	string_istream(const char* s)  : istringstream((char*)s) {}
	string_istream(string s) : istringstream((char*)~s) {}
/*{\Mcreate   creates an instance $I$ of type string\_istream connected to 
              the string $s$. }*/

string_istream(int argc, char** argv) : istringstream((char*)~string(argc,argv)) {}


/*{\Moperations 2 5.5 }*/

/*{\Mtext
All operations and operators ($>>$) defined for \CC istreams can
be applied to string input streams as well.
}*/


};


/*{\Manpage {string_ostream} {} {String Output Streams} }*/

struct string_ostream : public ostringstream {

/*{\Mdefinition
    An instance $O$ of the data type $string\_ostream$ is an \CC ostream
    connected to an internal string buffer, i.e., all output operations 
    or operators applied to $O$ write into this internal buffer.  The current
    value of the buffer is called the contents of $O$.}*/


/*{\Mcreation O }*/

string_ostream() {};
/*{\Mcreate creates an instance $O$ of type string\_ostream.}*/


/*{\Moperations 2 5.5 }*/

/*{\Moptions nextwarning=no}*/
string str()     { return ostringstream::str().c_str(); };
/*{\Mop   returns the current contents of $O$.}*/

/*{\Mtext
All operations and operators ($<<$) defined for \CC ostreams can
be applied to string output streams as well.
}*/

};


/*{\XManpage {cmd_istream} {} {Command Input Streams} }*/


// #if !defined(__unix__) || defined(__STRICT_ANSI__) || defined(__KCC)

#if 1

typedef file_ostream cmd_ostream;
typedef file_istream cmd_istream;

#else

#if defined(__mipspro__) && defined(_STANDARD_C_PLUS_PLUS)

// This code has been taken and modified (by Michael Hoffman) from 
// fstream. Copyright (c) 1998 Silicon Graphics Computer Systems, Inc.

// begin of code segment from SGI fstream
struct cmd_istream : public istream
{
  FILE* _f;
  std::filebuf _buf;

  cmd_istream(char* cmd) : istream(0), _f(popen(cmd,"r")) { 
    if (!_buf.open(fileno(_f)))
      setstate(std::ios_base::failbit);
  } 

  cmd_istream(string cmd) : istream(0), _f(popen(~cmd,"r")) { 
    if (!_buf.open(fileno(_f)))
      setstate(std::ios_base::failbit);
  }

  ~cmd_istream()               { if (is_open()) close(); }

  std::filebuf* rdbuf() const  { return const_cast<std::filebuf*>(&_buf); }

  bool is_open() const         { return rdbuf()->is_open(); }

  void open(const char* s, std::ios_base::openmode mod = std::ios_base::in) {
    if (!rdbuf()->open(s, mod | std::ios_base::in))
      setstate(std::ios_base::failbit);
  }

  void close() { 
    if (!rdbuf()->close() || !pclose(_f)) 
      setstate(std::ios_base::failbit); 
  }

}; 
// end of code segment from SGI fstream

#else

struct cmd_istream : public ifstream 
{ 
/*{\Xdefinition
An instance $I$ of the data type $cmd\_istream$ is an \CC istream
connected to the output of a shell command $cmd$, i.e., all input operations
or operators applied to $I$ read from the standard output of command $cmd$. }*/

/*{\Xcreation I }*/

/*{\Xoptions nextwarning=no}*/
  cmd_istream(char*  cmd) : ifstream(fileno(popen(cmd,"r"))) {} 
  cmd_istream(string cmd) : ifstream(fileno(popen(~cmd,"r"))) {}
/*{\Xcreate   creates an instance $I$ of type cmd\_istream connected to the 
              output of command $cmd$. }*/


/*{\Xoperations 2 5.5 }*/

/*{\Xtext
All operations and operators ($>>$) defined for \CC istreams can
be applied to command input streams as well.
}*/

 };
#endif




/*{\Xanpage {cmd_ostream} {} {Command Output Streams} }*/

//#if defined(__mipspro__) && defined(_STANDARD_C_PLUS_PLUS)

#if 1

// This code has been taken and modified (by Michael Hoffman) from 
// fstream. Copyright (c) 1998 Silicon Graphics Computer Systems, Inc.

// begin of code segment from SGI fstream
struct cmd_ostream : public ostream
{
  FILE* _f;
  std::filebuf _buf;

  cmd_ostream(char*  cmd) : ostream(0), _f(popen(cmd,"w")) { 
    if (!_buf.open(fileno(_f)))
      setstate(std::ios_base::failbit); 
  } 

  cmd_ostream(string cmd) : ostream(0), _f(popen(~cmd,"w")) { 
    if (!_buf.open(fileno(_f)))
      setstate(std::ios_base::failbit); 
  }

  ~cmd_ostream()               { if (is_open()) close(); }


  std::filebuf* rdbuf() const  { return const_cast<std::filebuf*>(&_buf); }

  bool is_open() const         { return rdbuf()->is_open(); }

  void open(const char* s, std::ios_base::openmode mod = std::ios_base::out) {
    if (!rdbuf()->open(s, mod | std::ios_base::in))
      setstate(std::ios_base::failbit);
  }

  void close() { 
    if (!rdbuf()->close() || !pclose(_f)) 
      setstate(std::ios_base::failbit); 
  }

}; 
// end of code segment from SGI fstream

#else

struct cmd_ostream : public ofstream 
{
/*{\Xdefinition
An instance $O$ of the data type $cmd\_ostream$ is an \CC ostream
connected to the input of a shell command $cmd$, i.e., all output operations
or operators applied to $O$ write into the standard input of command $cmd$. }*/

/*{\Xcreation O }*/

/*{\Xoptions nextwarning=no}*/
cmd_ostream(char*  cmd) : ofstream(fileno(popen(cmd,"w"))) {}
cmd_ostream(string cmd) : ofstream(fileno(popen(~cmd,"w"))) {}
/*{\Xcreate
creates an instance $O$ of type cmd\_ostream connected to the input of 
command $cmd$.  }*/


/*{\Xoperations 2 5.5 }*/

/*{\Xtext
All operations and operators ($<<$) defined for \CC ostreams can
be applied to command output streams as well.
}*/

};

#endif

#endif


#if LEDA_ROOT_INCL_ID == 721166
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE


#endif

