#include <LEDA/system/basic.h>

#include "img/minijpeg.h"

#include <string.h>
#include <assert.h>

LEDA_BEGIN_NAMESPACE

unsigned char* jpeg_decode_rgb(int& w, int& h,
                                const unsigned char* in, size_t in_sz)
{
  Jpeg::Decoder decoder(in,in_sz);

  if (decoder.GetResult() != Jpeg::Decoder::OK)
  { //error_handler(1,"JPG Decoder Error");
    return 0;
  }

  if (!decoder.IsColor())
  { //error_handler(1,"JPG Format Error");
    return 0;
   }

  w = decoder.GetWidth();
  h = decoder.GetHeight();

  size_t sz = 3*w*h;
  assert(decoder.GetImageSize() == sz);

/*
  unsigned char* p =  decoder.GetImage();
  unsigned char* rgb = new unsigned char[sz];
  memcpy(rgb,p,sz);
  return rgb;
*/

  return decoder.GetImage();
}


LEDA_END_NAMESPACE
