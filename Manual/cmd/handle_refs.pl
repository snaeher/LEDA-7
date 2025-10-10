$INPUT = $ARGV[0] && shift;
open (INPUT)  || die "Error: Can't find input file $INPUT: $!\n";

while (<INPUT>)
{ if (/\\ref\{([^\}]*)\}/)
  { $key = $1; $key1 = $key; $key1 =~ s/_/\\_/g;
    s/\\ref\{$key\}/\\htmlref\{$key1\}\{$key\}/g;
  }
  if (/\\pageref\{([^\}]*)\}/)
  { $key = $1; $key1 = $key; $key1 =~ s/_/\\_/g;
    s/\\pageref\{$key\}/\\htmlref\{$key1\}\{$key\}/g;
  }
  print;
}


