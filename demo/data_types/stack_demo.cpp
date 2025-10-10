/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  stack_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:19 $


#include<LEDA/core/stack.h>

using namespace leda;

using std::cout;
using std::endl;


   
int main()
{ char c;
  stack<int> S;  stack<char> Op;
  while ( (c = read_char("next symbol = ")) != '.' )
  { switch(c) 
     {  case '(' : break;

        case '1' : { S.push(1); break; }

        case '+' : { Op.push(c); break; }

        case '*' : { Op.push(c); break; }

        case ')' : { int x = S.pop();  int y = S.pop();
                     char op = Op.pop();
                     if ( op == '+' ) S.push(x+y); else S.push(x*y);
                     break;
                   }
     }
  }
  cout << endl;
  cout << "value = " << S.pop() << endl;
  cout << endl;
  return 0;
}

