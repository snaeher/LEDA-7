#include <LEDA/coding/png.h>

#include "img/lodepng.h"


LEDA_BEGIN_NAMESPACE

bool png_encode_rgb(unsigned char*& out, size_t& out_sz,
                    const unsigned char* image, unsigned w, unsigned h)
{ 
  unsigned err = lodepng_encode24(&out,&out_sz,image,w,h);
  return err == 0;
}

bool png_encode_rgba(unsigned char*& out, size_t& out_sz,
                     const unsigned char* image, unsigned w, unsigned h)
{ 
  unsigned err = lodepng_encode32(&out,&out_sz,image,w,h);
  return err == 0;
}

bool png_decode_rgb(unsigned char*& out, int& w, int& h,
                    const unsigned char* in, size_t in_sz)
{ 
  unsigned err = lodepng_decode24(&out,(unsigned*)&w,(unsigned*)&h,in,in_sz);
  return err == 0;
}

bool png_decode_rgba(unsigned char*& out, int& w, int& h,
                     const unsigned char* in, size_t in_sz)
{ 
  unsigned err = lodepng_decode32(&out,(unsigned*)&w,(unsigned*)&h,in,in_sz);
  return err == 0;
}




unsigned char* png_encode_rgb(size_t& sz, unsigned char* image, int w, int h)
{ unsigned char* rgb;
  if (lodepng_encode24(&rgb,&sz,image,w,h) == 0)
    return rgb;
  else
    return 0;
}

unsigned char* png_encode_rgba(size_t& sz, unsigned char* image, int w, int h)
{ unsigned char* rgba;
  if (lodepng_encode32(&rgba,&sz,image,w,h) == 0)
    return rgba;
  else
    return 0;
}



unsigned char* png_decode_rgb(int& w, int& h, unsigned char* in, size_t in_sz)
{ unsigned char* out = 0;
  unsigned int width;
  unsigned int height;
  unsigned err = lodepng_decode24(&out,&width,&height,in,(unsigned)in_sz);

  if (err == 0)
  { w = width;
    h = height;
    return out;
   }

  cout << "LODEPNG ERROR" << endl;
  cout << "size = " << in_sz << endl;
  cout << "error = " << lodepng_error_text(err) << endl;
  cout << endl;

  return 0;
}



unsigned char* png_decode_rgba(int& w, int& h, unsigned char* in, size_t in_sz)
{ unsigned char* out = 0;
  unsigned int width;
  unsigned int height;
  unsigned err = lodepng_decode32(&out,&width,&height,in,(unsigned)in_sz);

/*
  unsigned err = lodepng_decode_memory(&out, &width, &height, in, in_sz,
                                                              LCT_RGBA, 32);
*/

  if (err == 0)
  { w = width;
    h = height;
    return out;
   }

  cout << "LODEPNG ERROR" << endl;
  cout << "size = " << in_sz << endl;
  cout << "error = " << lodepng_error_text(err) << endl;
  cout << endl;
  return 0;
}


unsigned char* png_decode_rgba(int& w, int& h, const char* png_file)
{ unsigned char* out = 0;
  unsigned int width;
  unsigned int height;
  unsigned err = lodepng_decode32_file(&out,&width,&height,png_file);

  if (err == 0)
  { w = width;
    h = height;
    return out;
   }

  cout << "LODEPNG ERROR" << endl;
  cout << "file:  " << png_file << endl;
  cout << "error: " << lodepng_error_text(err) << endl;
  cout << endl;
  return 0;
}

static void alpha_blend(unsigned char* buf, int w, int h, int bg_col, int alpha)
{
  unsigned char bg_r = (bg_col >>  0) & 0xff;
  unsigned char bg_g = (bg_col >>  8) & 0xff;
  unsigned char bg_b = (bg_col >> 16) & 0xff;
  unsigned char bg_a = (bg_col >> 24) & 0xff;

  int sz = w*h;

  for(int i=0; i<sz; i++) {
    unsigned char r = buf[4*i+0];
    unsigned char g = buf[4*i+1];
    unsigned char b = buf[4*i+2];
    unsigned char a = buf[4*i+3];

    float f = (alpha > 0) ? alpha/255.0f : a/255.0f;

    buf[4*i+0] = (unsigned char)((1-f)*bg_r + f*r);
    buf[4*i+1] = (unsigned char)((1-f)*bg_g + f*g);
    buf[4*i+2] = (unsigned char)((1-f)*bg_b + f*b);
    buf[4*i+3] = 255;
  }

}


unsigned char* png_decode_rgba(int& w, int& h, const char* png_file, 
                                               unsigned int bg_col,
                                               int alpha)
{
  unsigned char* buf = png_decode_rgba(w,h,png_file);
  if (buf)
  { alpha_blend(buf,w,h,bg_col,-1);
    if (alpha > -1) alpha_blend(buf,w,h,bg_col,alpha);
   }
  return buf;
}


LEDA_END_NAMESPACE

