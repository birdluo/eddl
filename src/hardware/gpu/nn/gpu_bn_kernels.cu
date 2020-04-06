/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.3
* copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: October 2019
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cuda.h>

#include "gpu_nn_kernels.h"
#include "../gpu_kernels.h"


__global__ void bn_permute_channels_last(float *src, float *dest,int b,int z,int r,int c,long int size)
{
  long int thread_id_x = threadIdx.x+blockIdx.x*blockDim.x;

  if (thread_id_x < size) {
    int bo=thread_id_x/(z*r*c);
    int zom=thread_id_x%(z*r*c);
    int zo=zom/(r*c);
    int rom=zom%(r*c);
    int ro=rom/c;
    int co=rom%c;

    //int pos=(zo*(b*r*c))+(bo*(r*c))+(ro*c)+co;
    int pos=(bo*(r*c*z))+(ro*(c*z))+(co*z)+zo;
    dest[pos]=src[thread_id_x];

  }


}

__global__ void bn_permute_channels_first(float *src, float *dest,int b,int z,int r,int c,long int size)
{
  long int thread_id_x = threadIdx.x+blockIdx.x*blockDim.x;

  if (thread_id_x < size) {
    int bo=thread_id_x/(z*r*c);
    int zom=thread_id_x%(z*r*c);
    int zo=zom/(r*c);
    int rom=zom%(r*c);
    int ro=rom/c;
    int co=rom%c;

    //int pos=(zo*(b*r*c))+(bo*(r*c))+(ro*c)+co;
    int pos=(bo*(r*c*z))+(ro*(c*z))+(co*z)+zo;
    dest[thread_id_x]=src[pos];

  }


}
