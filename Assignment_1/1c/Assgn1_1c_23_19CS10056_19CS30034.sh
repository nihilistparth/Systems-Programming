#!/bin/bash

exs=$(find ./data -type f | sed -e 's/.*\.//' | sort -u)
for var in ${exs//,/ }
do 
    if [[ ${var:0:1} != "/" ]]; then 
        mkdir -p "${var}"
        find . -name "*.${var}" -exec mv --target-directory="${var}/" '{}' +
    fi
    
done
mkdir -p "Nil"
find data/* -type f -print0 | xargs -0 mv -t "Nil"