#line 4587 "ext.nw"
package extCdoc;

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

#line 4592 "ext.nw"
  
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


#line 4593 "ext.nw"
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









#line 4604 "ext.nw"
sub output_preamble {
  extout::unit("% begin cgal manual page");
}

#line 4610 "ext.nw"
sub new_manpage {
  extout::unit("\\begin{ccRefClass}{$Mname}\\ccCreationVariable{$Mvar}");
  extout::unit("\\ccSection{$title}") if ($print_title eq "yes");
}

sub index_entry {
  return if ($indexing ne "yes");
  my ($entry,$scope) = @_;
  extout::warning("Cdoc::index_entry not implemented.");
}

sub set_vars {
  if (@_[0] =~ /^[a-zA-Z]/) { 
    extout::unit("\\ccCreationVariable{@_[0]}");
    shift; }
  if (@_[1] ne "") { # two numbers a b
    extout::unit("\\ccSetTwoOfThreeColumns{@_[0]}{@_[1]}");
  } elsif (@_[0] ne "") {
    extout::unit("\\ccSetOneOfTwoColumns{@_[0]}");
  }
}

sub print_text {
  return if ($command eq "preamble" && $mode eq "Cdoc") ;
  local($text) = @_;
  extout::unit(convert_text($text));
}


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
    $superclass = convert_C($superclass);
    $generalization .= "\\classframe{$superclass}$genmode\n"; 
  }
  $superclass = $params[$#params];
  $generalization .= "\\classframe{".convert_C($superclass).
                     "}\\genarrow{2}\\indentframe\n"; 
  $generalization .= "\\classframe{".convert_C($Mname)."}\n";
  $generalization .= "\\end{picture}\n";
  return $generalization;
}


sub top_section {
  my($header,$semantics);
  $header = "\\cc".ucfirst($command);
  $semantics = &convert_text($Mcomment) if ($Mcomment);
  if ($includeline) 
  { $semantics .= "\n\ccInclude{$includeline}"; $includeline=""; }
  extout::unit($header);
  extout::unit($semantics);
}

sub print_constructor {
  extout::debug("print_constructor:".join(':',@_));
  my ($type,$var,$args,$semantics) = @_;
  $semantics = convert_text($semantics); $semantics =~ s/\n\s*\n/\n/gs;
  $template = "template \<$template\>\n" if ($template);
  extout::unit("\\ccConstructor{$template$type($args)}{$semantics}"); 
}

sub print_destructor {
  extout::debug("print_destructor:".join(':',@_));
  my ($type,$semantics) = @_;
  $semantics = convert_text($semantics); $semantics =~ s/\n\s*\n/\n/gs;
  my $signature = "~".$type."();";
  $template .= " " if $template;
  extout::unit("\\ccFunction{$template$signature\}\n\{$semantics\}");
}

sub print_function {
  extout::debug("print_function:".join(':',@_));
  my($type, $signature, $semantics, $prefix, $funcname, $postfix) = @_;
  $semantics = convert_text($semantics); $semantics =~ s/\n\s*\n/\n/gs;
  # if (!$prefix) 
  # { extout::unit("\\ccFunction{$type $funcname($postfix);}{$semantics}"); }
  # else
  # { extout::unit("\\ccMethod{$type $funcname($postfix);}{$semantics}"); }
  $template = "template \<$template\>\n" if ($template);
  if ( $command =~ /^opl?$/ || $command =~ /^binopl?$/ ||
       $command =~ /^arropl?$/ || $command =~ /^funopl?$/ ||
       $command =~ /^unopl?$/ || $command =~ /^staticl?$/ ||
       $command =~ /^conversion/ )
  { extout::unit("\\ccMethod{$template$cleanprototype;}{$semantics}"); }
  else
  { extout::unit("\\ccFunction{$template$cleanprototype;}{$semantics}"); }
}

sub print_typemember {
  my ($scope,$type,$semantics) = @_;
  $semantics = convert_text($semantics); $semantics =~ s/\n\s*\n/\n/gs;
  $template = "template \<$template\>\n" if ($template);
  extout::unit("\\ccNestedType{$template$type}{$semantics}");
}

sub print_typedef {
  my ($fromtype,$newtype,$semantics) = @_;
  my $signature = "typedef $fromtype $newtype;";
  $semantics = convert_text($semantics); $semantics =~ s/\n\s*\n/\n/gs;
  extout::unit("\\cctypedef{$signature}{$semantics}");
}

sub print_event {
  my ($scope,$eventname,$par_list,$semantics) = @_;
  my $signature = $scope.$eventname."(".$par_list.")";
  $semantics = &convert_text($semantics); $semantics =~ s/\n\s*\n/\n/gs;
  extout::unit("\\ccNestedType{$signature}{$semantics}");
}

sub print_enum {
  my ($type,$enumconsts,$semantics) = @_;
  my $signature = $Mname."::".$type;
  $signature = "enum $type { $enumconsts }";
  $semantics = convert_text($semantics); $semantics =~ s/\n\s*\n/\n/gs;
  extout::unit("\\ccEnum{$signature}{$semantics}");
}


#line 4747 "ext.nw"
sub post_process {
  close(OUTPUT);
  my $addmanpages = "";
  my $defines_own = 0;
  foreach my $key (keys(%main::existing_outfiles)) {
    if ( ! ($key =~ /ext\.tex/) ) { $addmanpages .= $key." "; }
    open OUTPUT, ">>$key"; 
    extout::unit("\\end{ccRefClass}\n"); 
    close OUTPUT;
    $defines_own = 1 if ( $key eq "$basename.man" );
  }

  system("mv /tmp/$pid-ext.tex $basename.man") if (!$defines_own);
  extout::info("  Created $basename.man $addmanpages");
  extout::info("Removing the manual comments (except for Mpreamble)".
              " from $INPUT...");
  my $tempfile = "/tmp/$pid-$basename";
  if ($delman eq "refined") {
    system("ldel $INPUT $tempfile.$ext refined Cdoc");
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

  extout::info("Calling lweave on $tempfile.$ext...");
  system( "lweave $tempfile.$ext Cdoc");
  system( "mv $tempfile.tex $basename.tex");

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
}


#line 4799 "ext.nw"
# we expect that placeholder substitution already took place
sub convert_text {
  local($text) = @_; # read argument into local variable
  $text = extbasic::convert_inline_code($text,\&convert_C,\&convert_C); 
  $text =~ s/\\precond/\\ccPrecond/gs;
  $text =~ s/\\headerline/\\ccHeading/gs;
  return $text;
}

sub convert_C {
 local($Ctext) = @_; # read argument into local variable
 $Ctext = "\\ccc{$Ctext}";
 $Ctext;
}




#line 4599 "ext.nw"
1;

