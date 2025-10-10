$project = $ARGV[0] && shift;
# this piece creates correct crosslinks for the web like
# chunk references as produced by emptyline

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


