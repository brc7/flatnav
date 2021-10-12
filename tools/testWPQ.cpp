#include <iostream> 
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <fstream>
#include <utility>

#include "../flatnav/WeightedPriorityQueue.h"
#include <algorithm>
#include <string>


int main(int argc, char **argv){

    int n = 20;
    WeightedPriorityQueue<int> Q(n);
    Q.print();

    Q.decrement(0,0.2);
    Q.print();
    Q.increment(10,0.2);
    Q.print();
    Q.increment(1,1.8);
    Q.print();
    Q.increment(9,2.0);
    Q.print();
    Q.increment(10,1.8);
    Q.print();

    Q.pop();
    std::cout<<Q.size()<<std::endl;
    Q.print();
    Q.pop();
    std::cout<<Q.size()<<std::endl;
    Q.print();
    Q.pop();
    std::cout<<Q.size()<<std::endl;
    Q.print();


}
