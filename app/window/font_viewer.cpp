/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  font.c
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
using std::cout;
using std::cin;
using std::endl;
#endif

ofstream* out_ptr = 0;


static string font_name = "";
static int font_index = 0;
static int font_sz = 64;
static color font_clr = invisible;

static string font_prefix = "ROMAN";

static int current_char = '@';

static bool write_font = false;


void set_font(window& W)
{
  switch(font_index) {

    case 0: font_name = string("T%02d",font_sz);
            font_prefix = "ROMAN";
            break;

    case 1: font_name = string("F%02d",font_sz);
            font_prefix = "FIXED";
            break;

    case 2: font_name = string("B%02d",font_sz);
            font_prefix = "BOLD";
            break;

    case 3: font_name = string("I%02d",font_sz);
            font_prefix = "ITALIC";
            break;
  }

  W.set_font(font_name);
}


void draw_char(window& W, int c)
{
  double pix = W.pix_to_real(1);

  double grid_x = 40*pix;
  double grid_y = 40*pix;
  double grid_h = W.ymax() - W.ymin() - 95*pix;

  string s = " ";
  if (current_char >=32) s = string(current_char);

  set_font(W);

  double tw = W.text_width(s);
  double th = W.text_height(s);

  int width = W.real_to_pix(tw);
  int height = W.real_to_pix(th);
  //int height = font_sz;


  // draw char and get pixels (bgra) from screen

  double xt = W.xmax() - W.text_width(s) - 20*pix;
  double yt = W.ymax() - 10*pix;

  W.draw_text(xt,yt,s);

  char* prect = W.get_pixrect(xt,yt,xt+(width-1)*pix,yt-(height-1)*pix);
  unsigned int* bgra = (unsigned int*)W.pixrect_to_bgra(prect);

  int char_w = W.get_pixrect_width(prect);
  assert(char_w == width);

  int char_h = W.get_pixrect_height(prect);
  assert(char_h == height);


  if (out_ptr)
  { // write alpha values to file
    ofstream& out = *out_ptr;
    for(int i = 0; i < height; i++) 
    { for(int j = 0; j < width; j++) 
      { int pix = bgra[i*width + j];
        int alpha = 255 - color(pix).get_brightness();
        out << string("%3d,",alpha);
       }
      out << endl;
    }
  }


  if (font_clr != invisible)
  { // draw char with color = font_clr
    for(int i = 0; i < height; i++) 
    { for(int j = 0; j < width; j++) 
      { int p = i*width + j;
        int pix = bgra[p];
        int r,g,b;
        color(pix).get_rgb(r,g,b);
        int a = 255 - (b + g + r)/3;
        float f = float(a)/255;
        bgra[p] = color(f,0xffffff,font_clr).get_value() | 0xff000000;
      }
    }

    char* pr = W.create_pixrect_from_bgra(width,height,(unsigned char*)bgra);


    W.put_pixrect(xt,yt-2*height*pix,pr);
  }


  // draw matrix

  W.set_font("T32");
  W.draw_text(grid_x,yt, font_name + 
                         string("    %d x %d   char %d", width,height,s[0]));

  double d = grid_h/height; // width and heigh of a single matrix cell 

  for(int i=0; i < height; i++) // rows
  { for (int j=0; j < width; j++) // cols
    { int pix = bgra[i*width+j];
      double x = grid_x+j*d;
      double y = grid_y+(height-i)*d;
      W.draw_box(x,y,x+d,y-d,pix);
      W.set_line_width(1);
      W.draw_rectangle(x,y,x+d,y-d,grey2);
     }
   }

}



void redraw(window* wp) 
{ window& W = *wp;
  W.start_buffering();
  W.clear();
  draw_char(*wp,current_char);
  W.stop_buffering();
  W.flush_buffer();
}



void family_handler(int i) {
  window* wp = window::get_call_window();
  font_index = i;
  redraw(wp);
}

void size_handler(int i) {
  window* wp = window::get_call_window();
  font_sz = i;
  redraw(wp);
}

void color_handler(int x) {
  font_clr = color(x);
  redraw(window::get_call_window());
}




int main()
{
  list<string> font_list;
  font_list.append("roman");
  font_list.append("fixed");
  font_list.append("bold");
  font_list.append("italic");

  window W("Font Viewer");

  W.set_clear_on_resize(false);
  W.set_redraw(redraw);

  W.choice_item("Font",font_index,font_list, family_handler);
  //W.int_item("Size",font_sz,1,64, size_handler);
  W.int_item("Size",font_sz,1,128, size_handler);
  W.color_item("Color",font_clr, color_handler);

  W.button(" < ", 1);
  W.button(" > ", 2);
  W.button("scan",3);

  W.display();

  current_char = '@';

  for(;;) 
  {
    redraw(&W);

    int value;
    double x,y;
    int event = no_event;

    while (event != button_press_event && event != key_press_event) {
         event = W.read_event(value,x,y);
    }

    if (event == key_press_event) {
      current_char = value;
      continue;
    }

    if (value == 3) 
    { // scan: write font table to <font_name>.table

      ofstream out(font_name + ".table");

      out_ptr = &out;

      set_font(W);

      int width[128];

      for(int i=0; i<128; i++) {
         double tw = 0;
         if (i < 32)
           tw = W.text_width(" ");
         else
           tw = W.text_width(string(char(i)));

         width[i] = W.real_to_pix(tw);
      }

      out << string("int %s_FONT_HEIGHT = %d;",~font_prefix,font_sz) << endl;
      out << endl;

      out << string("int %s_FONT_WIDTH[] = {",~font_prefix) << endl;

      for(int i=0; i<128; i++) {
         out << string("%3d,", width[i]);
         if ((i+1)%16 == 0) out << endl;
      }
      out << "};" << endl;
      out << endl;

      out << string("int %s_FONT_OFFSET[] = {",~font_prefix) << endl;
      int offset = 0;
      for(int i=0; i<128; i++) {
         out << string("%6d,",offset);
         offset += width[i] * font_sz;
         if ((i+1)%10 == 0) out << endl;
      }
      out << "};" << endl;
      out << endl;

      out << string("unsigned char %s_FONT_ALPHA[] = {",~font_prefix) << endl;

      for(int i=0; i<128; i++) {
        current_char = i;
        redraw(&W);
      }

      out << "};" << endl;

      out.close();
      out_ptr = 0;

      font_clr = invisible;
      continue;
    }

    if (value == 0) break;
 
    if (value == MOUSE_BUTTON(3)) break;

    if (value == 1) {
      if (--current_char == 31) current_char = 127;
    }

    if (value == 2) {
      if (++current_char == 128) current_char = 32;
    }

  }


  return 0;
}
