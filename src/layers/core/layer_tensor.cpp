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

#include "layer_core.h"
#include "../merge/layer_merge.h"  // TODO: Review dependency (LADD)


using namespace std;


int LTensor::total_layers = 0;


// From file
LTensor::LTensor(string fname) : LinLayer("ltensor" + to_string(total_layers), DEV_CPU) {
    data = input = output = new Tensor(fname);
}

// From list of shape
LTensor::LTensor(const initializer_list<int> &init, int dev) : LinLayer("ltensor" + to_string(total_layers), dev) {
    data = input = output = new Tensor(init, dev);
    delta = new Tensor(init, dev);
}

LTensor::LTensor(const vector<int> shape, float *fptr,int dev) : LinLayer("ltensor" + to_string(total_layers), dev) {
    data = input = output = new Tensor(shape, fptr, dev);
    delta = new Tensor(shape, dev);
}

// From vector<int>
LTensor::LTensor(const vector<int> shape, int dev) : LinLayer("ltensor" + to_string(total_layers), dev) {
    data = input = output = new Tensor(shape, dev);
    delta = new Tensor(shape, dev);
}


/*
void Ltensor::mult2D(LTensor *A,...){
  Tensor::mult2d(A->output, tA,B->output,tB,C->output,incC);
}
*/
void LTensor::resize(int batch){
  Layer::resize(batch);
}


// From Layer
LTensor::LTensor(Layer *l) : LinLayer("ltensor" + to_string(total_layers), l->dev) {
    data = input = output = l->output;
    delta = l->delta;
}


/// OP OVERLOAD
LTensor LTensor::operator+(LTensor L) {
    vector<Layer *> vl;

    vl.push_back(this);
    vl.push_back(&L);

    LTensor *l = new LTensor(new LAdd(vl, "", DEV_CPU));

    return *l;
}





//////
