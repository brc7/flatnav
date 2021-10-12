#include <iostream> 
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <fstream>
#include <utility>
#include <sstream>

#include "../flatnav/Index.h"
#include <algorithm>
#include <string>


int main(int argc, char **argv){

    if (argc < 9){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"query <index> <n_queries> <queries> <gtruth> <ef_search> <k> <Reorder ID> <temp-profile-dump>"<<std::endl;

        std::clog<<"\t <ef_search>: int,int,int,int...,int "<<std::endl;
        std::clog<<"\t <k>: number of neighbors "<<std::endl;
        return -1; 
    }

	std::string indexfilename(argv[1]);
	size_t dim = 128;
	int Nq = std::stoi(argv[2]);

	unsigned char* queries = new unsigned char[Nq*dim];
	std::ifstream querystream(argv[3] ,std::ios::binary);
	std::clog<<"Reading queries: ";
	for (size_t i = 0; i < Nq; i++){
		int dim_check;
		querystream.read((char*)&dim_check, 4);
		if (dim_check != dim){std::cerr<<"File error"<<std::endl; return -1; }
		querystream.read((char*)(queries + dim*i), dim);
	}
	querystream.close();
	std::clog<<"Read "<<Nq<<" queries"<<std::endl;


	int n_gtruth = 1000;
	unsigned int* gtruth = new unsigned int[Nq*n_gtruth];
	std::ifstream truthstream(argv[4] ,std::ios::binary);
	std::clog<<"Reading gtruth: ";
	for (size_t i = 0; i < Nq; i++){
		int n_gtruth_check;
		truthstream.read((char*)&n_gtruth_check, 4);
		if (n_gtruth_check != n_gtruth){std::cerr<<"File error"<<std::endl; return -1; }
		truthstream.read((char*)(gtruth + n_gtruth*i), n_gtruth * 4);
	}
	truthstream.close();
	std::clog<<"Read "<<Nq<<" gtruth"<<std::endl;


    std::vector<int> ef_searches;
	std::stringstream ss(argv[5]);
    int element = 0; 
    while(ss >> element){
        ef_searches.push_back(element);
        if (ss.peek() == ',') ss.ignore();
    }
    int k = std::stoi(argv[6]);
    
	if (k > n_gtruth){
        std::cerr<<"K is larger than the number of precomputed ground truth neighbors"<<std::endl;
        return -1;
    }

	L2SpaceI space(dim);
    Index<int, int> index(&space, indexfilename);

    // do reordering
    if (reorder_ID == 1){
        std::clog<<"Using GORDER"<<std::endl;
        std::clog << "Reordering: "<< std::endl; 
        auto start_r = std::chrono::high_resolution_clock::now();    
        index.reorder(Index<int, int>::GraphOrder::GORDER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 2){
        std::clog<<"Using IN-DEG-SORT"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<int, int>::GraphOrder::IN_DEG);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 3){
        std::clog<<"Using OUT-DEG-SORT"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<int, int>::GraphOrder::OUT_DEG);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 4){
        std::clog<<"Using Reverse-Cuthill-McKee"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<int, int>::GraphOrder::RCM);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 5){
        std::clog<<"Using HUBSORT"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<int, int>::GraphOrder::HUB_SORT);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 6){
        std::clog<<"Using HUBCLUSTER"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<int, int>::GraphOrder::HUB_CLUSTER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 7){
        std::clog<<"Using DBG"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<int, int>::GraphOrder::DBG);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 8){
        std::clog<<"Using BCORDER"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<int, int>::GraphOrder::BCORDER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 41){
        std::clog<<"Using RCM+Gorder"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<int, int>::GraphOrder::RCM);
        index.reorder(Index<int, int>::GraphOrder::GORDER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 9){
        std::clog<<"Using profile-based order"<<std::endl;
        std::clog<<"Reordering"<<std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.profile_reorder(queries, Nq, 100, 5);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
        std::clog<<"Dumping index "<<std::endl;
        index.save("profile-order.idx");
    }
    else{
        std::clog<<"No reordering"<<std::endl;
    }



    for (int& ef_search: ef_searches){
        // double mean_dists = 0;
        double mean_recall = 0;

        auto start_q = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < Nq; i++){
            unsigned char* q = queries + dim*i;
            unsigned int* g = gtruth + n_gtruth*i;

            std::vector<std::pair<int, int> > result = index.search(q, k, ef_search);
            
            double recall = 0;
            for (int j = 0; j <  k; j++){
                for (int l = 0; l <  k; l++){
                    if (result[j].second == g[l]){
                        recall = recall + 1;
                    }
                }
            }
            recall = recall / k;
            // mean_dists = mean_dists + index.N_DISTANCE_EVALS;
            mean_recall = mean_recall + recall;
        }
        auto stop_q = std::chrono::high_resolution_clock::now();
        auto duration_q = std::chrono::duration_cast<std::chrono::milliseconds>(stop_q - start_q);
        std::cout<<mean_recall/Nq<<","<<(float)(duration_q.count())/Nq<<std::endl;
    }

	delete[] queries; 
	delete[] gtruth; 
    return 0;
}
