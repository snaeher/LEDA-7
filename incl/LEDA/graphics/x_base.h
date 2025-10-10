#undef V
#define V virtual

#undef XXX
#define XXX = 0 

class x_base {
public:
#include <LEDA/graphics/x_basic.h>
};

extern x_base* XPTR;

#undef V
#define V

#undef XXX
#define XXX

class x_base_x11 : public x_base {
public:
#include <LEDA/graphics/x_basic.h>
};

