#ifndef PAILLIER_H
#define PAILLIER_H

#include <NTL/ZZ.h>
#include <vector>

class Paillier {
    public:
    /* Completely generate everything, from scratch */
    Paillier();
    Paillier(const NTL::ZZ& modulus, const NTL::ZZ& lambda); 
    Paillier(const long keyLength);
    //Paillier(path to public key, path to private key).

    /* Paillier encryption function. Takes in a message from the
     * integers modulo n (Paillier.modulus) and returns a message in
     * the integers modulo n**2.
     *
     * Parameters
     * ==========
     * NTL::ZZ message : The message to encrypt, as a number.
     *
     * Returns
     * =======
     * NTL:ZZ ciphertext : The encyrpted message.
     */
    NTL::ZZ encrypt(const NTL::ZZ& message); 

    /* Paillier encryption function with provided randomness, if user
     * wants to provide their own randomness.
     *
     * Random number should be coprime to modulus.
     *
     * Parameters
     * ==========
     * NTL::ZZ message : The message to encrypt, as a number.
     * NTL::ZZ random : The random mask.
     *
     * Returns
     * =======
     * NTL:ZZ ciphertext : The encyrpted message.
     */
    NTL::ZZ encrypt(const NTL::ZZ& message, const NTL::ZZ& random); 
    
    /* Creates encryptions of each bit of the secret key. The first
     * encryption is of the least significant bit of the key. You may
     * specify a message to multiply the bits of the secret key with.
     * It is the same message for all of the bits of the secret key.
     *
     * Parameters
     * ==========
     * message optional, NTL::ZZ : A message to pass in to multiply
     *      each bit of the secret key.
     *
     * Returns
     * =======
     * encryptedBits, std::vector<NTL::ZZ> : The encrypted bits of the
     * secret key. Each entry is basically equal to:
     *      p.encrypt(((p.lambda >> (bit - 1)) & 1) * message)
     * Where p is an object of class Paillier.
     */
    std::vector<NTL::ZZ> encryptBits(NTL::ZZ message = NTL::ZZ{1});

    

    /* Paillier decryption function. Takes in a cipertext from Z mod
     * n**2 and returns a message in the Z mod n.
     *
     * Parameters
     * ==========
     * NTL::ZZ cipertext : The encrypted message.
     *
     * Returns
     * =======
     * NTL::ZZ message : The original message.
     */
    NTL::ZZ decrypt(const NTL::ZZ& ciphertext); 

    NTL::ZZ getModulus() { return modulus; }
    NTL::ZZ getGenerator() { return generator; }

    private:
    /* modulus = pq, where p and q are primes */
    NTL::ZZ modulus;
    NTL::ZZ generator;
    NTL::ZZ lambda;
    NTL::ZZ lambdaInverse;

    /* The L function in the paillier cryptosystem.  See
     * <https://en.wikipedia.org/wiki/Paillier_cryptosystem> for more
     * details.
     *
     * Parameters
     * ==========
     * NTL::ZZ x : The argument to L.
     * NTL::ZZ n : The paillier modulus.
     *
     * Returns
     * =======
     * NTL::ZZ result : (x - 1) / n
     */
    NTL::ZZ L_function(const NTL::ZZ& n) { return (n - 1) / modulus; }

    void GenPrimePair(NTL::ZZ& p, NTL::ZZ& q, long keyLength); 
};

#endif
