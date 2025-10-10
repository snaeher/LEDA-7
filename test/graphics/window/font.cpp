/*******************************************************************************
+
+  LEDA 7.2  
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


static string font_name = "T36";
static int font_index = 0;
static int font_sz = 36;
static color font_clr = invisible;

static string font_prefix = "ROMAN";

static int current_char = '@';


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



void redraw(window* wp)
{
  window& W = *wp;

  double pix = W.pix_to_real(1);

  double grid_x = 40*pix;
  double grid_y = 40*pix;

  double grid_h = W.ymax() - W.ymin() - 95*pix;

  W.start_buffering();
  W.clear();

  set_font(W);

//W.set_text_mode(opaque);

  string s = " ";
  if (current_char >=32) s = string(current_char);

  double tw = W.text_width(s);
  double th = W.text_height(s);

  int width = W.real_to_pix(tw);

  int height = W.real_to_pix(th);
  //int height = font_sz;

  double d = grid_h/height;

  double xt = W.xmax() - W.text_width(s) - 20*pix;
  double yt = W.ymax() - 2*pix;

  W.draw_text(xt,yt,s);

  W.set_font("T32");
  W.draw_text(grid_x,W.ymax()-10*pix,
       font_name +string("    %d x %d (%d)",width,height,W.real_to_pix(th)));


  char* prect = W.get_pixrect(xt,yt,xt+(width-1)*pix,yt-(height-1)*pix);

  unsigned int* bgra = (unsigned int*)W.pixrect_to_bgra(prect);

  int pw = W.get_pixrect_width(prect);
  int ph = W.get_pixrect_height(prect);

  if (pw != width) {
    cout << "pw = " << pw << "  width = " << width << endl;
  }

  if (ph != height) {
    cout << "ph = " << ph << "  height = " << height << endl;
  }


  if (font_clr != invisible)
  { 
    for(int i = 0; i < height; i++) 
    { for(int j = 0; j < width; j++) 
      { int p = i*width + j;

        int pix = bgra[p];
      //cout << string("%08x",pix) << endl;

        int r,g,b;
        color(pix).get_rgb(r,g,b);
        int a = 255 - (b + g + r)/3;

        // write alpha
        cout << string("%3d,",a);

        float f = float(a)/255;
        bgra[p] = color(f,0xffffff,font_clr).get_value() | 0xff000000;
        //bgra[p] = color(f,0xffffff,font_clr).get_value();
      }

     cout << endl;

    }

    char* pr1 = W.create_pixrect_from_bgra(width,height,(unsigned char*)bgra);
    W.put_pixrect(xt,yt-200*pix,pr1);
  }

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

  W.flush_buffer();
  W.stop_buffering();
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
  W.int_item("Size",font_sz,1,64, size_handler);
  W.color_item("Color",font_clr, color_handler);

  W.button("write",1);

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

    if (value == 1) 
    { font_clr = 0x000000;

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

      cout << string("int %s_FONT_HEIGHT = %d;",~font_prefix,font_sz) << endl;
      cout << endl;

      cout << string("int %s_FONT_WIDTH[] = {",~font_prefix) << endl;

      for(int i=0; i<128; i++) {
         cout << string("%3d,", width[i]);
         if ((i+1)%16 == 0) cout << endl;
      }
      cout << "};" << endl;
      cout << endl;

      cout << string("int %s_FONT_OFFSET[] = {",~font_prefix) << endl;
      int offset = 0;
      for(int i=0; i<128; i++) {
         cout << string("%6d,",offset);
         offset += width[i] * font_sz;
         if ((i+1)%10 == 0) cout << endl;
      }
      cout << "};" << endl;
      cout << endl;

      cout << string("unsigned char %s_FONT_ALPHA[] = {",~font_prefix) << endl;
      for(int i=0; i<128; i++) {
        current_char = i;
        redraw(&W);
      }

      cout << "};" << endl;

      font_clr = invisible;
      continue;
    }

 
    if (value == MOUSE_BUTTON(3)) break;
    if (++current_char == 128) current_char = 32;
    //if (++current_char == 256) current_char = 32;

  }


  return 0;
}
