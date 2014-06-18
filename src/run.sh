#!/bin/bash

if [ $# -lt 1 ]
then
    echo 'usage ./prog [-r (reuse the computed results. Only run compute_p/ap again)]
                <selected file with names path>'
    exit -1
fi

while getopts ":r" opt; do
    case "$opt" in
        r) REUSE_RESULTS=1 ;;
    esac
done

shift $(( OPTIND - 1 ))

INDEX_DIR=indexes/`basename ${1} .txt`
if [ ! $REUSE_RESULTS ]; then
    rm subset_dir/*
    bash createSubsetDir.sh ../oxford_building_dataset/word_oxc1_hesaff_sift_16M_1M/ subset_dir/ ${1}
    mkdir -p $INDEX_DIR
    ./buildIndex -o $INDEX_DIR -d subset_dir/
    REUSE_FLAG="-r"
fi
bash  compute_map.sh $REUSE_FLAG -g ../oxford_building_dataset/gt/ -e eval/compute_p -i $INDEX_DIR -s ${1} 1 3 5 10
#bash  compute_map.sh -g ../oxford_building_dataset/gt/ -e ../oxford_building_dataset/compute_ap -i $INDEX_DIR -s ${1}
