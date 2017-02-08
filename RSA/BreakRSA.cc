// Grad student portion of the RSA assignment
// Fall 2015 ECE6122

#include <iostream>
#include <string>
#include "RSA_Algorithm.h"

using namespace std;

int main(int argc, char** argv)
{ // Arguments are as follows:
  //argv[1] = n;
  //argv[2] = e;  // n and e are the public key
  //argv[3] = first 6 characters of encrypted message
  //argv[4] = next 6 characters .. up to argv[12] are the lsat 6 characters
  // The number of arguments will always be exacty 12, and each argument past the
  // public key contain 6 ascii characters of the encrypted message.
  // Each of the 32bit values in the argv[] array are right justified in the
  // low order 48 bits of each unsigned long.  The upper 16 bits are always
  // zero, which insures the each unsigned long is < n (64 bits) and therefore
  // the RSA encryption will work.

  // Below is an example of the BreakRSA and command line arguments:

// ./BreakRSA  2966772883822367927 2642027824495698257  817537070500556663 1328829247235192134 1451942276855579785 2150743175814047358 72488230455769594 1989174916172335943 962538406513796755 1069665942590443121 72678741742252898 1379869649761557209

//   The corect output from the above is:
//   HelloTest  riley CekwkABRIZFlqmWTanyXLogFgBUENvzwHpEHRCZIKRZ
//
//   The broken (computed) private key for the above is 4105243553



  // Our one and only RSA_Algorithm object
  RSA_Algorithm rsa;
  // First "break" the keys by factoring n and computing d
  // Set the keys in the rsa object afer calculating d
  rsa.n = mpz_class(argv[1]);
  rsa.e = mpz_class(argv[2]);
  int sz = 32;
  mpz_class p,q;
  mpz_class res;
  int ret = 0, ret1, ret2;
  mpz_sqrt(p.get_mpz_t(), rsa.n.get_mpz_t());
  do
  {
	p = p + 1;
	mpz_cdiv_q(q.get_mpz_t(), rsa.n.get_mpz_t(), p.get_mpz_t());
	mpz_mod(res.get_mpz_t(), rsa.n.get_mpz_t(), p.get_mpz_t());
  }while(mpz_cmp(res.get_mpz_t(), mpz_class(0).get_mpz_t()) != 0);
  // Set rsa.d to the calculated private key d
  // rsa.d = mpz_class(// broken d value here) 
  mpz_class p0;
  mpz_class q0;
  mpz_class phi;
  mpz_class d;
  mpz_class one = 1;
  mpz_sub(p0.get_mpz_t(), p.get_mpz_t(), one.get_mpz_t());
  mpz_sub(q0.get_mpz_t(), q.get_mpz_t(), one.get_mpz_t());
  mpz_mul(phi.get_mpz_t(), p0.get_mpz_t(), q0.get_mpz_t());
  mpz_invert(d.get_mpz_t(), rsa.e.get_mpz_t(), phi.get_mpz_t());
  rsa.d = d;
  string str = "";
  string tmp;
  for (int i = 3; i < 13; ++i)
    { // Decrypt each set of 6 characters
      mpz_class c(argv[i]);
      mpz_class m = rsa.Decrypt(c);
      //  use the get_ui() method in mpz_class to get the lower 48 bits of the m
      unsigned long ul = m.get_ui();
      // Now print the 6 ascii values in variable ul.  As stated above the 6 characters
      // are in the low order 48 bits of ui.
	  tmp = "";
	  while(ul)
	  {
		tmp = ((char)(ul & 0x000000ff)) + tmp;
		ul >>= 8;
	  }
	  str += tmp;
    }
	cout << str;
  cout << endl;
}



