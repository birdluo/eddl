#include <math.h>
#include <stdio.h>
extern "C" {

void k_d_elu(float *D, float *I, float *PD, long int size, float param){

  #pragma HLS INTERFACE m_axi port=D offset=slave bundle=gmem
  #pragma HLS INTERFACE m_axi port=I offset=slave bundle=gmem
  #pragma HLS INTERFACE m_axi port=PD offset=slave bundle=gmem
  #pragma HLS INTERFACE s_axilite port=D  bundle=control
  #pragma HLS INTERFACE s_axilite port=I  bundle=control
  #pragma HLS INTERFACE s_axilite port=PD  bundle=control
  #pragma HLS INTERFACE s_axilite port=size bundle=control
  #pragma HLS INTERFACE s_axilite port=param bundle=control

  for (int i = 0; i < size; i++) {
    if (I[i] > 0.0) PD[i] += D[i];  // why +=
    else PD[i] += D[i] * (param * expf(I[i]));  // check expf is ok
  }

}

} // end extern "C"
