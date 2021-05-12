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

typedef std::vector< std::vector<unsigned int> > graph_t; 


int main(int argc, char **argv){

    if (argc < 6){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"knngraphdump <space> <data> <k> <ef_construction> <index> "<<std::endl;
        std::clog<<"\t <space> int, 0 for L2, 1 for inner product (angular)"<<std::endl;
		std::clog<<"\t <data> npy file from ann-benchmarks that was used to construct the index"<<std::endl;
		std::clog<<"\t <k>: int (number of k-nearest neighbors)"<<std::endl;
		std::clog<<"\t <ef_search>: int efficiency parameter used to query the graph to build a full k-nn graph"<<std::endl;
		std::clog<<"\t <index> a graph index, built using "<<std::endl;
        return -1; 
    }

	std::string indexfilename(argv[5]);
	int space_ID = std::stoi(argv[1]);
	cnpy::NpyArray datafile = cnpy::npy_load(argv[2]);
	int k = std::stoi(argv[3]);
    int ef_search = std::stoi(argv[4]);

    if ( (datafile.shape.size() != 2) ){
        return -1;
    }

    int dim = datafile.shape[1];
    int N = datafile.shape[0];

    std::clog<<"Loading "<<dim<<"-dimensional dataset with N = "<<N<<std::endl;
    float* data = datafile.data<float>();

    std::clog<<"Loading index"<<std::endl;

	SpaceInterface<float>* space; 
	if (space_ID == 0){
		space = new L2Space(dim);
	} else {
		space = new InnerProductSpace(dim);
	}

    Index<float, int> index(space, indexfilename);
    std::clog<<"Index has N = "<<index.size()<<std::endl;

	std::cout<<"\%\%MatrixMarket matrix coordinate pattern general"<<std::endl;
	std::cout<<N<<" "<<N<<" "<<N*k<<std::endl;

    for (int label = 0; label < N; label++){
    	float* q = data + dim*label; 
    	std::vector<std::pair<float,int> > result = index.search(q, k+1, ef_search);
        bool found = false;
        for (int i = 0; i < result.size(); i++){
            if (result[i].second != label){
                std::cout<< label+1 <<" "<< result[i].second + 1 << std::endl;    
            } else {
                found = true;
            }
            if (!found && i + 2 == result.size()) {
                break;
            }
        }
    }

    return 0;
}