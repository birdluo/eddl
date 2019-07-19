
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

#include "layer.h"
#include "../tensor/tensor.h"

using namespace std;


////////////////////////////////////
///// BASE LAYER CLASS
////////////////////////////////////

Layer::Layer(string name, int dev) {
    mode = TRMODE;
    target = delta = input = output = nullptr;
    this->name = name;
    this->dev = dev;
    lin = lout = 0;
    delta_bp = 0;
}


void Layer::initialize() {
    for (int i = 0; i != params.size(); i++) {
        if (params[i]->ndim == 1)
            params[i]->rand_suniform(0.1);
        else if (params[i]->ndim == 2)
            params[i]->rand_gaussian(0.0, sqrt(2.0 / params[i]->shape[0]));
        else
            params[i]->rand_gaussian(0.0, sqrt(2.0 / (params[i]->size / params[i]->shape[0])));
    }
}

void Layer::resize(int batch)
{
   //cout<<"Resize "<<name<<"\n";

   input=parent[0]->output;
   if (output!=nullptr) {
      tshape s=output->shape;
      s[0]=batch;
      delete output;
      output=new Tensor(s,dev);
   }

   if (delta!=nullptr) {
      tshape s=delta->shape;
      s[0]=batch;
      delete delta;
      delta=new Tensor(s,dev);
    }

   if (target!=nullptr) {
      tshape s=target->shape;
      s[0]=batch;
      delete target;
      target=new Tensor(s,dev);
    }

}


void Layer::reset() {
    delta->set(0.0);
}

void Layer::setmode(int m) {
    mode = m;
}

void Layer::save(FILE *fe)
{
  for (int i = 0; i != params.size(); i++)
    params[i]->save(fe);
}

void Layer::load(FILE *fe)
{
  for (int i = 0; i != params.size(); i++)
    params[i]->load(fe);
}

void Layer::info() {
    cout << "\n===============\n";
    cout << "Layer " << name << "\n";
    if (parent.size()) {
        cout << "Parent layers:\n";
        for (int i = 0; i < parent.size(); i++)
            cout << parent[i]->name << "\n";
    } else cout << "No parent layers\n";

    if (child.size()) {
        cout << "Child layers:\n";
        for (int i = 0; i != child.size(); i++)
            cout << child[i]->name << "\n";
    } else cout << "No child layers\n";

    cout << "Input tensor:\n";
    input->info();

    if (params.size()) {
        cout << "Params:\n";
        for (int i = 0; i < params.size(); i++)
            params[i]->info();
    } else cout << "No params\n";

    cout << "Output tensor:\n";
    output->info();
    cout << "===============\n\n";
}

Tensor* Layer::getWeights(){
    return nullptr;
}

Tensor* Layer::setWeights(Tensor bias){
    return nullptr;
}

Tensor* Layer::getBias(){
    return nullptr;
}

Tensor* Layer::setBias(Tensor bias){
    return nullptr;
}

////////////////////////////////////
///// LINEAR LAYERS
////////////////////////////////////
LinLayer::LinLayer(string name, int dev) : Layer(name, dev) {}

void LinLayer::addchild(Layer *l) {
    child.push_back(l);
    lout++;
}

void LinLayer::addparent(Layer *l) {
    if (parent.size() != 0) msg("This layers only can have one parent layer", l->name.c_str());
    parent.push_back(l);
    lin++;
}


////////////////////////////////////
///// Multiple LAYERS
////////////////////////////////////
MLayer::MLayer(string name, int dev) : Layer(name, dev) {}

void MLayer::addchild(Layer *l) {
    child.push_back(l);
    lout++;
}


void MLayer::addparent(Layer *l) {
    parent.push_back(l);
    lin++;
}
