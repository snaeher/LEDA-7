
#line 1861 "ext.nw"
sub create_indexentry {	
  my ($mode,$entry,$scope) = @_;
  if ($indexing eq "yes") {
    if ( $mode =~ /HTML/ ) { 
      if ($scope eq "local")  
      { $indexentry .= "\\index\{$entry\!$MIname\}\n"; }
      else { $indexentry .= "\\index\{$entry\}\n"; }
    } else { # latex like
      if ($scope eq "local")  
      { $indexentry .= "\\index\{".&convert_R($entry)."\!".
	               &convert_MI($MIname)."\}\n"; }
      elsif ($scope eq "global") 
      { $indexentry .= "\\index\{".&convert_R($entry)."\}\n"; }
      else # class symbol
      { $indexentry .= "\\index\{".&convert_MI($entry)."\}\n"; }
    }
  }
}

sub short_prototype {
  local($mode) = shift(@_);
  local($proto) = shift(@_);
  if ($proto eq "") { return ""; }
  local($params) = shift(@_);
  if ( ! ($mode =~ /HTML/) ) { 
    if (length($proto) > 20) 
    { $proto = substr($proto,0,17) . "..."; }
  }
  $proto = &convert_R($proto);
  if ($params) { $proto.="(...)"; } 
  else { $proto.="()"; }
  $proto;
}


sub print_unit {
   local($text) = @_; # read argument into local variable
   if ($indexentry ne "") { 
     chop $indexentry;
     $text .= "\n" . $indexentry;
     $indexentry=""; 
   } 
   if ( $text ne "" ) { $text .="\n\n"; }
   if ( $mode ne "Fman" ) { print OUTPUT $text; }
   else { print STDOUT $text; }
}


# the following is only used in HTML mode:

sub print_table_row {
  my @params = @_;
  my $row = "";
  #if ($indexentry ne "") { chop $indexentry; }
  foreach $i (0 .. $#params) {
    if ($params[$i] eq "") { $row.="\\ "; }
    else { $row.=$params[$i]; }
    if ($i == 0) { $row .= $indexentry; }
    if ($i == $#params) { $row .= " \\\\";} 
    else { $row .= " \& "; }
    # was { $row .= "\n" .$indexentry . " \\\\";}
  }
  if ($indexentry ne "") {  $indexentry=""; } 
  print OUTPUT $row , "\n";
}

#the following is only used in HTML mode:
sub multicol {
  my $colnum = shift(@_);
  my $content = shift(@_);
  my $line = "\\multicolumn\{$colnum\}\{l\}\{ ".$content."\}";
  $line;
}


#line 1950 "ext.nw"
# we expect that placeholder substitution already took place
sub convert_to_LaTeX {
  local($text) = @_; # read argument into local variable
  $text = &convert_inline_code($text,\&convert_M,\&convert_T);
  if ($kind eq "Cweb") {
    
#line 1964 "ext.nw"
local($i, $remline, $outline, $lines , $word); 
@lines = split(/^/,$text);  # split at new lines
$text = "";
foreach $i (0 .. $#lines) {
  if (length($lines[$i]) > 80) {
     $outline = "";
     $remline = $lines[$i];
     while ($remline ) {
       if ($remline =~ /[\w\,] /) 
       { $word = $` . $&; $remline = $'; }
       else 
       { $word = $remline; $remline = ""; }
       # a word charachter or komma followed by a blank or the entire line
       if (length($outline) + length($word) < 80)
       { $outline  .= $word; }
       else { 
         if ($outline) 
         { $text .= $outline . "\n"; }
         else { 
           &print_warning("unable to break line, " .
	   "might cause trouble with cweave\n$word\n\n");
         }
         $outline = $word;
       }
     }
     $text .= $outline;   # no newline here
   }
   else 
   { $text .= $lines[$i]; }   # no newline here
}  


#line 1955 "ext.nw"
                                           
  }
  return $text;
}


#line 3307 "ext.nw"
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
      { &print_warning("No Mcode $delimiter in rest of text chunk."); }
      $text = $';
      $output .= &$mconv($`);
      next iteration;
    }
    if ($text =~ /^\\Tcode(.)/) {
      $text = $';
      $delimiter=$1;
      if ( !( $text =~ /\\$1/s ))  # as above
      { &print_warning("No Tcode $delimiter in rest of text chunk."); }
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
      { &print_warning("Encountered non-closed verbatim :\n$text"); }
      $text = $';
      $output .= $`.$&;
      next iteration;
    }
    if ($text =~ /^\|/) {
      $text = $';
      if ( !( $text =~ /([^\@])\|/s )) 
      { &print_warning("Encountered | without matching |:\n|$text"); }
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
      { &print_warning("Encountered [[ without matching ]]:[[$text"); }
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

         

#line 3448 "ext.nw"
# the following procedure is copied from ext.nw. Please make changes only here.
# I still need a way to maintain consistency with ext.

sub convert_M {
 local($Ctext) = @_; # read argument into local variable
 # we first work on identifiers 
 &print_debug("convert_M $Ctext\n");
 if ($Ctext eq "") { return ""; }
 local($prefix) = "";
 local($suffix) = $Ctext;
 local($ident) = "";

identloop:
  while ($suffix =~ /^(\W*)(\w.*)$/) {
    $prefix .= $1;
    $suffix = $2;
    if ( $suffix =~ /^(\w+)(\W.*)$/ ) {
      $ident = $1;
      $suffix = $2;
    } else { 
      $ident = $suffix;
      $suffix = "";
    }
    if ($prefix =~ /\\$/) { # For things like \n
      if ($ident ne "n") 
      { &print_warning("Huch:\\ in  quoted code:\n".$Ctext); }
      $prefix .= "L" . $ident; next identloop;
    }
    if ($ident =~ /^[0-9]*$/ ) # just a number
    { $prefix .= $ident; next identloop; }
    if (($usesubscripts eq "yes") && ($ident =~ /^([a-zA-Z])([0-9]+)$/)) 
    { $ident = $1 ."\_\{$2\}"; }
    elsif ( length($ident) > 1 ) 
    { $ident = "\\mathit\{$ident\}"; }
    $prefix .= $ident;
  }
  $Ctext = $prefix . $suffix;
  $Ctext =~ s/&/\\&/g;
  $Ctext =~ s/_/\\nspaceunderscore\\_/g; # small negative space before _
  $Ctext =~ s/\./\\nspacedot\./g;  # small negative space before .
  $Ctext =~ s/::/\\DP /g;
  $Ctext =~ s/<</\\ll/g;
  $Ctext =~ s/>>/\\gg/g;
  $Ctext =~ s/ *<= */\\Lle/g;
  $Ctext =~ s/ *>= */\\Lge/g;
  $Ctext =~ s/ *== */\\Leq/g;
  $Ctext =~ s/ *-> */\\Larrow/g;
  $Ctext =~ s/ *\+ */+/g;   # LaTeX takes care of the spacing
  $Ctext =~ s/ *- */-/g;
  $Ctext =~ s/ *\* +/\*/g;
  if ($Ctext =~ /<.*>/) {
    # I guess that I discovered template brackets. Therefore, I interpret
    # all brackets in the text as template brackets and I keep the spacing
    # in the input.
    $Ctext =~ s/</\\Ltemplateless/g;
    $Ctext =~ s/>/\\Ltemplategreater/g;
  } else {
    # there are no template brackets. I interpret < and > as in Latex and 
    # leave the spacing to LATEX
    $Ctext =~ s/ *< */</g;
    $Ctext =~ s/ *> */>/g;
  }
  $Ctext =~ s/ *\+= */\\Lass\{\+\}/g;
  $Ctext =~ s/ *\-= */\\Lass\{\-\}/g;
  $Ctext =~ s/ *\*= */\\Lass\{\*\}/g;
  $Ctext =~ s/ *\/= */\\Lass\{\/\}/g;
  $Ctext =~ s/ *\\&= */\\Lass\{\\&\}/g;
  $Ctext =~ s/ *!= */\\Lass\{\!\}/g;
  $Ctext =~ s/ *\|= */\\Lass\{\|\}/g;

  # All blanks that are still in Ctext are to be preserved and
  # are hence quoted.
  $Ctext =~ s/ /\\ /g;
  $Ctext =~ s/\+\+/\\Dplus /g;
  $Ctext =~ s/\-\-/\\Dminus /g;
  # $Ctext =~ s/!=/\\Noteq /g;
  $Ctext =~ s/\\Lle/\\Lle /g;
  $Ctext =~ s/\\Lge/\\Lge /g;
  $Ctext =~ s/\\Leq/\\Leq /g;
  $Ctext =~ s/\\Ltemplateless/\\Ltemplateless /g;
  $Ctext =~ s/\\Ltemplategreater/\\Ltemplategreater /g;
  $Ctext =~ s/\\Larrow/\\Larrow /g;
  $Ctext =~ s/%/\\%/g; 
  $Ctext =~ s/\^/\\circumflexop /g;
  $Ctext =~ s/~/\\tildeop /g;
  $Ctext =~ s/'/\\Lrquote /g;
  $Ctext =~ s/`/\\Llquote /g; 
  $Ctext =~ s/\(\)/\(\\;\)/g;      # a little space for empty argument list
  $Ctext =~ s/\,\\ /\,\} \\ensuremath\{/g;  
  $Ctext =~ s/\\Ltemplateless\\ /\\Ltemplateless\}\n\\ensuremath\{/g;  
  $Ctext =~ s/\\ \\Ltemplategreater/\}\n\\ensuremath\{\\Ltemplategreater/g;  
  $Ctext = "\\ensuremath\{" . $Ctext . "\}";
  #$Ctext =~ s/\,\\ /\,\$\}\n\\mbox\{\$/g;  
  #$Ctext =~ s/\\Ltemplateless\\ /\\Ltemplateless\$\}\n\\mbox\{\$/g;  
  #$Ctext =~ s/\\ \\Ltemplategreater/\$\}\n\\mbox\{\$\\Ltemplategreater/g;  
    # separate mbox at ,blank OR <blank OR blank> by $}newline\mbox{$ 
     # to allow better line breaks
  #$Ctext = "\\mbox\{\$" . $Ctext . "\$\}";
  &print_debug("convert_M $Ctext\n");
  $Ctext;
}

#line 3554 "ext.nw"
# the following operation does just a roman typesetting of code text

sub convert_R {
  local($romantext) = @_;
  $romantext =~ s/([^\\])\_/$1\\nspaceunderscore\\_/g;
  $romantext;
}

# math like style for index
sub convert_MI {
  local($indexentry) = @_;
  $indexentry .= '@' . &convert_M($indexentry); 
  # this makes entry the key and the second the look of it
  $indexentry;
}

# ascci text formatting
sub convert_to_ASCII {
  local($text) = shift(@_);
  if ($text eq "") { return ""; }
  $text = &remove_confusing_latex($text);
  local($left) = shift(@_);
  local($cols) = shift(@_);
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




#line 3611 "ext.nw"
# the following procedure is copied from ext.nw. Please make changes only here.
# I still need  way to maintain consistency with ext.

sub convert_T{
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


#line 3634 "ext.nw"
sub convert_to_HTML {
 local($text) = @_; # read argument into local variable
 if ( $text eq "" ) { return ""; }
 &print_debug("convert_to_HTML $text\nTEXTEND convert_to_HTML");
 $text = &convert_inline_code($text,\&convert_M_for_HTML,\&convert_T);
 $text = &convert_math_for_HTML($text);
 $text = &deal_with_refs($text);
 return $text;
}


#line 3663 "ext.nw"
sub convert_M_for_HTML {
 local($Ctext) = @_; # read argument into local variable
 &print_debug("convert_M_for_HTML $Ctext\n");
 # we first work on identifiers 
 if ($Ctext eq "") { return ""; }
 local($prefix) = "";
 local($suffix) = $Ctext;
 local($ident) = "";

identloop:
  while ($suffix =~ /^(\W*)(\w.*)$/) {
    $prefix .= $1;
    $suffix = $2;
    if ( $suffix =~ /^(\w+)(\W.*)$/ ) {
      $ident = $1;
      $suffix = $2;
    } else { 
      $ident = $suffix;
      $suffix = "";
    }
    if ($prefix =~ /\\$/) { # For things like \n
      if ($ident ne "n") 
      { &print_warning("Huch:\\ in  quoted code:\n".$Ctext); }
      $prefix .= "L" . $ident; next identloop;
    }
    if ($ident =~ /^[0-9]*$/ ) # just a number
    { $prefix .= $ident; next identloop; }
    if (($usesubscripts eq "yes") && ($ident =~ /^([a-zA-Z])([0-9]+)$/)) 
    { $ident = $1 ."\\HTML\{SUB\}\{$2\}"; }
    else 
    { $ident = "\\HTML\{I\}\{$ident\}"; }
      
    $prefix .= $ident;
  }

  # All blanks that are still in Ctext are to be preserved and
  # are hence quoted.
  $Ctext = $prefix . $suffix;
  $Ctext =~ s/&/\\&/g;
  $Ctext =~ s/_/\\_/g;
  $Ctext =~ s/\%/\\%/g; 
  $Ctext =~ s/\#/\\#/g; 
  $Ctext =~ s/~/\$\\sim\$/g;
  $Ctext =~ s/\^/\$\\circumflexop\$/g;
  $Ctext;
}



#line 3735 "ext.nw"
sub protect_newlines {
 local ($latexenv,$text) = @_;
 if ( $text =~ /\\begin\s*\{(\s*$latexenv[\s\*]*)\}/s ) {
   &print_debug("protect_newlines $latexenv");
   local($unprocessed) = $text;
   local($transformed) = "";
   while ( $unprocessed =~ s/(.*?)(\\begin\s*\{\s*$latexenv[\s\*]*\})(.*?)(\\end\{\s*$latexenv[\s\*]*\})//s ) {
     local($before,$bk,$between,$ek,$unprocessed) = ($1,$2,$3,$4,$');
     $between =~ s/\\\\/LEDAARRAYNL/gs;
     $transformed .= $before . $bk . $between . $ek;
     &print_debug("protect_newlines $bk\n\n$between\n\n$ek\n\n");
   }
   $text = $transformed . $unprocessed;
 }
 return $text;
}

sub protect_math_environment {
 local ($latexenv,$text) = @_;
 if ( $text =~ /\\begin\{([^\}]*$latexenv[^\}]*)\}/s ) {
   local($lenv) = $1; $lenv =~ s/ //g;
   local($unprocessed) = $text;
   local($transformed) = "";
   while ( $unprocessed =~ s/(.*?)(\\begin\{[^\}]*$lenv[^\}]*\})(.*?)(\\end\{[^\}]*$lenv[^\}]*\})//s ) {
     local($before,$bk,$between,$ek,$unprocessed) = ($1,$2,$3,$4,$');
     $transformed .= $before .'\HTML[ALIGN=CENTER]{P}{\begin{makeimage}' 
	                     . $bk . $between . $ek 
			     .'\end{makeimage}}';
   }
   $text = $transformed . $unprocessed;
 }
 return $text;
}

sub protect_displaymath {
 # we convert $$ and \[ into normal math
 local ($text) = @_;
 if ( $text =~ /\$\$/s ) {
   local($unprocessed) = $text;
   local($transformed) = "";
   while ( $unprocessed =~ s/(.*?)(\$\$)(.*?)(\$\$)//s ) {
     local($before,$bk,$between,$ek,$unprocessed) = ($1,$2,$3,$4,$');
     $transformed .= $before . '\HTML{P}{}\HTML{CENTER}{' . 
	 "\\begin{makeimage}\n\$" . $between . "\$\n\\end{makeimage}" .
	 '}';
   }
   $text = $transformed . $unprocessed;
 }
 if ( $text =~ /\\\[/s ) {
   local($unprocessed) = $text;
   local($transformed) = "";
   while ( $unprocessed =~ s/(.*?)(\\\[)(.*?)(\\\])//s ) {
     local($before,$bk,$between,$ek,$unprocessed) = ($1,$2,$3,$4,$');
     $transformed .= $before . '\HTML{P}{}\HTML{CENTER}{' . 
	 "\\begin{makeimage}\n\$" . $between . "\$\n\\end{makeimage}" .
	 '}';
   }
   $text = $transformed . $unprocessed;
 }
 return $text;
}


sub is_complicated_math {
  local($math) = @_;
  if ($math=~ /array/   ||
      $math=~ /\\frac/s )
  { return 1; }
  return 0;
}

sub convert_math_for_HTML {
 local($Ctext) = @_; 
 &print_debug("convert_math_for_HTML1 $Ctext\nTEXTEND convert_math_for_HTML1");
 $Ctext =~ s/\n/LEDANEWLINE/gi;

  $Ctext = &protect_newlines("array",$Ctext);
  $Ctext = &protect_newlines("eqnarray",$Ctext);
  $Ctext = &protect_newlines("tabular",$Ctext);
  $Ctext = &protect_newlines("cases",$Ctext);
  $Ctext = &protect_newlines("matrix",$Ctext);

  $Ctext = &protect_math_environment("eqnarray",$Ctext);
  $Ctext = &protect_math_environment("displaymath",$Ctext);
  $Ctext = &protect_displaymath($Ctext);

  local($unprocessed) = $Ctext;
  $Ctext = "";
 
  while ( $unprocessed =~ /(.*?)\$([^\$]*)\$/s ) {
     # text contains two or more dollars
     $Ctext .= &quote_special_symbols_in_text_mode($1);
     local($math) = $2;
     $unprocessed = $';
     if ( &is_complicated_math($math) )
     { $Ctext .= "\\begin{makeimage}\$$math\$\\end{makeimage}"; } 
     else
     { $Ctext .= "\$".&simplify_math_for_HTML($math)."\$"; }
  }
  $Ctext .= &quote_special_symbols_in_text_mode($unprocessed);
  $Ctext =~ s/LEDANEWLINE/\n/gi;
  $Ctext =~ s/LEDAARRAYNL/\\\\/gi;
  &print_debug("convert_math_for_HTML2 RESULT:\n $Ctext\nENDRESULT convert_math_for_HTML2");
  return $Ctext;
}


#line 3845 "ext.nw"
# the following operation simplifies simple latex commands which 
# create complications when pumped through l2h.

sub replace_simple_latex_commands {
  local($math) = @_;
  $math =~ s/\\_/{\\_}/gs;
  $math =~ s/\\Lvert(\W|$)/|$1/gs;
  $math =~ s/\\cdot(\W|$)/*$1/gs;
  $math =~ s/\\leq(\W|$)/<=$1/gs;
  $math =~ s/\\le(\W|$)/<=$1/gs;
  $math =~ s/\\geq(\W|$)/>=$1/gs;
  $math =~ s/\\ge(\W|$)/>=$1/gs;
  $math =~ s/\\neq(\W|$)/!=$1/gs;
  $math =~ s/\\ne(\W|$)/!=$1/gs;
  $math =~ s/\\</</gs;
  $math =~ s/\\>/>/gs;
  $math =~ s/\\range\{(.*?)\}\{(.*?)\}/\[$1\.\.$2\]/gs;
  return $math;
}

sub step_over_braces {
  local($text) = @_;
  local($braced,$bracelevel) = ("",0);
  LOOP: while ( $text ) { 
    if ( $text =~ /([^\{\}]*)([\{\}])/s ) {
      $braced .= $`.$1.$2;
      $text = $';
      if ( $2 eq '{' ) { $bracelevel++; }
      if ( $2 eq '}' ) { $bracelevel--; }
    }
    last LOOP if $bracelevel==0;
  }
  if ($bacelevel != 0) { &error_handler("error in brace stepping"); }
  $braced =~ s/\{(.*)\}/$1/s; # remove outer backets
  return ($braced,$text);
}

sub simplify_math_for_HTML {
   local($math) = @_; 
   &print_debug("simplify_math_for_HTML $math\n");
   local($cleaned,$braced)=("","");
   while ( $math =~ /\\Labs/s ) {
     $cleaned .= $`;
     ($braced,$math) = &step_over_braces($');
     $cleaned .= "\\|$braced\\|";
   }
   $math = $cleaned.$math; $cleaned="";
   while ( $math =~ /\\Litem/s ) {
     $cleaned .= $`;
     ($braced,$math) = &step_over_braces($');
     $cleaned .= "<$braced>";
   }
   $math = $cleaned.$math; $cleaned="";
   # now we exclude the italic styled text from the math
   # by transporting them outside the math $..$ environment
   while ( $math =~ /\\HTML\{I\}/s ) {
     $cleaned .= $`;
     ($braced,$math) = &step_over_braces($');
     $cleaned .= "\\mathit\{$braced\}";
   }
   $math = $cleaned.$math; $cleaned="";
   $math = &replace_simple_latex_commands($math);
   &print_debug("simplify_math_for_HTML $math\n");
   $math;
}

#line 3914 "ext.nw"
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
   $text =~ s/\\text..\{([^\}]*)\}/$1/g;
   $text =~ s/\{\\\w{2}?([^\}]*)\}/$1/g;
   $text =~ s/\\headerline\{([^\}]*)\}/uc($1)."\n\n"/ge;
   $text =~ s/\\precond/Precondition\: /g;
   $text =~ s/\\bigskip//g;

   $text;
}


#line 3948 "ext.nw"
sub quote_special_symbols_in_text_mode {
  local($Ctext) = @_; # read argument into local variable
  # it remains to quote all special characters.
  $Ctext =~ s/\\\\/\\HTML\{BR\}\{\}/g;
  $Ctext =~ s/\\headerline/\\textbf/g;
  $Ctext =~ s/\\precond/\\textbf\{Precondition\}/sg;
  # starting latex2html 98.1 we can enforce a linebreak
  return $Ctext;
}



#line 3970 "ext.nw"
sub deal_with_refs{
 local($text) = @_; # read argument into local variable
 while ($text =~ /\\ref\{([^\}]*)\}/)
 { local($key) = $1;
   $key1 = $key; $key1 =~ s/_/\\_/g;
   $text =~ s/\\ref\{$key\}/\\hyperref{$key1}{}{}{$key}/;
   #$text =~ s/\\ref\{$key\}/\\htmlref{$key1}{$key}/;
 }
 return $text;

}



#line 3995 "ext.nw"
sub insert_URLs_and_quote{
  local($Ctext) = @_; 
  local(@fields) = split(/([\ \(\)\<\>\,\&\.\:\[\]\=\*\'])/,$Ctext);
  foreach $i (0 .. $#fields) {
    if ($fields[$i] =~ /^\w*$/) { # alphanumeric string
      local($key) = $fields[$i];
      local($label) = $namereplacementtable{$key};
      if ($label ne "" && $key ne $Mtype) {
        $key =~ s/_/\\_/g;
         $fields[$i] = "\\htmlref\{$key\}\{$label\}"; 
       }
       else {
         $key =~ s/_/\\_/g;
         $fields[$i] = $key;
       }
    }    
    else { # quote some characters
      if ($fields[$i] =~ /&/) {
        $fields[$i] = "\\$fields[$i]"; 
      }
      if ($fields[$i] =~ /</) {
        $fields[$i] = "\$<\$"; 
      }
      if ($fields[$i] =~ />/) {
        $fields[$i] = "\$>\$"; 
      }
      $fields[$i] =~ s/\\n/\$\\backslash\\mathtt\{n\}\$/g;  # takes care of \n
    }
  }
  $Ctext = join("",@fields);
  return $Ctext;
}





#line 2005 "ext.nw"
sub subst_vars {
  local(*string) = @_;
  # print "SUBST_VARS call on $string\n\n";
  if ($string eq "") { return; }
  $string =~ s/\\Mvar/$Mvar/g;
  $string =~ s/\\Mtype/$Mtype/g;
  $string =~ s/\\Mname/$Mname/g;
  if ($string =~ /\\var\W/ || $string =~ /Mvar/) 
  { &print_warning("found an occurrence of \\var or an unslashed ".
		   "occurrence of Mvar. Did you mean \\Mvar?"); }
  if ($string =~ /\\type\W/ || $string =~ /Mtype/) 
  { &print_warning("found an occurrence of \\type or an unslashed ".
		   "occurrence of Mtype. Did you mean \\Mtype?"); }
  if  ($string =~ /\\nameW/ || $string =~ /Mname/) 
  { &print_warning("found an occurrence of \\name or an unslashed ".
		  "occurrence of Mname. Did you mean \\Mname?"); }
}

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


#line 2769 "ext.nw"
sub print_function {

  &print_debug("print_function:".join(':',@_));
  my($mode, $type, $signature, $semantics, $prefix, $funcname, $postfix) = @_;

if ($mode eq "Fman") {
   if ($funcname) 
   { $signature = $prefix . $funcname . "(". $postfix. ")"; }
   &print_unit( $type." ".$signature );
   &print_unit(&convert_to_ASCII($semantics,5,70));
   return;
} elsif ($mode =~ /HTML/) {
  if ($funcname) 
  { $signature = $prefix . $funcname . "(". $postfix. ")"; }
  my $shortsig = ( length($signature) <= 25 );
  $type = &insert_URLs_and_quote($type);
  $signature = &insert_URLs_and_quote($signature);
  $semantics = &convert_to_HTML($semantics);
  &opentabular(3);
  if ( $shortsig ) 
  { &print_table_row($type, $signature, $semantics); }
  else {
    &print_table_row($type, &multicol(2,$signature));
    &print_table_row("","",$semantics);
  }
  return;
}

  $type = &convert_M($type);
  my $paralist = "";
  if ($funcname) {
    $funcname = &convert_M($prefix) . &convert_R($funcname);
    $paralist = &convert_M($postfix);
    if ($paralist eq "") { $paralist = "\$\\,\$"; } # small space
  } else {
    $signature = &convert_M($signature);
  }
  $semantics = &convert_to_LaTeX($semantics);

  if ( $type eq "" && $funcname eq "" && $signature =~ /\(\s*\)/ ) {
    &print_warning("I encountered an empty prototype ".
	  $signature . " and will not produce an entry.");
    return;
  }

  if ($funcname) {
      if (! ($command =~ /^opl?/ || $command =~ /^funcl?/ || $command =~ /^funobj/ ||
	     $command =~ /^staticl?/ || $command =~ /^conversion?/) )
      { &print_warning("wrong case for function print");}
    &print_unit("\\function\{$type\}\n\{$funcname\}\n".
		"\{$paralist\}\n\{$semantics\}");
  } else { 
    &print_unit("\\operator\{$type\}\n\{$signature\}\n\{$semantics\}"); 
  }
}

#line 2829 "ext.nw"
sub print_constructor {

  &print_debug("print_constructor:".join(':',@_));
  my ($mode,$Mname,$Mvar,$par_list,$semantics) = @_;

if (!$Mvar || !$Mname ) {
  &error_handler("You forgot to define either Mvar or Mname.");
}

my $signature;
if ($mode eq "Fman") {
  $signature = $Mname . " " . $Mvar;
  if ($par_list) { $signature .= " ( " . $par_list. " )"; }
  &print_unit($signature);
  &print_unit(&convert_to_ASCII($semantics,5,70));
  return;
}
if ($mode =~ /HTML/) {
  $semantics = &convert_to_HTML($semantics); 
  my ($sig1,$sig2);
  $sig2 = $Mvar;
  if ($par_list) { $sig2 .= "(" . $par_list. ")"; }
  my $shortsig = (length($sig2) <= 25);
  $sig1 = &insert_URLs_and_quote($Mname);
  $sig2 = &insert_URLs_and_quote($sig2);
  &opentabular(3);
  if ( $shortsig ) 
  { &print_table_row($sig1,$sig2,$semantics); }
  else {
    &print_table_row($sig1,&multicol(2,$sig2));
    &print_table_row("","",$semantics);
  }
  return;
}

my ($arg1,$arg2,$arg3,$arg4) = ("","","","");
$arg1 = &convert_M($Mname);  
$arg2 = &convert_M($Mvar);
$arg3 = &convert_M($par_list);
$arg4 = &convert_to_LaTeX($semantics);
print_unit("\\create\{$arg1\}\n\{$arg2\}\n\{$arg3}\n\{$arg4\}");
}

#line 2876 "ext.nw"
sub print_destructor {

  my ($mode,$Mname,$semantics) = @_;
  my $sig = "~".$Mname."()";
  if ($mode eq "Fman") {
    print_unit($sig);
    print_unit(&convert_to_ASCII($semantics,5,70));
  }
  elsif ($mode =~ /HTML/) {
    &print_table_row($sig,&convert_to_HTML($semantics));
  }

  my ($arg1,$arg2);
  $arg1 = &convert_M($Mname);
  $arg2 = &convert_to_LaTeX($semantics);
  &print_unit("\\destruct\{$arg1\}\n\{$arg2\}");
}


#line 2902 "ext.nw"
sub print_enum {

  my ($mode,$Mname,$type,$enumconsts,$semantics) = @_;
  my $signature = $Mname . "::" . "$type";
  my @enumvals = split(',',$enumconsts);
  &create_indexentry($mode, $type, "local");
  foreach my $enumv (@enumvals) 
  { &create_indexentry($mode, $enumv, "local"); }

if ($mode eq "Fman") {
  &print_unit("$signature \{ ".join(", ",@enumvals)." \}");
  &print_unit(&convert_to_ASCII($semantics,5,70));
  return;
}
if ($mode =~ /HTML/) {
  my $shortsig = (length($signature) <= 25);
  $signature = &insert_URLs_and_quote($signature);
  &opentabular(2);
  $semantics = &convert_to_HTML($semantics);
  if ($shortsig) 
  { &print_table_row($signature, $semantics); }
  else { 
    &print_table_row(&multicol(2,$signature)); 
    &print_table_row("",$semantics);
  }
  return;
}

  # now the latex print:
  $signature = &convert_M($signature);
  $enumconsts = &convert_R($enumconsts);
  $semantics = &convert_to_LaTeX($semantics);
  &print_unit("\\enum\{$signature\}\n\{$enumconsts\}\n\{$semantics\}");
}


#line 2947 "ext.nw"
sub print_typemember 
{
  my ($Mname,$type,$semantics) = @_;
  my $signature = $Mname."::"."$type";
  &create_indexentry($mode, $type, "local");

if ($mode eq "Fman") {
  &print_unit($signature);
  &print_unit(&convert_to_ASCII($semantics,5,70));
  return;
}
if ($mode =~ /HTML/) {
  $semantics = &convert_to_HTML($semantics);
  my $shortsig = ( length($signature) <= 25 );
  $signature = &insert_URLs_and_quote($signature);
  &opentabular(2);
  if ( $shortsig ) 
  { &print_table_row($signature, $semantics); }
  else {
    &print_table_row(&multicol(2,$signature));
    &print_table_row("",$semantics);
  }
  return;
}

  $signature = &convert_M($signature);
  $semantics = &convert_to_LaTeX($semantics);
  &print_unit("\\typemember\{$signature\}\n\{$semantics\}");
}


sub print_typedef 
{
  my ($fromtype,$newtype,$semantics) = @_;
  my $signature = "typedef $fromtype $newtype";
  &create_indexentry($mode, $newtype, "local");

if ($mode eq "Fman") {
  &print_unit($signature);
  &print_unit(&convert_to_ASCII($semantics,5,70));
  return;
}
if ($mode =~ /HTML/) {
  $semantics = &convert_to_HTML($semantics);
  my $shortsig = ( length($signature) <= 25 );
  $signature = &insert_URLs_and_quote($signature);
  &opentabular(2);
  if ( $shortsig ) 
  { &print_table_row($signature, $semantics); }
  else {
    &print_table_row(&multicol(2,$signature));
    &print_table_row("",$semantics);
  }
  return;
}

  $fromtype = &convert_M($fromtype);
  $newtype = &convert_M($newtype);
  $semantics = &convert_to_LaTeX($semantics);
  &print_unit("\\typedef\{$fromtype\}\{$newtype\}\n\{$semantics\}");
}

#line 3018 "ext.nw"
sub print_event 
{
  my ($scope,$eventname,$par_list,$semantics) = @_;
  my $signature = $scope.$eventname."(".$par_list.")";;
  &create_indexentry($mode, $eventname, "local");

if ($mode eq "Fman") {
  &print_unit($signature);
  &print_unit(&convert_to_ASCII($semantics,5,70));
  return;
}
if ($mode =~ /HTML/) {
  $semantics = &convert_to_HTML($semantics);
  my $shortsig = ( length($signature) <= 25 );
  $signature = &insert_URLs_and_quote($signature);
  &opentabular(2);
  if ( $shortsig ) 
  { &print_table_row($signature, $semantics); }
  else {
    &print_table_row(&multicol(2,$signature));
    &print_table_row("",$semantics);
  }
  return;
}

  $signature = &convert_M($signature);
  $semantics = &convert_to_LaTeX($semantics);
  &print_unit("\\event\{$signature\}\n\{$semantics\}");
}

#line 3201 "ext.nw"
sub error_handler{
  local($text) =  @_;  # read argument into local variable
  print STDERR "A problem occured near line " , $. ,"\n";
  print STDERR "ERROR: ",$text,"\n\n";
  print STDERR "The current code unit is:\n";
  print STDERR $original_code_unit, "\n";
  print STDERR "The current manual comment is:\n";
  print STDERR $original_comment, "\n\n";
  if ($ack eq "yes") 
  { print STDERR "*  "; read(STDIN,$meaningless,1); }
}

sub print_warning {
  if ($warnings eq "no" || $nextwarning eq "no") { return; }
  local($text) =  @_;  # read argument into local variable
  if ($warnings eq "log")
  { print LOGFILE "WARNING: $text\n"; 
    print LOGFILE "CODEUNIT:\n$original_code_unit\n";
    print LOGFILE "MANCOMMENT:\n$original_comment\n\n";
    return;
  }
  print STDERR "A problem occured near line " , $. ,"\n";
  print STDERR "WARNING: ",$text,"\n\n";
  print STDERR "The current code unit is:\n\n";
  print STDERR $original_code_unit, "\n";
  print STDERR "The current manual comment is:\n\n";
  print STDERR $original_comment, "\n\n";
  if ($ack eq "yes") 
  { print STDERR "*  "; read(STDIN,$meaningless,1); }
}

sub print_info {
  if ($informational eq "yes") {
    local($text) =  @_;  # read argument into local variable
    print STDERR "$text\n\n";
  }
}

sub print_debug {
  if ($debugging eq "yes") {
    local($text) =  @_;  # read argument into local variable
    print STDERR "DEBUG $text\n";
  }
}


#line 3256 "ext.nw"
sub remove_enclosing_blanks
{
  if ($_[0] =~ /^ *$/) { $_[0] = ""; }
  else { 
    $_[0] =~ / *(.*[^ ]) *$/; 
    $_[0] = $1;   
  }
}



#line 3271 "ext.nw"
sub opentabular{
 local($cols) = @_;  # this is either two, three or four
 if ($HTMLopentabular{$outfile} eq "no") {
   if ($cols == 2) {
     print OUTPUT "\\begin{tabular}{ll}\n"; }
   elsif ($cols == 3) {
     print OUTPUT "\\begin{tabular}{lll}\n"; }
   elsif ($cols == 4) {
     print OUTPUT "\\begin{tabular}{llll}\n"; }
   else {
     &error_handler("open_tabulator expects argument 2 or 3"); 
   }
   $HTMLopentabular{$outfile} = "yes";
 }
}

sub closetabular{
 if ($mode =~ /HTML/ && 
     $HTMLopentabular{$outfile} eq "yes") {
   print OUTPUT "\\end{tabular}\n\n";
   $HTMLopentabular{$outfile} = "no";
 }
}


#line 4035 "ext.nw"
sub print_usage {
if ($mode eq "Lman" || $mode eq "Ldoc") {
  print "Usage is
          $mode file [options]

Options are given in assignment syntax variable=value. There must be no
blank on either side of the equality sign. We list all variables and 
their possible values below. For each variable the default value of 
each option is given first.

size={12,11,10}
constref={no,yes}
partypes={no,yes}
numbered={no,yes}
xdvi={yes,no}
warnings={yes,no}
includefile={no,yes}
indexing={no,yes}
informational={yes,no}
ack={yes,no}
usesubscripts={no,yes}
latexruns={1,2,0}
delman={yes,no}
filter={all,signatures,definition,types,creation,
operations,implementation,example,opname}

$mode can be customized by putting options in a file 
$mode.cfg in either the home directory or the working 
directory.

Call 
      $mode ltools 
        for more information on the tools usage.
      $mode mancommands 
        for a short overview of usable manual commands.\n\n";
}

if ($mode eq "Fman") {
  print "Usage is
          $mode file filter

where the file name is of the form T.[h|w|lw] and T is either 
the name of a LEDA type, e.g., list, sortseq, or point, or 
the name of a user defined data type. The value of filter is 
one of 

 { all, signatures, definition, types, creation, 
   operations, implementation, example, opname }

Call 
      $mode ltools 
        for more information on the tools usage. 
      $mode mancommands 
        for a short overview of usable manual commands.\n\n" ;
}
exit;
}


sub print_mancommands {
if ($mode eq "Lman" || $mode eq "Ldoc" || $mode eq "Fman") {
print "Manpage start header is:

   /*{\\Manpage {DT} {T1,..,Tk} {short_description} [MVAR]}*/
   afterwards common variables are 
   \\Mname = DT<T1,..,Tk>
   \\Mvar =  MVAR

Manual sections are:

   /*{\\Mdefinition specification_text}*/

   /*{\\Mtypes [W]}*/
       /*{\\Mtypedef typedef_documentation}*/
       /*{\\Mtypemember type_documentation}*/
       /*{\\Menum type_documentation}*/

   /*{\\Mcreation [MVAR] [W]}*/
       /*{\\Mcreate constructor_specification }*/
       /*{\\Mdesctruct desctructor_specification }*/

   /*{\\Moperations [W1] [W2]}*/          
       /*{\\Mop semantic_description }*/ 	
       /*{\\Mstatic semantic_description }*/
       /*{\\Mconversion semantic_description }*/
       /*{\\Mbinop semantic_description }*/
       /*{\\Munop semantic_description }*/
       /*{\\Marrop semantic_description }*/ 
       /*{\\Mfunc semantic_description }*/
       /*{\\Mfunop semantic_description }*/
       /*{\\Mfunobj semantic_description }*/
       /*{\\Mbinopfunc semantic_description }*/
       /*{\\Munopfunc semantic_description }*/ 
       /*{\\Mfunobj{return_type} semantic_description }*/

   /*{\\Mevents [W]}*/
       /*{\\Mevent event_specification }*/

   /*{\\Mimplementation implemenation_information }*/
   /*{\\Mexample example_description }*/

Additional man commands
  /*{\\Mtext some_text }*/
  /*{\\Moptions nextwarning=no }*/
  /*{\\Moptions outfile=new_man_file_name }*/
  /*{\\Msubst pattern replacement }*/
Common tex macros for man comments are 
  \\setopdims[2], \\restoreopdims, \\precond[1], \\headerline[1]
Verbatim code in example section with environment 
  \\begin|end{Mverb}.\n\n"; 
}
exit;
}

sub print_ltools {
$owd = $ENV{"PWD"};
chdir ("/tmp");
local($pid)=$$;
if ($mode eq "Fman") {
  system("cat \$LEDAROOT/Manual/MANUAL/DocTools.tex");
}
else {
  $outfile = "/tmp/" . $pid . "-ext.tex";
  open(OUTPUT,">".$outfile);
  print OUTPUT "\\documentclass\[11pt,a4paper\]\{article\}\n\n"; 
  print OUTPUT "\\usepackage\{html\}";
  print OUTPUT "\\input " . $ENV{"LEDAROOT"} . "/Manual/tex/MANUAL.pagesize\n\n";
  print OUTPUT "\\input " . $ENV{"LEDAROOT"} . "/Manual/tex/MANUAL.mac\n\n"; 
  print OUTPUT "\\begin\{document\}\n\n";
  close(OUTPUT);
  system("cat \$LEDAROOT/Manual/MANUAL/DocTools.tex  >> $outfile");
  open(OUTPUT,">>".$outfile);
  print OUTPUT "\\end\{document\}\n\n";
  close(OUTPUT);
  system("latex /tmp/$pid-ext.tex $dontshowinfo");
  system("xdvi -s /tmp/$pid-ext.dvi $dontshowinfo");
  system("rm -f /tmp/$pid-ext.*");
}
chdir ("$owd");
exit;
}


#line 4881 "ext.nw"
$INPUT = $ARGV[0] && shift;
$OUTPUT = $ARGV[0];

if ($OUTPUT eq "") {
  print "usage

              ext_ldel infile outfile              

removes manual comments from infile\n";
  die;
}

$chunk_name = "";
open (INPUT)  || die "Error: Can't find input file $INPUT: $!\n";
open(OUTPUT,">$OUTPUT");

while (<INPUT>) {
  ground_state:
 
    $Mpreamble = 0;
    $Mignore = 0;
    $Manpage = 0;

    if (/\<\<.*\>\>=/) {  # we read a chunk name and remember it
      $chunk_name = $_;
      print OUTPUT $_;
      next;
    }
        
    if ( / *\/\*\{\\M/ || / *\/\*\{\\X/) {
      # current line contains the begin of a manual comment
      # we determine whether we are dealing with a preamble comment
      # or a text comment
      # and we delete the header of the comment
      if ( /Mpreamble/ ) { $Mpreamble = 1; }
      if ( /Mtext/ || /Manpage/ || /Mdefinition/ || /Mtypes/ || 
           /Moperations/ ||/Mimplementation/ || /Mcreation/ || 
	   /Moptions/ || /Msubst/ || /Mexample/ )    
      { $Mignore = 1; }

      if ( /Manpage/ ) { 
        $Manpage = 1; 
	$Manpageline = $_;

	while ($_ && (! ( /\}\*\// ) ) ) {
          $_ = <INPUT>;
	  chop $Manpageline;
          if (/\} +\*\//) {
            &print_warning("encountered } */ in manual comment. Did you mean }*/ ?");}
            $Manpageline .= $_;
          }
	  if (! $_){ die "Error: missing end comment\n"; }
	  $_ = $Manpageline; 
        }

        s/ *\/\*\{\\\S*//;  # remove the manual comment 

        if ($Manpage == 1) {
           /\{([^\{\}]*)\}\s*\{([^\{\}]*)\}\s*\{([^\{\}]*)\}\s*\{([^\{\}]*)\}/;               
           $Mtype = $1;
           $par_list = $2;
           $title = $3;
           $Mvar = $4;
           if ($par_list =~ /^ *$/) 
           { $Mname = $Mtype; }
           else 
           { # remove excessive blanks in parlist
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
        &subst_vars(*Mcomment);
        if ($Mpreamble == 1) {
          &print_unit(&convert_to_LaTeX($Mcomment));
          next;
        }
        if ($Mignore == 1) { next; }  # no output 
         
        # we are in the standard case and want to output the appropriate stuff
        &print_unit("\@ \\semantics{".&convert_to_LaTeX($Mcomment)."}");

        $_ = <INPUT>;
        while ($_ && /^\s*$/) {  $_ = <INPUT>; }
      
        # we are in the first non-empty line after the manual comment

        if (!(/^\@/ || /^\<\<.*\>\>=/)) # current chunk is continued
        {  print OUTPUT $chunk_name, "\n"; }
      
        goto ground_state; 
      }
      print OUTPUT $_; 
    }
  
close(OUTPUT);

