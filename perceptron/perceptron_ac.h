#include "perceptron.h"

#define IMG_SIZE 25

Neuron** learn(char** fileList, Neuron** layer, float theta, int wh);
void test(char** fileList, Neuron** layer, float theta, int wh);
int isInArray(int val, int* array, int size);
