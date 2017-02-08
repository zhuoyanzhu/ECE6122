// Distributed two-dimensional Discrete FFT transform
// YOUR NAME HERE
// ECE8893 Project 1


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>

#include "Complex.h"
#include "InputImage.h"

using namespace std;

#define PI 3.14159265
#define NCPUS 16;
#define NPROC 16;
void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  // 1) Use the InputImage object to read in the Tower.txt file and
  //    find the width/height of the input image.
  // 2) Use MPI to find how many CPUs in total, and which one
  //    this process is
  // 3) Allocate an array of Complex object of sufficient size to
  //    hold the 2d DFT results (size is width * height)
  // 4) Obtain a pointer to the Complex 1d array of input data
  // 5) Do the individual 1D transforms on the rows assigned to your CPU
  // 6) Send the resultant transformed values to the appropriate
  //    other processors for the next phase.
  // 6a) To send and receive columns, you might need a separate
  //     Complex array of the correct size.
  // 7) Receive messages from other processes to collect your columns
  // 8) When all columns received, do the 1D transforms on the columns
  // 9) Send final answers to CPU 0 (unless you are CPU 0)
  //   9a) If you are CPU 0, collect all values from other processors
  //       and print out with SaveImageData().
  InputImage image(inputFN);  // Create the helper object for reading the image
  // Step (1) in the comments is the line above.
  // Your code here, steps 2-9
  int w = image.GetWidth();
  int h = image.GetHeight();
  Complex* data = image.GetImageData();
  int rank, nums;
  
  MPI_Status stat;
  MPI_Datatype complextype, oldtype[1];
  MPI_Aint offset[1];
  int blockcount[1];
  MPI_Comm_size(MPI_COMM_WORLD,&nums);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  offset[0] = 0;
  oldtype[0] = MPI_DOUBLE;
  blockcount[0] = 2;
  MPI_Type_create_struct(1, blockcount, offset, oldtype, &complextype);
  MPI_Type_commit(&complextype);
  
  if (nums != NCPUS) 
  {
     printf("Must specify MP_PROCS= %d. Terminating.\n",NCPUS);
     MPI_Finalize();
     exit(0);
  }
  int number1 = h / NPROC;
  int number2 = w / NPROC;
  if(rank != 0)
  {
	  Complex* tmp = new Complex[w * number1];
	  Complex* res1 = new Complex[number2 * h];
	  Complex* res2 = new Complex[number2 * h];
	  for(int i = 0; i < number1; ++i)
	  {
		  Transform1D(data + rank * w * number1 + i * w, w, tmp + i * w);
	  }
	  MPI_Send(tmp, w * number1, complextype, 0, rank, MPI_COMM_WORLD);
	  
	  MPI_Receive(res1, h * number2, complextype, 0, rank + NPROC, MPI_COMM_WORLD, &stat);
	  for(int i = 0; i < number2; ++i)
	  {
		  Transform1D(res1 + i * h, h, res2 + i * h);
	  }
	  
	  
	  MPI_Request reqs;
	  MPI_Isend(res2, h * number2, complextype, 0, rank + 2 * NPROC, MPI_COMM_WORLD, &reqs);
	  MPI_Wait(&reqs, &stat);
  } 
  else
  {
	  Complex* tmpData = new Complex[w * h];
	  Complex* resData = new Complex[w * h];
	  Complex temp;
	  for(int i = 0; i < number1; ++i)
	  {
		  Transform1D(data + i * w, w, tmpData + i * w);
	  }
	  for(int i = 1; i < NPROC; ++i)
	  {
		  MPI_Receive(tmpData + i * w * number1, w * number1, complextype, i, i, MPI_COMM_WORLD, &Stat);
	  }
	  MPI_Barrier(MPI_COMM_WORLD);
	  string s1 = "MyAfter1D.txt";
	  image.SaveImageData(s1.c_str(), tmpData, w, h);
	  for(int i = 0; i < h; ++i)
	  {
		  for(int j = 0; j < w; ++j)
		  {
			  resData[j * h + i] = tmpData[i * w + j];
		  }
	  }
	  
	  for(int i = 1; i < NPROC; ++i)
	  {
		  MPI_Send(resData + i * h * number2, h * number2, complextype, i, i + NPROC, MPI_COMM_WORLD);
	  }
	  MPI_Barrier(MPI_COMM_WORLD);
	  
	  for(int i = 0; i < number2; ++i)
	  {
		  Transform1D(tmpData + i * h, h, resData + i * h);
	  }
	  
	  for(int i = 1; i < NPROC; ++i)
	  {
		  MPI_Receive(tmpData + i * h * number2, h * number2, complextype, i, i + 2 * NPROC, MPI_COMM_WORLD, &Stat);
	  }
	  MPI_Barrier(MPI_COMM_WORLD);
	  for(int i = 0; i < w; ++i)
	  {
		  for(int j = 0; j < h; ++j)
		  {
				resData[j * w + i] = tmpData[i * h + j];
		  }
	  }
	  string s2 = "MyAfter2D.txt";
	  image.SaveImageData(s2.c_str(), resData, w, h);
  }
  MPI_Type_free(&complextype);
  free(data);
  free(tmpData);
  free(resData);
}

void Transform1D(Complex* h, int w, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
  Complex W = Complex(cos(2 * PI / w), -sin(2 * PI / w));
  Complex WW = Complex(1.0), tmp;
  for(int i = 0; i < w; ++i)
  {
	  H[i] = Complex();
	  tmp = Complex(1.0);
	  for(int j = 0; j < w; ++j)
	  {
		  H[i] = H[i] + h[j] * tmp;
		  tmp = tmp * WW;
	  }
	  WW = WW * W;
  }
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  int rc = MPI_Init(&argc,&argv);
  if(rc != MPI_SUCCESS)
  {
	  printf("The MPI system does not start correctly!\n");
	  MPI_Abort(MPI_COMM_WORLD, rc);
  }
  Transform2D(fn.c_str()); // Perform the transform.
  // Finalize MPI here
  MPI_Finalize();
}  
  

  
