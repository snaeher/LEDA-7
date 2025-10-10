#include <LEDA/system/file.h>
#include <LEDA/coding/zlib.h>

#include <stdio.h>

using std::cout;
using std::endl;

using namespace leda;

int main(int argc, char** argv)
{
  if (argc < 2) {
    cerr << "usage: unzip file" << endl;
    return 1;
  }

  cerr << "file: " << argv[1] << endl;
  cerr << "size: " << size_of_file(argv[1]) << endl;

  int data_sz = size_of_file(argv[1]);
  char* data = new char[data_sz];

  ifstream in(argv[1], ios::binary);
  in.read(data,data_sz);

  int out_sz = 10*data_sz;

  unsigned char out[out_sz];
  int n = zlib().inflate(out,out_sz,(unsigned char*)data,data_sz);

  cerr << string("%d --> %d",data_sz,n) << endl;

  fwrite(out,1,n,stdout);

  return 0;
}

