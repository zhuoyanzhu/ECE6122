// ECE4122/6122 RSA Encryption/Decryption assignment
// Fall Semester 2015

#include <iostream>
#include "RSA_Algorithm.h"

using namespace std;

int main()
{
  // Instantiate the one and only RSA_Algorithm object
  RSA_Algorithm RSA;
  
  // Loop from sz = 32 to 1024 inclusive
  // for each size choose 10 different key pairs
  // For each key pair choose 10 differnt plaintext 
  // messages making sure it is smaller than n.
  // If not smaller then n then choose another
  // For eacm message encrypt it using the public key (n,e).
  // After encryption, decrypt the ciphertext using the private
  // key (n,d) and verify it matches the original message.
	mpz_class M;
	mpz_class C;
	int sz = 32;
	while(sz <= 1024)
	{
		for(int i = 0; i < 100; ++i)
		{
			RSA.GenerateRandomKeyPair(sz);
			RSA.PrintNDE();
			for(int j = 0; j < 100; ++j)
			{
				M = RSA.rng.get_z_bits(sz - 1);
				RSA.PrintM(M);
				C = RSA.Encrypt(M);
				RSA.PrintC(C);
			}
		}
		sz *= 2;
	}
  // your code here
}
  
