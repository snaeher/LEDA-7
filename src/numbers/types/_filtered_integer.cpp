/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _filtered_integer.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/numbers/filtered_integer.h>

LEDA_BEGIN_NAMESPACE

int filtered_integer::exact_value_count = 0;

integer filtered_integer::exact_value() const
{
  if (PTR == 0) return 0;

  if (ptr()->op == 0) return ptr()->num;

  integer i_left  = ptr()->left_child.exact_value();
  integer i_right = ptr()->right_child.exact_value();
  integer result;

  char op = ptr()->op;

  if (op == '+') result = i_left + i_right;
  if (op == '-') result = i_left - i_right;
  if (op == '*') result = i_left * i_right;
  if (op == 'u') result = -i_left;

  return result;
}
  
LEDA_END_NAMESPACE
