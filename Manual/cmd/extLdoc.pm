#line 2997 "ext.nw"
package extLdoc;

sub import {
  
#line 2922 "ext.nw"
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

#line 3001 "ext.nw"
  
#line 2944 "ext.nw"
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


#line 3002 "ext.nw"
}

#line 4822 "ext.nw"
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









#line 3024 "ext.nw"
sub output_preamble {
  local($preambletext)="";
  if ($mode eq "Lman") {
    $preambletext .= "\\documentclass\[".$size."pt,a4paper\]\{article\}\n\n"; 
    $preambletext .= "\\input $LEDAROOT/Manual/tex/MANUAL.pagesize\n";
    $preambletext .= "\\input $LEDAROOT/Manual/tex/MANUAL.mac\n\n";
    $preambletext .= "\\begin\{document\}\n";
    $preambletext .= "\\begin\{manual\}";                        
  } else { # Ldoc, Mkman, lextract
    $preambletext = "\\begin{manual}";
  }
  extout::unit($preambletext);
}

sub new_manpage {
  extout::unit("\\resetmancounter");
  if ($print_title eq "yes") {
    # we print \section*{title (type')}\label{type}\n 
    # where type' is obtained from type by quoting underscores.
    # if numbered is true we supress the star
    my $Mtype1 = $Mtype; $Mtype1 =~ s/_/\\_/g;
    if ($numbered eq "yes" ) {$star = "";} else {$star = "*";}
    extout::unit("\\$section$star\{$title ( $Mtype1 )\}\n".
                 "\\label\{$title\}\n\\label\{$Mtype\}");
  }  
}

#line 3054 "ext.nw"
sub index_entry {
  return if ($indexing ne "yes");
  my ($entry,$scope) = @_;
  $entry = shorten($entry);
  if ($scope eq "local")  
  { $indexentry .= "\\index\{".convert_R($entry)."\!".
                               convert_MI($MIname)."\}\n"; }
  elsif ($scope eq "global") 
  { $indexentry .= "\\index\{".convert_R($entry)."\}\n"; }
  else # class symbol
  { $indexentry .= "\\index\{".convert_MI($entry)."\}\n"; }
}

sub shorten {
  local($proto) = @_;
  if ($proto =~ /(.+?)\((.*)\)/ ) {
    my $name = $1;
    my $args = $2;
    if (length($name) > 20) 
    { $name = substr($name,0,17) . "..."; }
    if ($args) { $proto="$name(...)"; } 
    else { $proto="$name()"; }
  }  
  return $proto;
}


#line 3085 "ext.nw"
sub set_vars {
  if (@_[0] =~ /^[a-zA-Z]/) { shift; }
  if (@_[1] ne "") { # two numbers a b
    extout::unit("\\setlength{\\typewidth}{@_[0]}".
                 "\\setlength{\\callwidth}{@_[1]}".
                 "\\computewidths");
  } elsif (@_[0] ne "") {
    extout::unit("\\setlength{\\declwidth}{@_[0]}".
                 "\\computewidths");
  }
}

#line 3100 "ext.nw"
sub print_text {
  return if ($command eq "preamble" && $mode eq "Ldoc") ;
  local($text) = @_;
  extout::unit(convert_text($text));
}

#line 3110 "ext.nw"
sub generalization {
  @params = @_;
  my $gennum = 4*($#params+2);
  my $width = length($Mname); 
  foreach $i (0 .. $#params) {
    $width = length($params[$i]) if ( length($params[$i]) > $width );
  }
  $width += 20;
  local $generalization = "\\setlength{\\unitlength}{1ex}\n";
  $generalization .= "\\adjustclassframe{$width}{$gennum}\n";
  $generalization .= "\\begin{picture}(60,$gennum)\n";
  foreach $i (0 .. ($#params-1) ) {
    my $superclass = $params[$i];
    my $genmode;
    if ( $superclass =~ /(.*)\^$/ )
    { $superclass = $1; $genmode = "\\genarrow{2}\\indentframe"; }
    else
    { $genmode = "\\genarrow{4}";}
    $superclass = convert_M($superclass);
    $generalization .= "\\classframe{$superclass}$genmode\n"; 
  }
  $superclass = $params[$#params];
  $generalization .= "\\classframe{".convert_M($superclass).
                     "}\\genarrow{2}\\indentframe\n"; 
  $generalization .= "\\classframe{".convert_M($Mname)."}\n";
  $generalization .= "\\end{picture}\n";
  return $generalization;
}



#line 3145 "ext.nw"
sub top_section {
  my($header,$semantics);
  $header = "\\mansection\{" . ucfirst($command) . "\}";
  $semantics = convert_text($Mcomment) if ($Mcomment);
  if ($includeline) 
  { $semantics .= "\n".convert_M($includeline); $includeline=""; }
  extout::unit($header);
  extout::unit($semantics);
}

#line 3157 "ext.nw"
sub print_constructor {
  extout::debug("print_constructor:".join(':',@_));
  my ($type,$var,$args,$semantics) = @_;
  $type = convert_M($type);
  $var = convert_M($var);
  $args = convert_M($args);
  $semantics = convert_text($semantics);
  my $templatearg = '['.convert_M($template).']' if ($template);
  extout::unit("\\create$templatearg\{$type\}\{$var\}".
               "\{$args}\n\{$semantics\}"); 
}

sub print_destructor {
  extout::debug("print_destructor:".join(':',@_));
  my ($type,$semantics) = @_;
  $type = convert_M($type);
  $semantics = convert_text($semantics);
  extout::unit("\\destruct\{$type\}\n\{$semantics\}");
}


#line 3213 "ext.nw"
sub print_function {
  extout::debug("print_function:".join(':',@_));
  my($type, $signature, $semantics, $prefix, $funcname, $postfix) = @_;
  $type = convert_M($type);
  my $paralist = "";
  if ($funcname) {
    $funcname = convert_M($prefix) . &convert_R($funcname);
    $paralist = convert_M($postfix);
    if ($paralist eq "") { $paralist = "\$\\,\$"; } # small space
  } else {
    $signature = convert_M($signature);
  }
  $semantics = convert_text($semantics);
  my $templatearg = '['.convert_M($template).']' if ($template);
  if ( $type eq "" && $funcname eq "" && $signature =~ /\(\s*\)/ ) {
    extout::warning("I encountered an empty prototype ".
          $signature . " and will not produce an entry.");
    return;
  }
  if ($funcname) {
      if (! ($command =~ /^opl?/ || $command =~ /^funcl?/ || 
             $command =~ /^funobj/ ||
             $command =~ /^staticl?/ || $command =~ /^conversion?/) )
      { extout::warning("wrong case for print_function"); }
    extout::unit("\\function$templatearg\{$type\}\n".
                 "\{$funcname\}\n{$paralist\}\n\{$semantics\}");
  } else { 
    extout::unit("\\operator$templatearg\{$type\}\n".
                 "\{$signature\}\n\{$semantics\}"); 
  }
}

#line 3251 "ext.nw"
sub print_typemember {
  my ($scope,$type,$semantics) = @_;
  my $signature = $scope."::".$type;
  $signature = convert_M($signature);
  $semantics = convert_text($semantics);
  $templatearg = '['.convert_M($template).']' if ($template); 
  extout::unit("\\typemember$templatearg\{$signature\}\n".
               "\{$semantics\}"); 
}

sub print_typedef {
  my ($fromtype,$newtype,$semantics) = @_;
  my $signature = "typedef $fromtype $newtype";
  $fromtype = convert_M($fromtype);
  $newtype = convert_M($newtype);
  $semantics = convert_text($semantics);
  extout::unit("\\typedef\{$fromtype\}\{$newtype\}\n\{$semantics\}");
}

sub print_event {
  my ($scope,$eventname,$par_list,$semantics) = @_;
  my $signature = $scope.$eventname."(".$par_list.")";;
  $signature = &convert_M($signature);
  $semantics = &convert_text($semantics);
  extout::unit("\\event\{$signature\}\n\{$semantics\}");
}

sub print_enum {
  my ($type,$enumconsts,$semantics) = @_;
  my $signature = $Mname."::".$type;
  $signature = convert_M($signature);
  $enumconsts = convert_R($enumconsts);
  $semantics = convert_text($semantics);
  extout::unit("\\enum\{$signature\}\n\{$enumconsts\}\n\{$semantics\}");
}

#line 3291 "ext.nw"
sub post_process {
if ($mode eq "Mkman" || $mode eq "lextract") {
  extout::unit("\\end{manual}");
  close OUTPUT;
  system("mv $outfile $basename.man");  # move manpage
}
elsif ($mode eq "Lman") {
  extout::unit("\\end\{manual\}\n\\end{document}");          
  close OUTPUT;
  local($owd) = $ENV{"PWD"};
  if ($latexruns == 0) { exit; }
  chdir ("/tmp");
   
  extout::info("Preparing manual page with LaTeX...");

  if ($outfile ne "/tmp/$pid-ext.tex") {
    if ($outfile =~ /\//) # absolute path name 
    { system("cp $outfile /tmp/$pid-ext.tex"); }
    else 
    { system("cp $owd/$outfile /tmp/$pid-ext.tex"); }
  }

  system ("latex -interaction=nonstopmode /tmp/$pid-ext.tex $dontshowinfo");
  if ($latexruns == 2) {
    system ("latex -interaction=nonstopmode /tmp/$pid-ext.tex $dontshowinfo");
  }

  if ($xdvi eq "yes") {

#   extout::info("Starting xdvi previewer...");
#   system ("xdvi -s 0 /tmp/$pid-ext.dvi $dontshowinfo");

    system ("dvipdfm -o /tmp/$pid-ext.pdf /tmp/$pid-ext.dvi");
    extout::info("\nOpen pdf file ...");
    system ("xdg-open /tmp/$pid-ext.pdf");

  }
  elsif ($dvioutfile ne "") {
    extout::info("Copying dvi file into file $dvioutfile ".
                "in working directory...");
    system("cp /tmp/$pid-ext.dvi $dvioutfile");
  }
  else {
    extout::info("Copying dvi file into file $basename.dvi ".
                "in working directory...");
    system("cp /tmp/$pid-ext.dvi $owd/$basename.dvi");
  }
  system("rm -f /tmp/$pid-ext.*");
  chdir ("$owd");

}
else { # Ldoc..

  close(OUTPUT);
  my $addmanpages = "";
  my $defines_own = 0;
  foreach my $key (keys(%main::existing_outfiles)) {
    if ( ! ($key =~ /ext\.tex/) ) { $addmanpages .= $key." "; }
    # print STDERR "postprocessing $key\n";
    open OUTPUT, ">>$key"; 
    extout::unit("\\end{manual}\n"); 
    close OUTPUT;
    $defines_own = 1 if ( $key eq "$basename.man" );
  }
  system("mv /tmp/$pid-ext.tex $basename.man") if (!$defines_own);
  extout::info("  Created $basename.man $addmanpages");
  if ($extractonly eq "yes") { exit; }
  extout::info("Removing the manual comments (except for Mpreamble)".
              " from $INPUT...");
  my $tempfile = "/tmp/$pid-$basename";
  if ($delman eq "refined") {
    system("ldel $INPUT $tempfile.$ext refined");
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
  if ($kind eq "Cweb") {
    system( "cweave $tempfile.$ext");
    system( "mv $pid-$basename.scn $basename.scn");
    system( "mv $pid-$basename.idx $basename.idx");
    system( "mv $pid-$basename.tex $basename.tex");
  }
  elsif ($kind eq "Lweb") {
    system( "lweave $tempfile $noweaveoptions");
    system( "mv $tempfile.tex $basename.tex");
    system( "mv $tempfile.nw $basename.nw");
  }
  elsif ($kind eq "noweb") {
    system("noweave -delay $noweaveoptions $tempfile.nw".
           " > $basename.tex");
  }

  extout::info("  Created $basename.tex");
  if ($latexruns == 0) { exit; }
  extout::info("Calling latex...");
  system("latex $basename.tex $dontshowinfo");
  if ($latexruns == 2) {
    system("latex $basename.tex $dontshowinfo");
  }
  if ($xdvi eq "yes") {
    extout::info("Starting xdvi previewer...");
    system("xdvi -s 0 $basename.dvi $dontshowinfo");
  }
  system("rm -f /tmp/$pid*");
} # Ldoc
} # post_process


#line 3416 "ext.nw"
# we expect that placeholder substitution already took place
sub convert_text {
  local($text) = @_; # read argument into local variable
  $text = extbasic::convert_inline_code($text,\&convert_M,
          \&extbasic::convert_T); 
  if ($kind eq "Cweb") {
    
#line 3431 "ext.nw"
local($i, $remline, $outline, $lines , $word); 
@lines = split(/^/,$text);  # split at new lines
$text = "";
foreach $i (0 .. $#lines) {
  if (length($lines[$i]) > 80) {
     $outline = "";
     $remline = $lines[$i];
     while ($remline ) {
       if ($remline =~ /[\w\,] /) 
       { $word = $` . $&; $remline = $'; }
       else 
       { $word = $remline; $remline = ""; }
       # a word charachter or komma followed by a blank or the entire line
       if (length($outline) + length($word) < 80)
       { $outline  .= $word; }
       else { 
         if ($outline) 
         { $text .= $outline . "\n"; }
         else { 
           extout::warning("unable to break line, " .
           "might cause trouble with cweave\n$word\n\n");
         }
         $outline = $word;
       }
     }
     $text .= $outline;   # no newline here
   }
   else 
   { $text .= $lines[$i]; }   # no newline here
}  

#line 3422 "ext.nw"
                                           
  } 
  return $text;
}


#line 3518 "ext.nw"
sub convert_M {
 local($Ctext) = @_; # read argument into local variable
 # we first work on identifiers 
 extout::debug("convert_M $Ctext\n");
 if ($Ctext eq "") { return ""; }
 local($prefix) = "";
 local($suffix) = $Ctext;
 local($ident) = "";

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
    { $ident = $1 ."\\Lunderscore\{$2\}"; }
    elsif ( length($ident) > 1 ) 
    { $ident = "\\mathit\{$ident\}"; }
    $prefix .= $ident;
  }
  $Ctext = $prefix . $suffix;
  $Ctext =~ s/&/\\&/g;
  $Ctext =~ s/_/\\nspaceunderscore\\_/g; # small negative space before _
  $Ctext =~ s/\\Lunderscore/_/g;
  $Ctext =~ s/\./\\nspacedot\./g;  # small negative space before .
  $Ctext =~ s/::/\\DP /g;
  $Ctext =~ s/<</\\ll/g;
  $Ctext =~ s/>>/\\gg/g;
  $Ctext =~ s/ *<= */\\Lle/g;
  $Ctext =~ s/ *>= */\\Lge/g;
  $Ctext =~ s/ *== */\\Leq/g;
  $Ctext =~ s/ *-> */\\Larrow/g;
  $Ctext =~ s/ *\+ */+/g;   # LaTeX takes care of the spacing
  $Ctext =~ s/ *- */-/g;
  $Ctext =~ s/ *\* +/\*/g;
  if ($Ctext =~ /<.*>/) {
    # I guess that I discovered template brackets. Therefore, I interpret
    # all brackets in the text as template brackets and I keep the spacing
    # in the input.
    $Ctext =~ s/</\\Ltemplateless/g;
    $Ctext =~ s/>/\\Ltemplategreater/g;
  } else {
    # there are no template brackets. I interpret < and > as in Latex and 
    # leave the spacing to LATEX
    $Ctext =~ s/ *< */</g;
    $Ctext =~ s/ *> */>/g;
  }
  $Ctext =~ s/ *\+= */\\Lass\{\+\}/g;
  $Ctext =~ s/ *\-= */\\Lass\{\-\}/g;
  $Ctext =~ s/ *\*= */\\Lass\{\*\}/g;
  $Ctext =~ s/ *\/= */\\Lass\{\/\}/g;
  $Ctext =~ s/ *\\&= */\\Lass\{\\&\}/g;
  $Ctext =~ s/ *!= */\\Lass\{\!\}/g;
  $Ctext =~ s/ *\|= */\\Lass\{\|\}/g;

  # All blanks that are still in Ctext are to be preserved and
  # are hence quoted.
  $Ctext =~ s/ /\\ /g;
  $Ctext =~ s/\+\+/\\Dplus /g;
  $Ctext =~ s/\-\-/\\Dminus /g;
  # $Ctext =~ s/!=/\\Noteq /g;
  $Ctext =~ s/\\Lle/\\Lle /g;
  $Ctext =~ s/\\Lge/\\Lge /g;
  $Ctext =~ s/\\Leq/\\Leq /g;
  $Ctext =~ s/\\Ltemplateless/\\Ltemplateless /g;
  $Ctext =~ s/\\Ltemplategreater/\\Ltemplategreater /g;
  $Ctext =~ s/\\Larrow/\\Larrow /g;
  $Ctext =~ s/%/\\%/g; 
  $Ctext =~ s/\^/\\circumflexop /g;
  $Ctext =~ s/~/\\tildeop /g;
  $Ctext =~ s/'/\\Lrquote /g;
  $Ctext =~ s/`/\\Llquote /g; 
  $Ctext =~ s/\(\)/\(\\;\)/g;      # a little space for empty argument list
  $Ctext =~ s/\,\\ /\,\} \\ensuremath\{/g;  
  $Ctext =~ s/\\Ltemplateless\\ /\\Ltemplateless\}\n\\ensuremath\{/g;  
  $Ctext =~ s/\\ \\Ltemplategreater/\}\n\\ensuremath\{\\Ltemplategreater/g;  
  $Ctext = "\\ensuremath\{" . $Ctext . "\}";
  #$Ctext =~ s/\,\\ /\,\$\}\n\\mbox\{\$/g;  
  #$Ctext =~ s/\\Ltemplateless\\ /\\Ltemplateless\$\}\n\\mbox\{\$/g;  
  #$Ctext =~ s/\\ \\Ltemplategreater/\$\}\n\\mbox\{\$\\Ltemplategreater/g;  
    # separate mbox at ,blank OR <blank OR blank> by $}newline\mbox{$ 
    # to allow better line breaks
  #$Ctext = "\\mbox\{\$" . $Ctext . "\$\}";
  extout::debug("convert_M $Ctext\n");
  $Ctext;
}


#line 3622 "ext.nw"
# the following operation does just a roman typesetting of code text

sub convert_R {
  local($romantext) = @_;
  $romantext =~ s/([^\\])\_/$1\\nspaceunderscore\\_/g;
  $romantext;
}

# math like style for index
sub convert_MI {
  local($indexentry) = @_;
  $indexentry .= '@' . &convert_M($indexentry); 
  # this makes entry the key and the second the look of it
  $indexentry;
}






#line 3008 "ext.nw"
1;

