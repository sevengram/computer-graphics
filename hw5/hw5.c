/*
 *  HW5: Lighting
 *
 *  Jianxiang Fan
 *  jianxiang.fan@colorado.edu
 *
 *  Key bindings
 *    a          Toggle axes
 *    m          Change mode
 *    ESC        Exit
 *    l          Toggles lighting
 *    1          Toggles LIGHT_0
 *    2          Toggles LIGHT_1
 *    3          Toggles LIGHT_2
 *    c          Toggles GL_COLOR_MATERIAL
 *
 *  Orthogonal Mode
 *    arrows     Change view angle
 *    [/]        Change size of the world
 *
 *  Perspective Mode
 *    arrows     Change view angle
 *    +/-        Changes field of view
 *    [/]        Change size of the world
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

#define PI 3.1415927

int axes = 1;       //  Display axes
int mode = 1;       //  Perspective/Orthogonal/First Person mode
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

int light = 1;
int light0 = 1;      // Light1
int light1 = 1;      // Light2
int light2 = 1;      // Light3
int color_material = 0; // Using color material

//  Macro for sin & cos in degrees
#define Cos(th) cos(PI/180*(th))
#define Sin(th) sin(PI/180*(th))

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
 *  Draw vertex in polar coordinates
 */
static void vertex(double th, double ph)
{
    double x = Sin(th) * Cos(ph);
    double y = Sin(ph);
    double z = Cos(th) * Cos(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x, y, z);
    glVertex3d(x, y, z);
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
    glNormal3f(0, 0, 1);
    glVertex3f(-1, -1, 1);
    glVertex3f(+1, -1, 1);
    glVertex3f(+1, +1, 1);
    glVertex3f(-1, +1, 1);
    //  Back
    glNormal3f(0, 0, -1);
    glVertex3f(+1, -1, -1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, +1, -1);
    glVertex3f(+1, +1, -1);
    //  Right
    glNormal3f(+1, 0, 0);
    glVertex3f(+1, -1, +1);
    glVertex3f(+1, -1, -1);
    glVertex3f(+1, +1, -1);
    glVertex3f(+1, +1, +1);
    //  Left
    glNormal3f(-1, 0, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, +1);
    glVertex3f(-1, +1, +1);
    glVertex3f(-1, +1, -1);
    //  Top
    glNormal3f(0, +1, 0);
    glVertex3f(-1, +1, +1);
    glVertex3f(+1, +1, +1);
    glVertex3f(+1, +1, -1);
    glVertex3f(-1, +1, -1);
    //  Bottom
    glNormal3f(0, -1, 0);
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
 *  Draw a cylinder
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 */
static void cylinder(double x, double y, double z,
                     double dx, double dy, double dz)
{
    const int d = 2;
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(dx, dy, dz);
    int j;
    for (j = 0; j < 360; j += d) {
        glBegin(GL_QUADS);
        glNormal3d(Cos(j + d / 2.0), 0, Sin(j + d / 2.0));
        glVertex3d(Cos(j), 0.5, Sin(j));
        glVertex3d(Cos(j + d), 0.5, Sin(j + d));
        glVertex3d(Cos(j + d), -0.5, Sin(j + d));
        glVertex3d(Cos(j), -0.5, Sin(j));
        glEnd();
    }

    for (j = 0; j < 360; j += d) {
        glBegin(GL_TRIANGLES);
        glNormal3d(0, 0, 1);
        glVertex3d(0.0, 0.5, 0.0);
        glVertex3d(Cos(j), 0.5, Sin(j));
        glVertex3d(Cos(j + d), 0.5, Sin(j + d));
        glEnd();
    }
    for (j = 0; j < 360; j += d) {
        glBegin(GL_TRIANGLES);
        glNormal3d(0, 0, -1);
        glVertex3d(0.0, -0.5, 0.0);
        glVertex3d(Cos(j), -0.5, Sin(j));
        glVertex3d(Cos(j + d), -0.5, Sin(j + d));
        glEnd();
    }
    glPopMatrix();
};

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

    int j;
    for (j = 0; j < 360; j += d) {
        glBegin(GL_TRIANGLES);
        glNormal3d(0, 0, -1);
        glVertex3d(0.0, 0, 0.0);
        glVertex3d(Cos(j), 0, Sin(j));
        glVertex3d(Cos(j + d), 0, Sin(j + d));
        glEnd();
    }

    //  Undo transformations
    glPopMatrix();
}

/*
 *  Draw an stick
 *     at (x,y,z)
 *     radius (r)
 *     radius (length)
 *     rotated theta about the y axis
 *     rotated psi about the z axis
 *     rotated phi about the z axis
 */
static void stick(double x, double y, double z, double r, double length,
                  double phi, double theta, double psi)
{
    //  Save transformation
    glPushMatrix();

    //  Offset and scale
    glTranslated(x, y, z);
    glRotated(phi, 1, 0, 0);
    glRotated(theta, 0, 1, 0);
    glRotated(psi, 0, 0, 1);

    cylinder(0, 0, 0, r, length, r);
    semisphere(0, length / 2, 0, r);
    glTranslated(0, -length / 2, 0);
    glRotated(180, 1, 0, 0);
    semisphere(0, 0, 0, r);

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

    GLfloat mat_ambient[] = {0.05f, 0.05f, 0.0f, 1.0f};
    GLfloat mat_diffuse[] = {0.5f, 0.5f, 0.4f, 1.0f};
    GLfloat mat_specular[] = {0.7f, 0.7f, 0.04f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);

    //  Save transformation
    glPushMatrix();

    glColor3f(0.9254, 0.9254, 0.4392);

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
    //  Set specular color to white
    GLfloat mat_ambient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat mat_diffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat mat_specular[] = {0.7f, 0.7f, 0.7f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);

    //  Save transformation
    glPushMatrix();

    glColor3f(0.7, 0.7, 0.7);

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
 *  Draw an android robot
 *     at (x,y,z)
 *     scale (s)
 *     rotated theta about the y axis
 *     rotated psi about the z axis
 *     rotated phi about the z axis
 */
static void android(double x, double y, double z, double s,
                    double phi, double theta, double psi)
{
    //  Set specular color to white
    GLfloat mat_ambient[] = {0.0f, 0.1f, 0.0f, 1.0f};
    GLfloat mat_diffuse[] = {0.4f, 0.6f, 0.4f, 1.0f};
    GLfloat mat_specular[] = {0.04f, 0.7f, 0.04f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);

    //  Save transformation
    glPushMatrix();

    glColor3f(0.6, 0.8, 0);

    //  Offset and scale
    glTranslated(x, y, z);
    glRotated(phi, 1, 0, 0);
    glRotated(theta, 0, 1, 0);
    glRotated(psi, 0, 0, 1);
    glScaled(s, s, s);

    // Head
    semisphere(0.0, 1.15, 0, 1);

    // Body
    cylinder(0, 0, 0, 1, 2, 1);

    // Arms
    stick(-1.35, 0.2, 0, 0.25, 1.1, 0, 0, 0);
    stick(1.35, 0.2, 0, 0.25, 1.1, 0, 0, 0);

    // Legs
    stick(-0.35, -1.3, 0, 0.22, 0.6, 0, 0, 0);
    stick(0.35, -1.3, 0, 0.22, 0.6, 0, 0, 0);

    stick(-0.35, 2.2, 0, 0.05, 0.25, 0, 0, 10);
    stick(0.35, 2.2, 0, 0.05, 0.25, 0, 0, -10);

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

void open_light()
{
    glShadeModel(GL_SMOOTH);

    //  Translate intensity to color vectors
    float ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    //  Light position
    float position1[] = {30, 30, 15, 0};
    float position2[] = {-30, 30, 15, 0};
    float position3[] = {0, -30, -20, 0};
    //  Draw light position as ball (still no lighting here)
    glColor3f(1, 1, 1);
    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glColorMaterial(GL_FRONT, GL_SPECULAR);


    if (color_material) {
        glEnable(GL_COLOR_MATERIAL);
    } else {
        glDisable(GL_COLOR_MATERIAL);
    }

    if (light0) {
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0, GL_POSITION, position1);
    } else {
        glDisable(GL_LIGHT0);
    }

    if (light1) {
        //  Enable light 1
        glEnable(GL_LIGHT1);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT1, GL_POSITION, position2);
    } else {
        glDisable(GL_LIGHT1);
    }

    if (light2) {
        //  Enable light 2
        glEnable(GL_LIGHT2);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT2, GL_POSITION, position3);
    } else {
        glDisable(GL_LIGHT2);
    }
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

    glWindowPos2i(5, 5);
    print("Lighting=%s  Light_0=%s  Light_1=%s  Light_2=%s  Color Material=%s",
          light == 1 ? "On" : "Off",
          light0 == 1 ? "On" : "Off",
          light1 == 1 ? "On" : "Off",
          light2 == 1 ? "On" : "Off",
          color_material == 1 ? "On" : "Off");

    if (mode == 1) {
        //  Perspective - set eye position
        glWindowPos2i(5, 45);
        print("Perspective");
        double Ex = -2 * dim * Sin(th) * Cos(ph);
        double Ey = +2 * dim * Sin(ph);
        double Ez = +2 * dim * Cos(th) * Cos(ph);
        gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
        glWindowPos2i(5, 25);
        print("Angle=%d,%d  FOV=%d  Dim=%.2f", th, ph, fov, dim);
    } else if (mode == 0) {
        //  Orthogonal - set world orientationn
        glWindowPos2i(5, 45);
        print("Orthogonal");
        glRotatef(ph, 1, 0, 0);
        glRotatef(th, 0, 1, 0);
        glWindowPos2i(5, 25);
        print("Angle=%d,%d  Dim=%.2f", th, ph, dim);
    } else {
        glWindowPos2i(5, 85);
        print("First Person");
        double vx = fp_ex - Sin(fp_th);
        double vy = Sin(fp_ph);
        double vz = fp_ez - Cos(fp_th);
        gluLookAt(fp_ex, 0, fp_ez, vx, vy, vz, 0, 1, 0);
        glWindowPos2i(5, 65);
        print("Location: x=%.1f, y=%.1f z=%.1f", fp_ex, fp_ey, fp_ez);
        glWindowPos2i(5, 45);
        print("Center of View: x=%.1f, y=%.1f z=%.1f", vx, vy, vz);
        glWindowPos2i(5, 25);
        print("FOV=%d", fov);
    }
    if (light) {
        open_light();
    } else {
        glDisable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
    }

    android(0, 0, 0, 2, 0, 0, 0);
    android(6, 4, 4, 1, 30, 45, 30);
    robot(0, 1, 6, 0.6, 0, 15, 0);
    android(-5, 0, -5, 1.5, 0, -90, 0);
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
            //  Left arrow key - decrease angle by 5 degreesTo enable the screen reader compatible view, please press ctrl + z.

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
    else if (ch == 'a' || ch == 'A')
        axes = 1 - axes;
    else if (ch == 'm' || ch == 'M')
        mode = (mode + 1) % 3;
    else if (ch == '-' || ch == '_')
        fov++;
    else if (ch == '+' || ch == '=')
        fov--;
    else if (ch == 'l' || ch == 'L')
        light = 1 - light;
    else if (ch == '1')
        light0 = 1 - light0;
    else if (ch == '2')
        light1 = 1 - light1;
    else if (ch == '3')
        light2 = 1 - light2;
    else if (ch == 'c' || ch == 'C')
        color_material = 1 - color_material;
    else if ((ch == '[' || ch == '{') && dim > 1.0 && mode != 2)
        dim -= 1.0;
    else if ((ch == ']' || ch == '}') && mode != 2)
        dim += 1.0;
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
    glutCreateWindow("Assignment 5: Jianxiang Fan");
    //  Set callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    //  Pass control to GLUT so it can interact with the user
    glutMainLoop();
    return 0;
}

