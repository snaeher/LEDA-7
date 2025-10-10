$INPUT = $ARGV[0] && shift;
open (INPUT)  || die "Error:extract_mtype: can't find input file $INPUT: $!\n";
while (<INPUT>) {  
  if ( /\/\*\{\\Manpage\s*\{([^\{\}]*)\}/ )
  { print "$1\n"; }
}

  
