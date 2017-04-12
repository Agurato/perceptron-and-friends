#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define IMG_SIZE 25

typedef struct {
	float value;
	float *weight;
} Node;

Node** learn(char** fileList, Node** layer, float theta);
void test(char** fileList, Node** layer, float theta);
int isInArray(int val, int* array, int size);
