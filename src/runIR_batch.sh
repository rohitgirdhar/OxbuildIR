#!/bin/bash
if [ $# -lt 3 ]
then
    echo 'usage: ./run.sh <query imgs list file> ' \
        '<input dir, with the invIndex.txt etc files ' \
        '<output dir>'
    exit -1
fi
source config.sh
Q=$1
IP_DIR=$2
mkdir -p $2
./getClosest -d $WORDS_FILE -i ${IP_DIR}/${INV_IDX}  -k 5062 \
-q -1 -c ${IP_DIR}/${INV_IDX_FREQ} -s ${IP_DIR}/${IMG_STATS} -o $3 -f ${Q}
