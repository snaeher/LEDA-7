/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  word_count.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/d_array.h>
#include <LEDA/core/string.h>

using namespace leda;

using std::cout;
using std::cin;
using std::endl;


int main()
{ d_array<string,int> N(0); 
  string s; 
  while ( true ) 
  { cin >> s;
    if ( s == "end" ) break;
    N[s]++;
  }

  forall_defined(s,N) cout << "\n" << s << " " << N[s];

  return 0;
}

