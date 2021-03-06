# PyEDDL

**THIS IS NOT THE PYEDDL REPOSITORY**

To install the PyEDDL run the following commands (Ubuntu/Debian)

> Note: If you're using conda, first deactivate the environment by typing: `conda deactivate`

```bash
git clone https://gist.github.com/c4d599396297f7f6e322ab2373174547.git; 
cd c4d599396297f7f6e322ab2373174547/
chmod +x install_pyeddl.sh
sudo sh install_pyeddl.sh
```


Then, create a new file `mnist_mlp.py` with this content:

```python
import sys

import pyeddl._core.eddl as eddl
import pyeddl._core.eddlT as eddlT


eddl.download_mnist()

num_classes = 10
batch_size = 1000
epochs = 10
in_ = eddl.Input([784])

layer = in_
layer = eddl.LeakyReLu(eddl.Dense(layer, 1024))
layer = eddl.LeakyReLu(eddl.Dense(layer, 1024))
layer = eddl.LeakyReLu(eddl.Dense(layer, 1024))
out = eddl.Softmax(eddl.Dense(layer, num_classes))
net = eddl.Model([in_], [out])

eddl.build(
    net,
    eddl.rmsprop(0.01),
    ["soft_cross_entropy"],
    ["categorical_accuracy"],
    eddl.CS_CPU([1], "low_mem")
)

eddl.summary(net)
eddl.plot(net, "model.pdf")

x_train = eddlT.load("mnist_trX.bin")
y_train = eddlT.load("mnist_trY.bin")
x_test = eddlT.load("mnist_tsX.bin")
y_test = eddlT.load("mnist_tsY.bin")

eddlT.div_(x_train, 255.0)
eddlT.div_(x_test, 255.0)

eddl.fit(net, [x_train], [y_train], batch_size, epochs)
eddl.evaluate(net, [x_test], [y_test])

```

...and now, you can run it with:


```bash
sudo python3 mnist_mlp.py 
```


## Troubleshoot

If the previous `install_pyeddl.sh` script doesn't install the EDDL for GPU,
edit that file and replace this line:

```bash
cmake .. -DBUILD_PROTOBUF=ON -DBUILD_EXAMPLES=ON
```

...with this one:

```bash
CUDA_TOOLKIT=PATH WHERE THE CUDA TOOLKIT IS INSTALLED
CUDA_COMPILER=PATH WHERE THE CUDA COMPILER IS INSTALLED
cmake .. -DBUILD_PROTOBUF=ON -DBUILD_EXAMPLES=ON -DBUILD_TARGET=GPU -DCUDA_TOOLKIT_ROOT_DIR=$CUDA_TOOLKIT -DCMAKE_CUDA_COMPILER=$CUDA_COMPILER
```

> Example for CUDA 10:
>
> `CUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda-10`
>
> `CUDA_COMPILER=/usr/local/cuda-10/bin/nvcc` 
>
>  Try `which nvcc` to know the path

Then save and run the script again:

```bash
sudo sh install_pyeddl.sh
```
