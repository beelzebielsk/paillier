#include "paillier.h"
#include <vector>
#include <iostream>

NTL::ZZ seed = (NTL::ZZ)0;
//NTL::SetSeed(seed);

NTL::ZZ generateCoprimeNumber(const NTL::ZZ& n) {
    NTL::ZZ ret;
    while (true) {
        ret = RandomBnd(n);
        if (NTL::GCD(ret, n) == 1) { return ret; }
    }
}

std::vector<bool> ZZToBits(NTL::ZZ number) {
    long numBytes = NumBytes(number);
    std::vector<bool> bits;

    unsigned char * bytes = new unsigned char[numBytes];
    BytesFromZZ(bytes, number, numBytes);
    unsigned char * bytesEnd = bytes + numBytes;
    for (unsigned char * byte = bytes; byte != bytesEnd; byte++) {
        for (int i = 0; i < 8; i++) {
            bits.push_back(((*byte) >> i) & 1);
        }
    }
    delete bytes;
    return bits;
}


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


void Paillier::GenPrimePair(NTL::ZZ& p, NTL::ZZ& q,
                               long keyLength) {
    long err = 80;
    long primeLength = keyLength/2;
    while (true) {
        p = NTL::GenPrime_ZZ(primeLength, err); 
        q = NTL::GenPrime_ZZ(primeLength, err);
        while (p == q) {
            q = NTL::GenPrime_ZZ(primeLength, err);
        }
        NTL::ZZ n = p * q;
        NTL::ZZ phi = (p - 1) * (q - 1);
        if (NTL::GCD(n, phi) == 1) return;
    }
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
