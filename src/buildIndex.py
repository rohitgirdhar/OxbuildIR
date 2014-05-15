#!/usr/bin/python2.7

import argparse
import os
from multiprocessing import Pool, Lock
import sys
import numpy as np

def genIndex(img_id):
    # invIndex = { <feature_id> : [ .. list of images .. ] }
    invIndex = {}
    fpath = os.path.join(dirpath, 'oxc1_' + img_id + '.txt')
    img_features = np.genfromtxt(fpath, dtype=float, delimiter=' ',
            skip_header=2)
    img_features = img_features[:,0]
    for i in range(np.shape(img_features)[0]):
        ftr = int(img_features[i])
        if ftr in invIndex.keys():
            invIndex[ftr].append(img_id)
        else:
            invIndex[ftr] = [img_id]
    return invIndex

def writeToFile(invIndex, outf):
    f = open(outf, "w")
    for key in invIndex.keys():
        print >> f, key,
        for img in invIndex[key]:
            print >> f, img,
        print >> f, '\n',
    f.close()

def main():
    parser = argparse.ArgumentParser(description='Build inverted index')
    parser.add_argument('-d', '--dirpath', nargs=1, required=True, help='Input directory path with oxc1 files')
    parser.add_argument('-o', '--output-fname', nargs=1, required=True, help='Output Filename')
    parser.add_argument('-i', '--input-fname', nargs=1, required=True, 
            help="Input file with image IDs")
    parser.add_argument('-j', type=int, default=4, required=False, help="# of threads to use")

    args = parser.parse_args()
    f_imgs = open(args.input_fname[0]);
    nProcs = args.j
    global dirpath
    dirpath = args.dirpath[0]

    files = []
    for line in f_imgs:
        files.append(line.strip())
    f_imgs.close()

    p = Pool(nProcs)
    invIndexes = p.map(genIndex, files)
    
    cumInvIndex = {}
    for ind in invIndexes:
        for key in ind.keys():
            if key in cumInvIndex:
                cumInvIndex[key] += ind[key]
            else:
                cumInvIndex[key] = ind[key]
    print len(cumInvIndex.keys())
    writeToFile(cumInvIndex, args.output_fname[0])

if __name__ == '__main__':
    main()
