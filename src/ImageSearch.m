function ImageSearch(dpath, query_fpath, vocab_size, invIdxFPath)
qvec = genVecFromImg(query_fpath, vocab_size);
lines = textread(invIdxFPath, '%s', 'delimiter', '\n');
relLines = lines(qvec(qvec > 0));
clear lines;
relImgs = [];
for i = 1 : length(relLines)
    imgIds = strsplit(relLines{i});
    relImgs = union(relImgs, imgIds);
end
scores = zeros(1, numel(relImgs));
i = 0;
for relImg = relImgs'
    i = i + 1;
    if length(relImg{:}) == 0 % to ignore the '' string
        continue;
    end
    fpath = fullfile(dpath, strcat(relImg{:}, '.txt'));
    tvec = genVecFromImg(fpath, vocab_size);
    scores(i) = dot(tvec, qvec);
end
scores
