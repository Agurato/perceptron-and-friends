#include "kohonen.h"

/**** POI_Data ****/

POI_Data *poi;
int NB_POI;

int init_dataset(int size) {
    NB_POI = size;
    poi = malloc(NB_POI*sizeof(POI_Data));
    for (size_t i = 0; i < NB_POI; i++) {
        poi[i].x = 2*drand48()-1;
        poi[i].y = 2*drand48()-1;
        printf("%f ; %f\n", poi[i].x, poi[i].y);
    }
    return 0;
}

size_t __data_index = 0;
POI_Data get_next_data() {
    return poi[rand()%NB_POI];
}

POI_Data get_data(size_t index) {
    return poi[index];
}

/**** Neurons ****/

Neuron *neurons;
int NB_NEURONS;

int init_neurons(int size) {
    NB_NEURONS = size;
    neurons = malloc(NB_NEURONS*sizeof(Neuron));
    for (size_t i = 0; i < NB_NEURONS; i++) {
        neurons[i].Wx = cos(2*M_PI*i/NB_POI)/10;
        neurons[i].Wy = sin(2*M_PI*i/NB_POI)/10;
    }
    return 0;
}

Neuron get_neuron(size_t neuron_id) {
    return neurons[neuron_id];
}

void compute_neuron_activity(POI_Data data_selected) {
    for (size_t i = 0; i < NB_NEURONS; i++) {
        neurons[i].activity = pow(data_selected.x - neurons[i].Wx, 2) + pow(data_selected.y - neurons[i].Wy, 2);
    }
}

size_t winning_neuron() {
    size_t winner_index = 0;
    for (size_t i = 1; i < NB_NEURONS; i++) {
        if (neurons[i].activity < neurons[winner_index].activity) {
            winner_index = i;
        }
    }
    return winner_index;
}

double neighbourhood_function(int winner_index, int index_neuron, double eta) {
    int d = abs(winner_index - index_neuron);

    // If distance == 0, so there is no division by 0
    if(d == 0) {
        return eta/2;
    }
    return (eta/2)*((sin(1.8*d))/(1.8*d))*exp(-(1.8*d*d)/(eta*eta));
}

int update_neurons(POI_Data data_selected, size_t winner_index, double eta) {
    for (size_t i = 0; i < NB_NEURONS; i++) {
        neurons[i].Wx += EPSILON * (data_selected.x - neurons[i].Wx) * neighbourhood_function(winner_index, i, eta);
        neurons[i].Wy += EPSILON * (data_selected.y - neurons[i].Wy) * neighbourhood_function(winner_index, i, eta);
    }
    return 0;
}
