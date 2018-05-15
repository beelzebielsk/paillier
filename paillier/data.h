#ifndef DATA_H
#define DATA_H

#include <NTL/ZZ.h>
#include <vector>

class Value {
    public:
    enum class Type {Input, Memory};
    Type type;
    virtual void add(Value& destination, const Value& operand) = 0;
    virtual void multiply(Value& destination, const Value& operand) = 0;
    protected:
    Value(Type type) : type(type) {};
};

class Input : public Value {
    public:
    NTL::ZZ value;
    // I'm putting this here because use of any modulus other than the
    // correct modulus is wrong. The results will be wrong. You should
    // not be free to pass in whatever modulus you want to pass in.
    // Inputs should be used with one modulus only.
    // Even this seems incorrect. There should be one modulus for a
    // family of inputs. There's no correct thing to do when combining
    // inputs with different moduli.
    // Their... "context" is what should have the modulus. Some sort
    // of RMS instruction executor.
    NTL::ZZ modulus;
    std::vector<NTL::ZZ> bits;

    Input(NTL::ZZ value, std::vector<NTL::ZZ> bits, NTL::ZZ modulus);
    Input operator+(Input op);
    Input operator-();
    virtual void add(Value& destination, const Value& operand); 
    virtual void multiply(Value& destination, const Value& operand); 
};

class Memory : public Value {
    public:
    /* The actual value of the memory location. */
    NTL::ZZ value;
    /* Value that is only meangful for performing computations with
     * the memory location. */
    NTL::ZZ secret;

    Memory(NTL::ZZ value, NTL::ZZ secret);
    virtual void add(Value& destination, const Value& operand);
    virtual void multiply(Value& destination, const Value& operand); 
};

NTL::ZZ multMemoryEncryption(NTL::ZZ encryption, Memory mem, 
                             NTL::ZZ modulus, bool server);
/* Converts a number into two additive shares, a and b, such that a +
 * b = number.
 *
 * Parameters
 * ==========
 * number, ZZ : Number to split into shares. Must be in the range: 
 *      [0, modulus ** 2].
 * modulus, ZZ : The modulus of a related paillier cryptosystem.
 *
 * Returns
 * =======
 * 
 */
std::pair<NTL::ZZ, NTL::ZZ> share(NTL::ZZ number, NTL::ZZ modulus);
#endif
