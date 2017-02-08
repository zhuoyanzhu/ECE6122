// RSA Assignment for ECE4122/6122 Fall 2015

#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "RSA_Algorithm.h"

using namespace std;

// Implement the RSA_Algorithm methods here

// Constructor
RSA_Algorithm::RSA_Algorithm()
  : rng(gmp_randinit_default)
{
  // get a random seed for the random number generator
  int dr = open("/dev/random", O_RDONLY);
  if (dr < 0)
    {
      cout << "Can't open /dev/random, exiting" << endl;
      exit(0);
    }
  unsigned long drValue;
  read(dr, (char*)&drValue, sizeof(drValue));
  //cout << "drValue " << drValue << endl;
  rng.seed(drValue);
// No need to init n, d, or e.
}

// Fill in the remainder of the RSA_Algorithm methods
 void RSA_Algorithm::GenerateRandomKeyPair(size_t sz)
{
	mpz_class p;
	mpz_class q;
	int ret = 0, ret1, ret2;

		p = rng.get_z_bits(sz);
		q = rng.get_z_bits(sz);
		mpz_nextprime(p.get_mpz_t(), p.get_mpz_t());
		mpz_nextprime(q.get_mpz_t(), q.get_mpz_t());

	mpz_mul(n.get_mpz_t(), p.get_mpz_t(), q.get_mpz_t());
	mpz_class p0;
	mpz_class q0;
	mpz_class phi;
	mpz_class one = 1;
	mpz_sub(p0.get_mpz_t(), p.get_mpz_t(), one.get_mpz_t());
	mpz_sub(q0.get_mpz_t(), q.get_mpz_t(), one.get_mpz_t());
	mpz_mul(phi.get_mpz_t(), p0.get_mpz_t(), q0.get_mpz_t());
	mpz_class res;
	d = rng.get_z_bits(sz * 2);
	mpz_gcd(res.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
	while(mpz_cmp(d.get_mpz_t(), phi.get_mpz_t()) >= 0 || mpz_cmp(res.get_mpz_t(), one.get_mpz_t()) != 0)
	{
		d = rng.get_z_bits(sz * 2);
		mpz_gcd(res.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
	}
	mpz_invert(e.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());	
}
mpz_class RSA_Algorithm::Encrypt(mpz_class M)
{
	mpz_class res;
	mpz_powm(res.get_mpz_t(), M.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
	return res;
}
mpz_class RSA_Algorithm::Decrypt(mpz_class C)
{
	mpz_class res;
	mpz_powm(res.get_mpz_t(), C.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
	return res;
}
void RSA_Algorithm::PrintND()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << endl;
}

void RSA_Algorithm::PrintNE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " e " << e << endl;
}

void RSA_Algorithm::PrintNDE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << " e " << e << endl;
}

void RSA_Algorithm::PrintM(mpz_class M)
{ // Do not change this, right format for the grading script
  cout << "M " << M << endl;
}

void RSA_Algorithm::PrintC(mpz_class C)
{ // Do not change this, right format for the grading script
  cout << "C " << C << endl;
}




