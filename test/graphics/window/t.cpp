
#include <LEDA/graphics/window.h>

using namespace leda;


int main() {

  window W;


  W.display();

  W.set_font("F32");

  string s = "";
  for (int i=0; i<80; i++) s += "X";

  double tw = W.text_width("H");
  double th = W.text_height("H");

  W.resize(100,W.ypos(),W.real_to_pix(200*tw),W.real_to_pix(32*th));

  tw = W.text_width("H");
  th = W.text_height("H");

  double x = W.xmin() + 1;
  double y = W.ymax() - 1;

  double w = W.text_width(s);
  double w1 = s.length()*tw;

cout << "w = " << w << "  w1 = " << w1 << endl;

  W.draw_box(x,y,x+w,y-th,yellow);
  W.draw_box(x,y-10,x+w1,y-th-10,orange);
  W.draw_text(x,y,s);

  W.read_mouse();


  return 0;
}

