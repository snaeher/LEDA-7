#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

extern __exportF
unsigned char* jpeg_decode_rgb(int& w, int& h,
                               const unsigned char* in, size_t in_sz);

LEDA_END_NAMESPACE
