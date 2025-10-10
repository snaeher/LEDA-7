#include <LEDA/core/string.h>

LEDA_BEGIN_NAMESPACE

class __exportC zlib {


  bool z_header;
  int  win_size;
  int  mem_level;
  void* file;

public:

  zlib() {
    z_header = false;
    win_size = 15;
    mem_level = 8;
  }
  
  void set_header(bool b) { z_header = b; }
  void set_window_size(int sz) { win_size = sz; }
  void set_memory_level(int level) { mem_level = level; }

  int inflate(unsigned char* out,int out_sz, unsigned char* data,int data_sz);
  
  int deflate(unsigned char* out, int out_sz, unsigned char* data, int data_sz);
  int deflate(unsigned char* data,int data_sz);


  bool open_file(string fname, const char* mode);
  bool close_file();
  bool eof();
  int  write(char* data, int sz);
  int  read(char* data, int sz);

};

LEDA_END_NAMESPACE
