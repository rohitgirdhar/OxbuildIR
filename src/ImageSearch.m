function ImageSearch(dpath, query_fpath, vocab_size, invIdxFPath, IDF_fpath)
num_files = length(dir(dpath));
qvec = genVecFromImg(query_fpath, vocab_size, IDF_fpath, num_files);
lines = textread(invIdxFPath, '%s', 'delimiter', '\n');
[r,c,v] = find(qvec); % c gives the columns that are set
relLines = lines(c);
clear lines;
relImgs = [];
for i = 1 : length(relLines)
    imgIds = strsplit(relLines{i});
    relImgs = union(relImgs, imgIds);
end
scores = zeros(1, numel(relImgs));
i = 0;
disp(['To search from : ', numel(relImgs)]);
for relImg = relImgs'
    i = i + 1
    if length(relImg{:}) == 0 % to ignore the '' string
        continue;
    end
    fpath = fullfile(dpath, strcat(relImg{:}, '.txt'));
    tic;
    tvec = genVecFromImg(fpath, vocab_size, IDF_fpath, num_files);
    toc;
    scores(i) = dot(tvec, qvec);
end
scores
