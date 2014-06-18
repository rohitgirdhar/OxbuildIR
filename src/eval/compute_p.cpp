#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>

using namespace std;

vector<string>
load_list(const string& fname)
{
  vector<string> ret;
  ifstream fobj(fname.c_str());
  if (!fobj.good()) { cerr << "File " << fname << " not found!\n"; exit(-1); }
  string line;
  while (getline(fobj, line)) {
    ret.push_back(line);
  }
  return ret;
}

template<class T>
set<T> vector_to_set(const vector<T>& vec)
{ return set<T>(vec.begin(), vec.end()); }

float
compute_p(const set<string>& pos, const set<string>& amb, const vector<string>& ranked_list, 
        int original_pos_set_size)
{
  float p = 0.0;
  size_t intersect_size = 0;
  size_t i = 0;
  size_t j = 0;
  for ( ; i<ranked_list.size(); ++i) {
    if (amb.count(ranked_list[i])) continue;
    if (pos.count(ranked_list[i])) intersect_size++;

    j++;
  }
  if (intersect_size == original_pos_set_size) { // means all positive images in the set
    cerr << "All the pos images (" << intersect_size << ") were in set" << endl;
    return 1;
  }
  p = intersect_size / ( j * 1.0 );
  return p;
}

int
main(int argc, char** argv)
{
  if (argc < 3) {
    cout << "Usage: ./compute_ap [GROUNDTRUTH QUERY] [RANKED LIST] [SELECTED LIST (optional)]\n";
    return -1;
  }

  string gtq = argv[1];

  vector<string> ranked_list = load_list(argv[2]);
  set<string> good_set = vector_to_set( load_list(gtq + "_good.txt") );
  set<string> ok_set = vector_to_set( load_list(gtq + "_ok.txt") );
  set<string> junk_set = vector_to_set( load_list(gtq + "_junk.txt") );

  set<string> pos_set;
  pos_set.insert(good_set.begin(), good_set.end());
  pos_set.insert(ok_set.begin(), ok_set.end());

  int original_pos_set_size = pos_set.size();
  if (argc == 4) {
      cerr << "compute_p: Using the selected List from " << argv[3] << endl;
      set<string> bestK_set = vector_to_set( load_list(argv[3]) );
      set<string> output;
      set_intersection(pos_set.begin(), pos_set.end(), bestK_set.begin(), bestK_set.end(), 
              inserter(output, output.begin()));
      pos_set = output;
  }

  float p = compute_p(pos_set, junk_set, ranked_list, original_pos_set_size);
  
  cout << p << "\n";

  return 0;
}
