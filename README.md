## Near Neighbor Graph Reordering

This repository implements a graph near neighbor index and provides various ways to reorder the nodes in the graph to improve query latency. To reproduce our experiments, there are three tools that can be run:

construct - creates a near neighbor index from a data file
reorder - applies graph reordering to permute the node ordering of the index
query - queries the index and computes recall and other performance metrics


The tools are largely self-documenting and will provide help if run without any command line arguments. Note that the reordering tools can generally run without needing access to the dataset, queries or distance metrics (unless profile-guided reordering is used).

1. `$ cd flatnav`
2. `$ cmake -G "Unix Makefiles"`
3. `$ make` 

### Datasets from ANN-Benchmarks

ANN-Benchmarks provides HDF5 files for a standard benchmark of near-neighbor datasets, queries and ground-truth results. To run on these datasets, we provide a set of tools to process numpy (NPY) files: construct_npy, reorder_npy and query_npy.

To generate these NPY files from the HDF5 files provided by ANN-benchmarks, you may use the Python script dump.py, as follows

```python dump.py dataset.hdf5```


### Datasets from Big ANN Benchmarks

The Big ANN Benchmarks competitions from NeurIPS 2021 provides a new set of near-neighbor datasets. There is some overlap with ANN-Benchmarks (e.g. SIFT and DEEP), but the format is often different. For example, Big ANN Benchmarks uses 8-bit integers for SIFT features whlie ANN-Benchmarks uses 32-bit floats.

To process these datasets, you will need to use the appropriate choice of construct_float32, reorder_float32 and query_float32 or construct_uint8, reorder_uint8 and query_uint8.

Note: The original SIFT and DEEP benchmarks from CNRS/IRISA and FAIR use a slightly different format for each vector.


### Using Custom Datasets

The most straightforward way to include a new dataset for this evaluation is to put it into either the ANN-Benchmarks (NPY) format or to put it into the Big ANN-Benchmarks format. The NPY format requires a float32 2-D Numpy array for the train and test sets and an integer array for the ground truth. The Big ANN-Benchmarks format uses the following binary representation. For the train and test data, there is a 4-byte little-endian unsigned integer number of points followed by a 4-byte little-endian unsigned integer number of dimensions. This is followed by a flat list of `num_points * num_dimensions` values, where each value is a 32-bit float or an 8-bit integer (depending on the dataset type). The ground truth files consist of a 32-bit integer number of queries, followed by a 32-bit integer number of ground truth results for each query. This is followed by a flat list of ground truth results.


## Python Binding Instructions
We also provide python bindings for a subset of index types. This is very much a work in progress - the default build may or may not work with a given Pyton configuration. While we've successfully built the bindings on Windows, Linux and MacOS, this will still probably require some customization of the build system. To begin with, follow these instructions:

1. `$ cd python_bindings`
2. `$ make python-bindings`
3. `$ export PYTHONPATH=$(pwd)/build:$PYTHONPATH`
4. `$ python3 python_bindings/test.py`

You are likely to encounter compilation issues depending on your Python configuration. See below for notes and instructions on how to get this working.

### Note on python bindings: 
The python bindings require pybind11 to compile. This can be installed with `pip3 install pybind11`. The command `python3 -m pybind11 --includes` which is included in the Makefile gets the correct include flags for the `pybind11/pybind11.h` header file, as well as the include flags for the `Python.h` header file. On most Linux platforms, the paths in the Makefile should point to the correct include directories for this to work (for the system Python). If the `Python.h` file is not located at the specified include paths (e.g. for a non-system Python installation), then another include path may need to be added (specified by the PYTHON_INC_FLAGS variable in the Makefile). The headers may also need to be installed with `$ sudo apt-get install python3-dev`. 

If you encounter the following error:

`ld: can't open output file for writing: ../build/flatnav.so, errno=2 for architecture x86_64`

The reason is likely that you forgot to make the build directory. Run `mkdir build` in the top-level flatnav directory and re-build the Python bindings.

### Special Instructions for MacOS

On MacOS, the default installation directory (`/usr/lib`) is where the global, system Python libraries are located, but this is often not where we want to perform the installation. If the user has installed their own (non-system) version of Python via Homebrew or a similar tool, the actual Python libraries will be located somewhere else. This will result in many errors similar to the following:

```
Undefined symbols for architecture x86_64:
  "_PyBaseObject_Type...
```

This happens because homebrew does not install into the global installation directory, and we need to explicitly link the libpython object files on MacOS. To fix it, you will need the location of `libpython*.dylib` (where `*` stands in for the Python version). To find them, run 

`sudo find / -iname "libpython*"`

And pick the one corresponding to the version of Python you use. Once you've located the library, add the following to the Makefile:

`PYTHON_LINK_FLAGS := -L /path/to/directory/containing/dylib/ -lpythonX.Y`

For example, on an Intel MacBook, I installed Python 3.9 using Homebrew and found:

`/usr/local/Cellar/python@3.9/3.9.4/Frameworks/Python.framework/Versions/3.9/lib/libpython3.9.dylib`

This means that my link flags are:

`PYTHON_LINK_FLAGS := -L /usr/local/Cellar/python@3.9/3.9.4/Frameworks/Python.framework/Versions/3.9/lib/python3.9/config-3.9-darwin/ -lpython3.9`

If you installed Python in some other place (or if you use the system Python on MacOS), you will probably have a different, non-standard location for `libpython.dylib`. Note that building python bindings on M1 Macs is a work-in-progress, given the switch from x86 to arm64. 

### List of things to do

1. Fix the build system (again - integrate Python libraries into cmake with MacOS fix)
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
13. Automate tests


Note: before doing (4) and (5) check disassembly to see if the compiler is not already doing optimizations.





