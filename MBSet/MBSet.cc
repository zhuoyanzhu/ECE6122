// Calculate and display the Mandelbrot set
// ECE4893/8893 final project, Fall 2011

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stack>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "complex.h"

#define WINDOW_DIM 512
#define NUM_THREADS 16
using namespace std;

pthread_mutex_t countMutex;
pthread_t threads[NUM_THREADS];

bool clicked = false;
bool select_ready = false;
int win;
// Min and max complex plane values
Complex  minC(-2.0, -1.2);
Complex  maxC( 1.0, 1.8);
int      maxIt = 2000;     // Max iterations for the set computations

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.
int P, count;
bool global;
bool local[NUM_THREADS];
int fetch()
{
	pthread_mutex_lock(&countMutex);
	int myCount = count;
	count--;
	pthread_mutex_unlock(&countMutex);
	return myCount;
}
// Call MyBarrier_Init once in main
void MyBarrier_Init(int num)// you will likely need some parameters)
{
	P = num;
	count = num;
	pthread_mutex_init(&countMutex, 0);
	for(int i = 0; i < P; ++i)
		local[i] = true;
	global = true;
}

// Each thread calls MyBarrier after completing the row-wise DFT
void MyBarrier(int id) // Again likely need parameters
{
	local[id] = !local[id];
	if(fetch() == 1)
	{
		count = P;
		global = local[id];
	}
	else
	{
		while(global != local[id]) { }
	}
}


struct Position {
	Position() : x(0), y(0) {}
	float x,y;
};
Position start, end;

struct Memory 
{
	Memory(float a, float b, float c, float d) : minC_r(a), minC_i(b), maxC_r(c), maxC_i(d) {}
	float minC_r, minC_i, maxC_r, maxC_i;
};
stack<Memory> memory_stack;

class RGB
{
public:
	RGB(): r(0), g(0), b(0) {}
	RGB(double r0, double g0, double b0): r(r0), g(g0), b(b0) {}
	double r;
	double g;
	double b;
};
RGB* colors = NULL;

Complex* complexs = new Complex[WINDOW_DIM * WINDOW_DIM];
int iteration[WINDOW_DIM * WINDOW_DIM]; 

void* ComputeMBSet(void* i)
{
	unsigned long id = (unsigned long)i;
	int num = WINDOW_DIM / NUM_THREADS;
	for(unsigned i = num * id; i < num * id + num; ++i)
		for(unsigned j = 0; j < WINDOW_DIM; ++j)
		{
			unsigned index = i * WINDOW_DIM + j;
			iteration[index] = 0;
			double diff1 = maxC.real - minC.real;
			double diff2 = maxC.imag - minC.imag;
			Complex current = Complex(minC.real + diff1*double(i)/double(WINDOW_DIM), minC.imag + diff2*double(j)/double(WINDOW_DIM));
			complexs[index] = current;
			Complex c = Complex(current);
			while(iteration[index] < 2000 && current.Mag2() < 4.0)
			{
				current = current * current + c;
				iteration[index]++;
			}
		}
	MyBarrier(id);
}

void ComputeMBSet() 
{
	for(int i = 0; i < NUM_THREADS; ++i)
		pthread_create(threads + i, 0, ComputeMBSet, (void*)i);
	for(int i = 0; i < NUM_THREADS; ++i)
		pthread_join(*(threads + i), NULL);
}


void lineLoop()
{
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(start.x, start.y);
	glVertex2f(end.x, start.y);
	glVertex2f(end.x, end.y);
	glVertex2f(start.x, end.y);
	glEnd();
}

void display(void)
{ 
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glBegin(GL_POINTS);
	for(int i = 0; i < WINDOW_DIM; ++i)
	{
		for(int j = 0; j < WINDOW_DIM; ++j)
		{
			glColor3f(colors[iteration[i*WINDOW_DIM + j]].r, colors[iteration[i*WINDOW_DIM + j]].g, colors[iteration[i*WINDOW_DIM + j]].b);
			glVertex2d(i, j);
		}
	}
	glEnd();
	if (clicked){
		cout << "draw" << endl;
		lineLoop();
	}
	glFinish();
	glutSwapBuffers();
}



void InitializeColors()
{
	colors = new RGB[maxIt + 1];
	for (int i = 0; i < maxIt; ++i)
		{
		if (i < 5)
		{ 
			colors[i] = RGB(1, 1, 1);
		}
		else
        {
			colors[i] = RGB((double) rand() / RAND_MAX, (double) rand() / RAND_MAX, (double) rand() / RAND_MAX);
        }
    }
	colors[maxIt] = RGB(); 
}


void init()
{ // Your OpenGL initialization code here
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glViewport(0, 0, WINDOW_DIM, WINDOW_DIM);                                            
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_DIM, WINDOW_DIM, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
	InitializeColors();
	MyBarrier_Init(NUM_THREADS);
}

void reshape(int w, int h)
{ // Your OpenGL window reshape code here

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
}

void mouse(int button, int state, int x, int y)
{ // Your mouse click processing here
  // state == 0 means pressed, state != 0 means released
  // Note that the x and y coordinates passed in are in
  // PIXELS, with y = 0 at the top.
	if(button == GLUT_LEFT_BUTTON && state == 0) 
	{
		start.x = x;
		start.y = y;
		end.x = x;
		end.y = y;
		clicked = true;
	}
	else if(button == GLUT_LEFT_BUTTON && state != 0)                      
	{
		glutPostRedisplay();
		memory_stack.push(Memory(minC.real, minC.imag, maxC.real, maxC.imag));   
		double dx = abs(x - start.x);
		double dy = abs(y - start.y);
		double ds = dx > dy ? dy : dx;
		end.x = x > start.x ? start.x + ds : start.x - ds;
		end.y = y > start.y ? start.y + ds : start.y - ds;
		int min_i = min(start.x, end.x), min_j = min(start.y, end.y);
		minC.real = complexs[min_i*WINDOW_DIM + min_j].real;
		minC.imag = complexs[min_i*WINDOW_DIM + min_j].imag;
    
		int max_i = max(start.x, end.x), max_j = max(start.y, end.y);
		maxC.real = complexs[max_i*WINDOW_DIM + max_j].real;
		maxC.imag = complexs[max_i*WINDOW_DIM + max_j].imag;
		ComputeMBSet();
		clicked = false;
		glutPostRedisplay();
	}
}

void motion(int x, int y)
{ // Your mouse motion here, x and y coordinates are as above
	if (clicked){
		end.x = x;
		end.y = y;
		glutPostRedisplay();
  }
}

void keyboard(unsigned char c, int x, int y)
{ // Your keyboard processing here
	if(c == 'q')
		exit(0);
	else if(c == 'b')
	{
		if(memory_stack.size() > 0) {
			Memory temp = memory_stack.top();
			memory_stack.pop();
			minC.real = temp.minC_r;
			minC.imag = temp.minC_i;
			maxC.real = temp.maxC_r;
			maxC.imag = temp.maxC_i;
			ComputeMBSet(); 
			glutPostRedisplay();
		}
		else 
			cout << "You can not go back any more!" << endl;
	}
}

int main(int argc, char** argv)
{
	// Initialize OpenGL, but only on the "master" thread or process.
	// See the assignment writeup to determine which is "master" 
	// and which is slave.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_DIM, WINDOW_DIM);
	glutInitWindowPosition(100, 100);
	win = glutCreateWindow("MBSet");

	init();
	ComputeMBSet();
	glutDisplayFunc(display);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(display);
	glutMainLoop();
	return 0;
}

