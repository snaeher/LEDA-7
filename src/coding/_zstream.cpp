#include <LEDA/coding/zstream.h>
#include <string.h>

#include "zlib/zlib.h"


LEDA_BEGIN_NAMESPACE

static const int bufferSize = 47+256;    // size of data buff
// totals 512 bytes under g++ for izstream at the end.


bool zstreambuf::is_open() { return opened; }


zstreambuf::zstreambuf()
{ buffer = new char[bufferSize];
  file = 0;
  opened = false;
  mode = 0;
  setp( buffer, buffer + (bufferSize-1));
  setg( buffer + 4,     // beginning of putback area
        buffer + 4,     // read position
        buffer + 4);    // end position
  // ASSERT: both input & output capabilities will not be used together
}

zstreambuf::~zstreambuf() { close(); }



zstreambuf* zstreambuf::open( const char* name, int open_mode) {

    if ( is_open())
        return 0;

    mode = open_mode;

    // no append nor read/write mode
    if ((mode & std::ios::ate) || (mode & std::ios::app)
        || ((mode & std::ios::in) && (mode & std::ios::out))) return 0;


    char  mode_str[16];
    char* p = mode_str;

    if (mode & std::ios::in)  *p++ = 'r';  // read
    if (mode & std::ios::out) *p++ = 'w';  // write
    *p++ = 'b';  // binary
/*
    *p++ = '9';  // highest compression level
*/
    *p = '\0';

    file = gzopen(name,mode_str);

    if (file == 0) return 0;

    opened = true;
    return this;
}

zstreambuf * zstreambuf::close() {
    if (is_open()) {
        sync();
        opened = false;
        delete[] buffer;
        if ( gzclose(gzFile(file)) == Z_OK) return this;
    }

    return 0;
}

int zstreambuf::underflow() { // used for input buffer only
    if ( gptr() && ( gptr() < egptr()))
        return * reinterpret_cast<unsigned char *>( gptr());

    if ( ! (mode & std::ios::in) || !opened)
        return EOF;

    // Josuttis' implementation of inbuf
    //int n_putback = gptr() - eback();

    // correction for visual c++ (64bi)
    int n_putback = int(gptr() - eback());

    if ( n_putback > 4)
        n_putback = 4;
    memcpy( buffer + (4 - n_putback), gptr() - n_putback, n_putback);

    int num = gzread(gzFile(file), buffer+4, bufferSize-4);
    if (num <= 0) // ERROR or EOF
        return EOF;

    // reset buffer pointers
    setg( buffer + (4 - n_putback),   // beginning of putback area
          buffer + 4,                 // read position
          buffer + 4 + num);          // end of buffer

    // return next character
    return * reinterpret_cast<unsigned char *>( gptr());    
}

int zstreambuf::flush_buffer() {
    // Separate the writing of the buffer from overflow() and
    // sync() operation.

    //int w = pptr() - pbase();

    // correction for visual c++ (64bi)
    int w = int(pptr() - pbase());

    if ( gzwrite(gzFile(file), pbase(), w) != w)
        return EOF;
    pbump( -w);
    return w;
}

int zstreambuf::overflow( int c) { // used for output buffer only
    if ( ! ( mode & std::ios::out) || !opened)
        return EOF;
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    if ( flush_buffer() == EOF)
        return EOF;
    return c;
}

int zstreambuf::sync() {
    // Changed to use flush_buffer() instead of overflow( EOF)
    // which caused improper behavior with std::endl and flush(),
    // bug reported by Vincent Ricard.
    if ( pptr() && pptr() > pbase()) {
        if ( flush_buffer() == EOF)
            return -1;
    }
    return 0;
}

// --------------------------------------
// class zstreambase:
// --------------------------------------

zstreambase::zstreambase() {
    init( &buf);
}

zstreambase::zstreambase( const char* name, int mode) {
    init( &buf);
    open( name, mode);
}

zstreambase::~zstreambase() {
    buf.close();
}

zstreambuf* zstreambase::rdbuf() { return &buf; }


void zstreambase::open( const char* name, int open_mode) {
    if ( ! buf.open( name, open_mode))
        clear( rdstate() | std::ios::badbit);
}

void zstreambase::close() {
    if ( buf.is_open())
        if ( ! buf.close())
            clear( rdstate() | std::ios::badbit);
}

LEDA_END_NAMESPACE
