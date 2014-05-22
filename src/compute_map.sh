#!/bin/bash

if [ $# -lt 2 ]
then
    echo 'Usage ./compute_map.sh <path to GT directory> <path to compute_map executable>'
    exit -1
fi

COMPUTE_AP_EXEC=$2
TMP_FILE=`mktemp`
TMP_Q_FILE=`mktemp`
TMP_DIR=`mktemp -d`
tot=0
count=0

echo -n 'Making list of query in' $TMP_FILE '...'
for file in `ls ${1}/*_query.txt`
do
    qimg=`cat $file | cut -f1 -d' '`
    fbase=`basename $file | sed "s/..........$//"`
    echo $qimg | cat >> $TMP_FILE
    echo $fbase | cat >> $TMP_Q_FILE
done
echo 'Done'

echo 'Writing output into' $TMP_DIR
bash runIR_batch.sh $TMP_FILE $TMP_DIR
echo 'Done'

echo '---AP values---'
while read line && read -u 3 line2
do
    fbase=$line
    qimg=$line2
    ap=`${COMPUTE_AP_EXEC} ${1}/${fbase} ${TMP_DIR}/${qimg}.out`
    echo $qimg $ap
    tot=$(bc <<< "scale=2;$tot+$ap")
    count=`expr $count + 1`
done < $TMP_Q_FILE 3< $TMP_FILE

echo -n 'mAP: '
bc <<< "scale=2;$tot/$count"

rm -rf $TMP_FILE $TMP_DIR $TMP_Q_FILE
