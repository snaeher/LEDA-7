#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

extern __exportF string base64(unsigned char* bytes, size_t len);

inline string base64(string txt) {
  size_t  len = txt.length();
  unsigned char* bytes = (unsigned char*)txt.cstring();
  return base64(bytes,len);
}

LEDA_END_NAMESPACE
