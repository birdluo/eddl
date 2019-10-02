
/////////////////////////////////////////////////////////////////////////////
// This file is part of EDDLL an European Distributed Deep Learning Library.
// Developed within the DeepHealth project.
// Boosting AI in Europe.
//
// Main authors and developers:
//      Roberto Paredes: rparedes@prhlt.upv.es
//      Joan Ander Gómez: jon@prhlt.upv.es
//
//
// Collaborators:
//      Salva Carrión: salcarpo@prhlt.upv.es
//      Mario Parreño: maparla@prhlt.upv.es
//
//
// To collaborate please contact rparedes@prhlt.upv.es
//
/////////////////////////////////////////////////////////////////////////////


#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>

#include "cpu_nn.h"

void cpu_relu(Tensor *A, Tensor *B){
    for (int i = 0; i < A->size; i++) {
        if (A->ptr[i] > 0.0) B->ptr[i] = A->ptr[i];
        else B->ptr[i] = 0.0;
    }
}

void cpu_d_relu(Tensor *D, Tensor *I, Tensor *PD){
    for (int i = 0; i < D->size; i++) {
        if (I->ptr[i] > 0.0) PD->ptr[i] = D->ptr[i];
        else PD->ptr[i] = 0.0;
    }
}


void cpu_softmax(Tensor *A, Tensor *B) {
    float max, sum;

    for (int i = 0; i < A->shape[0]; i++) {
        max = (*A->ptr2).col(i).maxCoeff();
        for (int j = 0; j < A->shape[1]; j++)
            (*B->ptr2)(j, i) = std::exp((*A->ptr2)(j, i) - max);

        sum = (*B->ptr2).col(i).sum();
        for (int j = 0; j < B->shape[1]; j++)
            (*B->ptr2)(j, i) = (*B->ptr2)(j, i) / sum;
    }
}

void cpu_d_softmax(Tensor *D, Tensor *I, Tensor *PD) {
    PD->tsem->lock();

    for (int i = 0; i < D->size; i++)
        PD->ptr[i] += D->ptr[i] * (I->ptr[i] * (1.0 - I->ptr[i]));

    PD->tsem->unlock();
}