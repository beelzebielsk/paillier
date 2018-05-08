#include <NTL/ZZ.h>
#include <vector>

#ifndef DATA_H
#define DATA_H

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
    NTL::ZZ modulus;
    std::vector<NTL::ZZ> bits;

    Input(NTL::ZZ value, NTL::ZZ key, NTL::ZZ modulus);
    Input(NTL::ZZ value, std::vector<NTL::ZZ> bits, NTL::ZZ modulus);
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

#endif
