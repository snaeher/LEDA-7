#include <LEDA/system/file.h>
#include <LEDA/system/https.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
#endif


int main(int argc, char** argv)
{
  if (argc < 2)
  { cerr << "usage: wget url <file>" << endl;
    return 1;
   }


/*
  list<string> header;
  if (wget(argv[1],header,cout,10) == 0)
  { string status = header.front();
    cerr << status << endl;
    return 1;
   }
*/

  string fname = "";
  if (argc > 2) fname = argv[2];

  https H;
  H.set_timeout(10);

  if (!H.connect(argv[1]))
  { cerr << H.get_error() << endl;
    return 1;
  }

  // print header
  list<string> header = H.get_headers();
  string s;
  forall(s,header) cerr << s << endl;
  cerr << endl;

  if (fname != "") 
    H.get_file(fname);
  else
  { H.get(cout);
/*
    string line;
    while (H.get_line(line)) cout << line.utf_to_iso8859() << endl;
*/
   }

  return 0;
}

