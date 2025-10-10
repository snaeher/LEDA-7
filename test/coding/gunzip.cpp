#include <LEDA/coding/zlib.h>
#include <stdio.h>

using std::cerr;
using std::endl;

using namespace leda;

int main( int argc, char*argv[]) 
{
  if ( argc != 3) {
    cerr << "usage: gunzip infile out-file" << endl;
    return 1;
  }

  char buf[1024];

  FILE* fp = fopen(argv[2],"wb");

  zlib z;
  z.open_file(argv[1],"rb");

  int sz = 0;
  while ((sz = z.read(buf,1024)) > 0) fwrite(buf,1,sz,fp); 

  z.close_file();
  fclose(fp);

  return 0;
}
