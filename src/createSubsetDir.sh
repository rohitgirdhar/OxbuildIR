#!/bin/bash

if [ $# -lt 3 ]
then
    echo '/createSubsetdir.sh <input dir path> <output dir path> <fpath of files to select>'
    exit -1
fi
inpdir=$1
outdir=$2
fpath=$3
PREFIX='oxc1_'
POSTFIX='.txt'

mkdir -p $outdir

while read line
do
    ln -s `readlink -f ${inpdir}`/${PREFIX}${line}${POSTFIX} `readlink -f ${outdir}`/${PREFIX}${line}${POSTFIX}
done < $fpath
