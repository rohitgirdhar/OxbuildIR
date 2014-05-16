#!/bin/bash
if [ $# -lt 1 ]
then
    echo 'usage: ./run.sh <query img index>'
    exit -1
fi
Q=$1
./getClosest -d ../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/ -i invIndex2.txt  -k 5062 -q $Q -c invIndexFreq2.txt \
    -s imgStats.txt
# bash matlab_batcher.sh RerankMatching \'../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/\',\'../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/${Q}.txt\',1000000,\'invIndex.txt\',\'invIndexFreq.txt\',\'${tmpfile}\',\'res.txt\'
