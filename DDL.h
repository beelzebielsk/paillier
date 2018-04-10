#include <NTL/ZZ.h>
#include <NTL/RR.h>
#include <openssl/sha.h>

/* Determines if a number is a special point, based on delta. Delta is
 * the probability that any given number is a special point.
 *
 * Parameters
 * ==========
 * NTL:ZZ num : The number to check for specialness.
 * double delta : The probability that a number is a special point.
 *      Must be between 0 and 1. If 1, then all points are special. If
 *      0, then no points are special.
 *
 * Returns
 * =======
 * bool ret : True if the number is a special point, false otherwise.
 */
bool isSpecialPoint(NTL::ZZ num, double delta) {
    long hashBytes = 160/8;
    RR maxValue = NTL::MakeRR(((ZZ)1 << (hashBytes * 8)) - 1, 0);
    ZZ threshold = NTL::RoundToZZ(maxValue * delta);
    cout << "maximum hash value: " << maxValue << endl;
    cout << "Threshold: " << threshold << endl;
    ZZ hash = hashZZ(num);
    cout << "Hash: " << hash << endl;
    cout << threshold - hash << endl;
    bool ret = hash <= threshold;
    hash.kill();
    return ret;
}

/* Returns the hash of the bytes of num, interpreted as another
 * NTL::ZZ number.
 *
 * Parameters
 * ==========
 * NTL::ZZ num : The number to hash.
 *
 * Returns
 * =======
 * NTL::ZZ ret : The value of the hash of num, interpreted as a
 *      number.
 *
 */
NTL::ZZ hashZZ(NTL::ZZ num) {
    // Length of an SHA1 hash is 160 bits.
    long hashBytes = 160/8;
    unsigned char * buf = numToBytes(num);
    long numBytes = NTL::NumBytes(num);
    // Output of sha1 is 160 bits, which is 160/8 = 20 bytes.
    unsigned char * output = new unsigned char[hashBytes];
    SHA1(buf, numBytes, output);
    for (int i = 0; i < hashBytes; i++) {
        printf("%02x", output[i]);
    }
    cout << endl;
    ZZ ret = NTL::ZZFromBytes(output, hashBytes);
    // cout << "Hash as number: " << ret << endl;
    delete buf;
    delete output;
    return ret;
}

/* A wrapper around the NTL::BytesFromZZ function.
 *
 * Parameters
 * ==========
 * NTL:ZZ num : An integer to transform into bytes.
 *
 * Returns
 * =======
 * unsigned char * buf : A dynamically allocated buffer of characters.
 *      The characters fit the same description as those that come out
 *      of NTL::BytesFromZZ.
 */
unsigned char * numToBytes(NTL::ZZ num) {
    unsigned char * buf;
    long numBytes = NumBytes(num);
    buf = new unsigned char[numBytes];
    NTL::BytesFromZZ(buf, num, numBytes);
    cout << "Num Bytes in number: " << numBytes << endl;
    return buf;
}
