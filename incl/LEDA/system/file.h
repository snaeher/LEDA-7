/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  file.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef __LEDA_FILE__
#define __LEDA_FILE__

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 722163
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/system/basic.h>
#include <LEDA/core/list.h>

LEDA_BEGIN_NAMESPACE


extern __exportF bool create_process(string cmd, bool show=true);
extern __exportF bool create_process_wait(string cmd, bool show=true);
extern __exportF bool create_process_timeout(string cmd, float sec);

extern __exportF void play_sound(string fname);
extern __exportF bool battery_status(int& status, int& percent, int& minutes);

extern __exportF int get_number_of_cores();

extern __exportF bool fd_poll(int fd, int msec);

extern __exportF char read_console(int msec);
extern __exportF bool peek_console(int msec);
extern __exportF void flush_console();



/*{\Manpage {file} {} {Files and Directories}  }*/

/*{\Moperations 2 5 }*/

extern __exportF string set_directory(string new_dir);
/*{\Mfuncl     sets the current working directory to |new_dir| and 
               returns the name of the old cwd. }*/

extern __exportF string get_directory();
/*{\Mfuncl     returns the name of the current working directory. }*/

extern __exportF string get_home_directory();
/*{\Mfuncl     returns the name of the user's home directory. }*/

extern __exportF string get_directory_delimiter();
/*{\Mfuncl     returns the character that delimits directory names in a path
               (i.e.~``$\backslash$'' on Windows and ``/'' on Unix).}*/

extern __exportF void append_directory_delimiter(string& dir);
/*{\Mfuncl     appends the directory delimiter to |dir| if |dir| does not 
               already end with the delimiter.}*/

extern __exportF void remove_trailing_directory_delimiter(string& dir);
/*{\Mfuncl     removes the directory delimiter from |dir| if |dir| ends with 
               it.}*/

extern __exportF list<string> get_directories(string dir);
/*{\Mfuncl     returns the list of names of all sub-directories in 
               directory |dir|. }*/

extern __exportF list<string> get_directories(string dir, string pattern);
/*{\Mfuncl     returns the list of names of all sub-directories in 
               directory |dir| matching pattern. }*/

extern __exportF list<string> get_files(string dir);
/*{\Mfuncl     returns the list of names of all regular files in 
               directory |dir|. }*/

extern __exportF list<string> get_files(string dir, string pattern);
/*{\Mfuncl     returns the list of names of all regular files in 
               directory |dir| matching pattern. }*/

extern __exportF list<string> get_entries(string dir);
/*{\Mfuncl     returns the list of all entries (directory and files) of 
               directory |dir|. }*/

extern __exportF bool create_directory(string fname);
/*{\Mfuncl     creates a directory with  name |dname|, returns
               |true| on success. }*/

extern __exportF bool is_directory(string fname);
/*{\Mfuncl     returns true if |fname| is the path name of a directory
               and false otherwise. }*/

extern __exportF bool is_file(string fname);
/*{\Mfuncl     returns true if |fname| is the path name of a regular file
               and false otherwise. }*/

extern __exportF bool create_link(string name, string target);
/*{\Mfuncl     creates a symbolic link from |name| to |target|, returns
               |true| on success. }*/

extern __exportF bool is_link(string fname);
/*{\Mfuncl     returns true if |fname| is the path name of a symbolic link
               and false otherwise. }*/

extern __exportF unsigned long long size_of_file64(string fname);

extern __exportF size_t size_of_file(string fname);
/*{\Mfuncl     returns the size of file |fname| in bytes. }*/

extern __exportF time_t time_of_file(string fname);
/*{\Mfuncl     returns the time of last access to file |fname|. }*/

extern __exportF string tmp_dir_name();
/*{\Mfuncl     returns name of the directory for temporary files. }*/

extern __exportF string tmp_file_name();
/*{\Mfuncl     returns a unique name for a temporary file. }*/

extern __exportF bool delete_file(string fname);
/*{\Mfuncl     deletes file |fname| returns true on success and 
               false otherwise. }*/

extern __exportF bool copy_file(string src, string dest);
/*{\Mfuncl     copies file |src| to file |dest| returns true on success and 
               false otherwise. }*/

extern __exportF bool move_file(string src, string dest);
/*{\Mfuncl     moves file |src| to file |dest| returns true on success and 
               false otherwise. }*/

extern __exportF bool chmod_file(string fname, string option);
/*{\Mfuncl     change file permission bits. }*/

extern __exportF bool open_file(string fname);
/*{\Mfuncl     opens file |fname| with associated application. }*/

extern __exportF bool open_url(string url);
/*{\Mfuncl     opens web page |url| with associated application. }*/

extern __exportF int compare_files(string fname1, string fname2);
/*{\Mfuncl     returns $1$ if the contents of |fname1| and |fname2| differ 
               and $0$ otherwise. }*/

extern __exportF string first_file_in_path(string fname, string path, 
                                                         char sep = ':');
/*{\Mfuncl     searches all directories in string $path$ (separated by $sep$)
               for the first directory $dir$ that contains a file with
               name $fname$ and returns $dir/fname$ (the empty string
               if no such directory is contained in path). }*/

extern __exportF list<string> get_disk_drives();
/*{\Mfuncl     returns the list of all disk drives of the system. }*/


#if LEDA_ROOT_INCL_ID == 722163
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif



LEDA_END_NAMESPACE

#endif
