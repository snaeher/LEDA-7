sub clean_label {
  local($label) = @_;
  $label =~ s/\\//g;
  $label =~ s/\_//g;
  $label =~ s/\$//g;
  return $label;
}

$project = $ARGV[0] && shift;
$mode = $ARGV[0] && shift;

# this piece creates the link structure for the web like
# chunk references created by noweave
if ( $mode eq "BEFORE" ) {
  open(FILE,$project);
  rename($project, $project.'.bak');
  open(ARGVOUT, ">$project");
  select(ARGVOUT);
  while (<FILE>) {
    s/\<\!.*\>//;   
      # remove wrong html comment
    s/(\\documentclass.*$)/$1\n\\usepackage\{html\}/;
      # insert html package usage
      # insert some linebreaks
    if ( /(\<dfn\>\&lt\;)(.*)(\&gt\;\=\<\/dfn\>)/ ) {
      $lkl = "$`$1"; #&clean_label($chunk)
      my $chunk = $2;
      $rkl = $3.$';
      my $label = "HTMLDOC".&clean_label($chunk);
      $_ = "$lkl<A NAME\=\"$label\">$chunk$rkl";
    }
    if ( /(<i>\&lt\;)(.*)(\&gt\;<\/i>)/ ) {
      $lkl = $`.$1;
      my $chunk = $2;
      $rkl = $3.$';
      my $label = "HTMLDOC".&clean_label($chunk);
      $_ = "$lkl<A HREF=\"\#$label\">$chunk<\/A>$rkl";
    }
    s/(\\begin\{rawhtml\})(<pre>)/\n$1\n$2/;
    s/(\\end\{rawhtml\})(\\par)/\n$1\n$2\n/;
    s/\&quot\;/\"/g; 
    print;
  }
  select(STDOUT);
}

# this piece creates correct crosslinks for the web like
# chunk references induced in part BEFORE
if ( $mode eq "AFTER" ) {

  @files = glob("$project/*.html");
  foreach $file (@files) {
    my $fname = $file; $fname =~ s/.*\///; # basename
    open(FILE,$file);
    while (<FILE>) {
      if (/<A NAME=\"HTMLDOC(.*?)\">/) 
      { $labelfile{$1}=$fname; 
        # print STDERR "$fname $& $1\n"; 
      }
    }
    close(FILE);
  }
  foreach $file (@files) {
    open(FILE,$file);
    rename($file, $file.'.bak');
    open(ARGVOUT, ">$file");
    select(ARGVOUT);
    while (<FILE>) {
      s/<A HREF=\"\#HTMLDOC(.*?)\">/<A HREF=\"$labelfile{$1}\#$1\">/g;
      s/<A NAME=\"HTMLDOC(.*?)\">/<A NAME=\"$1\">/g;
    } continue {
      print;
    }
    close(FILE);
  }
  select(STDOUT);
}


