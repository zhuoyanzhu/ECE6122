// Threaded two-dimensional Discrete FFT transform
// YOUR NAME HERE
// ECE8893 Project 2


#include <iostream>
#include <string>
#include <math.h>
#include "pthread.h"
#include "Complex.h"
#include "InputImage.h"

// You will likely need global variables indicating how
// many threads there are, and a Complex* that points to the
// 2d image being transformed.

using namespace std;
#define NTHREADS 16

Complex *h;
int N;
pthread_mutex_t mutex, countMutex;


// Function to reverse bits in an unsigned integer
// This assumes there is a global variable N that is the
// number of points in the 1D transform.
unsigned ReverseBits(unsigned v)
{ //  Provided to students
	unsigned n = N; // Size of array (which is even 2 power k value)
	unsigned r = 0; // Return value
	for (--n; n > 0; n >>= 1)
		{
		r <<= 1;        // Shift return value
		r |= (v & 0x1); // Merge in next bit
		v >>= 1;        // Shift reversal value
		}
	return r;
}

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.
int P, count;
bool global;
bool local[NTHREADS];

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
void SWAP(Complex &a, Complex &b)
{
    Complex temp;
    temp = a;
    a = b;
    b = temp;
}
void Inverse_transform1D(Complex* h, int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)
	Complex W[N];
	for(int i = 0; i < N; ++i)
	{
		int r = ReverseBits(i);
			if(i < r)
				SWAP(h[i], h[r]);
			if(i < N /2)
			{
				W[i] = Complex(cos((2 * M_PI * i)/ N), sin((2 * M_PI * i )/ N));
				W[i + N / 2]  = W[i] * -1;
			}
	}
	Complex *tmp;
	int times = log2(N);
	for(int i = 1; i <= times; ++i)
	{
		int step = pow(2, i);
		tmp = new Complex[step];
		for(int j = 0; j < N; j += step)
		{
		    for(int k = 0; k < step; k++)
                tmp[k] = h[j + k];
			for(int k = j; k < j + step; k++)
			{
			    int dis = step / 2;
				h[k] = tmp[(k - j) % dis] + W[(N / step * k) % N] * tmp[step / 2 + (k - j) % dis];
			}
		}
	}
	delete[] tmp;
}            
void Transform1D(Complex* h, int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)
	Complex W[N];
	for(int i = 0; i < N; ++i)
	{
		int r = ReverseBits(i);
			if(i < r)
				SWAP(h[i], h[r]);
			if(i < N /2)
			{
				W[i] = Complex(cos((2 * M_PI * i)/ N), -sin((2 * M_PI * i )/ N));
				W[i + N / 2]  = W[i] * -1;
			}
	}
	Complex *tmp;
	int times = log2(N);
	for(int i = 1; i <= times; ++i)
	{
		int step = pow(2, i);
		tmp = new Complex[step];
		for(int j = 0; j < N; j += step)
		{
		    for(int k = 0; k < step; k++)
                tmp[k] = h[j + k];
			for(int k = j; k < j + step; k++)
			{
			    int dis = step / 2;
				h[k] = tmp[(k - j) % dis] + W[(N / step * k) % N] * tmp[step / 2 + (k - j) % dis];
			}
		}
	}
	delete[] tmp;
}

void* Transform2DTHread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  // Calculate 1d DFT for assigned rows
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
  	pthread_mutex_lock(&mutex);
	unsigned long myID = (unsigned long)v;
	unsigned number = N / NTHREADS;
	for(unsigned i = 0; i < number; ++i)
	{
		Transform1D(h + myID * number * N + i * N, N);
	}
	pthread_mutex_unlock(&mutex);
	MyBarrier(myID);
	return 0;
}
void* Inverse_transform2D(void* v)
{ // This is the thread startign point.  "v" is the thread number
  // Calculate 1d DFT for assigned rows
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
    pthread_mutex_lock(&mutex);
	unsigned long myID = (unsigned long)v;
	unsigned number = N / NTHREADS;
	for(unsigned i = 0; i < number; ++i)
	{
		Inverse_transform1D(h + myID * number * N + i * N, N);
	}
	pthread_mutex_unlock(&mutex);
	MyBarrier(myID);
	return 0;
}
void TransposeInPlace(Complex* h, int N)
{
	Complex tmp;
	for(int i = 0; i < N; ++i)
			for(int j = i; j < N; ++j)
			{
				tmp = h[i * N + j];
				h[i * N + j] = h[j * N + i];
				h[j * N + i] = tmp;
			}			
}

void Spec_TransposeInPlace(Complex* h, int N)
{
	Complex tmp;
	for(int i = 0; i < N; ++i)
			for(int j = i; j < N; ++j)
			{
				tmp = h[i * N + j];
				h[i * N + j] = h[j * N + i] * (1.0 /(N * N));
				h[j * N + i] = tmp * (1.0 /(N * N));
			}	
}
void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
	InputImage image(inputFN);  // Create the helper object for reading the image
  // Create the global pointer to the image array data
  // Create 16 threads
  // Wait for all threads complete
  // Write the transformed data
    N = image.GetWidth();
	h = image.GetImageData();
	pthread_t threads[NTHREADS];
	pthread_mutex_init(&mutex, 0);
	MyBarrier_Init(NTHREADS);
	for(int i = 0; i < NTHREADS; ++i)
	{
		pthread_create(threads + i, 0, Transform2DTHread, (void*)i);
	}
	for(int i = 0; i < NTHREADS; ++i)
		pthread_join(*(threads + i), NULL);
	string s = "MyAfter1D.txt";
	image.SaveImageData(s.c_str(), h, N, N);
	
	TransposeInPlace(h, N);
	
	for(int i = 0; i < NTHREADS; ++i)
	{
		pthread_create(threads + i, 0, Transform2DTHread, (void*)i);
	}
	for(int i = 0; i < NTHREADS; ++i)
		pthread_join(*(threads + i), NULL);
	TransposeInPlace(h, N);
	
	s = "MyAfter2D.txt";
	image.SaveImageData(s.c_str(), h, N, N);
	
	for(int i = 0; i < NTHREADS; ++i)
	{
		pthread_create(threads + i, 0, Inverse_transform2D, (void*)i);
	}
	for(int i = 0; i < NTHREADS; ++i)
		pthread_join(*(threads + i), NULL);
	TransposeInPlace(h, N);
	
	for(int i = 0; i < NTHREADS; ++i)
	{
		pthread_create(threads + i, 0, Inverse_transform2D, (void*)i);
	}
	for(int i = 0; i < NTHREADS; ++i)
		pthread_join(*(threads + i), NULL);
	Spec_TransposeInPlace(h, N);
	
	s = "MyAfterInverse.txt";
	image.SaveImageDataReal(s.c_str(), h, N, N);
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  Transform2D(fn.c_str()); // Perform the transform.
}  
  

  
