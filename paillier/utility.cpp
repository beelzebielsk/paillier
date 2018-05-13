#include "utility.h"

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
    delete[] bytes;
    return bits;

}

NTL::ZZ generateCoprimeNumber(const NTL::ZZ& n) {
    NTL::ZZ ret;
    while (true) {
        ret = RandomBnd(n);
        if (NTL::GCD(ret, n) == 1) { return ret; }
    }
}

void GenPrimePair(NTL::ZZ& p, NTL::ZZ& q, long keyLength) {
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
