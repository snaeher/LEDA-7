/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  string.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>

#ifndef LEDA_STRING_H
#define LEDA_STRING_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 722053
#include <LEDA/internal/PREAMBLE.h>
#endif

LEDA_BEGIN_NAMESPACE

class string;

//LEDA_BEGIN_NAMESPACE

class __exportC string_rep : public handle_rep {

      char*   s;
    //int     len;
      size_t  len;

 string_rep(const char*);
 string_rep(const char*,const char*); //concat
 string_rep(char);
 string_rep();

public:

~string_rep() 
 { delete[] s; 
   //std_memory.deallocate_bytes(s,len);
  }

friend class __exportC string;
};




/*{\Manpage {string} {} {Strings} {s}}*/

class __exportC string  : public HANDLE_BASE(string_rep)
{

/*{\Mdefinition An instance $s$ of the data type $string$ is a
sequence of characters (type $char$).  The number of characters in the
sequence is called the length of $s$. A string of length zero is
called the empty string.  Strings can be used wherever a \CC{}
|const char*| string can be used. 

$Strings$ differ from the \CC{} type $char*$ in several aspects:
parameter passing by value and assignment works properly (i.e., the
value is passed or assigned and not a pointer to the value) and
$strings$ offer many additional operations.  }*/

 friend class __exportC string_rep;
 friend class __exportC window;

 static char* str_dup(const char*);
 static char* str_cat(const char*,const char*);
 static char* str_ncat(int, char**);

 string(string_rep* rep) { PTR = rep; }  // for private use only

 string_rep* ptr() const { return (string_rep*)PTR; }
public:

/*{\Mtypes 5}*/ 

 typedef int size_type;
/*{\Mtypemember the size type.}*/ 

 static int cmp(const char*, const char*);

/*{\Mcreation s }*/

string();
/*{\Mcreate introduces a variable $s$ of type $string$. $s$ is initialized 
            with the empty string.}*/

string(const char* p);
/*{\Mcreate introduces a variable $s$ of type $string$. $s$ is initialized 
            with a copy of the \CC{} string $p$.}*/


explicit
string(char c);
/*{\Mcreate introduces a variable $s$ of type $string$. $s$ is initialized 
            with the one-character string ``$c$''.}*/

string(int argc, char** argv);


/*{\Moptions nextwarning=no}*/
/*
string(const char* format, ...); 
*/
/*{\Mcreate introduces a variable $s$ of type $string$. $s$ is initialized 
            with the string produced by printf($format$,\dots). }*/


 string(const char*, int, ...);
 string(const char*, unsigned int, ...);
 string(const char*, long, ...);
 string(const char*, unsigned long, ...);
 string(const char*, double, ...);
 string(const char*, const char*, ...);
 string(const char*, void*, ...);



 string(const string& x) : HANDLE_BASE(string_rep)(x)  {}
~string() {}

string& operator=(const string& x) 
{ HANDLE_BASE(string_rep)::operator=(x); return *this; }


/*{\Moperations 2. 4.7 }*/

// conversion
const char* operator~() const { return ptr()->s; }
operator const char*()  const { return ptr()->s; }
char* cstring()         const { return ptr()->s; }
const char* c_str()     const { return ptr()->s; }

int length() const  { return (int)ptr()->len; }
/*{\Mop        returns the length of string $s$.}*/

bool empty() const  { return length() == 0; }
/*{\Mop        returns whether |\Mvar| is the empty string.}*/

char  char_at(int i) const  { return ptr()->s[i]; }
/*{\Mop     returns the character at position $i$.\\ 
	    \precond{$0 \le i \le s$.length()-1.} }*/


char  operator[](int i) const { return char_at(i); }
/*{\Marrop     returns $\Mvar.char\_at(i)$. }*/


char& operator[](int i)
{ if (refs() > 1) *this = cstring();    //disconnect
  return ptr()->s[i];
}
/*{\Marrop     returns a reference to the character at position $i$.\\ 
	       \precond{$0 \le i \le s$.length()-1.} }*/

string substring(int i, int j) const;
/*{\Mop       returns the substring of $s$ starting at 
	      position $\max(0,i)$ and ending at 
              position $\min(j,s$.length()$)-1$.
	     }*/

string substring(int i) const { return substring(i,length()); }
/*{\Mop       returns the substring of $s$ starting at position $\max(0,i)$.
	     }*/


string operator()(int i, int j)  const { return substring(i,j+1); }
/*{\Mfunop    returns the substring of $s$ starting at 
	      position $\max(0,i)$ and ending at 
              position $\min(j,s$.length()$-1)$.\\
              If $\min(j,s$.length()$-1) < \max(0,i)$
              then the empty string is returned.  
	     }*/

string head(int i) const 
{ return (i >= 0) ? substring(0,i) : substring(0,length()+i); }
/*{\Mop       returns the first $i$ characters of $s$ if
              $i \ge 0$ and the first ($length()+i$) characters of $s$
              if $i < 0$. }*/

string tail(int i) const
{ return (i >= 0) ? substring(length()-i,length()) : substring(-i,length()); }
/*{\Mop       returns the last $i$ characters of $s$ if
              $i \ge 0$ and the last ($length()+i$) characters of $s$
              if $i < 0$. }*/



int index(string x, int i) const;
/*{\Mop       returns the minimum $j$ such that $j \ge i$ and $x$
              is a substring of $s$ starting at position $j$
	      (returns -1 if no such $j$ exists).}*/

int index(const string& x) const { return index(x,0); }
/*{\Mop       returns |\Mvar|.index$(x,0)$.}*/

int index(char c, int i) const;
/*{\Mop       returns the minimum $j$ such that $j \ge i$ and $s[j] = c$
	      (-1 if no such $j$ exists).}*/

int index(char c) const { return index(string(c),0); }
/*{\Mop       returns |\Mvar|.index$(c,0)$.}*/



int last_index(string x, int i) const;
/*{\Mop       returns the maximum $j$ such that $j \le i$ and $x$
              is a substring of $s$ starting at position $j$
	      (returns -1 if no such $j$ exists).}*/

int last_index(const string& x) const { return last_index(x,length()-1); }
/*{\Mop       returns |\Mvar|.last\_index$(x,\Mvar.length()-1)$.}*/

int last_index(char c, int i) const;
/*{\Mop       returns the maximum $j$ such that $j \le i$ and $s[j] = c$
	      (-1 if no such $j$ exists).}*/

int last_index(char c) const { return last_index(string(c),length()-1); }
/*{\Mop       returns |\Mvar|.last\_index$(c,\Mvar.length()-1)$.}*/


string next_word(int& i, char sep) const;
/*{\Mop       returns word (substring separated by sep characters)
              starting at index $i$ and assigns start of next 
              word to $i$ (-1 if not existing).}*/


int split(string* A, int sz, char sep=-1) const; 
/*{\Mop       splits $s$ into substrings separated by $sep$ characters
              or white space (if $sep = -1$) and stores them in the 
              array $A[0..sz-1]$. The operation returns the number of 
              created substrings (at most $sz$).\\
              \precond{$A$ is an array of length $sz$.} }*/

int count_words(char sep=-1) const; 
/*{\Mop       returns the number of substrings separated by $sep$ 
              characters or white space (if $sep = -1$). }*/


int break_into_words(string* A, int sz) const { return split(A,sz,-1); }
/*{\Mop       breaks $s$ into words separated by white space characters
              and stores them in the array $A$. Same as $s$.split$(A,sz,-1)$ }*/


string expand_tabs(int tab_w) const;
/*{\Mop       return the result of expanding all tabs in $s$ using 
              tabulator width $tab\_w$. }*/


// for backward compatibility

char get(int i) const  { return char_at(i); }
int pos(string x, int i) const { return index(x,i); }
int pos(string x) const { return index(x); }
int pos(char c, int i) const { return index(c,i); }
int pos(char c) const { return index(c); }



/*{\Moptions nextwarning=no}*/
bool contains(const string& x) const { return index(x) >= 0; }
/*{\Mop       true iff $x$ is a substring of |\Mvar|.}*/


bool starts_with(const string& x) const  
{ //return x == substring(0,x.length()); 
  return index(x) == 0; 
}
/*{\Mop       true iff |\Mvar| starts with $x$. }*/

bool begins_with(const string& x) const { return starts_with(x); }
/*{\Mop       true iff |\Mvar| starts with $x$. }*/


bool ends_with(const string& x) const 
{ return x == substring(length()-x.length()); }
/*{\Mop       true iff |\Mvar| starts with $x$. }*/


/*{\Moptions nextwarning=no}*/
string insert(string x, int i =0)     const;
string insert(int i, string x)       const;
/*{\Mop       returns $s(0,i-1)$ + $s_1$ + $s(i,s$.length()$-1)$.}*/


string insert(char c, int i=0) const { return insert(string(c),i); }
string insert(int i, char c)   const { return insert(i,string(c)); }



string replace(const string& s1, const string& s2, int i=1) const;
/*{\Mopl      returns the string created from $s$ by replacing 
	      the $i$-th occurrence of $s_1$ in $s$ by $s_2$. \\
		  Remark: The occurences of $s_1$ in $s$ are counted in a 
		  non-overlapping manner, for instance the string $sasas$ 
		  contains only one occurence of the string $sas$.}*/

string replace(int i, int j , const string& x) const;
/*{\Mopl     returns the string created from $s$ by replacing 
	      $s(i,j)$ by $x$.\\ \precond{$i \leq j$.} }*/

string replace(int i, const string& x) const { return replace(i,i,x);  }
/*{\Mopl      returns the string created from $s$ by replacing
	      $s[i]$ by $x$. }*/

string replace_all(const string& s1, const string& s2) const 
{ return replace(s1,s2,0); }
/*{\Mopl      returns the string created from $s$ by replacing  
	      all occurrences of $s_1$ in $s$ by $s_2$.\\
              \precond{The occurrences of $s_1$ in $s$ 
               do not overlap (it's hard to say what the function returns
               if the precondition is violated.).} }*/

string replace(char c, const string& s2, int i=1) const
{ return replace(string(c),s2,i); }

string replace_all(char c, const string& s2) const 
{ return replace(c,s2,0); }

int atoi() const;
/*\Mop        returns the int value represented by |\Mvar|
              (zero if it is not the representation of an int). }*/ 

double atof() const;
/*\Mop        returns the double value represented by |\Mvar|
              (zero if it is not the representation of a double). }*/ 

bool equals(const string& x) const
{ return string::cmp(cstring(),x.cstring()) == 0; }
/*\Mop        returns true iff |\Mvar| and $x$ are equal. }*/ 


string to_lower() const;
/*\Mop        returns a lower-character copy of |\Mvar|. }*/ 

string to_upper() const;
/*\Mop        returns an upper-character copy of |\Mvar|. }*/ 

string utf_to_iso8859() const;
/*\Mop        returns an iso8859 encoded copy of |\Mvar|. }*/ 

string iso8859_to_utf() const;
/*\Mop        returns an utf encoded copy of |\Mvar|. }*/ 

string del(const string& x , int i=1) const;
/*{\Mopl       returns $s$.replace($x,"",i$). }*/

string del(int i , int j) const;
/*{\Mop       returns $s$.replace($i,j,""$). }*/

string del(int i) const  { return del(i,i); }
/*{\Mop       returns $s$.replace($i,""$). }*/

string del_all(const string& x) const  { return del(x,0); }
/*{\Mop       returns $s$.replace\_all($x,""$). }*/

string trim() const;
/*/Mop        returns a copy of |\Mvar| whith all leading and trailing 
              whitespace removed. }*/ 

string trim(char x) const;
/*/Mop        returns a copy of |\Mvar| whith all leading and trailing 
              characters equal to $x$ removed. }*/ 


void   read(istream& I, char delim = ' ');
/*{\Mopl      reads characters from input stream $I$ into $s$ 
              until the first occurrence of character $delim$.
			  (If |delim| is '$\backslash$ $n$' it is extracted 
			  from the stream, otherwise it remains there.)}*/

void   read(char delim = ' ')           { read(cin,delim); }
/*{\Mop       same as $s$.read($cin$,$delim$). }*/

void   read_line(istream& I)    { read(I,'\n'); }
/*{\Mop       same as $s$.read($I$,'$\backslash$ $n$'). }*/

void   read_line() { read_line(cin); }
/*{\Mop       same as $s$.read\_line($cin$). }*/


void   read_file(istream& I)    { read(I,(char)EOF); }
/*{\Mop       same as $s$.read($I$,'EOF'). }*/

void   read_file() { read_file(cin); }
/*{\Mop       same as $s$.read\_file($cin$). }*/


string format(string) const;

string& operator+=(const string& x);
/*{\Mbinop     appends $x$  to $s$ and returns a reference to $s$.}*/

string& operator+=(char c) { return operator+=(string(c)); }



friend string operator+(const string& x ,const string& y)
{ string s=x; s+=y; return s; }
/*{\Mbinopfunc     returns the concatenation of $x$ and $y$.}*/


friend string operator+(const string& x, char c)
{ string s=x; s+=c; return s; }



friend bool operator==(const string& x, const string& y)
{ return string::cmp(x.cstring(),y.cstring())==0; }
/*{\Mbinopfunc     true iff $x$ and $y$ are equal.}*/

friend bool operator==(const string& x, const char* y)
{ return string::cmp(x.cstring(),y)==0; }

friend bool operator==(const char* x, const string& y)
{ return string::cmp(x,y.cstring())==0; }



friend bool operator!=(const string& x, const string& y)
{ return string::cmp(x.cstring(),y.cstring())!=0; }
/*{\Mbinopfunc     true iff $x$ and $y$ are not equal.}*/

friend bool operator!=(const string& x, const char* y)
{ return string::cmp(x.cstring(),y)!=0; }

friend bool operator!=(const char* x, const string& y)
{ return string::cmp(x,y.cstring())!=0; }



friend bool operator< (const string& x, const string& y)
{ return string::cmp(x.cstring(),y.cstring())<0; }
/*{\Mbinopfunc     true iff $x$ is lexicographically smaller than $y$.}*/

friend bool operator> (const string& x, const string& y)
{ return string::cmp(x.cstring(),y.cstring())>0; }
/*{\Mbinopfunc     true iff $x$ is lexicographically greater than $y$.}*/

friend bool operator<=(const string& x, const string& y)
{ return string::cmp(x.cstring(),y.cstring())<=0; }
/*{\Mbinopfunc     returns $(x < y)\ \Lvert \ (x == y)$.}*/

friend bool operator>=(const string& x, const string& y)
{ return string::cmp(x.cstring(),y.cstring())>=0; }
/*{\Mbinopfunc     returns $(x > y)\ \Lvert \ (x == y)$.}*/


friend __exportF istream& operator>>(istream& I, string& s);
/*{\Mbinopfunc     same as $s$.read($I$,' '). }*/

friend __exportF ostream& operator<<(ostream& O, const string& s) ;
/*{\Mbinopfunc     writes string $s$ to the output stream $O$. }*/

};



// strings with a line by line input operator

class line_string : public string {

friend istream& operator>>(istream& is, line_string& s) 
{ s.read_line(is); return is; }

};




COMPARE_DECL_PREFIX
inline int  compare(const string& x, const string& y) 
{ return string::cmp(x.cstring(),y.cstring()); }

unsigned long ID_Number(const string& x);

inline const char* leda_tname(const string*) { return "string"; }

inline int Hash(const string& s)
{
    int size = s.length();
    int step = size/10;
    ++step;
    int i=0,j;
    for(j=0;j<size;j+=step)
        i = 131*i + s[j];
    
    return i;
}
        

//------------------------------------------------------------------------------
// Iteration Macros
//------------------------------------------------------------------------------

/*{\Mtext \headerline{Iteration}

{\bf forall\_words}($x,s$)       
$\{$ ``the words of $s$ are successively assigned to $x$'' $\}$

{\bf forall\_lines}($x,s$)       
$\{$ ``the lines of $s$ are successively assigned to $x$'' $\}$

}*/

#define forall_words(x,s) for(int p=0; x=s.next_word(p, -1), p!=-1; )

#define forall_lines(x,s) for(int p=0; x=s.next_word(p,'\n'), p!=-1; )

  


/*{\Mimplementation 
Strings are implemented by \CC{} character vectors. All operations involving
the search for a pattern $x$ in a string $s$ take time $O(s.lenght() * x.length())$, $[\ ]$ takes constant time and all other operations on a 
string $s$ take time $O(s.length())$.}*/



/*
// stream output into strings

template <class T>
string& operator<<(string& str, const T& x)
{ ostringstream os;
  os << x << ends;
  return str += os.str();
 }
*/



#if LEDA_ROOT_INCL_ID == 722053
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
