#!/bin/bash

if [ $# -lt 1 ]
then
    echo 'usage ./prog <selected file with names path>'
    exit -1
fi

rm subset_dir/*
bash createSubsetDir.sh ../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/ subset_dir/ ${1}
INDEX_DIR=indexes/`basename ${1} .txt`
mkdir -p $INDEX_DIR
./buildIndex -o $INDEX_DIR -d subset_dir/
bash  compute_map.sh -g ../oxford_building_dataset/gt/ -e ../oxford_building_dataset/compute_p -i $INDEX_DIR -s ${1} 1 3 5 10
