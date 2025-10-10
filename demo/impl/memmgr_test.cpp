/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  memmgr_test.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:16 $


#include <LEDA/system/basic.h>
#include <LEDA/core/list.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;
using std::ostream;
using std::istream;


class pair {

double x;
double y;

public:

pair(double a=0, double b=0) : x(a), y(b) { }
pair(const pair& p) : x(p.x), y(p.y) { }

friend ostream&  operator<<(ostream& ostr, const pair&) 
                 { return  ostr; }

friend istream&  operator>>(istream& istr, pair&) 
                 { return istr; }

LEDA_MEMORY(pair)  // removed in dumb_pair

};


class dumb_pair {

double x;
double y;

public:

dumb_pair(double a=0, double b=0) : x(a), y(b) { }
dumb_pair(const dumb_pair& p) : x(p.x), y(p.y) { }

friend ostream&  operator<<(ostream& ostr, const dumb_pair&) 
                 { return  ostr; }

friend istream&  operator>>(istream& istr, dumb_pair&) 
                 { return istr; }

};


int main()
{ 
  IO_interface I;
  int n = I.read_int("Number of pairs = ",1000000);
  I.write_table(n);

  
 
  float T = used_time(); float UT;

  
    list<pair> L;
    
    int i;
    for (i = 0; i < n; i++ ) L.append(pair());
    print_statistics();
    L.clear();
    print_statistics();
    UT = used_time(T);



  I.write_table(" & ",UT);
  I.write_demo("time with LEDA memory management = ",UT);

 

  used_time(T);

  
  list<dumb_pair> DL;
  for (i = 0; i < n; i++ ) DL.append(dumb_pair());
  print_statistics();
  DL.clear();
  print_statistics();
  UT = used_time(T);





  I.write_table(" & ",UT);
  I.write_demo("time without LEDA memory management = ",UT);
  
  DL.clear();
  
  I.write_table("  \\\\ \\hline \\hline");
  return 0;
}
