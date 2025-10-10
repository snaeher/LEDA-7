#include <LEDA/system/file.h>
#include <LEDA/coding/zlib.h>

#include <stdio.h>

using std::cout;
using std::endl;

using namespace leda;

int main(int argc, char** argv)
{
  if (argc < 2) {
    cerr << "usage: zip file" << endl;
    return 1;
  }

  cerr << "file: " << argv[1] << endl;
  cerr << "size: " << size_of_file(argv[1]) << endl;

  int data_sz = size_of_file(argv[1]);
  unsigned char* data = new unsigned char[data_sz];

  ifstream in(argv[1], ios::binary);
  in.read((char*)data,data_sz);

  int out_sz = data_sz;
  if (out_sz < 1024) out_sz = 1024;
  unsigned char* out = new unsigned char[out_sz];

  int n = zlib().deflate(out,out_sz,data,data_sz);

  cerr << string("%d --> %d",data_sz,n) << endl;

  fwrite(out,1,n,stdout);

  return 0;
}

