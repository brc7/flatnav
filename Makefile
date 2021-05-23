# How to use 
# To make all tools: make tools

CXX = g++
CFLAGS= -std=c++11 -Ofast -DHAVE_CXX0X -DNDEBUG -openmp -march=native -fpic -w -ffast-math -funroll-loops -ftree-vectorize -ftree-vectorizer-verbose=0 -g
LDFLAGS= -L/usr/local/lib/ -lcnpy -lz

SRCS = 
SRCS_DIR = src/

BUILD_DIR = build/
BIN_DIR = bin/

INC := -I ../flatnav -I/usr/local/include

# List of target executables
TARGETS = construct.cpp query.cpp query_sparse.cpp graphstats.cpp graphdump.cpp reorder.cpp
TARGETS_DIR = tools/

# Everything beyond this point is determined from previous declarations, don't modify
OBJECTS = $(addprefix $(BUILD_DIR), $(SRCS:.cpp=.o))
BINARIES = $(addprefix $(BIN_DIR), $(TARGETS:.cpp=))

$(BUILD_DIR)%.o: $(SRCS_DIR)%.cpp | $(BUILD_DIR:/=)
	$(CXX) $(INC) -c $(CFLAGS) $< -o $@

binaries: $(BINARIES)
tools: $(BINARIES)
targets: $(BINARIES)
all: $(BINARIES)

$(BUILD_DIR:/=):
	mkdir -p $@
$(BIN_DIR:/=): 
	mkdir -p $@

$(BINARIES): $(addprefix $(TARGETS_DIR), $(TARGETS)) $(OBJECTS) | $(BIN_DIR:/=)
	$(CXX) $(INC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(addsuffix .cpp,$(@:$(BIN_DIR)%=$(TARGETS_DIR)%)) -o $@

clean:
	rm -f $(OBJECTS); 
	rm -f $(BINARIES); 

.PHONY: clean targets binaries all 

PYBIND_SRC := ./python_bindings/flatnav.cpp
PYBINd_TARGET := ./build/flatnav.so
PYTHON_INC_FLAGS := -I/usr/include/python3.6
FLATNAV_SRC := ./flatnav

python-bindings: 
	$(CXX) $(CFLAGS) $(shell python3 -m pybind11 --includes) $(PYTHON_INC_FLAGS) -I./$(FLATNAV_SRC) --shared -fPIC $(PYBIND_SRC) -o $(PYBINd_TARGET)

.PHONY: python-bindings

# CFLAGS= -std=c++11 -Ofast -DHAVE_CXX0X -DNDEBUG -openmp -march=native -fpic -w -ffast-math -funroll-loops -ftree-vectorize -ftree-vectorizer-verbose=0 -g
# INCLUDES= -I/usr/local/include # for cnpy include
# CXX=/usr/local/Cellar/gcc/10.2.0/bin/g++-10

# construct: flatnav/Index.h flatnav/SpaceInterface.h flatnav/HashBasedBooleanSet.h tools/construct.cpp
# 	$(CXX) -Wall $(CFLAGS) $(INCLUDES) -L/usr/local/lib/ -lcnpy -lz -o construct tools/construct.cpp

# query: flatnav/Index.h flatnav/SpaceInterface.h flatnav/HashBasedBooleanSet.h tools/query.cpp flatnav/reordering.h
# 	$(CXX) -Wall $(CFLAGS) $(INCLUDES) -L/usr/local/lib/ -lcnpy -lz -o query tools/query.cpp

# query_sparse: flatnav/Index.h flatnav/SpaceInterface.h flatnav/HashBasedBooleanSet.h tools/query_sparse.cpp
# 	$(CXX) -Wall $(CFLAGS) $(INCLUDES) -L/usr/local/lib/ -lcnpy -lz -o query_sparse tools/query_sparse.cpp

# graphstats: flatnav/Index.h flatnav/SpaceInterface.h flatnav/HashBasedBooleanSet.h tools/graphstats.cpp
# 	$(CXX) -Wall $(CFLAGS) $(INCLUDES) -L/usr/local/lib/ -lcnpy -lz -o gs tools/graphstats.cpp

# graphdump: flatnav/Index.h flatnav/SpaceInterface.h flatnav/HashBasedBooleanSet.h tools/graphdump.cpp
# 	$(CXX) -Wall $(CFLAGS) $(INCLUDES) -L/usr/local/lib/ -lcnpy -lz -o gd tools/graphdump.cpp

# reorder: flatnav/Index.h flatnav/SpaceInterface.h flatnav/HashBasedBooleanSet.h tools/reorder.cpp
# 	$(CXX) -Wall $(CFLAGS) $(INCLUDES) -L/usr/local/lib/ -lcnpy -lz -o reorder tools/reorder.cpp

# clean:
# 	rm *.o
