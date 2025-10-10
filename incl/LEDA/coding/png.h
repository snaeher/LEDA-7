#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

extern __exportF
bool png_encode_rgb(unsigned char*& out, size_t& out_sz,
                    const unsigned char* image, unsigned w, unsigned h);

extern __exportF
bool png_encode_rgba(unsigned char*& out, size_t& out_sz,
                     const unsigned char* image, unsigned w, unsigned h);

extern __exportF
bool png_decode_rgb(unsigned char*& out, int& w, int& h,
                    const unsigned char* in, size_t in_sz);

extern __exportF
bool png_decode_rgba(unsigned char*& out, int& w, int& h,
                     const unsigned char* in, size_t in_sz);



extern __exportF
unsigned char* png_encode_rgb(size_t& sz, unsigned char* image, int w, int h);

extern __exportF
unsigned char* png_encode_rgba(size_t& sz, unsigned char* image, int w, int h);

extern __exportF
unsigned char* png_decode_rgb(int& w, int& h, unsigned char* in, 
                                              size_t in_sz);

extern __exportF
unsigned char* png_decode_rgba(int& w, int& h, unsigned char* in, 
                                               size_t in_sz);

// decode file

extern __exportF
unsigned char* png_decode_rgba(int& w, int& h, const char* png_file);


extern __exportF
unsigned char* png_decode_rgba(int& w, int& h, const char* png_file, 
                                               unsigned int bg_col,
                                               int alpha);


LEDA_END_NAMESPACE

