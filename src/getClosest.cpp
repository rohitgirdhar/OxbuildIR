#include <cstdio>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>
#include "IndexUtils.hpp"
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
namespace fs = boost::filesystem;
namespace po = boost::program_options;

using namespace std;

#define INV_IDX_FNAME "invIndex.txt"
#define IMG_STATS_FNAME "imgStats.txt"

void runSearch(string dir,
        string query,
        map<int, map<string, int> > &invIdx,
        map<string, pair<int, int> > &imgStats,
        vector<pair<string, float> > &outputIDs) {
    int start_time = clock();
    set<int> vws = readDescriptors(dir + "/" +
            query + ".txt");
    outputIDs = getClosestImgs(
            vws,
            dir,
            invIdx,
            imgStats);
    cerr << "Time in search " << clock() - start_time << " ms" << endl;
}

int main(int argc, char *argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "produce this help message")
        ("dir,d", po::value<string>()->required(), "Images ocx1 dir")
        ("input-dir,i", po::value<string>()->required(), "Directory with iidx, idex_freq, imgstats files")
        ("query,q", po::value<string>()->required(), "Query image ID. Enter -1 iif giving a query file")
        ("num-select,k", po::value<int>()->required(), "num of images to select")
        ("query-file,f", po::value<string>(), "Query image IDs file path. Required if q = -1")
        ("output-dir,o", po::value<string>(), "Outputs directory. Required if q = -1")
        ("debug,g", "Set flag to print scores in output")
        ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error& e) {
        cout << e.what() << endl;
        cout << desc << endl;
        return -1;
    }

    if (vm.count("help")) {
        cout << desc << endl;
        return -1;
    }

    int K = vm["num-select"].as<int>();
    map<int, map<string, int> > invIdx = readFromFileInvIndex(vm["input-dir"].as<string>() + "/" + INV_IDX_FNAME);
    cerr << "Read inverted index" << endl;
    map<string, pair<int, int> > imgStats = readFromFileImgStats(vm["input-dir"].as<string>() + "/" + IMG_STATS_FNAME);

    string query = vm["query"].as<string>();
    vector<pair<string,float> > ids;
    if (query.compare("-1") == 0) {
        if (!vm.count("query-file") || !vm.count("output-dir")) {
            cerr << "Check CLI" << endl;
            return -1;
        }
        string inp_file = vm["query-file"].as<string>();
        string output_dir = vm["output-dir"].as<string>();
        ifstream fin(inp_file.c_str());
        string line;
        while (getline(fin, line)) {
            runSearch(vm["dir"].as<string>(),
                    line,
                    invIdx,
                    imgStats,
                    ids);
            ofstream fout((output_dir + "/" + line + ".out").c_str(), ios::out);
            for (int i = 0; i < min((int)ids.size(), K); i++) {
                fout << ids[i].first.substr(5);
                if (vm.count("debug")) {
                    fout << " " << ids[i].second;
                }
                fout << endl;
            }
            fout.close();
            cerr << "Done for " << line << endl;
        }
        fin.close();
    } else {
        runSearch(vm["dir"].as<string>(),
                query,
                invIdx,
                imgStats,
                ids);
        for (int i = 0; i < min((int)ids.size(), K); i++) {
            cout << ids[i].first.substr(5);
            if (vm.count("debug")) {
                cout << " " << ids[i].second;
            }
            cout << endl;
        }
    }
    return 0;
}


