#line 6131 "ext.nw"
use extout; 
use exthelp; 
use extbasic; 

$INPUT = $ARGV[0] && shift;
$mode = $ARGV[0] && shift;
if ($INPUT eq "") {
  print "
Usage is
          lw2nw file [HTML|Cdoc]

Converts foo.lw to foo.nw.\n";
  exit;
}

if ( $mode eq "Cdoc" )    { require extCdoc; extCdoc->import(); }
elsif ( $mode eq "HTML" ) { require extHTML; extHTML->import(); }
else                      { require extLdoc; extLdoc->import(); }

$chunktext="";
if ($mode eq "Cdoc") { $chunkenv="ccExampleCode"; $lkl=$rkl="";}
elsif ( $mode =~ /HTML/ ) { $chunkenv="verbatim"; $lkl=$rkl=""; }
else { 
  $chunkenv="Lcode"; $lkl='[['; $rkl=']]'; 
  $mynewline = ' \\\\ '; 
  $myemptynewline = ' \\\\[\\Lcodelineskip] ';  
}
open (INPUT)  || die "Error: Can't find input file $INPUT: $!\n";
while (<INPUT>) {
  if (/^\@c/) { # unnamed code section
    print MODE::convert_text($chunktext); $chunktext="";
    # I read with one line lookahead because I want to remove empty lines 
    # at the end of codechunks.
    $_ = <INPUT>; $previous = "\\begin{$chunkenv}"; $preprevious = "";
    if (!$_) { die "Error: File terminated without @ \\end{document}"; }      
    while (!/^\@\s/) {
      $current = $_;

      if ( !($preprevious eq "") ) 
      { if ( $preprevious =~ /\\begin{$chunkenv}/ )
        { print $preprevious . "\n"; }
        else { 
          if ( $preprevious =~ /\[\[\s*\]\]/ ) {
            print $preprevious . $myemptynewline . "\n"; 
	  } else { 
	    print $preprevious .  $mynewline . "\n"; 
          }    
	}
      }

      chop($current);  # der reine Code ohne newline
      if ($current =~ /\<\<(.*)\>\>/) {  # quote underscore in junk name
        $junk_name = $1;
        $cleft = $` . "<<";
        $cright = ">>" .$';
        $junk_name =~ s/_/\\_/g;
        $current = $cleft .$junk_name . $cright;
      }
      $preprevious = $previous;
      $previous = $lkl.$current.$rkl;     
      $_ = <INPUT>;
      if (!$_) { die "Error: File terminated without @ \\end{document}"; }
    }
    # current line starts text after code
    if ( $previous =~ /$lkl\s*$rkl/ ) 
    { print $preprevious . "\n";
      print "\\end{$chunkenv}\n"; 
    }
    else  
    { if ( $preprevious =~ /$lkl\s*$rkl/ )
      { $preprevious .= $myemptynewline;  }
      else { $preprevious .=  $mynewline; }    
      print $preprevious . "\n"; 
      print $previous . "\\end{$chunkenv}\n"; 
    }
  } # current line starts with @space (if condition applied)
  s/^\@[\(\<](.*)\@> *\+ *=/\<\<$1\>\>=/;  
    # named code section: Cweb-style convert to Noweb style 
  s/(\<\<.*\>\>)\+ *=/$1=/;
    # correct common mistake: replace >>+ = by  >>=
  if (/^\<\<.*\>\>=/) {
    print MODE::convert_text($chunktext);$chunktext=""; 
    while (!/^\@\s/) {
      # I print line after treating applied occurrences
      # of Cweb-chunks
      s/\@\<(.*)\@\>/\<\<$1\>\>/;
      if (/\<\<(.*)\>\>/) {  # quote underscore in junk name
        $junk_name = $1;
        $cleft = $` . "<<";
        $cright = ">>" .$';
        $junk_name =~ s/_/\\_/g;
        $_ = $cleft .$junk_name . $cright;
      }
      print; 
      $_ = <INPUT>;
      if (!$_) 
      { die "Error: file terminated without @ \\end{document}"; }
    }
  }
  $chunktext .= $_;
}
print MODE::convert_text($chunktext);$chunktext=""; 
close(INPUT);
 


