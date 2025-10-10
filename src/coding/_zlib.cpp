#include <LEDA/coding/zlib.h>

#include <string.h>
#include <assert.h>

#include "zlib/zlib.h"

using std::cout;
using std::endl;

LEDA_BEGIN_NAMESPACE


bool zlib::open_file(string fname, const char* mode)
{ file = gzopen(fname,mode); 
  return (file != 0);
}
  
bool zlib::close_file() {
  return gzclose(gzFile(file)) == Z_OK;
}

int zlib::write(char* data, int sz) {
 return  gzwrite(gzFile(file),data,sz);
}

int zlib::read(char* data, int sz) {
 return  gzread(gzFile(file),data,sz);
}

bool zlib::eof() {
 return  gzeof(gzFile(file)) != 0;
}

int zlib::deflate(unsigned char* out,int out_sz, 
                  unsigned char* data,int data_sz)
{
  z_stream strm;

  strm.zalloc = Z_NULL;
  strm.zfree  = Z_NULL;
  strm.opaque = Z_NULL;

  strm.next_in = data;
  strm.avail_in = data_sz;
  strm.next_out = out;
  strm.avail_out = out_sz;

  int win_bits = win_size; 

  if (z_header) 
    win_bits = win_size + 16;
  else
    win_bits = -win_size;

  int status = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, 
                                   Z_DEFLATED,
                                   win_bits,
                                   mem_level,
                                   Z_DEFAULT_STRATEGY);

  if (status != Z_OK) return -1;

  status = ::deflate(&strm,Z_FINISH);
  deflateEnd(&strm);

  if (status != Z_STREAM_END) return -1;

  return out_sz-strm.avail_out;
}


int zlib::deflate(unsigned char* data,int data_sz)
{ 
  int buf_sz = data_sz;
  if (buf_sz < 1024) buf_sz = 1024;
  unsigned char* buf = new unsigned char[buf_sz];

  int sz = zlib::deflate(buf,buf_sz,data,data_sz);
  memcpy(data,buf,sz);
  delete[] buf;

  return sz;
}


int zlib::inflate(unsigned char* out,int out_sz, 
                  unsigned char* data,int data_sz)
{
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree  = Z_NULL;
  strm.opaque = Z_NULL;

  strm.next_in = data;
  strm.avail_in = data_sz;
  strm.next_out = out;
  strm.avail_out = out_sz;

  int win_bits = win_size; 
  if (!z_header) win_bits += 16;

  if (inflateInit2(&strm, win_bits) != Z_OK) return -1;

  ::inflate(&strm,Z_FINISH);

  inflateEnd(&strm);

  return out_sz-strm.avail_out;
}


LEDA_END_NAMESPACE
