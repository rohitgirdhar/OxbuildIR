#include <cstdio>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "IndexUtils.hpp"
namespace fs = boost::filesystem;
namespace po = boost::program_options;

using namespace std;

void addToIndex(set<int> descs, string img_idx, map<int, vector<string> > &invIdx) {
    for (auto iter = descs.begin(); iter != descs.end(); iter++) {
        if (invIdx.count(*iter) <= 0) {
            // not exists
            invIdx[*iter] = vector<string>();
        }
        invIdx[*iter].push_back(img_idx);
    }
}

/**
 * Output format:
 * line i : [.. list of image IDs that have descriptor i..] 
 */
void dumpToFile(string output_fpath,
        string output_idf_fpath,
        map<int, vector<string> > invIdx) {
    ofstream fout, fout_idf;
    fout.open(output_fpath.c_str(), ios::out);
    fout_idf.open(output_idf_fpath.c_str(), ios::out);
    for (auto iter = invIdx.begin();
            iter != invIdx.end(); ++iter) {
        // fout << iter->first << " "; // not writing the word ID
        fout_idf << (iter->second).size() << endl; 
        for (auto iter2 = iter->second.begin(); 
                iter2 != iter->second.end(); ++iter2) {
            fout << *iter2 << " ";
        }
        fout << endl;
    }
    fout.close();
    fout_idf.close();
}

int main(int argc, char *argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "produce this help message")
        ("dir,d", po::value<string>()->required(), "directory with oxc1 files")
        ("output,o", po::value<string>()->required(), "Output file")
        ("output-counts,c", po::value<string>()->required(), "Output IDF counts file")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return -1;
    }
    string dir = vm["dir"].as<string>();
    
    map<int, vector<string> >invIdx;
    fs::recursive_directory_iterator rdi(dir), end;
    string img_fpath, img_fname;
    int count = 0;
    while (rdi != end) {
        img_fpath = (*rdi).path().string();
        img_fname = (*rdi).path().filename().string();
        set<int> descs = readDescriptors(img_fpath);
        string img_idx = fpathToIdx(img_fname);
        addToIndex(descs, img_idx, invIdx);
        rdi++; count++;
        cout << "Done for " << img_fname << " (" << count << ")"<< endl;
    }
    dumpToFile(vm["output"].as<string>(),
            vm["output-counts"].as<string>(),
            invIdx);
    return 0;
}
