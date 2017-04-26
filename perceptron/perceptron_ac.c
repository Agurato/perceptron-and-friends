#include "perceptron_ac.h"

int main(int argc, char const *argv[]) {

	const float theta = 0.5;
	int i = 0, wh = 1;

	if(argc >= 2) {
		if(strcmp(argv[1], "--help") == 0) {
			puts("Use \"./perceptron_ac s\" to learn via the simple algorithm.\nThe Widrow-Hoff learning algorithm is used if nothing is specified");
			exit(0);
		}
		else if(strcmp(argv[1], "s") == 0) {
			wh = 0;
		}
	}

	srand(time(NULL));
	srand48(time(NULL));

	char** fileList = malloc(2*sizeof(char*));
	fileList[0] = "letters/A.txt";
	fileList[1] = "letters/C.txt";

	Neuron** layer;
	layer = malloc(2*sizeof(Neuron*));
	layer[0] = malloc(IMG_SIZE*sizeof(Neuron));
	layer[1] = malloc(sizeof(Neuron));

	for(i=0; i<IMG_SIZE ; i++) {
		layer[0][i].value = 0;
		layer[0][i].weight = malloc(sizeof(float));
		layer[0][i].weight[0] = drand48()/IMG_SIZE;
	}

	layer = learn(fileList, layer, theta, wh);

	test(fileList, layer, theta, wh);

	return 0;
}

Neuron** learn(char** fileList, Neuron** layer, float theta, int wh) {
	float epsilon = 0.05, errorDiff = 0;
	int learning = 1, counter = 0, i = 0, errors = 0, learnSteps = 0, c = 0;
	FILE* outputFile;
	int** init;

	if(wh) {
		outputFile = fopen("results/out_ac_wh_learn.txt", "w");
	}
	else {
		outputFile = fopen("results/out_ac_s_learn.txt", "w");
	}

	if(outputFile == NULL) {
		puts("Error writing the file");
		exit(1);
	}

	init = malloc(2*sizeof(int*));
	for(counter=0 ; counter<2 ; counter++) {
		init[counter] = malloc(IMG_SIZE*sizeof(int));
		FILE* inputFile = fopen(fileList[counter], "r");
		if(inputFile == NULL) {
			puts("Error reading the file");
			exit(1);
		}
		i = 0;
		while((c = fgetc(inputFile)) != EOF) {
			if(c != 48 && c != 49) {
				break;
			}
			c -= 48;
			init[counter][i] = c;
			i++;
		}
		fclose(inputFile);
	}

	while(learning) {
		errors = 0;
		errorDiff = 0;

		for(counter=0 ; counter<2 ; counter++) {

			float sum = 0;
			for(i=0 ; i<IMG_SIZE ; i++) {
				layer[0][i].value = init[counter][i];
				sum += init[counter][i]*layer[0][i].weight[0];
			}

			if(sum < theta) {
				layer[1][0].value = 0;
			}
			else {
				layer[1][0].value = 1;
			}

			for(i=0; i<IMG_SIZE ; i++) {
				layer[0][i].weight[0] += epsilon*(counter-sum)*layer[0][i].value;
			}

			if(counter-layer[1][0].value != 0) {
				errors ++;
			}
			errorDiff += pow(counter-sum, 2);

		}

		learnSteps ++;
		fprintf(outputFile, "%d %f %d\n", learnSteps, errorDiff, errors);
		printf("Iter %d:   diff = %f   ->   errors = %d\n", learnSteps, errorDiff, errors);

		if(wh) {
			if(errorDiff <= 0.000001) {
				learning = 0;
			}
		}
		else {
			if(errors == 0) {
				learning = 0;
			}
		}
	}

	fclose(outputFile);

	return layer;
}

void test(char** fileList, Neuron** layer, float theta, int wh) {
	int errors = 0, fileCount = 0, c = 0, nbModifs = 0, testCount = 0;
	const int nbTests = 100;
	int i = 0;
	int modifIndex = -1;
	int init[IMG_SIZE];
	int modif[IMG_SIZE];

	for(i=0 ; i<IMG_SIZE ; i++) {
		init[i] = -1;
		modif[i] = -1;
	}

	for(fileCount=0 ; fileCount<2 ; fileCount++) {
		FILE* inputFile = fopen(fileList[fileCount], "r");

		char filename[30];
		if(wh) {
			sprintf(filename, "results/out_ac_wh_test_%c.txt", fileList[fileCount][8]);
		}
		else {
			sprintf(filename, "results/out_ac_s_test_%c.txt", fileList[fileCount][8]);
		}
		FILE* outputFile = fopen(filename, "w");

		if(inputFile == NULL) {
			puts("Error reading the file");
			exit(1);
		}
		if(outputFile == NULL) {
			puts("Error writing the file");
			exit(1);
		}

		i = 0;
		while((c = fgetc(inputFile)) != EOF) {
			if(c != 48 && c != 49) {
				break;
			}
			c -= 48;
			init[i] = c;
			i++;
		}
		printf("In %s :\n", fileList[fileCount]);
		for(nbModifs=0 ; nbModifs<IMG_SIZE ; nbModifs++) {
			errors = 0;
			for(testCount=0 ; testCount<nbTests ; testCount++) {
				for(i=0 ; i<IMG_SIZE ; i++) {
					layer[0][i].value = init[i];
				}

				for(i=0 ; i<nbModifs ; i++) {
					modifIndex = rand()%IMG_SIZE;
					while(isInArray(modifIndex, modif, IMG_SIZE)) {
						modifIndex = rand()%IMG_SIZE;
					}
					layer[0][modifIndex].value = -layer[0][modifIndex].value + 1;
					modif[i] = modifIndex;
				}

				float sum = 0;
				for(i=0 ; i<IMG_SIZE ; i++) {
					sum += layer[0][i].value*layer[0][i].weight[0];
				}

				if(sum < theta) {
					layer[1][0].value = 0;
				}
				else {
					layer[1][0].value = 1;
				}

				if(layer[1][0].value != fileCount) {
					errors ++;
				}

				for(i=0 ; i<nbModifs ; i++) {
					modif[i] = -1;
				}
			}
			printf("\t%d (%.1f %%) modif.  \t->   %d (%.1f %%) errors\n", nbModifs, (float) nbModifs*(100/IMG_SIZE), errors, (float) errors*(100/nbTests));
			fprintf(outputFile, "%f %f\n", (float) nbModifs*(100/IMG_SIZE), (float) errors*(100/nbTests));
		}
		fclose(outputFile);
	}
}

int isInArray(int val, int* array, int size) {
	int i;
	for (i=0; i < size; i++) {
		if (array[i] == val)
		return 1;
	}
	return 0;
}
