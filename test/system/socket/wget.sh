#!/bin/sh 

#/usr/bin/wget  -O 1064.697.png 'https://2.base.maps.ls.hereapi.com/maptile/2.1/maptile/newest/normal.day/11/1064/697/256/png?apiKey=ruvQJwbg7zAc4aNzW9FEKbejw-QBnS4u6MsEsqWmBBQ'

url='https://2.base.maps.ls.hereapi.com/maptile/2.1/maptile/newest/normal.day/11/1064/697/256/png?apiKey=ruvQJwbg7zAc4aNzW9FEKbejw-QBnS4u6MsEsqWmBBQ'

echo
echo $url
echo

./wget "$url" > x.png


#openssl s_client -connect 2.base.maps.ls.hereapi.com:443 -tls1_3
