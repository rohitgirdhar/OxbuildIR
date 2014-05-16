function RerankMatching(dpath, query_fpath, vocab_size, invIdxFPath, IDF_fpath, curRes_fpath, output_fpath)
num_files = length(dir(dpath));
qvec = genVecFromImg(query_fpath, vocab_size, IDF_fpath, num_files);
relImgs = textread(curRes_fpath, '%s');
scores = zeros([1 numel(relImgs)]);
i = 0;
for relImg = relImgs'
    i = i + 1;
    if length(relImg{:}) == 0 % to ignore the '' string
        continue;
    end
    fpath = fullfile(dpath, strcat(relImg{:}, '.txt'));
    tvec = genVecFromImg(fpath, vocab_size, IDF_fpath, num_files);
    scores(i) = dot(tvec, qvec);
end
[scores, order] = sort(scores, 'descend');
relImgs = relImgs(order);
fout = fopen(output_fpath, 'w');
for i = 1 : numel(relImgs)
    fprintf(fout, '%s\n', relImgs{i});
end
fclose(fout);
