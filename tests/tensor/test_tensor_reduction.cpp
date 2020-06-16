#include <gtest/gtest.h>
#include <random>
#include <string>
#include <ctime>

#include "eddl/tensor/tensor.h"
#include "eddl/tensor/tensor_reduction.h"
#include "eddl/tensor/nn/tensor_nn.h"
#include "eddl/descriptors/descriptors.h"


using namespace std;



TEST(TensorTestSuite, tensor_math_reduction_max) {
    // Test #1
    Tensor *t1_ref = new Tensor({5.0f, 6.0f, 9.0f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, -3.0f, 9.0f,
                                    1.0f, 6.0f, 0.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t = t1->max({0}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({4.0f, 8.0f, 9.0f, 6.0f},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, -3.0f, 9.0f,
                                    1.0f, 6.0f, 0.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->max({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}

TEST(TensorTestSuite, tensor_math_reduction_argmax) {
    // Test #1
    Tensor *t1_ref = new Tensor({1.0f, 3.0f, 2.0f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, -3.0f, 9.0f,
                                    1.0f, 6.0f, 0.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t = t1->argmax({0}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({1.0f, 2.0f, 2.0f, 1.0f},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, -3.0f, 9.0f,
                                    1.0f, 6.0f, 0.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->argmax({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}


TEST(TensorTestSuite, tensor_math_reduction_min) {
    // Test #1
    Tensor *t1_ref = new Tensor({1.0f, -3.0f, 0.0f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, -3.0f, 9.0f,
                                    1.0f, 6.0f, 0.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t = t1->min({0}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({1.0f, 4.0f, -3.0f, 0.0f},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, -3.0f, 9.0f,
                                    1.0f, 6.0f, 0.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->min({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}

TEST(TensorTestSuite, tensor_math_reduction_argmin) {
    // Test #1
    Tensor *t1_ref = new Tensor({0.0f, 2.0f, 3.0f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, -3.0f, 9.0f,
                                    1.0f, 6.0f, 0.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t = t1->argmin({0}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({0.0f, 1.0f, 1.0f, 2.0f},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, -3.0f, 9.0f,
                                    1.0f, 6.0f, 0.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->argmin({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}

TEST(TensorTestSuite, tensor_math_reduction_sum) {
    // Test #1
    Tensor *t1_ref = new Tensor({
                                        4.0f, 4.0f,
                                        4.0f, 4.0f,
                                        4.0f, 4.0f}, {3, 2}, DEV_CPU);
    Tensor *t1 = Tensor::ones({3, 2, 4}, DEV_CPU);

    Tensor *new_t = t1->sum({2}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({
                                        2.0f, 2.0f, 2.0f, 2.0f,
                                        2.0f, 2.0f, 2.0f, 2.0f,
                                        2.0f, 2.0f, 2.0f, 2.0f}, {3, 4}, DEV_CPU);
    Tensor *t2 = Tensor::ones({3, 2, 4}, DEV_CPU);

    Tensor *new_t2 = t2->sum({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));

    // Test GPU
#ifdef cGPU
    Tensor* t_cpu = Tensor::ones({10, 10});  // High mismatch CPU/GPU
    Tensor* t_gpu = t_cpu->clone(); t_gpu->toGPU();

    Tensor *t_cpu_sum = t_cpu->sum({1}, false);
    Tensor *t_gpu_sum = t_gpu->sum({1}, false); t_gpu_sum->toCPU();
    t_cpu_sum->print();
    t_gpu_sum->print();

    ASSERT_TRUE(Tensor::equivalent(t_cpu_sum, t_gpu_sum, 10e-4));
#endif
}


TEST(TensorTestSuite, tensor_math_reduction_sum_abs) {
    // Test #1
    Tensor *t1_ref = new Tensor({10.0f, 11.0f, 7.0f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({-4.0f, 7.0f, 3.0f,
                             6.0f, 4.0f, -4.0f}, {2, 3}, DEV_CPU);

    Tensor *new_t = t1->sum_abs({0}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #1
    Tensor *t2_ref = new Tensor({14.0f, 14.0f},  {2}, DEV_CPU);
    Tensor *t2 = new Tensor({-4.0f, 7.0f, 3.0f,
                             6.0f, 4.0f, -4.0f}, {2, 3}, DEV_CPU);

    Tensor *new_t2 = t2->sum_abs({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}

TEST(TensorTestSuite, tensor_math_reduction_prod) {
    // Test #1
    Tensor *t1_ref = new Tensor({24.0f, 28.0f, 12.0f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({4.0f, 7.0f, 3.0f,
                             6.0f, 4.0f, 4.0f}, {2, 3}, DEV_CPU);

    Tensor *new_t = t1->prod({0}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({ 16., 160.,  48.,  32. },  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, 3.0f, 8.0f,
                                    1.0f, 4.0f, 8.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->prod({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}

TEST(TensorTestSuite, tensor_math_reduction_mean) {
    // Test #1
    Tensor *t1_ref = new Tensor({5.0f, 5.5f, 3.5f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({4.0f, 7.0f, 3.0f,
                             6.0f, 4.0f, 4.0f}, {2, 3}, DEV_CPU);

    Tensor *new_t = t1->mean({0}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({ 3.0000, 5.6667, 4.3333, 4.3333},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, 3.0f, 8.0f,
                                    1.0f, 4.0f, 8.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->mean({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}

TEST(TensorTestSuite, tensor_math_reduction_var) {
    // Test #1
    Tensor *t1_ref = new Tensor({2.0f, 4.5f, 0.5f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({4.0f, 7.0f, 3.0f,
                             6.0f, 4.0f, 4.0f}, {2, 3}, DEV_CPU);

    Tensor *new_t = t1->var({0}, false, true);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({ 3.0000,  4.3333, 10.3333, 12.3333},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, 3.0f, 8.0f,
                                    1.0f, 4.0f, 8.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->var({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}


TEST(TensorTestSuite, tensor_math_reduction_std) {
    // Test #1
    Tensor *t1_ref = new Tensor({1.4142f, 2.1213f, 0.7071f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({4.0f, 7.0f, 3.0f,
                                 6.0f, 4.0f, 4.0f}, {2, 3}, DEV_CPU);

    Tensor *new_t = t1->std({0}, false, true);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({1.7321, 2.0817, 3.2146, 3.5119},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, 3.0f, 8.0f,
                                    1.0f, 4.0f, 8.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->std({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}



TEST(TensorTestSuite, tensor_math_reduction_mode) {
    // Test #1
    Tensor *t1_ref = new Tensor({1.0f, 4.0f, 8.0f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({
                             1.0f, 4.0f, 4.0f,
                             5.0f, 4.0f, 8.0f,
                             2.0f, 3.0f, 8.0f,
                             1.0f, 4.0f, 8.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t = t1->mode({0}, false);
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({4., 5., 2., 1.},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 5.0f, 8.0f,
                                    2.0f, 3.0f, 2.0f,
                                    1.0f, 1.0f, 1.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->mode({1}, false);
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}


TEST(TensorTestSuite, tensor_math_reduction_norm) {
    // Test #1
    Tensor *t1_ref = new Tensor({5.5678f,  7.5498f, 14.4222f},  {3}, DEV_CPU);
    Tensor *t1 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, 3.0f, 8.0f,
                                    1.0f, 4.0f, 8.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t = t1->norm({0}, false, "fro");
    ASSERT_TRUE(Tensor::equivalent(t1_ref, new_t, 10e-4));

    // Test #2
    Tensor *t2_ref = new Tensor({5.7446f, 10.2470f, 8.7750f, 9.0000f},  {4}, DEV_CPU);
    Tensor *t2 = new Tensor({
                                    1.0f, 4.0f, 4.0f,
                                    5.0f, 4.0f, 8.0f,
                                    2.0f, 3.0f, 8.0f,
                                    1.0f, 4.0f, 8.0f}, {4, 3}, DEV_CPU);

    Tensor *new_t2 = t2->norm({1}, false, "fro");
    ASSERT_TRUE(Tensor::equivalent(t2_ref, new_t2, 10e-4));
}
