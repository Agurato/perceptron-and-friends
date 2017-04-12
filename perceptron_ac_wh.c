#include "perceptron.h"

int main(int argc, char const *argv[]) {

	const float theta = 0.5;
	int i = 0;

	srand(time(NULL));
	srand48(time(NULL));

	char** fileList = malloc(2*sizeof(char*));
	fileList[0] = "A.txt";
	fileList[1] = "C.txt";

	Node** layer;
	layer = malloc(2*sizeof(Node*));
	layer[0] = malloc(IMG_SIZE*sizeof(Node));
	layer[1] = malloc(sizeof(Node));

	for(i=0; i<IMG_SIZE ; i++) {
		layer[0][i].value = 0;
		layer[0][i].weight = malloc(sizeof(float));
		layer[0][i].weight[0] = drand48()/IMG_SIZE;
	}

	layer = learn(fileList, layer, theta);

	test(fileList, layer, theta);

	return 0;
}

Node** learn(char** fileList, Node** layer, float theta) {
	float epsilon = 0.05, errorDiff = 0;
	int learning = 1, counter = 0, i = 0, learnSteps = 0, c = 0;

	while(learning) {
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
					break;
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

			for(i=0; i<IMG_SIZE ; i++) {
				layer[0][i].weight[0] += epsilon*(counter-sum)*layer[0][i].value;
			}

			errorDiff += fabsf(counter-sum);

			fclose(inputFile);
		}

		printf("error = %f\n", errorDiff);
		learnSteps ++;

		if(errorDiff <= 0.00001) {
			learning = 0;
		}
	}
	printf("Learning finished in %d steps\n", learnSteps);
	return layer;
}

void test(char** fileList, Node** layer, float theta) {
	int errors = 0, fileCount = 0, c = 0, nbModifs = 0, testCount = 0;
	const int nbTests = 100;
	int i = 0;
	int modifIndex = -1;
	int init[IMG_SIZE] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	int modif[IMG_SIZE] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

	for(fileCount=0 ; fileCount<2 ; fileCount++) {
		FILE* inputFile = fopen(fileList[fileCount], "r");
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
			printf("\t%d modif.\t->\t%.2f %% errors\n", nbModifs, (float) errors*(100/nbTests));
		}
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
