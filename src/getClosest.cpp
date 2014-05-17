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

int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + 
            (t1.tv_usec - t2.tv_usec))/1000;
}

int main(int argc, char *argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "produce this help message")
        ("dir,d", po::value<string>()->required(), "Images ocx1 dir")
        ("iidx,i", po::value<string>()->required(), "Inverted Index file")
        ("iidx-counts,c", po::value<string>()->required(), "Inverted Index Counts file")
        ("imgstats,s", po::value<string>()->required(), "Image stats ffile")
        ("query,q", po::value<string>()->required(), "Query image ID")
        ("num-select,k", po::value<int>()->required(), "num of images to select")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return -1;
    }

    int K = vm["num-select"].as<int>();
    map<int, map<string, int> > invIdx = readFromFileInvIndex(vm["iidx"].as<string>());
    cerr << "Read inverted index" << endl;
    map<string, pair<int, int> > imgStats = readFromFileImgStats(vm["imgstats"].as<string>());
    
    set<int> vws = readDescriptors(vm["dir"].as<string>() + "/" +
            vm["query"].as<string>() + ".txt");
    int start_time = clock();
    vector<string> ids = getClosestImgs(
            vws,
            vm["dir"].as<string>(),
            invIdx,
            imgStats);
    cerr << "Time in search " << clock() - start_time << " ms" << endl;
    for (int i = 0; i < min((int)ids.size(), K); i++) {
        cout << ids[i].substr(5) << endl;
    }
    return 0;
}
