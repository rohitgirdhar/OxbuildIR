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

#define INV_IDX_FNAME "invIndex.txt"
#define INV_IDX_FREQ_FNAME "invIndexFreq.txt"
#define IMG_STATS_FNAME "imgStats.txt"

using namespace std;

void addToIndex(const map<int,int>& descs, string img_idx, map<int, map<string, int> > &invIdx) {
    for (auto iter = descs.begin(); iter != descs.end(); iter++) {
        if (invIdx.count(iter->first) <= 0) {
            // not exists
            invIdx[iter->first] = map<string, int>();
        }
        invIdx[iter->first][img_idx] = iter->second;
    }
}

int main(int argc, char *argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "produce this help message")
        ("dir,d", po::value<string>()->required(), "directory with oxc1 files")
        ("output,o", po::value<string>()->required(), "Output files dir [MUST EXIST]")
        ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch(po::error& e) {
        cout << e.what() << endl;
        cout << desc << endl;
        return -1;
    }

    if (vm.count("help")) {
        cout << desc << endl;
        return -1;
    }
    string dir = vm["dir"].as<string>();
    
    map<int, map<string, int> > invIdx;
    map<string, pair<int,int> > img2total_max;
    fs::recursive_directory_iterator rdi(dir), end;
    string img_fpath, img_fname;
    int count = 0;
    while (rdi != end) {
        img_fpath = (*rdi).path().string();
        img_fname = (*rdi).path().filename().string();
        map<int,int> descs;
        readDescriptorsWithCounts(img_fpath, vector<float>(), descs);
        int total_descs = 0, max_descs = -1;
        for(auto iter = descs.begin(); iter != descs.end(); ++iter) {
            total_descs += iter->second;
            max_descs = max(iter->second, max_descs);
        }
        string img_idx = fpathToIdx(img_fname);
        addToIndex(descs, img_idx, invIdx);
        img2total_max[img_idx] = make_pair(total_descs, max_descs);
        rdi++; count++;
        cout << "Done for " << img_fname << " (" << count << ")"<< endl;
    }
    dumpToFileInvIndex(vm["output"].as<string>() + "/" + INV_IDX_FNAME,
            vm["output"].as<string>() + "/" + INV_IDX_FREQ_FNAME,
            invIdx);
    // Write a binary index (for experiment sake)
    dumpToBinFileInvIndex(vm["output"].as<string>() + "/" + "binFileSize.txt",
            invIdx);
    dumpToFileImgStats(vm["output"].as<string>() + "/" + IMG_STATS_FNAME,
            img2total_max);
                
    return 0;
}
