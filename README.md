## Python Binding Instructions
1. `$ make python-bindings`
2. `$ ./test.py

### Note on python bindings: 
The python bindings require pybind11 to compile. This can be installed with `pip3 install pybind11`. The command `python3 -m pybind11 --includes` which is included in the Makefile gets the correct include flags for the `pybind11/pybind11.h` header file, as well as the include flags for the `Python.h` header file. If the `Python.h` file is not located at the specified include paths, then another include path may need to be added (specified by the PYTHON_INC_FLAGS variable in the Makefile), or it may need to be installed with `$ sudo apt-get install python3-dev`. 

Note that all of these files/modules are installed and located in the correct locations on terminator5 and the python bindings can be compiled without modifcation to the Makefile there. 

## Instructions with CMake 

1. `$ cd flatnav`
2. `$ mkdir bin && cd bin`
3. `$ cmake ..`
4. `$ make` 

This will generate an executable called `construct` and an executable called `query`.


### List of things to do

1. Fix the build system
2. Fix off-by-one bug in HashBasedBooleanSet
3. The node layout is currently [data] [links] [label] (all flat). There is no reason to have the labels be in the same data space. Does this improve cache performance?
4. Make the distance function pointer in SpaceInterface a template like SpaceInterface<dist_t, dimensions> so that dim is known at compile-time.
This would permit us to do more optimizations and also we don't have to pass a stupid dimensionality void pointer around. Also, the distance function is called as a FUNCTION POINTER. Profiling indicates that it's not a huge overhead when compared with the rest of the search but maybe we want to do it somehow else
5. Make the number of links M a template parameter like Index<dist_t, label_t, M> so that we can do more aggressive loop unrolling and inlining
6. Write my own priority queue that supports iteration (probably read-only const access). This would save us some pain in a lot of places and write shorter code.
7. Fix include guards and stop relying on #pragma
8. Put everything in a namespace
9. Parallel construction algorithms
10. Consider running beam search candidates in parallel on multi-core machines
11. Carefully review all templates to be sure they're necessary. Do we really need a templated label type? (A: Possibly - people may use strings as labels, or if you wanted to use this for kNN classifications you could use class names as labels. The label is any non-vector data associated with each point)
12. Python bindings

Note: before doing (4) and (5) check disassembly to see if the compiler is not already doing optimizations.





