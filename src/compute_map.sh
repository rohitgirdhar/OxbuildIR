#!/bin/bash

COMPUTE_AP_EXEC=../oxford_building_dataset/compute_ap
tot=0
count=0
for file in `ls ../oxford_building_dataset/gt/*_query.txt`
do
    qimg=`cat $file | cut -f1 -d' '`
    fbase=`basename $file | sed "s/..........$//"`
    echo $fbase
    bash runIR.sh $qimg > res
    ap=`${COMPUTE_AP_EXEC} ~/work/project/datasets/oxford_building/gt//${fbase} res`
    echo $ap
    tot=$(bc <<< "scale=2;$tot+$ap")
    count=`expr $count + 1`
done
bc <<< "scale=2;$tot/$count"
