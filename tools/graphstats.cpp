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

typedef std::vector< std::vector<unsigned int> > graph_t; 


int count_triangles(graph_t& graph){
	std::vector< unsigned int > current_nodes;
	unsigned int cur_num_nodes = graph.size();

	int n_triangles = 0;
	for(unsigned int node = 0; node < cur_num_nodes; node++){

		for (unsigned int edge_a : graph[node]){
			if (edge_a != node){
				for (unsigned int edge_b : graph[edge_a]){
					if ((edge_b != node) && (edge_b != edge_a)) {
						for (unsigned int edge_c : graph[edge_b]){
							if (edge_c == node){
								n_triangles++;
							}
						}
					}
				}
			}
		}
	}

	return n_triangles;
}

int count_squares(graph_t& graph){
	std::vector< unsigned int > current_nodes;
	unsigned int cur_num_nodes = graph.size();

	int n_squares = 0;

	for(unsigned int node = 0; node < cur_num_nodes; node++){
		for (unsigned int edge_a : graph[node]){
			if (edge_a != node){
				for (unsigned int edge_b : graph[edge_a]){
					if ((edge_b != node) && (edge_b != edge_a)) {
						for (unsigned int edge_c : graph[edge_b]){
							if ((edge_c != node) && (edge_c != edge_b) && (edge_c != edge_a) ){
								for (unsigned int edge_d : graph[edge_c]){
									if (edge_d == node){
										n_squares++;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return n_squares;
}


std::vector< int > degree_dist(graph_t& graph){
	int max_out_deg = 0;
	for (int n = 0; n < graph.size(); n++){
		if (graph[n].size() > max_out_deg){
			max_out_deg = graph[n].size();
		}
	}

	std::vector< int > deg(max_out_deg);

	for (int n = 0; n < graph.size(); n++){
		deg[graph[n].size()]++;
	}
	return deg;
}




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


	std::vector<int> deg = degree_dist(graph);
	std::cout<<"deg_dist = [";
	for (int i = 0; i < deg.size(); i++){
		std::cout<<deg[i];
		if (i < (deg.size()-1)){
			std::cout<<",";
		}
		else{
			std::cout<<"]"<<std::endl;
		}
	}



	int n_triangles = count_triangles(graph);
	std::cout<<"n_triangles = "<<n_triangles<<std::endl;

	int n_squares = count_squares(graph);
	std::cout<<"n_squares = "<<n_squares<<std::endl;


    return 0;
}
