// The MIT License (MIT)
//
// Copyright (c) 2019 PRHLT Research Group. Inc. http://www.prhlt.upv.es
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "loss.h"

using namespace std;


LMeanSquaredError::LMeanSquaredError() : Loss("mean_squared_error"){}

void LMeanSquaredError::delta(Tensor *T, Tensor *Y, Tensor *D) {
    //delta: (T-Y)
    Tensor::sum(1.0, T, -1.0, Y, D, 0);
}

float LMeanSquaredError::value(Tensor *T, Tensor *Y) {
    float f;
    // batch error: sum((T-Y)^2)
    Tensor *aux = new Tensor(T->getShape(), T->device);
    Tensor::sum(1.0, T, -1.0, Y, aux, 0);
    Tensor::el_mult(aux, aux, aux, 0);
    f = aux->total_sum();
    delete aux;
    return f;
}