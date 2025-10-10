#line 5792 "ext.nw"
use extout;
use extbasic;

$INPUT = $ARGV[0] && shift;
$OUTPUT = $ARGV[0] && shift;
$mode = $ARGV[0] && shift;
$refined = 0;
@kill_stuff;

if ( $mode eq "refined" ) {
  $refined = 1; 
  $mode = $ARGV[0] && shift;
}
# print STDERR "$INPUT $OUTPUT $mode $refined\n";

if ( $mode eq "Cdoc" )    
{ require extCdoc; extCdoc->import(); $sem="\\textbf\{Semantics\:\ \}"; }
elsif ( $mode eq "HTML" ) 
{ require extHTML; extHTML->import(); $sem="\\textbf\{Semantics\:\ \}"; }
else                      
{ require extLdoc; extLdoc->import(); $sem="\\semantics"; }

if ($OUTPUT eq "") {
  print "usage
                ldel infile outfile [refined] [Ldoc|HTML|Cdoc]

removes manual comments from infile\n";
  exit;
}

$chunk_name = "";
open(INPUT)  || die "Error: Can't find input file $INPUT: $!\n";
open(OUTPUT,">$OUTPUT");
while ( <INPUT> ) {
  ground_state:
 
    $Mpreamble = 0;
    $Mignore = 0;
    $Manpage = 0;

    if ( /\<\<.*\>\>=/ ) {  # we read a chunk name and remember it
      $chunk_name = $_;
      print OUTPUT $_;
      next;
    }
    if ( s/\%LDEL (.*)// )
    { push @kill_stuff, $1; }
        
    if ( /\/\*\{\\(M|X)/ ) {
      # current line contains the begin of a manual comment
      # we determine whether we are dealing with a preamble comment
      # or a text comment
      # and we delete the header of the comment
      if ( /Mpreamble/ ) { $Mpreamble = 1; }
      if ( /Mtext/ || /Manpage/ || /Mdefinition/ || /Mgeneralization/ || 
           /Mtypes/ || /Moperations/ ||/Mimplementation/ || /Mcreation/ || 
           /Moptions/ || /Msubst/ || /Mexample/ || 
           /Mtypemember/ || /Mtypedef/ || /Menum/ )    
      { $Mignore = 1; }


      if ( /Manpage/ ) { 
        $Manpage = 1; 
        $Manpageline = $_;

        while ($_ && (! ( /\}\*\// ) ) ) {
          $_ = <INPUT>;
          chop $Manpageline;
          if (/\} +\*\//) {
            extout::warning("encountered } */ in manual comment. ".
                            "Did you mean }*/ ?");
          }
          $Manpageline .= $_;
        }
        if ( !$_ ) { die "Error: missing end comment\n"; }
        $_ = $Manpageline; 
      }
      s/(\}\*\/)/ $1/;    # insert space before end of comment
      s/ *\/\*\{\\\S*//;  # remove the manual comment 

      if ($Manpage == 1) {
        /\{([^\{\}]*)\}\s*\{([^\{\}]*)\}\s*\{([^\{\}]*)\}\s*\{([^\{\}]*)\}/; 
        $Mtype = $1;
        $par_list = $2;
        $title = $3;
        $Mvar = $4;
        if ($par_list =~ /^ *$/) 
        { $Mname = $Mtype; }
        else { # remove excessive blanks in parlist
          $par_list =~ s/ //g;
          $Mname = $Mtype."<".$par_list.">";
        } 
        $Manpage = 0;
      }
      $Mcomment = "";

      # we scan lines until we find the end of a manual comment
      while ($_ && (! ( /\}\*\// ) ) ) {
        $Mcomment .= $_;
        $_ = <INPUT>; 
      }

      # the current line contains the end of a manual comment 
      s/\}\*\/ *//; 
      $Mcomment .= $_;
      if ($Mpreamble == 1) {
        extbasic::subst_vars(*Mcomment);
        print OUTPUT MODE::convert_text($Mcomment);
        next;
      }
      $Mignore = 1 if ( !$chunk_name ); # clean all comments in @c 
      $Mignore = 1 if ( !$refined );    # pure ldel
      if ($Mignore == 1 ) { next; }  # no output 
         
      # we are in the standard case and want to output the appropriate stuff
      extbasic::subst_vars(*Mcomment);
      $Mcomment = extbasic::format_block($Mcomment,0,80);
      print OUTPUT "\@ $sem\{\%\n".MODE::convert_text($Mcomment)."}\n";

      $_ = <INPUT>;
      while ($_ && /^\s*\n$/) {  $_ = <INPUT>; }
      
      # we are in the first non-empty line after the manual comment

      if (!(/^\@/ || /^\<\<.*\>\>=/)) # current chunk is continued
      {  print OUTPUT $chunk_name; }
      
      goto ground_state; 
    }
    foreach my $REGEXP (@kill_stuff) 
    { s/$REGEXP//g;  }
    print OUTPUT $_; 
} # end while
close(OUTPUT);

