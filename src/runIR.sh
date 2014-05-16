#!/bin/bash
if [ $# -lt 1 ]
then
    echo 'usage: ./run.sh <query img index>'
    exit -1
fi
Q=$1
tmpfile=`mktemp`
./getClosest -d ../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/ -i invIndex.txt  -k 20 -q $Q > $tmpfile
bash matlab_batcher.sh RerankMatching \'../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/\',\'../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/${Q}.txt\',1000000,\'invIndex.txt\',\'invIndexFreq.txt\',\'${tmpfile}\',\'res.txt\'
rm $tmpfile
