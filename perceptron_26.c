#include "perceptron_26.h"

int main(int argc, char const *argv[]) {

	const float theta = 0.5;
	int i = 0, j = 0;

	srand(time(NULL));
	srand48(time(NULL));

	char** fileList = malloc(26*sizeof(char*));
	for(i=65 ; i<91 ; i++) {
		fileList[i-65] = malloc(5*sizeof(char));
		sprintf(fileList[i-65], "%c.txt", i);
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

	// test(fileList, layer, theta);

	return 0;
}

Neuron** learn(char** fileList, Neuron** layer, float theta) {
	float epsilon = 0.05;
	float *errorDiff;
	int learning = 1, letter = 0, i = 0, j = 0, learnSteps = 0, c = 0;
	int **init, **expected, *errors;

	FILE* outputFile = fopen("out_26_learn.txt", "w");
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
					// printf("i=%d;j=%d - %f\n", i, j, sum[j]);
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

				if(expected[letter][j]-layer[1][j].value != 0) {
					errors[j] ++;
				}
				errorDiff[j] += fabsf(expected[letter][j]-sum[j]);
			}
		}

		learnSteps ++;

		float totalErrorDiff = 0;
		int totalErrors = 0;
		for(j=0 ; j<26 ; j++) {
			totalErrorDiff += errorDiff[j];
			totalErrors += errors[j];
		}
		if(totalErrorDiff <= 34.14) {
			learning = 0;
		}

		fprintf(outputFile, "%d %f %d\n", learnSteps, totalErrorDiff, totalErrors);
	}

	printf("Learning finished in %d steps\n", learnSteps);

	fclose(outputFile);

	return layer;
}

void test(char** fileList, Neuron** layer, float theta) {
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

	for(fileCount=0 ; fileCount<26 ; fileCount++) {
		FILE* inputFile = fopen(fileList[fileCount], "r");

		char filename[30];
		sprintf(filename, "out_26_test_%c.txt", fileList[fileCount][0]);
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
