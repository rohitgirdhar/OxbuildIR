#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
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

map<int, int> readDescriptorsWithCounts(string fpath) {
    map<int,int> descs;
    ifstream fin(fpath.c_str());
    if (!fin.is_open()) {
        cerr << "Unable to open file: " << fpath << endl;
        return map<int,int>();
    }
    string line;
    getline(fin, line); getline(fin, line);
    int desc;
    while (getline(fin, line)) {
        istringstream iss(line);
        iss >> desc;
        if (descs.count(desc) <= 0) {
            descs[desc] = 0;
        }
        descs[desc]++;
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
        set<int> vws, 
        string dir,
        map<int, map<string, int> > invIdx,
        map<string, pair<int, int> > imgStats) {
    map<string, float> wordCounts;
    auto iter = vws.begin();
    while (iter != vws.end()) {
        map<string, int> imgs_tfs = invIdx[*iter];
        for (auto iter = imgs_tfs.begin(); iter != imgs_tfs.end(); ++iter) {
            string img = iter->first;
            if (img.length() == 0) { // discard empty strings
                continue;
            }
            if (wordCounts.count(img) <= 0) {
                wordCounts[img] = 0;
            }
            float tf = 0.5f + (0.5f + imgs_tfs[img] / imgStats[img].first);
            float idf = (imgStats.size() * 1.0f / imgs_tfs.size());
            wordCounts[img] += tf * idf;
        }
        iter++;
    }
    multimap<float, string> countsToImgs = flip_map(wordCounts);
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
