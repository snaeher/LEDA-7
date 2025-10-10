/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _color.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/system/basic.h>
#include <LEDA/graphics/color.h>
/*
#include <LEDA/graphics/x_basic.h>
*/
#include <stdio.h>
#include <string.h>

LEDA_BEGIN_NAMESPACE


int color::default_colors[] = { invisible,
                                black,
                                white,
                                red,
                                green,
                                blue,
                                yellow,
                                violet,
                                orange,
                                cyan,
                                brown,
                                pink,
                                green2,
                                blue2,
                                grey1,
                                grey2,
                                grey3,
                                ivory };


void color::construct(int r, int g, int b)
{ if (r == -1 && g == -1 && b == -1) 
    color_value = invisible;
  else
    color_value =  (r<<16) + (g<<8) + b;
}

void color::construct(int a, int r, int g, int b) {
    color_value =  (a<<24) + (r<<16) + (g<<8) + b;
}



color::color() { color_value = 0; }

color::color(const char* name) 
{ 
  string clr_names[] = { "invisible",
                         "black",
                         "white",
                         "red",
                         "green",
                         "blue",
                         "yellow",
                         "violet",
                         "orange",
                         "cyan",
                         "brown",
                         "pink",
                         "green2",
                         "blue2",
                         "grey1",
                         "grey2",
                         "grey3",
                         "ivory" };
  int skip = 0;
  if (name[0] == '#') skip = 1;
  if (name[0] == '0' && name[1] == 'x') skip = 2;

  if (skip == 0)
  { int i = 17;
    while (i >= 0 && clr_names[i] != name) i--;
    if (i > 0)
      color_value = default_colors[i];
    else
      color_value = black;
    return;
  }
    

  char rgb_str[16];
  strcpy(rgb_str,name+skip);

  if (strlen(rgb_str) == 12)
  { rgb_str[2] = rgb_str[4];
    rgb_str[3] = rgb_str[5];
    rgb_str[4] = rgb_str[8];
    rgb_str[5] = rgb_str[9];
    rgb_str[6] = 0;
  }
    
  sscanf(rgb_str,"%x",&color_value);
}


color::color(int clr ) { color_value = clr;  }

color::color(float alpha,int clr1, int clr2) 
{
  int r1,g1,b1;
  color(clr1).get_rgb(r1,g1,b1);
  
  int r2,g2,b2;
  color(clr2).get_rgb(r2,g2,b2);
  
  int r = int((1-alpha)*r1 + alpha*r2);
  int g = int((1-alpha)*g1 + alpha*g2);
  int b = int((1-alpha)*b1 + alpha*b2);
  
  construct(r,g,b);
}


color::color(int r, int g, int b) { construct(r,g,b); }


color color::text_color() const
{ int sum = ((color_value >>  0) & 0xff) + 
            ((color_value >>  8) & 0xff) + 
            ((color_value >> 16) & 0xff);

  //return (sum < 385) ? white : black;

  return (sum < 450) ? white : black;

  //return (sum < 575) ? white : black;
}

void color::get_rgb(int& r,int& g,int& b) const
{ 
  if (color_value == invisible)
  { r = -1;
    g = -1;
    b = -1;
    return;
   }

  b = color_value & 0xff;
  g = (color_value >> 8) & 0xff;
  r = (color_value >> 16) & 0xff;
 }


int color::get_brightness() const
{ if (color_value == invisible) return 0;
  int b = color_value & 0xff;
  int g = (color_value >> 8) & 0xff;
  int r = (color_value >> 16) & 0xff;
  return (b+g+r)/3;
}



void color::set_rgb(int r, int g, int b) { construct(r,g,b); }

void color::set_red(int x)
{ int r,g,b;
  get_rgb(r,g,b); 
  construct(x,g,b);
}

void color::set_green(int x)
{ int r,g,b;
  get_rgb(r,g,b); 
  construct(r,x,b);
}

void color::set_blue(int x)
{ int r,g,b;
  get_rgb(r,g,b); 
  construct(r,g,x);
}


ostream& operator<<(ostream& out, const color& c) { 
  //0xaarrggbb
  return out << string("0x%08x",c.get_value());
}

istream& operator>>(istream& in, color& c)
{ string s; in >> s;
  if (s.starts_with("0x"))
  { int x;
    sscanf(~s,"0x%x",&x);
    c = color(x);
   }
  else
  { int r = atoi(s);
    int g,b;
    in >> g >> b;
    c = color(r,g,b);
  }
  
  return in;
}

LEDA_END_NAMESPACE
