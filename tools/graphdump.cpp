#include <iostream> 
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <fstream>
#include <utility>

#include "flatnav/Index.h"
#include "cnpy.h"
#include <algorithm>
#include <string>

typedef std::vector< std::vector<unsigned int> > graph_t; 


int main(int argc, char **argv){

    if (argc < 2){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"graphstats <index>"<<std::endl;
        return -1; 
    }

	std::string indexfilename(argv[1]);
	SpaceInterface<float>* space = new L2Space(1);
    Index<float, int> index(space, indexfilename);
	graph_t graph = index.graph();

	std::cout<<"\%\%MatrixMarket matrix coordinate pattern general"<<std::endl;
	// %%MatrixMarket matrix coordinate pattern general
	// N N M 
	// N = # nodes, M = # edges

	int N = graph.size();
	int M = 0;
	for (auto& node : graph){
		M += node.size();
	}
	std::cout<<N<<" "<<N<<" "<<M<<std::endl;
	for (int node = 0; node < N; node++){
		for (auto& edge : graph[node]){
			std::cout<<node+1<<" "<<edge+1<<std::endl;
		}
	}
    return 0;
}
