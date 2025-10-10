/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  two_windows.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/graphics/window.h>

using namespace leda;

char* prect = 0;

unsigned int* read_bgra(string fname, int& w, int& h)
{ 
  ifstream in(fname);

  in >> w >> h;

  int sz = w*h;

  cout << "w = " << w << endl;
  cout << "h = " << h << endl;
  cout << "sz = " << sz << endl;

  unsigned int* pixels = new unsigned int[sz];

  unsigned int clr = 0;
  for(int i=0; i < sz; i++) {
     in >> clr;
     pixels[i] = clr | 0xff000000;
  }

  return pixels;
}

unsigned int* read_binary_bgra(string fname, int& w, int& h)
{ 
  ifstream in(fname, ios::in | ios::binary);

  if (w == -1) in.read((char*)&w,4);
  if (h == -1) in.read((char*)&h,4);

  int sz = w*h;

  cout << "w = " << w << endl;
  cout << "h = " << h << endl;
  cout << "sz = " << sz << endl;

  unsigned int* pixels = new unsigned int[sz];

  in.read((char*)pixels,4*sz);

  for(int i=0; i < sz; i++) pixels[i] |= 0xff000000;

  return pixels;
}



void redraw(window* wp) {
   window& W = *wp;
   W.clear();
   //W.set_pixrect(prect);
   W.center_pixrect(prect);
}


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

   if (argc < 2) {
     cerr << "usage: bgrea [width] [height] file" << endl;
     return 1;
   }

   string fname;

   int w = -1;
   int h = -1;

   if (argc == 4) {
      w = atoi(argv[1]);
      h = atoi(argv[2]);
      fname = argv[3];
   }
   else
      fname = argv[1];

   cerr << string("size: %d x %d",w,h) << endl;

   unsigned int* pixels = 0;

   if (fname.ends_with(".txt")) pixels= read_bgra(fname,w,h);

   if (fname.ends_with(".bin") || fname.ends_with(".bgra")) 
     pixels= read_binary_bgra(fname,w,h);

   window W(w+100,h+100,fname);

   prect = W.create_pixrect_from_bgra(w,h,(unsigned char*)pixels);

   W.set_redraw(redraw);

   W.display();
   redraw(&W);
   W.read_mouse();

  return 0;
}
