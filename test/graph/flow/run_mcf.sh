#!/bin/sh

window_label() {
#printf "\e]2;MIN-COST FLOW      %s" "$1"
echo -n \]2\;"MIN-COST FLOW        $1"
}

run_mcf() {

# $1: name 
# $2: program 
# $3: options 
# $4: net file
# $5: time sum (reference)

  declare -n x5=$5

  printf "%-9s" $1

  cat $4 | $2 $3 | grep time > mcf.tmp

  if [ $status ]; then 
     break;
  fi

  cat mcf.tmp 

  t=`cat mcf.tmp | sed -e "s/.*time://" -e "s/cost:.*//"`

  if [ "$t" != "" ]; then
    x5=`echo "$x5 + $t" | bc`
  fi
}

run_cs2() {

# $1: name 
# $2: program 
# $3: options 
# $4: net file
# $5: time sum (reference)

 declare -n x5=$5

 printf "%-9s" $1

 cat $4 | $2 | grep time | sed s/"c time: *"/"time:   "/ | sed s/" *cost:     "/"  cost: "/ > mcf.tmp

 cat mcf.tmp 

 t=`cat mcf.tmp | sed -e "s/.*time: *//" -e "s/ *cost:.*//"`

 if [ "$t" != "" ]; then
   x5=`echo "$x5 + $t" | bc`
 fi
}


if [ "$1" == "" ]
then
# read network files from server
  get="wget -q -O -"
  net_dir=chomsky.uni-trier.de/leda/networks/mcf
  net_files=`wget -q -O - $net_dir/lst`
else
# read network files from local dir ($1)
  get=cat
  net_dir=$1
  net_files=`ls -1 $net_dir`
fi


echo
echo "We compare the running times of LEDA's Min-Cost Flow implementation (mcf)"
echo "and Goldbergs's cs2 code (cs2). A summary of the total used cpu time will"
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
 
  $get $net_dir/$f > mcf.net
 
  run_mcf leda mcf_cost_scaling "" mcf.net sum1
 
  run_cs2 cs2-4.5 cs2-4.5 "" mcf.net sum2
 
  line=""
  total=0.0
 
  for x in $sum1 $sum2 $sum3 $sum4 $sum5; do
    if [ $x = "" ]; then
      total=`echo "$total + $x" | bc`
    fi
  done

  window_label \
  "`printf 'leda: %-8.2f  cs2: %-8.2f  total: %.2f sec' $sum1  $sum2 $total`"

done


echo
printf 'leda: %-8.2f  cs2: %-8.2f  total: %.2f sec\n' $sum1  $sum2 $total
echo

