#include <cstdio>
#include <cstdlib>
/****************************************************************/
/* works on mac using freeglut and xquartz                      */
/* you can install freeglut using homebrew                      */
/* xquartz can be downloaded at http://xquartz.macosforge.org   */
/****************************************************************/
#include <GL/freeglut.h>
#include <jpeglib.h>
#include <jerror.h>
#include <math.h>
#include <iostream>

#ifdef __WIN32
#pragma comment (lib, "jpeg.lib")
#endif


const int largimg = 256;
const int hautimg = 256;

unsigned char image1[largimg*hautimg*3];//*3 car RVB
unsigned char image2[largimg*hautimg*3];
unsigned char image3[largimg*hautimg*3];

char presse;
int anglex=30,angley=20,x,y,xold,yold;

void affichage();
void clavier(unsigned char touche,int x,int y);
void souris(int boutton, int etat,int x,int y);
void sourismouv(int x,int y);
void redim(int l,int h);
void loadJpegImage(char *fichier, unsigned char image[]);

        /*************/
        /*MES CLASSES*/
        /*************/

/*class Point*/
class Point
{
    public :
        //coordonn�es x, y et z du point
        double x;
        double y;
        double z;
};

        /****************/
        /*MES PRIMITIVES*/
        /****************/

void MonCylindre(float r, float h, int n);
void MaSphere(float r, int NM, int NP);
void cube();

        /****************/
        /*MES FONCTIONS*/
        /****************/

void tete();
void cou();
void corps();
void Queue();
void ailes();
void pattesAvant();
void pattesArriere();

void animation();
void touchesSpecial(int key, int x, int y);

int main(int argc,char **argv)
{
  /* Creation de la fenetre OpenGL */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1000,800);
  glutCreateWindow("Texture JPEG");

  /* Initialisation de l'etat d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);

  //Mes lumi�res

    //Mat�riaux
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess = 50.0;

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

  /* Mise en place de la projection perspective */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0,1,1.0,5.0);
  glMatrixMode(GL_MODELVIEW);

  /* Parametrage du placage de textures */
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    //Mes images

    glEnable(GL_TEXTURE_2D);

    loadJpegImage("./texture_dragon_noire.jpg", image1);
    loadJpegImage("./texture_dragon_grise.jpg", image2);
    loadJpegImage("./face.jpg", image3);

    //Interaction Texture/Lumi�re
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  /* Mise en place des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutMouseFunc(souris);
  glutMotionFunc(sourismouv);
  glutReshapeFunc(redim);
  //Pour les touches sp�ciales du clavier
  glutSpecialFunc(touchesSpecial);

  /* Entr�e dans la boucle principale glut */
  glutMainLoop();
  return 0;
}

        /*********************/
        /*VARIABLES AFFICHAGE*/
        /*********************/

float ZOOM = 9;
float AVANT = 5;
float HAUT = 0;
float COTE = 0;
int STYLE = GL_FILL;

        /*********************/
        /*VARIABLES ANIMATION*/
        /*********************/

bool activateAnimation = false;
//Translations
bool mouve = false; //bool�en permettant l'incr�mentation et la d�cr�mentation de dh
float dh = 0; //Valeur de la translation
float h=0.2; //Valeur absolue de la translation maximale
int nb_mvt = 80; //Nombre d'image cr�� lors d'une "demi-animation"
//Rotations
bool rot = false; //bool�en permettant l'incr�mentation et la d�cr�mentation de dr
float dr = 0; //Angle de rotation
float r = 5; //Valeur absolue de la rotation maximale
int machoire = 0;

//Bool�en pour appliquer une texture sp�cifique � la face du dragon
bool face = false;

        /**********/
        /*LUMIERES*/
        /**********/
//GL_LIGHT0
GLfloat light_position_0[] = {15.0, 5.0, 3.0, 1.0};
GLfloat dif_0[] = {1.0, 1.0, 1.0, 1.0};
GLfloat amb_0[] = {1.0, 1.0, 1.0, 1.0};
GLfloat spec_0[] = {1.0, 1.0, 1.0, 1.0};

//GL_LIGHT1
GLfloat light_position_1[] = {5.0, -5.0, 0.0, 0.0};
GLfloat dif_1[] = {1.0, 1.0, 1.0, 1.0};
GLfloat amb_1[] = {1.0, 1.0, 1.0, 1.0};
GLfloat spec_1[] = {1.0, 1.0, 1.0, 1.0};
GLfloat dir_1[] = {-1, 1, 0};
int angle = 0;

void affichage()
{
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glLoadIdentity();

      gluLookAt(0.0,0.0,2.5,0.0,0.0,0.0,0.0,1.0,0.0);
      glRotatef(angley,1.0,0.0,0.0);
      glRotatef(anglex,0.0,1.0,0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ZOOM, ZOOM, -ZOOM, ZOOM, -1000, 1000);
    glMatrixMode(GL_MODELVIEW);

    //Lumi�res
    glLightfv(GL_LIGHT0, GL_POSITION, light_position_0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb_0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif_0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec_0);

    glRotated(angle, 0, 1, 0);

    glLightfv(GL_LIGHT1, GL_POSITION, light_position_1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb_1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif_1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec_1);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 60.0);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir_1);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);

    glRotated(-angle, 0, 1, 0);

    angle++;

    //Mouvement du Dragon
    animation();

    //Mod�lisation du dragon
    tete();
    cou();
    corps();
    Queue();
    ailes();
    pattesAvant();
    pattesArriere();

    //glutPostRedisplay() n�cessaire pour mettre � jour les actualisations de lumi�res
    glutPostRedisplay();

    glutSwapBuffers();
}

        /****************/
        /*Mes Primitives*/
        /****************/


void MaSphere(float r, int NM, int NP)
{
    float x[NM*NP];
    float y[NM*NP];
    float z[NM*NP];

    for(int j=0; j<NP; j++)
    {
        for(int i=0; i<NM; i++)
        {
            float theta = (2*M_PI*i)/NM;
            float phi = (-M_PI)/2 + j*(M_PI/(NP-1));
            x[i+j*NM] = r*cos(theta)*cos(phi);
            y[i+j*NM] = r*sin(theta)*cos(phi);
            z[i+j*NM] = r*sin(phi);
        }
    }


    for(int j=0; j<NP-1; j++)
    {
        for(int i=0; i<NM; i++)
        {
            //Calcul des normales
            GLfloat a1 = x[i+j*NM];
            GLfloat b1 = y[i+j*NM];
            GLfloat c1 = z[i+j*NM];
            GLfloat a2 = x[(i+1)%NM + j*NM];
            GLfloat b2 = y[(i+1)%NM + j*NM];
            GLfloat c2 = z[(i+1)%NM + j*NM];
            GLfloat a3 = x[(i+1)%NM + (j+1)*NM];
            GLfloat b3 = y[(i+1)%NM + (j+1)*NM];
            GLfloat c3 = z[(i+1)%NM + (j+1)*NM];

            GLfloat n1 = b2*c3 - b2*c1 - b1*c3 - c2*b3 + c2*b1 + c1*b3;
            GLfloat n2 = c2*a3 - c2*a1 - c1*a3 -a2*c3 + a2*c1 + a1*c3;
            GLfloat n3 = a2*b3 - a2*b1 - a1*b3 - b2*a3 + b2*a1 +b1*a3;

            GLfloat norme_n = sqrt(pow(n1,2) + pow(n2,2) + pow(n3,2));

            glNormal3f(n1/norme_n, n2/norme_n, n3/norme_n);

            //Formation des faces de la boule
            glBegin(GL_POLYGON);
                glTexCoord2f(i/float(NM), 1-(j/(float(NP)-1)));
                glVertex3f(x[i+j*NM], y[i+j*NM], z[i+j*NM]);

                glTexCoord2f((i+1)/float(NM), 1-(j/(float(NP)-1)));
                glVertex3f(x[(i+1)%NM + j*NM], y[(i+1)%NM + j*NM], z[(i+1)%NM + j*NM]);

                glTexCoord2f((i+1)/float(NM), 1-((j+1)/(float(NP)-1)));
                glVertex3f(x[(i+1)%NM + (j+1)*NM], y[(i+1)%NM + (j+1)*NM], z[(i+1)%NM + (j+1)*NM]);

                glTexCoord2f(i/float(NM), 1-((j+1)/(float(NP)-1)));
                glVertex3f(x[i+(j+1)*NM], y[i+(j+1)*NM], z[i+(j+1)*NM]);
            glEnd();
        }
    }
}

void MonCylindre(float r, float h, int n)
{
  float x[n];
  float y[n];

  for(int i=0; i<n; i++)
  {
    x[i] = r*cos(2*M_PI*i/n);
    y[i] = r*sin(2*M_PI*i/n);
  }

  for(int i=0; i<n; i++)
  {
    //Calcul normale couvercle sup
    GLfloat a1 = 0;
    GLfloat b1 = 0;
    GLfloat c1 = h/2;
    GLfloat a2 = x[i];
    GLfloat b2 = y[i];
    GLfloat c2 = h/2;
    GLfloat a3 = x[(i+1)%n];
    GLfloat b3 = y[(i+1)%n];
    GLfloat c3 = h/2;

    GLfloat n1 = b2*c3 - b2*c1 - b1*c3 - c2*b3 + c2*b1 + c1*b3;
    GLfloat n2 = c2*a3 - c2*a1 - c1*a3 -a2*c3 + a2*c1 + a1*c3;
    GLfloat n3 = a2*b3 - a2*b1 - a1*b3 - b2*a3 + b2*a1 +b1*a3;

    GLfloat norme_n = sqrt(pow(n1,2) + pow(n2,2) + pow(n3,2));

    glNormal3f(n1/norme_n, n2/norme_n, n3/norme_n);

  //Couvercle sup
    glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0);
        glVertex3f(x[i], y[i], h/2);

        glTexCoord2f(1, 1);
        glVertex3f(x[(i+1)%n], y[(i+1)%n], h/2);

        glTexCoord2f(0, 1);
        glVertex3f(0, 0, h/2);
    glEnd();

    //Calcul normal couvercle inf
    a1 = 0;
    b1 = 0;
    c1 = -h/2;
    a2 = x[i];
    b2 = y[i];
    c2 = -h/2;
    a3 = x[(i+1)%n];
    b3 = y[(i+1)%n];
    c3 = -h/2;

    n1 = b2*c3 - b2*c1 - b1*c3 - c2*b3 + c2*b1 + c1*b3;
    n2 = c2*a3 - c2*a1 - c1*a3 -a2*c3 + a2*c1 + a1*c3;
    n3 = a2*b3 - a2*b1 - a1*b3 - b2*a3 + b2*a1 +b1*a3;

    norme_n = sqrt(pow(n1,2) + pow(n2,2) + pow(n3,2));

    glNormal3f(n1/norme_n, n2/norme_n, n3/norme_n);

    //Couvercle inf
    glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0);
        glVertex3f(x[i], y[i], -h/2);

        glTexCoord2f(1, 1);
        glVertex3f(x[(i+1)%n], y[(i+1)%n], -h/2);

        glTexCoord2f(0, 1);
        glVertex3f(0, 0, -h/2);
    glEnd();

    //Calcul normal corps
    a1 = x[i];
    b1 = y[i];
    c1 = h/2;
    a2 = x[i];
    b2 = y[i];
    c2 = -h/2;
    a3 = x[(i+1)%n];
    b3 = y[(i+1)%n];
    c3 = -h/2;

    n1 = b2*c3 - b2*c1 - b1*c3 - c2*b3 + c2*b1 + c1*b3;
    n2 = c2*a3 - c2*a1 - c1*a3 -a2*c3 + a2*c1 + a1*c3;
    n3 = a2*b3 - a2*b1 - a1*b3 - b2*a3 + b2*a1 +b1*a3;

    norme_n = sqrt(pow(n1,2) + pow(n2,2) + pow(n3,2));

    glNormal3f(n1/norme_n, n2/norme_n, n3/norme_n);

    //Corps
    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);

    glTexCoord2f(double(i)/double(n), 0.0);
    glVertex3f(x[i], y[i], h/2);

    glTexCoord2f(double(i)/double(n), 1.0);
    glVertex3f(x[i], y[i], -h/2);

    glTexCoord2f((double(i)+1)/double(n), 1.0);
    glVertex3f(x[(i+1)%n], y[(i+1)%n], -h/2);

    glTexCoord2f((double(i)+1)/double(n), 0);
    glVertex3f(x[(i+1)%n], y[(i+1)%n], h/2);

    glEnd();
  }
}

void cube()
{
    //Tableau pour stocker les sommets du cube
    Point pCube[8]={
    { 0.5, -0.5, -0.5},
    { 0.5, 0.5, -0.5},
    { -0.5, 0.5, -0.5},
    { -0.5, -0.5, -0.5},
    { 0.5, -0.5, 0.5},
    { 0.5, 0.5, 0.5},
    { -0.5, 0.5, 0.5},
    { -0.5, -0.5, 0.5}};

    //Tableau pour stocker les indices des sommets par face pour le cube
    int fCube[6][4]={
    {0,3,2,1},
    {0,1,5,4},
    {1,2,6,5},
    {2,3,7,6},
    {0,4,7,3},
    {4,5,6,7}};

    for (int i=0;i<6;i++)
    {
        if(i==1 && face)
        {
            //Si i vaut 1 et que le bool�en face est vrai on applique la texture de la face et on tourne le cube de -90�
            glRotated(-90, 1, 0, 0);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image3);
        }

        //Calcul de la normale
        GLfloat a1 = pCube[fCube[i][0]].x;
        GLfloat b1 = pCube[fCube[i][0]].y;
        GLfloat c1 = pCube[fCube[i][0]].z;
        GLfloat a2 = pCube[fCube[i][1]].x;
        GLfloat b2 = pCube[fCube[i][1]].y;
        GLfloat c2 = pCube[fCube[i][1]].z;
        GLfloat a3 = pCube[fCube[i][2]].x;
        GLfloat b3 = pCube[fCube[i][2]].y;
        GLfloat c3 = pCube[fCube[i][2]].z;

        GLfloat n1 = b2*c3 - b2*c1 - b1*c3 - c2*b3 + c2*b1 + c1*b3;
        GLfloat n2 = c2*a3 - c2*a1 - c1*a3 -a2*c3 + a2*c1 + a1*c3;
        GLfloat n3 = a2*b3 - a2*b1 - a1*b3 - b2*a3 + b2*a1 +b1*a3;

        GLfloat norme_n = sqrt(pow(n1,2) + pow(n2,2) + pow(n3,2));

        glNormal3f(n1/norme_n, n2/norme_n, n3/norme_n);

      glBegin(GL_POLYGON);
      for (int j=0;j<4;j++){
          //glColor3f(couleur.r, couleur.g, couleur.b);
          switch(j)
          {
            case  0:
                glTexCoord2f(0, 1);
                break;
            case 1:
                glTexCoord2f(1, 1);
                break;
            case 2:
                glTexCoord2f(1, 0);
                break;
            case 3:
                glTexCoord2f(0, 0);
                break;
            default :
                std::cout<<"Probleme chargement texture cube"<<std::endl;
                break;
          }

          glVertex3f(pCube[fCube[i][j]].x,pCube[fCube[i][j]].y,pCube[fCube[i][j]].z);


      }
      glEnd();
      if(i==1 && face)
      {
        //On annule la rotation et le changement de texture pour la face
        glRotated(90, 1, 0, 0);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image1);
      }
    }
}

        /***************/
        /*MES FONCTIONS*/
        /***************/

void tete()
{
    //Chargement de la texture noire
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image1);

    face = true;

    //Tete
    glPushMatrix();
        glScaled(1, 1, 1);
        cube();
    glPopMatrix();

    face = false;

    //Machoire Inf�rieure
    glPushMatrix();
        glTranslated(1, 0.125-0.5, 0);

        glTranslated(-0.5, 0, 0);
        glRotated(machoire, 0, 0, 1);
        glTranslated(0.5, 0, 0);

        glScaled(1, 0.25, 0.8);
        cube();
    glPopMatrix();

    //Machoire Sup�rieure
    glPushMatrix();
        glTranslated(1, 0.125-0.5 + 0.35, 0);
        glScaled(1, 0.25, 0.8);
        cube();
    glPopMatrix();

    //Naseau Droit
    glPushMatrix();
        glTranslated(1.25, 0.175, -0.25);
        glScaled(0.3, 0.15, 0.2);
        cube();
    glPopMatrix();

    //Naseau Gauche
    glPushMatrix();
        glTranslated(1.25, 0.175, 0.25);
        glScaled(0.3, 0.15, 0.2);
        cube();
    glPopMatrix();

    //Chargement texture grise
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image2);

    //Oreille Droite
    glPushMatrix();
        glTranslated(0, 0.65, -0.25);
        glScaled(0.4, 0.3, 0.15);
        cube();
    glPopMatrix();

    //Oreille Droite
    glPushMatrix();
        glTranslated(0, 0.65, 0.25);
        glScaled(0.4, 0.3, 0.15);
        cube();
    glPopMatrix();
}

void cou()
{
    //Cou
    //Chargement texture noire
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image1);

    glPushMatrix();
        glTranslated(-(0.5+1.5), 0, 0);
        glScaled(3, 0.6, 0.6);
        cube();
    glPopMatrix();

    //Ecailles

    //Chargement texture grise
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image2);

    glPushMatrix();
        glTranslated(-0.5-1.5, 0.3+0.15, 0);
        int nb_ecailles = 5;
        for(int i=1; i<nb_ecailles+1; i++)
        {
            glPushMatrix();
                glTranslated(-1.5+((3-0.4*double(nb_ecailles))/(double(nb_ecailles)+1))*i + (0.4*(i-1)) + 0.2, 0, 0);
                glScaled(0.4, 0.3, 0.15);
                cube();
            glPopMatrix();
        }
    glPopMatrix();
}

void corps()
{
    //Corps
    //Chargement texture noire
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image1);

    glPushMatrix();
        glTranslated(-(0.5+3+2.5), 0, 0);
        glScaled(5, 1.8, 1.5);
        cube();
    glPopMatrix();

    //Ecailles
    //Chargement texture grise
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image2);

    glPushMatrix();
        glTranslated(-(0.5+3+2.5), 0.9+0.25, 0);
        int nb_ecailles = 3;
        for(int i=1; i<nb_ecailles+1; i++)
        {
            glPushMatrix();
                glTranslated(-2.5 + ((5-1*double(nb_ecailles))/(double(nb_ecailles)+1))*i + (i-1)*1 + 0.5, 0, 0);
                glScaled(1, 0.5, 0.3);
                cube();
            glPopMatrix();
        }
    glPopMatrix();
}

void Queue()
{
    glPushMatrix();
        glTranslated(-(0.5+3+5), 0.3, 0);
        for(int i=0; i<8; i++)
        {
            glPushMatrix();
                //Animation
                glTranslated(0, 0, dh*i);

                glTranslated(-(0.35*(i+1) + 0.35*i), 0, 0);

                //Ecailles
                //Chargement texture grise
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image2);

                glPushMatrix();
                    glTranslated(0, 0.35+0.15, 0);
                    glScaled(0.4, 0.3, 0.15);
                    cube();
                glPopMatrix();

                //Queue
                //Chargement texture noire
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image1);

                glScaled(0.7, 0.7, 0.7);
                cube();
            glPopMatrix();
        }
    glPopMatrix();
}

void ailes()
{
    //Constantes des composants
    float rsphere = 0.25;
    int nsphere = 10;
    float rcylindre = 0.1, lcylindre = 4;
    int ncylindre = 10;

    //Boucle pour cr�er une aile et sa sym�trie (� l'aide de l'expression pow(-1,i))
    for(int i=0; i<2; i++)
    {
        glPushMatrix();
            //Animation des ailes
            glTranslated(-(0.5+3+rsphere), 0.9 - rsphere, pow(-1,i)*0.75);
            glRotated(-pow(-1,i)*10*dr, 1, 0, 0);

            //Peau
            //Chargement texture noire
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image1);

            glPushMatrix();
                //glColor3f(0.2, 0.2, 0.2);

                //Polygone 1
                glBegin(GL_POLYGON);
                    //1
                    glTexCoord2f(0, 0);
                    glVertex3f(0, 0, 0);

                    //7
                    glTexCoord2f(0, 1);
                    glVertex3f(-lcylindre*sin(M_PI/4) + 0.3, 0, 0);

                    //6
                    glTexCoord2f(1, 1);
                    glVertex3f(-lcylindre*sin(M_PI/4), 0, pow(-1,i)*(lcylindre - lcylindre*cos(M_PI/4)) );

                    //2
                    glTexCoord2f(1, 0);
                    glVertex3f(0, 0, pow(-1,i)*lcylindre);
                glEnd();

                //Polygon 2
                glBegin(GL_TRIANGLES);
                    //2
                    glTexCoord2f(0, 0);
                    glVertex3f(0, 0, pow(-1,i)*lcylindre);

                    //6
                    glTexCoord2f(1, 1);
                    glVertex3f(-lcylindre*sin(M_PI/4), 0, pow(-1,i)*(lcylindre - lcylindre*cos(M_PI/4)) );

                    //5
                    glTexCoord2f(0, 1);
                    glVertex3f(-lcylindre, 0, pow(-1,i)*lcylindre);
                glEnd();


                glPushMatrix();
                    //Animation de la peau
                    glTranslated(0, 0, pow(-1,i)*lcylindre);
                    glRotated(pow(-1,i)*(-10*dr), 1, 0, 0);
                    glTranslated(0, 0, -pow(-1,i)*lcylindre);

                    //Polygon 3
                    glBegin(GL_TRIANGLES);
                        //2
                        glTexCoord2f(0, 0);
                        glVertex3f(0, 0, pow(-1,i)*lcylindre);

                        //5
                        glTexCoord2f(1, 1);
                        glVertex3f(-lcylindre, 0, pow(-1,i)*lcylindre);

                        //4
                        glTexCoord2f(0, 1);
                        glVertex3f(-lcylindre*cos(M_PI/9)*cos(M_PI/4), -lcylindre*sin(M_PI/9), pow(-1,i)*( lcylindre + lcylindre*cos(M_PI/9)*sin(M_PI/4) ) );
                    glEnd();

                    //Polygon 4
                    glBegin(GL_TRIANGLES);
                        //2
                        glTexCoord2f(0, 0);
                        glVertex3f(0, 0, pow(-1,i)*lcylindre);

                        //4
                        glTexCoord2f(1, 1);
                        glVertex3f(-lcylindre*cos(M_PI/9)*cos(M_PI/4), -lcylindre*sin(M_PI/9), pow(-1,i)*( lcylindre + lcylindre*cos(M_PI/9)*sin(M_PI/4) ) );

                        //3
                        glTexCoord2f(0, 1);
                        glVertex3f(0, -sin(M_PI/9)*lcylindre, pow(-1,i)*(lcylindre + cos(M_PI/9)*lcylindre));
                    glEnd();
                glPopMatrix();

            glPopMatrix();

            //Squelette
            //Chargement texture grise
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image2);

            glPushMatrix();
                //Premi�re sph�re
                glColor3f(170.0/255.0, 250.0/250.0, 150.0/255.0);
                MaSphere(rsphere, nsphere, nsphere);

                //Premier cylindre
                glTranslated(0, 0, pow(-1,i)*lcylindre/2);
                glColor3f(247.0/255.0, 235.0/255.0, 153.0/255.0);
                MonCylindre(rcylindre, lcylindre, ncylindre);

                //Deuxi�me sph�re
                glTranslated(0, 0, pow(-1,i)*lcylindre/2);
                glColor3f(170.0/255.0, 250.0/250.0, 150.0/255.0);
                MaSphere(rsphere, nsphere, nsphere);

                glPushMatrix();
                    //Deuxi�me cylindre (On effetue la translation avant la rotation)
                    //Inclinaison du bout de l'aile
                    glRotated(pow(-1,i)*(20 - 10*dr), 1, 0, 0);
                    glTranslated(0, 0, pow(-1,i)*lcylindre/2);
                    glColor3f(247.0/255.0, 235.0/255.0, 153.0/255.0);
                    MonCylindre(rcylindre, lcylindre, ncylindre);
                glPopMatrix();

                glPushMatrix();
                    //Premi�re ar�te (On effetue la translation avant la rotation)
                    glRotated(pow(-1,i)*45, 0, 1, 0);
                    glTranslated(0, 0, -pow(-1,i)*lcylindre/2);
                    glColor3f(247.0/255.0, 235.0/255.0, 153.0/255.0);
                    MonCylindre(rcylindre/1.5, lcylindre, ncylindre);
                    glTranslated(0, 0, pow(-1,i)*lcylindre/2);

                    //Deuxi�me ar�te (On effetue la translation avant la rotation)
                    glRotated(pow(-1,i)*45, 0, 1, 0);
                    glTranslated(0, 0, -pow(-1,i)*lcylindre/2);
                    glColor3f(247.0/255.0, 235.0/255.0, 153.0/255.0);
                    MonCylindre(rcylindre/1.5, lcylindre, ncylindre);
                    glTranslated(0, 0, pow(-1,i)*lcylindre/2);

                    //Trois�me ar�te
                    glRotated(pow(-1,i)*45, 0, 1, 0);
                    glRotated(-pow(-1,i)*(20 - 10*dr), 1, 0, 0);
                    glTranslated(0, 0, -pow(-1,i)*lcylindre/2);
                    glColor3f(247.0/255.0, 235.0/255.0, 153.0/255.0);
                    MonCylindre(rcylindre/1.5, lcylindre, ncylindre);

                glPopMatrix();

            glPopMatrix();
        glPopMatrix();
    }
}

void pattesAvant()
{
    //Constantes des pattes
    float xBras = 0.55, yBras = 1.1,zBras = 0.55;
    float proportionnalite = 0.8;
    float rotBrasAvtBras = -30; //En degr�
    float xAvtBras = proportionnalite*xBras, yAvtBras = yBras, zAvtBras = proportionnalite*zBras;
    float xPatte = 2*xAvtBras, yPatte = 0.15*yAvtBras, zPatte = 1.3*zAvtBras;

    //Chargement de la texture noire
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image1);

    //Boucle cr�eant les 2 pattes avant
    for(int i=0; i<2; i++)
    {
        glPushMatrix();
            glTranslated(-(0.5+3+zBras/2), -yBras/2, pow(-1,i)*(0.75 + zBras/2));
            glRotated(rotBrasAvtBras, 0, 0, 1);

            //Animation des pattes
            glTranslated(0, yBras/2, 0);
            glRotated(-3*dr, 0, 0, 1);
            glTranslated(0, -yBras/2, 0);

            //Bras
            glPushMatrix();
                glScaled(xBras, yBras, zBras);
                cube();
            glPopMatrix();

            //Rotation des Avant-Bras
            glTranslated(0, -yBras/2 , 0);
            glRotated(-30, 0, 0, 1);

            //Avant Bras
            glPushMatrix();
                glTranslated(0, -(yAvtBras/2) , 0);
                glScaled(xAvtBras, yAvtBras, zAvtBras);
                cube();
            glPopMatrix();

            //Patte
            glPushMatrix();
                glTranslated((xPatte*0.7 - xAvtBras)/2, -(yAvtBras + yPatte/2), 0);
                glScaled(xPatte, yPatte, zPatte);
                cube();
            glPopMatrix();

        glPopMatrix();
    }
}

void pattesArriere()
{
    //Constantes des pattes
    float xBras = 0.8, yBras = 1.4,zBras = 0.8;
    float proportionnalite = 0.8;
    float rotBrasAvtBras = -30; //En degr�
    float xAvtBras = proportionnalite*xBras, yAvtBras = yBras, zAvtBras = proportionnalite*zBras;
    float xPatte = 2*xAvtBras, yPatte = 0.15*yAvtBras, zPatte = 1.3*zAvtBras;

    //Chargement de la texture noire
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,image1);

    //Boucle cr�eant les 2 pattes avant
    for(int i=0; i<2; i++)
    {
        glPushMatrix();
            glTranslated(-(0.5+3+5-xBras*1.8/2), -yBras/2, pow(-1,i)*(0.75 + zBras/2));
            glRotated(rotBrasAvtBras, 0, 0, 1);

            //Animation des Pates
            glTranslated(0, yBras/2, 0);
            glRotated(-3*dr, 0, 0, 1);
            glTranslated(0, -yBras/2, 0);

            //Bras
            glPushMatrix();
                glScaled(xBras, yBras, zBras);
                cube();
            glPopMatrix();

            //Rotation des Avant-Bras
            glTranslated(0, -yBras/2 , 0);
            glRotated(-30, 0, 0, 1);

            //Avant Bras
            glPushMatrix();
                glTranslated(0, -(yAvtBras/2) , 0);
                glScaled(xAvtBras, yAvtBras, zAvtBras);
                cube();
            glPopMatrix();

            //Patte
            glPushMatrix();
                glTranslated((xPatte*0.7 - xAvtBras)/2, -(yAvtBras + yPatte/2), 0);
                glScaled(xPatte, yPatte, zPatte);
                cube();
            glPopMatrix();

        glPopMatrix();
    }

}

void animation()
{
    glTranslated(AVANT, HAUT + 3*dh, COTE);
    glRotated(dr, 1, 0, 0);
    if(activateAnimation)
    {
        //Mouvement du corps (Mouvement de la queue dans la fonction Queue())
        if(mouve) dh -= h/nb_mvt;
        else dh += h/nb_mvt;

        if(dh >= h) mouve = true;
        else if (dh <= -h) mouve = false;

        //Angle de rotation
        if(rot) dr -= r/(nb_mvt/2);
        else dr += r/(nb_mvt/2);

        if(dr >= r) rot = true;
        else if (dr <= -r) rot = false;

        glutPostRedisplay();
    }
}

void clavier(unsigned char touche,int x,int y)
{
    //std::cout<<"Touche : "<<touche<<" Code : "<<int(touche)<<std::endl;
    switch(touche)
    {
        case 'p': /* affichage du carre plein */
            STYLE = GL_FILL;
            glPolygonMode(GL_FRONT_AND_BACK, STYLE);
            glutPostRedisplay();
            break;
        case 'f': /* affichage en mode fil de fer */
            STYLE = GL_LINE;
            glPolygonMode(GL_FRONT_AND_BACK, STYLE);
            glutPostRedisplay();
            break;
        case 'z':
            if(ZOOM == 0.5) break;
            ZOOM -= 0.5;
            glutPostRedisplay();
            break;
        case 'Z':
            ZOOM += 0.5;
            glutPostRedisplay();
            break;
        case 'h':
            HAUT += 0.2;
            glutPostRedisplay();
            break;
        case  'H':
            HAUT -= 0.2;
            glutPostRedisplay();
            break;
        case  'a':
            AVANT += 0.2;
            glutPostRedisplay();
            break;
        case  'A':
            AVANT -= 0.2;
            glutPostRedisplay();
            break;
        case  'c':
            COTE -= 0.2;
            glutPostRedisplay();
            break;
        case  'C':
            COTE += 0.2;
            glutPostRedisplay();
            break;
        case  'r':
            if(machoire >= 0) break;
            else machoire += 1;
            glutPostRedisplay();
            break;
        case  'R':
            if(machoire <= -30) break;
            else machoire -= 1;
            glutPostRedisplay();
            break;

        case  32: /* touche espace */
            activateAnimation = !activateAnimation;
            glutPostRedisplay();
            break;

        case 27: /* touche ESC */
            exit(0);
        default:
            break;
    }
}

void touchesSpecial(int key, int x, int y)
{
    switch (key)
	{
		case GLUT_KEY_UP :
			angley += 5;
			glutPostRedisplay();
			break;

		case GLUT_KEY_DOWN :
			angley -= 5;
			glutPostRedisplay();
			break;

		case GLUT_KEY_LEFT :
			anglex += 5;
			glutPostRedisplay();
			break;

		case GLUT_KEY_RIGHT :
			anglex -= 5;
			glutPostRedisplay();
			break;

		default :
			//std::cout<<"Autre : "<<key<<std::endl;
			break;
    }
}

void souris(int bouton, int etat,int x,int y)
{
  if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_DOWN)
  {
    presse = 1;
    xold = x;
    yold=y;
  }
  if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_UP)
    presse=0;
}

void sourismouv(int x,int y)
  {
    if (presse)
    {
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay();
    }

    xold=x;
    yold=y;
  }

void redim(int l,int h)
{
  if (l<h)
    glViewport(0,(h-l)/2,l,l);
  else
    glViewport((l-h)/2,0,h,h);
}



void loadJpegImage(char *fichier, unsigned char image[])
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
#ifdef __WIN32
  if (fopen_s(&file,fichier,"rb") != 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#elif __GNUC__
  if ((file = fopen(fichier,"rb")) == 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#endif
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*largimg*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
}
