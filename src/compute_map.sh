#!/bin/bash

HELP_TEXT="Usage ./compute_map.sh -g <path to GT directory>
    -e <path to compute_map executable> 
    -i <input dir, with invIndex.txt etc>
    [-r (set flag to re-use output dir already created. Useful for testing changes to compute_p) ]
    [-s <Path to file with names of selected images, to use with compute_ap> (optional)]
    [<K : precision at K. Use only with using compute_p. Otherwise (for AP) use #images> ...] (multiple)"

while getopts ":g:e:i:s:r" opt; do
    case "$opt" in 
        g) GT_DIR=${OPTARG} ;;
        e) COMPUTE_AP_EXEC=${OPTARG} ;;
        i) IP_DIR=${OPTARG} ;;
        s) SEL_LIST=${OPTARG} ;;
        r) REUSE_OUTPUT_DIR=1 ;;
        \?) echo $HELP_TEXT >&2
            exit -1
            ;;
    esac
done

shift $(( OPTIND - 1 ))

K=$@ # all the rest is K values
if [ -z "$K" ]; then
    K="5062"
    echo "setting K=${K} since not supplied"
fi
TMP_FILE=`mktemp`
TMP_Q_FILE=`mktemp`
#TMP_DIR=`mktemp -d`
if [ -z $SEL_LIST ]
then
    TMPDIR_NAME="full"
else
    TMPDIR_NAME=`basename ${SEL_LIST} .txt`
fi
TMP_DIR=/tmp/user/1000/oxbuild_exp/${TMPDIR_NAME}/
mkdir -p $TMP_DIR
tot=0
count=0

echo -n 'Making list of query in' $TMP_FILE '...'
for file in `ls ${GT_DIR}/*_query.txt`
do
    qimg_with_bounds=`cat $file`
    qimg=`echo $qimg_with_bounds | cut -f1 -d' '`
    fbase=`basename $file | sed "s/..........$//"`
    echo $qimg_with_bounds | cat >> $TMP_FILE
    echo $fbase | cat >> $TMP_Q_FILE
done
echo 'Done'

if [ -d $TMP_DIR ] && [ $REUSE_OUTPUT_DIR ]; then
    echo "NOT COMPUTING IR. Reusing $TMP_DIR"
else
    echo 'Writing output into' $TMP_DIR
    K_arr=($K)
    IFS=$'\n'
    MAX_K=`echo "${K_arr[*]}" | sort -nr | head -n1`
    echo "MAX K = ${MAX_K} out of ${K}"
    bash runIR_batch.sh $TMP_FILE $IP_DIR $TMP_DIR $MAX_K
    echo 'Done'
fi

## CREATE the smaller files (_k)
echo "Creating the smaller files for evaluation"
for k in ${K_arr[@]}; do
    while read line
    do
        qimg_with_bounds=$line
        qimg=`echo $qimg_with_bounds | cut -f1 -d' '`
        head -n${k} ${TMP_DIR}/${qimg}.out > ${TMP_DIR}/${qimg}_${k}.out
    done < $TMP_FILE
done

for k in ${K_arr[@]}; do
    echo "---AP values (at K= ${k})---"
    while read line && read -u 3 line2
    do
        fbase=$line
        qimg_with_bounds=$line2
        qimg=`echo $qimg_with_bounds | cut -f1 -d' '`
        ap_o=`${COMPUTE_AP_EXEC} ${GT_DIR}/${fbase} ${TMP_DIR}/${qimg}_${k}.out ${SEL_LIST}`
        ap=`echo ${ap_o} | sed -e 's/[eE]+*/\\*10\\^/'` # to handle scientific notation for bc
        echo $qimg $ap_o '=' $ap
        # IGNORE NAN values
        if [ $ap = "-nan" ] || [ $ap = "-1" ]
        then
            continue
        fi
        tot=$(bc <<< "scale=2;$tot+$ap")
        count=`expr $count + 1`
    done < $TMP_Q_FILE 3< $TMP_FILE

    echo -n 'mAP: '
    bc <<< "scale=2;$tot/$count"
done
#rm -rf $TMP_FILE $TMP_DIR $TMP_Q_FILE
