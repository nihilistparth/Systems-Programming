#!/bin/bash
mkdir -p files_mod
cat -n $1 | awk '{$1=$1};1' | sed -e 's/\s\+/,/g' > files_mod/$1