#include "tensor_nn.h"

#include "../../hardware/cpu/cpu_convol.h"

#ifdef cGPU
#include "../../hardware/gpu/tensor_cuda.h"
#include "../../hardware/gpu/tensor_cuda_op.h"
#endif


void Conv2D(ConvolDescriptor *D) {
    /////////////////////////////////////////////////////////////////////
    //// Conv2D
    //// Dimensions must be compatible
    //// A is input 4D Tensor, Batch x Channels x Rows x Cols
    //// D is a ConvolDescriptor
    /////////////////////////////////////////////////////////////////////
    if ((D->I->ndim != 4)) msg("Tensors are not 4D", "Tensor::Conv2D");

    D->O->tsem->lock();
    if (D->I->isCPU()) {
        cpu_conv2D(D);
    }
#ifdef cGPU
    else if (D->I->isGPU())
      {
         //gpu_conv2D_old(D);
         gpu_conv2D(D);
      }
#endif
#ifdef cFPGA
    else {

    }
#endif
    D->O->tsem->unlock();
}

void Conv2D_grad(ConvolDescriptor *D) {
    /////////////////////////////////////////////////////////////////////
    //// Conv2D Grad
    //// Dimensions must be compatible
    //// A is input 4D Tensor, Batch x Channels x Rows x Cols
    //// D is a ConvolDescriptor
    /////////////////////////////////////////////////////////////////////
    if ((D->I->ndim != 4)) msg("Tensors are not 4D", "Tensor::Conv2D");

    D->gK->tsem->lock();
    if (D->I->isCPU()) {
        D->gK->set(0.0);
        D->gbias->set(0.0);
        cpu_conv2D_grad(D);
    }
#ifdef cGPU
    else if (D->I->isGPU())
      {
         D->gK->set(0.0);
         D->gbias->set(0.0);
         gpu_conv2D_grad(D);
      }
#endif
#ifdef cFPGA
    else {

    }
#endif
    D->gK->tsem->unlock();
}

void Conv2D_back(ConvolDescriptor *D) {
    /////////////////////////////////////////////////////////////////////
    //// Conv2D Back
    //// Dimensions must be compatible
    //// A is input 4D Tensor, Batch x Channels x Rows x Cols
    //// D is a ConvolDescriptor
    /////////////////////////////////////////////////////////////////////
    if ((D->I->ndim != 4)) msg("Tensors are not 4D", "Tensor::Conv2D");

    D->ID->tsem->lock();
    if (D->I->isCPU()) {
        cpu_conv2D_back(D);
    }
#ifdef cGPU
    else if (D->I->isGPU())
      {
         gpu_conv2D_back(D);
      }
#endif
#ifdef cFPGA
    else {

    }
#endif
    D->ID->tsem->unlock();
}
