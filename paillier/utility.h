#ifndef UTILITY_H
#define UTILITY_H

#include <NTL/ZZ.h>
#include <vector>

/* Create a bit vector out of the bytes of an NTL integer. The result
 * is a vector such that bits[i] * 2 ** i = number.
 *
 * Parameters
 * ==========
 * number, NTL::ZZ : The number to create a bit vector out of.
 *
 * Returns
 * =======
 * bits, std::vector<bool> : The bit vector corresponding to the
 *      input.
 */
std::vector<bool> ZZToBits(NTL::ZZ number);

/* Generate a number that's coprime to the argument.
 *
 * Parameters
 * ==========
 * n, NTL::ZZ : An integer.
 *
 * Returns
 * =======
 * ret, NTL::ZZ : An integer that's comprime to n.
 */
NTL::ZZ generateCoprimeNumber(const NTL::ZZ& n);

/* Generate a pair of non equal prime numbers.
 *
 * Parameters
 * ==========
 * p&, q&, NTL::ZZ : References to the primes to generate.
 * keyLength, long : The length in bits of each key.
 */
void GenPrimePair(NTL::ZZ& p, NTL::ZZ& q, long length);
#endif
