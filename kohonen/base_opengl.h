#include <stdarg.h>
#include <GL/glut.h>
#include "kohonen.h"

/* structure de coordonnée 2D pour affichage*/

void draw_text(float x, float y, const char *fmt, ...);
void affichage();
void idle();
void clavier(unsigned char touche, int x, int y);
void reshape(int x, int y);
void mouse(int bouton, int etat, int x, int y);
void mousemotion(int x, int y);
