#include <cstdio>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>
#include <tuple>
#include <sys/time.h>
#include "IndexUtils.hpp"
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
namespace fs = boost::filesystem;
namespace po = boost::program_options;

using namespace std;

#define INV_IDX_FNAME "invIndex.txt"
#define IMG_STATS_FNAME "imgStats.txt"

int TAU_FOR_GEOM_RERANK = 20; // default value

int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + 
            (t1.tv_usec - t2.tv_usec))/1000;
}

void runSearch(string dir,
        string query,
        vector<float> bounding_box,
        const map<int, map<string, int> > &invIdx,
        const map<string, pair<int, int> > &imgStats,
        vector<tuple<string, float, int> > &outputIDs, // id, tf-idf, #inliers
        bool geomRerank) {
    timeval ts_start, ts_end;
    gettimeofday(&ts_start, NULL);
    map<int, int> vws = readDescriptorsWithCounts(dir + "/" +
            query + ".txt", bounding_box);
    outputIDs = getClosestImgs(
            vws,
            dir,
            invIdx,
            imgStats);
    if (geomRerank) {
        vws.clear(); // for memory free
        map<int, vector<pair<float,float> > > vws_pos = readDescriptorsWithPos(dir + "/" +
                query + ".txt", bounding_box);
        geometricReranking(outputIDs, vws_pos, dir, TAU_FOR_GEOM_RERANK);
    }
    gettimeofday(&ts_end, NULL);
    cerr << "Elapsed time (ms) : " << diff_ms(ts_end, ts_start) << endl;
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
        ("bounding-box,b", po::value<vector<float> >()->multitoken(), 
         "The bounding box coordinates, in oxford query file order. qx1, qy1, qx2, qy2")
        ("geom-rerank,r", po::bool_switch()->default_value(false), "Pass flag to perform geometric reranking")
        ("geom-rerank-tau,t", po::value<int>()->default_value(TAU_FOR_GEOM_RERANK), 
         "Min # of inliers to consider for geometric reranking")
        ("debug,g", po::bool_switch()->default_value(false), "Set flag to print scores in output")
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
    TAU_FOR_GEOM_RERANK = vm["geom-rerank-tau"].as<int>();

    int K = vm["num-select"].as<int>();
    map<int, map<string, int> > invIdx = readFromFileInvIndex(vm["input-dir"].as<string>() + "/" + INV_IDX_FNAME);
    cerr << "Read inverted index" << endl;
    map<string, pair<int, int> > imgStats = readFromFileImgStats(vm["input-dir"].as<string>() + "/" + IMG_STATS_FNAME);

    string query = vm["query"].as<string>();
    vector<tuple<string, float, int>> ids;
    vector<float> bounding_box;
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
            istringstream iss(line);
            string img_name;
            float qx1 = -1, qx2 = -1, qy1 = -1, qy2 = -1;
            iss >> img_name >> qx1 >> qy1 >> qx2 >> qy2;
            bounding_box.clear();
            if (qx1 != -1 && qx2 != -1 && qy1 != -1 && qy2 != -1) {
                bounding_box.push_back(qx1);
                bounding_box.push_back(qy1);
                bounding_box.push_back(qx2);
                bounding_box.push_back(qy2);
            }
            runSearch(vm["dir"].as<string>(),
                    img_name,
                    bounding_box,
                    invIdx,
                    imgStats,
                    ids,
                    vm["geom-rerank"].as<bool>());
            ofstream fout((output_dir + "/" + img_name + ".out").c_str(), ios::out);
            for (int i = 0; i < min((int)ids.size(), K); i++) {
                fout << get<0>(ids[i]).substr(5);
                if (vm["debug"].as<bool>()) {
                    fout << " " << get<1>(ids[i]) << " " << get<2>(ids[i]);
                }
                fout << endl;
            }
            fout.close();
            cerr << "Done for " << img_name << endl;
        }
        fin.close();
    } else {
        if (vm.count("bounding-box")) {
            bounding_box = vm["bounding-box"].as<vector<float> >();
        }
        runSearch(vm["dir"].as<string>(),
                query,
                bounding_box,
                invIdx,
                imgStats,
                ids,
                vm["geom-rerank"].as<bool>());
        for (int i = 0; i < min((int)ids.size(), K); i++) {
            cout << get<0>(ids[i]).substr(5);
            if (vm["debug"].as<bool>()) {
                cout << " " << get<1>(ids[i]) << " " << get<2>(ids[i]);
            }
            cout << endl;
        }
    }
    return 0;
}


