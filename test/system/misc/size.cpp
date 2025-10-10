
#include <iostream>

using namespace std;

int main() {

  cout << endl;
  cout << "sizeof(char)      = " << sizeof(char)  << endl;
  cout << "sizeof(short)     = " << sizeof(short) << endl;
  cout << "sizeof(int)       = " << sizeof(int)   << endl;
  cout << "sizeof(long)      = " << sizeof(long)  << endl;
  cout << "sizeof(long long) = " << sizeof(long long) << endl;

  cout << endl;
  cout << "sizeof(char*)     = " << sizeof(char*) << endl;

  cout << endl;
  cout << "sizeof(float)     = " << sizeof(float)   << endl;
  cout << "sizeof(double)    = " << sizeof(double)   << endl;


  cout << endl;
  cout << "sizeof(size_t)    = " << sizeof(size_t)  << endl;
//cout << "sizeof(ssize_t)   = " << sizeof(ssize_t) << endl;
  
  return 0;
}
