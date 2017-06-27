#include <stdio.h>

int main(int argc, char const *argv[]) {
    int i = 0;
    double eta = 2;

    for(i=0 ; i<1000000000 ; i++) {
        eta -= eta/2000000;
        if(i%100000 == 0) {
            printf("%d : %f\n", i, eta);
        }
        if(eta < 0.6) {
            return 1;
        }
    }
    return 0;
}
