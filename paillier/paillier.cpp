#include "paillier.h"
#include <vector>
#include "utility.h"
#include <iostream>

NTL::ZZ seed = (NTL::ZZ)0;
//NTL::SetSeed(seed);

Paillier::Paillier() : Paillier(512) {}

Paillier::Paillier(const long keyLength) {
    NTL::ZZ p, q;
    GenPrimePair(p, q, keyLength);
    modulus = p * q;
    generator = modulus + 1;
    NTL::ZZ phi = (p - 1) * (q - 1);

    lambda = (NTL::ZZ)0;
    NTL::CRT(lambda, phi, (NTL::ZZ)1, modulus);
    phi = (p - 1) * (q - 1);
    modulus = p * q;
    // NTL returns values that may be negative. lambda is obtained via
    // the CRT. It's a number that is calculated modulo (modulus *
    // phi).
    lambda = lambda % (modulus * phi);
}

Paillier::Paillier(const NTL::ZZ& modulus, const NTL::ZZ& lambda) {
    this->modulus = modulus;
    generator = this->modulus + 1;
    this->lambda = lambda;
}

NTL::ZZ Paillier::encrypt(const NTL::ZZ& message) {
    NTL::ZZ random = generateCoprimeNumber(modulus * modulus);
    return Paillier::encrypt(message, random);
}

NTL::ZZ Paillier::encrypt(const NTL::ZZ& message, const NTL::ZZ& random) {
    NTL::ZZ ciphertext = 
        NTL::PowerMod(generator, message, modulus * modulus) *
        NTL::PowerMod(random, modulus, modulus * modulus);
    return ciphertext % (modulus * modulus);
}

std::vector<NTL::ZZ> Paillier::encryptBits(NTL::ZZ message) {
    std::vector<bool> keyBits = ZZToBits(lambda);
    std::vector<NTL::ZZ> bits;
    for (bool bit : keyBits) {
        bits.push_back(this->encrypt(bit * message));
    }
    return bits;
}

NTL::ZZ Paillier::decrypt(const NTL::ZZ& ciphertext) {
    /* NOTE: NTL::PowerMod will fail if the first input is too large
     * (which I assume means larger than modulus).
     */

    NTL::ZZ deMasked = NTL::PowerMod(
            ciphertext, lambda, modulus * modulus);
    NTL::ZZ power = L_function(deMasked);
    return power % modulus;
}
