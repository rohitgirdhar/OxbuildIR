#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <vector>
#include <map>
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

vector<string> getClosestImgs(set<int> vws, string invIdxFPath) {
    ifstream fin(invIdxFPath.c_str());
    string line;
    map<string, int> wordCounts;
    auto iter = vws.begin();
    int lineno = 0;
    while(getline(fin, line) && iter != vws.end()) {
        lineno++;
        if (lineno != *iter) {
            continue;
        }
        iter++;
        vector<string> imgs;
        boost::split(imgs, line, boost::is_any_of(" "));
        for (string img : imgs) {
            if (img.length() == 0) { // discard empty strings
                continue;
            }
            if (wordCounts.count(img) <= 0) {
                wordCounts[img] = 0;
            }
            wordCounts[img]++;
        }
    }
    multimap<int, string> countsToImgs = flip_map(wordCounts);
    vector<string> res;
    for (auto iter2 = countsToImgs.rbegin(); 
            iter2 != countsToImgs.rend();
            ++iter2) {
        res.push_back(iter2->second);
    }
    return res;
}
