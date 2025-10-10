#line 2447 "ext.nw"
package extout;
local ($code,$comment);

# we glue local variables to the main scope
sub import {
  *warnings =      *main::warnings;
  *nextwarning =   *main::nextwarning;
  *debugging =     *main::debugging;
  *ack =           *main::ack;
  *mode =          *main::mode;
  *indexentry =    *main::indexentry;
  *informational = *main::informational;
  *outfile =       *main::outfile;
}

sub error {
  local($text) =  @_;  # read argument into local variable
  print STDERR "A problem occured near line " , $. ,"\n";
  print STDERR "ERROR: ",$text,"\n\n";
  print STDERR "The current code unit is:\n";
  print STDERR $code, "\n";
  print STDERR "The current manual comment is:\n";
  print STDERR $comment, "\n\n";
  if ($ack eq "yes") 
  { print STDERR "*  "; read(STDIN,$meaningless,1); }
}

sub warning {
  local($text) =  @_;
  if ($warnings eq "no" || $nextwarning eq "no") { return; }
  if ($warnings eq "log")
  { open(LOGFILE,">>$mode.log");
    print LOGFILE "WARNING: $text\n"; 
    print LOGFILE "CODEUNIT:\n$code\n";
    print LOGFILE "MANCOMMENT:\n$comment\n\n";
    close(LOGFILE);
    return;
  }
  print STDERR "A problem occured near line " , $. ,"\n";
  print STDERR "WARNING: ",$text,"\n\n";
  print STDERR "The current code unit is:\n\n";
  print STDERR $code, "\n";
  print STDERR "The current manual comment is:\n\n";
  print STDERR $comment, "\n\n";
  if ($ack eq "yes") 
  { print STDERR "*  "; read(STDIN,$meaningless,1); }
}

sub info {
  local($text) =  @_;
  if ($informational eq "yes") {
    local($text) =  @_;  # read argument into local variable
    print STDERR "$text\n\n";
  }
}

sub debug {
  local($text) =  @_;
  if ($debugging eq "yes") {
    local($text) =  @_;  # read argument into local variable
    print STDERR "DEBUG $text\n";
  }
}

sub unit {

  local($text) = @_; # read argument into local variable
  if ($indexentry ne "") { 
    chop $indexentry;
    $text .= "\n" . $indexentry;
    $indexentry=""; 
  } 
  if ( $text ne "" ) { $text .="\n\n"; }
  if ( $mode ne "Fman" ) { print main::OUTPUT $text; }
  else { print STDOUT $text; }
}

1;

