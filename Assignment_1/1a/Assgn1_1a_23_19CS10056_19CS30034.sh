#!/bin/bash
a=$1
for (( c=2; c<=$a; c++ ))
do  
   	while (( $a % $c == 0 )) 
	do
     	echo -n $c" "
     	tmp=$((a / c))
     	a=tmp
	done
done
echo ""
