#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include "kohonen.h"

/**** Data ****/

Data __data_set[DATA_SIZE];

int init_dataset() {
    for (size_t i = 0; i < DATA_SIZE; i++) {
        __data_set[i].x = 2*drand48()-1;
        __data_set[i].y = 2*drand48()-1;
        printf("%f ; %f\n", __data_set[i].x, __data_set[i].y);
    }
    return 0;
}

size_t __data_index = 0;
Data get_next_data() {
    return __data_set[rand()%DATA_SIZE];
}

Data get_data(size_t index) {
    return __data_set[index];
}

/**** Neurons ****/

Neuron __neuron_set[NB_NEURONS];

int init_neurons() {
    for (size_t i = 0; i < NB_NEURONS; i++) {
        __neuron_set[i].Wx = cos(2*M_PI*i/DATA_SIZE)/10;
        __neuron_set[i].Wy = sin(2*M_PI*i/DATA_SIZE)/10;
    }
    return 0;
}

Neuron get_neuron(size_t neuron_id) {
    return __neuron_set[neuron_id];
}

void compute_neuron_activity(Data data_selected) {
    for (size_t i = 0; i < NB_NEURONS; i++) {
        __neuron_set[i].activity = pow(data_selected.x - __neuron_set[i].Wx, 2) + pow(data_selected.y - __neuron_set[i].Wy, 2);
    }
}

size_t winning_neuron() {
    size_t winner_index = 0;
    for (size_t i = 1; i < NB_NEURONS; i++) {
        if (__neuron_set[i].activity < __neuron_set[winner_index].activity) {
            winner_index = i;
        }
    }
    return winner_index;
}

double neighbourhood_function(size_t winner_index, size_t index_neuron, double eta) {
    int d = abs((int)(winner_index - index_neuron));

    // If distance == 0, so there is no division by 0
    if(d == 0) {
        return eta/2;
    }
    return (eta/2)*((sin(1.8*d))/(1.8*d))*exp(-(1.8*d*d)/(eta*eta));
}

int update_neurons(Data data_selected, size_t winner_index, double eta) {
    for (size_t i = 0; i < NB_NEURONS; i++) {
        //printf("(%f, %f) -> ", __neuron_set[i].Wx, __neuron_set[i].Wy);
        __neuron_set[i].Wx += EPSILON*eta*(data_selected.x - __neuron_set[i].Wx) * neighbourhood_function(winner_index, i, eta);
        __neuron_set[i].Wy += EPSILON*eta*(data_selected.y - __neuron_set[i].Wy) * neighbourhood_function(winner_index, i, eta);
        //printf("(%f, %f)\n", __neuron_set[i].Wx, __neuron_set[i].Wy);
    }
    return 0;
}
