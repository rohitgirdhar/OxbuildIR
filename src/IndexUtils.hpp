#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

set<int> readDescriptors(string fpath) {
    set<int> descs;
    ifstream fin(fpath.c_str());
    if (!fin.is_open()) {
        cerr << "Unable to open file: " << fpath << endl;
        return set<int>();
    }
    string line;
    getline(fin, line); getline(fin, line);
    int desc;
    while (getline(fin, line)) {
        istringstream iss(line);
        iss >> desc;
        descs.insert(desc);
    }
    fin.close();
    return descs;
}

vector<int> readAllDescriptors(string fpath) {
    vector<int> descs;
    ifstream fin(fpath.c_str());
    if (!fin.is_open()) {
        cerr << "Unable to open file: " << fpath << endl;
        return vector<int>();
    }
    string line;
    getline(fin, line); getline(fin, line);
    int desc;
    while (getline(fin, line)) {
        istringstream iss(line);
        iss >> desc;
        descs.push_back(desc);
    }
    fin.close();
    return descs;
}

bool isInside(float x, float y, vector<float> bounding_box) {
    if (bounding_box.size() == 0) return true;
    float qx1 = bounding_box[0];
    float qy1 = bounding_box[1];
    float qx2 = bounding_box[2];
    float qy2 = bounding_box[3];
    if (x < qx1 || x > qx2 || y < qy1 || y > qy2) return false;
    return true;
}

map<int, int> readDescriptorsWithCounts(string fpath,
        vector<float> bounding_box) {
    if (bounding_box.size() == 4) {
        cerr << "Using bounding box to filter "<< fpath << endl;
    }
    map<int,int> descs;
    ifstream fin(fpath.c_str());
    if (!fin.is_open()) {
        cerr << "Unable to open file: " << fpath << endl;
        return map<int,int>();
    }
    string line;
    getline(fin, line); getline(fin, line);
    int desc;
    float x, y;
    while (getline(fin, line)) {
        istringstream iss(line);
        iss >> desc >> x >> y;
        if (!isInside(x, y, bounding_box)) continue; 
        if (descs.count(desc) <= 0) {
            descs[desc] = 0;
        }
        descs[desc]++;
    }
    fin.close();
    return descs;
}

map<int, vector<pair<float,float> > > readDescriptorsWithPos(string fpath,
        vector<float> bounding_box) {
    if (bounding_box.size() == 4) {
        cerr << "Using bounding box to filter "<< fpath << endl;
    }
    map<int, vector<pair<float,float> > > descs;
    ifstream fin(fpath.c_str());
    if (!fin.is_open()) {
        cerr << "Unable to open file: " << fpath << endl;
        return descs;
    }
    string line;
    getline(fin, line); getline(fin, line);
    int desc;
    float x, y;
    while (getline(fin, line)) {
        istringstream iss(line);
        iss >> desc >> x >> y;
        if (!isInside(x, y, bounding_box)) continue; 
        if (descs.count(desc) <= 0) {
            descs[desc] = vector<pair<float,float> >();
        }
        descs[desc].push_back(make_pair(x,y));
    }
    fin.close();
    return descs;
}

string fpathToIdx(string fpath) {
    return fpath.substr(0, fpath.size() - 4); 
}

template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
                   flip_pair<A,B>);
    return dst;
}

/**
 * Reads the image file and computes TF
 * @return Pair<freq of vis word, total # of visual words
 */
pair<int,int> getWordCounts(int visualWord, string img_id, string dir) {
    static map<string, vector<int> > vws_rem;
    if (vws_rem.count(img_id) <= 0) {
        vws_rem[img_id] = readAllDescriptors(dir + "/" + img_id + ".txt");
    }
    vector<int> vws = vws_rem[img_id];
    return make_pair(count(vws.begin(), vws.end(), visualWord), (int)vws.size());
}

/**
 * @param vws The visual words set of the query image
 * @param N The number of images to search from
 */
vector<pair<string, float> > getClosestImgs(
        map<int,int> vws, 
        string dir,
        map<int, map<string, int> > invIdx,
        map<string, pair<int, int> > imgStats) {
    map<string, double> wordCounts;
    auto iter = vws.begin();
    int total_vw_in_query = accumulate(begin(vws), 
           end(vws), 
           0, 
           [](const int previous, const pair<int,int>& p) { 
           return previous+p.second; });
    while (iter != vws.end()) {
        map<string, int> imgs_tfs = invIdx[iter->first];
        for (auto iter2 = imgs_tfs.begin(); iter2 != imgs_tfs.end(); ++iter2) {
            string img = iter2->first;
            if (img.length() == 0) { // discard empty strings
                continue;
            }
            if (wordCounts.count(img) <= 0) {
                wordCounts[img] = 0;
            }
            double tf = 0.5f + (0.5f + imgs_tfs[img] / imgStats[img].second);
            //double tf = min(iter->second, imgs_tfs[img]) * 1.0 / max( imgStats[img].first, total_vw_in_query);
            double idf = imgStats.size() * 1.0 / imgs_tfs.size();
            //double idf = log10(imgStats.size() * 1.0 / imgs_tfs.size());
            wordCounts[img] += tf * idf;
        }
        iter++;
    }
    multimap<double, string> countsToImgs = flip_map(wordCounts);
    vector<pair<string, float> > res;
    for (auto iter2 = countsToImgs.rbegin(); 
            iter2 != countsToImgs.rend();
            ++iter2) {
        res.push_back(make_pair(iter2->second, iter2->first));
    }
    return res;
}

/**
 * Output format:
 * line i : [.. list of image IDs that have descriptor i:<TF>..] 
 */
void dumpToFileInvIndex(string output_fpath,
        string output_idf_fpath,
        map<int, map<string, int> > invIdx) {
    ofstream fout, fout_idf;
    fout.open(output_fpath.c_str(), ios::out);
    fout_idf.open(output_idf_fpath.c_str(), ios::out);
    int prev_kpt_printed = 1;
    for (auto iter = invIdx.begin();
            iter != invIdx.end(); ++iter) {
        // leave lines blank for ids that don't exist
        for (int i = 0; i < (iter->first - prev_kpt_printed); i++) fout << endl;

        fout_idf << (iter->second).size() << endl; 
        for (auto iter2 = iter->second.begin(); 
                iter2 != iter->second.end(); ++iter2) {
            fout << iter2->first << ":" << iter2->second << " ";
        }
        fout << endl;
        prev_kpt_printed = iter->first + 1;
    }
    fout.close();
    fout_idf.close();
}

map<int, map<string, int> > readFromFileInvIndex(string fpath) {
    ifstream fin(fpath.c_str());
    string line;
    vector<string> imgs;
    map<int, map<string, int> > res;
    int lineno = 0;
    while (getline(fin, line)) {
        lineno++;
        boost::split(imgs, line, boost::is_any_of(" "));
        map<string, int> desc_info;
        for (int i = 0; i < imgs.size(); i++) {
            string img = imgs[i];
            if (img.length() == 0) continue;
            int pos = img.find(':');
            desc_info[img.substr(0, pos)] = stoi(img.substr(pos + 1));
        }
        res[lineno] = desc_info;
    }
    fin.close();
    return res;
}

void dumpToFileImgStats(string fname, map<string, pair<int,int> > img2total_max) {
    ofstream fout(fname.c_str(), ios::out);
    for (auto iter = img2total_max.begin(); iter != img2total_max.end(); ++iter) {
        fout << iter->first << " " << (iter->second).first << " " << (iter->second).second << endl;
    }
    fout.close();
}

map<string, pair<int,int> > readFromFileImgStats(string fpath) {
    ifstream fin(fpath.c_str());
    string img_id;
    int tot, mx;
    map<string, pair<int,int> > res;
    while (fin >> img_id >> tot >> mx) {
        res[img_id] = make_pair(tot, mx);
    }
    fin.close();
    return res;
}

int countFInliers(map<int, vector<pair<float, float> > > vws1,
        map<int, vector<pair<float, float> > > vws2) {
    vector<cv::Point2f> pts1, pts2;
    for (auto iter = vws2.begin(); iter != vws2.end(); ++iter) {
        if (vws1.count(iter->first) <= 0) continue;
        for (auto psns1 = begin(iter->second); 
                psns1 != end(iter->second); ++psns1) {
            for (auto psns2 = vws1[iter->first].begin(); 
                    psns2 != vws1[iter->first].end(); ++psns2) {
                pts1.push_back(cv::Point2f(psns1->first, psns1->second));
                pts2.push_back(cv::Point2f(psns2->first, psns2->second));
            }
        }
    }
    vector<uchar> mask;
    cv::findFundamentalMat(pts1, pts2, cv::RANSAC, 3, 0.99, mask);
    return count_if(mask.begin(), mask.end(), [](uchar d){ return d; });
}

/**
 * @param TAU ignore the inliers if # < TAU
 */
void geometricReranking(vector<pair<string, float> > &rankedList,
        const map<int, vector<pair<float,float> > > &vws,
        const string &dir,
        int TAU) {
    int K = 100;
    cerr << "Geometrical Reranking of first " << K << " elements" << endl;
    vector<pair<int,int> > num_inliers;
    for (int i = 0; i < min(K, (int) rankedList.size()); i++) {
        auto vws2 = readDescriptorsWithPos(
                dir + "/" + rankedList[i].first + ".txt",
                vector<float>());
        int inliers = countFInliers(vws, vws2);
        if (inliers < TAU) inliers = 0;
//        cerr << "got inliers: " << rankedList[i].first << " " << inliers << endl;
        num_inliers.push_back(make_pair(inliers, i));
    }
    // sort in descending order of first element and ascending or second 
    sort(num_inliers.begin(), 
            num_inliers.end(),
            [](const pair<int,int> a, const pair<int,int> b) {
                if (a.first != b.first)
                    return a.first > b.first; // sort only on first
                else 
                    return a.second < b.second;
            });
    // re-rank
    int i = 0;
    vector<pair<string, float> > res;
    vector<bool> done(rankedList.size(), false);
    for (auto iter = num_inliers.begin(); 
            iter != num_inliers.end() && iter->first > 0; ++iter, ++i) {
        res.push_back(rankedList[iter->second]);
        done[iter->second] = true;
    }
    cerr << "Reranked " << count_if(done.begin(), 
            done.end(),
            [](bool d) { return d; }) << endl;
    for (int i = 0; i < rankedList.size(); i++) {
        if (! done[i]) {
            res.push_back(rankedList[i]);
        }
    }
    rankedList = res;
}
