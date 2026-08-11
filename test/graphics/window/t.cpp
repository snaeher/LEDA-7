#include <LEDA/graphics/window.h>

using namespace leda;

int show() 
{ window W;
  W.display();
  W.draw_ctext("Hello World");
  return W.read_mouse();
}


int main() 
{ while (show() != MOUSE_BUTTON(3)) {}
  return 0;
}
