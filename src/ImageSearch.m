function ImageSearch(dpath, query_fpath, vocab_size)
files = dir(fullfile(dpath, '*.txt'));
nfiles = length(files);
scores = zeros(1, nfiles);
qvec = genVecFromImg(query_fpath, vocab_size);
i = 1;
for file = files'
    fpath = fullfile(dpath, file.name);
    tvec = genVecFromImg(fpath, vocab_size);
    scores(i) = dot(tvec, qvec);
    i = i + 1
end
scores
