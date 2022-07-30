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

    if (argc < 8){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"query <index> <n_queries> <queries> <gtruth> <ef_search> <k> <Reorder ID>"<<std::endl;

        std::clog<<"\t <ef_search>: int,int,int,int...,int "<<std::endl;
        std::clog<<"\t <k>: number of neighbors "<<std::endl;
        return -1; 
    }

    std::string indexfilename(argv[1]);
    size_t dim = 200;
    int Nq = std::stoi(argv[2]);
    int reorder_ID = std::stoi(argv[7]);

    std::vector<int> ef_searches;
    std::stringstream ss(argv[5]);
    int element = 0; 
    while(ss >> element){
        ef_searches.push_back(element);
        if (ss.peek() == ',') ss.ignore();
    }
    int k = std::stoi(argv[6]);
    


    std::ifstream querystream(argv[3] ,std::ios::binary);
    float* queries = new float[Nq*dim];
    unsigned int dim_check;
    unsigned int num_check_query;
    querystream.read((char*)&num_check_query, 4);
    querystream.read((char*)&dim_check, 4);
    std::clog<<"Reading "<<Nq<<" queries of "<<num_check_query<<" total queries of dimension "<<dim_check<<"."<<std::endl;
    if (dim_check != dim){std::cerr<<"Error reading dimensions."<<std::endl; return -1; }
    if (num_check_query != Nq){std::clog<<"Warning: Using only "<< Nq << " points of total "<< num_check_query <<"."<<std::endl;}
    for (size_t i = 0; i < Nq; i++){
        querystream.read((char*)(queries + dim*i), 4*dim);
    }
    querystream.close();
    std::clog<<"Read "<<Nq<<" queries"<<std::endl;

    InnerProductSpace space(dim);
    Index<float, int> index(&space, indexfilename);

    // do reordering
    if (reorder_ID == 1){
        std::clog<<"Using GORDER"<<std::endl;
        std::clog << "Reordering: "<< std::endl; 
        auto start_r = std::chrono::high_resolution_clock::now();    
        index.reorder(Index<float, int>::GraphOrder::GORDER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 2){
        std::clog<<"Using IN-DEG-SORT"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<float, int>::GraphOrder::IN_DEG);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 3){
        std::clog<<"Using OUT-DEG-SORT"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<float, int>::GraphOrder::OUT_DEG);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 4){
        std::clog<<"Using Reverse-Cuthill-McKee"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<float, int>::GraphOrder::RCM);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 5){
        std::clog<<"Using HUBSORT"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<float, int>::GraphOrder::HUB_SORT);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 6){
        std::clog<<"Using HUBCLUSTER"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<float, int>::GraphOrder::HUB_CLUSTER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 7){
        std::clog<<"Using DBG"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<float, int>::GraphOrder::DBG);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 41){
        std::clog<<"Using RCM+Gorder"<<std::endl;
        std::clog << "Reordering: "<< std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.reorder(Index<float, int>::GraphOrder::RCM);
        index.reorder(Index<float, int>::GraphOrder::GORDER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 19){
        std::clog<<"Using profile-based GORDER"<<std::endl;
        std::clog<<"Reordering"<<std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.profile_reorder(queries, Nq, 100, Index<float, int>::ProfileOrder::GORDER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 49){
        std::clog<<"Using profile-based RCM"<<std::endl;
        std::clog<<"Reordering"<<std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.profile_reorder(queries, Nq, 100, Index<float, int>::ProfileOrder::RCM);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else{
        std::clog<<"No reordering"<<std::endl;
    }

    for (int& ef_search: ef_searches){
        for (int i = 0; i < Nq; i++){
            float* q = queries + dim*i;

            std::vector<std::pair<float, int> > result = index.search(q, k, ef_search);
            
            double recall = 0;
            std::clog<<i<<": ";
            for (int j = 0; j <  k; j++){
                std::clog<<result[j].second<<" ";
            }
            std::clog<<std::endl;
        }
    }

    delete[] queries; 
    return 0;
}