import sys
import h5py
import numpy as np 

filename = sys.argv[1]
normalize = False
if '-normalize' in sys.argv: 
    normalize = True

f = h5py.File(filename, 'r')

# ['distances', 'neighbors', 'test', 'train']
# file format:
train = f['train'][:]
test = f['test'][:]
gtruth = f['neighbors'][:]

print(train.shape,train.dtype)
print(test.shape,test.dtype)
print(gtruth.shape,gtruth.dtype)

if normalize:
    train /= ( np.linalg.norm(train, axis = 1, keepdims = True) + 1e-30 )
    test /= ( np.linalg.norm(test, axis = 1,keepdims = True) + 1e-30)

fname = filename.replace('.hdf5','')
np.save(fname+".train", train)
np.save(fname+".test", test)
np.save(fname+".gtruth", gtruth)

