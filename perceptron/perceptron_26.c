#include "perceptron_26.h"

int main(int argc, char const *argv[]) {

	const float theta = 0.5;
	int i = 0, j = 0, h = 0;
	int nbHiddenNeurons = 0;

	if(argc >= 2) {
		if(strcmp(argv[1], "--help") == 0) {
			puts("Use \"./perceptron_26 <nb of neurons in the hidden layer>\" to use a hidden layer with the specified number of neurons");
			exit(0);
		}
		else {
			int input = atoi(argv[1]);
			if(input > 0) {
				nbHiddenNeurons = input;
			}
		}
	}

	srand(time(NULL));
	srand48(time(NULL));

	char** fileList = malloc(26*sizeof(char*));
	for(i=65 ; i<91 ; i++) {
		fileList[i-65] = malloc(13*sizeof(char));
		sprintf(fileList[i-65], "letters/%c.txt", i);
	}

	Neuron** layer;
	if(nbHiddenNeurons == 0) {
		printf("No hidden layer used\n\n");
		layer = malloc(2*sizeof(Neuron*));
		layer[0] = malloc(IMG_SIZE*sizeof(Neuron));
		layer[1] = malloc(26*sizeof(Neuron));

		for(i=0; i<IMG_SIZE ; i++) {
			layer[0][i].value = 0;
			layer[0][i].weight = malloc(26*sizeof(float));
			for(j=0 ; j<26 ; j++) {
				layer[0][i].weight[j] = drand48()/IMG_SIZE;
			}
		}

		layer = learnNoHidden(fileList, layer, theta);

		testNoHidden(fileList, layer, theta);
	}
	else {
		printf("Hidden layer used with %d neurons\n\n", nbHiddenNeurons);
		layer = malloc(3*sizeof(Neuron*));
		layer[0] = malloc(IMG_SIZE*sizeof(Neuron));
		layer[1] = malloc(nbHiddenNeurons*sizeof(Neuron));
		layer[2] = malloc(26*sizeof(Neuron));

		for(i=0; i<IMG_SIZE ; i++) {
			layer[0][i].value = 0;
			layer[0][i].weight = malloc(nbHiddenNeurons*sizeof(float));
			for(j=0 ; j<nbHiddenNeurons ; j++) {
				layer[0][i].weight[j] = drand48()/IMG_SIZE;
			}
		}
		for(h=0; h<nbHiddenNeurons ; h++) {
			layer[1][h].value = 0;
			layer[1][h].weight = malloc(26*sizeof(float));
			for(j=0 ; j<26 ; j++) {
				layer[1][h].weight[j] = drand48()/nbHiddenNeurons;
			}
		}

		layer = learnHidden(fileList, layer, 0.2, nbHiddenNeurons);

		testHidden(fileList, layer, theta, nbHiddenNeurons);
	}

	return 0;
}

Neuron** learnNoHidden(char** fileList, Neuron** layer, float theta) {
	float epsilon = 0.05, minErrorDiff = -1;
	float *errorDiff;
	int minOverride = 0;
	int learning = 1, letter = 0, i = 0, j = 0, learnSteps = 0, c = 0;
	int **init, **expected, *errors;

	FILE* outputFile = fopen("results/out_26_learn.txt", "w");
	if(outputFile == NULL) {
		puts("Error writing the file");
		exit(1);
	}

	init = malloc(26*sizeof(int*));
	expected = malloc(26*sizeof(int*));
	for(letter=0 ; letter<26 ; letter++) {
		int line = 0;
		init[letter] = malloc(IMG_SIZE*sizeof(int));
		expected[letter] = malloc(26*sizeof(int));
		FILE* inputFile = fopen(fileList[letter], "r");
		if(inputFile == NULL) {
			puts("Error reading the file");
			exit(1);
		}
		i = 0;
		while((c = fgetc(inputFile)) != EOF) {
			if(c != 48 && c != 49) {
				line = 1;
				i = 0;
				continue;
			}
			c -= 48;
			if(line == 0) {
				init[letter][i] = c;
			}
			else {
				expected[letter][i] = c;
			}
			i++;
		}
		fclose(inputFile);
	}

	errorDiff = malloc(26*sizeof(float));
	errors = malloc(26*sizeof(int));
	while(learning) {
		for(j=0 ; j<26 ; j++) {
			errorDiff[j] = 0;
			errors[j] = 0;
		}

		// For each letter to learn
		for(letter=0 ; letter<26 ; letter++) {
			for(i=0 ; i<IMG_SIZE ; i++) {
				layer[0][i].value = init[letter][i];
			}
			float *sum;
			sum = malloc(26*sizeof(float));
			for(j=0 ; j<26 ; j++) {
				sum[j] = 0;
				for(i=0 ; i<IMG_SIZE ; i++) {
					sum[j] += layer[0][i].value*layer[0][i].weight[j];
				}

				if(sum[j] < theta) {
					layer[1][j].value = 0;
				}
				else {
					layer[1][j].value = 1;
				}

				for(i=0; i<IMG_SIZE ; i++) {
					layer[0][i].weight[j] += epsilon*(expected[letter][j]-sum[j])*layer[0][i].value;
				}

				if(expected[letter][j] != layer[1][j].value) {
					errors[j] ++;
				}
				errorDiff[j] += fabsf(expected[letter][j]-sum[j]);
			}
		}

		learnSteps ++;

		float totalErrorDiff = 0;
		int totalErrors = 0;
		// printf("Iter %d\n", learnSteps);
		for(j=0 ; j<26 ; j++) {
			totalErrorDiff += errorDiff[j];
			totalErrors += errors[j];
			// printf("\t%d: %f - %d\n", j, errorDiff[j], errors[j]);
		}
		if(totalErrorDiff < minErrorDiff || minErrorDiff == -1) {
			minErrorDiff = totalErrorDiff;
			minOverride = 0;
		}
		// If the error is above the minimum found for 5 consecutive turns, stops the laerning
		else if(minOverride < 5) {
			minOverride ++;
		}
		else {
			learning = 0;
		}

		fprintf(outputFile, "%d %f %d\n", learnSteps, totalErrorDiff, totalErrors);
	}

	printf("Learning finished in %d steps\n", learnSteps);

	fclose(outputFile);

	return layer;
}

void testNoHidden(char** fileList, Neuron** layer, float theta) {
	int fileCount = 0, c = 0, nbModifs = 0, testCount = 0;
	const int nbTests = 100, maxNbModifs = 1;
	int i = 0, j = 0;
	int modifIndex = -1, line = 0;
	int *init, *modif, *expected, *errors, *modifErrors;

	init = malloc(IMG_SIZE*sizeof(int));
	modif = malloc(IMG_SIZE*sizeof(int));
	expected = malloc(26*sizeof(int));
	errors = malloc(nbTests*sizeof(int));
	modifErrors = malloc(maxNbModifs*sizeof(int));

	for(nbModifs=0 ; nbModifs<maxNbModifs ; nbModifs++) {
		modifErrors[nbModifs] = 0;
	}

	for(i=0 ; i<IMG_SIZE ; i++) {
		init[i] = -1;
		modif[i] = -1;
	}

	FILE* totalOutputFile = fopen("results/out_26_test_total.txt", "w");

	for(fileCount=0 ; fileCount<26 ; fileCount++) {
		FILE* inputFile = fopen(fileList[fileCount], "r");

		char filename[30];
		sprintf(filename, "results/out_26_test_%c.txt", fileList[fileCount][8]);
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
		line = 0;
		while((c = fgetc(inputFile)) != EOF) {
			if(c != 48 && c != 49) {
				line = 1;
				i = 0;
				continue;
			}
			c -= 48;
			if(line == 0) {
				init[i] = c;
			}
			else {
				expected[i] = c;
			}
			i++;
		}
		for(nbModifs=0 ; nbModifs<maxNbModifs ; nbModifs++) {
			for(j=0 ; j<nbTests ; j++) {
				errors[j] = 0;
			}
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

				float *sum;
				sum = malloc(26*sizeof(float));
				int maxJ = -1;
				float max = -1;
				for(j=0 ; j<26 ; j++) {
					sum[j] = 0;
					for(i=0 ; i<IMG_SIZE ; i++) {
						sum[j] += layer[0][i].value*layer[0][i].weight[j];
					}


					if(max < sum[j] || max == -1) {
						max = sum[j];
						maxJ = j;
					}

					layer[1][j].value = 0;
				}

				// To test the result, we only "light" the neuron with the highest potential to 1
				// Then we compare is this neuron is different to the one expected
				// If there is no difference, the result is correct
				// If there is a difference, there is an error
				layer[1][maxJ].value = 1;
				if(layer[1][maxJ].value != expected[maxJ]) {
					errors[testCount] = 1;
					modifErrors[nbModifs] ++;
				}
				else {
					errors[testCount] = 0;
				}

				for(i=0 ; i<nbModifs ; i++) {
					modif[i] = -1;
				}

			}
			int totalErrors = 0;
			for(testCount=0 ; testCount<nbTests ; testCount++) {
				totalErrors += errors[testCount];
			}

			// Prints the percentage of modification, then the percentage of errors according to the number of tests for each number of modification (letter by letter)
			fprintf(outputFile, "%f %f\n", (float) nbModifs*(100/IMG_SIZE), (float) totalErrors*(100/nbTests));
		}
		fclose(outputFile);
	}

	// Mean values for the whole tests of the fprintf right above this
	for(nbModifs=0 ; nbModifs<maxNbModifs ; nbModifs++) {
		printf("%d (%.1f %%) modif.  \t->   %d (%.1f %%) errors\n", nbModifs, (float) nbModifs*(100/IMG_SIZE), modifErrors[nbModifs], (float) 100*modifErrors[nbModifs]/(26*nbTests));
		fprintf(totalOutputFile, "%f %f\n", (float) nbModifs*(100/IMG_SIZE), (float) 100*modifErrors[nbModifs]/(26*nbTests));
	}

	fclose(totalOutputFile);
}

Neuron** learnHidden(char** fileList, Neuron** layer, float theta, int nbHiddenNeurons) {
	float error = 0;
	int learning = 1, letter = 0, i = 0, j = 0, h = 0, learnSteps = 0, c = 0;
	int **init, **expected;

	float *sumJ = malloc(26*sizeof(float));
	float *sumH = malloc(nbHiddenNeurons*sizeof(float));
	float *deltaJ = malloc(26*sizeof(float));
	float *deltaH = malloc(nbHiddenNeurons*sizeof(float));

	char *filename = malloc(30*sizeof(char));
	sprintf(filename, "results/out_26_%d_learn.txt", nbHiddenNeurons);

	FILE* outputFile = fopen(filename, "w");
	if(outputFile == NULL) {
		puts("Error writing the file");
		exit(1);
	}

	init = malloc(26*sizeof(int*));
	expected = malloc(26*sizeof(int*));
	for(letter=0 ; letter<26 ; letter++) {
		int line = 0;
		init[letter] = malloc(IMG_SIZE*sizeof(int));
		expected[letter] = malloc(26*sizeof(int));
		FILE* inputFile = fopen(fileList[letter], "r");
		if(inputFile == NULL) {
			puts("Error reading the file");
			exit(1);
		}
		i = 0;
		while((c = fgetc(inputFile)) != EOF) {
			if(c != 48 && c != 49) {
				line = 1;
				i = 0;
				continue;
			}
			c -= 48;
			if(line == 0) {
				init[letter][i] = c;
			}
			else {
				expected[letter][i] = c;
			}
			i++;
		}
		fclose(inputFile);
	}

	while(learning) {
		error = 0;

		// For each letter to learn
		for(letter=0 ; letter<26 ; letter++) {
			// printf("Letter %c\n", letter+65);
			for(i=0 ; i<IMG_SIZE ; i++) {
				layer[0][i].value = init[letter][i];
			}

			for(h=0 ; h<nbHiddenNeurons ; h++) {
				// Calculate hidden layer neuron values
				sumH[h] = 0;
				for(i=0 ; i<IMG_SIZE ; i++) {
					sumH[h] += layer[0][i].value*layer[0][i].weight[h];
				}
				layer[1][h].value = 1/(1+expf(-sumH[h]));
			}
			for(j=0 ; j<26 ; j++) {
				// Calculate output layer neuron values
				sumJ[j] = 0;
				for(h=0 ; h<nbHiddenNeurons ; h++) {
					sumJ[j] += layer[1][h].value*layer[1][h].weight[j];
				}
				layer[2][j].value = 1/(1+expf(-sumJ[j]));

				// Compute delta_j
				deltaJ[j] = layer[2][j].value*(1-layer[2][j].value)*(expected[letter][j]-layer[2][j].value);

				// Compute error
				error += powf(expected[letter][j]-layer[2][j].value, 2);
			}
			for(h=0; h<nbHiddenNeurons ; h++) {
				float sumBack = 0;
				for(j=0 ; j<26 ; j++) {
					sumBack += layer[1][h].weight[j]*deltaJ[j];
				}
				// Compute delta_h
				deltaH[h] = layer[1][h].value*(1-layer[1][h].value)*sumBack;

				// Modify weights of neurons in the hidden layer
				for(j=0 ; j<26 ; j++) {
					layer[1][h].weight[j] += 0.2*deltaJ[j]*layer[1][h].value;
				}
			}
			// Modify weights of neurons in the input layer
			for(i=0 ; i<IMG_SIZE ; i++) {
				for(h=0; h<nbHiddenNeurons ; h++) {
					layer[0][i].weight[h] += 0.2*deltaH[h]*layer[0][i].value;
				}
			}
		}

		if(learnSteps % 10000 == 0) {
			printf("iter %d : %f\n", learnSteps, error);
		}
		learnSteps ++;
		if(error < 0.001) {
			learning = 0;
		}
	}

	printf("Learning finished in %d steps\n", learnSteps);

	fclose(outputFile);

	return layer;
}

void testHidden(char** fileList, Neuron** layer, float theta, int nbHiddenNeurons) {
	int letter = 0, c = 0, nbModifs = 0, testCount = 0;
	const int nbTests = 100, maxNbModifs = IMG_SIZE;
	int i = 0, j = 0, h = 0;
	int modifIndex = -1, line = 0;
	int *init, *modif, *expected, *errors, *modifErrors;
	float sum = 0, output = 0;

	init = malloc(IMG_SIZE*sizeof(int));
	modif = malloc(IMG_SIZE*sizeof(int));
	expected = malloc(26*sizeof(int));
	errors = malloc(nbTests*sizeof(int));
	modifErrors = malloc(maxNbModifs*sizeof(int));

	for(nbModifs=0 ; nbModifs<maxNbModifs ; nbModifs++) {
		modifErrors[nbModifs] = 0;
	}

	for(i=0 ; i<IMG_SIZE ; i++) {
		init[i] = -1;
		modif[i] = -1;
	}

	FILE* totalOutputFile = fopen("results/out_26_test_total.txt", "w");

	for(letter=0 ; letter<26 ; letter++) {
		FILE* inputFile = fopen(fileList[letter], "r");

		char filename[30];
		sprintf(filename, "results/out_26_test_%c.txt", fileList[letter][8]);
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
		line = 0;
		while((c = fgetc(inputFile)) != EOF) {
			if(c != 48 && c != 49) {
				line = 1;
				i = 0;
				continue;
			}
			c -= 48;
			if(line == 0) {
				init[i] = c;
			}
			else {
				expected[i] = c;
			}
			i++;
		}
		fclose(inputFile);
		for(nbModifs=0 ; nbModifs<maxNbModifs ; nbModifs++) {
			for(j=0 ; j<nbTests ; j++) {
				errors[j] = 0;
			}
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

				for(h=0 ; h<nbHiddenNeurons ; h++) {
					// Calculate hidden layer neuron values
					sum = 0;
					for(i=0 ; i<IMG_SIZE ; i++) {
						sum += layer[0][i].value*layer[0][i].weight[h];
					}
					layer[1][h].value = 1/(1+expf(-sum));
				}

				int maxJ = -1;
				float max = -1;
				for(j=0 ; j<26 ; j++) {
					// Calculate output layer neuron values
					sum = 0;
					for(h=0 ; h<nbHiddenNeurons ; h++) {
						sum += layer[1][h].value*layer[1][h].weight[j];
					}
					output = 1/(1+expf(-sum));

					if(max < output || max == -1) {
						max = output;
						maxJ = j;
					}

					layer[2][j].value = 0;
				}

				// To test the result, we only "light" the neuron with the highest potential to 1
				// Then we compare is this neuron is different to the one expected
				// If there is no difference, the result is correct
				// If there is a difference, there is an error
				layer[2][maxJ].value = 1;
				if(layer[2][maxJ].value != expected[maxJ]) {
					errors[testCount] = 1;
					modifErrors[nbModifs] ++;
				}
				else {
					errors[testCount] = 0;
				}

				for(i=0 ; i<nbModifs ; i++) {
					modif[i] = -1;
				}

			}
			int totalErrors = 0;
			for(testCount=0 ; testCount<nbTests ; testCount++) {
				totalErrors += errors[testCount];
			}

			// Prints the percentage of modification, then the percentage of errors according to the number of tests for each number of modification (letter by letter)
			fprintf(outputFile, "%f %f\n", (float) nbModifs*(100/IMG_SIZE), (float) totalErrors*(100/nbTests));
		}
		fclose(outputFile);
	}

	// Mean values for the whole tests of the fprintf right above this
	for(nbModifs=0 ; nbModifs<maxNbModifs ; nbModifs++) {
		printf("%d (%.1f %%) modif.  \t->   %d (%.1f %%) errors\n", nbModifs, (float) nbModifs*(100/IMG_SIZE), modifErrors[nbModifs], (float) 100*modifErrors[nbModifs]/(26*nbTests));
		fprintf(totalOutputFile, "%f %f\n", (float) nbModifs*(100/IMG_SIZE), (float) 100*modifErrors[nbModifs]/(26*nbTests));
	}

	fclose(totalOutputFile);
}

int isInArray(int val, int* array, int size) {
	int i;
	for (i=0; i < size; i++) {
		if (array[i] == val)
		return 1;
	}
	return 0;
}

void displayNetwork(Neuron **layer, int nbHiddenNeurons) {
	int i=0, h=0, j=0;
	// for(i=0 ; i<IMG_SIZE ; i++) {
	// 	printf("%d\n\t", (int) layer[0][i].value);
	// 	for(h=0 ; h<nbHiddenNeurons ; h++) {
	// 		printf("%f ", layer[0][i].weight[h]);
	// 	}
	// 	puts("");
	// }
	// puts("");
	// for(h=0 ; h<nbHiddenNeurons ; h++) {
	// 	printf("%d\n\t", (int) layer[1][h].value);
	// 	for(j=0 ; j<26 ; j++) {
	// 		printf("%f ", layer[1][h].weight[j]);
	// 	}
	// 	puts("");
	// }
	// puts("");
	for(i=0 ; i<IMG_SIZE ; i++) {
		printf("%d", (int) layer[0][i].value);
	}
	puts("");
	for(h=0 ; h<nbHiddenNeurons ; h++) {
		printf("%d", (int) layer[1][h].value);
	}
	puts("");
	for(j=0 ; j<26 ; j++) {
		printf("%d", (int) layer[2][j].value);
	}
	puts("");
}
