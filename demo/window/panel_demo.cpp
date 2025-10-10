/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  panel_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/system/file.h>
#include <LEDA/graphics/bitmaps/button32.h>

using namespace leda;



static unsigned char* bm_bits [] = {
  triang_bits,
  voro_bits,
  empty_circle_bits,
  encl_circle_bits,
  grid_bits,
  hull_bits,
};


int main()
{ 
  bool   B     = false;
  color  col   = blue2;
  double R     = 3.1415926535;
  int    c     = 0;
  int    c1    = 0;
  int    c2    = 0;
  int    cm    = 5;
  int    N     = 100;
  string s0    = "string0";
  string s     = "Hello World";
  string s1    = "menu";

  int but_num = 4;
  int slider_val = 0;

  list<string> M1 = get_files(".");
  list<string> M2 = get_files(".");


  list<string> CML;
  CML.append("0");
  CML.append("1");
  CML.append("2");
  CML.append("3");
  CML.append("4");


  for(;;)
  {
    panel P("PANEL DEMO");

    //P.load_button_font("lucidasans-12");

    string text;
    text += " The panel section of a window is used for displaying text";
    text += " and for updating the values of variables. It consists";
    text += " of a list of panel items and a list of buttons.";
    text += " All operations adding panel items or buttons to the panel";
    text += " section of a window have to be called before";
    text += " the window is displayed for the first time.";
  
    P.text_item("\\bf\\blue Text Item");
    P.text_item("\\n");
    P.text_item(text);

    P.text_item("");
    P.text_item("\\bf\\blue Bool Items");
    P.bool_item("bool item 1",B);
    P.bool_item("bool item 2",B);

    P.text_item("");
    P.text_item("\\bf\\blue Color Item");
    P.color_item("color item",col);

    P.text_item("");
    P.text_item("\\bf\\blue Slider Item");
    P.int_item("slider item",slider_val,-100,100);

    P.text_item("");
    P.text_item("\\bf\\blue Simple Items");
    P.string_item("string item",s);
    P.int_item("int item",N);
    P.double_item("double item",R);

    P.text_item("");
    P.text_item("\\bf\\blue String Menu Items");
    P.string_item("string menu",s0,M1,8);

    P.text_item("");
    P.text_item("\\bf\\blue Choice Items");
    P.choice_item("simple choice",c,"one","two","three","four","five");
    P.choice_mult_item("multiple choice",cm,CML);
    P.int_item("integer choice",c1,0,80,20);
    P.choice_item("bitmap choice",c2,6,32,32,bm_bits,
    "#choice1#choice2#choice3#choice4#choice4#choice6#");

    P.text_item("");
    P.text_item("\\bf\\blue Buttons");
  
    int i;
    for(i=1; i <= but_num; i++) P.button(string("button %d",i));
  
    P.display();

    if (P.open() == 0) break;
  }
 
  return 0;
}
  
