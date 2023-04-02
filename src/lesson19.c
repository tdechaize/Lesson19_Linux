/*
=====================================================================
OpenGL Lesson 19 :  Creating Another OpenGL Window with GLX on Linux
=====================================================================

  Authors Name: Jeff Molofee ( NeHe )

  This code was created by Jeff Molofee '99 (ported to Linux/GLX by Mihael Vrbanec '01)

  If you've found this code useful, please let me know.

  Visit Jeff at http://nehe.gamedev.net/

  or for port-specific comments, questions, bugreports etc.
  email to Mihael.Vrbanec@stud.uni-karlsruhe.de

  Disclaimer:

  This program may crash your system or run poorly depending on your
  hardware.  The program and code contained in this archive was scanned
  for virii and has passed all test before it was put online.  If you
  use this code in project of your own, send a shout out to the author!

=====================================================================
                NeHe Productions 1999-2004
=====================================================================
*/

 /*******************************************************************
 *  Project: $(project)
 *  Function : Main program
 ***************************************************************
 *  $Author: Jeff Molofee 2000 ( NeHe )
 *  $Name:  $
 ***************************************************************
 *
 *  Copyright NeHe Production
 *
 *******************************************************************/

/**         Comments manageable by Doxygen
*
*  Modified smoothly by Thierry DECHAIZE
*
*  Paradigm : obtain one source (only one !) compatible for multiple free C Compilers
*    and provide for all users an development environment on Linux (64 bits compatible),
*    the great Code::Blocks manager (version 20.03), and don't use glaux.lib or glaux.dll.
*
*	a) gcc 11.3.0 (32 and 64 bits) version officielle : commande pour l'installation sur Linux Mint
*       -> sudo apt-get install freeglut3 libglew-dev gcc g++ mesa-common-dev build-essential libglew2.2 libglm-dev binutils libc6 libc6-dev gcc-multilib g++-multilib; option -m32 to 32 bits ; no option to 64 bits
*	b) CLANG version 14.0.6 (32 et 64 bits), adossé aux environnements gcc : commande pour l'installation sur Linux Mint
*       -> sudo apt-get install llvm clang ; options pour la compilation et l'édition de liens -> --target=i686-pc-linux-gnu (32 bits) --target=x86_64-pc-linux-gnu (64 bits)
*
*  Date : 2023/04/02
*
* \file            lesson19.c
* \author          Jeff Molofee ( NeHe ) originely, ported to Linux/GLX by Mihael Vrbanec, and after modified smotthly by Thierry Dechaize on Linux Mint
* \version         2.0.1.0
* \date            2 avril 2023
* \brief           Ouvre une simple fenêtre X11 on Linux et dessine un système de génération de particules à partir d'un modèle d'étoile issu d'un fichier BITMAP.
* \details         Ce programme gére plusieurs événements : le clic sur le bouton "Fermé" de la fenêtre, la sortie du programme par la touche ESCAPE ou par les touches "q" ou "Q" [Quit]
* \details                                                  les touches "b" ou "B" qui active ou non le "blending", les touches "i" ou "I" qui active ou non le "Filter",
* \details                                                  les touches "l" ou "L" qui active ou non le "ligthing", les touches "f" ou "F" qui active ou non le "Full Screen".
* \details                                                  les touches spéciales UP, DOWN, LEFT, RIGTH qui agissent sur les rotations respectivement selon l'orientation choisie
* \details                                                  les touches PG-UP / PG-DOWN qui agissent sur la profondeur choisie et enfin la touche "Espace" qui permet de choisir la forme à afficher.
*
* \details          Point d'attention : il faut lancer l'exécutable à partir des sous répertoires bin...../Release ou bin...../Debug car le chargement des données est indiqué sous une forme relative : ../../Data/...
*
*       FIXME :  2023/01/28 -> Le passage en Fullscreen sur une configuration à deux écrans physiques donne un résultat inattendu : deux fenêtres d'affichage GL Windows se "dupliquent" avec le même contenu,
*                              et plus grave, la résolution de la deuxième fenêtre (sur le deuxième écran) est très faible, ce qui provoque un recouvrement de la deuxième fenêtre sur les deux écrans physiques !!!
*                              Et encore plus gênant, le "grab" s'applique sur cette deuxième fenêtre, donc les actions de la souris comme du clavier restent restreints à cette fenêtre et ne permettent
*                              plus l'accès par exemple aux icônes et aux menus importants sous Linux Mint. Seul un appui sur le bouton d'extinction de mon PC portable permet d'arrêter le système proprement.
*                              Juste pour comprendre la configuration de mon PC portable sous Linux Mint 22.1, j'ai un écran principal (celui du portable) à gauche (écran primaire) et j'ai un deuxième écran
*                              secondaire à droite (HDMI). Et sous Linux Mint, le paramètrage de l'affichage étendu ne permet pas la duplication de la barre de "tâches" sur les deux écrans comme avec Windows 11.
*
*/

#define _XOPEN_SOURCE   600  // Needed because use of function usleep depend of these two define ...!!! However function usleep appear like "... warning: implicit declaration of ..."
#include <unistd.h>             // Header file for sleeping (function usleep)
#include <stdio.h>              // Header file needed by use of printf in this code
#include <string.h>             // Header file needed by use of strcmp in this code
#include <stdlib.h>             // Header file needed by use of malloc/free function in this code  				// Header file for memset.
#include <GL/glx.h>             // Header File For The X library for OpenGL
#include <GL/gl.h>              // Header File For The OpenGL Library
#include <GL/glu.h>             // Header File For The GLu Library (Utilities)
#include <X11/Xlib.h>       			// Standard X header for X libraries
#include <X11/extensions/xf86vmode.h>	// Header to provide acces to extensions XFree86 (example : videomode)
#include <X11/Xatom.h>     				// Header to provide X's Atom functionality
#include <X11/keysym.h>	   				// Header to provide keyboard accessibility

#include "logger.h"     //  added by Thierry DECHAIZE : logger for trace and debug if needed ... only in mode Debug !!!

#define MAX_PARTICLES 1000      /* number of particles to create */

char *level_debug;    // added by Thierry DECHAIZE, needed in test of logging activity (only with DEBUG mode)

/* Global variables */
Atom    wmDeleteWindow; 		// Custom message to Delete Window
Display *dpDisplay;     		// Display variable
Window  win;            		// Current Window variable

XWindowAttributes window_attr;  // Attributs of X11 window
/* Indicator of Full Screen */
int nFullScreen=0;

/* stuff about our window grouped together */
typedef struct {
    Display 			*dpy;
    int 				screen;
    Window 				win;
    GLXContext 			ctx;
    XSetWindowAttributes attr;
    Bool 				fs;
    XF86VidModeModeInfo deskMode;
    int 				x, y;
    unsigned int 		width, height;
    unsigned int 		depth;
} GLWindow;

typedef struct {
    int 			width;
    int 			height;
    unsigned char 	*data;
} textureImage;

/* stuff to describe our particle */
typedef struct {
    Bool  active;
    float life;
    float fade;
    float red;
    float green;
    float blue;
    float xPos;
    float yPos;
    float zPos;
    float xSpeed;
    float ySpeed;
    float zSpeed;
    float xGrav;
    float yGrav;
    float zGrav;
} particles;

/* attributes for a single buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListSgl[] = {GLX_RGBA,
	GLX_RED_SIZE, 4,
    GLX_GREEN_SIZE, 4,
    GLX_BLUE_SIZE, 4,
    GLX_DEPTH_SIZE, 16,
    None
};

/* attributes for a double buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER,
    GLX_RED_SIZE, 4,
    GLX_GREEN_SIZE, 4,
    GLX_BLUE_SIZE, 4,
    GLX_DEPTH_SIZE, 16,
    None
};

/* our array of rainbow colors */
static GLfloat colors[12][3]= {	{1.0f, 0.5f, 0.5f}, {1.0f, 0.75f, 0.5f},
    {1.0f, 1.0f, 0.5f}, {0.75f, 1.0f, 0.5f}, {0.5f, 1.0f, 0.5f},
    {0.5f, 1.0f, 0.75f}, {0.5f, 1.0f, 1.0f}, {0.5f, 0.75f, 1.0f},
	{0.5f, 0.5f, 1.0f}, {0.75f, 0.5f, 1.0f}, {1.0f, 0.5f, 1.0f},
    {1.0f, 0.5f, 0.75f}
};


GLWindow    GLWin;
Bool        done;
Bool        keys[256];
int         keyCodes[17];       /* array to hold our fetched keycodes */
Bool        rainbow;           /* rainbow mode ?? */
float       slowdown = 2.0f;
float       xSpeed;
float       ySpeed;
float       zoom = -40.0f;
GLuint      loop;
GLuint      color;
GLuint      delay;
GLuint      texture[1];
particles   particle[MAX_PARTICLES];

void go_fullscreen(Display *dsp, Window win)
{
  XEvent xev;
  Atom wm_state = XInternAtom(dsp, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(dsp, "_NET_WM_STATE_FULLSCREEN", False);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
         log_print(__FILE__, __LINE__,"In function go_fullscreen -> at the beginning");
#endif // DEBUG

  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = win;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
  xev.xclient.data.l[1] = fullscreen;
  xev.xclient.data.l[2] = 0;
  XSendEvent(dsp, DefaultRootWindow(dsp), False, SubstructureNotifyMask, &xev);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
       log_print(__FILE__, __LINE__,"In function go_fullscreen -> at the end");
#endif // DEBUG
}

void return_fullscreen(Display *dsp, Window win)
{
  XEvent xev;
  Atom wm_state = XInternAtom(dsp, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(dsp, "_NET_WM_STATE_FULLSCREEN", False);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
         log_print(__FILE__, __LINE__,"In function return_fullscreen -> at the beginning");
#endif // DEBUG

  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = win;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 0; // _NET_WM_STATE_REMOVE
  xev.xclient.data.l[1] = fullscreen;
  xev.xclient.data.l[2] = 0;
  XSendEvent(dsp, DefaultRootWindow(dsp), False, SubstructureNotifyMask | SubstructureRedirectMask, &xev);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
       log_print(__FILE__, __LINE__,"In function return_fullscreen -> at the end");
#endif // DEBUG
}

/*
 * getint and getshort are help functions to load the bitmap byte by byte on
 * SPARC platform.
 * I've got them from xv bitmap load routine because the original bmp loader didn't work
 * I've tried to change as less code as possible.
 */

static unsigned int getint(fp)
     FILE *fp;
{
  int c, c1, c2, c3;

  // get 4 bytes
  c = getc(fp);
  c1 = getc(fp);
  c2 = getc(fp);
  c3 = getc(fp);

  return ((unsigned int) c) +
    (((unsigned int) c1) << 8) +
    (((unsigned int) c2) << 16) +
    (((unsigned int) c3) << 24);
}

static unsigned int getshort(fp)
     FILE *fp;
{
  int c, c1;

  //get 2 bytes
  c = getc(fp);
  c1 = getc(fp);

  return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more info.

/**	            This function load image stored in file BMP (quick and dirty bitmap loader !! bug on SOLARIS/SPARC, two functions added below correct this code)
*
* \brief      Fonction ImageLoad : fonction chargeant une image stockée dans un fichier BMP
* \details    En entrée, le nom du fichier stockant l'image, en retour l'image chargée en mémoire.
* \param	  *filename			Le nom du fichier stockant l'image
* \param	  *texture			l'image chargée en mémoire
* \return     int               un entier de type booléen (0 / 1).
*
*/

int ImageLoad(char *filename, textureImage *texture) {
    FILE *file;
#ifdef __x86_64__
    unsigned int size;                  // size of the image in bytes
#else
    unsigned long size;                 // size of the image in bytes
#endif

#ifdef __x86_64__
    unsigned int i;                     // standard counter (4 bytes stored value)
#else
    unsigned long i;                    // standard counter (4 bytes stored value)
#endif

    unsigned short int planes;          // number of planes in image (must be 1)
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // used to convert bgr to rgb color.

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function ImageLoad.");
#endif // defined DEBUG

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL) {
      printf("File Not Found : %s\n",filename);
      return 0;
    }

    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // No 100% errorchecking anymore!!!

    // read the width
    texture->width = getint (file);
    printf("Width of %s: %u\n", filename, texture->width);

    // read the height
    texture->height = getint (file);
    printf("Height of %s: %u\n", filename, texture->height);

    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = texture->width * texture->height * 3;

    // read the planes
    planes = getshort(file);
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    // read the bpp
    bpp = getshort(file);
    if (bpp != 24) {
      printf("Bpp from %s is not 24: %u\n", filename, bpp);
      return 0;
    }

    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data.
    texture->data = (char *) malloc(size);
    if (texture->data == NULL) {
	  printf("Error allocating memory for color-corrected image data");
	  return 0;
    }

    if ((i = fread(texture->data, size, 1, file)) != 1) {
	  printf("Error reading image data from %s.\n", filename);
	  return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	  temp = texture->data[i];
	  texture->data[i] = texture->data[i+2];
	  texture->data[i+2] = temp;
    }

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function ImageLoad.");
#endif // defined DEBUG

    // we're done.
    return 1;
}

// Load texture into memory

/**	            This function load textures used for object with instructions OpenGL
*
* \brief      Fonction LoadGLTextures : fonction chargeant la texture à appliquer à un objet avec des instructions OpenGL
* \details    Aucun paramètre en entrée, et justeb un statut d'appel en retour (Bool).
* \return     Bool              un booléen donnant le statut OK/NOK du chargement des textures.
*
*/

Bool loadGLTextures()   /* Load Bitmaps And Convert To Textures */
{
    Bool status;
    textureImage *texti;

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function LoadGLTextures.");
#endif // defined DEBUG

    status = False;
    texti = malloc(sizeof(textureImage));
    if (ImageLoad("../../Data/Particle.bmp", texti))
    {
        status = True;
        glGenTextures(1, &texture[0]);   /* create three textures */
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        /* use linear filtering */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /* actually generate the texture */
        glTexImage2D(GL_TEXTURE_2D, 0, 3, texti->width, texti->height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, texti->data);
    }
    /* free the ram we used in our texture generation process */
    if (texti)
    {
        if (texti->data)
            free(texti->data);
        free(texti);
    }

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function LoadGLTextures.");
#endif // defined DEBUG

    return status;
}


/* function called when our window is resized */

/**	            This function manage the new dimension of the scene when resize of windows with instructions OpenGL
*
* \brief      Fonction ReSizeGLScene : fonction gerant les nouvelles dimensions de la scéne lorsque l'utilisateur agit sur un redimensionnement de la fenêtre
* \details    En retour les deux paramètres des nouvelles valeurs de largeur et de hauteur de la fenêtre redimensionnée.
* \param	  width			    la  nouvelle largeur de la fenêtre redimensionnée					*
* \param	  height			la  nouvelle hauteur de la fenêtre redimensionnée					*
*
*/

void resizeGLScene(unsigned int width, unsigned int height)
{
#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function ResizeGLScene.");
#endif // defined DEBUG

    if (height == 0)    /* Prevent A Divide By Zero If The Window Is Too Small */
        height = 1;
    glViewport(0, 0, width, height);    /* Reset The Current Viewport And Perspective Transformation */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function ResizeGLScene.");
#endif // defined DEBUG
}

/* general OpenGL initialization function */

/**	            This function initialize the basics characteristics of the scene with instructions OpenGL (background, depth, type of depth, reset of the projection matrix, ...)
*
* \brief      Fonction InitGL : fonction gerant les caractéristiques de base de la scéne lorsque avec des instructions OpenGL (arrière plan, profondeur, type de profondeur, ...)
* \details    Juste un statut en retour, aucun paramètre en entrée (GLVoid).
* \return     int               statut du retour.
*
*/

int initGL(GLvoid)
{
#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function InitGL.");
#endif // defined DEBUG

    if (!loadGLTextures())
    {
        return False;
    }
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    /* initializing our particles */
    for (loop = 0; loop < MAX_PARTICLES; loop++) {
        particle[loop].active = True;
        particle[loop].life = 1.0f;
        particle[loop].fade = (float) (rand() % 100) / 1000 + 0.003f;
        particle[loop].red = colors[loop / (MAX_PARTICLES / 12)][0];
        particle[loop].green = colors[loop / (MAX_PARTICLES / 12)][0];
        particle[loop].blue = colors[loop / (MAX_PARTICLES / 12)][0];
        particle[loop].xSpeed = (float) ((rand() % 50) - 26.0f) * 10.0f;
        particle[loop].ySpeed = (float) ((rand() % 50) - 25.0f) * 10.0f;
        particle[loop].zSpeed = (float) ((rand() % 50) - 25.0f) * 10.0f;
        particle[loop].xGrav = 0.0f;
        particle[loop].yGrav = -0.8f;
        particle[loop].zGrav = 0.0f;
    }
    /* we use resizeGLScene once to set up our initial perspective */
    resizeGLScene(GLWin.width, GLWin.height);
    glFlush();

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function InitGL.");
#endif // defined DEBUG

    return True;
}

/* Here goes our drawing code */

/**	            This function generate the scene with instructions OpenGL
*
* \brief      Fonction DrawGLScene : fonction generant l'affichage attendu avec des instructions OpenGL
* \details    Aucun paramètre dans ce cas de figure car tout est géré directement à l'intérieur de cette fonction d'affichage.
* \return     int             un entier "booléen" False/True.
*
*/

int drawGLScene(GLvoid)
{
    float x, y, z;

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Begin function DrawGLScene.");
#endif // defined DEBUG

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    for (loop = 0; loop < MAX_PARTICLES; loop++) {
        if (particle[loop].active)
        {
            x = particle[loop].xPos;
            y = particle[loop].yPos;
            z = particle[loop].zPos + zoom;
            glColor4f(particle[loop].red, particle[loop].green,
                      particle[loop].blue, particle[loop].life);
            glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2d(1, 1);
                glVertex3f(x + 0.5f, y + 0.5f, z);
                glTexCoord2d(0, 1);
                glVertex3f(x - 0.5f, y + 0.5f, z);
                glTexCoord2d(1, 0);
                glVertex3f(x + 0.5f, y - 0.5f, z);
                glTexCoord2d(0, 0);
                glVertex3f(x - 0.5f, y - 0.5f, z);
            glEnd();
            particle[loop].xPos += particle[loop].xSpeed / (slowdown * 1000);
            particle[loop].yPos += particle[loop].ySpeed / (slowdown * 1000);
            particle[loop].zPos += particle[loop].zSpeed / (slowdown * 1000);
            particle[loop].xSpeed += particle[loop].xGrav;
            particle[loop].ySpeed += particle[loop].yGrav;
            particle[loop].zSpeed += particle[loop].zGrav;
            particle[loop].life -= particle[loop].fade;
            if (particle[loop].life < 0.0f)
            {
                particle[loop].life = 1.0f;
                particle[loop].fade = (float) (rand() % 100) / 1000 + 0.003f;
                particle[loop].xPos = 0.0f;
                particle[loop].yPos = 0.0f;
                particle[loop].zPos = 0.0f;
                particle[loop].xSpeed = xSpeed + (float) (rand() % 60) - 32.0f;
                particle[loop].ySpeed = ySpeed + (float) (rand() % 60) - 30.0f;
                particle[loop].zSpeed = (float) (rand() % 60) - 30.0f;
                particle[loop].red = colors[color][0];
                particle[loop].green = colors[color][1];
                particle[loop].blue = colors[color][2];
            }
            /* if  keypad 8 and y gravity is less than 1.5 increase pull upwards */
            if (keys[keyCodes[2]] && (particle[loop].yGrav < 1.5f))
                particle[loop].yGrav += 0.01f;
            /* if  keypad 2 and y gravity is greater than -1.5 increase pull
            * upwards
            */
            if (keys[keyCodes[3]] && (particle[loop].yGrav > -1.5f))
                particle[loop].yGrav -= 0.01f;
            /* if  keypad 6 and x gravity is less than 1.5 increase pull right */
            if (keys[keyCodes[4]] && (particle[loop].xGrav < 1.5f))
                particle[loop].xGrav += 0.01f;
            /* if  keypad 4 and y gravity is less than 1.5 increase pull left */
            if (keys[keyCodes[5]] && (particle[loop].xGrav > -1.5f))
                particle[loop].xGrav -= 0.01f;
            if (keys[keyCodes[6]]) {
                particle[loop].xPos = 0.0f;
                particle[loop].yPos = 0.0f;
                particle[loop].zPos = 0.0f;
                particle[loop].xSpeed = (float) ((rand() % 52) - 26.0f) * 10;
                particle[loop].ySpeed = (float) ((rand() % 50) - 25.0f) * 10;
                particle[loop].zSpeed = (float) ((rand() % 50) - 25.0f) * 10;
            }
        }
    }
    glXSwapBuffers(GLWin.dpy, GLWin.win);

#ifdef DEBUG
    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"End function DrawGLScene.");
#endif // defined DEBUG

    return True;
}

/* function to release/destroy our resources and restoring the old desktop */

/**	            This function kill the GL window properly
*
* \brief      Fonction KillGLWindow : fonction détruisant proprement la fenêtre OpenGL
* \details    Aucun paramètre dans ce cas de figure car tout est géré directement à l'intérieur de cette fonction.
* \return     GLvoid             aucun retour.
*
*/

GLvoid killGLWindow(GLvoid)
{
    if (GLWin.ctx)
    {
        if (!glXMakeCurrent(GLWin.dpy, None, NULL))
        {
            printf("Could not release drawing context.\n");
        }
        glXDestroyContext(GLWin.dpy, GLWin.ctx);
        GLWin.ctx = NULL;
    }
    /* switch back to original desktop resolution if we were in fs */
    if (GLWin.fs)
    {
        XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
        XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
    }
    XCloseDisplay(GLWin.dpy);
}

/* this function creates our window and sets it up properly */
/* FIXME: bits is currently unused */

/**	            This function create the GL Window with instructions GLX
*
* \brief      Fonction createGLWindow : fonction créant une nouvelle fenêtre OpenGL sous X11, avec des instruction GLX.
* \details    En entrée cinq paramètres pour la création de la fénbêtre : son titre, sa largeur, sa hauteur, le nombre de bits pour la gestion de la couleur, et un flag Fullscreen ou non.
* \param	  title			    le titre ou le nom de la fenêtre
* \param	  width			    la largeur de la fenêtre
* \param	  height			la hauteur de la fenêtre
* \param	  bits				le nombre de bits pour la gestion de la couleur (1, 2 3 ou 4 bits)
* \param	  fullscreenflag	Fullscreen ou non
* \return     Bool        		un boolean correspondant au statut de d'appel de cette fonction.
*
*/

Bool createGLWindow(char* title, int width, int height, int bits,
                    Bool fullscreenflag)
{
    XVisualInfo 		*vi;
    Colormap 			cmap;
    int 				dpyWidth, dpyHeight;
    int 				i;
    int 				glxMajorVersion, glxMinorVersion;
    int 				vidModeMajorVersion, vidModeMinorVersion;
    XF86VidModeModeInfo **modes;
    int 				modeNum;
    int 				bestMode;
    Atom 				wmDelete;
    Window 				winDummy;
    unsigned int 		borderDummy;

    GLWin.fs = fullscreenflag;
    /* set best mode to current */
    bestMode = 0;
    /* get a connection */
    GLWin.dpy = XOpenDisplay(0);
    GLWin.screen = DefaultScreen(GLWin.dpy);
    XF86VidModeQueryVersion(GLWin.dpy, &vidModeMajorVersion,
        &vidModeMinorVersion);
    printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
        vidModeMinorVersion);
    XF86VidModeGetAllModeLines(GLWin.dpy, GLWin.screen, &modeNum, &modes);
    /* save desktop-resolution before switching modes */
    GLWin.deskMode = *modes[0];
    /* look for mode with requested resolution */
    for (i = 0; i < modeNum; i++)
    {
        if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
        {
            bestMode = i;
        }
    }
    /* get an appropriate visual */
    vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListDbl);
    if (vi == NULL)
    {
        vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListSgl);
        printf("Only Singlebuffered Visual!\n");
    }
    else
    {
        printf("Got Doublebuffered Visual!\n");
    }
    glXQueryVersion(GLWin.dpy, &glxMajorVersion, &glxMinorVersion);
    printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);
    /* create a GLX context */
    GLWin.ctx = glXCreateContext(GLWin.dpy, vi, 0, GL_TRUE);
    /* create a color map */
    cmap = XCreateColormap(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
        vi->visual, AllocNone);
    GLWin.attr.colormap = cmap;
    GLWin.attr.border_pixel = 0;

    if (GLWin.fs)
    {
/*
Name
XF86VidModeQueryExtension, XF86VidModeQueryVersion, XF86VidModeSetClientVersion, XF86VidModeGetModeLine, XF86VidModeGetAllModeLines, XF86VidModeDeleteModeLine,
XF86VidModeModModeLine, XF86VidModeValidateModeLine, XF86VidModeSwitchMode, XF86VidModeSwitchToMode, XF86VidModeLockModeSwitch, XF86VidModeGetMonitor,
XF86VidModeGetViewPort, XF86VidModeSetViewPort, XF86VidModeGetDotClocks, XF86VidModeGetGamma, XF86VidModeSetGamma, XF86VidModeGetGammaRamp,
XF86VidModeSetGammaRamp, XF86VidModeGetGammaRampSize, XF86VidModeGetPermissions - Extension library for the XFree86-VidMode X extension
Syntax

#include <X11/extensions/xf86vmode.h>

Bool XF86VidModeQueryExtension(
    Display *display,
    int *event_base_return,
    int *error_base_return);

Bool XF86VidModeQueryVersion(
    Display *display,
    int *major_version_return,
    int *minor_version_return);

Bool XF86VidModeSetClientVersion(
    Display *display);

Bool XF86VidModeGetModeLine(
    Display *display,
    int screen,
    int *dotclock_return,
    XF86VidModeModeLine *modeline);

Bool XF86VidModeGetAllModeLines(
    Display *display,
    int screen,
    int *modecount_return,
    XF86VidModeModeInfo ***modesinfo);

Bool XF86VidModeDeleteModeLine(
    Display *display,
    int screen,
    XF86VidModeModeInfo *modeline);

Bool XF86VidModeModModeLine(
    Display *display,
    int screen,
    XF86VidModeModeLine *modeline);

Status XF86VidModeValidateModeLine(
    Display *display,
    int screen,
    XF86VidModeModeLine *modeline);

Bool XF86VidModeSwitchMode(
    Display *display,
    int screen,
    int zoom);

Bool XF86VidModeSwitchToMode(
    Display *display,
    int screen,
    XF86VidModeModeInfo *modeline);

Bool XF86VidModeLockModeSwitch(
    Display *display,
    int screen,
    int lock);

Bool XF86VidModeGetMonitor(
    Display *display,
    int screen,
    XF86VidModeMonitor *monitor);

Bool XF86VidModeGetViewPort(
    Display *display,
    int screen,
    int *x_return,
    int *y_return);

Bool XF86VidModeSetViewPort(
    Display *display,
    int screen,
    int x,
    int y);

XF86VidModeGetDotClocks(
    Display *display,
    int screen,
    int *flags return,
    int *number of clocks return,
    int *max dot clock return,
    int **clocks return);

XF86VidModeGetGamma(
    Display *display,
    int screen,
    XF86VidModeGamma *Gamma);

XF86VidModeSetGamma(
    Display *display,
    int screen,
    XF86VidModeGamma *Gamma);

XF86VidModeGetGammaRamp(
    Display *display,
    int screen,
    int size,
    unsigned short *red array,
    unsigned short *green array,
    unsigned short *blue array);

XF86VidModeSetGammaRamp(
    Display *display,
    int screen,
    int size,
    unsigned short *red array,
    unsigned short *green array,
    unsigned short *blue array);

XF86VidModeGetGammaRampSize(
    Display *display,
    int screen,
    int *size);

Arguments

display
    Specifies the connection to the X server.
screen
    Specifies which screen number the setting apply to.
event_base_return
    Returns the base event number for the extension.
error_base_return
    Returns the base error number for the extension.
major_version_return
    Returns the major version number of the extension.
minor_version_return
    Returns the minor version number of the extension.
dotclock_return
    Returns the clock for the mode line.
modecount_return
    Returns the number of video modes available in the server.
zoom
    If greater than zero, indicates that the server should switch to the next mode, otherwise switch to the previous mode.
lock
    Indicates that mode switching should be locked, if non-zero.
modeline
    Specifies or returns the timing values for a video mode.
modesinfo
    Returns the timing values and dotclocks for all of the available video modes.
monitor
    Returns information about the monitor.
x
    Specifies the desired X location for the viewport.
x_return
    Returns the current X location of the viewport.
y
    Specifies the desired Y location for the viewport.
y_return
    Returns the current Y location of the viewport.
*/

// Structures

// Video Mode Settings:
// typedef struct {
//    unsigned short    hdisplay;    	/* Number of display pixels horizontally */
//    unsigned short    hsyncstart;    	/* Horizontal sync start */
//    unsigned short    hsyncend;    	/* Horizontal sync end */
//    unsigned short    htotal;    		/* Total horizontal pixels */
//    unsigned short    vdisplay;    	/* Number of display pixels vertically */
//    unsigned short    vsyncstart;    	/* Vertical sync start */
//    unsigned short    vsyncend;    	/* Vertical sync start */
//    unsigned short    vtotal;    		/* Total vertical pixels */
//    unsigned int    	flags;    		/* Mode flags */
//    int    			privsize;    	/* Size of private */
//    INT32    			*private;    	/* Server privates */
// } XF86VidModeModeLine;

// typedef struct {
//    unsigned int    	dotclock;    	/* Pixel clock */
//    unsigned short    hdisplay;    	/* Number of display pixels horizontally */
//    unsigned short    hsyncstart;    	/* Horizontal sync start */
//    unsigned short    hsyncend;    	/* Horizontal sync end */
//    unsigned short    htotal;    		/* Total horizontal pixels */
//    unsigned short    vdisplay;    	/* Number of display pixels vertically */
//    unsigned short    vsyncstart;    	/* Vertical sync start */
//    unsigned short    vsyncend;    	/* Vertical sync start */
//    unsigned short    vtotal;    		/* Total vertical pixels */
//    unsigned int    	flags;    		/* Mode flags */
//    int    			privsize;    	/* Size of private */
//    INT32    			*private;    	/* Server privates */
// } XF86VidModeModeInfo;

// Monitor information:

// typedef struct {
//    char*    					vendor;   /* Name of manufacturer */
//    char*    					model;    /* Model name */
//    float    					EMPTY;    /* unused, for backward compatibility */
//    unsigned char    			nhsync;   /* Number of horiz sync ranges */
//    XF86VidModeSyncRange*    	hsync;    /* Horizontal sync ranges */
//    unsigned char    			nvsync;   /* Number of vert sync ranges */
//    XF86VidModeSyncRange*    	vsync;    /* Vertical sync ranges */
// } XF86VidModeMonitor;

// typedef struct {
//    float    		hi;    		/* Top of range */
//    float    		lo;    		/* Bottom of range */
// } XF86VidModeSyncRange;

// typedef struct {
//    int 			type;           /* of event */
//    unsigned long serial;    		/* # of last request processed by server */
//    Bool 			send_event;     /* true if this came from a SendEvent req */
//    Display 		*display;       /* Display the event was read from */
//    Window 		root;        	/* root window of event screen */
//    int 			state;          /* What happened */
//    int 			kind;           /* What happened */
//    Bool 			forced;        	/* extents of new region */
//    Time 			time;           /* event timestamp */
// } XF86VidModeNotifyEvent;

// typedef struct {
//    float red;            /* Red Gamma value */
//    float green;        	/* Green Gamma value */
//    float blue;           /* Blue Gamma value */
// } XF86VidModeGamma;
/*
Description

These functions provide an interface to the server extension XFree86-VidModeExtension which allows the video modes to be queried and adjusted dynamically
and mode switching to be controlled. Applications that use these functions must be linked with -lXxf86vm

Modeline Functions

The XF86VidModeGetModeLine function is used to query the settings for the currently selected video mode. The calling program should pass a pointer to a
XF86VidModeModeLine structure that it has already allocated. The function fills in the fields of the structure.

If there are any server private values (currently only applicable to the S3 server) the function will allocate storage for them. Therefore, if the privsize
field is non-zero, the calling program should call Xfree(private) to free the storage.

XF86VidModeGetAllModeLines returns the settings for all video modes. The calling program supplies the address of a pointer which will be set by the function
to point to an array of XF86VidModeModeInfo structures. The memory occupied by the array is dynamically allocated by the XF86VidModeGetAllModeLines function
and should be freed by the caller. The first element of the array corresponds to the current video mode.

The XF86VidModeModModeLine function can be used to change the settings of the current video mode provided the requested settings are valid (e.g. they don't
exceed the capabilities of the monitor).

Modes can be deleted with the XF86VidModeDeleteModeLine function. The specified mode must match an existing mode. To be considered a match, all of the fields
of the given XF86VidModeModeInfo structure must match, except the privsize and private fields. If the mode to be deleted is the current mode, a mode switch
to the next mode will occur first. The last remaining mode can not be deleted.

The validity of a mode can be checked with the XF86VidModeValidateModeLine function. If the specified mode can be used by the server (i.e. meets all the
constraints placed upon a mode by the combination of the server, card, and monitor) the function returns MODE_OK, otherwise it returns a value indicating
the reason why the mode is invalid (as defined in xf86.h).

Mode Switch Functions

When the function XF86VidModeSwitchMode is called, the server will change the video mode to next (or previous) video mode. The XF86VidModeSwitchToMode function
can be used to switch directly to the specified mode. Matching is as specified in the description of the XF86VidModeAddModeLine function above.
The XF86VidModeLockModeSwitch function can be used to allow or disallow mode switching whether the request to switch modes comes from a call to the
XF86VidModeSwitchMode or XF86VidModeSwitchToMode functions or from one of the mode switch key sequences.

Note: Because of the asynchronous nature of the X protocol, a call to XFlush is needed if the application wants to see the mode change immediately.
To be informed of the execution status of the request, a custom error handler should be installed using XSetErrorHandler before calling the mode switching function.

Monitor Functions

Information known to the server about the monitor is returned by the XF86VidModeGetMonitor function. The hsync and vsync fields each point to an array of
XF86VidModeSyncRange structures. The arrays contain nhsync and nvsync elements, respectively. The hi and low values will be equal if a discreate value was
given in the XF86Config file.

The vendor, model, hsync, and vsync fields point to dynamically allocated storage that should be freed by the caller.

Viewport Functions

The XF86VidModeGetViewPort and XF86VidModeSetViewPort functions can be used to, respectively, query and change the location of the upper left corner of the
viewport into the virtual screen.

Other Functions

The XF86VidModeQueryVersion function can be used to determine the version of the extension built into the server.
The function XF86VidModeQueryExtension returns the lowest numbered error and event values assigned to the extension.

Bugs
The XF86VidModeSetClientVersion, XF86VidModeGetDotClocks, XF86VidModeGetGamma, XF86VidModeSetGamma, XF86VidModeSetGammaRamp, XF86VidModeGetGammaRamp,
XF86VidModeGetGammaRampSize, and XF86VidModeGetPermissions functions need to be documented. In the meantime, check the source code for information about
how to use them.
See Also
xorg(1), xorg.conf(5), xflush(3), xseterrorhandler(3), xvidtune(1)
*/
        XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, modes[bestMode]);
        XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
        dpyWidth = modes[bestMode]->hdisplay;
        dpyHeight = modes[bestMode]->vdisplay;
        printf("Resolution %dx%d\n", dpyWidth, dpyHeight);
        XFree(modes);

        /* create a fullscreen window */
        GLWin.attr.override_redirect = True;
        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
            KeyReleaseMask | StructureNotifyMask;
/*

Window XCreateWindow(display, parent, x, y, width, height, border_width, depth,
                       class, visual, valuemask, attributes)
      Display *display;
      Window parent;
      int x, y;
      unsigned int width, height;
      unsigned int border_width;
      int depth;
      unsigned int class;
      Visual *visual
      unsigned long valuemask;
      XSetWindowAttributes *attributes;


Window XCreateSimpleWindow(display, parent, x, y, width, height, border_width,
                             border, background)
      Display *display;
      Window parent;
      int x, y;
      unsigned int width, height;
      unsigned int border_width;
      unsigned long border;
      unsigned long background;

ARGUMENTS
attributes 		Specifies the structure from which the values (as specified by the value mask) are to be taken. The value mask should have the appropriate bits set to indicate which attributes have been set in the structure.
background 		Specifies the background pixel value of the window.
border 			Specifies the border pixel value of the window.
border_width 	Specifies the width of the created window's border in pixels.
class 			Specifies the created window's class. You can pass InputOutput, InputOnly, or CopyFromParent. A class of CopyFromParent means the class is taken from the parent.
depth 			Specifies the window's depth. A depth of CopyFromParent means the depth is taken from the parent.
display 		Specifies the connection to the X server.
parent 			Specifies the parent window.
valuemask 		Specifies which window attributes are defined in the attributes argument. This mask is the bitwise inclusive OR of the valid attribute mask bits. If valuemask is zero, the attributes are ignored and are not referenced.
visual 			Specifies the visual type. A visual of CopyFromParent means the visual type is taken from the parent.
width
height 			Specify the width and height, which are the created window's inside dimensions and do not include the created window's borders
x
y 	Specify the x and y coordinates, which are the top-left outside corner of the window's borders and are relative to the inside of the parent window's borders.
DESCRIPTION
The XCreateWindow function creates an unmapped subwindow for a specified parent window, returns the window ID of the created window, and causes the X server to generate a CreateNotify event. The created window is placed on top in the stacking order with respect to siblings.

The coordinate system has the X axis horizontal and the Y axis vertical with the origin [0, 0] at the upper-left corner. Coordinates are integral, in terms of pixels, and coincide with pixel centers. Each window and pixmap has its own coordinate system. For a window, the origin is inside the border at the inside, upper-left corner.

The border_width for an InputOnly window must be zero, or a BadMatch error results. For class InputOutput, the visual type and depth must be a combination supported for the screen, or a BadMatch error results. The depth need not be the same as the parent, but the parent must not be a window of class InputOnly, or a BadMatch error results. For an InputOnly window, the depth must be zero, and the visual must be one supported by the screen. If either condition is not met, a BadMatch error results. The parent window, however, may have any depth and class. If you specify any invalid window attribute for a window, a BadMatch error results.

The created window is not yet displayed (mapped) on the user's display. To display the window, call XMapWindow(). The new window initially uses the same cursor as its parent. A new cursor can be defined for the new window by calling XDefineCursor(). The window will not be visible on the screen unless it and all of its ancestors are mapped and it is not obscured by any of its ancestors.

XCreateWindow can generate BadAlloc BadColor, BadCursor, BadMatch, BadPixmap, BadValue, and BadWindow errors.

The XCreateSimpleWindow function creates an unmapped InputOutput subwindow for a specified parent window, returns the window ID of the created window, and causes the X server to generate a CreateNotify event. The created window is placed on top in the stacking order with respect to siblings. Any part of the window that extends outside its parent window is clipped. The border_width for an InputOnly window must be zero, or a BadMatch error results. XCreateSimpleWindow inherits its depth, class, and visual from its parent. All other window attributes, except background and border, have their default values.

XCreateSimpleWindow can generate BadAlloc, BadMatch, BadValue, and BadWindow errors.
*/
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
            0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
            CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
            &GLWin.attr);
/*
XWarpPointer(display, src_w, dest_w, src_x, src_y, src_width, src_height, dest_x,
                dest_y)
        Display *display;
        Window src_w, dest_w;
        int src_x, src_y;
        unsigned int src_width, src_height;
        int dest_x, dest_y;

Arguments
display 	Specifies the connection to the X server.
src_w 	    Specifies the source window or None.
dest_w 	    Specifies the destination window or None.
src_x
src_y
src_width
src_height 	Specify a rectangle in the source window.
dest_x
dest_y 	    Specify the x and y coordinates within the destination window.

Description
If dest_w is None, XWarpPointer() moves the pointer by the offsets (dest_x, dest_y) relative to the
current position of the pointer. If dest_w is a window, XWarpPointer() moves the pointer to the
offsets (dest_x, dest_y) relative to the origin of dest_w. However, if src_w is a window, the move
only takes place if the window src_w contains the pointer and if the specified rectangle of src_w
contains the pointer.

The src_x and src_y coordinates are relative to the origin of src_w. If src_height is zero, it is
replaced with the current height of src_w minus src_y. If src_width is zero, it is replaced with the
current width of src_w minus src_x.

There is seldom any reason for calling this function. The pointer should normally be left to the user.
If you do use this function, however, it generates events just as if the user had instantaneously moved
the pointer from one position to another. Note that you cannot use XWarpPointer() to move the pointer
outside the confine_to window of an active pointer grab. An attempt to do so will only move the pointer
as far as the closest edge of the confine_to window.

XWarpPointer() can generate a BadWindow error.
*/

        XWarpPointer(GLWin.dpy, None, GLWin.win, 0, 0, 0, 0, 0, 0);
/*
XMapRaised(display, w)
      Display *display;
      Window w;

Arguments
display 	Specifies the connection to the X server.
w 	Specifies the window.
Description
The XMapRaised() function essentially is similar to XMapWindow() in that it maps the window and all of its subwindows that have had map requests. However, it also raises the specified window to the top of the stack. For additional information, see XMapWindow().

XMapRaised() can generate multiple BadWindow errors.
*/
		XMapRaised(GLWin.dpy, GLWin.win);
/*
grab : Confine pointer and keyboard events to a window sub-tree

int XGrabKeyboard(display, grab_window, owner_events, pointer_mode, keyboard_mode, time)
      Display *display;
      Window grab_window;
      Bool owner_events;
      int pointer_mode, keyboard_mode;
      Time time;

Arguments
display 		Specifies the connection to the X server.
grab_window 	Specifies the grab window.
owner_events 	Specifies a Boolean value that indicates whether the keyboard events are to be reported as usual.
pointer_mode 	Specifies further processing of pointer events. You can pass GrabModeSync or GrabModeAsync.
keyboard_mode 	Specifies further processing of keyboard events. You can pass GrabModeSync or GrabModeAsync.
time 			Specifies the time. You can pass either a timestamp or CurrentTime.

Description
The XGrabKeyboard() function actively grabs control of the keyboard and generates FocusIn and FocusOut events. Further key events are reported only to the grabbing client. XGrabKeyboard() overrides any active keyboard grab by this client. If owner_events is False, all generated key events are reported with respect to grab_window. If owner_events is True and if a generated key event would normally be reported to this client, it is reported normally; otherwise, the event is reported with respect to the grab_window. Both KeyPress and KeyRelease events are always reported, independent of any event selection made by the client.

If the keyboard_mode argument is GrabModeAsync, keyboard event processing continues as usual. If the keyboard is currently frozen by this client, then processing of keyboard events is resumed. If the keyboard_mode argument is GrabModeSync , the state of the keyboard (as seen by client applications) appears to freeze, and the X server generates no further keyboard events until the grabbing client issues a releasing XAllowEvents() call or until the keyboard grab is released. Actual keyboard changes are not lost while the keyboard is frozen; they are simply queued in the server for later processing.

If pointer_mode is GrabModeAsync , pointer event processing is unaffected by activation of the grab. If pointer_mode is GrabModeSync , the state of the pointer (as seen by client applications) appears to freeze, and the X server generates no further pointer events until the grabbing client issues a releasing XAllowEvents() call or until the keyboard grab is released. Actual pointer changes are not lost while the pointer is frozen; they are simply queued in the server for later processing.

If the keyboard is actively grabbed by some other client, XGrabKeyboard() fails and returns AlreadyGrabbed. If grab_window is not viewable, it fails and returns GrabNotViewable. If the keyboard is frozen by an active grab of another client, it fails and returns GrabFrozen. If the specified time is earlier than the last-keyboard-grab time or later than the current X server time, it fails and returns GrabInvalidTime. Otherwise, the last-keyboard-grab time is set to the specified time (CurrentTime is replaced by the current X server time).

XGrabKeyboard() can generate BadValue and BadWindow errors.
*/
        XGrabKeyboard(GLWin.dpy, GLWin.win, True, GrabModeAsync,
            GrabModeAsync, CurrentTime);
/*
grab : Confine pointer and keyboard events to a window sub-tree

int XGrabPointer(display, grab_window, owner_events, event_mask, pointer_mode,
               keyboard_mode, confine_to, cursor, time)
      Display *display;
      Window grab_window;
      Bool owner_events;
      unsigned int event_mask;
      int pointer_mode, keyboard_mode;
      Window confine_to;
      Cursor cursor;
      Time time;

Arguments
display 		Specifies the connection to the X server.
grab_window 	Specifies the grab window.
owner_events 	Specifies a Boolean value that indicates whether the pointer events are to be reported as usual or reported with respect to the grab window if selected by the event mask.
event_mask 		Specifies which pointer events are reported to the client. The mask is the bitwise inclusive OR of the valid pointer event mask bits.
pointer_mode 	Specifies further processing of pointer events. You can pass GrabModeSync or GrabModeAsync.
keyboard_mode 	Specifies further processing of keyboard events. You can pass GrabModeSync or GrabModeAsync.
confine_to 		pecifies the window to confine the pointer in or None.
cursor 			Specifies the cursor that is to be displayed during the grab or None.
time 			Specifies the time. You can pass either a timestamp or CurrentTime.

Description
The XGrabPointer() function actively grabs control of the pointer and returns GrabSuccess if the grab was successful. Further pointer events are reported only to the grabbing client. XGrabPointer() overrides any active pointer grab by this client. If owner_events is False, all generated pointer events are reported with respect to grab_window and are reported only if selected by event_mask. If owner_events is True and if a generated pointer event would normally be reported to this client, it is reported as usual. Otherwise, the event is reported with respect to the grab_window and is reported only if selected by event_mask. For either value of owner_events, unreported events are discarded.

If the pointer_mode is GrabModeAsync, pointer event processing continues as usual. If the pointer is currently frozen by this client, the processing of events for the pointer is resumed. If the pointer_mode is GrabModeSync, the state of the pointer, as seen by client applications, appears to freeze, and the X server generates no further pointer events until the grabbing client calls XAllowEvents() or until the pointer grab is released. Actual pointer changes are not lost while the pointer is frozen; they are simply queued in the server for later processing.

If the keyboard_mode is GrabModeAsync, keyboard event processing is unaffected by activation of the grab. If the keyboard_mode is GrabModeSync, the state of the keyboard, as seen by client applications, appears to freeze, and the X server generates no further keyboard events until the grabbing client calls XAllowEvents() or until the pointer grab is released. Actual keyboard changes are not lost while the pointer is frozen; they are simply queued in the server for later processing.

If a cursor is specified, it is displayed regardless of what window the pointer is in. If None is specified, the normal cursor for that window is displayed when the pointer is in grab_window or one of its subwindows; otherwise, the cursor for grab_window is displayed.

If a confine_to window is specified, the pointer is restricted to stay contained in that window. The confine_to window need have no relationship to the grab_window. If the pointer is not initially in the confine_to window, it is warped automatically to the closest edge just before the grab activates and enter/leave events are generated as usual. If the confine_to window is subsequently reconfigured, the pointer is warped automatically, as necessary, to keep it contained in the window.

The time argument allows you to avoid certain circumstances that come up if applications take a long time to respond or if there are long network delays. Consider a situation where you have two applications, both of which normally grab the pointer when clicked on. If both applications specify the timestamp from the event, the second application may wake up faster and successfully grab the pointer before the first application. The first application then will get an indication that the other application grabbed the pointer before its request was processed.

XGrabPointer() generates EnterNotify and LeaveNotify events.

Either if grab_window or confine_to window is not viewable or if the confine_to window lies completely outside the boundaries of the root window, XGrabPointer() fails and returns GrabNotViewable. If the pointer is actively grabbed by some other client, it fails and returns AlreadyGrabbed. If the pointer is frozen by an active grab of another client, it fails and returns GrabFrozen. If the specified time is earlier than the last-pointer-grab time or later than the current X server time, it fails and returns GrabInvalidTime. Otherwise, the last-pointer-grab time is set to the specified time (CurrentTime is replaced by the current X server time).

XGrabPointer() can generate BadCursor, BadValue, and BadWindow errors.
*/
        XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask,
            GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
    }
    else
    {
        /* create a window in window mode*/
        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
            KeyReleaseMask | StructureNotifyMask;
/*
RootWindow

RootWindow(display, screen_number)

Window XRootWindow(display, screen_number)
      Display *display;
      int screen_number;

display 	Specifies the connection to the X server.
screen_number 	Specifies the appropriate screen number on the host server.

Both return the root window. These are useful with functions that need a drawable of a particular screen and for creating top-level windows.
*/
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
            0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
            CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
        /* only set window title and handle wm_delete_events if in windowed mode */
        wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
/*
Status XSetWMProtocols(display, w, protocols, count)
      Display *display;
      Window w;
      Atom *protocols;
      int count;

Arguments
display 	Specifies the connection to the X server.
w 	        Specifies the window.
protocols 	Specifies the list of protocols.
count 	    Specifies the number of protocols in the list.

Description
The XSetWMProtocols() function replaces the WM_PROTOCOLS property on the specified window with the list of atoms specified by the protocols argument. If the property does not already exist, XSetWMProtocols() sets the WM_PROTOCOLS property on the specified window to the list of atoms specified by the protocols argument. The property is stored with a type of ATOM and a format of 32. If it cannot intern the WM_PROTOCOLS atom, XSetWMProtocols() returns a zero status. Otherwise, it returns a nonzero status.

XSetWMProtocols() can generate BadAlloc and BadWindow errors.
*/
        XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
/*
XSetStandardProperties(display, w, window_name, icon_name, icon_pixmap, argv, argc, hints)
      Display *display;
      Window w;
      char *window_name;
      char *icon_name;
      Pixmap icon_pixmap;
      char **argv;
      int argc;
      XSizeHints *hints;

Arguments
display 	    Specifies the connection to the X server.
w 	            Specifies the window.
window_name 	Specifies the window name, which should be a null-terminated string.
icon_name 	    Specifies the icon name, which should be a null-terminated string.
icon_pixmap 	Specifies the bitmap that is to be used for the icon or None.
argv 	        Specifies the application's argument list.
argc 	        Specifies the number of arguments.
hints 	        Specifies a pointer to the size hints for the window in its normal state.

Description
The XSetStandardProperties() function provides a means by which simple applications set the most essential properties with a single call. XSetStandardProperties() should be used to give a window manager some information about your program's preferences. It should not be used by applications that need to communicate more information than is possible with XSetStandardProperties(). (Typically, argv is the argv array of your main program.) If the strings are not in the Host Portable Character Encoding, the result is implementation dependent.

XSetStandardProperties() can generate BadAlloc and BadWindow errors.
*/
        XSetStandardProperties(GLWin.dpy, GLWin.win, title,
            title, None, NULL, 0, NULL);
/*
XMapRaised(display, w)
      Display *display;
      Window w;

Arguments
display 	Specifies the connection to the X server.
w 	Specifies the window.
Description
The XMapRaised() function essentially is similar to XMapWindow() in that it maps the window and all of its subwindows
that have had map requests. However, it also raises the specified window to the top of the stack.
For additional information, see XMapWindow().

XMapRaised() can generate multiple BadWindow errors.
*/
        XMapRaised(GLWin.dpy, GLWin.win);
    }
    /* connect the glx-context to the window */
    glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);

    XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
        &GLWin.width, &GLWin.height, &borderDummy, &GLWin.depth);
    printf("Depth %d\n", GLWin.depth);
    if (glXIsDirect(GLWin.dpy, GLWin.ctx))
        printf("Congrats, you have Direct Rendering!\n");
    else
        printf("Sorry, no Direct Rendering possible!\n");
    initGL();
    return True;
}

/**	            This function build keycodes map of keys used +by this program
*
* \brief      Fonction InitKeys : fonction construisant le tableau des "keycodes" utilisés par ce programme
* \details    Aucun paramètre dans ce cas de figure car tout est géré directement à l'intérieur de cette fonction.
*
*/

void initKeys()
{
    printf("Initializing keys...\n");
    /* get keycode for escape-key */
    keyCodes[0] = XKeysymToKeycode(GLWin.dpy, XK_Escape);
    /* get keycode for F1 */
    keyCodes[1] = XKeysymToKeycode(GLWin.dpy, XK_F1);
    /* get keycode for "keypad 8" */
    keyCodes[2] = XKeysymToKeycode(GLWin.dpy, XK_KP_8);
    /* get keycode for "keypad 2" */
    keyCodes[3] = XKeysymToKeycode(GLWin.dpy, XK_KP_2);
    /* get keycode for "keypad 6"*/
    keyCodes[4] = XKeysymToKeycode(GLWin.dpy, XK_KP_6);
    /* get keycode for "keypad 4" */
    keyCodes[5] = XKeysymToKeycode(GLWin.dpy, XK_KP_4);
    /* get keycode for TAB */
    keyCodes[6] = XKeysymToKeycode(GLWin.dpy, XK_Tab);
    /* get keycode for "keypad add" */
    keyCodes[7] = XKeysymToKeycode(GLWin.dpy, XK_KP_Add);
    /* get keycode for "keypad substract" */
    keyCodes[8] = XKeysymToKeycode(GLWin.dpy, XK_KP_Subtract);
    /* get keycode for page up */
    keyCodes[9] = XKeysymToKeycode(GLWin.dpy, XK_Page_Up);
    /* get keycode for page down */
    keyCodes[10] = XKeysymToKeycode(GLWin.dpy, XK_Page_Down);
    /* get keycode for "return" */
    keyCodes[11] = XKeysymToKeycode(GLWin.dpy, XK_Return);
    /* get keycode for "space" */
    keyCodes[12] = XKeysymToKeycode(GLWin.dpy, XK_space);
    /* get keycode for arrow up */
    keyCodes[13] = XKeysymToKeycode(GLWin.dpy, XK_Up);
    /* get keycode for arrow down */
    keyCodes[14] = XKeysymToKeycode(GLWin.dpy, XK_Down);
    /* get keycode for arrow right */
    keyCodes[15] = XKeysymToKeycode(GLWin.dpy, XK_Right);
    /* get keycode for arrow left */
    keyCodes[16] = XKeysymToKeycode(GLWin.dpy, XK_Left);
}

/**	            This function run actions when key pressed
*
* \brief      Fonction keyAction : fonction déclenchant les actions attendues lors de l'appui sur certaines touches du clavier
* \details    Aucun paramètre dans ce cas de figure car tout est géré directement à l'intérieur de cette fonction.
*
*/

void keyAction()
{
    if (keys[keyCodes[0]])
        done = True;
    if (keys[keyCodes[1]])
    {
        killGLWindow();
        GLWin.fs = !GLWin.fs;
        createGLWindow("NeHe's Particle Engine - Lesson 19", 640, 480, 24, GLWin.fs);
        keys[keyCodes[1]] = False;
    }
    if (keys[keyCodes[7]] && slowdown > 1.0f)
        slowdown -= 0.01f;
    if (keys[keyCodes[8]] && slowdown < 4.0f)
        slowdown += 0.01f;
    if (keys[keyCodes[9]])
        zoom += 0.1f;
    if (keys[keyCodes[10]])
        zoom -= 0.1f;
    if (keys[keyCodes[11]])
    {
        rainbow = !rainbow;
        keys[keyCodes[11]] = False;
    }
    if ((keys[keyCodes[12]]) || (rainbow && delay > 25))
    {
        if (keys[keyCodes[12]])
            rainbow = False;
        delay = 0;
        color++;
        if (color > 11)
            color = 0;
        keys[keyCodes[12]] = False;
    }
    if ((keys[keyCodes[13]]) && (ySpeed < 200.0f))
        ySpeed += 1.0f;
    if ((keys[keyCodes[14]]) && (ySpeed > -200.0f))
        ySpeed -= 1.0f;
    if ((keys[keyCodes[15]]) && (xSpeed < 200.0f))
        xSpeed += 1.0f;
    if ((keys[keyCodes[16]]) && (xSpeed > -200.0f))
        xSpeed -= 1.0f;
    delay++;
}

/* Main function : GLX run as a console application starting with program call main()  */

/**         Comments manageable by Doxygen
*
* \brief      Programme Main obligatoire pour les programmes sous Linux (OpenGL en mode console).
* \details    Programme principal de lancement de l'application qui appelle ensuite toutes les fonctions utiles OpenGL et surtout glut.
* \param      argc         le nombre de paramètres de la ligne de commande.
* \param      argv         un pointeur sur le tableau des différents paramètres de la ligne de commande.
* \return     int          un entier permettant de connaître la statut du lancement du programme.
*
*/

int main(int argc, char **argv)
{
    XEvent event;

    done = False;
    rainbow = True;
    /* default to fullscreen _> GLWin.fs = True   -> changed by Thierry DECHAIZE, window mode privilegied   */
    GLWin.fs = False;

   if (getenv("LEVEL")) {                 // LEVEL is set
       level_debug=getenv("LEVEL");           // Added by Thierry DECHAIZE : récupérer la valeur de la variable d'environnement LEVEL si elle existe
       }
    else {                                // if LEVEL not set, same with Debug mode, nothing log is generated (file log.txt in current directory)
       level_debug = '\0';
    }

#ifdef DEBUG
    printf("Niveau de trace : %s.\n",level_debug);

    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Enter within main, before call of createGLWindow.");
#endif // defined DEBUG

    createGLWindow("NeHe's Particle Engine - Lesson 19", 640, 480, 24, GLWin.fs);
    initKeys();
    /* wait for events*/
    while (!done)
    {
        /* handle the events in the queue */
        while (XPending(GLWin.dpy) > 0)
        {
            XNextEvent(GLWin.dpy, &event);
            switch (event.type)
            {
                case Expose:
	                if (event.xexpose.count != 0)
	                    break;
                    drawGLScene();
                    break;
                case ConfigureNotify:
                /* call resizeGLScene only if our window-size changed */
                    if ((event.xconfigure.width != GLWin.width) ||
                        (event.xconfigure.height != GLWin.height))
                    {
                        GLWin.width = event.xconfigure.width;
                        GLWin.height = event.xconfigure.height;
                        printf("Resize event\n");
                        resizeGLScene(event.xconfigure.width,
                            event.xconfigure.height);
                    }
                    break;
                /* exit in case of a mouse button press */
                case ButtonPress:
                    done = True;
                    break;
                case KeyPress:
                    keys[event.xkey.keycode] = True;
                    break;
                case KeyRelease:
                    keys[event.xkey.keycode] = False;
                    break;
                case ClientMessage:
                    if (*XGetAtomName(GLWin.dpy, event.xclient.message_type) ==
                        *"WM_PROTOCOLS")
                    {
                        printf("Exiting sanely...\n");
                        done = True;
                    }
                    break;
                default:
                    break;
            }
        }

#ifdef DEBUG
    printf("Niveau de trace : %s.\n",level_debug);

    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step in main, in the loop event, before call of keyAction.");
#endif // defined DEBUG

        keyAction();

#ifdef DEBUG
    printf("Niveau de trace : %s.\n",level_debug);

    if (strcmp(level_debug,"BASE") == 0 || strcmp(level_debug,"FULL") == 0)
        log_print(__FILE__, __LINE__,"Next step in main, in the lopp event, before call of drawGLScene.");
#endif // defined DEBUG

        drawGLScene();
    }
    killGLWindow();


    return 0;
}
