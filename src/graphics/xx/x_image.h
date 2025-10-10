
//------------------------------------------------------------------------------
// pixrects / images
//------------------------------------------------------------------------------

struct x_image {

typedef unsigned int pixel;

static pixel blend_pixels(pixel clr1, pixel clr2, bool use_alpha)
{
  if (!use_alpha) {
    return clr2 & 0xffffff;
  }

  int b1 = clr1 & 0xff;
  int g1 = (clr1 >>  8) & 0xff;
  int r1 = (clr1 >> 16) & 0xff;
  int a1 = (clr1 >> 24) & 0xff;

  if (a1 != 0 && a1 != 1) cout << "a1 = " << a1 << endl;

  assert(a1 == 0 || a1 == 1);

  if (a1 == 1) return clr1; // clip 

  int b2 = clr2 & 0xff;
  int g2 = (clr2 >>  8) & 0xff;
  int r2 = (clr2 >> 16) & 0xff;
  int a2 = (clr2 >> 24) & 0xff;

  float f =  float(a2)/255;

  int b = int((1-f)*b1 + f*b2);
  int g = int((1-f)*g1 + f*g2);
  int r = int((1-f)*r1 + f*r2);

//int a = 0xff;
//return (a<<24) + (r<<16) + (g<<8) + b;

  return (r<<16) + (g<<8) + b;
}


 pixel* buf;
 unsigned char* bmp;

 int size;
 int w;
 int h;
 bool use_alpha;
 int clip_x0;
 int clip_y0;
 int clip_x1;
 int clip_y1;
 float phi;
 int anchor_x;
 int anchor_y;

void fill(int clr) {
  if (buf) {
    for(int i=0; i<size; i++) buf[i] = clr;
  }
}

~x_image() {
  if (buf) delete[] buf;
  if (bmp) delete[] bmp;
}

x_image(int width, int height, pixel bg_clr = 0)
{ size = width*height;
  buf = new pixel[size];
  for(int i=0; i<size; i++) buf[i] = bg_clr;
  bmp = 0;
  w = width;
  h = height;
  use_alpha = false;
  clip_x0 = 0;
  clip_y0 = 0;
  clip_x1 = width-1;
  clip_y1 = height-1;
  phi = 0;
  anchor_x = 0;
  anchor_y = 0;
}

x_image(unsigned char* p, int width, int height)
{ // bitmap 1 bit pixels (buf == 0 and bmp != 0)
  buf = 0;
  bmp = p;
  w = width;
  h = height;
  size = height * ((width+7)/8);
  use_alpha = false;
  clip_x0 = 0;
  clip_y0 = 0;
  clip_x1 = width-1;
  clip_y1 = height-1;
  phi = 0;
  anchor_x = 0;
  anchor_y = 0;
}


x_image(pixel* p, int width, int height)
{ // pixmap  4-byte pixels 
  buf = p;
  bmp = 0;
  w = width;
  h = height;
  use_alpha = false;
  clip_x0 = 0;
  clip_y0 = 0;
  clip_x1 = width-1;
  clip_y1 = height-1;
  phi = 0;
  anchor_x = 0;
  anchor_y = 0;
}

void blend_pixel(int x, int y, int clr, int mode=src_mode)
{
  if (x < clip_x0 || x > clip_x1 || y < clip_y0 || y > clip_y1) return;

  int index = x + y*w;

  if (mode == xor_mode)
    buf[index] ^= ~clr;
  else
    buf[index] = blend_pixels(buf[index],clr,true);
}


pixel getpix(int x, int y)
{ if (x < clip_x0 || x > clip_x1 || y < clip_y0 || y > clip_y1) return 0;
  int i = x + y*w;
  return buf[i];
}


void setpix(int x, int y, int clr, int mode=src_mode)
{ 
  if (x < clip_x0 || x > clip_x1 || y < clip_y0 || y > clip_y1) return;

  pixel* q = buf + (y*w + x);

  if ((*q & 0x01000000) != 0 && mode == src_mode) 
  { // clip bit set
    return;
   }

  switch (mode) {

    case src_mode: *q = clr;
                   break;

    case or_mode:  *q |= clr;
                   break;

    case and_mode: *q &= clr;
                   break;

    case xor_mode: *q ^= (~clr & 0xffffff);
                   break;


    case -1:  // used in fill polygon (add clr as a number)
              *q += clr;
              break;

  }
}


void bigpix(int x, int y, int w, int clr, int mode=src_mode)
{ for(int i=0; i<w; i++)
  { int dx = (i%2) ? -(i+1)/2 : +(i+1)/2;
    for(int j=0; j<w; j++)
    { int dy = (j%2) ? -(j+1)/2 : +(j+1)/2;
      setpix(x+dx,y+dy,clr,mode);
     }
   }
}


void bigpix1(int x, int y, int lw, int clr, int mode=src_mode)
{ int w = lw + (1 - lw % 2);
  int x0 = x - w/2; 
  int y0 = y - w/2; 

  for(int i=0; i<w; i++)
  { for(int j=0; j<w; j++)
    { int alpha = 255;
      if (w > lw) {
        if (i == 0 || i == w-1) alpha /= 4;
        if (j == 0 || j == w-1) alpha /= 4;
      }
      clr |= (alpha << 24);
      blend_pixel(x0+i,y0+j,clr,mode);
    }
  }
}



void hline(int x0, int x1, int y, int clr, int mode=src_mode)
{ // 1-pixel horizontal line

  if (x0 > x1) std::swap(x0,x1);

  if (y < clip_y0 || y > clip_y1) return; 
  if (x0 < clip_x0) x0 = clip_x0;
  if (x1 > clip_x1) x1 = clip_x1;

  for(int x = x0; x<=x1; x++) {
    setpix(x,y,clr,mode);
    //blend_pixel(x,y,clr|0x33000000,mode);
  }
}


void vline(int x, int y0, int y1, int clr, int mode=src_mode)
{ // 1-pixel vertical line

  if (y0 > y1) std::swap(y0,y1);

  if (x  < clip_x0 || x  > clip_x1) return;
  if (y0 < clip_y0) y0 = clip_y0;
  if (y1 > clip_y1) y1 = clip_y1;

  for(int y = y0; y<=y1; y++) setpix(x,y,clr,mode);
}


void bitmap_line(int x, int y, unsigned char* bits, int len, int clr, 
                                                             int mode=src_mode)
{ unsigned char* p = bits;
  unsigned char* stop = bits+len;
  while (p < stop)
  { unsigned char c = *p++;
    for(int i=7; i>=0; i--)
    { if (c & (1<<i)) setpix(x,y,clr,mode);
      x++;
     }
   }
}




pixel interpolate_colors(double f, pixel clr1, pixel clr2)
{ unsigned int result = 0;
  for(int i=0; i<4; i++)
  { unsigned int x = ((clr1 >> 8*i)& 0xff);
    unsigned int y = ((clr2 >> 8*i)& 0xff);
    unsigned int z = (unsigned int)((1-f)*x + f*y);
    result += (z << 8*i);
   }

  return result;
}


pixel interpolate_pixel(double x, double y)
{
  if (x < 0 || x >= w || y < 0 || y >= h) return 0x00000000;

  int x1 = int(x);
  int y1 = int(y);

  double xf = x - x1;
  double yf = y - y1;

  int i = y1*w+x1;

  pixel C11 = buf[i];
  pixel C12 = (x1<w-1) ? buf[i+1] : C11;
  pixel C21 = (y1<h-1) ? buf[i+w] : C11;
  pixel C22 = (y1<h-1 && x1<w-1) ? buf[i+w+1] : C11;

  pixel R1 = interpolate_colors(xf,C11,C12);
  pixel R2 = interpolate_colors(xf,C21,C22);

  return interpolate_colors(yf,R1,R2);
}


x_image* resize_rotate(int width, int height, float phi)
{ return resize_rotate(width,height,phi,0,0,width,height); }


x_image* resize_rotate(int width, int height, float phi, int bx0, int by0,
                                                         int bx1, int by1)
{
  x_image* im = new x_image(width,height);

  im->use_alpha = use_alpha;

  pixel* q = im->buf;

  double fx = double(width)/w;
  double fy = double(height)/h;


  if (bx1 > width) bx1 = width;
  if (by1 > height) by1 = height;

  if (bx1 < 0) bx1 = 0;
  if (by1 < 0) by1 = 0;

/*
cout << "width = " << (bx1-bx0) << endl;
cout << "height = " << (by1-by0) << endl;
cout << "phi = " << phi << endl;
*/

  int cx = w/2;
  int cy = h/2;

  for(int x = bx0; x<bx1; x++)
  { for(int y = by0; y<by1; y++)
    { double xf = x/fx;
      double yf = y/fy;
      unsigned int clr = 0;
      if (phi == 0)
        clr = interpolate_pixel(xf,yf);
      else
      { double r = hypot(xf-cx,yf-cy);
        double alpha = atan2(xf-cx,yf-cy) + phi;
        double xx = cx + r*sin(alpha);
        double yy = cy + r*cos(alpha);
        clr = interpolate_pixel(xx,yy);
       }
      q[y*width+x] = clr;
     }
   }

  im->anchor_x = int(0.5 + fx * anchor_x);
  im->anchor_y = int(0.5 + fy * anchor_y);

  return im;
}


void write(ostream& out)
{ out << w << endl;
  out << h << endl;
  for(int i=0; i<w*h; i++) out << buf[i] << endl;
}

void write(string fname)
{ ofstream out(fname);
  write(out);
}

void write_bin(string fname)
{ cout << "write_bin: " << fname << endl;
  unsigned int bytes = 4*w*h;
  FILE* fp = fopen(fname,"wb");
  fwrite((char*)&w,4,1,fp);
  fwrite((char*)&h,4,1,fp);
  fwrite((char*)buf,bytes,1,fp);
  fclose(fp);
}

void write_bin(string fname, int x0, int y0, int x1, int y1)
{ 
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= w) x1 = w-1;
  if (y1 >= h) y1 = h-1;

  int width = x1-x0+1;
  int height = y1-y0+1;

 cout << string("write_bin: %s  %d x %d",~fname,width,height) << endl;

  unsigned int sz = 4*width*height;

  pixel* p = new pixel[sz];
  pixel* q = p;
  for(int j=y0; j <= y1; j++)
    for(int i=x0; i <= x1; i++)
      *q++ = buf[i + j*w];

  FILE* fp = fopen(fname,"wb");
  fwrite((char*)&width,4,1,fp);
  fwrite((char*)&height,4,1,fp);
  fwrite((char*)p,sz,1,fp);
  fclose(fp);
}



};

