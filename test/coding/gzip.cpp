#include <LEDA/coding/zlib.h>
#include <stdio.h>

using std::cerr;
using std::endl;


using namespace leda;

int main(int argc, char*argv[]) 
{
  if ( argc != 3) {
    cerr << "usage: gzip infile outfile" << endl;
    return 1;
  }

  char buf[1024];

  FILE* fp = fopen(argv[1],"rb");

  zlib z;
  z.open_file(argv[2],"wb");
  int sz = 0;
  while ((sz = fread(buf,1,1024,fp)) > 0) z.write(buf,sz);
  z.close_file();

  fclose(fp);
  return 0;
}
