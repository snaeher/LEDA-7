#include <LEDA/system/basic.h>

#include <fstream>

LEDA_BEGIN_NAMESPACE

class zstreambuf : public std::streambuf {

  void* file;
  char* buffer;
  bool  opened;
  int   mode;

  int flush_buffer();

public:

  zstreambuf();
 ~zstreambuf();

  zstreambuf* open(const char* name, int open_mode);
  zstreambuf* close();
  bool        is_open();
    
  virtual int overflow(int c = EOF);
  virtual int underflow();
  virtual int sync();
};


class zstreambase : virtual public std::ios {

protected:

  zstreambuf buf;

public:

  zstreambase();
  zstreambase(const char* name, int open_mode);
 ~zstreambase();

  zstreambuf* rdbuf();

  void open(const char* name, int open_mode);
  void close();
};



class izstream : public zstreambase, public std::istream {

public:

  izstream() : std::istream(&buf) {} 

  izstream(const char* name, int open_mode = std::ios::in)
           : zstreambase(name,open_mode), std::istream(&buf) {}  

  zstreambuf* rdbuf() { return zstreambase::rdbuf(); }

  void open(const char* name, int open_mode = std::ios::in) {
      zstreambase::open(name, open_mode);
  }

};


class ozstream : public zstreambase, public std::ostream {

public:

  ozstream() : std::ostream(&buf) {}

  ozstream(const char* name, int mode = std::ios::out)
           : zstreambase(name,mode), std::ostream(&buf) {}  

  zstreambuf* rdbuf() { return zstreambase::rdbuf(); }

  void open(const char* name, int open_mode = std::ios::out) {
     zstreambase::open(name, open_mode);
  }

};


LEDA_END_NAMESPACE
