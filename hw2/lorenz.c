/*
 * Assignment 2
 * Simple program to demonstrate generating coordinates
 * using the Lorenz Attractor
 *
 * Jianxiang Fan
 * jianxiang.fan@colorado.edu
 */

#include <stdarg.h>
#include <stdio.h>

#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
#include <GLUT/glut.h>
#else

#include <GL/glut.h>

#endif

#define MENU_S 0
#define MENU_B 1
#define MENU_R 2

double rotAngle = 10;

/*  Lorenz Parameters  */
double s = 10;
double b = 2.6666;
double r = 28;

int menu_pos = MENU_S;

int u = 0;

#define LEN 8192  //  Maximum length of text string


static void
showText(const char *format, ...)
{
    char buf[LEN];
    char *ch = buf;
    va_list args;
    //  Turn the parameters into a character string
    va_start(args, format);
    vsnprintf(buf, LEN, format, args);
    va_end(args);
    //  Display the characters one at a time at the current raster position
    while (*ch)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

void drawLorenz()
{
    glPushMatrix();
    glBegin(GL_LINE_STRIP);

    int i;
    /*  Coordinates  */
    double x = 1;
    double y = 1;
    double z = 1;
    /*  Time step  */
    double dt = 0.001;

    /*
     *  Integrate 50,000 steps (50 time units with dt = 0.001)
     *  Explicit Euler integration
     */
    int n = 50000;
    for (i = 0; i < n; i++) {
        double dx = s * (y - x);
        double dy = x * (r - z) - y;
        double dz = x * y - b * z;
        x += dt * dx;
        y += dt * dy;
        z += dt * dz;
        glColor3d(1.0 - (double) i / n, 0.0 + (double) i / n, 0.0);
        glVertex3d(x, y, z);
    }
    glEnd();
    glPopMatrix();
}

void drawAxis(double length, char label)
{
    glPushMatrix();
    glColor3f(0.7f, 0.7f, 0.7f);

    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, length);
    glEnd();

    glTranslated(0.0, 0.0, length - 0.2);
    glutWireCone(1.2, 8, 8, 8);

    glColor3f(0.9f, 0.9f, 0.9f);
    glRasterPos3d(0.0, 5.0, 0.0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, label);

    glPopMatrix();
}

void drawCoordinateSystem(double length)
{
    glPushMatrix();
    drawAxis(length, 'z');

    glRotated(90.0, 0, 1.0, 0);
    drawAxis(length, 'x');
    glPopMatrix();

    glPushMatrix();
    glRotated(-90.0, 1.0, 0.0, 0.0);
    drawAxis(length, 'y');
    glPopMatrix();
}

void showParameters()
{
    glColor3f(1.0f, (menu_pos == MENU_S) ? 0.0f : 1.0f, (menu_pos == MENU_S) ? 0.0f : 1.0f);
    glWindowPos2i(5, 45);
    showText("s=%.3f", s);
    glColor3f(1.0f, (menu_pos == MENU_B) ? 0.0f : 1.0f, (menu_pos == MENU_B) ? 0.0f : 1.0f);
    glWindowPos2i(5, 25);
    showText("b=%.3f", b);
    glColor3f(1.0f, (menu_pos == MENU_R) ? 0.0f : 1.0f, (menu_pos == MENU_R) ? 0.0f : 1.0f);
    glWindowPos2i(5, 5);
    showText("r=%.3f", r);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT |     // clear the frame buffer (color)
            GL_DEPTH_BUFFER_BIT);     // clear the depth buffer (depths)
    glLoadIdentity();

    gluLookAt(                    // set up the camera
            100.0 + u, 150.0 + u, 100.0 + u,  // eye position
            0.0, 0.0, 0.0,        // lookat position
            0.0, 1.0, 0.0);       // up direction

    glPushMatrix();
    glRotated(rotAngle, 0.0, 1.0, 0.0);    // rotate by rotAngle about y-axis
    drawCoordinateSystem(80);
    drawLorenz();
    showParameters();
    glPopMatrix();

    glFlush();                    // force OpenGL to render now
    glutSwapBuffers();            // make the image visible
}

void special(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT) {
        if (menu_pos == MENU_R) {
            r += 0.5;
        } else if (menu_pos == MENU_S) {
            s += 0.5;
        } else if (menu_pos == MENU_B) {
            b += 0.05;
        }
    }
    else if (key == GLUT_KEY_LEFT) {
        if (menu_pos == MENU_R) {
            r -= 0.5;
        } else if (menu_pos == MENU_S) {
            s -= 0.5;
        } else if (menu_pos == MENU_B) {
            b -= 0.05;
        }
    } else if (key == GLUT_KEY_UP) {
        menu_pos = (menu_pos + 2) % 3;
    } else if (key == GLUT_KEY_DOWN) {
        menu_pos = (menu_pos + 1) % 3;
    }
    glutPostRedisplay();
}

void keyborad(unsigned char key, int x, int y)
{
    if (key == '-') {
        u += 5;
    } else if (key == '=') {
        u -= 5;
    } else if (key == '[') {
        rotAngle -= 2;
    } else if (key == ']') {
        rotAngle += 2;
    }
    glutPostRedisplay();
}

/*
 * This function is called by GLUT when the window is resized
 */
void reshape(int width, int height)
{
    //  Calculate width to height ratio
    double w2h = (height > 0) ? (double) width / height : 1;
    //  Set viewport as entire window
    glViewport(0, 0, width, height);
    //  Select projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Set projection to identity
    glLoadIdentity();
    //  Orthogonal projection:  unit cube adjusted for aspect ratio
    gluPerspective(60, w2h, 1, 10000);    // setup a perspective projection
    //  Select model view matrix
    glMatrixMode(GL_MODELVIEW);
    //  Set model view to identity
    glLoadIdentity();
}

void init()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);      // enable hidden surface removal

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 10000);    // setup a perspective projection

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(                    // set up the camera
            100.0, 150.0, 100.0,  // eye position
            0.0, 0.0, 0.0,        // lookat position
            0.0, 1.0, 0.0);       // up direction
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(        // initialize GLUT
            GLUT_DOUBLE |       // use double buffering
            GLUT_DEPTH |        // request memory for z-buffer
            GLUT_RGB);          // set RGB color mode

    glutInitWindowSize(640, 640);

    glutCreateWindow("Assignment 2: Jianxiang Fan");    // create the window

    glutDisplayFunc(display);        // call display() to redraw window
    glutSpecialFunc(special);        // call special() when special key is hit
    glutKeyboardFunc(keyborad);      // call keyboard() when key is hit
    glutReshapeFunc(reshape);

    init();                    // initializations

    glutMainLoop();            // let GLUT take care of everything
    return 0;
}
