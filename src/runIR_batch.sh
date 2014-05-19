#!/bin/bash
if [ $# -lt 1 ]
then
    echo 'usage: ./run.sh <query imgs list file> <output dir>'
    exit -1
fi
Q=$1
mkdir -p $2
./getClosest -d ../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/ -i invIndex2.txt  -k 5062 -q -1 -c invIndexFreq2.txt \
    -s imgStats.txt -o $2 -f ${Q}
