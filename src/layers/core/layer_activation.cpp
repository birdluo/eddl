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

using namespace std;

int LActivation::total_layers = 0;

LActivation::LActivation(Layer *parent, string act, string name, int dev) : LinLayer(name, dev) {

    // Set default name
    if(name.empty()) this->name = "activation" + to_string(++total_layers);

    this->act = act;

    input = parent->output;
    output = new Tensor(input->getShape(), dev);
    delta = new Tensor(output->getShape(), dev);
    delta_bp = 0;

    parent->addchild(this);
    addparent(parent);
}
// virtual
void LActivation::resize(int batch){
  Layer::resize(batch);
}


void LActivation::forward() {

    if (act == "relu")
        Tensor::ReLu(input, output);
    else if (act == "softmax") {
        Tensor::Softmax(input, output);
    }
}


void LActivation::backward() {


    if (parent.size()) {
        if (delta_bp) {
            Tensor::inc(delta, parent[0]->delta);
        } else {
            if (act == "relu")
                Tensor::D_ReLu(delta, input, parent[0]->delta);
            else if (act == "softmax")
                Tensor::D_Softmax(delta, output, parent[0]->delta);
        }
    }
}


Layer *LActivation::share(int c, int bs, vector<Layer *> p) {

    LActivation *n = new LActivation(p[0], act, "share_" + to_string(c) + name, dev);
    n->orig = this;
    n->delta_bp = delta_bp;

    return n;
}

Layer *LActivation::clone(int c, int bs, vector<Layer *> p, int todev) {

    LActivation *n = new LActivation(p[0], act, "clone_" + to_string(todev) + name, todev);
    n->orig = this;
    n->delta_bp = delta_bp;

    return n;
}


string LActivation::plot(int c) {
    string s;

    if (c) s = name + " [label=" + "\"" + name + "\",style=filled,fontsize=12,fillcolor=LightBlue,shape=box]";
    else s = name + " [label=" + "\"" + name + "\",style=filled,fontsize=12,fillcolor=LightSalmon,shape=box]";

    return s;
}
