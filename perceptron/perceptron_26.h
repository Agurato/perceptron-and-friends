#include "perceptron.h"

#define IMG_SIZE 25

Neuron** learnNoHidden(char** fileList, Neuron** layer, float theta);
void testNoHidden(char** fileList, Neuron** layer, float theta);
Neuron** learnHidden(char** fileList, Neuron** layer, float theta, int nbHiddenNeurons);
void testHidden(char** fileList, Neuron** layer, float theta, int nbHiddenNeurons);
int isInArray(int val, int* array, int size);
