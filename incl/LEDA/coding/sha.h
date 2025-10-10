#include <LEDA/system/basic.h>
#include <cstdint>

LEDA_BEGIN_NAMESPACE

extern __exportF void   sha256(uint8_t* buffer, uint8_t* data, size_t data_sz);
extern __exportF string sha256(string txt);

extern __exportF void   sha1(uint8_t* buffer, uint8_t* data, size_t data_sz);
extern __exportF string sha1(string txt);

LEDA_END_NAMESPACE
