#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define EPSILON 0.5
#define DATA_SIZE 20
#define NB_NEURONS 20

float heaviside(float value);

/**** Data ****/

typedef struct data_struct {
    double x;
    double y;
} Data;

int init_dataset();
Data get_next_data();
Data get_data(size_t index);

/**** Neurons ****/

typedef struct neuron_struct {
    double Wx;
    double Wy;
    double activity;
} Neuron;

Neuron __neuron_set[NB_NEURONS];

int init_neurons();
Neuron get_neuron(size_t neuron_id);
void compute_neuron_activity(Data data_selected);
size_t winning_neuron();
double neighbourhood_function(size_t winner_index, size_t index_neuron, double eta);
int update_neurons(Data data_selected, size_t winner_index, double eta);
