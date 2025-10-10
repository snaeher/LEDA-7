#line 6310 "ext.nw"
$INPUT = $ARGV[0] && shift;
if ($INPUT eq "") {
  print "Usage is
          emptyline filename\n";
  exit;
}
$mode = $ARGV[0] && shift;

if ($mode eq HTML) { #  now HTML
  open(INPUT);
  while (<INPUT>) {
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
  close(INPUT);
} elsif ($mode ne "Cdoc") {
  open (INPUT)  || die "Error: Can't find input file $INPUT: $!\n";
  $anzahl = 0;
  while (<INPUT>) {
    $codechunk = ""; 
    if (/nwbegincode/) {
      $line = $_; chop($line);
      print $line,"\\nwcodepenalty=\\Lhighpen\n"; 
      $_ = <INPUT>;
      # $anzahl = 0;           
      while (!/nwendcode/ || /nwbegincode/) {
        if (/\S/) { $codechunk .= $_; }
        else { $codechunk .= "\\vspace{\\Lemptyline}"; $anzahl++; }         
        $_ = <INPUT>;
      }
    }
    if (/begin{Lcode}/) {
      print;
      $_ = <INPUT>; 
      # $anzahl = 0;          
      while (!/end{Lcode}/) {
        #if (/^\{\\Tt\{\}\}/)
        #{ $codechunk .= "\\vspace{\\Lemptyline}"; $anzahl++; }
        #else { $codechunk .= $_; }         
        #$_ = <INPUT>;
          # ge"andert am 27. M"arz 2000 by KM
          $line = $_;
          $line =~ s/\\\\/Lcodelineforward/g;
          $line =~ s/\\ /\\LcodeS /g;
          $line =~ s/Lcodelineforward/\\\\/g;
          $codechunk .= $line;      
          $_ = <INPUT>;	
      }
    }
    if ($codechunk) {
      $nlines = ($codechunk =~ tr/\n/\n/); $lcount = 0;
      while ($codechunk =~ /^([^\n]*)\n/) {
        $line = $1;  $lcount++;
        $codechunk = $';
        if ($lcount <= 2 || ($lcount >= $nlines - 2 && $lcount < $nlines)) 
        # 3 changed to 2 by KM on March 22nd 99
        { print $line, "\\nwcodepenalty=\\Lhighpen\n"; }
        else { print $line , "\\nwcodepenalty=\\Llowpen\n"; }
      }
      print $codechunk;
    }
    
    print;   # Prints text and nwendcode and end{Lcode} respectively.
    while ($anzahl > 0) {print "\%\n"; $anzahl--;}
  }
  close(INPUT);
} else {
  open (INPUT)  || die "Error: Can't find input file $INPUT: $!\n";
  while (<INPUT>) {
    print;
  }  
  close(INPUT);
}

sub clean_label {
  local($label) = @_;
  $label =~ s/\\//g;
  $label =~ s/\_//g;
  $label =~ s/\$//g;
  return $label;
}

