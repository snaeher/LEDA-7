#line 2529 "ext.nw"
package exthelp;
use Cwd;

# we glue local variables to the main scope
sub import {
  *LEDAROOT = *main::LEDAROOT;
}

sub usage {
  local($mode) = @_;
  if ($mode eq "Lman" || $mode eq "Ldoc" || 
      $mode eq "HTMLman" || $mode eq "HTMLdoc" || $mode eq "Cdoc") {
    print "Usage is
          $mode file [options]

Options are given in assignment syntax variable=value. There must be no
blank on either side of the equality sign. We list all variables and 
their possible values below. For each variable the default value of 
each option is given first.

size={12,11,10}
constref={no,yes}
partypes={no,yes}
numbered={no,yes}
xdvi={yes,no}
warnings={yes,no}
includefile={no,yes}
indexing={no,yes}
informational={yes,no}
ack={yes,no}
usesubscripts={no,yes}
latexruns={1,2,0}
delman={yes,no}
filter={all,signatures,definition,types,creation,
operations,implementation,example,opname}

$mode can be customized by putting options in a file 
$mode.cfg in either the home directory or the working 
directory.

Call 
      $mode ltools 
        for more information on the tools usage.
      $mode mancommands 
        for a short overview of usable manual commands.\n\n";
} # end of if Lman,Ldoc,Cdoc

if ($mode eq "Fman") {
  print "Usage is
          $mode file filter

where the file name is of the form T.[h|w|lw] and T is either 
the name of a LEDA type, e.g., list, sortseq, or point, or 
the name of a user defined data type. The value of filter is 
one of 

 { all, signatures, definition, types, creation, 
   operations, implementation, example, opname }

Call 
      $mode ltools 
        for more information on the tools usage. 
      $mode mancommands 
        for a short overview of usable manual commands.\n\n" ;
} # end of if Fman

  exit;
}

sub mancommands {
  local($mode) = @_;
  if ($mode eq "Lman" || $mode eq "Ldoc" || $mode eq "Fman") {
    print "Manpage start header is:

   /*{\\Manpage {DT} {T1,..,Tk} {short_description} [MVAR]}*/
   afterwards common variables are 
   \\Mname = DT<T1,..,Tk>
   \\Mvar =  MVAR

Manual sections are:

   /*{\\Mdefinition specification_text}*/

   /*{\\Mgeneralization C1#C2#...#Ck}*/

   /*{\\Mtypes [W]}*/
       /*{\\Mtypedef typedef_documentation}*/
       /*{\\Mtypemember type_documentation}*/
       /*{\\Menum type_documentation}*/

   /*{\\Mcreation [MVAR] [W]}*/
       /*{\\Mcreate constructor_specification }*/
       /*{\\Mdesctruct desctructor_specification }*/

   /*{\\Moperations [W1] [W2]}*/          
   /*{\\Mvirtual [W1] [W2]}*/          
   /*{\\Mpurevirtual [W1] [W2]}*/          
       /*{\\Mop semantic_description }*/        
       /*{\\Mstatic semantic_description }*/
       /*{\\Mconversion semantic_description }*/
       /*{\\Mbinop semantic_description }*/
       /*{\\Munop semantic_description }*/
       /*{\\Marrop semantic_description }*/ 
       /*{\\Mfunc semantic_description }*/
       /*{\\Mfunop semantic_description }*/
       /*{\\Mfunobj semantic_description }*/
       /*{\\Mbinopfunc semantic_description }*/
       /*{\\Munopfunc semantic_description }*/ 
       /*{\\Mfunobj{return_type} semantic_description }*/

   /*{\\Mevents [W]}*/
       /*{\\Mevent event_specification }*/

   /*{\\Mimplementation implemenation_information }*/
   /*{\\Mexample example_description }*/

Additional man commands
  /*{\\Mtext some_text }*/
  /*{\\Moptions nextwarning=no }*/
  /*{\\Moptions outfile=new_man_file_name }*/
  /*{\\Msubst pattern replacement }*/
Common tex macros for man comments are 
  \\setopdims[2], \\restoreopdims, \\precond[1], \\headerline[1]
Verbatim code in example section with environment 
  \\begin|end{Mverb}.\n\n"; 
}
exit;
}

sub ltools {
  local ($mode,$dontshowinfo) = @_;
  local $owd = $ENV{"PWD"};
  chdir ("/tmp");
  local($pid)=$$;
if ($mode eq "Fman") {
  system("cat $LEDAROOT/Manual/MANUAL/DocTools.tex");
}
else {
  $outfile = "/tmp/" . $pid . "-ext.tex";
  open(OUTPUT,">$outfile");
  print OUTPUT "\\documentclass\[11pt,a4paper\]\{article\}\n\n"; 
  print OUTPUT "\\usepackage\{html\}";
  print OUTPUT "\\input $LEDAROOT/Manual/tex/MANUAL.pagesize\n\n";
  print OUTPUT "\\input $LEDAROOT/Manual/tex/MANUAL.mac\n\n"; 
  print OUTPUT "\\begin\{document\}\n\n";
  close(OUTPUT);
  system("cat $LEDAROOT/Manual/MANUAL/DocTools.tex  >> $outfile");
  open(OUTPUT,">>".$outfile);
  print OUTPUT "\\end\{document\}\n\n";
  close(OUTPUT);
  system("latex /tmp/$pid-ext.tex $dontshowinfo");
  system("xdvi -s 0 /tmp/$pid-ext.dvi $dontshowinfo");
  system("rm -f /tmp/$pid-ext.*");
}
chdir ("$owd");
exit;
}

1;

