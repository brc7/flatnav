#include <iostream> 
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <fstream>
#include <utility>

#include "../flatnav/Index.h"
#include "cnpy.h"
#include <algorithm>
#include <string>


int main(int argc, char **argv){

    if (argc < 6){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"construct <space> <data> <M> <ef_construction> <outfile>"<<std::endl;
        std::clog<<"\t <space> int, 0 for L2, 1 for inner product (angular)"<<std::endl;
		std::clog<<"\t <data> npy file from ann-benchmarks"<<std::endl;
        std::clog<<"\t <M>: int "<<std::endl;
        std::clog<<"\t <ef_construction>: int "<<std::endl;
        std::clog<<"\t <outfile>: where to stash the index"<<std::endl;
        return -1;
    }

	int space_ID = std::stoi(argv[1]);
	cnpy::NpyArray datafile = cnpy::npy_load(argv[2]);
	int M = std::stoi(argv[3]);
    int ef_construction = std::stoi(argv[4]);

    if ( (datafile.shape.size() != 2) ){
        return -1;
    }

    int dim = datafile.shape[1];
    int N = datafile.shape[0];

    std::clog<<"Loading "<<dim<<"-dimensional dataset with N = "<<N<<std::endl;
    float* data = datafile.data<float>();
    
	SpaceInterface<float>* space; 
	if (space_ID == 0){
		space = new L2Space(dim);
	} else {
		space = new InnerProductSpace(dim);
	}

    Index<float, int> index(space, N, M);

    auto start = std::chrono::high_resolution_clock::now();

    for (int label = 0; label < N; label++) {
        float* element = data + dim*label;
		// std::clog<<"Adding "<<label<<std::endl;
        index.add((void*) element, label, ef_construction);
		if (label%100000==0) std::clog<<"."<<std::flush;
    }
	std::clog<<std::endl;

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::clog << "Build time: " << (float)(duration.count())/(1000.0) << " seconds" << std::endl; 

	std::clog << "Saving index to: " << argv[5] << std::endl;
	std::string filename(argv[5]);
	index.save(filename);

    return 0;
}
