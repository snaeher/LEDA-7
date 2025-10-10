#line 2693 "ext.nw"
package extbasic;

sub import {
  *mode =          *main::mode;
  *Mvar =          *main::Mvar;
  *Mtype =         *main::Mtype;
  *Mname =         *main::Mname;
}


#line 2715 "ext.nw"
sub subst_vars {
  local(*string) = @_;
  if ($string eq "") { return; }
  $string =~ s/\\Mvar/$Mvar/g;
  $string =~ s/\\Mtype/$Mtype/g;
  $string =~ s/\\Mname/$Mname/g;
  if ($string =~ /\\var\W/ || $string =~ /Mvar/) 
  { extout::warning("found an occurrence of \\var or an unslashed ".
                   "occurrence of Mvar. Did you mean \\Mvar?"); }
  if ($string =~ /\\type\W/ || $string =~ /Mtype/) 
  { extout::warning("found an occurrence of \\type or an unslashed ".
                   "occurrence of Mtype. Did you mean \\Mtype?"); }
  if  ($string =~ /\\nameW/ || $string =~ /Mname/) 
  { extout::warning("found an occurrence of \\name or an unslashed ".
                  "occurrence of Mname. Did you mean \\Mname?"); }
}


#line 2736 "ext.nw"
@substlist = ();     
# a list L1,R1,L2,R2,....  of all substitutions

sub apply_Msubst{
  local(*alias) = @_;
  my $i = 0;
  while ($i < @substlist)
  { $leftside =  $substlist[$i]; $i++;
    $rightside = $substlist[$i]; $i++;
    $alias =~ s/^$leftside/$rightside/g;
    $alias =~ s/([^\\])$leftside/$1$rightside/g;
    # \name can be used as an escape symbol to keep name
    $alias =~ s/\\$leftside/$leftside/g;
    # when escaping \name we delete the \ for typesetting
  }
}

sub store_Msubst{
  local($leftside,$rightside) = @_;
  remove_enclosing_blanks($leftside);
  remove_enclosing_blanks($rightside);
  push(@substlist,$leftside); 
  push(@substlist,$rightside);
}


#line 2770 "ext.nw"
sub remove_enclosing_blanks {
  if ($_[0] =~ /^ *$/) { $_[0] = ""; }
  else { 
    $_[0] =~ / *(.*[^ ]) *$/; 
    $_[0] = $1;   
  }
}

#line 2785 "ext.nw"
sub convert_inline_code {
  local($text,$mconv,$tconv) = @_; 
  # read argument into local variable
  local($output) = "";
  local($code);
  $text =~ s/\\begin\{Mverb\}/\\begin\{verbatim\}/gs;
  $text =~ s/\\end\{Mverb\}/\\end\{verbatim\}/gs;
iteration:
  while ($text) {
    if ( $text =~ /^([^\\\[\|\@]+)/s ) { 
      # transport string that does not start with [ or | or \ to output
      $text = $';
      $output .= $1;
      if ($text eq "") { last iteration; }
    } 
    # text is nonempty and starts with [ or | or \       
    if ($text =~ /^\\Mcode(.)/) {
      $text = $';
      $delimiter=$1;
      if ( !( $text =~ /\\$1/s ))  # \ to protect meta characters
      { extout::warning("No Mcode $delimiter in rest of text chunk."); }
      $text = $';
      $output .= &$mconv($`);
      next iteration;
    }
    if ($text =~ /^\\Tcode(.)/) {
      $text = $';
      $delimiter=$1;
      if ( !( $text =~ /\\$1/s ))  # as above
      { extout::warning("No Tcode $delimiter in rest of text chunk."); }
      $text = $';
      $output .= &$tconv($`);
      next iteration;
    }
    # next we deal with | in its exceptional meanings
    if ( $text =~ /^\\begin\{tabular\}[^\{]*\{[^\}]*\}/s )
    { $text = $'; $output .= $&; next iteration; }
    if ( $text =~ /^\\left\|/) 
    { $text = $'; $output .= $&; next iteration; }
    if ( $text =~ /^\\right\|/)
    { $text = $'; $output .= $&; next iteration; }
    if ( $text =~ /^\|\|/) 
    { $text = $'; $output .= $&; next iteration; }
    if ( $text =~ /^\@\[\[/) 
    { $text = $'; $output .= $&; next iteration; }
    if ( $text =~ /^\\begin\{.*?verbatim.*?\}/ ) {
      $text = $'; $output .= $&;
      if ( !( $text =~ /\\end\{.*?verbatim.*?\}/s ))
      { extout::warning("Encountered non-closed verbatim :\n$text"); }
      $text = $';
      $output .= $`.$&;
      next iteration;
    }
    if ($text =~ /^\|/) {
      $text = $';
      if ( !( $text =~ /([^\@])\|/s )) 
      { extout::warning("Encountered | without matching |:\n|$text"); }
      $text = $';
      $code = $`.$1; 
      $code =~ s/\@\|/\|/gs;
      $code =~ s/\n/\ \n/gs;
      $output .= &$mconv($code);
      next iteration;
    }
    if ($text =~ /^\[\[/) {
      $text = $';
      if ( !( $text =~ /([^\@])\]\]/s ))
      { extout::warning("Encountered [[ without matching ]]:[[$text"); }
      $text = $';
      $code = $`.$1; $code =~ s/\@\]\]/\]\]/gs;
      $output .= &$tconv($code);
      next iteration;
    }
     
    # text does not start with a special symbol. Move first symbol to output.
    $text =~ /^(.)/;
    $output .= $1;
    $text = $';
  }
  return $output . "\n";
}

#line 2870 "ext.nw"
sub convert_T {
 local($Ctext) = @_; # read argument into local variable
 $Ctext =~ s/&/\\&/g;
 $Ctext =~ s/_/\\_/g;
 $Ctext =~ s/{/\\{/g;
 $Ctext =~ s/}/\\}/g;
 $Ctext =~ s/%/\\%/g; 
 $Ctext =~ s/\^/\\circumflexop /g;
 $Ctext =~ s/~/\\Tildeop /g; 
 return "\{\\tt " . $Ctext . "\}";
}

sub format_block {
  local($text,$left,$cols) = @_;
  local($indent);
  for (1 .. $left) { $indent.=' '; }
  local($result) = "";
  $text =~ s/\n\n/\\\\/g;
  $text =~ s/\n/\ /g;
  $text =~ s/\s+/\ /g; 
  $text =~ s/^\s*//g;
  local($line)=$indent; 
  local($rest)="";
  getloop: {
    if ($text =~ s/(\S+\s+)//) { $rest .= $1; }
    else { last getloop; }
    if (length($line.$rest) < $cols) 
    { if ( $rest =~ /\\\\\s*/ ) { $line .= $`; $rest = $'; }
      else { $line .= $rest; $rest=""; redo getloop; }
    }
    $result .= $line."\n"; $line=$indent;
    redo getloop;
  }
  $line .= $rest;
  if ($line ne "") { $result .= $line; } 
  else { chop $result; }
  $result;
}




#line 2707 "ext.nw"
1;

