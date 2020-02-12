<p style="text-align: center;">
  <img src="https://raw.githubusercontent.com/salvacarrion/salvacarrion.github.io/master/assets/hot-linking/logo-eddl.png" alt="EDDL" height="140" width="300">
</p>

-----------------

**EDDL** is an open source library for numerical computation tailored to the healthcare domain.

For more information about DeepHealth project go to: [deephealth-project.eu](https://deephealth-project.eu/)

**Continuous build status:**

| System  |  Compiler  | Status |
|:-------:|:----------:|:------:|
| Windows (CPU) | VS 15.9.11 | [![Build Status](https://jenkins-master-deephealth-unix01.ing.unimore.it/badge/job/DeepHealth/job/eddl/job/master/windows_end?)](https://jenkins-master-deephealth-unix01.ing.unimore.it/job/DeepHealth/job/eddl/job/master/)      |
| Linux (CPU)   | GCC 5.5.0  | [![Build Status](https://jenkins-master-deephealth-unix01.ing.unimore.it/badge/job/DeepHealth/job/eddl/job/master/linux_end?)](https://jenkins-master-deephealth-unix01.ing.unimore.it/job/DeepHealth/job/eddl/job/master/)        |
| Windows (GPU) | VS 15.9.11 | [![Build Status](https://jenkins-master-deephealth-unix01.ing.unimore.it/badge/job/DeepHealth/job/eddl/job/master/windows_gpu_end?)](https://jenkins-master-deephealth-unix01.ing.unimore.it/job/DeepHealth/job/eddl/job/master/)  |
| Linux (GPU)   | GCC 5.5.0  | [![Build Status](https://jenkins-master-deephealth-unix01.ing.unimore.it/badge/job/DeepHealth/job/eddl/job/master/linux_gpu_end?)](https://jenkins-master-deephealth-unix01.ing.unimore.it/job/DeepHealth/job/eddl/job/master/)    |


**Documentation:**
- [Available NN features](https://github.com/deephealthproject/eddl/blob/master/eddl_progress.md)
- [Available Tensor features](https://github.com/deephealthproject/eddl/blob/master/eddl_progress_tensor.md)
- [Doyxigen documentation](http://imagelab.ing.unimore.it/eddl/)



## Installation

**Try this:**

You will need a [anaconda package manager](https://docs.conda.io/en/latest/miniconda.html)

```bash
git clone https://github.com/deephealthproject/eddl.git
cd eddl
conda env create -f environment.yml
conda activate eddl
mkdir build
cd build
cmake .. -DBUILD_TARGET=CPU  # {CPU, GPU, FPGA}
make -j 4  # num_cores
sudo make install
```


**Or step by step:**

### Download source code

To get the source, download one of the release .tar.gz or .zip packages in the release page:

```bash
git clone https://github.com/deephealthproject/eddl.git
```

### Prerequisites

To build EDDL from source, the following tools are needed:

- C++11-standard-compliant compiler
- CMake >= 3.9.2
- CUDA Toolkit >= 8.0
- Eigen3
- BLAS
- LAPACK
- graphviz
- wget
- zlib
- Protobuf
- Google tests
- Google Benchmark

These dependencies can be installed either manually or using a conda package manager (recommended).



### Anaconda package manager (recommended)

The required libraries are easier to install if you use using a [anaconda package manager](https://docs.conda.io/en/latest/miniconda.html)).
Once conda is installed in your system, you can use the `environment.yml` file inside the `eddl/`folder to install the requirements.

To create and activate the conda environment use the following commands:

```bash
conda env create -f environment.yml
conda activate eddl
```

> Note:
> If the conda environment misses some dependency, please write an issue and complete the installation manually


### Manual management of dependencies

Regardless of your platform, install:

- CUDA: https://developer.nvidia.com/cuda-toolkit
- Google benchmark: https://github.com/google/benchmark
- Protobuf: https://github.com/protocolbuffers/protobuf/blob/master/src/README.md

Then, on Ubuntu/Debian install:

```
sudo apt-get install build-essential git graphviz wget zlib1g-dev cmake  # Utilities
sudo apt-get install libblas-dev liblapack-dev  # BLAS + LAPACK
sudo apt-get install libeigen3-dev  # Eigen3
sudo apt-get install libgtest-dev  # Google tests
```

Or, on MacOS install:

```
brew install git graphviz wget zlib cmake  # Utilities
brew install openblas lapack # BLAS + LAPACK
brew install eigen
# Install Google Tests: https://github.com/google/googletest
```



## Compilation


To build and install the EDDL library from source, execute the following commands inside the `eddl/` folder:

```bash
mkdir build
cd build
cmake .. -DBUILD_TARGET=CPU  # {CPU, GPU, FPGA}
make -j 4  # num_cores
sudo make install
```

> Note:
> To known the number of logical cores type: `nproc` (linux) or `sysctl -n hw.logicalcpu` (mac os)


### Building flags


#### Backend support

**CPU support:**
By default the EDDL is build for CPU. If you have any problem and want to compile for CPU, try adding `BUILD_TARGET=CPU` to your cmake options.

```bash
-DBUILD_TARGET=CPU
```

**GPU (CUDA) support:**
If you have CUDA installed, you can build EDDL with GPU support by adding `BUILD_TARGET=GPU` to your cmake options.

```bash
-DBUILD_TARGET=GPU
```

**FPGA support:**
If available, you can build EDDL with FPGA support by adding `BUILD_TARGET=FPGA` to your cmake options.

```bash
-DBUILD_TARGET=FPGA
```

> Not yet implemented


#### Additional flags

These flags can enable additional features of the EDDL or help you troubleshooting the installation.

**C++ compiler::**
If you have problems with the default g++ compiler, try setting `EIGEN3_INCLUDE_DIR`, such as:

```bash
-DCMAKE_CXX_COMPILER=/path/to/c++compiler
```

**Eigen3:**
At the core of many numerical operations, we use [Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page).
If CMake is unable to find Eigen3 automatically, try setting `Eigen3_DIR`, such as:

```bash
-DEigen3_DIR=/path/to/eigen
```

**Intel MKL:**
EDDL can leverage Intel's MKL library to speed up computation on the CPU.

To use MKL, include the following cmake option:

```bash
-DMKL=TRUE
```

If CMake is unable to find MKL automatically, try setting MKL_ROOT, such as:

```bash
-DMKL_ROOT="/path/to/MKL"
```

**CUDA:**
If CMake is unable to find CUDA automatically, try setting `CUDA_TOOLKIT_ROOT_DIR`, such as:

```bash
-DCUDA_TOOLKIT_ROOT_DIR=/path/to/cuda
```

**Build examples:**
To compile the examples, use the setting `BUILD_EXAMPLES`, such as:

```bash
-DBUILD_EXAMPLES=ON
```

> Notes: The examples can be found in `build/targets/`


**Build tests:**
To compile the tests, use the setting `BUILD_TESTS`, such as:

```bash
-DBUILD_TESTS=ON
```

**Build shared library:**
To compile the EDDL as a shared library, use the setting `BUILD_SHARED_LIB`, such as:

```bash
-DBUILD_SHARED_LIB=ON
```


### Windows specific installation

Default for `Visual Studio 15 2017` build environment is x86, while EDDL requires x64. This can be changed by typing `cmake -A x64 .` as cmake command.

On Windows, the POSIX threads library is required. Path to this library can be specified to cmake as follows: `env PTHREADS_ROOT=path_to_pthreads cmake -A x64 .`
The PThreads library can be found at [https://sourceforge.net/projects/pthreads4w/](https://sourceforge.net/projects/pthreads4w/).


### Docker

To build the image, run the following command from the `eddl/` folder:

```
docker build -t eddl .
```

Then, you can execute it using:

```
docker run -it eddl /bin/bash
```

Or mount it, if you want to **edit the code** in the host machine:

```
docker run -it -v $(pwd):/eddl/ eddl /bin/bash
```


## Getting started

You can find examples in  `examples/`.

```C++
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "apis/eddl.h"
#include "apis/eddlT.h"

using namespace eddl;

int main(int argc, char **argv) {

    // Download mnist
    download_mnist();

    // Settings
    int epochs = 1;
    int batch_size = 100;
    int num_classes = 10;

    // Define network
    layer in = Input({784});
    layer l = in;  // Aux var

    l = LeakyReLu(Dense(l, 1024));
    l = LeakyReLu(Dense(l, 1024));
    l = LeakyReLu(Dense(l, 1024));

    layer out = Activation(Dense(l, num_classes), "softmax");
    model net = Model({in}, {out});

    // dot from graphviz should be installed:
    plot(net, "model.pdf");

    // Build model
    build(net,
          rmsprop(0.01), // Optimizer
          {"soft_cross_entropy"}, // Losses
          {"categorical_accuracy"} // Metrics
          //CS_GPU({1}) // one GPU
          //CS_CPU() // CPU with maximum threads availables
    );
    toGPU(net,{1},100,"low_mem"); // In two gpus, syncronize every 100 batches, low_mem setup

    // View model
    summary(net);

    // Load dataset
    tensor x_train = eddlT::load("trX.bin");
    tensor y_train = eddlT::load("trY.bin");
    tensor x_test = eddlT::load("tsX.bin");
    tensor y_test = eddlT::load("tsY.bin");

    // Preprocessing
    eddlT::div_(x_train, 255.0);
    eddlT::div_(x_test, 255.0);

    // Train model
    fit(net, {x_train}, {y_train}, batch_size, epochs);

    // Evaluate
    evaluate(net, {x_test}, {y_test});
}
```




## Python wrapper

If you are not a C++ fan, try [PyEDDL](https://github.com/deephealthproject/pyeddl), a python wrapper for this library.


## FAQs

- **When I run an example from `examples/` I get `segmentation fault (core dumped)`**:
    - **CPU**: This is probably because your processor does not support
    AVX instructions. Try to compile the source with the optimization flags: `OPT=2` or `OPT=3` (uppercase).
    - **GPU**: Make sure you are using the computing service: `CS_GPU`.
- **Protobuf doesn't work/compilation error(temporal fix)**:
    1) Make sure you have `protbuf` and `libprotobuf` installed
    2) Go to `src/serialization/onnx/` and delete these files: `onnx.pb.cc` and `onnx.pb.cc`.
    3) Rebuild them using `protoc --cpp_out=. onnx.proto` (you need to be at `src/serialization/onnx/`)
