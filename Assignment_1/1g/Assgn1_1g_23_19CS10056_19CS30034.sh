#!/bin/bash
echo -n "Name of the file: "
read -r file

echo -n > "$file"

for ((c=1; c<=150;c++))
do 
    for ((i=1; i<=9; i++))
    do
        var=$(shuf -i 0-69420 -n 1)
        echo -n "$var, " >> "$file"        
    done
    var=$(shuf -i 0-69420 -n 1)
    echo -n "$var" >> "$file"
    echo >> "$file"
done

echo -n "Column Number: "
read -r col_num

echo -n "Regular Expression: "
read -r regl

for ((c=1; c<=150; c++))
do
    arr=( $(cut -d ',' -f${col_num} "$file" | tail -n +${c}))
    if [[ $arr =~ $regl ]] ; then
        echo "YES"
        exit
    fi
done

echo "NO"