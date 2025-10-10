/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  color.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_COLOR_H
#define LEDA_COLOR_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721289
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/internal/system.h>
#include <LEDA/system/basic.h>
#include <LEDA/graphics/x_window.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage {color} {} {Colors}}*/

class __exportC color {

/*{\Mdefinition
The data type $color$ is the type of all colors available for
drawing operations in windows (cf. \ref{Windows}).
Each color is defined by a triple of integers $(r,g,b)$ with
$0\le r,g,b \le 255$, the so-called {\em rgb-value} of the color.
The number of available colors is restricted and depends on the 
underlying hardware. Colors can be created from rgb-values,
from names in a color data base (X11), or from the 16 integer 
constants (enumeration in \<LEDA/graphics/x\_window.h\>)
$black$, $white$, $red$, $green$, $blue$, $yellow$, $violet$, $orange$;
$cyan$, $brown$, $pink$, $green2$, $blue2$, $grey1$, $grey2$, $grey3$. 
}*/

  int  color_value;

void construct(int r, int g, int b);
void construct(int a, int r, int g, int b);

public:

/*
  const int black  = 0x000000;
  const int white  = 0xffffff;
  const int red    = 0xff0000;
  const int green  = 0x00ee00;
  const int blue   = 0x0000cd;
  const int yellow = 0xffff00;
  const int violet = 0xa020f0;
  const int orange = 0xff8c00;
  const int cyan   = 0x00ffff;
  const int brown  = 0xa0522d;
  const int pink   = 0xff00ff;
  const int green2 = 0x0cb3a0;
  const int blue2  = 0x6495ed;
  const int grey1  = 0xd9d9d9;
  const int grey2  = 0xb3b3b3;
  const int grey3  = 0x737373;
  const int ivory  = 0xffffe4;
*/


static int default_colors[];

public:

static color get(int i) { 
  // i = -1 .. 16
  return default_colors[i+1]; 
}

static int index(int clr) {
  int i = -1;
  for (i=-1; i<=16; i++) {
     if (color::get(i) == clr) break;
  }
  return i;
}



/*{\Mcreation col}*/

  color();
/*{\Mcreate  creates a color with rgb-value $(0,0,0)$ (i.e. black). }*/

  color(int r, int g, int b);
/*{\Mcreate  creates a color with rgb-value $(r,g,b)$. }*/

  color(const char* name);
/*{\Mcreate  creates a color and initializes it with the rgb-string
             $name$. }*/

  color(int val);
/*{\Mcreate  creates a color and initializes it with a color integer value.
             In particular one of the 16 predefined color values constants 
             can be used: $black$, $white$, $red$, $green$, $blue$, 
             $yellow$, $violet$, $orange$, $cyan$, $brown$, $pink$,                          $green2$, $blue2$, $grey1$, $grey2$, or $grey3$.  }*/


  color(float a, int val1, int val2);


/*{\Moperations 1.5 6}*/

  void set_rgb(int r, int g, int b);
/*{\Mop    sets the red, blue, and green components of |\Mvar| 
           to $r$, $g$, $b$. }*/

  void get_rgb(int& r, int& g, int& b) const;
/*{\Mop    assigns the red, green, and blue components of |\Mvar| 
           to $r$, $g$, $b$. }*/

  int  get_brightness() const;
/*{\Mop    returns the brightness of |\Mvar| an integer 
           between $0$ and $255$. }*/

  void set_red(int x);
/*{\Mop    sets the red component of |\Mvar| to $x$. }*/

  void set_green(int x);
/*{\Mop    sets the green component of |\Mvar| to $x$. }*/

  void set_blue(int x);
/*{\Mop    sets the blue component of |\Mvar| to $x$. }*/


  string get_string() const { return string("#%06x",color_value); } 
/*{\Mop    returns a string representation of |\Mvar|. }*/


  color text_color() const ;
/*{\Mop    returns a suitable color ($black$ or $white$) for writing 
           text on a background of color |\Mvar|. }*/



  void set_value(int x)  { color_value = x; }
  int  get_value() const { return color_value; }

  operator int() const  { return color_value; }

  //explicit operator string() const { return get_string(); }

};

extern __exportF istream& operator>>(istream& in, color& c);
extern __exportF ostream& operator<<(ostream& out, const color& c);


#if LEDA_ROOT_INCL_ID == 721289
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
