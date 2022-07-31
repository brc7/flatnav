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

    if (argc < 7){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"query <index> <space> <queries> <gtruth> <ef_search> <k>";
        std::clog<<" [--nq num_queries] [--reorder_id reorder_id] [--ef_profile ef_profile] [--num_profile num_profile]"<<std::endl;
        std::clog<<"Positional arguments:"<<std::endl;
        std::clog<<"\t index: Filename for input index (float32 index)."<<std::endl;
        std::clog<<"\t space: Integer distance ID: 0 for L2 distance, 1 for inner product (angular distance)."<<std::endl;
        std::clog<<"\t queries: Filename for queries (float32 file)."<<std::endl;
        std::clog<<"\t gtruth: Filename for ground truth (int32 file)."<<std::endl;
        std::clog<<"\t ef_search: CSV list of int,int,int...,int ef_search parameters."<<std::endl;
        std::clog<<"\t k: Number of neighbors to return."<<std::endl;
        
        std::clog<<"Optional arguments:"<<std::endl;
        std::clog<<"\t [--nq num_queries]: (Optional, default 0) Number of queries to use. If 0, uses all queries."<<std::endl;
        std::clog<<"\t [--reorder_id reorder_id]: (Optional, default 0) Which reordering algorithm to use? 0:none 1:gorder 2:indegsort 3:outdegsort 4:RCM 5:hubsort 6:hubcluster 7:DBG 8:corder 91:profiled_gorder 94:profiled_rcm 41:RCM+gorder"<<std::endl;
        std::clog<<"\t [--ef_profile ef_profile]: (Optional, default 100) ef_search parameter to use for profiling."<<std::endl;
        std::clog<<"\t [--num_profile num_profile]: (Optional, default 1000) Number of queries to use for profiling."<<std::endl;
        return -1;
    }

    // Optional arguments.
    int num_queries = 0; 
    int reorder_ID = 0;
    int ef_profile = 100;
    int num_profile = 1000;

    for (int i = 0; i < argc; ++i){
        if (std::strcmp("--nq",argv[i]) == 0){
            if ((i+1) < argc){
                num_queries = std::stoi(argv[i+1]);
            } else {
                std::cerr<<"Invalid argument for optional parameter --nq"<<std::endl; 
                return -1;
            }
        }
        if (std::strcmp("--reorder_id",argv[i]) == 0){
            if ((i+1) < argc){
                reorder_ID = std::stoi(argv[i+1]);
            } else {
                std::cerr<<"Invalid argument for optional parameter --reorder_id"<<std::endl; 
                return -1;
            }
        }
        if (std::strcmp("--ef_profile",argv[i]) == 0){
            if ((i+1) < argc){
                ef_profile = std::stoi(argv[i+1]);
            } else {
                std::cerr<<"Invalid argument for optional parameter --ef_profile"<<std::endl; 
                return -1;
            }
        }
        if (std::strcmp("--num_profile",argv[i]) == 0){
            if ((i+1) < argc){
                num_profile = std::stoi(argv[i+1]);
            } else {
                std::cerr<<"Invalid argument for optional parameter --num_profile"<<std::endl; 
                return -1;
            }
        }
    }
    // Positional arguments.
    std::string indexfilename(argv[1]);  // Index filename.
    int space_ID = std::stoi(argv[2]); // Space ID for querying.

    // Load queries.
    std::ifstream querystream(argv[3], std::ios::binary);
    unsigned int dim;
    unsigned int num_queries_check;
    querystream.read((char*)&num_queries_check, 4);
    querystream.read((char*)&dim, 4);
    if (num_queries == 0){ // If nq not specified, use all queries.
        num_queries = num_queries_check;
    }
    std::clog<<"Reading "<<num_queries<<" queries of "<<num_queries_check<<" total queries of dimension "<<dim<<"."<<std::endl;
    if (num_queries_check != num_queries){std::clog<<"Warning: Using only "<< num_queries << " points of total "<< num_queries_check <<"."<<std::endl;}
    // Allocate and load the queries into RAM.
    unsigned char* queries = new unsigned char[num_queries * dim];
    for (size_t i = 0; i < num_queries; i++){
        querystream.read((char*)(queries + dim*i), 4*dim);
    }
    querystream.close();
    std::clog<<"Read "<<num_queries<<" queries into RAM."<<std::endl;

    // Load ground truth.
    std::ifstream truthstream(argv[4], std::ios::binary);
    int num_gtruth_lists;
    int num_gtruth_entries;
    truthstream.read((char*)&num_gtruth_lists, 4);
    truthstream.read((char*)&num_gtruth_entries, 4);
    std::clog<<"Reading "<<num_gtruth_lists<<" ground truth lists of "<<num_gtruth_entries<<" results."<<std::endl;
    if (num_gtruth_lists != num_queries){
        std::clog<<"Warning: There are "<<num_queries<<" queries but only "<<num_gtruth_lists<<" ground truth lists!"<<std::endl;
        if (num_gtruth_lists < num_queries){
            std::cerr<<"Error: Need at least "<<num_queries<<" gtruth lists."<<std::endl;
            return -1;
        }
    }
    std::clog<<"Reading ground truth."<<std::endl;
    unsigned int* gtruth = new unsigned int[num_gtruth_lists * num_gtruth_entries];
    for (size_t i = 0; i < num_gtruth_lists; i++){
        truthstream.read((char*)(gtruth + num_gtruth_entries*i), num_gtruth_entries * 4);
    }
    truthstream.close();
    std::clog<<"Read "<<num_gtruth_lists<<" gtruth vectors into RAM."<<std::endl;

    // EF search vector.
    std::vector<int> ef_searches;
    std::stringstream ss(argv[5]);
    int element = 0; 
    while(ss >> element){
        ef_searches.push_back(element);
        if (ss.peek() == ',') ss.ignore();
    }
    // Number of search results.
    int k = std::stoi(argv[6]);
    if (k > num_gtruth_entries){
        std::cerr<<"K is larger than the number of precomputed ground truth neighbors."<<std::endl;
        return -1;
    }

    // Load the index from disk.
    SpaceInterface<int>* space;
    space = new L2SpaceI(dim);
    // TODO: Support integer inner product spaces (even though no benchmark datasets use this).
    // if (space_ID == 0){
    //     space = new L2Space(dim);
    // } else {
    //     space = new InnerProductSpace(dim);
    // }
    std::clog<<"Loading index from "<<indexfilename<<std::endl;
    Index<int, int> index(space, indexfilename);

    // Do reordering, if necessary.
    if (num_profile > num_queries){
        std::clog<<"Warning: Number of profiling queries ("<<num_profile<<") is greater than number of queries ("<<num_queries<<")!"<<std::endl;
        num_profile = num_queries;
    }
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
        std::clog<<"Using CORDER"<<std::endl;
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
    else if (reorder_ID == 91){
        std::clog<<"Using profile-based GORDER"<<std::endl;
        std::clog<<"Reordering"<<std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.profile_reorder(queries, num_profile, ef_profile, Index<int, int>::ProfileOrder::GORDER);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else if (reorder_ID == 94){
        std::clog<<"Using profile-based RCM"<<std::endl;
        std::clog<<"Reordering"<<std::endl;
        auto start_r = std::chrono::high_resolution_clock::now();
        index.profile_reorder(queries, num_profile, ef_profile, Index<int, int>::ProfileOrder::RCM);
        auto stop_r = std::chrono::high_resolution_clock::now();
        auto duration_r = std::chrono::duration_cast<std::chrono::milliseconds>(stop_r - start_r);
        std::clog << "Reorder time: " << (float)(duration_r.count())/(1000.0) << " seconds" << std::endl; 
    }
    else{
        std::clog<<"No reordering"<<std::endl;
    }

    // Now, finally, do the actual search.
    std::cout<<"recall, mean_latency_ms"<<std::endl;
    for (int& ef_search: ef_searches){
        double mean_recall = 0;

        auto start_q = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < num_queries; i++){
            unsigned char* q = queries + dim*i;
            unsigned int* g = gtruth + num_gtruth_entries*i;

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
            mean_recall = mean_recall + recall;
        }
        auto stop_q = std::chrono::high_resolution_clock::now();
        auto duration_q = std::chrono::duration_cast<std::chrono::milliseconds>(stop_q - start_q);
        std::cout<<mean_recall/num_queries<<","<<(float)(duration_q.count())/num_queries<<std::endl;
    }

    delete[] queries; 
    delete[] gtruth; 
    return 0;
}
