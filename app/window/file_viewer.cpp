/******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  fileview.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/system/file.h>

using namespace leda;

using std::ofstream;
using std::endl;


inline void remove_quotes(string& s)
{
	if (s.length() >= 2 && (s[0] == '\'' || s[0] == '"') 
		&& (s[0] == s[s.length()-1]))
	{ s = s(1,s.length()-2); }
}

#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  string fname;
  string dname;
  string filter;

  if (argc > 1) fname = argv[1];
  if (argc > 2) dname = argv[2];
  if (argc > 3) filter= argv[3];

  if (dname == "") dname = ".";

  remove_quotes(fname);
  remove_quotes(dname);
  remove_quotes(filter);

  LedaFileViewer(fname,dname,filter);

  return 0;
}
