#!/bin/bash
_V=0

while getopts "v" OPTION
do
  case $OPTION in
    v) _V=1
       ;;
  esac
done

function log () {
    if [[ $_V -eq 1 ]]; then
        echo "$@"
    fi
}

log "Setting the request headers......."

export REQ_HEADERS="Host,Accept,traceparent,ip"


log "Getting the example page........."
curl -o example.html https://www.example.com

log "Getting the ip........."
curl -v http://ip.jsontest.com/
log "Getting the header list........."
curl -I http://www.jsontest.com/

log "Getting the request from required headers........."

for var in ${REQ_HEADERS//,/ }
do
    tp=$(curl -G http://headers.jsontest.com/ | jq -c ".${var}")
    
    if [ $tp == "null" ] ;then
        echo "Not found"
    else
        echo $var " : " $tp
    fi
done

log "validating the json files in the current directory........"

echo -n > valid.txt
echo -n > invalid.txt

find ./ -print0 | while IFS= read -r -d '' fn
do
    if [[ -f "$fn" ]]; then
      ba=${fn%.*}
      ext=${fn#$ba.}
        if [[ $ext == "json" ]]; then
            tp=$(curl -d "json=`cat $fn`" -X POST http://validate.jsontest.com | jq -c ".validate") ;
            if [[ $tp == "true" ]]; then
                echo ${fn} >> valid.txt
            else
                echo ${fn} >> invalid.txt
            fi
        fi
    fi
done