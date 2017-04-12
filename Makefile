all: ac_simple ac_wh display_letters

ac_simple: perceptron_ac_simple.c perceptron.h
	gcc -Wall -pedantic perceptron_ac_simple.c -o perceptron_ac_simple

ac_wh: perceptron_ac_wh.c perceptron.h
	gcc -Wall -pedantic perceptron_ac_wh.c -o perceptron_ac_wh

display_letters: display_letters.c
	gcc -Wall -pedantic display_letters.c -o display_letters

clean:
	rm -rf perceptron_ac_simple perceptron_ac_wh display_letters
