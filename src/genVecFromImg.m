function S = genVecFromImg(img_fpath, vocab_size, IDF_fpath, N)
% Read the oxc1 file of image and create a sparse vector
img_features = dlmread(img_fpath);
img_features = img_features(3:end, 1);
nfeatures = length(img_features);
S = sparse(ones(1, nfeatures), img_features', ones(1, nfeatures), 1, vocab_size);
max_tf = full(max(S));

% Set as TF x IDF
persistent word_freqs;
if isempty(word_freqs)
    disp('Reading word frequency file...');
    word_freqs = dlmread(IDF_fpath);
end
[r, c, v] = find(S);
for i = c
    S(i) = 0.5 + (0.5 * S(i) / max_tf); % TF
    S(i) = S(i) * log(N / word_freqs(i)); % IDF
end
