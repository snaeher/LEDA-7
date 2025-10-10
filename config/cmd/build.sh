#!/bin/sh

error_handler() {
 echo
 echo "build.sh line $1: exit (code = $2)";
 echo
}

set -e
trap 'error_handler $LINENO $?' ERR

#-----------------------------------------------------------------------------

dirs="\
src/core \
src/numbers \
src/system \
src/coding \
src/coding/zlib \
src/coding/img \
\
src/geo/d3_geo \
src/geo/plane/float \
src/geo/plane/rational \
src/geo/plane/real \
src/geo/plane_alg/float \
src/geo/plane_alg/rational \
src/geo/plane_alg/real \
\
src/graph/graph_types \
src/graph/graph_alg \
src/graph/graph_draw \
src/graph/graph_iso \
\
src/graphics/anim \
src/graphics/geowin \
src/graphics/window \
src/graphics/graphwin"

if [ $OSTYPE == cygwin ]; then
  dirs="$dirs src/graphics/mswin"
else
  dirs="$dirs src/graphics/x11"
fi


CDPATH=

base_dir=$(pwd)

for x in $dirs; do
 echo $x
 cd $x
 xx=$(printf "%-27s" $x)
 if [ $OSTYPE == cygwin ]; then
  (sleep 2; make -i obj &> make.out; echo $x) &
 else
  (sleep 2; /usr/bin/time -o /dev/tty -f "$xx %e s" make -i obj &>> make.out)&
 fi
 cd $base_dir
done


echo
echo wait for make jobs to finish
echo
wait

echo

rm -f libleda.so libleda.a leda.lib leda.dll *.tag

if [ -f shared.mk ]; then
  make shared
elif [ -f static.mk ]; then
   make static
fi



echo

