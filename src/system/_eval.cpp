/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _eval.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/system/basic.h>
#include <LEDA/internal/eval.h>

#include <stdlib.h>

#if defined(__win32__)
#if defined(MAXINT)
#undef MAXINT
#endif
#include<windows.h>
#endif





LEDA_BEGIN_NAMESPACE

/*
static long eval_id[64]={0xfa6be8dc,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xfa6be8dc};
*/


static const char* evalmsg="LEDA's functionality during evaluation is restricted. If you have further questions, please contact leda@algorithmic-solutions.com\nRestriction: ";

	EvalMsg evalHandler;

	string EvalMsg::GetWinTmpDir()
	{
		// function to retrieve Windows TMP directory
		char* tmpname = getenv("TEMP");
		if(tmpname == NULL) return string();
		else return string(tmpname);
	}

	string EvalMsg::GetLogFileName()
	{
#if defined(__unix__) || !defined(__win32__)
		// default for all other unix based systems
		string logfilename = "/var/tmp/leda_eval.log";
#else
		string logfilename = GetWinTmpDir();
		if(logfilename.length() == 0)
		{
			// ups, we could not retrieve the tmp directory, fallback to C:\"
			logfilename = "C:";
		}
		// append filename
		logfilename += "\\leda_eval.log";
#endif
		return logfilename;
	}

	void EvalMsg::LogMsg(const char* msg)  
	{
		FILE* log = fopen(GetLogFileName(), "a");
		if (log && !ferror(log)) {
			fputs("\t", log); fputs(msg, log); fputs("\n", log);
		}
		if (log) fclose(log);

//		NOTE: Sometimes we cannot use stream IO with Borland ...
//		ofstream log(GetLogFileName(),ios::app);
//		if (log.good()) log << "\t" << msg << endl;
	}

	void EvalMsg::print(const char* msg, bool append_eval_msg)
	{
		// build new message
		string mesg(msg);
		if (append_eval_msg) mesg = string(evalmsg) + mesg;

		// first log the message
		LogMsg(mesg);

#if defined(__win32__)
		// then write to window if possible
		MessageBox(NULL,mesg,"LEDA EVALUATION",MB_OK);
#endif
#if !defined(__BORLANDC__) || !defined(GUIApp)
		// finally write to stderr (this may crash on Borland)
		cerr<< mesg << endl;
#endif

		abort();
	}

LEDA_END_NAMESPACE
