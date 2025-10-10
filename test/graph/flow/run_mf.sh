#!/bin/sh 

window_label() {
# printf "\e]2;MAXFLOW        %s" "$1"
echo -n \]2\;"MAXFLOW        $1"
}


run_mf() {

# $1: name 
# $2: program 
# $3: net file (path)
# $4: time sum (reference)

# $5: filter (mf_rand, mf_sort, ...)

  declare -n x4=$4

  printf "%-6s" $1

  if [ "$5" != "" ]; then
    cat $3 | $5 | $2  | egrep "^time:" > mf.tmp
  else
    cat $3 | $2  | egrep "^time:" > mf.tmp
  fi


  if [ $status ]; then 
     break;
  fi

  cat mf.tmp 
  t=`cat mf.tmp | sed -e "s/.*time://" -e "s/f =.*//"`

  if [ "$t" != "" ]; then
    x4=`echo "$x4 + $t" | bc`
  fi
}



run_hi_pr() {

# $1: name 
# $2: program 
# $3: net
# $4: time sum (reference)

# $5: filter (mf_rand, mf_sort, ...)

  declare -n x4=$4

  printf "%-6s" $1 

  if [ "$5" != "" ]; then
    cat $3 | $5 | $2 > mf.tmp
  else
    cat $3 | $2 > mf.tmp
  fi

  if [ $status ]; then 
     break;
  fi

  t=`cat mf.tmp | grep time     | sed s/"c time: *"//`
  f=`cat mf.tmp | grep flow     | sed s/"c flow: *"//`
  p=`cat mf.tmp | grep pushes   | sed s/"c pushes: *"//`
  r=`cat mf.tmp | grep relabels | sed s/"c relabels: *"//`
  u=`cat mf.tmp | grep updates  | sed s/"c updates: *"//`

  printf "time: %5.2f  f = %.0f  pushes:%8d  relabels:%8d  updates:%3d\n" \
          $t $f $p $r $u

  if [ "$t" != "" ]; then
    x4=`echo "$x4 + $t" | bc`
  fi
}


if [ "$1" == "" ]; then
# read network files from server
  get="wget -q -O -" 
  net_dir=chomsky.uni-trier.de/leda/networks/mf
  net_files=`wget -q -O - $net_dir/lst`
  echo "net_files: " $net_files
else
# read network files from local dir $1
  get=cat
  net_dir=$1
  net_files=`ls -1 $net_dir`
fi


echo
echo "We compare the running times of LEDA's Maxflow implementation (mf) and"
echo "Goldbergs's hi_pr code (hipr). A summary of the total used cpu time will" 
echo "be displayed in the title bar."
echo
echo "Press <return> to continue or <escape> to quit."
read x

if [ "$x" = "" ]; then
  exit
fi



sum1=0.0
sum2=0.0

for f in  $net_files; do 
  echo " "
  echo $f

  $get $net_dir/$f > mf.net

  run_mf leda max_flow  mf.net sum1
  run_mf leda max_flow  mf.net sum1 mf_rand

  run_hi_pr hipr hi_pr mf.net sum2
  run_hi_pr hipr hi_pr mf.net sum2 mf_rand

  total=0.0

  for x in $sum1 $sum2 $sum3 $sum4 $sum5; do
   if [ "$x" != "" ]; then
    total=`echo "$total + $x" | bc`
   fi
  done

  window_label \
   "`printf 'leda: %-8.2f hipr: %-8.2f  total: %.2f sec' $sum1 $sum2 $total`"

done


echo
printf 'leda: %-8.2f hipr: %-8.2f  total: %.2f sec\n' $sum1 $sum2 $total
echo

