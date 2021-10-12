#include <iostream> 
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <fstream>
#include <utility>

#include "flatnav/Index.h"
#include <algorithm>
#include <string>


int main(int argc, char **argv){

    if (argc < 6){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"construct <data> <N> <M> <ef_construction> <outfile>"<<std::endl;
		std::clog<<"\t <data> bvecs sift file"<<std::endl;
		std::clog<<"\t <N> int, number of vectors to include from SIFT1B"<<std::endl;
        std::clog<<"\t <M>: int "<<std::endl;
        std::clog<<"\t <ef_construction>: int "<<std::endl;
        std::clog<<"\t <outfile>: where to stash the index"<<std::endl;
        return -1;
    }

	//int space_ID = std::stoi(argv[1]);
	// cnpy::NpyArray datafile = cnpy::npy_load(argv[2]);
	int M = std::stoi(argv[3]);
    int ef_construction = std::stoi(argv[4]);
	int dim = 128;
    int N = std::stoi(argv[2]);

	std::ifstream input(argv[1], std::ios::binary);

	L2SpaceI space(dim);
    Index<int, int> index(&space, N, M);

    auto start = std::chrono::high_resolution_clock::now();

	unsigned char *element = new unsigned char[dim];
    for (int label = 0; label < N; label++) {
		
		int dim_check;
		input.read((char*)&dim_check, 4); 
		if (dim_check != dim){std::cerr<<"File error"<<std::endl; return -1; }
		input.read((char*) element, dim);
        index.add((void*) element, label, ef_construction, 1000);
    	if (label%100000 == 0){std::clog<<"+"<<std::endl;}
    }
	delete[] element;
	input.close();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::clog << "Build time: " << (float)(duration.count())/(1000.0) << " seconds" << std::endl; 

	std::clog << "Saving index to: " << argv[5] << std::endl;
	std::string filename(argv[5]);
	index.save(filename);

    return 0;
}