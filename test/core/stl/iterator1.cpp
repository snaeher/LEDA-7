/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  iterator1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/list.h>

#include <iterator>
#include <algorithm>
#include <list>

#if defined(LEDA_STD_HEADERS)
using namespace std;
#endif

int main()
{
  leda::list<int> L;
  leda::list<int> LL;
  std::copy(std::istream_iterator<int>(std::cin), std::istream_iterator<int>(),
            std::back_inserter(L));
  std::copy( L.begin(), L.end(), std::back_inserter(LL));

  cout << "L  = " << L  << endl;
  cout << "LL = " << LL << endl;

  return 0;
}
