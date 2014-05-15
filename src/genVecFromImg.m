function S = genVecFromImg(img_fpath, vocab_size)
% Read the oxc1 file of image and create a sparse vector
img_features = dlmread(img_fpath);
img_features = img_features(3:end, 1);
nfeatures = length(img_features);
S = sparse(ones(1, nfeatures), img_features', ones(1, nfeatures), 1, vocab_size);
