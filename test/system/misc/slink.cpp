#include <LEDA/system/file.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::cerr;
using std::endl;


void create_link1(string src, string dst)
{ 

#if defined(__win32__) || defined(__win64__)
   string cmd = "mklink ";
   if (is_directory(src)) cmd += "/D ";

   src = src.replace_all("/","\\");
   dst = dst.replace_all("/","\\");

   //if (dst == ".") 
   if (is_directory(dst)) {
     int p = src.length()-1;
     while (p >= 0 && src[p] != '\\') p--;
     if (p >= 0) dst = dst + src.tail(-p);
    }

   cmd += " "  + dst + " " + src + " > NUL";
#else
   string cmd = "ln -s "  + src + " " + dst;
#endif

   system(cmd);
}

void link_recursive(string src, string dst)
{
  if (!is_directory(src))
  { create_link(src,dst); 
    return;
   }

  cout << dst << endl;

  list<string> files = get_entries(src);

  create_directory(dst);

  string fn;
  forall(fn,files)
  { if (fn[0] == '.') continue;
    string src_path = src + "/" + fn;
    string dst_path = dst + "/" + fn;
    link_recursive(src_path,dst_path);
   }

}


int main(int argc, char** argv)
{
  bool recursive = false;

  int i = 1;

  if (argc > 1 && string(argv[1]).starts_with("-")) 
  { i = 2;
    if (string(argv[1]) == "-r") recursive = true;
  //if (string(argv[1]) == "-s") symbolic = true;
  }


  if (argc < 3 || i+1 >= argc) {
   cerr << "usage: symlink [-r|-s] path name" << endl;
   return 1;
  }

  string path = string(argv[i]);
  string name = string(argv[i+1]);
  

  if (!is_file(path) && !is_directory(path)) {
   cerr << "symlink: " << path << " does not exist. " << path << endl;
   return 1;
  }


  if (name == ".") {
    int p = path.last_index("/");
    if (p == -1) p = path.last_index("\\");
    name = path.substring(p+1);
  }


  if (is_file(name) || is_directory(name)) {
   cerr << "symlink: '" << name << "' already exists." << endl;
   return 1;
 }

/*
  char buf[256];
  readlink(src,buf,256);
*/

  if (recursive)
    link_recursive(path,name);
  else
    create_link(path,name);

  return 0;
}
