#line 3645 "ext.nw"
package extHTML;

# we glue local variables to the main scope
sub import {
  
#line 2921 "ext.nw"
  # export interface operations:
  *MODE::parse_parameters = *parse_parameters;
  *MODE::output_preamble =  *output_preamble;
  *MODE::new_manpage =      *new_manpage;
  *MODE::index_entry =      *index_entry;
  *MODE::set_vars =         *set_vars;
  *MODE::top_section =      *top_section;
  *MODE::generalization =   *generalization;
  *MODE::print_text =       *print_text;
  *MODE::print_constructor =*print_constructor;
  *MODE::print_destructor = *print_destructor;
  *MODE::print_function =   *print_function;
  *MODE::print_typemember = *print_typemember;
  *MODE::print_typedef =    *print_typedef;
  *MODE::print_event =      *print_event;
  *MODE::print_enum =       *print_enum;
  *MODE::post_process =     *post_process;
  *MODE::convert_text =     *convert_text;

#line 3650 "ext.nw"
  
#line 2943 "ext.nw"
  # import variables:
  *mode =           *main::mode;
  *filearg =        *main::filearg;
  *numbered =       *main::numbered;  
  *ack =            *main::ack;
  *constref =       *main::constref;  
  *partypes =       *main::partypes;
  *usesubscripts =  *main::usesubscripts;
  *size =           *main::size;
  *xdvi =           *main::xdvi;
  *warnings =       *main::warnings;
  *informational =  *main::informational;
  *debugging =      *main::debugging;
  *indexing =       *main::indexing;
  *indexentry =     *main::indexentry;
  *includefile =    *main::includefile;
  *filter =         *main::filter;
  *pid =            *main::pid;
  *latexruns =      *main::latexruns;
  *extractonly =    *main::extractonly;
  *delman =         *main::delman;
  *outfile =        *main::outfile;
  *dvioutfile =     *main::dvioutfile;
  *section =        *main::section;
  *nextwarning =    *main::nextwarning;
  *justset =        *main::justset;
  *noweaveoptions = *main::noweaveoptions;
  *print_title =     *main::print_title;
  *showsem =        *main::showsem;
  *dontshowinfo =   *main::dontshowinfo;
  *basename =       *main::basename;
  *ext =            *main::ext;
  *kind =           *main::kind;
  *INPUT =          *main::INPUT;
  *OUTPUT =         *main::OUTPUT;

  *title =          *main::title;
  *Mvar  =          *main::Mvar;
  *Mtype =          *main::Mtype;
  *Mname =          *main::Mname;
  *MIname =         *main::MIname;
  *Mcomment =       *main::Mcomment;
  *command =        *main::command;
  *includeline =    *main::includeline;
  *indexentry =     *main::indexentry;
  *cleanprototype = *main::cleanprototype;
  *template =       *main::template;

  *LEDAROOT =       *main::LEDAROOT;



#line 3652 "ext.nw"
  if ( $mode eq "HTMLext" ) { init_namerep(); }
}

#line 4821 "ext.nw"
sub parse_parameters {
  foreach my $path ("HOME","PWD") {
    if ($path eq "HOME") {
      $CFG = $ENV{$path} . "\/" . $mode . "\.cfg";
    } else {
      $CFG = "$mode.cfg";
    }
    if (-e $CFG) {
      open (CFG); 
      extout::info("Reading $CFG");
      while (<CFG>) {
        if (/^(\w+)=(.*)$/) 
        { eval "\$main::$1 = \$2"; } 
      }
      close (CFG);
    }
  }
  eval "\$main::$1 = \$2" while $ARGV[0] =~ /(\w+)=(.*)/ && shift(@ARGV); 
}









#line 3672 "ext.nw"
sub output_preamble {
  local($preambletext)="";
  if ($mode eq "HTMLman" ) { 
    $preambletext .= "\\documentclass[a4paper]{article}\n";
    $preambletext .= "\\usepackage{html}\n";
    $preambletext .= "\\input $LEDAROOT/Manual/tex/MANUAL.mac\n\n";
    $preambletext .= "\\begin{document}";
    extout::unit($preambletext);
  }
  inittabular($outfile);
}

sub new_manpage {
  closetabular();
  my $Mtype1 = $Mtype; $Mtype1 =~ s/_/\\_/g;
  extout::unit("\\$section \{ $title \( $Mtype1 )}\n".
               "\\label\{$title\}\n\\label\{$Mtype\}");
}


sub index_entry { 
  return if ($indexing ne "yes");
  my ($entry,$scope) = @_;
  $entry =~ s/\&/\\&/g;
  if ($scope eq "local")  
  { $indexentry .= "\\index\{$entry\!$MIname\}\n"; }
  else { $indexentry .= "\\index\{$entry\}\n"; }
}

sub set_vars {}

sub generalization {
  @params = @_;
  my $gennum = 4*($#params+2);
  my $width = length($Mname); 
  foreach $i (0 .. $#params) {
    $width = length($params[$i]) if ( length($params[$i]) > $width );
  }
  $width += 20;
  local $generalization = "\\begin{makeimage}\n\\setlength{\\unitlength}{1ex}\n";
  $generalization .= "\\adjustclassframe{$width}{$gennum}\n";
  $generalization .= "\\begin{picture}(60,$gennum)\n";
  foreach $i (0 .. ($#params-1) ) {
    my $superclass = $params[$i];
    my $genmode;
    if ( $superclass =~ /(.*)\^$/ )
    { $superclass = $1; $genmode = "\\genarrow{2}\\indentframe"; }
    else
    { $genmode = "\\genarrow{4}";}
    $superclass = convert_code($superclass);
    $generalization .= "\\classframe{$superclass}$genmode\n"; 
  }
  $superclass = $params[$#params];
  $generalization .= "\\classframe{".convert_code($superclass).
                     "}\\genarrow{2}\\indentframe\n"; 
  $generalization .= "\\classframe{".convert_code($Mname)."}\n";
  $generalization .= "\\end{picture}\n";
  $generalization .= "\\end{makeimage}\n";
  return $generalization;
}

sub convert_code { 
  local($Ctext) = @_;
  $Ctext =~ s/(\w+)/\\textbf{$1}/g;
  $Ctext =~ s/_/\\nspaceunderscore\\_/g;
  $Ctext =~ s/</\\Ltemplateless/g;
  $Ctext =~ s/>/\\Ltemplategreater/g;
  $Ctext =~ s/ /\\ /g;
  $Ctext =~ s/::/\\DP /g;
  return $Ctext;
}

sub top_section {
  closetabular();
  my($header,$semantics);
  $header = "\\textbf\{\\large ".ucfirst($command)."\}";
  $semantics = convert_text($Mcomment);
  if ($includeline) 
  { $includeline =~ s/\_/\\\_/g;
    $semantics .= "\n".$includeline; $includeline=""; }
  extout::unit($header);
  extout::unit($semantics);
}

sub print_text {
  return if ($command eq "preamble" && $mode eq "HTMLdoc") ;
  local($text) = @_;
  closetabular();
  extout::unit(convert_text($text));
}

sub print_constructor {
  extout::debug("print_constructor:".join(':',@_));
  my ($type,$var,$args,$semantics) = @_;
  my $signature = $var;
  $signature .= "($args)" if ($args);
  my $shortsig = (length($signature) <= 25);
  $type = insert_URLs_and_quote($type);
  $signature = insert_URLs_and_quote($signature);
  $semantics = convert_text($semantics);
  opentabular(3);
  my $templatearg = insert_URLs_and_quote($template);  
  $templatearg = 'template $<$'.$templatearg.'$>$';
  print_table_row(multicol(3,$templatearg)) if ( $template );
  if ( $shortsig ) 
  { print_table_row($type,$signature,$semantics); }
  else {
    print_table_row($type,multicol(2,$signature));
    print_table_row("","",$semantics);
  }
}

sub print_destructor {
  extout::debug("print_destructor:".join(':',@_));
  my ($type,$semantics) = @_;
  my $signature = "~".$type."()";
  opentabular(3);
  $semantics = convert_text($semantics);
  print_table_row($signature,convert_text($semantics));
}

#line 3804 "ext.nw"
sub print_function {
  extout::debug("print_function:".join(':',@_));
  my($type, $signature, $semantics, $prefix, $funcname, $postfix) = @_;
  if ($funcname) 
  { $signature = $prefix . $funcname . "(". $postfix. ")"; }
  my $shortsig = ( length($signature) <= 25 );
  $type = insert_URLs_and_quote($type);
  $signature = insert_URLs_and_quote($signature);
  $semantics = convert_text($semantics);
  opentabular(3);
  my $templatearg = insert_URLs_and_quote($template);  
  $templatearg = 'template $<$'.$templatearg.'$>$';
  print_table_row(multicol(3,$templatearg)) if ( $template );
  if ( $shortsig ) 
  { print_table_row($type, $signature, $semantics); }
  else {
    print_table_row($type, multicol(2,$signature));
    print_table_row("","",$semantics);
  }
}

sub print_typemember {
  my ($scope,$type,$semantics) = @_;
  my $signature = $scope."::".$type;
  my $shortsig = ( length($signature) <= 50 );
  $signature = insert_URLs_and_quote($signature);
  $semantics = convert_text($semantics);
  opentabular(2);
  my $templatearg = insert_URLs_and_quote($template); 
  $templatearg = 'template $<$'.$templatearg.'$>$';
  print_table_row(multicol(2,$templatearg)) if ( $template );
  if ( $shortsig ) 
  { print_table_row($signature, $semantics); }
  else {
    print_table_row(multicol(2,$signature));
    print_table_row("",$semantics);
  }
}

sub print_typedef {
  my ($fromtype,$newtype,$semantics) = @_;
  my $signature = "typedef $fromtype $newtype";
  my $shortsig = ( length($signature) <= 50 );
  $signature = insert_URLs_and_quote($signature);
  $semantics = convert_text($semantics);
  opentabular(2);
  if ( $shortsig ) 
  { print_table_row($signature, $semantics); }
  else {
    print_table_row(extHTML::multicol(2,$signature));
    print_table_row("",$semantics);
  }
}

sub print_event {
  my ($scope,$eventname,$par_list,$semantics) = @_;
  my $signature = $scope.$eventname."(".$par_list.")";;
  my $shortsig = ( length($signature) <= 50 );
  $signature = insert_URLs_and_quote($signature);
  $semantics = convert_text($semantics);
  extHTML::opentabular(2);
  if ( $shortsig ) 
  { print_table_row($signature, $semantics); }
  else {
    print_table_row(multicol(2,$signature));
    print_table_row("",$semantics);
  }
}

sub print_enum {
  my ($type,$enumconsts,$semantics) = @_;
  my $signature = $Mname."::".$type." { ".$enumconsts." }";
  my $shortsig = (length($signature) <= 25);
  $signature = insert_URLs_and_quote($signature);
  $semantics = convert_text($semantics);
  opentabular(2);
  if ($shortsig) 
  { extHTML::print_table_row($signature, $semantics); }
  else { 
    extHTML::print_table_row(multicol(2,$signature)); 
    extHTML::print_table_row("",$semantics);
  }
}

sub post_process {

if ($mode eq "HTMLext") {
  closetabular(); 
  close OUTPUT;
}
elsif ($mode eq "HTMLman") { 
  closetabular(); 
  extout::unit("\\end{document}");
  close OUTPUT;
  local($tmpname) = "/tmp/$pid-ext.tex";
  exit if ($outfile ne $tmpname);

  extout::info("Preparing manual page with LaTeX...");
  @CP = ("cp","/tmp/$pid-ext.tex","$basename.tex"); 
  system(@CP) == 0 or die "system @CP failed: $?"; 

  @LA = ("latex","$basename","$dontshowinfo");
  if ($latexruns == 1) 
  { system(@LA) == 0 or die "system @LA failed: $?"; }
  if ($latexruns == 2) 
  { system(@LA) == 0 or die "system @LA failed: $?"; }
  if ($xdvi eq "yes") {
    extout::info("Starting xdvi previewer...");
    system("xdvi -s 0 $basename $dontshowinfo");
  }

  system("rm -rf /tmp/$pid-ext.*");
  
  system("latex2html -split 0 -local_icons -html_version 3.2,math ".
         "-no_math $basename");
  local($owd) = $ENV{"PWD"};
  system("netscape -raise -remote \"openURL($owd/$basename/$basename.html)\" ");
 
} else { # HTMLdoc
  close(OUTPUT);
  my $addmanpages = "";
  my $defines_own = 0;
  foreach my $key (keys(%main::existing_outfiles)) {
    if ( ! ($key =~ /ext\.tex/) ) { $addmanpages .= $key." "; }
    if ( tabularisopen($key) ) {
      $outfile = $key;
      open OUTPUT, ">>$outfile"; 
      closetabular();
      close OUTPUT;
    }
    $defines_own = 1 if ( $key eq "$basename.man" );
  }
  system("mv /tmp/$pid-ext.tex $basename.man") if (!$defines_own);
  extout::info("  Created $basename.man $addmanpages");
  if ($extractonly eq "yes") { exit; }
  extout::info("Removing the manual comments (except for Mpreamble)".
              " from $INPUT...");
  my $tempfile = "/tmp/$pid-$basename";
  if ($delman eq "refined") {
    system("ldel $INPUT $tempfile.$ext refined HTML");
    extout::info("  Created $tempfile.$ext through refined ldel");
  }
  if ($delman eq "yes" || $kind eq "Cweb") {
    if ($delman eq "no") 
    { extout::info("Option delman=no not implemented for Cweb");}
    system("ldel $INPUT $tempfile.$ext");
    extout::info("  Created $tempfile.$ext through ldel");
  }
  if ($delman eq "no" && $kind ne "Cweb") {
    system("cp  $INPUT $tempfile.$ext"); 
    extout::info("  Created $tempfile.$ext by copying");
  }

  extout::info("Calling $kind weave on $tempfile.$ext...");
  if ($mode eq "HTMLdoc") {
    system( "lweave $tempfile.$ext HTML");
    system( "mv $tempfile.tex $basename.tex");
  }

  extout::info("  Created $basename.tex");
  if ($latexruns == 0) { exit; }
  extout::info("Calling latex...");
  system("latex $basename.tex $dontshowinfo");
  if ($latexruns == 2) {
    system("latex $basename.tex $dontshowinfo");
  }
  system("rm -f /tmp/$pid*");
  
}
}


#line 3985 "ext.nw"
%namerep;

sub insert_URLs_and_quote {
  local($code) = @_; 
  local(@fields) = split(/([\ \(\)\<\>\{\}\,\&\.\:\[\]\=\*\'])/,$code);
  foreach $i (0 .. $#fields) {
    if ($fields[$i] =~ /^\w*$/) { # alphanumeric string
      local($key) = $fields[$i];
      local($label) = $namerep{$key};
      if ($label ne "" && $key ne $Mtype) {
        $key =~ s/_/\\_/gi;
        $fields[$i] = "\\htmlref\{$key\}\{$label\}"; 
      } else {
        $key =~ s/_/\\_/gi;
        $fields[$i] = $key;
      }
    }    
    else { # quote some characters
      $fields[$i] = "\\$fields[$i]" if ($fields[$i] =~ /&/); 
      $fields[$i] = "\$<\$" if ($fields[$i] =~ /</); 
      $fields[$i] = "\$>\$" if ($fields[$i] =~ />/); 
      $fields[$i] = "\\}" if ($fields[$i] eq "}"); 
      $fields[$i] = "\\{" if ($fields[$i] eq "{"); 
      $fields[$i] =~ s/\\n/\$\\backslash\\mathtt\{n\}\$/g;  # takes care of \n
    }
  }
  $code = join("",@fields);
  return $code;
}

#line 4018 "ext.nw"
sub init_namerep {
  $INPUT = "namereplacementtable";
  open (INPUT)  || die "Error: Can't find input file $INPUT: $!\n";
  while (<INPUT>) {
    chop;
    if (/^\s*$/) { next; } # skip empty lines
    s/\t/ /g;  # subsitute a tab by a blank; some people write xxx\tyyy
    if (/^\s*(\S+)\s+(\S+)\s*$/) {
      $LEDAname = $1;
      $URL = $2;
    }
    else {
      /^\s*(\S+)\s*$/;
      $LEDAname = $1;
      $URL = $1;
    }
    $namerep{"$LEDAname"} = $URL;
  }
  close (INPUT);
}


#line 4347 "ext.nw"
%TabularOpen;

sub inittabular {
  local($outfile) = @_;
  $TabularOpen{$outfile} = "no";
}

sub tabularisopen {
  local($outfile) = @_;
  return 1 if ($TabularOpen{$outfile} eq "yes");
  return 0;
}

sub opentabular {
 local($cols) = @_;  # this is either two, three or four
 if ($TabularOpen{$outfile} eq "no") {
   if ($cols == 2) {
     print OUTPUT "\\begin{tabular}{ll}\n"; }
   elsif ($cols == 3) {
     print OUTPUT "\\begin{tabular}{lll}\n"; }
   elsif ($cols == 4) {
     print OUTPUT "\\begin{tabular}{llll}\n"; }
   else {
     extout::error("open_tabulator expects argument 2 or 3"); 
   }
   $TabularOpen{$outfile} = "yes";
 }
}

sub closetabular {
  if ($mode =~ /HTML/ && 
      $TabularOpen{$outfile} eq "yes") {
    print OUTPUT "\\end{tabular}\n\n";
    $TabularOpen{$outfile} = "no";
  }
}

sub print_table_row {
  my @params = @_;
  my $row = "";
  #if ($indexentry ne "") { chop $indexentry; }
  foreach $i (0 .. $#params) {
    if ($params[$i] eq "") { $row.="\\ "; }
    else { $row.=$params[$i]; }
    if ($i == 0) { $row .= $indexentry; }
    if ($i == $#params) { $row .= " \\\\";} 
    else { $row .= " \& "; }
    # was { $row .= "\n" .$indexentry . " \\\\";}
  }
  if ($indexentry ne "") {  $indexentry=""; } 
  print OUTPUT $row , "\n";
}

#the following is only used in HTML mode:
sub multicol {
  my $colnum = shift(@_);
  my $content = shift(@_);
  my $line = "\\multicolumn\{$colnum\}\{l\}\{ ".$content."\}";
  $line;
}


#line 4046 "ext.nw"
sub convert_text {
 local($text) = @_; # read argument into local variable
 if ( $text eq "" ) { return ""; }
 extout::debug("HTML::convert $text\nTEXTEND HTML::convert");
 $text = extbasic::convert_inline_code($text,\&convert_M_for_HTML,
         \&extbasic::convert_T);
 $text = &convert_math_for_HTML($text);
 $text = &deal_with_refs($text);
 return $text;
}

#line 4060 "ext.nw"
sub convert_M_for_HTML {
 local($Ctext) = @_; # read argument into local variable
 extout::debug("convert_M_for_HTML $Ctext\n");
 # we first work on identifiers 
 if ($Ctext eq "") { return ""; }
 local($prefix,$suffix,$ident) = ("",$Ctext,"");

identloop:
  while ($suffix =~ /^(\W*)(\w.*)$/) {
    $prefix .= $1;
    $suffix = $2;
    if ( $suffix =~ /^(\w+)(\W.*)$/ ) {
      $ident = $1;
      $suffix = $2;
    } else { 
      $ident = $suffix;
      $suffix = "";
    }
    if ($prefix =~ /\\$/) { # For things like \n
      if ($ident ne "n") 
      { extout::warning("Huch:\\ in  quoted code:\n".$Ctext); }
      $prefix .= "L" . $ident; next identloop;
    }
    if ($ident =~ /^[0-9]*$/ ) # just a number
    { $prefix .= $ident; next identloop; }
    if (($usesubscripts eq "yes") && ($ident =~ /^([a-zA-Z])([0-9]+)$/)) 
    { $ident = $1 ."\\HTML\{SUB\}\{$2\}"; }
    else 
    { $ident = "\\HTML\{I\}\{$ident\}"; }
    $prefix .= $ident;
  }

  # All blanks that are still in Ctext are to be preserved and
  # are hence quoted.
  $Ctext = $prefix . $suffix;
  $Ctext =~ s/&/\\&/g;
  $Ctext =~ s/_/\\_/g;
  $Ctext =~ s/\%/\\%/g; 
  $Ctext =~ s/\#/\\#/g; 
  $Ctext =~ s/~/\$\\sim\$/g;
  $Ctext =~ s/\^/\$\\circumflexop\$/g;
  $Ctext;
}

#line 4197 "ext.nw"
sub protect_newlines {
 local ($latexenv,$text) = @_;
 if ( $text =~ /\\begin\s*\{(\s*$latexenv[\s\*]*)\}/s ) {
   extout::debug("protect_newlines $latexenv");
   local($unprocessed) = $text;
   local($transformed) = "";
   while ( $unprocessed =~ s/(.*?)(\\begin\s*\{\s*$latexenv[\s\*]*\})(.*?)(\\end\{\s*$latexenv[\s\*]*\})//s ) {
     local($before,$bk,$between,$ek,$unprocessed) = ($1,$2,$3,$4,$');
     $between =~ s/\\\\/LEDAARRAYNL/gs;
     $transformed .= $before . $bk . $between . $ek;
     extout::debug("protect_newlines $bk\n\n$between\n\n$ek\n\n");
   }
   $text = $transformed . $unprocessed;
 }
 return $text;
}

#line 4217 "ext.nw"
sub protect_math_environment {
 local ($latexenv,$text) = @_;
 if ( $text =~ /\\begin\{([^\}]*$latexenv[^\}]*)\}/s ) {
   local($lenv) = $1; $lenv =~ s/ //g;
   local($unprocessed) = $text;
   local($transformed) = "";
   while ( $unprocessed =~ s/(.*?)(\\begin\{[^\}]*$lenv[^\}]*\})(.*?)(\\end\{[^\}]*$lenv[^\}]*\})//s ) {
     local($before,$bk,$between,$ek,$unprocessed) = ($1,$2,$3,$4,$');
     $transformed .= $before .'\HTML[ALIGN=CENTER]{P}{\begin{makeimage}' 
                             . $bk . $between . $ek 
                             .'\end{makeimage}}';
   }
   $text = $transformed . $unprocessed;
 }
 return $text;
}

#line 4237 "ext.nw"
sub protect_displaymath {
 # we convert $$ and \[ into normal math
 local ($text) = @_;
 if ( $text =~ /\$\$/s ) {
   local($unprocessed) = $text;
   local($transformed) = "";
   while ( $unprocessed =~ s/(.*?)(\$\$)(.*?)(\$\$)//s ) {
     local($before,$bk,$between,$ek,$unprocessed) = ($1,$2,$3,$4,$');
     $transformed .= $before . '\HTML{P}{}\HTML{CENTER}{' . 
         "\\begin{makeimage}\n\$" . $between . "\$\n\\end{makeimage}" .
         '}';
   }
   $text = $transformed . $unprocessed;
 }
 if ( $text =~ /\\\[/s ) {
   local($unprocessed) = $text;
   local($transformed) = "";
   while ( $unprocessed =~ s/(.*?)(\\\[)(.*?)(\\\])//s ) {
     local($before,$bk,$between,$ek,$unprocessed) = ($1,$2,$3,$4,$');
     $transformed .= $before . '\HTML{P}{}\HTML{CENTER}{' . 
         "\\begin{makeimage}\n\$" . $between . "\$\n\\end{makeimage}" .
         '}';
   }
   $text = $transformed . $unprocessed;
 }
 return $text;
}

#line 4267 "ext.nw"
sub is_complicated_math {
  local($math) = @_;
  if ($math=~ /array/   ||
      $math=~ /\\frac/s )
  { return 1; }
  return 0;
}



#line 4112 "ext.nw"
sub convert_math_for_HTML {
 local($Ctext) = @_; 
 extout::debug("convert_math_for_HTML1 $Ctext\nTEXTEND convert_math_for_HTML1");
  $Ctext =~ s/\n/LEDANEWLINE/gi;
  $Ctext =~ s/\\\\\[.*?\]/\\\\/gi;

  $Ctext = &protect_newlines("array",$Ctext);
  $Ctext = &protect_newlines("eqnarray",$Ctext);
  $Ctext = &protect_newlines("tabular",$Ctext);
  $Ctext = &protect_newlines("cases",$Ctext);
  $Ctext = &protect_newlines("matrix",$Ctext);

  $Ctext = &protect_math_environment("eqnarray",$Ctext);
  $Ctext = &protect_math_environment("displaymath",$Ctext);
  $Ctext = &protect_displaymath($Ctext);

  local($unprocessed) = $Ctext;
  $Ctext = "";
 
  while ( $unprocessed =~ /(.*?)\$([^\$]*)\$/s ) {
     # text contains two or more dollars
     $Ctext .= &quote_special_symbols_in_text_mode($1);
     local($math) = $2;
     $unprocessed = $';
     if ( &is_complicated_math($math) )
     { $Ctext .= "\\begin{makeimage}\$$math\$\\end{makeimage}"; } 
     else
     { $Ctext .= "\$".&simplify_math_for_HTML($math)."\$"; }
  }
  $Ctext .= &quote_special_symbols_in_text_mode($unprocessed);
  $Ctext =~ s/LEDANEWLINE/\n/gi;
  $Ctext =~ s/LEDAARRAYNL/\\\\/gi;
  extout::debug("convert_math_for_HTML2 RESULT:\n $Ctext\nENDRESULT convert_math_for_HTML2");
  return $Ctext;
}

#line 4158 "ext.nw"
sub deal_with_refs{
 local($text) = @_; # read argument into local variable
 while ($text =~ /\\ref\{([^\}]*)\}/)
 { local($key) = $1;
   $key1 = $key; $key1 =~ s/_/\\_/g;
   $text =~ s/\\ref\{$key\}/\\hyperref{$key1}{}{}{$key}/;
   #$text =~ s/\\ref\{$key\}/\\htmlref{$key1}{$key}/;
 }
 return $text;
}




#line 4177 "ext.nw"
sub quote_special_symbols_in_text_mode {
  local($text) = @_; # read argument into local variable
  # it remains to quote all special characters.
  $text =~ s/\\\\/\\HTML\{BR\}\{\}/g;
  $text =~ s/\\headerline/\\textbf/g;
  $text =~ s/\\precond/\\textbf\{Precondition\}/sg;
  $text =~ s/\\setopdims\{.*?\}\{.*?\}//sg;
  return $text;
}


#line 4280 "ext.nw"
sub replace_simple_latex_commands {
  local($math) = @_;
  $math =~ s/\\_/{\\_}/gs;
  $math =~ s/\\Lvert(\W|$)/|$1/gs;
  $math =~ s/\\cdot(\W|$)/*$1/gs;
  $math =~ s/\\leq(\W|$)/<=$1/gs;
  $math =~ s/\\le(\W|$)/<=$1/gs;
  $math =~ s/\\geq(\W|$)/>=$1/gs;
  $math =~ s/\\ge(\W|$)/>=$1/gs;
  $math =~ s/\\neq(\W|$)/!=$1/gs;
  $math =~ s/\\ne(\W|$)/!=$1/gs;
  $math =~ s/\\</</gs;
  $math =~ s/\\>/>/gs;
  $math =~ s/\\range\{(.*?)\}\{(.*?)\}/\[$1\.\.$2\]/gs;
  return $math;
}

sub step_over_braces {
  local($text) = @_;
  local($braced,$bracelevel) = ("",0);
  LOOP: while ( $text ) { 
    if ( $text =~ /([^\{\}]*)([\{\}])/s ) {
      $braced .= $`.$1.$2;
      $text = $';
      if ( $2 eq '{' ) { $bracelevel++; }
      if ( $2 eq '}' ) { $bracelevel--; }
    }
    last LOOP if $bracelevel==0;
  }
  if ($bacelevel != 0) { extout::error("error in brace stepping"); }
  $braced =~ s/\{(.*)\}/$1/s; # remove outer backets
  return ($braced,$text);
}

sub simplify_math_for_HTML {
   local($math) = @_; 
   extout::debug("simplify_math_for_HTML $math\n");
   local($cleaned,$braced)=("","");
   while ( $math =~ /\\Labs/s ) {
     $cleaned .= $`;
     ($braced,$math) = &step_over_braces($');
     $cleaned .= "\\|$braced\\|";
   }
   $math = $cleaned.$math; $cleaned="";
   while ( $math =~ /\\Litem/s ) {
     $cleaned .= $`;
     ($braced,$math) = &step_over_braces($');
     $cleaned .= "<$braced>";
   }
   $math = $cleaned.$math; $cleaned="";
   # now we exclude the italic styled text from the math
   # by transporting them outside the math $..$ environment
   while ( $math =~ /\\HTML\{I\}/s ) {
     $cleaned .= $`;
     ($braced,$math) = &step_over_braces($');
     $cleaned .= "\\mathit\{$braced\}";
   }
   $math = $cleaned.$math; $cleaned="";
   $math = &replace_simple_latex_commands($math);
   extout::debug("simplify_math_for_HTML $math\n");
   $math;
}


#line 3660 "ext.nw"
1;

