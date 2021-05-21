#include <stdexcept>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <Index.h>
#include <SpaceInterface.h>

namespace py = pybind11;

template<typename dist_t, typename label_t>
class PyIndex {
  private:
    Index<dist_t, label_t>* index;
    SpaceInterface<dist_t>* space;
    size_t dim;
    int added;

    void getSpaceFromType(std::string& spaceType) {
      if (spaceType == "L2") {
        space = new L2Space(dim);
      } else if (spaceType == "Angular") {
        space = new InnerProductSpace(dim);
      } else {
        throw std::invalid_argument("Invalid Space '" + spaceType + "' used to construct Index");
      }
    }
  
public:
  PyIndex(std::string spaceType, size_t _dim, int _N, int _M): dim(_dim), added(0) {
    getSpaceFromType(spaceType);
    index = new Index<dist_t, label_t>(space, _N, _M);
	}

  PyIndex(std::string spaceType,std::string filename) {
    getSpaceFromType(spaceType);
    index = new Index<dist_t, label_t>(space, filename);
  }

  void Add(py::array_t<float, py::array::c_style | py::array::forcecast> dataset, int ef_construction) {
    if (dataset.ndim() != 2 || dataset.shape(1) != dim) {
      throw std::invalid_argument("Dataset has incorrect dimensions");
    }

    for (size_t n = 0; n < dataset.shape(0); n++) {
      this->index->add((void*)dataset.data(n), added, ef_construction);  
      added++;
    }
  }

  py::array_t<label_t> Search(py::array_t<float, py::array::c_style | py::array::forcecast> queries, int K, int ef_search) {
    if (queries.ndim() != 2 || queries.shape(1) != dim) {
      throw std::invalid_argument("Dataset has incorrect dimensions");
    }
    size_t num_queries = queries.shape(0);

    label_t* results = new label_t[num_queries * K];

    for (size_t q = 0; q < num_queries; q++) {
      std::vector<std::pair<dist_t, label_t>> topK = this->index->search(queries.data(q), K, ef_search);
      for (size_t i = 0; i < topK.size(); i++) {
        results[q * K + i] = topK[i].second;
      }
    }

    py::capsule free_when_done(results, [](void* ptr){ delete ptr;});

    return py::array_t<label_t>(
      {num_queries,(size_t) K},
      {K * sizeof(label_t), sizeof(label_t)},
      results,
      free_when_done
    );
  }

  void Reorder(std::string alg) {
      if (alg =="gorder") {
        this->index->reorder(Index<dist_t, label_t>::GraphOrder::GORDER);
      } else if (alg == "in_deg") {
        this->index->reorder(Index<dist_t, label_t>::GraphOrder::IN_DEG);
      } else if (alg == "out_deg") {
        this->index->reorder(Index<dist_t, label_t>::GraphOrder::OUT_DEG);
      } else if (alg == "rcm") {
        this->index->reorder(Index<dist_t, label_t>::GraphOrder::RCM);
      } else if (alg == "hub_sort") {
        this->index->reorder(Index<dist_t, label_t>::GraphOrder::HUB_SORT);
      } else if (alg == "hub_cluster") {
        this->index->reorder(Index<dist_t, label_t>::GraphOrder::HUB_CLUSTER);
      } else if (alg == "DBG") {
        this->index->reorder(Index<dist_t, label_t>::GraphOrder::DBG);
      } else {
        throw std::invalid_argument("'" + alg + "' is not a supported graph reordering algorithm");
      }
  }

  void Save(std::string filename) {
    this->index->save(filename);
  }

  ~PyIndex() {
    delete index;
    delete space;
  }
};

template<typename label_t>
double compute_recall(py::array_t<label_t> results, py::array_t<label_t> gtruths) {
  double avg_recall = 0.0;
  for (size_t q = 0; q < results.shape(0); q++) {
    double recall = 0.0;
    const label_t* result = results.data(q);
    const label_t* topk = gtruths.data(q);
    for (size_t i = 0; i < results.shape(1); i++) {
      for (size_t j = 0; j < results.shape(1); j++) {
        if (result[i] == topk[j]) {
          recall += 1.0;
          break;
        }
      }
    }
    avg_recall += recall;
  }

  return avg_recall /= (results.shape(0) * results.shape(1));
}

using PyIndexWithTypes = PyIndex<float, int>;

PYBIND11_MODULE(flatnav, m) {
  py::class_<PyIndexWithTypes>(m, "Index")
      .def(py::init<std::string, size_t, int, int>(), py::arg("space"), py::arg("dim"), py::arg("N"), py::arg("M"))
      .def(py::init<std::string, std::string>(), py::arg("space"), py::arg("save_loc"))
      .def("Add", &PyIndexWithTypes::Add)
      .def("Search", &PyIndexWithTypes::Search)
      .def("Reorder", &PyIndexWithTypes::Reorder)
      .def("Save", &PyIndexWithTypes::Save);

  m.def("compute_recall", &compute_recall<int>);
}
