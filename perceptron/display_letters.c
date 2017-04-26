#include <stdio.h>

int main(int argc, char const *argv[]) {
	int i = 0, c = 0, count = 0;
	char fileName[5];

	for(i=65 ; i<91 ; i++) {
		sprintf(fileName, "%c.txt", i);
		printf("\n %s\n\t", fileName);
		FILE* inputFile = fopen(fileName, "r");
		while((c = fgetc(inputFile)) != EOF) {
			if(c == 48) {
				printf("\033[0m  \033[0m");
			}
			else if(c == 49) {
				printf("\033[047m  \033[0m");
			}
			else {
				break;
			}
			count ++;
			if(count % 5 == 0) {
				printf("\n\t");
			}
		}
	}
	puts("");

	return 0;
}
