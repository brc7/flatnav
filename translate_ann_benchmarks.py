import numpy as np

# Workaround to demonstrate how to turn ANN-benchmarks HDF5 format into
# the fbin / u8bin format used by NeurIPS Big ANN-benchmarks. This might
# be necessary if you are on a platform with an unresolved cnpy bug (e.g. M1 Mac)

data = np.load('sift-128-euclidean.train.npy')
uint8_data = data.astype('uint8')
float32_data = data.astype('float32')

queries = np.load('sift-128-euclidean.test.npy')
uint8_queries = queries.astype('uint8')
float32_queries = queries.astype('float32')

gtruth = np.load('sift-128-euclidean.gtruth.npy')
uint32_gtruth = gtruth.astype('uint32')

with open('sift1M.data.u8bin', 'wb') as f:
	metadata = np.array([uint8_data.shape[0], uint8_data.shape[1]], dtype = np.uint32)
	f.write(metadata.tobytes())
	for thing in uint8_data:
		f.write(thing.tobytes())

with open('sift1M.queries.u8bin', 'wb') as f:
	metadata = np.array([uint8_queries.shape[0], uint8_queries.shape[1]], dtype = np.uint32)
	f.write(metadata.tobytes())
	for thing in uint8_queries:
		f.write(thing.tobytes())

with open('sift1M.gtruth.u32bin', 'wb') as f:
	metadata = np.array([uint32_gtruth.shape[0], uint32_gtruth.shape[1]], dtype = np.int32)
	f.write(metadata.tobytes())
	for thing in uint32_gtruth:
		f.write(thing.tobytes())

with open('sift1M.data.fbin', 'wb') as f:
	metadata = np.array([float32_data.shape[0], float32_data.shape[1]], dtype = np.uint32)
	f.write(metadata.tobytes())
	for thing in float32_data:
		f.write(thing.tobytes())

with open('sift1M.queries.fbin', 'wb') as f:
	metadata = np.array([float32_queries.shape[0], float32_queries.shape[1]], dtype = np.uint32)
	f.write(metadata.tobytes())
	for thing in float32_queries:
		f.write(thing.tobytes())

