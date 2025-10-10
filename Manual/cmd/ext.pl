#line 1170 "ext.nw"
  
#line 2429 "ext.nw"
use Cwd;
use extout; 
use exthelp; 
use extbasic; 


#line 1171 "ext.nw"
  
#line 1186 "ext.nw"
$mode = $ARGV[0] && shift;
$filearg = $ARGV[0] && shift;

if ($mode eq "Mkman" || $mode eq "lextract") { $numbered = "yes"; }
else { $numbered  = "no";}      
  # Usually manual pages are not numbered
$ack = "yes";            
  # ext asks for acknowledgments when it is confused
$constref = "no";   
  # const-& pairs are usually suppressed
$partypes = "no";   
  # operators suppress argument types that are identical to Mname
$usesubscripts = "no";    
  # usually we do not write indexed variables as subscripts
$size = 12;             
  # default size is 12pt
$xdvi = "yes";
$warnings = "yes";
$informational = "yes";
  # default is to give warnings, to give progress report 
$debugging = "no";
  # no debugging information
$indexing = "no";
$indexentry = "";
  # create no indexing output, indexentry is global index entry buffer

$filter = "all";        
  # usually we show everything

$pid = "";            
  # we do not know the process id yet
$latexruns=1;
  # one latex compilation
$extractonly = "no";
  # switch to extract only manpages
$delman = "yes";     
  # Ldoc usually removes manual comments
$outfile = "";       
  # the default is that we determine the outfile
$dvioutfile = "";     
  # only relevant for Mkdvi
$section = "section"; 
  # each manual page is a LaTeX section 
$nextwarning = "yes"; 
  # default is to show the next warning
$justset = "no";      
  # $nextwarning was not just set to no
$noweaveoptions = "";  
  # we start with no options
$xcomments = "no";
  # we don't show comments starting with X

$includefile = "no";
$includeprefix = "LEDA";
  # an include statement starts with LEDA/
$LEDAROOT = "LEDAROOT";
$LEDAROOT = $ENV{"$LEDAROOT"};
@invisible_words = ('__exportC', '__exportF', '__exportD', '__typename');

if ($mode eq "Lman" || $mode eq "Mkman" || $mode eq "lextract" ||
    $mode eq "HTMLman" || $mode eq "HTMLext" || $mode eq "Fman" )
      { $print_title = "yes"; }
else  { $print_title = "no"; }

if ( $mode eq "Fman" )    { require extFman; extFman->import(); }
elsif ( $mode eq "Cdoc" ) { require extCdoc; extCdoc->import(); }
elsif ( $mode =~ /HTML/ ) { require extHTML; extHTML->import(); }
else                      { require extLdoc; extLdoc->import(); }


#line 1172 "ext.nw"
  
#line 1260 "ext.nw"
MODE::parse_parameters();
if ($informational eq "no") { $dontshowinfo = '> /dev/null'; }
if ($pid eq "") { $pid = $$; }

$showsem = 1;          # usually we show the semantics of functions
if ($filter eq "signatures") {
  $filter = "operations";
  $showsem = 0;
}

#line 1173 "ext.nw"
  
#line 1302 "ext.nw"
extout::info("$mode version 3.2");
if ( !$filearg ) { exthelp::usage($mode); }
if ( $filearg eq "mancommands" ) {
  exthelp::mancommands($mode);
}
if ( $filearg eq "ltools" || $filearg eq "Lman" ||
     $filearg eq "Ldoc") {
  exthelp::ltools($mode,$donshowinfo);
}

#$MANFILES = "$LEDAROOT/incl/$includeprefix:$LEDAROOT/incl/$includeprefix/generic";

#
# sn 05/2023
# adjust MANFILES variable to new (hierarchical) include directory structure
#

$MANFILES = "$LEDAROOT/incl/$includeprefix/coding:$LEDAROOT/incl/$includeprefix/core:$LEDAROOT/incl/$includeprefix/geo:$LEDAROOT/incl/$includeprefix/geo/generic:$LEDAROOT/incl/$includeprefix/graph:$LEDAROOT/incl/$includeprefix/graphics:$LEDAROOT/incl/$includeprefix/numbers:$LEDAROOT/incl/$includeprefix/system";

#printf("MANFILES = %s\n",$MANFILES); 


$MANDVIS = "$LEDAROOT/Manual/DVI";
$LMANFILES = $ENV{"LMANFILES"};
if ( $LMANFILES ) { $MANFILES = $LMANFILES; }
$LMANDVIS = $ENV{"LMANDVIS"};
if ( $LMANDVIS ) { $MANDVIS = $LMANDVIS; }




if ($filearg =~ /(.*)\.[whln]/) { $basepath = $1; }
else                            { $basepath = $filearg; }

if ( $basepath =~ /.*\/(\w+).*/ ) { $basename = $1; }
else { $basename = $basepath; }

if (-e ($basepath . "\.w")) 
{ $INPUT = $basepath . "\.w" ; $kind = "Cweb"; $ext = "w"; goto DONE; }
if (-e ($basepath . "\.web")) 
{ $INPUT = $basepath . "\.web" ; $kind = "Cweb"; $ext = "web"; goto DONE; }
if (-e ($basepath . "\.lw")) 
{ $INPUT = $basepath . "\.lw" ; $kind = "Lweb"; $ext = "lw"; goto DONE; }
if (-e ($basepath . "\.nw")) 
{ $INPUT = $basepath . "\.nw" ; $kind = "noweb"; $ext = "nw"; goto DONE; }
if (-e ($basepath . "\.h")) 
{ $INPUT = $basepath . "\.h" ; $kind = "h"; goto DONE; }
@MANPATHS = split(/\:/,$MANFILES);
if ($mode =~ /man$/ ) {
  foreach my $manpath (@MANPATHS) {
    my $longname = "$manpath/$basename";
    if (-e ($longname . "\.h") ) {
      $INPUT = $longname . "\.h" ; $kind = "MANROOTtype"; goto LMAN;
    }
  }
  foreach my $manpath (@MANPATHS) {
    $greppath = qx{grep \'$basename\' $manpath/*.h };
    @greplist = split('\n',$greppath);
    FOUND: while ( $filecand = shift(@greplist) ) {
             if ( $filecand =~ s/(.*?)\:.*\\Manpage.*/$1/ ) { last FOUND; }
           }
    $INPUT = $filecand;
  }
}

LMAN:
if ($mode eq "Lman" && $xdvi eq "yes" && 
    (-e "$MANDVIS/$basename.dvi")) {
  extout::info("  Taking $MANDVIS/$basename.dvi as a shortcut");

# sn 2023
# set xdvi shrinkfactor to automatic "xdvi -s 0"   
# the default (8) makes xdvi use very tiny fonts

# system("xdvi -s 0 $MANDVIS/$basename.dvi $dontshowinfo");

  system ("dvipdfm -o $MANDVIS/$basename.pdf $MANDVIS/$basename.dvi");

  extout::info("\n  open pdf file ...");
  system ("xdg-open $MANDVIS/$basename.pdf");

  exit;
}

DONE:
open (INPUT) || 
die "Error: Cannot find input file $basepath.[w|lw|nw|h]: $!\n\n";

if ($mode ne "Fman") {
  if ($outfile eq ""  || $mode eq "lextract" ||
      $mode eq "Ldoc" || $mode eq "Cdoc" || $mode eq "HTMLdoc") {
    $outfile = "/tmp/" . $pid . "-ext.tex";
    $existing_outfiles{$outfile} = 1;
  }

  open(OUTPUT,">$outfile") or die "Cannot open $outfile\n";
  extout::info("Reading input file $INPUT to extract manual ...");
}

MODE::output_preamble(); 
# procedure depending on dynamic linkage via require module


#line 1174 "ext.nw"
  
#line 1402 "ext.nw"
$Mvar    = "";
$Mtype   = ""; 
$Mname   = "";
$MIname =  "";
main_loop:
{ # $_ is either undefined or the last line of an Mcomment
  
#line 1431 "ext.nw"
# I advance $_ to a non-empty line (WEB-directives are output but count as 
# empty lines otherwise) or beyond the end of the file

$_ = <INPUT>;

build_code_unit:
    while ($_ && (($_ =~ /^\s*$/) || ($_ =~ /^\@[sf]/)))
    { $_ = <INPUT>; }
    if (! $_) { last main_loop; }   # input exhausted
    $code_unit = "";
    # $_ exists, is non_empty, and contains no @, and $code_unit is empty
    # The code_unit is either terminated by an Mcomment or an empty line
    # (empty line: only white space characters) or the end of the file
    while ($_ && !(/\/\*\{\\(M|X)/ || /^\s*$/)) {
      if ($_ =~ /\/\* *\{M/) {
        extout::warning("I encountered /*{M... did you really mean it?");
      }     
      $code_unit .= $_;   # append current line to code unit
      $_ = <INPUT>;
    }

# The current line either does not exist or is either empty or the 
# begin of an Mcomment.
# Skip empty lines.

while ( $_ && /^ *$/ ) { $_ = <INPUT>; }
# the current line is non-empty (if it exists).

if (! $_) { last main_loop; }   # input exhausted
# the current line is non-empty.

if (! (/\/\*\{\\(M|X)/) ) { 
  goto build_code_unit;  # start new code unit.
}  

foreach $invis_word (@invisible_words) {
  $code_unit =~ s/$invis_word//g;
}
$extout::code = $code_unit; # stored in output module for error message


#line 1409 "ext.nw"
  # $_ is an Mline and variables $extout::code and $code_unit 
  # contain the current code unit. 
  
#line 1484 "ext.nw"
s/^(.*)\/\*\{\\//; # remove begin comment and everything before it
local($stuff)=$1;
$extout::comment = $&.$_;   # stored in output module for error message
if ($stuff =~ /\S/) { # issue warning if something non-white before comment
  extout::warning("Appending non-white stuff in front of begin comment."); 
  $code_unit .= $stuff;
}

$Mcomment = "";

while ($_ && (! ( /\}\*\// ) ) ) {

  if (/\} *\*\//) 
  { extout::warning("Encountered } */ in manual comment. Did you mean }*/ ?"); }
  $Mcomment .= $_;
  $_ = <INPUT>;
}
if (! $_) { die "Error: missing end comment\n"; }

s/\}\*\/(.*)$/ /;  # replace end comment and everything after it by a blank
if ($1 =~ /\S/) { extout::warning("I ignored non-white after Mcomment"); }

$Mcomment .= $_;   
$extout::comment = $Mcomment; # stored in output module for error message

# Mcomment contains the entire manual comment. We extract the command (the
# maximal alphanumeric prefix).

$Mcomment =~ /^(\w*)\W.*/;
$command = $1;
$Mcomment =~ s/$command *//;  # remove command and succeeding blanks
$command =~ s/^M//;           # remove the M
if ( $xcomments eq "yes" ) { $command =~ s/^X//; } # transform Xcommand into command
if ( $xcomments eq "no" && $command =~ /^X/ ) 
{ goto build_code_unit; }  # start new code unit.

if ( $command ne "options" ) {  extbasic::apply_Msubst(*Mcomment); }
# we replace all substitution strings in the comment as an exception
# you can escape any string by a preceding backslash.


#line 1412 "ext.nw"
  # $Mcomment contains the manual command (without the brackets /*{\M and }*/
  # $Mcommand contains the command (without the leading M)
  
#line 1541 "ext.nw"
if ($nextwarning eq "no" && $justset eq "no") { $nextwarning = "yes";} 
$justset = "no";
switch: {
  if ($command eq 'options') {
    
#line 1592 "ext.nw"
$Mcomment =~ s/\s//g;
$Mcomment =~ /^(\w+)=(.*)$/ ;
if ($1 eq "outfile") {
  if ( (!($mode =~ /man/)) && ($mode ne "lextract") ) { 
  # gives information if we are in Lman/Mkman mode 
  # where we don't want to change the outfile but 
  # just parse the manual comments in one file in
  # the given sequence
    close(OUTPUT);
    $outfile = $2;
    if ( $existing_outfiles{$outfile} == 1 )
    { open OUTPUT, ">>$outfile"; }
    else
    { $existing_outfiles{$outfile} = 1;
      open OUTPUT, ">$outfile";
      MODE::output_preamble();
    }
  }
} else {  eval "\$$1 = \$2"; }
if ($nextwarning eq "no") { $justset = "yes"; }


#line 1545 "ext.nw"
                
    last switch;
  }
  if ($command eq 'subst') {
    
#line 1624 "ext.nw"
while ($Mcomment =~ s/^([^\n\r]*)[\r\n]//) 
{ $substline = $1;
  if ($substline =~ /^(.*)#(.*)$/ || 
      $substline =~ /^ *(\S+) +(\S+) *$/)
  { extbasic::store_Msubst($1,$2); }
  elsif ($substline =~ /\S/) 
  { extout::warning("Msubst does not understand $substline"); }
}
 
#line 1549 "ext.nw"
              
    last switch;
  }
  if ($command eq 'anpage') {
    
#line 1641 "ext.nw"
if (! ($Mcomment =~ /\{([^\{\}]*)\}\s*\{([^\{\}]*)\}\s*\{([^\{\}]*)\}\s*\{([^\{\}]*)\}/ || 
       $Mcomment =~ /\{([^\{\}]*)\}\s*\{([^\{\}]*)\}\s*\{([^\{\}]*)\}/ ) )          
{ extout::warning("Manpage expects either three or four arguments"); }
$Mtype = $1;
$par_list = $2;
$title = $3;
if ($4) { $Mvar = $4; }
if ($par_list =~ /^ *$/) 
{ $MIname = $Mname = $Mtype; }
else {
  # remove excessive blanks in parlist but be case sensitive
  $par_list =~ s/ //g;
  local($opar_list) = $par_list;
  @params = split(',',$par_list);
  local($longtemplatepar)= (length($par_list) > 20);
  foreach my $para (@params) 
  { if ( length($para) > 5 ) { $longtemplatepar=1; } }
  if ( $longtemplatepar ) 
  { $par_list = " ".join(", ",@params)." "; } 
  # we do something for long template params 
  # if we produce ,blank the command covert_M enables
  # line breaks in the template list, thus we do this
  # as soon as one param is longer than 7 characters or
  # if the parameter list is longer than 20 characters
  $Mname = $Mtype."<".$par_list.">";
  if ( length($opar_list) < 11 ) { $MIname = $Mname; }
  else { $MIname = $Mtype."<".substr($opar_list,0,7)."...>"; }
}

MODE::index_entry($MIname,"class");
MODE::new_manpage();

$includeline="";
if (($kind eq "MANROOTtype" || $kind eq "h") && $includefile eq "yes") {
  $includeline = $INPUT;
  if ( $includeline =~ /$includeprefix/ ) 
  { $includeline =~ s/.*($includeprefix.*)/$1/; } 
  # chop off before $includeprefix
  $includeline = "\\\#include \< $includeline \>"; 
}

 
#line 1553 "ext.nw"
                        
    last switch;
  }
  if ($command eq 'definition' || 
      $command eq 'generalization' ||
      $command eq 'types' ||
      $command eq 'creation' ||
      $command eq 'operations' ||
      $command eq 'virtual' ||
      $command eq 'purevirtual' ||
      $command eq 'events' ||
      $command eq 'implementation' || 
      $command eq 'example') {
    
#line 1704 "ext.nw"
if ($command eq "creation" || $command eq "types" || $command eq "events") {
  # Mcomment may contain a varname and/or a length.
  @params = split(' ',$Mcomment);  # split at blanks
  foreach $i (0 .. $#params) 
  { $params[$i].='cm' if $params[$i] =~ /^[0-9\.]*$/; 
    if ( $params[$i]=~ /^[a-zA-Z]/ ) {
      $Mvar = $params[$i];
      extout::error("Mvar not at first position in comment.") if ( $i != 0 );
    } 
  }
  MODE::set_vars(@params);
  $Mcomment = "";
}
elsif ($command eq "operations" || $command eq "virtual" || 
       $command eq "purevirtual" ) {
  # Mcomment is either empty or a b where a and b are lengths
  # If the lengths are without dimension then we add cm
  local($settings)= "";
  @params = split(' ',$Mcomment);  # split at blanks
  foreach $i (0 .. $#params) 
  { $params[$i].='cm' if $params[$i] =~ /^[0-9\.]*$/; }
  MODE::set_vars(@params);
  $Mcomment = "";
}
elsif ($command eq "generalization") {
  # Mcomment is a list of classes which generalize the current class
  @params = split('#',$Mcomment);  # split at '#'
  $Mcomment = MODE::generalization(@params);
}
$command =~s/purevirtual/Pure Virtual Operations/;
$command =~s/virtual/Virtual Operations/;

$currentsection = $command;

# print mansections:

if ($filter eq "all" || $filter eq $currentsection) {
  extbasic::subst_vars(*Mcomment);
  MODE::top_section();
}
      

#line 1566 "ext.nw"
                        
    last switch;
  }
  if ($command eq 'text'  || $command eq "preamble") {
    
#line 1753 "ext.nw"
if ($filter eq "all" || $filter eq $currentsection ) { 
  extbasic::subst_vars(*Mcomment);
  MODE::print_text($Mcomment);
}


#line 1570 "ext.nw"
                         
    last switch;
  }
  
#line 1778 "ext.nw"
# PARSING STARTS HERE:
$prefix = "";
$funcname = "";
$postfix = "";
$signature = ""; 
$enumconsts = "";
$type = "";
$fname = "";
$template = "";
$cleanprototype = "";
$static = 0;
$constructor = 0;
$operator = 0;
$conversion = 0;
$destructor = 0;
$enum = 0;
$typedef = 0;
$localclass = 0;


if (!$code_unit) {
  extout::warning("current code unit is empty"); 
  last switch;
}
#line 1859 "ext.nw"
$code_unit =~ s/\@\+/ /g; # remove @+
$code_unit =~ s/\@\// /g; # remove @/
$code_unit =~ s/\@\|/ /g; # remove @|
$code_unit =~ s/\@\#/ /g; # remove @#
$code_unit =~ s/\@\;/ /g; # remove @;
$code_unit =~ s/\@\,/ /g; # remove @,

extout::debug("\nDEBUG code unit at start=$code_unit");

if ($code_unit =~ /\/\*/) {      # commented code units
  extout::warning("code unit contains a comment. ".
                  "I remove the lines containing /* and */");
  $code_unit =~ s/ *\/\*.*//;  # remove first line
  $code_unit =~ s/ *\*\/ *//;  # remove last line
  $original_code_unit = $code_unit;
}

$code_unit =~ s/.*\<\<.*?\>\>\=//g;  # remove chunk labels and chars in front
$code_unit  =~ s/\/\/.*\n//g;  # remove C++ comment to end of line
$code_unit =~ s/\t/ /g;        # replace tab by blank
$code_unit =~ s/ *\#.*\n/ /g;  # remove all lines with compiler directives 
$code_unit =~ s/\n/ /g;        # replace newline characters by blanks

if ($code_unit =~ s/\{(.*)\}/\;/ ) 
{ $enumconsts = $1; }
$code_unit =~ s/\{.*/\;/g; # just an opening bracket purges rest
# note that we did the previous to enable a opening bracket behind
# the prototype not balanced by a closing one following way behind
# the man comment
while ($code_unit =~ s/\;\s*\;/\;/) {}        
# replaces white space-separated ; by a single ;
$code_unit =~ s/= *0 *; *$//; # removes = 0 ;
$code_unit =~ s/\; *$//; # remove last semicolon if only followed by whitespace
if ($code_unit =~ /\;([^\;]*)$/) {
  extout::warning("code unit contains several function definitions. " .
                 "I extracted\n $1");
  $code_unit = $1;
}

# At this point we have a single function definition in code_unit
extout::debug("code unit after bracket removal=$code_unit"); #DEBUGOUT


#line 1908 "ext.nw"
$code_unit =~ s/const *$//;  # remove const qualifier at end
$code_unit =~ s/::/doppeldoppel/g;  # replace :: by doppeldoppel
$code_unit =~ s/\) *:.*$/\)/;    # remove initialization constructor call
$code_unit =~ s/doppeldoppel/::/g;   # reintroduce ::
$code_unit  =~ s/(^|\W+)virtual(\W+)/$1$2/;             # remove blanks virtual blanks     
$code_unit  =~ s/(^|\W+)friend(\W+)/$1$2/;              # remove friend
$code_unit  =~ s/(^|\W+)inline(\W+)/$1$2/;              # remove inline
$code_unit  =~ s/(^|\W+)extern(\W+)/$1$2/;              # remove extern
extbasic::apply_Msubst(*code_unit);
$cleanprototype = $code_unit;

$template = $2 if ($code_unit =~ s/(^|\W+)template *<([ ,\w]*)>/$1/ ); 
# remove template definition
$static = ($code_unit  =~ s/(^|\W+)static(\W+)/$1$2/);  #remove static and record
$enum   = ($code_unit  =~ s/(^|\W+)enum(\W+)/$1$2/);     #remove enum
$typedef = ($code_unit  =~ s/(^|\W+)typedef(\W+)/$2/); #remove typedef

$localclass = ($code_unit  =~ s/(^|\W+)class(\W+.*)/$2/); #remove class
extout::debug("code unit after qualifier removal=$code_unit"); #DEBUGOUT

#line 1947 "ext.nw"
  $conversion = ($code_unit =~ /operator\s.+?\s*\(\s*\)/); 
  if ($code_unit =~ /\( *\) *\(.*\)/) { 
    $code_unit =~ s/\( *\) *\((.*)\)//; 
    $par_list = $1;
  } else { 
    $code_unit =~ s/\((.*)\)//; 
    $par_list = $1;
  }

  $operator = (($code_unit =~ /operator/) && !$conversion);  # symbol operator
  $destructor = ($code_unit =~ /^ *\~/);

  extbasic::remove_enclosing_blanks($code_unit); # I do anchored matches below

  extout::debug("code unit after first clean-up=$code_unit");
  extout::debug("par_list = $par_list");

extraction:
{ if ($destructor) { last extraction; }
 
  if ($conversion) {    # a conversion function: operator type
    if (!($code_unit =~ s/ *operator *//)) {
      extout::warning("expected a conversion function");
    }
    $fname = $code_unit;
    last extraction;
  }

  if ($operator) { # an operator: type operator opsymbol  
    $code_unit =~ /^(.*) *operator *(.*)$/;
    $type = $1;
    $fname = $2;
    last extraction;
  }
  
#line 2001 "ext.nw"
if ( $code_unit eq $Mtype ) 
{ $constructor = 1; }
elsif ( $currentsection ne "types" ) {
  # a function
  if ($code_unit =~ /^([:\w]+)$/ ) {
    $type = "int";
    $fname = $code_unit;
  } else { 
    extout::debug("proper function $code_unit");
    $code_unit =~ /^(.*[^\:\w])([\:\w]+)$/ ;
    $type = $1; 
    $fname = $2;
    extout::debug("proper function $type $fname");
    while ($type =~ s/^ //) {}
    while ($type =~ s/ $//) {} # remove beginning and trailing blanks
    if (($type =~ /^\s*const\W/) && ($constref eq "no")) {  
      # remove const & bracket
      $type =~ s/^\s*const//; 
      $type =~ s/ *\& */ /;
    }
    $type =~ s/ *\& */\& /g;
    extout::debug("proper function $type $fname");
  }
}


#line 1982 "ext.nw"
}    

#line 2036 "ext.nw"
extbasic::remove_enclosing_blanks($type);
extbasic::remove_enclosing_blanks($fname);
extbasic::remove_enclosing_blanks($Mvar);
extbasic::remove_enclosing_blanks($Mtype);
extbasic::remove_enclosing_blanks($command);

#line 2051 "ext.nw"
@params = split(/,/,$par_list);  # split at commas
$par_list = "";
$i = 0;
while ($i <= $#params) {
  $j = $i + 1;
  while ($params[$i] =~ /</  && !($params[$i] =~ /\(.*<.*\)/) &&
         (($params[$i]=~tr/</</) > ($params[$i]=~tr/>/>/))) {
    # append all comma separated parameter parts together
    # as part of a template type list or comma separated
    # init parts, the second condition allows initialization
    # by bracketed expressions containing < or <<
    $params[$i] .= "," . $params[$j]; $j++;
  }
  if (($params[$i] =~ /const .*\&/) && ($constref eq "no")) {
    $params[$i] =~ s/ *const //;
    $params[$i] =~ s/ *\& */ /; # replace blanks&blanks by a single blank
    # $params[$i] =~ s/\&//;     simply remove & if not followed by a blank.
  }
  extbasic::remove_enclosing_blanks($params[$i]);
  $params[$i] =~ s/ *\& */\& /g;

  if ($i > 0) {$par_list .= "\, ";}
  $par_list .= $params[$i];
  $i = $j;
}

$varname = $Mvar;
$signature = $fname . "(" . $par_list . ")";

if ( !$showsem ) { $Mcomment = ""; }
extbasic::subst_vars(*Mcomment);

if ($command =~ /^enum$/) {
  
#line 2393 "ext.nw"
if ( !$enum ){ extout::warning("Enum should start with keyword enum"); }
$code_unit =~ s/^ *(\w*)//;    # remove blanks type blanks {
$type = $1;
$enumconsts =~ s/\s*//g;
$enumconsts =~ s/,/, /g;
if ($filter eq "all" || $filter eq $currentsection || 
    $type =~ /$filter/ || $enumconsts =~ /$filter/ ) {
  MODE::index_entry($type, "local");
  my @enumvals = split(',',$enumconsts);
  foreach $i (0 .. $#enumvals) {
    $enumvals[$i] =~ s/(.*?)=.*/$1/;
    MODE::index_entry($enumvals[$i],"local"); 
  }
  MODE::print_enum($type,join(", ",@enumvals),$Mcomment);
}

#line 2084 "ext.nw"
            
  last switch;
}
if ($command =~ /^typemember$/) {
  
#line 2360 "ext.nw"
if ( $typedef ) {
  $code_unit =~ s/.*\s+(\w+)\s*$/$1/;
  $type = $1;
} elsif ($localclass) {
  $type = $code_unit;
} else 
{ extout::warning("Type member should be a typedef or a class"); }
if ($filter eq "all" || $filter eq $currentsection || 
    $fname =~ /$filter/ ) {
  MODE::index_entry($type, "local");
  MODE::print_typemember($Mname,"$type",$Mcomment);
}

#line 2088 "ext.nw"
                  
  last switch;
}
if ($command =~ /^typedef$/) {
  
#line 2374 "ext.nw"
if ( !$typedef ) 
{ extout::warning("Typedef should countain a typedef"); }
local $from;
$code_unit =~ s/typename//g;
$code_unit =~ s/(.*)\s+(\w+)\s*$/$1 $2/;
$from = $1;
$type = $2; 
if ($filter eq "all" || $filter eq $currentsection || 
    $fname =~ /$filter/ ) {
  MODE::index_entry($type,"local");
  MODE::print_typedef($from,$type,$Mcomment);
}


#line 2092 "ext.nw"
               
  last switch;
}
if ($command =~ /^event$/) {
  
#line 2412 "ext.nw"
# $code_unit =~ /^\s*\w+\<(.*)\>\s*(\w+)/;
# change due to Stefan/Oliver
$code_unit =~ /^\s*\w+\<(.*)\>\s*(.*)/;
$par_list=$1;
my $event = $2;
my $scoper;
if ($static) { $scoper = $Mname."::"; }
else { $scoper = $Mvar."."; }
if ($filter eq "all" || $filter eq $currentsection || 
    $fname =~ /$filter/ ) {
  MODE::index_entry($event, "local");
  MODE::print_event($scoper,$event,$par_list,$Mcomment);
} 

#line 2096 "ext.nw"
             
  last switch;
}
if ($command =~ /^opl?$/) {
  
#line 2153 "ext.nw"
if ($operator || $constructor || $conversion || $static) {
 extout::warning("Mop applies only to member functions " .
                 "and not to operators,...");
}
if ($filter eq "all" || $filter eq $currentsection || 
    ($fname =~ /$filter/)) {
  MODE::index_entry("$fname($par_list)", "local");
  MODE::print_function($type,$signature,$Mcomment,
                       "$Mvar\.",$fname,$par_list); 
}

#line 2100 "ext.nw"
          
  last switch;
} 
if ($command =~ /^funcl?$/) {
  
#line 2165 "ext.nw"
if ($operator || $constructor || $conversion || $static) {
  extout::warning("Mfunc applies only to functions " .
                  "and not to operators,... ");
}
if ($filter eq "all" || $filter eq $currentsection || 
    ($fname =~ /$filter/) ) {
  MODE::index_entry("$fname($par_list)", "global");
  MODE::print_function($type,$signature,$Mcomment,"",$fname,$par_list); 
}
     
#line 2104 "ext.nw"
            
  last switch;
}
if ($command =~ /^binopl?$/) {
  
#line 2178 "ext.nw"
if (!$operator) 
{ extout::warning("Mbinop applies only to operators"); }

my $Mreplace = $Mname . " ";
if ($partypes eq "no") {
  while ($par_list =~ /$Mreplace/) { $par_list = $` . $';} 
}    

$signature = $Mvar. " " . $fname . " " . $par_list;
if ($filter eq "all" || $filter eq $currentsection || 
    $filter eq ("operator" . $fname) ){
  MODE::print_function($type,$signature,$Mcomment);
}


#line 2108 "ext.nw"
             
  last switch;
}
if ($command =~ /^binopfuncl?$/) {
  
#line 2197 "ext.nw"
if (! $operator) {
  extout::warning("Mbinopfunc applies only to operators");
}
my $Mreplace = $Mname . " ";
if ($partypes eq "no") {
  while ($par_list =~ /$Mreplace/) { $par_list = $` . $'; } 
}        
$par_list =~ s/\, / $fname /;
$signature = $par_list;
if ($filter eq "all" || $filter eq $currentsection || 
    $filter eq "operator".$fname) {
  MODE::print_function($type,$signature,$Mcomment);
}

#line 2112 "ext.nw"
                 
  last switch;
}
if ($command =~ /^arropl?$/) {
  
#line 2213 "ext.nw"
if ( !$operator ) {
  extout::warning("Marrop applies only to operators");
}
$signature = $varname . "\[" . $par_list . "\]";
if ($filter eq "all" || $filter eq $currentsection ||
    $filter eq "arrop" ) {
  MODE::print_function($type,$signature,$Mcomment);
}

#line 2116 "ext.nw"
             
  last switch;
}
if ($command =~ /^funopl?$/) {
  
#line 2223 "ext.nw"
if (! $operator) {
  extout::warning("Mfunop applies only to operators");
}
$signature = $varname . "\(" . $par_list . "\)";
if ($filter eq "all" || $filter eq $currentsection || 
    $filter eq "funop" ) {
  MODE::print_function($type,$signature,$Mcomment);
}

#line 2120 "ext.nw"
             
  last switch;
}
if ($command =~ /^funobjl?$/) {
  
#line 2336 "ext.nw"
if ($operator || $conversion || $static) {
  extout::warning("Mfunobj applies only to function objects " .
                 "and not to operators,...");
}
$prefix = "";
$funcname = $fname;
$postfix = $par_list;
if ( $Mcomment =~ /\s*\{([^\}]*)\}/ ) {
  $type = $1;
  $Mcomment = $';
} 
$signature = $Mname."(".$par_list.")";
if ($filter eq "all" || $filter eq $currentsection ||
    $filter eq "funobj" ) {
  MODE::print_constructor($mode,$type,$Mname,$par_list,$Mcomment); 
}


#line 2124 "ext.nw"
              
  last switch;
}
if ($command =~ /^unopl?$/) {
  
#line 2236 "ext.nw"
if (! $operator) {
  extout::warning("Munop applies only to operators");
}
unopcases: {
  if ($fname eq "->") {
    $signature = $varname . $fname;   # -> is postfix
    last unopcases;
  }
  if ($fname eq "new" || $fname eq "delete" ) {
    $signature = $fname . ' ' . $varname;  # new and delete are prefix
    last unopcases;
  }
  if ($par_list) {
    $signature = $varname . $fname; # postfix ++ and --
    last unopcases;
  }       
  $signature = $fname . $varname; # all others are prefix operators 
}
if ($filter eq "all" || $filter eq $currentsection || 
    $filter =~ /$fname/ || $filter eq ("operator" . $fname )) {
  MODE::print_function($type,$signature,$Mcomment);
}

#line 2128 "ext.nw"
            
  last switch;
}
if ($command =~ /^unopfuncl?$/) {
  
#line 2264 "ext.nw"
if (! $operator) {
  extout::warning("Munop applies only to operators");
}
     
unopfunccases: {
  if ($fname eq "->") {
    $signature = $varname . $fname;   # -> is postfix
    last unopfunccases;
  }
  if ($fname eq "new" || $fname eq "delete" ) {
    $signature = $fname . ' ' . $varname;  # new and delete are prefix
    last unopfunccases;
  }
  if ($par_list =~ /,/) {
    $signature = $varname . $fname; # postfix ++ and --
    last unopfunccases;
  }       
  $signature = $fname . $varname; # all others are prefix operators 
}

if ($filter eq "all" || $filter eq $currentsection ||
    $filter =~ /$fname/ || $filter eq ("operator" . $fname )) {
  MODE::print_function($type,$signature,$Mcomment);
}

#line 2132 "ext.nw"
                
  last switch;
}
if ($command =~ /^staticl?$/) {
  
#line 2291 "ext.nw"
if (!$static) {
  extout::warning("Mstatic applies only to static member functions");
}
if ($filter eq "all" || $filter eq $currentsection || 
    ($fname =~ /$filter/)) {
  MODE::index_entry("$fname($par_list)", "local");
  MODE::print_function($type,$signature,$Mcomment,"$Mname\:\:",$fname,$par_list);
}

#line 2136 "ext.nw"
              
  last switch;
}
if ($command =~ /^conversion/) {
  
#line 2303 "ext.nw"
if (!$conversion) {
  extout::warning("Mconversion applies only to conversion operators");
}
if ($filter eq "all" || $filter eq $currentsection ){
  MODE::print_function($type,$signature,$Mcomment,"",$fname,$Mvar);
}


#line 2140 "ext.nw"
                  
  last switch;
}
if ($command =~ /^create/) {
  
#line 2313 "ext.nw"
if (!$constructor) 
{ extout::warning("Mcreate applies only to constructors"); }
if (!$Mvar || !$Mname ) 
{ extout::error("You forgot to define either Mvar or Mname."); }
if ($filter eq "all" || $filter eq $currentsection ) {
  MODE::print_constructor($Mname,$Mvar,$par_list,$Mcomment);
}

#line 2144 "ext.nw"
              
  last switch;
}
if ($command =~ /^destruct/) {
  
#line 2323 "ext.nw"
if (!$destructor) {
  extout::warning("Mdestruct applies only to destructors");
}
if ($filter eq "all" || $filter eq $currentsection ){ 
  MODE::print_destructor($Mname,$Mcomment);
}

#line 2148 "ext.nw"
                
  last switch;
}


#line 1574 "ext.nw"
  extout::warning("I did not recognize command name M$command");

} # end of switch:

#line 1415 "ext.nw"
redo main_loop;
}


#line 1175 "ext.nw"
  MODE::post_process();
  # procedure depending on dynamic linkage via require module


