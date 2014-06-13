#!/bin/bash
if [ $# -lt 4 ]
then
    echo 'usage: ./run.sh <query imgs list file> ' \
        '<input dir, with the invIndex.txt etc files ' \
        '<output dir> <K: Number of results to get from getClosest>'
    exit -1
fi
source config.sh
Q=$1
K=$4
IP_DIR=$2
mkdir -p $2
./getClosest -d $WORDS_FILE -i ${IP_DIR} -k ${K} \
-q -1 -o $3 -f ${Q} -r -t 15
