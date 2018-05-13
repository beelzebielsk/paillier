#include "paillier.h"
#include <vector>
#include "utility.h"
#include <iostream>

NTL::ZZ seed = (NTL::ZZ)0;
//NTL::SetSeed(seed);



Paillier::Paillier() {
    Paillier(512);
}

Paillier::Paillier(const long keyLength) {
    NTL::ZZ p, q;
    GenPrimePair(p, q, keyLength);
    modulus = p * q;
    generator = modulus + 1;
    NTL::ZZ phi = (p - 1) * (q - 1);
    // LCM(p, q) = p * q / GCD(p, q);
    lambda = phi / NTL::GCD(p - 1, q - 1);
    lambdaInverse = NTL::InvMod(lambda, modulus);
}

Paillier::Paillier(const NTL::ZZ& modulus, const NTL::ZZ& lambda) {
    this->modulus = modulus;
    generator = this->modulus + 1;
    this->lambda = lambda;
    lambdaInverse = NTL::InvMod(this->lambda, this->modulus);
}

NTL::ZZ Paillier::encrypt(const NTL::ZZ& message) {
    NTL::ZZ random = generateCoprimeNumber(modulus);
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
        bits.push_back(NTL::power(message, bit));
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
    return (power * lambdaInverse) % modulus;
}
