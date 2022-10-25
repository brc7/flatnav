#include <iostream> 
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <fstream>
#include <utility>

#include "../flatnav/Index.h"
#include <algorithm>
#include <string>


int main(int argc, char **argv){

    if (argc < 4){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"construct_float32 <data> <space> <outfile>";
        std::clog<<" [--N num_vectors] [--M num_links] [--ef ef_construction] [--verbose num_verbose]"<<std::endl;

        std::clog<<"Positional arguments: "<<std::endl;
        std::clog<<"\t data: Filename pointing to an fvecs file (4 byte uint N, 4 byte uint dim, then list of 32-bit little-endian floats)."<<std::endl;
        std::clog<<"\t space: Integer distance ID: 0 for L2 distance, 1 for inner product (angular distance)."<<std::endl;
        std::clog<<"\t outfile: Filename for the index (.idx extension recommended)."<<std::endl;

        std::clog<<"Optional arguments: "<<std::endl;
        std::clog<<"\t [--N num_vectors]: (Optional, default 0) Number of vectors to include. If 0, uses full dataset."<<std::endl;
        std::clog<<"\t [--M num_links]: (Optional, default 8) Max number of links per node."<<std::endl;
        std::clog<<"\t [--ef ef_construction]: (Optional, default 400) Search parameter used for construction."<<std::endl;
        std::clog<<"\t [--verbose num_verbose]: (Optional, default 100000) Number of vectors for progress bar. If zero, no progress bar."<<std::endl;
        return -1;
    }

    // Positional arguments.
    std::ifstream input(argv[1], std::ios::binary);
    int space_ID = std::stoi(argv[2]);
    std::string outfilename(argv[3]);

    // Optional arguments.
    int N = 0;
    int M = 8;
    int ef_construction = 400;
    int num_verbose = 100000;

    for (int i = 0; i < argc; ++i){
        if (std::strcmp("--N",argv[i]) == 0){
            if ((i+1) < argc){
                N = std::stoi(argv[i+1]);
            } else {
                std::cerr<<"Invalid argument for optional parameter --N"<<std::endl; 
                return -1;
            }
        }
        if (std::strcmp("--M",argv[i]) == 0){
            if ((i+1) < argc){
                M = std::stoi(argv[i+1]);
            } else {
                std::cerr<<"Invalid argument for optional parameter --M"<<std::endl; 
                return -1;
            }
        }
        if (std::strcmp("--ef",argv[i]) == 0){
            if ((i+1) < argc){
                ef_construction = std::stoi(argv[i+1]);
            } else {
                std::cerr<<"Invalid argument for optional parameter --ef"<<std::endl; 
                return -1;
            }
        }
        if (std::strcmp("--verbose",argv[i]) == 0){
            if ((i+1) < argc){
                num_verbose = std::stoi(argv[i+1]);
            } else {
                std::cerr<<"Invalid argument for optional parameter --verbose"<<std::endl; 
                return -1;
            }
        }
    }

    if (M <= 0){
        std::cerr<<"Invalid argument for optional parameter --M: Must be positive integer."<<std::endl;
        return -1;
    }
    if (ef_construction <= 0){
        std::cerr<<"Invalid argument for optional parameter --ef: Must be positive integer."<<std::endl;
        return -1;
    }

    unsigned int dim_check;
    unsigned int num_check;
    input.read((char*)&num_check, 4);
    input.read((char*)&dim_check, 4);

    if (N <= 0){
        N = num_check;
    }

    std::clog<<"Reading "<<N<<" points of "<<num_check<<" total points of dimension "<<dim_check<<"."<<std::endl;
    if (num_check != N){std::clog<<"Warning: Using only "<< N << " points of total "<< num_check <<"."<<std::endl;}


    SpaceInterface<float>* space;
    if (space_ID == 0){
        space = new L2Space(dim_check);
    } else {
        space = new InnerProductSpace(dim_check);
    }
    Index<float, int> index(space, N, M);

    auto start = std::chrono::high_resolution_clock::now();
    float *element = new float[dim_check];
    for (int label = 0; label < N; label++) {
        input.read((char*) element, 4*dim_check);
        index.add((void*) element, label, ef_construction, 1000);
        if (num_verbose > 0){
            if (label%num_verbose == 0){std::clog<<"+";}
        }
    }
    std::clog<<std::endl;
    delete[] element;
    input.close();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::clog << "Build time: " << (float)(duration.count())/(1000.0) << " seconds" << std::endl; 

    std::clog << "Saving index to: " << outfilename << std::endl;
    index.save(outfilename);

    return 0;
}