/*
 *  HW4: Projection
 *
 *  Jianxiang Fan
 *  jianxiang.fan@colorado.edu
 *
 *  Key bindings
 *    a          Toggle axes
 *    m          Change mode
 *    ESC        Exit
 *
 *  Orthogonal Mode
 *    arrows     Change view angle
 *
 *  Perspective Mode
 *    arrows     Change view angle
 *    +/-        Changes field of view
 *
 *  First Person Mode
 *    up/down      Move forwards/backwards
 *    left/right   Turn left/right
 *    PgUp/PgDn    Look up/down (less than 90 degrees)
 *    +/-          Changes field of view
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else

#include <GL/glut.h>

#endif

int axes = 1;       //  Display axes
int mode = 0;       //  Perspective/Orthogonal/First Person mode
int th = 0;         //  Azimuth of view angle
int ph = 0;         //  Elevation of view angle
int fov = 55;       //  Field of view (for perspective)
double asp = 1;     //  Aspect ratio
double dim = 20.0;   //  Size of world

double fp_ex = 0.0;
double fp_ey = 0.0;
double fp_ez = 30.0;
int fp_th = 0; // Azimuth of view angle for first person
int fp_ph = 0; // Elevation of view angle for first person

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string

void print(const char *format, ...)
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

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x, double y, double z,
                 double dx, double dy, double dz,
                 double th)
{
    //  Save transformation
    glPushMatrix();
    //  Offset
    glTranslated(x, y, z);
    glRotated(th, 0, 1, 0);
    glScaled(dx, dy, dz);
    //  Cube
    glBegin(GL_QUADS);
    //  Front
    glColor3f(1, 0, 0);
    glVertex3f(-1, -1, 1);
    glVertex3f(+1, -1, 1);
    glVertex3f(+1, +1, 1);
    glVertex3f(-1, +1, 1);
    //  Back
    glColor3f(0, 0, 1);
    glVertex3f(+1, -1, -1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, +1, -1);
    glVertex3f(+1, +1, -1);
    //  Right
    glColor3f(1, 1, 0);
    glVertex3f(+1, -1, +1);
    glVertex3f(+1, -1, -1);
    glVertex3f(+1, +1, -1);
    glVertex3f(+1, +1, +1);
    //  Left
    glColor3f(0, 1, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, +1);
    glVertex3f(-1, +1, +1);
    glVertex3f(-1, +1, -1);
    //  Top
    glColor3f(0, 1, 1);
    glVertex3f(-1, +1, +1);
    glVertex3f(+1, +1, +1);
    glVertex3f(+1, +1, -1);
    glVertex3f(-1, +1, -1);
    //  Bottom
    glColor3f(1, 0, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(+1, -1, -1);
    glVertex3f(+1, -1, +1);
    glVertex3f(-1, -1, +1);
    //  End
    glEnd();
    //  Undo transformations
    glPopMatrix();
}

/*
 *  Draw vertex in polar coordinates
 */
static void vertex(double th, double ph)
{
    glColor3f(Cos(th) * Cos(th), Sin(ph) * Sin(ph), Sin(th) * Sin(th));
    glVertex3d(Sin(th) * Cos(ph), Sin(ph), Cos(th) * Cos(ph));
}

/*
 *  Draw a sphere
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere(double x, double y, double z, double r)
{
    const int d = 5;
    int th, ph;

    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x, y, z);
    glScaled(r, r, r);

    //  South pole cap
    glBegin(GL_TRIANGLE_FAN);
    vertex(0, -90);
    for (th = 0; th <= 360; th += d) {
        vertex(th, d - 90);
    }
    glEnd();

    //  Latitude bands
    for (ph = d - 90; ph <= 90 - 2 * d; ph += d) {
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d) {
            vertex(th, ph);
            vertex(th, ph + d);
        }
        glEnd();
    }

    //  North pole cap
    glBegin(GL_TRIANGLE_FAN);
    vertex(0, 90);
    for (th = 0; th <= 360; th += d) {
        vertex(th, 90 - d);
    }
    glEnd();

    //  Undo transformations
    glPopMatrix();
}


/*
 *  Draw a semisphere
 *     at (x,y,z)
 *     radius (r)
 */
static void semisphere(double x, double y, double z, double r)
{
    const int d = 5;
    int th, ph;

    //  Save transformation
    glPushMatrix();

    //  Offset and scale
    glTranslated(x, y, z);
    glScaled(r, r, r);

    //  North pole cap
    glBegin(GL_TRIANGLE_FAN);
    vertex(0, 90);
    for (th = 0; th <= 360; th += d) {
        vertex(th, 90 - d);
    }
    glEnd();

    //  Latitude bands
    for (ph = 0; ph <= 90 - 2 * d; ph += d) {
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d) {
            vertex(th, ph);
            vertex(th, ph + d);
        }
        glEnd();
    }

    //  Undo transformations
    glPopMatrix();
}

/*
 *  Draw an UFO
 *     at (x,y,z)
 *     scale (r)
 *     rotated theta about the y axis
 *     rotated psi about the z axis
 *     rotated phi about the z axis
 */
static void ufo(double x, double y, double z, double r, double phi, double theta, double psi)
{
    const int d = 5;
    const int a = 15;
    int th;

    //  Save transformation
    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(phi, 1, 0, 0);
    glRotated(theta, 0, 1, 0);
    glRotated(psi, 0, 0, 1);
    glScaled(r, r, r);

    semisphere(0, 0, 0, 0.1);

    //  Save transformation
    glPushMatrix();

    //  Translate to position
    glTranslated(0, -Sin(90 - a), 0);

    //  North side
    glBegin(GL_TRIANGLE_FAN);
    vertex(0, 90);
    for (th = 0; th <= 360; th += d) {
        vertex(th, 90 - a);
    }
    glEnd();

    glPopMatrix();

    //  Save transformation
    glPushMatrix();

    //  Translate to position
    glTranslated(0, Sin(90 - a), 0);

    //  South side
    glBegin(GL_TRIANGLE_FAN);
    vertex(0, -90);
    for (th = 0; th <= 360; th += d) {
        vertex(th, a - 90);
    }
    glEnd();

    glPopMatrix();

    glPopMatrix();
}

/*
 *  Draw a robot
 *     at (x,y,z)
 *     scale (s)
 *     rotated theta about the y axis
 *     rotated psi about the z axis
 *     rotated phi about the z axis
 */
static void robot(double x, double y, double z, double s, double phi, double theta, double psi)
{
    //  Save transformation
    glPushMatrix();

    //  Offset and scale
    glTranslated(x, y, z);
    glRotated(phi, 1, 0, 0);
    glRotated(theta, 0, 1, 0);
    glRotated(psi, 0, 0, 1);
    glScaled(s, s, s);

    // Head
    sphere(0.0, 4.0, 0, 1.5);
    // Body
    cube(0, 0, 0, 2.5, 2.5, 2, 0);
    // Shoulders
    semisphere(3.5, 1.5, 0, 1);
    semisphere(-3.5, 1.5, 0, 1);
    // Arms
    cube(3.5, 0, 0, 0.5, 1.5, 0.5, 0);
    cube(-3.5, 0, 0, 0.5, 1.5, 0.5, 0);
    // Hands
    sphere(3.5, -2.5, 0, 1);
    sphere(-3.5, -2.5, 0, 1);
    // Legs
    cube(1, -4.5, 0, 0.6, 2.0, 1, 0);
    cube(-1, -4.5, 0, 0.6, 2.0, 1, 0);
    // Feet
    cube(1, -7, 0, 0.7, 0.5, 1.5, 0);
    cube(-1, -7, 0, 0.7, 0.5, 1.5, 0);

    glPopMatrix();
}

/*
 *  Set projection
 */
static void project()
{
    //  Switch to manipulating the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    if (mode == 1 || mode == 2)
        //  Perspective transformation
        gluPerspective(fov, asp, dim / 4, 10 * dim);
    else
        //  Orthogonal projection
        glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
    const double len = 15;  //  Length of axes
    //  Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //  Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);
    //  Undo previous transformations
    glLoadIdentity();
    if (mode == 1) {
        //  Perspective - set eye position
        glWindowPos2i(5, 25);
        print("Perspective");
        double Ex = -2 * dim * Sin(th) * Cos(ph);
        double Ey = +2 * dim * Sin(ph);
        double Ez = +2 * dim * Cos(th) * Cos(ph);
        gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
        glWindowPos2i(5, 5);
        print("Angle=%d,%d  FOV=%d", th, ph, fov);
    } else if (mode == 0) {
        //  Orthogonal - set world orientationn
        glWindowPos2i(5, 25);
        print("Orthogonal");
        glRotatef(ph, 1, 0, 0);
        glRotatef(th, 0, 1, 0);
        glWindowPos2i(5, 5);
        print("Angle=%d,%d", th, ph);
    } else {
        glWindowPos2i(5, 65);
        print("First Person");
        double vx = fp_ex - Sin(fp_th);
        double vy = Sin(fp_ph);
        double vz = fp_ez - Cos(fp_th);
        gluLookAt(fp_ex, 0, fp_ez, vx, vy, vz, 0, 1, 0);
        glWindowPos2i(5, 45);
        print("Location: x=%.1f, y=%.1f z=%.1f", fp_ex, fp_ey, fp_ez);
        glWindowPos2i(5, 25);
        print("Center of View: x=%.1f, y=%.1f z=%.1f", vx, vy, vz);
        glWindowPos2i(5, 5);
        print("FOV=%d", fov);
    }

    robot(0, 0, 0, 1, 0, 0, 0);
    robot(6, 4, 4, 0.6, 30, 45, 30);
    robot(-5, 0, -5, 0.7, 0, -90, 0);
    ufo(1, 10, -3, 30, 5, 5, 5);
    ufo(-10, 7, 3, 20, 15, -5, -5);

    //  Draw axes
    glColor3f(1, 1, 1);
    if (axes) {
        glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(len, 0.0, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, len, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, 0.0, len);
        glEnd();
        //  Label axes
        glRasterPos3d(len, 0.0, 0.0);
        print("X");
        glRasterPos3d(0.0, len, 0.0);
        print("Y");
        glRasterPos3d(0.0, 0.0, len);
        print("Z");
    }
    //  Render the scene and make it visible
    glFlush();
    glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
    if (mode != 2) {
        //  Right arrow key - increase angle by 5 degrees
        if (key == GLUT_KEY_RIGHT)
            th += 5;
            //  Left arrow key - decrease angle by 5 degrees
        else if (key == GLUT_KEY_LEFT)
            th -= 5;
            //  Up arrow key - increase elevation by 5 degrees
        else if (key == GLUT_KEY_UP)
            ph += 5;
            //  Down arrow key - decrease elevation by 5 degrees
        else if (key == GLUT_KEY_DOWN)
            ph -= 5;
        //  Keep angles to +/-360 degrees
        th %= 360;
        ph %= 360;
    } else {
        if (key == GLUT_KEY_UP) {
            fp_ez -= 2 * Cos(fp_th);
            fp_ex -= 2 * Sin(fp_th);
        }
        else if (key == GLUT_KEY_DOWN) {
            fp_ez += 2 * Cos(fp_th);
            fp_ex += 2 * Sin(fp_th);
        } else if (key == GLUT_KEY_RIGHT) {
            fp_th -= 2;
        } else if (key == GLUT_KEY_LEFT) {
            fp_th += 2;
        } else if (key == GLUT_KEY_PAGE_UP && fp_ph < 90) {
            fp_ph = (fp_ph + 5) % 360;
        } else if (key == GLUT_KEY_PAGE_DOWN && fp_ph > -90) {
            fp_ph = (fp_ph - 5) % 360;
        }
        fp_th %= 360;
    }
    //  Update projection
    project();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
    //  Exit on ESC
    if (ch == 27)
        exit(0);
        //  Toggle axes
    else if (ch == 'a' || ch == 'A')
        axes = 1 - axes;
        //  Switch display mode
    else if (ch == 'm' || ch == 'M')
        mode = (mode + 1) % 3;
        //  Change field of view angle
    else if (ch == '-' || ch == '_')
        fov++;
    else if (ch == '+' || ch == '=')
        fov--;
    //  Reproject
    project();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
    //  Ratio of the width to the height of the window
    asp = (height > 0) ? (double) width / height : 1;
    //  Set the viewport to the entire window
    glViewport(0, 0, width, height);
    //  Set projection
    project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char *argv[])
{
    //  Initialize GLUT
    glutInit(&argc, argv);
    //  Request double buffered, true color window with Z buffering at 600x600
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Assignment 4: Jianxiang Fan");
    //  Set callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    //  Pass control to GLUT so it can interact with the user
    glutMainLoop();
    return 0;
}

