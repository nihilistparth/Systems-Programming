#!/bin/bash
awk -v column=$2 '{print tolower($column)}' $1 |sort |uniq -c | awk '{print $2,$1}' > tmp.txt 
sort -k 2nr tmp.txt > 1c_output_$2_column.freq
rm tmp.txt 