#include "perceptron_26.h"

int main(int argc, char const *argv[]) {

	const float theta = 0.5;
	int i = 0, j = 0;

	srand(time(NULL));
	srand48(time(NULL));

	char** fileList = malloc(26*sizeof(char*));
	for(i=65 ; i<91 ; i++) {
		fileList[i-65] = malloc(13*sizeof(char));
		sprintf(fileList[i-65], "letters/%c.txt", i);
	}

	Neuron** layer;
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

	layer = learn(fileList, layer, theta);

	test(fileList, layer, theta);

	return 0;
}

Neuron** learn(char** fileList, Neuron** layer, float theta) {
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

void test(char** fileList, Neuron** layer, float theta) {
	int fileCount = 0, c = 0, nbModifs = 0, testCount = 0;
	const int nbTests = 100;
	int i = 0, j = 0;
	int modifIndex = -1, line = 0;
	int *init, *modif, *expected, *errors;

	init = malloc(IMG_SIZE*sizeof(int));
	modif = malloc(IMG_SIZE*sizeof(int));
	expected = malloc(26*sizeof(int));
	errors = malloc(nbTests*sizeof(int));

	for(i=0 ; i<IMG_SIZE ; i++) {
		init[i] = -1;
		modif[i] = -1;
	}

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
		printf("In %s :\n", fileList[fileCount]);
		for(nbModifs=0 ; nbModifs<IMG_SIZE ; nbModifs++) {
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
				// If there is a diffrence, there is an error
				layer[1][maxJ].value = 1;
				if(layer[1][maxJ].value != expected[maxJ]) {
					errors[testCount] = 1;
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

			// Prints the number of errors per number of modification, then the percentage of errors according to the number of tests
			printf("\t%d (%.1f %%) modif.  \t->   %d (%.1f %%) errors\n", nbModifs, (float) nbModifs*(100/IMG_SIZE), totalErrors, (float) totalErrors*(100/nbTests));
			fprintf(outputFile, "%f %f\n", (float) nbModifs*(100/IMG_SIZE), (float) totalErrors*(100/nbTests));
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
