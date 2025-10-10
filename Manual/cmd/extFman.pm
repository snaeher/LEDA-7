#line 4412 "ext.nw"
package extFman;

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

#line 4416 "ext.nw"
  
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


#line 4417 "ext.nw"
}

#line 4426 "ext.nw"
sub parse_parameters {
  if ($ARGV[0]) {
    $filter = $ARGV[0];    
    foreach my $candidate ("all","signatures","definition","types", "events",
                           "creation","operations","implementation","example") {
      if ($filter eq $candidate) { return; }
    }
    extout::info("Searching for operation $filter of type $filearg");
  }   
}

sub output_preamble {}

sub new_manpage { 
  extout::unit("$title ( $Mtype )"); 
}

sub index_entry {}
sub set_vars {}

sub generalization {
  @params = @_;
  my $generalization;
  my $prefix;
  foreach $i (0 .. ($#params) ) {
    my $superclass = $prefix . $params[$i];
    if ( $superclass =~ s/\^$// ) { $prefix = " <-- "; }
    else { $prefix = ""; }
    $generalization .= "$superclass\n\n"; 
  }
  $generalization .= " <-- $Mtype\n";
  return $generalization;
}


sub top_section {
  my($header,$semantics);
  $header = ucfirst($command)."\n____________________________";
  $semantics = &convert_to_ASCII($Mcomment,1,80);
  if ( $includeline ne "" ) 
  { $semantics .= "\n$includeline"; $includeline=""; }
  extout::unit($header);
  extout::unit($semantics);
}

sub print_text {
  extout::unit(&convert_to_ASCII($Mcomment,1,80)) if ($showsem); 
}

sub print_constructor {
  extout::debug("print_constructor:".join(':',@_));
  my ($type,$var,$args,$semantics) = @_;
  my $signature = $type . " " . $var;
  $signature .= " ( $args )" if ($args); 
  $template = "template <$template>\n" if ($template);
  extout::unit($template.$signature);
  extout::unit(convert_to_ASCII($semantics,5,70));
}

sub print_destructor {
  extout::debug("print_destructor:".join(':',@_));
  my ($type,$semantics) = @_;
  my $signature = "~".$type."()";
  extout::unit($signature);
  extout::unit(convert_to_ASCII($semantics,5,70));
}

sub print_function {
  extout::debug("print_function:".join(':',@_));
  my($type, $signature, $semantics, $prefix, $funcname, $postfix) = @_;
  if ($funcname) 
  { $signature = $prefix . $funcname . "(". $postfix. ")"; }
  $template = "template <$template>\n" if ($template);
  extout::unit($template.$type." ".$signature );
  extout::unit(convert_to_ASCII($semantics,5,70));
}

sub print_typemember {
  my ($scope,$type,$semantics) = @_;
  my $signature = $scope."::".$type;
  $template = "template <$template>\n" if ($template);
  extout::unit($template.$signature);
  extout::unit(&convert_to_ASCII($semantics,5,70));
}

sub print_typedef {
  my ($fromtype,$newtype,$semantics) = @_;
  my $signature = "typedef $fromtype $newtype";
  extout::unit($signature);
  extout::unit(&convert_to_ASCII($semantics,5,70));
}

sub print_event {
  my ($scope,$eventname,$par_list,$semantics) = @_;
  my $signature = $scope.$eventname."(".$par_list.")";;
  extout::unit($signature);
  extout::unit(&convert_to_ASCII($semantics,5,70));
}

sub print_enum {
  my ($type,$enumconsts,$semantics) = @_;
  my $signature = $Mname."::".$type;
  extout::unit("$signature \{ ".$enumconsts." \}");
  extout::unit(&convert_to_ASCII($semantics,5,70));
}

sub post_process { exit; }

#line 4419 "ext.nw"
                   
#line 4538 "ext.nw"
# ascci text formatting
sub convert_to_ASCII {
  local($text) = shift(@_);
  if ($text eq "") { return ""; }
  $text = remove_confusing_latex($text);

  local($left) = shift(@_);
  local($cols) = shift(@_);
  return extbasic::format_block($text,$left,$cols);
}


#line 4552 "ext.nw"
sub remove_confusing_latex {
   local($text) = @_; 
   $text =~ s/\\\_/\_/g;
   $text =~ s/\\\{/{/g;
   $text =~ s/\\\}/}/g;
   $text =~ s/\\ / /g;
   $text =~ s/\\\&/\&/g;
   $text =~ s/\\dots/.../g;
   $text =~ s/\\ldots/.../g;
   $text =~ s/\\pi[\W]/pi/g;
   $text =~ s/\\leq/<=/g;
   $text =~ s/\\times/ x /g;
   $text =~ s/\\le/</g;
   $text =~ s/\\geq/>=/g ;
   $text =~ s/\\ge/\>/g;
   $text =~ s/\\ne/!=/g ;
   $text =~ s/\\in/ in/g;
   $text =~ s/\\Lvert/\|/g;
   $text =~ s/\\</</g;
   $text =~ s/\\>/>/g;
   $text =~ s/\\text..\{([^\}]*)\}/$1/g;
   $text =~ s/\{\\\w{2}?([^\}]*)\}/$1/g;
   $text =~ s/\\headerline\{([^\}]*)\}/uc($1)."\n\n"/ge;
   $text =~ s/\\Litem\{([^\}]*)\}/'<'.$1.'>'/ge;
   $text =~ s/\$([^\$]*)\$/'|'.$1.'|'/ge;
   $text =~ s/\\precond/Precondition\: /g;
   $text =~ s/\\bigskip//g;

   $text;
}




#line 4422 "ext.nw"
1;

