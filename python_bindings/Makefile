# How to use 
# To make all tools: make tools

CXX = g++
CFLAGS= -std=c++11 -Ofast -DHAVE_CXX0X -DNDEBUG -openmp -march=native -fpic -w -ffast-math -funroll-loops -ftree-vectorize -g
LDFLAGS= -L/usr/local/lib/

all: python-bindings

PYBIND_SRC := ./flatnav.cpp
PYBIND_TARGET := ../build/flatnav.so
PYTHON_INC_FLAGS := -I/usr/include/python3.6
PYTHON_LINK_FLAGS := -L /usr/local/Cellar/python@3.9/3.9.4/Frameworks/Python.framework/Versions/3.9/lib/python3.9/config-3.9-darwin/ -lpython3.9
FLATNAV_SRC := ../flatnav

python-bindings: 
	$(CXX) $(CFLAGS) $(shell python3 -m pybind11 --includes) $(PYTHON_INC_FLAGS) -I./$(FLATNAV_SRC) --shared -fPIC $(PYBIND_SRC) $(PYTHON_LINK_FLAGS) -o $(PYBIND_TARGET)

.PHONY: all python-bindings
