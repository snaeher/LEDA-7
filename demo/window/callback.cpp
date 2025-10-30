/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  callback.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using  std::cout;
using  std::endl;
#endif

static int i_slider = 0;
static int i_choice = 0;
static int i_multi = 0;

void f_slider(int i_new)
{ cout << "\nslider: old = " << i_slider << ", new = " << i_new; }

void f_choice(int i_new)
{ cout << "\nchoice: old = " << i_choice << ", new = " << i_new; }

void f_multi(int i_new)
{ cout << "\nmulti:  old = " << i_multi  << ", new = " << i_new; }


int main()
{
  list<string> L; 
  for(int i=0; i<8; i++) L.append(string("%d",i));

  window W(300,300);

  W.int_item("slider", i_slider, 0, 100, f_slider);
  W.int_item("choice", i_choice, 0, 7, 1, f_choice);
  W.choice_mult_item("multi", i_multi, L, f_multi);

  W.display();
  W.read_mouse();

  W.screenshot("callback");
  return 0;
}

