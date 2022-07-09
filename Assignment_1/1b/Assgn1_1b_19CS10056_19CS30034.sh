#!/bin/bash
mkdir -p 1.b.files.out
for fn in 1.b.files/*.txt; do
	sort -n ${fn} > 1.b.files.out/${fn:10}
done
cat 1.b.files.out/*.txt > t.txt
(sort -n t.txt | uniq -c | awk '{ t=$1 ; $1=$2; $2=t; print }') > 1.b.files.out/1.b.files.out.txt
rm t.txt