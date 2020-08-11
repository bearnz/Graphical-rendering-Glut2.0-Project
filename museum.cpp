//  ========================================================================
//  COSC363: Computer Graphics (2020);  University of Canterbury.
//  Assignment 1
//
//  Name: Joshua Smith
//  usercode: jsm160
//  ID number: 32109650
//
//  FILE NAME: museum.cpp
//
//  Move camera with arrow keys, change camera height with Page Up/Down
//
//  TODO:
//  - Completely rebuild this to fit new assignment brief
//    - Use same base classes for building walls etc, but change some dimensions
//    - Retexture everything
//  ========================================================================

#define GL_CLAMP_TO_EDGE 0x812F
#include <iostream>
#include <fstream>
#include <climits>
#include <math.h>
#include <GL/freeglut.h>
#include <cmath>
#include <GL/glut.h>
#include "loadTGA.h"
#include "loadBMP.h"

//--Globals-----------------------------------------------------------------------
using namespace std;
GLuint txId[11];   //Texture ids
GLUquadric *q;    //Required for creating cylindrical objects
float *x, *y, *z;  //vertex coordinate arrays
int *t1, *t2, *t3; //triangles
int nvrt, ntri;    //total number of vertices and triangles
int thetaState = 0; //state of the angle theta, used in timing the movement of the robots
float theta = 0, theta1 = 0, theta2 = 0, theta3 = 0; //angle theta of the poition of the robots and the angles of their limbs
float angle=0, look_x = 0, look_y = 10, look_z=-1., eye_x = 0, eye_y = 30, eye_z = 200.0, move_speed = 5.0, rotation_speed = 0.08; //Camera parameters
float ctr = 0.01745; //convert deg to rad
float ballx = 40, bally = 65, time_ball = 0; //position of the cannonball
float shipz = 40, shipy = 3; //position of the spaceship
int shipTime = 0, padState = 0, landed = 0;
const float GRAVITY = 2.5; //Gravity constant for physics-base animation
const double pi = 3.1415926;
const int N = 57; //Total number of vertices on base curve for spaceship
const int M = 161; //Total number of points on the ship flight path


void loadMeshFile(const char* fname)
{
    ifstream fp_in;
    int num, ne;

    fp_in.open(fname, ios::in);
    if(!fp_in.is_open())
    {
        cout << "Error opening mesh file" << endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n');				//ignore first line
    fp_in >> nvrt >> ntri >> ne;			    // read number of vertices, polygons, edges

    x = new float[nvrt];                        //create arrays
    y = new float[nvrt];
    z = new float[nvrt];

    t1 = new int[ntri];
    t2 = new int[ntri];
    t3 = new int[ntri];

    for(int i=0; i < nvrt; i++)                         //read vertex list
        fp_in >> x[i] >> y[i] >> z[i];

    for(int i=0; i < ntri; i++)                         //read polygon list
    {
        fp_in >> num >> t1[i] >> t2[i] >> t3[i];
        if(num != 3)
        {
            cout << "ERROR: Polygon with index " << i  << " is not a triangle." << endl;  //not a triangle!!
            exit(1);
        }
    }

    fp_in.close();
    cout << " File successfully read." << endl;
}
//--Loads required textures into program------------------------------------------
void loadTexture()
//  This will need retexturing
{
   glGenTextures(11, txId); 	// Create 10 texture ids
   glBindTexture(GL_TEXTURE_2D, txId[0]);
   loadTGA("Wall3.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glBindTexture(GL_TEXTURE_2D, txId[1]);
   loadTGA("grass.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glBindTexture(GL_TEXTURE_2D, txId[2]);
   loadTGA("Wall4.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //Skybox left
   glBindTexture(GL_TEXTURE_2D, txId[3]);
   loadTGA("hills_lf.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Skybox front
   glBindTexture(GL_TEXTURE_2D, txId[4]);
   loadTGA("hills_ft.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Skybox right
   glBindTexture(GL_TEXTURE_2D, txId[5]);
   loadTGA("hills_rt.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Skybox back
   glBindTexture(GL_TEXTURE_2D, txId[6]);
   loadTGA("hills_bk.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Skybox top
   glBindTexture(GL_TEXTURE_2D, txId[7]);
   loadTGA("hills_up.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Skybox bottom
   glBindTexture(GL_TEXTURE_2D, txId[8]);
   loadTGA("hills_dn.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Mona Lisa
   glBindTexture(GL_TEXTURE_2D, txId[9]);
   loadTGA("mona_lisa.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Museum sign
   glBindTexture(GL_TEXTURE_2D, txId[10]);
   loadTGA("sign.tga");
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_BIT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

   glFlush();
}
//--Function to compute the normal vector of a triangle with index tindx ---------
void normal(int tindx)
{
    float x1 = x[t1[tindx]], x2 = x[t2[tindx]], x3 = x[t3[tindx]];
    float y1 = y[t1[tindx]], y2 = y[t2[tindx]], y3 = y[t3[tindx]];
    float z1 = z[t1[tindx]], z2 = z[t2[tindx]], z3 = z[t3[tindx]];
    float nx, ny, nz;
    nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
    ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
    nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
    glNormal3f(nx, ny, nz);

    glFlush();
}
//--Function to compute normal when performing surface of revolution--------------
void normal2(float x1, float y1, float z1,
            float x2, float y2, float z2,
              float x3, float y3, float z3 )
{
      float nx, ny, nz;
      nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
      ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
      nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);

      glNormal3f(nx, ny, nz);

      glFlush();
}
//--Draws the mesh model of the cannon from lab 2---------------------------------
void drawCannonBarrel()
{
    //Construct the object model here using triangles read from OFF file
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_TRIANGLES);
        for(int tindx = 0; tindx < ntri; tindx++)
        {
           normal(tindx);
           glVertex3d(x[t1[tindx]], y[t1[tindx]], z[t1[tindx]]);
           glVertex3d(x[t2[tindx]], y[t2[tindx]], z[t2[tindx]]);
           glVertex3d(x[t3[tindx]], y[t3[tindx]], z[t3[tindx]]);
        }
    glEnd();
    glFlush();
}
//--Draws the support for under the cannon barrel---------------------------------
void drawCannonSupport()
{
  glPushMatrix();
    glColor3f(0.5, 0.2, 0.6);
    glRotatef(90.0, 0, 1, 0);
    glutSolidCylinder(7.5, 60.0, 90, 90);
  glPopMatrix();

  glPushMatrix();
    glTranslatef(60, 0, 0);
    glutSolidSphere(7.5, 90, 90);
  glPopMatrix();

  glPushMatrix();
    glutSolidSphere(7.5, 90, 90);
  glPopMatrix();
}
//--Draws and textures the Cannon-------------------------------------------------
void drawCannon()
{
    //Mid right
    glPushMatrix();
     glTranslatef(-10, 20, 17);
     glScalef(40, 20, 6);
     glutSolidCube(1);
    glPopMatrix();

    //Mid left
    glPushMatrix();
     glTranslatef(-10, 20, -17);
     glScalef(40, 20, 6);
     glutSolidCube(1);
    glPopMatrix();

    //Cannon barrel
    glPushMatrix();
     glColor3f(0.2, 0.2, 0.2);
     glTranslatef(-20, 30, 0);
     glRotatef(30.0, 0, 0, 1);
     glTranslatef(20, -30, 0);
     drawCannonBarrel();
    glPopMatrix();

    //Support
    glPushMatrix();
      glTranslatef(-42, -7, 0);
      glRotatef(35.0, 0, 0, 1);
      drawCannonSupport();
    glPopMatrix();

    //Wheel Pin
    glPushMatrix();
      glColor3f(0.1, 0.7, 0.4);
      glTranslatef(-5, 16.5, -25);
      glutSolidCylinder(3.0, 52, 18, 36);
    glPopMatrix();

    //Left wheel
    glPushMatrix();
      glColor3f(1.0, 0, 0);
      glTranslatef(-5, 16.5, -21.5);
      glutSolidCylinder(30.75, 2.5, 18, 18);
    glPopMatrix();

    //Right wheel
    glPushMatrix();
      glColor3f(1.0, 0, 0);
      glTranslatef(-5, 16.5, 21.5);
      glutSolidCylinder(30.75, 2.5, 18, 18);
    glPopMatrix();

    //Cannonball
    glPushMatrix();
     glColor3f(0.0, 0.0, 0.0);
     glTranslatef(ballx, bally, 0);
     glRotatef(45, 0, 0, 1);
     glutSolidSphere(5, 36, 18);
    glPopMatrix();
    glFlush();
}
//--Draw spaceship exhibit--------------------------------------------------------
void drawSpaceship(void)
{
    float vx[N] = {0, 1, 2, 3, 4, 4.0975, 4.1899999999999995, 4.2775, 4.36, 4.4375,
                  4.51, 4.5775, 4.64, 4.6975, 4.75, 4.7975, 4.84, 4.8775, 4.91, 4.9375,
                  4.96, 4.9775, 4.99, 4.9975, 5.0, 4.99984375, 4.99875, 4.99578125, 4.99,
                  4.98046875, 4.96625, 4.94640625, 4.92, 4.88609375, 4.84375, 4.79203125,
                  4.73, 4.65671875, 4.57125, 4.47265625, 4.36, 4.23234375, 4.08875, 3.92828125,
                  3.75, 3.55296875, 3.3362499999999997, 3.0989062499999998, 2.84, 2.55859375,
                  2.25375, 1.9245312499999998, 1.5699999999999998, 1.1892187499999998, 0.78125,
                  0.3451562499999996, 0};
    float vy[N] = {0, 0, 0, 0, 0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0, 2.25, 2.5, 2.75,
                  3.0, 3.25, 3.5, 3.75, 4.0, 4.25, 4.5, 4.75, 5.0, 5.25, 5.5, 5.75, 6.0, 6.25,
                  6.5, 6.75, 7.0, 7.25, 7.5, 7.75, 8.0, 8.25, 8.5, 8.75, 9.0, 9.25, 9.5, 9.75,
                  10.0, 10.25, 10.5, 10.75, 11.0, 11.25, 11.5, 11.75, 12.0, 12.25, 12.5, 12.75, 13.0};
    float vz[N] = {0};
    float wx[N] = {0};
    float wy[N] = {0};
    float wz[N] = {0};
    float slices = 20;
    float step = (1/slices)*360;

    for(int i = 0; i < slices; i++)
    {
        glColor3f(0.7, 0.2, 0.2);
        glBegin(GL_TRIANGLE_STRIP);
        for(int j = 0; j < N; j++)
        {
            wx[j] = vx[j]*cos(step*ctr) + vz[j]*sin(step*ctr);
            wy[j] = vy[j];
            wz[j] = -vx[j]*sin(step*ctr) + vz[j]*cos(step*ctr);
            if (j > 0) normal2( wx[j-1], wy[j-1], wz[j-1],
                    vx[j-1], vy[j-1], vz[j-1],
                    vx[j], vy[j], vz[j] );
            glVertex3f(vx[j], vy[j], vz[j]);
            if (j > 0) normal2( wx[j-1], wy[j-1], wz[j-1],
                    vx[j], vy[j], vz[j],
                    wx[j], wy[j], wz[j] );
            glVertex3f(wx[j], wy[j], wz[j]);
            vx[j] = wx[j];
            vy[j] = wy[j];
            vz[j] = wz[j];
        }

        glEnd();
    }
    for (int i = 0; i<360; i+=120)
    {
      glPushMatrix();
        glRotatef(i, 0, 1, 0);
        glTranslatef(0, 1, 3.0);
        glRotatef(55, 1, 0, 0);
        glutSolidCone(2, 5, 30, 30);
      glPopMatrix();
    }
    glFlush();

}
//--Draw painting-----------------------------------------------------------------
void drawPainting(void)
{
  glPushMatrix();
    glColor3f(0.5, 0.1, 0.5);
    glTranslatef(0, 7.5, 0);
    glScalef(10.0, 15.0, 10.0);
    glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
    glColor3f(1,0,0);
    glTranslatef(0,22.5,-4);
    glScalef(8, 13, 0.5);
    glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
    glColor3f(1,1,0);
    glTranslatef(-4.5,22.5,-3.5);
    glScalef(1, 15, 0.5);
    glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
    glColor3f(1,1,0);
    glTranslatef(4.5,22.5,-3.5);
    glScalef(1, 15, 0.5);
    glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
    glColor3f(1,1,0);
    glTranslatef(0,29.5,-3.5);
    glScalef(8, 1, 0.5);
    glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
    glColor3f(1,1,0);
    glTranslatef(0,15.5,-3.5);
    glScalef(8, 1, 0.5);
    glutSolidCube(1);
  glPopMatrix();

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, txId[9]);

 glBegin(GL_QUADS);
  //Texture front of the wall
  glNormal3f(1.0, 0.0, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(-4, 29, -3.74);
  glTexCoord2f(0.0, 0.0); glVertex3f(-4, 16, -3.74);
  glTexCoord2f(1.0, 0.0); glVertex3f(4, 16, -3.74);
  glTexCoord2f(1.0, 1.0); glVertex3f(4, 29, -3.74);
 glEnd();
 glDisable(GL_TEXTURE_2D);
 glFlush();
}
//--Ground Plane -----------------------------------------------------------------
void floor()
{
    float floor_height = -0.1;
    glColor3f(0.5, 0.5, 0.5);
    glNormal3f(0.0, 1.0, 0.0);
    //The floor plane is a 200x200 grid
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glBegin(GL_QUADS);
    for(int i = -200; i < 200; i++)
    {
        for(int j = -200;  j < 200; j++)
        {
            glVertex3f(i, floor_height, j);
            glVertex3f(i, floor_height, j+1);
            glVertex3f(i+1, floor_height, j+1);
            glVertex3f(i+1, floor_height, j);
        }
    }
     glTexCoord2f(0.0, 100.0); glVertex3f(-200, 0, 200);
     glTexCoord2f(0.0, 0.0); glVertex3f(-200, 0, -200);
     glTexCoord2f(100.0, 0.0); glVertex3f(200, 0, -200);
     glTexCoord2f(100.0, 100.0); glVertex3f(200, 0, 200);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glFlush();
}
//--Draw Skybox-------------------------------------------------------------------
void skybox()
{
    glEnable(GL_TEXTURE_2D);
  /* I've had to translate the textures by approx 12 pixels along the
   x-axis because the textures weren't clamping properly despite GL_CLAMP_TO_EDGE being set */
  // Left Wall
  glBindTexture(GL_TEXTURE_2D, txId[3]);
  glBegin(GL_QUADS);
  glTexCoord2f(1.0235, 1); glVertex3f(-512,  -512, 512);
  glTexCoord2f(0.0235, 1); glVertex3f(-512, -512, -512);
  glTexCoord2f(0.0235, 0); glVertex3f(-512, 512, -512);
  glTexCoord2f(1.0235, 0); glVertex3f(-512, 512, 512);
  glEnd();

  // Front Wall
  glBindTexture(GL_TEXTURE_2D, txId[4]);
  glBegin(GL_QUADS);
  glTexCoord2f(1.0235, 1); glVertex3f(-512, -512, -512);
  glTexCoord2f(0.0235, 1); glVertex3f(512, -512, -512);
  glTexCoord2f(0.0235, 0); glVertex3f(512, 512, -512);
  glTexCoord2f(1.0235, 0); glVertex3f(-512, 512, -512);
  glEnd();

 // Right Wall
  glBindTexture(GL_TEXTURE_2D, txId[5]);
  glBegin(GL_QUADS);
  glTexCoord2f(1.0235, 1); glVertex3f(512, -512, -512);
  glTexCoord2f(0.0235, 1); glVertex3f(512, -512, 512);
  glTexCoord2f(0.0235, 0); glVertex3f(512, 512, 512);
  glTexCoord2f(1.0235, 0); glVertex3f(512, 512, -512);
  glEnd();


  // Rear Wall
  glBindTexture(GL_TEXTURE_2D, txId[6]);
  glBegin(GL_QUADS);
  glTexCoord2f(1.0235, 1); glVertex3f(512, -512, 512);
  glTexCoord2f(0.0235, 1); glVertex3f(-512, -512, 512);
  glTexCoord2f(0.0235, 0); glVertex3f(-512, 512, 512);
  glTexCoord2f(1.0235, 0); glVertex3f(512, 512, 512);
  glEnd();

  // Top
  glBindTexture(GL_TEXTURE_2D, txId[7]);
  glBegin(GL_QUADS);
  glTexCoord2f(1.0235, 0); glVertex3f(-512, 512, -512);
  glTexCoord2f(1.0235, 1); glVertex3f(512, 512,  -512);
  glTexCoord2f(0.0235, 1); glVertex3f(512, 512, 512);
  glTexCoord2f(0.0235, 0); glVertex3f(-512, 512, 512);
  glEnd();

  // Floor
  glBindTexture(GL_TEXTURE_2D, txId[8]);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 1); glVertex3f(-512, -512, 512);
  glTexCoord2f(1, 1); glVertex3f(512, -512,  512);
  glTexCoord2f(1, 0); glVertex3f(512, -512, -512);
  glTexCoord2f(0, 0); glVertex3f(-512, -512, -512);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glFlush();
}
//--Draw and texture each wall----------------------------------------------------
void drawWall(void)
{
    glPushMatrix();
     glColor3f(0.0, 0.0, 1.0);
     glScalef(100.0, 40.0, 2.0);
     glutSolidCube(1);
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[0]);

   glBegin(GL_QUADS);
    //Texture front of the wall
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 5.0); glVertex3f(-50, 20, 1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(-50, -20, 1.01);
    glTexCoord2f(10.0, 0.0); glVertex3f(50, -20, 1.01);
    glTexCoord2f(10.0, 5.0); glVertex3f(50, 20, 1.01);
    //Texture the back of the wall
    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 5.0); glVertex3f(-50, 20, -1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(-50, -20, -1.01);
    glTexCoord2f(10.0, 0.0); glVertex3f(50, -20, -1.01);
    glTexCoord2f(10.0, 5.0); glVertex3f(50, 20, -1.01);
    //Texture the side of the wall
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 5.0); glVertex3f(-50.01, 20, -1);
    glTexCoord2f(0.0, 0.0); glVertex3f(-50.01, -20, -1);
    glTexCoord2f(0.2, 0.0); glVertex3f(-50.01, -20, 1);
    glTexCoord2f(0.2, 5.0); glVertex3f(-50.01, 20, 1);
    //Texture the side of the wall
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 5.0); glVertex3f(50.01, 20, -1);
    glTexCoord2f(0.0, 0.0); glVertex3f(50.01, -20, -1);
    glTexCoord2f(0.2, 0.0); glVertex3f(50.01, -20, 1);
    glTexCoord2f(0.2, 5.0); glVertex3f(50.01, 20, 1);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glFlush();
}
//--Draw and texture the front door of the Museum---------------------------------
void drawDoor(void)
{
    //Right side of the gate
    glPushMatrix();
     glTranslatef(55.0, 0.0, 0.0);
     glScalef(89.0, 40.0, 2.0);
     glutSolidCube(1);
    glPopMatrix();
    //Left side of the gate
    glPushMatrix();
     glTranslatef(-55.0, 0.0, 0.0);
     glScalef(89.0, 40.0, 2.0);
     glutSolidCube(1);
    glPopMatrix();
    //Middle of the gate
    glPushMatrix();
     glTranslatef(0.0, 10.0 , 0.0);
     glScalef(22.0, 20.0, 2.0);
     glutSolidCube(1);
    glPopMatrix();
    //Museum Sign
    glPushMatrix();
      glColor3f(1.0, 1.0, 1.0);
      glTranslatef(0.0, 10.0, 1.0);
      glScalef(30.0, 7.5, 1.0);
      glutSolidCube(1.0);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[10]);
    glBegin(GL_QUADS);
     glNormal3f(1.0, 0.0, 0.0);
     //Sign
     glNormal3f(1.0, 0.0, 0.0);
     glTexCoord2f(0.0, 1); glVertex3f(-15, 13.75, 1.51);
     glTexCoord2f(0.0, 0.0); glVertex3f(-15, 6.25, 1.51);
     glTexCoord2f(1.0, 0.0); glVertex3f(15, 6.25, 1.51);
     glTexCoord2f(1.0, 1.0); glVertex3f(15, 13.75, 1.51);
    glEnd();
    glDisable(GL_TEXTURE_2D);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, txId[0]);
   glColor3f(0.8, 0.7, 0.3);
   glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    //South-West Side
    glTexCoord2f(0.0, 5.0); glVertex3f(-100, 20, 1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(-100, -20, 1.01);
    glTexCoord2f(7.8, 0.0); glVertex3f(-10.5, -20, 1.01);
    glTexCoord2f(7.8, 5.0); glVertex3f(-10.5, 20, 1.01);
    //South-East Side
    glTexCoord2f(0.0, 5.0); glVertex3f(10.5, 20, 1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(10.5, -20, 1.01);
    glTexCoord2f(7.8, 0.0); glVertex3f(100, -20, 1.01);
    glTexCoord2f(7.8, 5.0); glVertex3f(100, 20, 1.01);
    //South-Middle
    glTexCoord2f(0.0, 2.5); glVertex3f(-10.5, 20, 1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10.5, 0.0, 1.01);
    glTexCoord2f(2, 0.0); glVertex3f(10.5, 0.0, 1.01);
    glTexCoord2f(2, 2.5); glVertex3f(10.5, 20, 1.01);
    //North-West Side
    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 5.0); glVertex3f(-100, 20, -1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(-100, -20, -1.01);
    glTexCoord2f(7, 0.0); glVertex3f(-10.5, -20, -1.01);
    glTexCoord2f(7, 5.0); glVertex3f(-10.5, 20, -1.01);
    //North-East Side
    glTexCoord2f(0.0, 5.0); glVertex3f(10.5, 20, -1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(10.5, -20, -1.01);
    glTexCoord2f(7, 0.0); glVertex3f(100, -20, -1.01);
    glTexCoord2f(7, 5.0); glVertex3f(100, 20, -1.01);
    //North-middle
    glTexCoord2f(0.0, 2.5); glVertex3f(-10.5, 20, -1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10.5, 0.0, -1.01);
    glTexCoord2f(2, 0.0); glVertex3f(10.5, 0.0, -1.01);
    glTexCoord2f(2, 2.5); glVertex3f(10.5, 20, -1.01);
    //Underside of gate
    glNormal3f(0.0, -1.0, 0.0);
    glTexCoord2f(0.0, .33); glVertex3f(-10.5, -0.01, 1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10.5, -0.01, -1.0);
    glTexCoord2f(2, 0.0); glVertex3f(10.5, -0.01, -1.0);
    glTexCoord2f(2, .33); glVertex3f(10.5, -0.01, 1.0);
    //Inner East of gate
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 3.5); glVertex3f(10.49, 0, -1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(10.49, -20, -1.01);
    glTexCoord2f(0.33, 0.0); glVertex3f(10.49, -20, 1.01);
    glTexCoord2f(0.33, 3.5); glVertex3f(10.49, 0, 1.01);
    //Inner West of gate
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 3.5); glVertex3f(-10.49, 0, -1.01);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10.49, -20, -1.01);
    glTexCoord2f(0.33, 0.0); glVertex3f(-10.49, -20, 1.01);
    glTexCoord2f(0.33, 3.5); glVertex3f(-10.49, 0, 1.01);
    //Texture the side of the wall
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 5.0); glVertex3f(-50.01, 20, -1);
    glTexCoord2f(0.0, 0.0); glVertex3f(-50.01, -20, -1);
    glTexCoord2f(0.2, 0.0); glVertex3f(-50.01, -20, 1);
    glTexCoord2f(0.2, 5.0); glVertex3f(-50.01, 20, 1);
    //Texture the side of the wall
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 5.0); glVertex3f(50.01, 20, -1);
    glTexCoord2f(0.0, 0.0); glVertex3f(50.01, -20, -1);
    glTexCoord2f(0.2, 0.0); glVertex3f(50.01, -20, 1);
    glTexCoord2f(0.2, 5.0); glVertex3f(50.01, 20, 1);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glFlush();
}
//--Draws a single support pillar-------------------------------------------------
void drawPillar(void)
{
    float radius = 3.0;
    float height = 40.0;
    float slices = 90.0;

    glPushMatrix();
     glTranslatef(0, height, 0);
     glRotatef(90, 1, 0, 0);
     glutSolidCylinder(radius, height, slices, 1);
    glPopMatrix();

    glPushMatrix();
     glTranslatef(0, height-0.5, 0);
     glRotatef(90, 1, 0, 0);
     glutSolidTorus(0.5, radius, 90, 90);
    glPopMatrix();

    glPushMatrix();
     glTranslatef(0, 0.5, 0);
     glRotatef(90, 1, 0, 0);
     glutSolidTorus(0.5, radius, 90, 90);
    glPopMatrix();

    glFlush();
}
//--Draws the roof of the museum--------------------------------------------------
void drawMuseumRoof(void)
{
    //Top Side
    glPushMatrix();
     glTranslatef(0, 5.0, 0);
     glRotatef(90, 1, 0, 0);
     glScalef(250, 150, 10.0);
     glutSolidCube(1.0);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    glBegin(GL_QUADS);
      glNormal3f(1, 0, 0);
      //Texture front of the roof
      glNormal3f(1.0, 0.0, 0.0);
      glTexCoord2f(0.0, 2.5); glVertex3f(-125, 10, 75.01);
      glTexCoord2f(0.0, 0.0); glVertex3f(-125, 0, 75.01);
      glTexCoord2f(25.0, 0.0); glVertex3f(125, 0, 75.01);
      glTexCoord2f(25.0, 2.5); glVertex3f(125, 10, 75.01);
      //Texture back of the roof
      glTexCoord2f(0.0, 2.5); glVertex3f(-125, 10, -75.01);
      glTexCoord2f(0.0, 0.0); glVertex3f(-125, 0, -75.01);
      glTexCoord2f(25.0, 0.0); glVertex3f(125, 0, -75.01);
      glTexCoord2f(25.0, 2.5); glVertex3f(125, 10, -75.01);
      //Texture right of the roof
      glNormal3f(0.0, 0.0, 1.0);
      glTexCoord2f(0.0, 2.5); glVertex3f(125.01, 10, -75);
      glTexCoord2f(0.0, 0.0); glVertex3f(125.01, 0, -75);
      glTexCoord2f(15.0, 0.0); glVertex3f(125.01, 0, 75);
      glTexCoord2f(15.0, 2.5); glVertex3f(125.01, 10, 75);
      //Texture left of the roof
      glTexCoord2f(0.0, 2.5); glVertex3f(-125.01, 10, -75);
      glTexCoord2f(0.0, 0.0); glVertex3f(-125.01, 0, -75);
      glTexCoord2f(15.0, 0.0); glVertex3f(-125.01, 0, 75);
      glTexCoord2f(15.0, 2.5); glVertex3f(-125.01, 10, 75);
      //Texture bottom of the roof
      glNormal3f(0.0, 1.0, 0.0);
      glTexCoord2f(0.0, 25.0); glVertex3f(-125, -0.01, 75);
      glTexCoord2f(0.0, 0.0); glVertex3f(-125, -0.01, -75);
      glTexCoord2f(15.0, 0.0); glVertex3f(125, -0.01, -75);
      glTexCoord2f(15.0, 25.0); glVertex3f(125, -0.01, 75);
      //Texture top of the roof
      glTexCoord2f(0.0, 25.0); glVertex3f(-125, 10.01, 75);
      glTexCoord2f(0.0, 0.0); glVertex3f(-125, 10.01, -75);
      glTexCoord2f(15.0, 0.0); glVertex3f(125, 10.01, -75);
      glTexCoord2f(15.0, 25.0); glVertex3f(125, 10.01, 75);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glFlush();
}

//--Draws the shell of the museum from other function calls-----------------------
void drawMuseumShell(void)
{
    //East Wall
    glPushMatrix();
     glColor3f(0.0,  0.0, 1.0);
     glRotatef(-90.0, 0.0, 1.0, 0.0);
     glTranslatef(0.0, 20.0, -99.0);
     drawWall();
    glPopMatrix();

    //West Wall
    glPushMatrix();
     glRotatef(90.0, 0.0, 1.0, 0.0);
     glTranslatef(0.0, 20.0, -99.0);
     drawWall();
    glPopMatrix();

    //South Door Entrance
    glPushMatrix();
     glTranslatef(0.0, 20.0, 49);
     drawDoor();
    glPopMatrix();

    //North Wall
    glPushMatrix();
     glScalef(1.0, 1.0, 1.0);
     glTranslatef(-50, 20.0, -50);
     drawWall();
    glPopMatrix();

    glPushMatrix();
     glScalef(1.0, 1.0, 1.0);
     glTranslatef(50.0, 20.0, -50.0);
     drawWall();
    glPopMatrix();

    //Roof
    glPushMatrix();
     glTranslatef(0, 40, 0);
     drawMuseumRoof();
    glPopMatrix();

    //NS Pillars
    for(int x=-120; x<=120; x+=20)
    {
      glColor3f(1.0, 1.0, 0.7);
      //North Side
      glPushMatrix();
        glColor3f(0.0, 1.0, 0.0);
        glTranslatef(x, 0, -71);
        drawPillar();
      glPopMatrix();
      if(x != 0) //Prevents pillar from being drawn in front of the door
      {
        //South Side
        glPushMatrix();
          glColor3f(0.0, 1.0, 0.0);
          glTranslatef(x, 0, 71);
          drawPillar();
        glPopMatrix();
      }
    }
    //EW Pillars
    for(int z=-51; z<=51; z+=20)
    {
      //West Side
      glPushMatrix();
        glColor3f(0.0, 1.0, 0.0);
        glTranslatef(-120, 0, z);
        drawPillar();
      glPopMatrix();
      //East Side
      glPushMatrix();
        glColor3f(0.0, 1.0, 0.0);
        glTranslatef(120, 0, z);
        drawPillar();
      glPopMatrix();
    }
    glFlush();
}
//--Draws exhibits----------------------------------------------------------------
void drawExhibits(void)
{
    glPushMatrix();
      glTranslatef(90, 3.1, 35.0);
      glRotatef(90, 0, 1, 0);
      glScalef(0.2, 0.2, 0.2);
      drawCannon();
    glPopMatrix();

    glPushMatrix();
      glTranslatef(0, 0, -45);
      drawPainting();
    glPopMatrix();

    glPushMatrix();
      glColor3f(0,1,0);
      glTranslatef(-90, 1.0, 40);
      glRotatef(90, 1, 0, 0);
      glutSolidTorus(.5, 3, 10, 36);
    glPopMatrix();

    glPushMatrix();
      glColor3f(0,1,0);
      glTranslatef(-90, 1.0, -40);
      glRotatef(90, 1, 0, 0);
      glutSolidTorus(.5, 3, 10, 36);
    glPopMatrix();

    glPushMatrix();
      glColor3f(1,0,0);
      glTranslatef(-90, shipy, shipz);
      glScalef(.5,.5,.5);
      drawSpaceship();
    glPopMatrix();

    glFlush();
}

//--Calcuates the coordinates and timing for the cannonball after firing----------
void fireCannon(int value)
//  We are going to assume that each increment of this timer counts as 1 second for physics calculations
{
    float velocity = 40;
    float firingAngle = 30;
    float V_y = (sin(firingAngle*pi/180)*velocity);

    if(bally > -10)
      {
        if(bally + V_y-GRAVITY*(pow(time_ball, 2))/2 < -10)
        {
          bally = -10;
        }
        else
        {
          bally += V_y-GRAVITY*(pow(time_ball, 2))/2;
        }
        ballx += (cos(firingAngle*pi/180)*velocity);
        time_ball+=1;
      }

    glutPostRedisplay();
    glutTimerFunc(30, fireCannon, 0);

    glFlush();
}

//--Launches the spaceship from one pad to the other------------------------------
void launchShip(int value)
{
    float flightz[M] = {40.0, 39.5, 39.0, 38.5, 38.0, 37.5, 37.0, 36.5, 36.0,
      35.5, 35.0, 34.5, 34.0, 33.5, 33.0, 32.5, 32.0, 31.5, 31.0, 30.5, 30.0, 29.5,
      29.0, 28.5, 28.0, 27.5, 27.0, 26.5, 26.0, 25.5, 25.0, 24.5, 24.0, 23.5, 23.0,
      22.5, 22.0, 21.5, 21.0, 20.5, 20.0, 19.5, 19.0, 18.5, 18.0, 17.5, 17.0, 16.5,
      16.0, 15.5, 15.0, 14.5, 14.0, 13.5, 13.0, 12.5, 12.0, 11.5, 11.0, 10.5, 10.0,
      9.5, 9.0, 8.5, 8.0, 7.5, 7.0, 6.5, 6.0, 5.5, 5.0, 4.5, 4.0, 3.5, 3.0, 2.5, 2.0,
      1.5, 1.0, 0.5, 0.0, -0.5, -1.0, -1.5, -2.0, -2.5, -3.0, -3.5, -4.0, -4.5, -5.0,
      -5.5, -6.0, -6.5, -7.0, -7.5, -8.0, -8.5, -9.0, -9.5, -10.0, -10.5, -11.0, -11.5,
      -12.0, -12.5, -13.0, -13.5, -14.0, -14.5, -15.0, -15.5, -16.0, -16.5, -17.0, -17.5,
      -18.0, -18.5, -19.0, -19.5, -20.0, -20.5, -21.0, -21.5, -22.0, -22.5, -23.0, -23.5,
      -24.0, -24.5, -25.0, -25.5, -26.0, -26.5, -27.0, -27.5, -28.0, -28.5, -29.0, -29.5,
      -30.0, -30.5, -31.0, -31.5, -32.0, -32.5, -33.0, -33.5, -34.0, -34.5, -35.0, -35.5,
      -36.0, -36.5, -37.0, -37.5, -38.0, -38.5, -39.0, -39.5, -40};

    float flighty[M] = {3.120000000000001, 4.439009343750001, 5.708869500000002, 6.9308168437500015,
      8.106072000000001, 9.23583984375, 10.321309500000002, 11.36365434375, 12.364032000000002,
      13.32358434375, 14.2434375, 15.124701843750001, 15.968472, 16.77582684375, 17.5478295,
      18.28552734375, 18.989952000000002, 19.66211934375, 20.3030295, 20.91366684375, 21.495, 22.04798184375,
      22.5735495, 23.07262434375, 23.546112, 23.99490234375, 24.4198695, 24.82187184375, 25.201752,
      25.56033684375, 25.8984375, 26.21684934375, 26.516352, 26.79770934375, 27.0616695, 27.30896484375,
      27.540312, 27.75641184375, 27.9579495, 28.14559434375, 28.32, 28.48180434375, 28.6316295, 28.77008184375,
      28.897752, 29.01521484375, 29.1230295, 29.22173934375, 29.311872, 29.39393934375, 29.4684375, 29.53584684375,
      29.596632, 29.65124184375, 29.7001095, 29.74365234375, 29.782272, 29.81635434375, 29.8462695, 29.87237184375,
      29.895, 29.91447684375, 29.9311095, 29.94518934375, 29.956992, 29.96677734375, 29.9747895, 29.98125684375,
      29.986392, 29.99039184375, 29.9934375, 29.99569434375, 29.997312, 29.99842434375, 29.9991495, 29.99958984375,
      29.999832, 29.99994684375, 29.9999895, 29.99999934375, 30.0, 29.99999934375, 29.9999895, 29.99994684375,
      29.999832, 29.99958984375, 29.9991495, 29.99842434375, 29.997312, 29.99569434375, 29.9934375, 29.99039184375,
      29.986392, 29.98125684375, 29.9747895, 29.96677734375, 29.956992, 29.94518934375, 29.9311095, 29.91447684375,
      29.895, 29.87237184375, 29.8462695, 29.81635434375, 29.782272, 29.74365234375, 29.7001095, 29.65124184375,
      29.596632, 29.53584684375, 29.4684375, 29.39393934375, 29.311872, 29.22173934375, 29.1230295, 29.01521484375,
      28.897752, 28.77008184375, 28.6316295, 28.48180434375, 28.32, 28.14559434375, 27.9579495, 27.75641184375,
      27.540312, 27.30896484375, 27.0616695, 26.79770934375, 26.516352, 26.21684934375, 25.8984375, 25.56033684375,
      25.201752, 24.82187184375, 24.4198695, 23.99490234375, 23.546112, 23.07262434375, 22.5735495, 22.04798184375,
      21.495, 20.91366684375, 20.3030295, 19.66211934375, 18.989952000000002, 18.28552734375, 17.5478295,
      16.77582684375, 15.968472, 15.124701843750001, 14.2434375, 13.32358434375, 12.364032000000002, 11.36365434375,
      10.321309500000002, 9.23583984375, 8.106072000000001, 6.9308168437500015, 5.708869500000002, 4.439009343750001,
      3.120000000000001};

    if(shipTime < M)
    {
      shipz = flightz[shipTime];
      shipy = flighty[shipTime];
      shipTime++;
    }
    glutPostRedisplay();
    glutTimerFunc(30, launchShip, 0);

    glFlush();
}

//--Timer for the animation of the machine in the castle--------------------------
void machineTimer(int value)
{
    if (theta3 >= 360) {
        theta3 = 0;
    }
    else {
        theta3 += 2;
    }
    glutPostRedisplay();
    glutTimerFunc(30, machineTimer, 0);

    glFlush();
}
//--Special key interactions------------------------------------------------------
void special(int key, int x, int y)
{
    if(key == GLUT_KEY_LEFT) angle -= rotation_speed;  //Change direction
    else if(key == GLUT_KEY_RIGHT) angle += rotation_speed;
    else if(key == GLUT_KEY_DOWN)
    {  //Move backward
        eye_x -= move_speed*sin(angle);
        eye_z += move_speed*cos(angle);
    }
    else if(key == GLUT_KEY_UP)
    { //Move forward
        eye_x += move_speed*sin(angle);
        eye_z -= move_speed*cos(angle);
    }
    else if(key == GLUT_KEY_PAGE_UP)
    {//increase camera height
        eye_y += move_speed;
    }
    else if(key == GLUT_KEY_PAGE_DOWN)
    {//decrease camera height
        eye_y -= move_speed;
    }

    look_x = eye_x + 100*sin(angle);
    look_z = eye_z - 100*cos(angle);
    glutPostRedisplay();
}
void keyHandler(unsigned char key, int x, int y)
{
    if (key == 'c')
    {
        glutTimerFunc(30, fireCannon, 0);
    }
    if (key == 'x')
    {
      shipTime = 0;
      glutTimerFunc(30, launchShip, 0);
    }

    glutPostRedisplay();
}
//--------------------------------------------------------------------------------
void initialize(void)
{
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};

    q = gluNewQuadric();
    loadMeshFile("Cannon.off");
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 50);
//	Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    gluQuadricDrawStyle(q, GLU_FILL);
    glClearColor (1., 1., 1., 1.);  //Background colour

    glEnable(GL_TEXTURE_2D);
      gluQuadricTexture(q, GL_TRUE);
      loadTexture();
    glDisable(GL_TEXTURE_2D);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(80.0, 1.0, 1.0, 2000.0);  //Perspective projection

    glFlush();
}
//--------------------------------------------------------------------------------
void display(void)
{
   float lgt_pos[] = {0.0f, 50.0f, 0.0f, 1.0f};  //light0 position (directly above the origin)
   glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);   //light position
   glLoadIdentity();
   gluLookAt(eye_x, eye_y, eye_z,  look_x, look_y, look_z,   0, 1, 0);

   glEnable(GL_TEXTURE_2D);
    floor();
    skybox();
    drawMuseumShell();
    drawExhibits();
   glDisable(GL_TEXTURE_2D);

   glutSwapBuffers();

   glFlush();
}
//--------------------------------------------------------------------------------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
   glutInitWindowSize (800, 800);
   glutInitWindowPosition (50, 50);
   glutCreateWindow ("OpenGL Museum");
   initialize();
   glutDisplayFunc(display);
   glutTimerFunc(30, machineTimer, 0);
   glutSpecialFunc(special);
   glutKeyboardFunc(keyHandler);
   glutMainLoop();
   return 0;
}
//--------------------------------------------------------------------------------
