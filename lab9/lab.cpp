/*
   CPE 471 Assignment 1
   Lighting.cpp
 */

#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLSL_helper.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>

#include <math.h>
#define PI 3.14159

using namespace std;
using namespace glm;

GLint h_aPosition;
GLint h_aNormal;
GLint h_uModelMatrix;
GLint h_uViewMatrix;
GLint h_uProjMatrix;
GLint h_uColor;
GLuint vbo_cube;
GLuint ibo_cube;
GLuint vbo_grnd;
GLuint ibo_grnd;

int g_size, c_size;

int ShadeProg;

float g_height;
float g_width;

float g_angle = 0;
vec3 g_trans(0,0,0);

float g_transx[4] = {0.0, -3.0, 0.0, 3.0};
float g_transy[4] = {0.0, 0.0, 0.0, 0.0};
float g_transz[4] = {3.0, 0.0, -3.0, 0.0};
float g_color[4][3] = {{1.0, 0.0, 0.0},
   {0.0, 1.0, 0.0},
   {0.0, 0.0, 1.0},
   {1.0, 1.0, 0.0}};

bool g_mousedown;
float g_basex = 0;
float g_basey = 0;

float obeta = (PI/2.0);
float oalpha = 0;
float beta = 0;
float alpha = 0;

vec3 eyePos(0,0,0);
vec3 lookAtPos(0,0,1);
vec3 olookAtPos(0,0,1);
vec3 up(0,1,0);

vec3 w(0,0,0);
vec3 u(0,0,0);
vec3 v(0,0,0);

void SetProjectionMatrix()
{
   glm::mat4 Projection = glm::perspective(80.0f, (float)g_width/g_height, 0.1f, 100.f);
   safe_glUniformMatrix4fv(h_uProjMatrix, glm::value_ptr(Projection));
}

void SetView()
{
   glm::mat4 lookAt = glm::lookAt(eyePos, lookAtPos, up);

   safe_glUniformMatrix4fv(h_uViewMatrix, glm::value_ptr(lookAt));
}

void drawGround()
{
   safe_glEnableVertexAttribArray(h_aPosition);

   safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(mat4(1.0f)));
   glUniform3f(h_uColor, 0.7, 0.98, 0.9);

   // bind vbo
   glBindBuffer(GL_ARRAY_BUFFER, vbo_grnd);
   safe_glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // draw!
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_grnd);
   glDrawElements(GL_TRIANGLES, g_size, GL_UNSIGNED_INT, 0);

   // Disable the attributes used by our shader
   safe_glDisableVertexAttribArray(h_aPosition);
}

void drawCubes()
{
   safe_glEnableVertexAttribArray(h_aPosition);

   glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
   safe_glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube);

   for(int i = 0; i < 4; i++)
   {
      mat4 TM = translate(mat4(1.0f), vec3(g_transx[i], g_transy[i], g_transz[i]));
      safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(mat4(TM)));

      glUniform3f(h_uColor, g_color[i][0], g_color[i][1], g_color[i][2]);
      glDrawElements(GL_TRIANGLES, c_size, GL_UNSIGNED_INT, 0);
   }

   safe_glDisableVertexAttribArray(h_aPosition);

}

int InstallShader(const GLchar *vShaderName, const GLchar *fShaderName)
{
   GLuint VS; //handles to shader object
   GLuint FS; //handles to frag shader object
   GLint vCompiled, fCompiled, linked; //status of shader

   VS = glCreateShader(GL_VERTEX_SHADER);
   FS = glCreateShader(GL_FRAGMENT_SHADER);

   glShaderSource(VS, 1, &vShaderName, NULL);
   glShaderSource(FS, 1, &fShaderName, NULL);

   //compile shader and print log
   glCompileShader(VS);
   /* check shader status requires helper functions */
   printOpenGLError();
   glGetShaderiv(VS, GL_COMPILE_STATUS, &vCompiled);
   printShaderInfoLog(VS);

   //compile shader and print log
   glCompileShader(FS);
   /* check shader status requires helper functions */
   printOpenGLError();
   glGetShaderiv(FS, GL_COMPILE_STATUS, &fCompiled);
   printShaderInfoLog(FS);

   if (!vCompiled || !fCompiled) {
      printf("Error compiling either shader\n\n%s\n\nor\n\n%s\n\n", vShaderName, fShaderName);
      return -1;
   }

   //create a program object and attach the compiled shader
   ShadeProg = glCreateProgram();
   glAttachShader(ShadeProg, VS);
   glAttachShader(ShadeProg, FS);

   glLinkProgram(ShadeProg);
   /* check shader status requires helper functions */
   printOpenGLError();
   glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
   //printProgramInfoLog(ShadeProg);

   glUseProgram(ShadeProg);

   /* get handles to attribute data */
   h_aPosition = safe_glGetAttribLocation(ShadeProg, "aPosition");
   h_uColor = safe_glGetUniformLocation(ShadeProg,  "uColor");
   h_uProjMatrix = safe_glGetUniformLocation(ShadeProg, "uProjMatrix");
   h_uViewMatrix = safe_glGetUniformLocation(ShadeProg, "uViewMatrix");
   h_uModelMatrix = safe_glGetUniformLocation(ShadeProg, "uModelMatrix");
   //printf("sucessfully installed shader %d\n", ShadeProg);
   return 1;
}

void Draw(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glUseProgram(ShadeProg);

   SetProjectionMatrix();
   SetView();

   drawGround();
   drawCubes();

   glUseProgram(0);

   glutSwapBuffers();
}

void initGround(void) {

   float vbo[] = {
      -100, -10, -100,
      -100, -10, 100,
      100, -10, 100,
      100, -10, -100
   };

   unsigned int ibo[] = {
      0, 1, 2, 0, 2, 3
   };

   g_size = 6;
   glGenBuffers(1, &vbo_grnd);
   glBindBuffer(GL_ARRAY_BUFFER, vbo_grnd);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vbo), vbo, GL_STATIC_DRAW);

   glGenBuffers(1, &ibo_grnd);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_grnd);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ibo), ibo, GL_STATIC_DRAW);
}


void initCube() {

   float vbo[] = {
      -0.1, -0.1, -0.1,
      -0.1, 0.1, -0.1,
      0.1, 0.1, -0.1,
      0.1, -0.1, -0.1,
      0.0, 0.1, -0.1,
      -0.1, -0.1, 0.1,
      -0.1, 0.1, 0.1,
      0.1, 0.1, 0.1,
      0.1, -0.1, 0.1,
      0.0, 0.1, 0.1,
      -0.1, -0.1, 0.1,
      -0.1, -0.1, -.1,
      -0.1, 0.1, -0.1,
      -0.1, 0.1, 0.1,
      0.1, -0.1, 0.1,
      0.1, -0.1, -.1,
      0.1, 0.1, -0.1,
      0.1, 0.1, 0.1,
   };

   unsigned int ibo[] = {
      0, 1, 2,
      2, 3, 0,
      1, 4, 2,
      5, 6, 7,
      7, 8, 5,
      6, 9, 7,
      10, 11, 12,
      12, 13, 10,
      14, 15, 16,
      16, 17, 14,
   };

   c_size = 30;
   glGenBuffers(1, &vbo_cube);
   glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vbo), vbo, GL_STATIC_DRAW);

   glGenBuffers(1, &ibo_cube);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ibo), ibo, GL_STATIC_DRAW);
};

void ReshapeGL(int width, int height)
{
   g_width = (float)width;
   g_height = (float)height;
   glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));
}

void Initialize()
{
   glClearColor(1.0, 1.0, 1.0, 1.0f);

   glClearDepth(1.0f);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);
}


void computeBasis()
{
   vec3 temp = lookAtPos - eyePos;
   w = normalize(-temp);
   u = normalize(cross(up,w));
   v = normalize(cross(w,u));
}

void keyboard(unsigned char key, int x, int y)
{
   computeBasis();
   switch(key)
   {
   case('w'):
      eyePos = eyePos - w;
      lookAtPos = lookAtPos - w;
      break;
   case('a'):
      eyePos = eyePos - u;
      lookAtPos = lookAtPos - u;
      break;
   case('s'):
      eyePos = eyePos + w;
      lookAtPos = lookAtPos + w;
      break;
   case('d'):
      eyePos = eyePos + u;
      lookAtPos = lookAtPos + u;
      break;
   }
   glutPostRedisplay();
}

void motion(int x, int y)
{
   if(g_mousedown)
   {
      int height = glutGet(GLUT_WINDOW_HEIGHT);
      int width = glutGet(GLUT_WINDOW_WIDTH);

      y = height - y;

      int xdiff = g_basex-x;
      int ydiff = g_basey-y;

      beta = obeta + ((float)xdiff/width)*PI;
      
      alpha = oalpha + ((float)ydiff/height)*PI;

      //cout << xdiff << " " << ydiff << endl;
      //cout << base_x << " " << base_y << endl;
      //cout << alpha << " " << beta << endl;

      float vx = 1*cos(alpha)*cos(beta);
      float vy = 1*sin(alpha);
      float vz = 1*cos(alpha)*cos((PI/2.0)-beta);


      lookAtPos = vec3(vx+eyePos.x,vy+eyePos.y,vz+eyePos.z);
      
      //cout << lookAtPos.x << " " << lookAtPos.y << " " << lookAtPos.z << endl;
   }
   
   glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
   int height = glutGet(GLUT_WINDOW_HEIGHT);
   if(state==0)
   {
      g_mousedown=true;
      g_basex = x;
      g_basey = height - y;
   } else if(state==1) {
      g_mousedown=false;
      obeta = beta;
      oalpha = alpha;

   }
   
   glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitWindowPosition(20, 20);
   glutInitWindowSize(600, 600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutCreateWindow("Lighting");
   glutReshapeFunc(ReshapeGL);
   glutDisplayFunc(Draw);
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   Initialize();

   getGLversion();

   if(!(InstallShader(textFileRead((char*)"vert.glsl"), textFileRead((char*)"frag.glsl"))))
   {
      perror("Error installing shader");
      exit(-1);
   }

   initGround();
   initCube();

   glutMainLoop();
   return 0;
}



