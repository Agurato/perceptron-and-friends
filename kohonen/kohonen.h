#ifndef __KOHONEN_DEF
#define __KOHONEN_DEF

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define EPSILON 0.01

/****** POI_Data ******/
/* Points of Interest */

typedef struct data_struct {
    double x;
    double y;
} POI_Data;

int init_dataset(int size);
POI_Data get_next_data();
POI_Data get_data(size_t index);

/**** Neurons ****/

typedef struct neuron_struct {
    double Wx;
    double Wy;
    double activity;
} Neuron;

int init_neurons(int size);
Neuron get_neuron(size_t neuron_id);
void compute_neuron_activity(POI_Data data_selected);
size_t winning_neuron();
double neighbourhood_function(int winner_index, int index_neuron, double eta);
int update_neurons(POI_Data data_selected, size_t winner_index, double eta);

#endif
