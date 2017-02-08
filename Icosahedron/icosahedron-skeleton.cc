// Draw an Icosahedron
// ECE4893/8893 Project 4
// YOUR NAME HERE

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#define NFACE 20
#define NVERTEX 12

#define X .525731112119133606 
#define Z .850650808352039932

// These are the 12 vertices for the icosahedron
static GLfloat vdata[NVERTEX][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[NFACE][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

int testNumber; // Global variable indicating which test number is desired
int depth;
static float updateRate = 10.0;
float *rVal, *bVal, *gVal;
GLfloat rot = 360.0;
void setColor(int number)
{
	srand(time(NULL));
	for(int i = 0; i < number; ++i)
	{
		rVal[i] = (float) rand() / RAND_MAX;
		bVal[i] = (float) rand() / RAND_MAX;
		gVal[i] = (float) rand() / RAND_MAX;
	}
}
void drawTriangle(GLfloat *v1, GLfloat *v2, GLfloat *v3, int n)
{
	glBegin(GL_TRIANGLES);
	glColor3f(rVal[n], bVal[n], gVal[n]);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v3);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v3);
	glEnd();
}
void subDivide(GLfloat* v1, GLfloat* v2, GLfloat* v3, int d, int n)
{
	int de = depth - d;
	int i = pow(4 ,de)
	GLfloat v12[3];
	GLfloat v23[3];
	GLfloat v31[3];

	for(int i = 0; i < 3; i++)
	{
		v12[i] = v1[i] + v2[i];
		v23[i] = v2[i] + v3[i];
		v31[i] = v3[i] + v1[i];
	}
	normalize(v12);
	normalize(v23);
	normalize(v31);
	if(d == 1)
	{
		DrawTriangle(v1, v12, v31, 4*face*i);
		DrawTriangle(v12, v23, v2, 4*face*i+1);
		DrawTriangle(v31, v23, v3, 4*face*i+2);
		DrawTriangle(v12, v31, v23,4*face*i+3);
	}
	else
	{
		subDivide(v1, v12, v31, d - 1, n);
		subDivide(v12, v23, v2, d - 1, n);
		subDivide(v31, v23, v3, d - 1, n);
		subDivide(v31, v23, v3, d - 1, n);
	}
}

void normalize(GLfloat *v)
{
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / d;
	v[1] = v[1] / d;
	v[2] = v[2] / d;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_LINE_SMOOTH);

    glLineWidth(0.5); 
	
    switch(testNumber)
    {
      case 1:
        Test1();
        break;

      case 2:
        Test2();
        break;

      case 3:
        Test3();
        break;

      case 4:
        Test4();
        break;

      case 5:
        Test5(depth);
        break;

      case 6:
        Test6(depth);
        break;

      default:
        cout << "ERROR: Invalid testNumber." << endl;
        exit(3);
    }

	glutSwapBuffers(); // For double buffering 
}

void init()
{
	//select clearing (background) color
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_LINE_SMOOTH);
	glLoadIdentity(); 
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0, (GLsizei)w, (GLsizei)h);
}

void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000.0 / updateRate, timer, 0);
}

// Test cases.  Fill in your code for each test case
void Test1()
{
	glPushMatrix();
	for(int i = 0; i < NFACE; ++i)
	{
		drawTriangle(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], i);
	}
	glPopMatrix();
}

void Test2()
{
	static GLfloat rotX = 0.0;
	static GLfloat rotY = 0.0;
	glRotatef(rotX, 1.0, 0.0, 0.0); 
	glRotatef(rotY, 0.0, 1.0, 0.0); 
	rotX += 1.0;
	rotY += 1.0;

	if(rotX == rotLim)
		rotX = 0.0;

	if(rotY == rotLim)
		rotY = 0.0;

	Test1();
}

void Test3()
{
	glPushMatrix();
	for (int i = 0; i < NFACE; ++i) 
	{
		subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], 1, i);
	}
	glPopMatrix();
}

void Test4()
{
	static GLfloat rotX = 0.0;
	static GLfloat rotY = 0.0;
	glRotatef(rotX, 1.0, 0.0, 0.0); 
	glRotatef(rotY, 0.0, 1.0, 0.0); 
	rotX += 1.0;
	rotY += 1.0;

	if(rotX == rotLim)
		rotX = 0.0;

	if(rotY == rotLim)
		rotY = 0.0;

	Test3();
}

void Test5(int depth)
{
	int d = depth;
	glPushMatrix();

	glPushMatrix();
	for (int i = 0; i < NFACE; i++) 
	{
		subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], d, i);
	}
	glPopMatrix();
	glPopMatrix();
}

void Test6(int depth)
{
	int d = depth;
	static GLfloat rotX = 0.0;
	static GLfloat rotY = 0.0;
	glRotatef(rotX, 1.0, 0.0, 0.0); 
	glRotatef(rotY, 0.0, 1.0, 0.0); 
	rotX += 1.0;
	rotY += 1.0;

	if(rotX == rotLim)
		rotX = 0.0;

	if(rotY == rotLim)
		rotY = 0.0;

	Test5(d);
}

int main(int argc, char** argv)
{

	if (argc < 2)
    {
      std::cout << "Usage: icosahedron testnumber" << endl;
      exit(1);
    }
	// Set the global test number
	testNumber = atol(argv[1]);
	if(testNumber >= 3)
		depth = 1;
	else 
		depth = 0;
	if(testNumber > 4)
	{
		if(argc < 3)
		{
			cout << "Error : missing depth value";
			exit(2);
		}
		depth = atoi(argv[2]);
		if(depth > 5)
		{
			cout << "Error : depth can not be larger than 5";
			exit(3);
		}
	}
	glutInit(&argc, argv);
	int number = (int) NFACE * pow(4, depth);
	rVal = new float[number];
	bVal = new float[number];
	gVal = new float[number];
	setColor(number);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Icosahedron");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(1000.0 / updateRate, timer, 0);
	glutMainLoop();
	
	delete(bVal);
	delete(rVal);
	delete(gVal);
	
	return 0;
}

