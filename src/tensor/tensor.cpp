/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.2
* copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: October 2019
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/

#include <iostream>

#include "tensor.h"
#include "../utils.h"

#ifdef cGPU
#include "../hardware/gpu/gpu_tensor.h"
#include "../hardware/gpu/gpu_hw.h"
#include "../hardware/gpu/nn/gpu_nn.h"
#endif

using namespace std;

// TODO: Don't like here
int initcuda[MAX_GPUS] = {0, 0, 0, 0, 0, 0, 0, 0};
int linpos;
extern ostream &operator<<(ostream &os, const vector<int> shape);


Tensor::Tensor() : device(DEV_CPU), ndim(0), size(0) {}

Tensor::Tensor(const vector<int> &shape, float *fptr, int dev){
    /*
     * Important! If we are creating a GPU tensor, "fptr" must point to a GPU pointer.
     */
#ifndef cGPU
    if ((dev > DEV_CPU)&&(dev<DEV_FPGA)) {
        fprintf(stderr, "Not compiled for GPU\n");
        exit(0);
    }
#endif
#ifndef cFPGA
    if (dev >= DEV_FPGA) {
        fprintf(stderr, "Not compiled for FPGA\n");
        exit(0);
    }
#endif

    // Update values
    updateDevice(dev);
    updateShape(shape);
    updateSize();
    updateStrides();
    updateData(fptr);

    tsem = new mutex();
}

// From shape and device
Tensor::Tensor(const vector<int> &shape, int dev):Tensor(shape, nullptr, dev){}

// From shape and Tensor (sharing ptr)
Tensor::Tensor(const vector<int> &shape, Tensor *T):Tensor(shape,T->ptr,T->device) {}


void Tensor::updateDevice(int dev){
    this->device = dev;
}

void Tensor::updateShape(const vector<int> &shape){
    this->shape = vector<int>(shape);
    this->ndim = this->shape.size();
}

void Tensor::updateSize() {
    this->size = 1;

    for(auto &d : this->shape) {
        this->size *= d;
    }
}

void Tensor::updateStrides() {
    this->stride.clear();  // Remove all elements

    int new_size = this->size;
    for(int i=0;i<ndim;i++) {
        new_size /= shape[i];
        this->stride.push_back(new_size);
    }
}

void Tensor::updateData(float *fptr){

    if (isCPU()) {
        // If null => Reserve memory
        // else => point to data
        if (fptr==nullptr) { this->ptr = get_fmem(this->size,"Tensor::Tensor"); }
        else { this-> ptr = fptr; };

        // For 2 dimensions, map to data to Eigen for efficiency
        // Efficient operations will be done over ptr2, which also points to ptr
        if (this->ndim == 2) {
            this->ptr2=(Eigen::MatrixXf*)new Eigen::Map<Eigen::MatrixXf>(this->ptr, this->shape[1], this->shape[0]);
        }
    }
#ifdef cGPU
    else if (isGPU())
        {
          gpu_device=this->device-DEV_GPU;
          if (!initcuda[gpu_device]){
              gpu_init(gpu_device);
              initcuda[gpu_device]=1;
          }

          // If null => Reserve memory
          // else => point to data  | CAREFUL! This pointer MUST be a GPU pointer. We cannot check it.
          if (fptr == nullptr) { this->ptr = gpu_create_tensor(gpu_device, this->size); }
          else { this->ptr = fptr; }

        }
#endif
#ifdef cFPGA
    else {
        // create FPGA Tensor
      }
#endif
}

void Tensor::toCPU(int dev){
#ifdef cGPU
    if (isGPU())
      {
        this->device = dev;

        float *cpu_ptr = get_fmem(size, "Tensor::toCPU");
        float *gpu_ptr = ptr;

        if (ndim == 2) {
            ptr2=(Eigen::MatrixXf*)new Eigen::Map<Eigen::MatrixXf>(cpu_ptr, shape[1], shape[0]);
        }

        gpu_copy_from_gpu(this, cpu_ptr);
        this->ptr = cpu_ptr;
        gpu_delete_tensor(gpu_device,gpu_ptr);

      }
#endif
#ifdef cFPGA
    else {

    }
#endif
}

void Tensor::toGPU(int dev){
#ifdef cGPU
    if (isCPU()) {
        this->device = dev;
        this->gpu_device = this->device - DEV_GPU;

        float *cpu_ptr = ptr;
        float *gpu_ptr = gpu_create_tensor(this->gpu_device, this->size);

        if (!initcuda[gpu_device]){
            gpu_init(gpu_device);
            initcuda[gpu_device] = 1;
        }

        this->ptr = gpu_ptr;
        gpu_copy_to_gpu(cpu_ptr, this);
        delete cpu_ptr;
    }
    else if (isGPU())
      {
//        printf("Tensor already in GPU\n");
      }
#endif
#ifdef cFPGA
    else {

    }
#endif
}

Tensor* Tensor::clone(){
    Tensor* t_new = new Tensor(this->shape, this->device);
    Tensor::copy(this, t_new);
    return t_new;
}


Tensor::~Tensor() {
    if (isCPU()) {
      delete ptr;
    }
#ifdef cGPU
    else if (isGPU())
      {
        gpu_delete_tensor(gpu_device,ptr);
      }
#endif
#ifdef cFPGA
    else {
      // delete FPGA Tensor
    }
#endif
    delete tsem;
}

int Tensor::isCPU() { return (device == DEV_CPU); }

int Tensor::isGPU() { return ((device >= DEV_GPU) && (device < DEV_FPGA)); }

int Tensor::isFPGA() { return (device >= DEV_FPGA); }

vector<int> Tensor::getShape() {
    return vector<int>(this->shape);
}

void Tensor::info() {
    int cols = 15;
    cout << "-------------------------------" << endl;
    cout << setw(cols) << left << "class: "        << "Tensor" << endl;
    cout << setw(cols) << left << "ndim: "         << this->ndim << endl;
    cout << setw(cols) << left << "shape: "        << "(" << printVector<int>(this->shape) << ")" << endl;
    cout << setw(cols) << left << "strides: "      << "(" << printVector<int>(this->stride) <<  ")" << endl;
    cout << setw(cols) << left << "itemsize: "     << this->size << endl;
    cout << setw(cols) << left << "contiguous: "   << true << endl; // for future
    cout << setw(cols) << left << "order: "        << 'C' << endl;  // C=>C order, F=>Fortran order
    cout << setw(cols) << left << "data pointer: " << &this->ptr << endl;
    cout << setw(cols) << left << "type: "         << "float" << " (" << sizeof(float) << " bytes)" << endl;
    cout << setw(cols) << left << "device: "         << this->getStrDevice() << " (code = " << this->device << ")" << endl;
    cout << "-------------------------------" << endl;
}

void Tensor::print() {

    if (isCPU()) {
        if (ndim == 1)
            for (int i = 0; i < shape[0]; ++i)
                printf("%f ", ptr[i]);
        else if (ndim == 2) {
            cout << (*ptr2).transpose() << "\n";
        } else {
            int i;
            for (i = 0; i < size; ++i)
                printf("%f ", ptr[i]);
            printf("\n");
        }
    }
#ifdef cGPU
    else if (isGPU())
      {
        gpu_set_device(gpu_device);
        float *v= get_fmem(size,"Tensor::Tensor");
        cudaMemcpy(v,ptr,size*sizeof(float),cudaMemcpyDeviceToHost);
        if (ndim==2)
          {
            int i,j,p=0;
            for(i=0;i<shape[0];++i)
              {
                for(j=0;j<shape[1];++j,++p)
                  printf("%f ",v[p]);
                  printf("\n");
              }
          }
        else
          {
            int i;
            for(i=0;i<size;++i)
              printf("%f ",v[i]);
              printf("\n");
          }
          delete v;
      }
#endif
#ifdef cFPGA
    else {

    }
#endif
    cout << "\n";
}

string Tensor::getStrDevice(){
    if ((this->device >= DEV_CPU) && (this->device < DEV_GPU)) { return "CPU"; }
    else if ((device >= DEV_GPU) && (this->device < DEV_FPGA)) { return "GPU"; }
    else if (this->device >= DEV_FPGA) { return "FPGA"; }
    return "unknown";
}

int Tensor::get_mode(string mode){
    if(mode == "constant"){
        // (k k k k | a b c d | k k k k)
        // The input is extended by filling all values beyond the edge with the same constant value, defined by the cval parameter.
        return 0;
    }else if(mode == "reflect"){
        // (d c b a | a b c d | d c b a)
        // The input is extended by reflecting about the edge of the last pixel.
        return 1;
    }else if(mode == "nearest"){
        // (a a a a | a b c d | d d d d)
        // The input is extended by replicating the last pixel.
        return 2;
    }else if(mode == "mirror"){
        // (d c b | a b c d | c b a)
        // The input is extended by reflecting about the center of the last pixel.
        return 3;
    }else if(mode == "wrap"){
        // (a b c d | a b c d | a b c d)
        // The input is extended by wrapping around to the opposite edge.
        return 4;
    }else if(mode == "original"){
        // (o o o o | a b c d | o o o o)
        // The input is extended by filling all values beyond the edge with the original values
        return 5;
    }else {  // constant
        return -1;
    }
}


bool Tensor::isSquared(Tensor *A){
    int last_dim = A->shape[0];
    for(int i=0; i<A->ndim; i++){
        if(last_dim!=A->shape[i]){
            return false;
        }
    }
    return true;
}
