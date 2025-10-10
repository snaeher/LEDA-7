/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _filtered_integer1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/filtered_integer1.h>

int filtered_integer1::exact_value_count = 0;

integer filtered_integer1::exact_value() const
{
  filtered_integer1_rep* prep = ptr();

  if (prep == 0) return 0;

  if (prep->op == 0) return prep->num;

  integer i_left  = prep->left_child.exact_value();
  integer i_right = prep->right_child.exact_value();
  integer result;

  char op = prep->op;

  if (op == '+') result = i_left + i_right;
  if (op == '-') result = i_left - i_right;
  if (op == '*') result = i_left * i_right;
  if (op == 'u') result = -i_left;

  return result;
}
  

