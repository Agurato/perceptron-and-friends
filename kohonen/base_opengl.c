#include "base_opengl.h"

int cpt = 0;
char pushed, calc;
int anglex, angley, x, y, xold, yold;
double alpha = 1, sigma = 1.0;
int NB_POI, NB_NEURONS;


/* affiche la chaine fmt a partir des coordonnées x, y*/
void draw_text(float x, float y, const char *fmt, ...)
{
    char		buf[1024];			//Holds Our String
    char		*text = buf;
    va_list		ap;				// Pointer To List Of Arguments

    if (fmt == NULL)				// If There's No Text
    return;					// Do Nothing

    va_start(ap, fmt);				// Parses The String For Variables
    vsprintf(text, fmt, ap);			// And Converts Symbols To Actual Numbers
    va_end(ap);					// Results Are Stored In Text

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    while (*text)
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text++);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* *************************************************** */
int main(int argc, char **argv) {

    if(argc != 3) {
        printf("Expected 2 arguments : <nb of PoI> <nb of Neurons>\n");
        exit(1);
    }

    NB_POI = atoi(argv[1]);
    NB_NEURONS = atoi(argv[2]);
    srand(time(NULL));
    srand48(time(NULL));
    printf("TSP started with %d points of interest and %d neurons\n", NB_POI, NB_NEURONS);
    init_poi(NB_POI);
    init_neurons(NB_NEURONS);

    /* initilisation de glut et creation de la fenetre */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(530, 530);
    glutCreateWindow("Kohonen");

    /* initialisation OpenGL */
    glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);
    glPointSize(4);
    glEnable(GL_DEPTH_TEST);

    /* enregistrement des fonctions de rappel */
    glutDisplayFunc(affichage);
    glutKeyboardFunc(clavier);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);

    /* Entree dans la boucle principale */
    glutMainLoop();
    return 0;
}


/* *************************************************** */

/* fonction d'affichage appelée a chaque refresh*/
void affichage() {
    /* effacement de l'image avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glRotatef(-angley, 1, 0, 0);
    glRotatef(-anglex, 0, 1, 0);

    glColor3f(1.0, 1.0, 1.0);
    draw_text(-0.9, 0.9, "nb iter: %d", cpt);


    glBegin(GL_POINTS);
    // dataset
    glColor3f(0.0, 1.0, 0.0);
    for (size_t i = 0; i < NB_POI; i++) {
        POI_Data data = get_data(i);
        glVertex2f(data.x, data.y);
    }
    // neurons
    glColor3f(1.0, 0.0, 0.0);
    for (size_t i = 0; i < NB_NEURONS; i++) {
        Neuron neuron = get_neuron(i);
        glVertex2f(neuron.Wx, neuron.Wy);
    }
    glEnd();


    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < NB_NEURONS; i++) {
        Neuron neuron = get_neuron(i);
        glVertex2f(neuron.Wx, neuron.Wy);
    }
    glEnd();

    glFlush();

    /* On echange les buffers */
    glutSwapBuffers();
}


void idle() {
    if(calc) {
        cpt++;
        // select image
        POI_Data data_selected = get_next_data();

        // simulate
        compute_neuron_activity(data_selected);
        // neurons_Activity();

        // compare
        size_t winner_index = winning_neuron();

        double x = cpt/1000000.0;
        alpha = exp(-(x*x)/4);

        sigma = 1.9 - (cpt/1500000.0);
        if(sigma <= 0) {
            sigma = 0.000001;
        }

        // update
        update_neurons(data_selected, winner_index, alpha, sigma);

        // display
        if (cpt%1000 == 0) {
            glutPostRedisplay();
        }
        if(cpt%100000 == 0) {
            printf("%f\t%f\n", sigma, alpha);
        }
    }
}


void clavier(unsigned char touche, int x, int y) {
    switch(touche) {
        case 'p':
        calc=!calc;
        //if(calc) calc=0; else calc=1;
        break;

        case 'q': /* la touche 'q' permet de quitter le programme */
        exit(0);
    } /* switch */

} /* clavier */


void reshape(int x, int y) {
    if (x<y)
    glViewport(0, (y-x)/2, x, x);
    else
    glViewport((x-y)/2, 0, y, y);
} /* reshape */


/* getion des boutons de  la souris*/
void mouse(int bouton, int etat, int x, int y) {
    /* si on appuie sur la bouton de gauche */
    if (bouton==GLUT_LEFT_BUTTON && etat ==GLUT_DOWN) {
        pushed=1; // vrai
        xold=x;   // sauvegarde de la position de la souris
        yold=y;
    }

    /* si on relache la souris */
    if (bouton==GLUT_LEFT_BUTTON && etat ==GLUT_UP) {
        pushed=0; // faux
    }
} /* mouse */


/*gestion des mouvements de la souris */
void mousemotion(int x, int y) {
    if (pushed) { /* si le bouton gauche est presse */
        /* on mofifie les angles de rotation de l'objet en fonction de la position actuelle de la souris et de la derniere position sauvegard?e */
        anglex=anglex+(x-xold);
        angley=angley+(y-yold);
        glutPostRedisplay();
    }

    xold=x; /* sauvegarde des valeurs courante des positions de la souris */
    yold=y;
} /* mousemotion */
