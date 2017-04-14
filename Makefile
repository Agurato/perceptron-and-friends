all: ac 26 display_letters

ac: perceptron_ac.c perceptron_ac.h perceptron.h
	gcc -Wall -pedantic perceptron_ac.c -o perceptron_ac

26: perceptron_26.c perceptron_26.h perceptron.h
	gcc -Wall -pedantic perceptron_26.c -o perceptron_26

display_letters: display_letters.c
	gcc -Wall -pedantic display_letters.c -o display_letters

clean: clean_ac clean_26
	rm -rf display_letters

clean_out: clean_out_ac clean_out_26

clean_ac: clean_out_ac
	rm -rf perceptron_ac

clean_out_ac:
	rm -rf out_ac_*.txt

clean_26: clean_out_26
	rm -rf perceptron_26

clean_out_26:
	rm -rf out_26_*.txt
