#include "perceptron.h"

int main(int argc, char const *argv[]) {

	int c;
	float theta = 0.5, epsilon = 0.05, errorDiff = 0;
	int learning = 1, counter = 0, i = 0, errors = 0, learnSteps = 0;

	srand(time(NULL));
	srand48(time(NULL));

	char** fileList = malloc(2*sizeof(char*));
	fileList[0] = "A.txt";
	fileList[1] = "C.txt";

	Node** layer;
	layer = malloc(2*sizeof(Node*));
	layer[0] = malloc(20*sizeof(Node));
	layer[1] = malloc(sizeof(Node));

	for(i=0; i<20 ; i++) {
		layer[0][i].value = 0;
		layer[0][i].weight = malloc(sizeof(float));
		layer[0][i].weight[0] = drand48()/20;
	}

	/* Learning loop */
	while(learning) {
		errors = 0;
		errorDiff = 0;

		for(counter=0 ; counter<2 ; counter++) {

			FILE* inputFile = fopen(fileList[counter], "r");
			if(inputFile == NULL) {
				puts("Error reading the file");
				exit(1);
			}

			i = 0;
			float sum = 0;
			while((c = fgetc(inputFile)) != EOF) {
				if(c != 48 && c != 49) {
					continue;
				}
				c -= 48;
				layer[0][i].value = c;
				sum += c*layer[0][i].weight[0];
				i++;
			}

			if(sum < theta) {
				layer[1][0].value = 0;
			}
			else {
				layer[1][0].value = 1;
			}

			for(i=0; i<20 ; i++) {
				layer[0][i].weight[0] += epsilon*(counter-sum)*layer[0][i].value;
			}

			errorDiff += fabsf(counter-sum);

			if(counter-layer[1][0].value != 0) {
				errors ++;
			}

			fclose(inputFile);
		}
		printf("error = %f\n", errorDiff);

		learnSteps ++;

		/* Simple rule */
		// if(errors == 0) {
		// 	learning = 0;
		// }

		if(errorDiff <= 0.00001) {
			learning = 0;
		}
	}

	/* Tests & noise */
	errors = 0;
	int modifIndex = rand()%20;

	counter = 0;
	FILE* inputFile = fopen(fileList[counter], "r");
	if(inputFile == NULL) {
		puts("Error reading the file");
		exit(1);
	}

	i = 0;
	float sum = 0;
	while((c = fgetc(inputFile)) != EOF) {
		if(c != 48 && c != 49) {
			continue;
		}
		c -= 48;
		if(i == modifIndex) {
			layer[0][i].value = -c + 1;
		}
		else {
			layer[0][i].value = c;
		}
		sum += c*layer[0][i].weight[0];
		i++;
	}

	layer[0][modifIndex].value = -layer[0][modifIndex].value + 1;

	for(i=0 ; i<20 ; i++) {
		sum += layer[0][i].value*layer[0][i].weight[0];
	}

	if(sum < theta) {
		layer[1][0].value = 0;
	}
	else {
		layer[1][0].value = 1;
	}

	if(layer[1][0].value != counter) {
		errors ++;
	}

	return 0;
}
