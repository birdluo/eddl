/**********
**********/

#include "libs/xcl2.hpp"
#include <vector>
#include <math.h>
#include "tensor_hls_op.h"
#include "../../tensor/tensor.h"
#include "../../descriptors/descriptors.h"
#include "gemx_wrapper.h"

//#include <thrust/device_ptr.h>
//#include <thrust/transform.h>
//#include <thrust/reduce.h>
//#include <thrust/functional.h>
//#include <thrust/extrema.h>

#define DBG_FPGA

cl::Context context;
cl::CommandQueue q;
cl::CommandQueue com;
cl::Program program;
cl::Kernel tensor_op;
cl::Kernel multitensor_op;
cl::Kernel kernel_add;
cl::Kernel mult2D;
cl::Kernel sum2D_rowwise;
cl::Kernel kernel_cent;
cl::Kernel relu_soft_d;
cl::Kernel reduce_sum2D;
cl::Kernel kernel_accuracy;
cl::Kernel kernel_total_sum;
cl::Kernel kernel_normalize;
cl::Kernel el_div;
cl::Kernel kernel_gemx;
cl::Kernel kernel_core;

//cl::Buffer buffer;

uint64_t get_duration_ns (const cl::Event &event) {
    cl_int err;
    uint64_t nstimestart, nstimeend;
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START,&nstimestart));
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END,&nstimeend));
    return(nstimeend-nstimestart);
}

void verify2(cl::Buffer &buffer, int size) {

    cl_int err;
    float *ptr = (float *) malloc (size*sizeof(float));
    OCL_CHECK(err, err = q.enqueueReadBuffer(buffer, CL_TRUE, 0, size*sizeof(int), ptr));
    int correct = 1;
    for (int i=0; i<size; i++){
        if (ptr[i] < 1.299) {correct = 0; printf("%f ", ptr[i]);break;}
    }
    if (correct) printf("Verified buffer\n"); else printf("Something WENT wrong \n");
}

void verify(Tensor *T) {
    cl_int err;
    printf("veryfying");
    float *ptr = (float *) malloc (T->size*sizeof(float));
    OCL_CHECK(err, err = q.enqueueReadBuffer(T->fpga_ptr, CL_TRUE, 0, T->size*sizeof(int), ptr));
    for (int i=0; i<T->size; i++){
	printf("ptr = %f \n", ptr[i]);
    }
}

void fpga_init(){ // initialize only once

    cl_int err;
    std::string binaryFile = "eddl-gemx.xclbin";
    unsigned fileBufSize;
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));
    char *fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};

    devices.resize(1);
    OCL_CHECK(err, program = cl::Program(context, devices, bins, NULL, &err));
    /*OCL_CHECK(err, tensor_op= cl::Kernel(program,"tensor_op", &err));*/
    OCL_CHECK(err, multitensor_op = cl::Kernel(program,"multitensor_op", &err));
    OCL_CHECK(err, kernel_add = cl::Kernel(program,"kernel_add", &err));
    //OCL_CHECK(err, mult2D = cl::Kernel(program,"kernel_mult2D", &err));
    OCL_CHECK(err, sum2D_rowwise = cl::Kernel(program,"kernel_sum2D_rowwise", &err));
    OCL_CHECK(err, kernel_cent = cl::Kernel(program,"kernel_cent", &err));
    OCL_CHECK(err, relu_soft_d = cl::Kernel(program,"relu_soft_d", &err));
    OCL_CHECK(err, reduce_sum2D = cl::Kernel(program,"reduce_sum2D", &err));
    OCL_CHECK(err, kernel_core = cl::Kernel(program,"kernel_core", &err));
    OCL_CHECK(err, kernel_accuracy = cl::Kernel(program,"kernel_accuracy", &err));
    OCL_CHECK(err, kernel_total_sum = cl::Kernel(program,"kernel_total_sum", &err));
    //OCL_CHECK(err, el_div = cl::Kernel(program,"el_div", &err));
    //OCL_CHECK(err, kernel_normalize = cl::Kernel(program,"kernel_normalize", &err));*/
    //kernel_gemx = clCreateKernel(program(), "gemxKernel_0", &err);
    if (err != CL_SUCCESS) printf("Error creating kernel\n");

}

void fpga_create_tensor(Tensor *T, int dev)
{
    cl_int err;
    int size = T->size;
    //cl::Buffer buf;
    //printf("Creating Buffer at ref %d -- size %d\n", 0, size);

    OCL_CHECK(err,T->fpga_ptr = cl::Buffer(context,CL_MEM_READ_WRITE, size*sizeof(float), NULL, &err));

    //OCL_CHECK(err, err= q.enqueueWriteBuffer(T->fpga_ptr, CL_TRUE, 0, T->tam*sizeof(float), ptr, nullptr, nullptr));
    //verify2(T->fpga_ptr, T->tam);


    //T->fpga_ptr = &buf;
    //printf("Creating Buffer at ref %d -- %d size %d\n", buf,(T->fpga_ptr), size);
}

void fpga_delete_tensor(Tensor *T)
{

//  T->fpga_ptr.release();

}



void close_fpga(){
 //delete fileBuf;
}

///////////////////////////////////////////
void fpga_copy_fpga(Tensor *A, Tensor *B)
{
    cl_int err;
    OCL_CHECK(err, err= q.enqueueCopyBuffer((A->fpga_ptr), (B->fpga_ptr), 0, 0, A->size*sizeof(float)));
    q.finish();
}

void fpga_copy_to_fpga(float *nptr, Tensor *A)
{
    cl_int err;
    cl::Event blocking_event;
    //OCL_CHECK(err, err= q.enqueueWriteBuffer((A->fpga_ptr), CL_TRUE, 0, A->size*sizeof(float), nptr, nullptr, &blocking_event));
    OCL_CHECK(err, err= q.enqueueWriteBuffer((A->fpga_ptr), CL_TRUE, 0, A->size*sizeof(float), nptr, nullptr, &blocking_event));
    //printf("A->sizeof(float): %f\n", A->size*sizeof(float));
    //printf("nptr-> %f\n", nptr);
    //printf("A->: %f\n", A->fpga_ptr);
    q.finish();
    //blocking_event.wait();
    //printf("Copy Tensor with tam %d in Buffer ref %d -- %f\n", A->tam, A->fpga_ptr,*nptr);
//    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({A->fpga_ptr},0/* 0 means from host*/));

}

///////////////////////////////////////////
void fpga_copy_from_fpga(Tensor *A,float *nptr)
{
    cl_int err;
    cl::Event event;
    OCL_CHECK(err, err= q.enqueueReadBuffer((A->fpga_ptr), CL_TRUE, 0, A->size*sizeof(float), nptr, nullptr, &event));
    q.finish();;
//    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST));
}


void tensor_op_hls(Tensor *A, float fp, int kernel_id)
{
    cl_int err;
    cl::Event task_end;

    #ifdef DBG_FPGA
        printf("TENSOR_OP_HLS %d\n", kernel_id);
    #endif

//    printf("Tensor with XX size %d in Buffer ref %d -- %d\n", A->size, 0,kernel_id);

    OCL_CHECK(err, tensor_op= cl::Kernel(program,"tensor_op", &err));
    OCL_CHECK(err, err = tensor_op.setArg(0, A->fpga_ptr));
    OCL_CHECK(err, err = tensor_op.setArg(1, fp));
    OCL_CHECK(err, err = tensor_op.setArg(2, A->size));
    OCL_CHECK(err, err = tensor_op.setArg(3, kernel_id));


    OCL_CHECK(err, err = q.enqueueTask(tensor_op, NULL, &task_end));
    //printf("Tensor with XX size %d in Buffer ref %d -- %d\n", A->size, 0/*A->fpga_ptr*/,kernel_id);
    //verify(A);
    q.finish();
}

void fpga_tensor_add(float scA,Tensor *A, float scB,Tensor *B, Tensor *C,int incC)
{
    cl_int err;
    cl::Event event;
    #ifdef DBG_FPGA
        printf("FPGA::add\n");
    #endif

    OCL_CHECK(err, err = kernel_add.setArg(0, scA));
    OCL_CHECK(err, err = kernel_add.setArg(1, (A->fpga_ptr)));
    OCL_CHECK(err, err = kernel_add.setArg(2, scB));
    OCL_CHECK(err, err = kernel_add.setArg(3, (B->fpga_ptr)));
    OCL_CHECK(err, err = kernel_add.setArg(4, (C->fpga_ptr)));
    OCL_CHECK(err, err = kernel_add.setArg(5, incC));
    OCL_CHECK(err, err = kernel_add.setArg(6, A->size));

    OCL_CHECK(err, err = q.enqueueTask(kernel_add, NULL, &event));
    event.wait();
}

void fpga_tensor_normalize(Tensor *A, float min, float max)
{
    cl_int err;
    cl::Event event;

    #ifdef DBG_FPGA
        printf("FPGA::NORMALIZE\n");
    #endif
    float min_ori = 0;//gpu_min(A);
    float max_ori = 1;//gpu_max(A);


    OCL_CHECK(err, err = kernel_normalize.setArg(0, A->fpga_ptr));
    OCL_CHECK(err, err = kernel_normalize.setArg(1, max));
    OCL_CHECK(err, err = kernel_normalize.setArg(2, min));
    OCL_CHECK(err, err = kernel_normalize.setArg(3, max_ori));
    OCL_CHECK(err, err = kernel_normalize.setArg(4, min_ori));
    OCL_CHECK(err, err = kernel_normalize.setArg(5, A->size));
    OCL_CHECK(err, err = q.enqueueTask(kernel_normalize, NULL, &event));
    q.finish();
}



void fpga_reduce_sum2D( Tensor *A, Tensor *B, int axis,int incB)
{
    cl_int err;
    cl::Event event;

    #ifdef DBG_FPGA
        //printf("FPGA::REDUCESUM2D\n");
    #endif

    OCL_CHECK(err, err = reduce_sum2D.setArg(0, (A->fpga_ptr)));
    OCL_CHECK(err, err = reduce_sum2D.setArg(1, (B->fpga_ptr)));
    OCL_CHECK(err, err = reduce_sum2D.setArg(2, A->shape[0]));
    OCL_CHECK(err, err = reduce_sum2D.setArg(3, A->shape[1]));
    OCL_CHECK(err, err = reduce_sum2D.setArg(4, axis));
    OCL_CHECK(err, err = reduce_sum2D.setArg(5, incB));

    OCL_CHECK(err, err = q.enqueueTask(reduce_sum2D, NULL, &event));
    q.finish();

}

void fpga_el_div_mult(Tensor *A, Tensor *B, Tensor *C, int incC, int op) {
   cl_int err;
   cl::Event event;

   #ifdef DBG_FPGA
        printf("FPGA::EL_VIV_MULT %d\n", op);
   #endif

   OCL_CHECK(err, err = el_div.setArg(0, (A->fpga_ptr)));
   OCL_CHECK(err, err = el_div.setArg(1, (B->fpga_ptr)));
   OCL_CHECK(err, err = el_div.setArg(2, (C->fpga_ptr)));
   OCL_CHECK(err, err = el_div.setArg(3, A->size));
   OCL_CHECK(err, err = el_div.setArg(4, incC));
   OCL_CHECK(err, err = el_div.setArg(5, op));

   OCL_CHECK(err, err = q.enqueueTask(el_div, NULL, &event));
   q.finish();



//  for (int i = 0; i < A->size; i++)
//    if (incC) C->ptr[i] += A->ptr[i] / B->ptr[i];
//    else C->ptr[i] = A->ptr[i] / B->ptr[i];
}


void fpga_sum2D_rowwise(Tensor *A, Tensor *B, Tensor *C){

    cl_int err;
    cl::Event event;

    #ifdef DBG_FPGA
        //printf("FPGA::SUM2DROWWISE\n");
    #endif

    OCL_CHECK(err, err = sum2D_rowwise.setArg(0, (A->fpga_ptr)));
    OCL_CHECK(err, err = sum2D_rowwise.setArg(1, (B->fpga_ptr)));
    OCL_CHECK(err, err = sum2D_rowwise.setArg(2, (C->fpga_ptr)));
    OCL_CHECK(err, err = sum2D_rowwise.setArg(3, A->shape[0]));
    OCL_CHECK(err, err = sum2D_rowwise.setArg(4, A->shape[1]));

    OCL_CHECK(err, err = q.enqueueTask(sum2D_rowwise, NULL, &event));
    q.finish();
}

void fpga_mult2D(Tensor *A,int tA, Tensor *B, int tB, Tensor *C, int incC){

    cl_int err;
    cl::Event event;
    /*if (incC != 0) {
       printf("WARNING:: Mult2D with inc not supported\n");
    }else printf("REGULAR MATMULT\n");*/

    #ifdef DBG_FPGA
        printf("FPGA::MULT2D\n");
    #endif

    OCL_CHECK(err, err = mult2D.setArg(0, (A->fpga_ptr)));
    OCL_CHECK(err, err = mult2D.setArg(1, (B->fpga_ptr)));
    OCL_CHECK(err, err = mult2D.setArg(2, (C->fpga_ptr)));
    OCL_CHECK(err, err = mult2D.setArg(3, A->shape[0]));
    OCL_CHECK(err, err = mult2D.setArg(4, A->shape[1]));
    OCL_CHECK(err, err = mult2D.setArg(5, B->shape[0]));
    OCL_CHECK(err, err = mult2D.setArg(6, B->shape[1]));
    OCL_CHECK(err, err = mult2D.setArg(7, tA));
    OCL_CHECK(err, err = mult2D.setArg(8, tB));

    //printf("sizes A(%dx%d) B(%dx%d) C(%dx%d)\n", A->sizes[0], A->sizes[1], B->sizes[0], B->sizes[1],C->sizes[0], C->sizes[1]);
    //OCL_CHECK(err, err = tensor_op.setArg(5, incC));
    OCL_CHECK(err, err = q.enqueueTask(mult2D, NULL, &event));
 //   event.wait();
    q.finish();
}

float fpga_total_sum (Tensor *A){
   cl_int err;
   cl::Event event, result_ready;

   #ifdef DBG_FPGA
        printf("FPGA::TOTAL_SUM\n");
    #endif

   float *sum = (float*) malloc(sizeof(float));
   *sum = 0;
   OCL_CHECK(err, cl::Buffer a(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(float) ,sum, &err));

   OCL_CHECK(err, err = kernel_total_sum.setArg(0, (A->fpga_ptr)));
   OCL_CHECK(err, err = kernel_total_sum.setArg(1, A->size));
   OCL_CHECK(err, err = kernel_total_sum.setArg(2, a));
   OCL_CHECK(err, err = q.enqueueTask(kernel_total_sum, NULL, &event));
   event.wait();
   OCL_CHECK(err, err = q.enqueueMigrateMemObjects({a},CL_MIGRATE_MEM_OBJECT_HOST, NULL, &result_ready));
   result_ready.wait();
   return *sum;


}


int fpga_accuracy (Tensor *A, Tensor *B){
   cl_int err;
   cl::Event event, result_ready;


   #ifdef DBG_FPGA
        printf("FPGA::ACCURACY\n");
    #endif

   int *acc = (int*) malloc(sizeof(int));
   *acc = 0;
   OCL_CHECK(err, cl::Buffer a(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 4 ,acc, &err));

   OCL_CHECK(err, err = kernel_accuracy.setArg(0, (A->fpga_ptr)));
   OCL_CHECK(err, err = kernel_accuracy.setArg(1, (B->fpga_ptr)));
   OCL_CHECK(err, err = kernel_accuracy.setArg(2, A->shape[0]));
   OCL_CHECK(err, err = kernel_accuracy.setArg(3, A->shape[1]));
   OCL_CHECK(err, err = kernel_accuracy.setArg(4, a));
   OCL_CHECK(err, err = q.enqueueTask(kernel_accuracy, NULL, &event));
   event.wait();
   OCL_CHECK(err, err = q.enqueueMigrateMemObjects({a},CL_MIGRATE_MEM_OBJECT_HOST, NULL, &result_ready));
   result_ready.wait();
   return *acc;

}

void fpga_cent(Tensor *A, Tensor *B, Tensor *C){
    cl_int err;
    cl::Event event;

    #ifdef DBG_FPGA
        printf("FPGA::CENT\n");
    #endif

    OCL_CHECK(err, err = kernel_cent.setArg(0, (A->fpga_ptr)));
    OCL_CHECK(err, err = kernel_cent.setArg(1, (B->fpga_ptr)));
    OCL_CHECK(err, err = kernel_cent.setArg(2, (C->fpga_ptr)));
    OCL_CHECK(err, err = kernel_cent.setArg(3, A->size));

    OCL_CHECK(err, err = q.enqueueTask(kernel_cent, NULL, &event));
  //  event.wait();
    q.finish();

}

void fpga_relu_soft_d(Tensor *D, Tensor *I, Tensor *PD, int kernel_id){

    cl_int err;
    cl::Event event;

    #ifdef DBG_FPGA
        //printf("FPGA::RELU_SOFT\n", kernel_id);
    #endif


    OCL_CHECK(err, err = relu_soft_d.setArg(0, (D->fpga_ptr)));
    OCL_CHECK(err, err = relu_soft_d.setArg(1, (I->fpga_ptr)));
    OCL_CHECK(err, err = relu_soft_d.setArg(2, (PD->fpga_ptr)));
    OCL_CHECK(err, err = relu_soft_d.setArg(3, D->size));
    OCL_CHECK(err, err = relu_soft_d.setArg(4, kernel_id));

    OCL_CHECK(err, err = q.enqueueTask(relu_soft_d, NULL, &event));
    q.finish();
}


void fpga_tensor_operation(Tensor *A, Tensor *B, int kernel_id)
{
    cl_int err;
    cl::Event event;
    int aux;
    if (kernel_id == FPGARELU) 	{aux = A->size;}
    else {
	     if (kernel_id == FPGASOFTM) {aux = A->shape[0];}
        else{printf("Tensor Operation not supported %d\n", kernel_id); exit(1);}
    }

    #ifdef DBG_FPGA
        //printf("FPGA::TENSOR_OPERATION %d\n", kernel_id);
    #endif

    OCL_CHECK(err, err = multitensor_op.setArg(0, (A->fpga_ptr)));
    OCL_CHECK(err, err = multitensor_op.setArg(1, (B->fpga_ptr)));
    OCL_CHECK(err, err = multitensor_op.setArg(2, aux));
    OCL_CHECK(err, err = multitensor_op.setArg(3, A->shape[1]));
    OCL_CHECK(err, err = multitensor_op.setArg(4, kernel_id));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(multitensor_op, NULL, &event));
    q.finish();
}


void fpga_gemx_mult2D_CPU(Tensor *A,int tA, Tensor *B, int tB, Tensor *C, int incC) {

    cl_int ret;
    cl::Event event;

    gemx_setup(A->shape[0]/*rows_a*/,A->shape[1] /*cols_a*/,B->shape[1] /*cols_b*/, A->shape[1]/*cols_a*/,B->shape[1] /*cols_b*/, B->shape[1]/*cols_b*/, B->shape[1]/*cols_b*/);

    cl::Buffer instr_buffer;
    OCL_CHECK(ret, instr_buffer = cl::Buffer(context,CL_MEM_READ_WRITE ,gemx_instr_buffer_size() , nullptr, &ret));

    OCL_CHECK(ret, ret= q.enqueueWriteBuffer(instr_buffer, CL_TRUE, 0,gemx_instr_buffer_size() , gemx_instr_buffer(), nullptr, &event));

    OCL_CHECK(ret, ret= q.enqueueWriteBuffer(instr_buffer, CL_TRUE, gemx_page_A() * 4096 ,A->shape[0] * A->shape[1] * sizeof(float) , A->ptr, nullptr, &event));
    OCL_CHECK(ret, ret= q.enqueueWriteBuffer(instr_buffer, CL_TRUE, gemx_page_B() * 4096 ,B->shape[0] * B->shape[1] * sizeof(float) , B->ptr, nullptr, &event));

    OCL_CHECK(ret, ret = kernel_gemx.setArg(0, instr_buffer));
    OCL_CHECK(ret, ret = kernel_gemx.setArg(1, instr_buffer));
    OCL_CHECK(ret, ret = q.enqueueTask(kernel_gemx, NULL, &event));
    event.wait();
    OCL_CHECK(ret, ret = q.enqueueReadBuffer(instr_buffer, CL_TRUE, gemx_page_C() * 4096, A->shape[0] * B->shape[1] * sizeof(float), C->ptr));
    q.finish();
}

void fpga_gemx_mult2D(Tensor *A,int tA, Tensor *B, int tB, Tensor *C, int incC) {

    cl_int ret;
    cl::Event event;

    printf("GEMX \n");
    gemx_setup(A->shape[0]/*rows_a*/,A->shape[1] /*cols_a*/,B->shape[1] /*cols_b*/, A->shape[1]/*cols_a*/,B->shape[1] /*cols_b*/, B->shape[1]/*cols_b*/, B->shape[1]/*cols_b*/);

    cl::Buffer instr_buffer;
    OCL_CHECK(ret, instr_buffer = cl::Buffer(context,CL_MEM_READ_WRITE ,gemx_instr_buffer_size() , nullptr, &ret));

    OCL_CHECK(ret, ret= q.enqueueWriteBuffer(instr_buffer, CL_TRUE, 0,gemx_instr_buffer_size() , gemx_instr_buffer(), nullptr, &event));
//    OCL_CHECK(ret, ret = q.enqueueMigrateMemObjects({instr_buffer},CL_MIGRATE_MEM_OBJECT_HOST, NULL, &event));
    printf("AFTER MIGRATE %d %d\n", gemx_instr_buffer_size(), gemx_instr_buffer());

//    clEnqueueMigrateMemObjects(command_queue, 1, &instr_buffer, 0, 0, NULL, &event);

    OCL_CHECK(ret, ret = q.enqueueCopyBuffer(A->fpga_ptr, instr_buffer, 0, gemx_page_A() * 4096, A->shape[0] * A->shape[1] * sizeof(float)));
    OCL_CHECK(ret, ret = q.enqueueCopyBuffer(B->fpga_ptr, instr_buffer, 0, gemx_page_B() * 4096, B->shape[0] * B->shape[1] * sizeof(float)));

    printf("AFTER COPY BUFFER \n");

//    OCL_CHECK(ret, ret= q.enqueueWriteBuffer(instr_buffer,CL_TRUE, 0, gemx_instr_buffer_size(), gemx_instr_buffer(), nullptr, nullptr));
    A->print();

    OCL_CHECK(ret, ret = kernel_gemx.setArg(0, instr_buffer));
    OCL_CHECK(ret, ret = kernel_gemx.setArg(1, instr_buffer));

    printf("AFTER SETTING ARGS \n");
    //ret = clSetKernelArg(kernel_gemx(), 0, sizeof(cl_mem), (void *)&instr_buffer);
    //if (ret != CL_SUCCESS) {printf("Error setting argument\n");}

    //ret = clSetKernelArg(kernel_gemx(), 1, sizeof(cl_mem), (void *)&instr_buffer);
    //if (ret != CL_SUCCESS) {printf("Error setting argument\n");}

    OCL_CHECK(ret, ret = q.enqueueTask(kernel_gemx, NULL, &event));
    printf("AFTER ENQUEUE TASK \n");

    C->print();
    float ptr[64][64];
    OCL_CHECK(ret, ret = q.enqueueReadBuffer(instr_buffer, CL_TRUE, gemx_page_A() * 4096, A->shape[0] * B->shape[1] * sizeof(float), ptr));
    OCL_CHECK(ret, ret= q.enqueueCopyBuffer(instr_buffer,C->fpga_ptr, gemx_page_C() * 4096, 0, A->shape[0] * B->shape[1] * sizeof(float)));
    printf("AFTER COPYBUFFER \n");
    q.finish();
    C->print();
    printf("EL %f %f\n", ptr[0][0], ptr[1][0]);
    printf("EL %f %f\n", ptr[0][0], ptr[1][0]);
//      ret = clEnqueueCopyBuffer(q(), instr_buffer,(cl_mem)C->fpga_ptr(), gemx_page_C() * 4096, 0, A->shape[0] * B->shape[1] * sizeof(float), 0, NULL, &event);
 //   if (ret != CL_SUCCESS) {printf("Error setting argument\n");}
 //   ret = clWaitForEvents(1, &event);
 //   if (ret != CL_SUCCESS) {printf("Error setting argument\n");}


 //   clReleaseMemObject(instr_buffer);

}


void fpga_core(Tensor *A, float v, int kernel_id)
{
    cl_int err;
    cl::Event event;

    #ifdef DBG_FPGA
        printf("FPGA::fpga_core %d\n", kernel_id);
    #endif

    OCL_CHECK(err, err = kernel_core.setArg(0, (A->fpga_ptr)));
    OCL_CHECK(err, err = kernel_core.setArg(1, (A->size)));
    OCL_CHECK(err, err = kernel_core.setArg(2, v));
    OCL_CHECK(err, err = kernel_core.setArg(3, kernel_id));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(kernel_core, NULL, &event));
    q.finish();
}
