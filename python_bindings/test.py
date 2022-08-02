import flatnav
import numpy as np

data = np.load("mnist-784-euclidean.train.npy")
print("Data: ", data.shape)
queries = np.load("mnist-784-euclidean.test.npy")
print("Queries: ", queries.shape)
gtruths = np.load("mnist-784-euclidean.gtruth.npy")
print("Gtruths: ", gtruths.shape)

M = 16
ef_construction = 100
ef_search = 100
K = 100

index = flatnav.Index(space="L2", dim = data.shape[1], N=data.shape[0], M=M)

# labels = np.array([i for i in range(data.shape[0])])
# index.Add(data=data, ef_construction=ef_construction, labels=labels)

index.Add(data=data, ef_construction=ef_construction, labels=None)

index.Reorder("gorder")

index.Save("mnist16.bin")

results = index.Search(queries, K, ef_search)
print("Results: ", results.shape)

mean_recall = 0.0

for query in range(queries.shape[0]):
  recalled = 0
  for k in range(K):
    for i in range(K):
      if results[query, k] == gtruths[query, i]:
        recalled += 1
        break;
  mean_recall += (float(recalled) / float(K))

mean_recall /= queries.shape[0]

print("Python Recall: ", mean_recall)
print("Cpp Recall: ", flatnav.ComputeRecall(results, gtruths))

index2 = flatnav.Index(space="L2", dim=queries.shape[1], save_loc="./mnist16.bin")

results2 = index2.Search(queries, K, ef_search)
print("Recall with Reloaded Index: ", flatnav.ComputeRecall(results2, gtruths))