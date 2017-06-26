all: tsp

tsp: base_opengl.o kohonen.o
	gcc base_opengl.o kohonen.o -o tsp.out -lglut -lGLU -lGL -lm

base_opengl.o: base_opengl.c base_opengl.h kohonen.h
	gcc -c base_opengl.c -o base_opengl.o

kohonen.o: kohonen.c kohonen.h
	gcc -c kohonen.c -o kohonen.o

clean:
	rm -f tsp.out *.o
