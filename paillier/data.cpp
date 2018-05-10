#include "./data.h"
    using NTL::ZZ;
    using std::vector;
    using NTL::BytesFromZZ;
    using NTL::NumBytes;
#include <exception>
    using std::exception;

vector<bool> ZZToBits(ZZ number) {
    long numBytes = NumBytes(number);
    unsigned long numBits = numBytes * 8;
    vector<bool> bits;

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

Input::Input(ZZ value, ZZ key, ZZ modulus) 
    : Value(Value::Type::Input), value(value), modulus(modulus)
{
    vector<bool> keyBits = ZZToBits(key);
    vector<ZZ> bits;
    for (bool bit : keyBits) {
        bits.push_back(NTL::power(this->value, bit));
    }
    this->bits = bits;
}

Input::Input(ZZ value, vector<ZZ> bits, ZZ modulus) 
    : Value(Value::Type::Input), value(value), modulus(modulus) {}

Memory::Memory(ZZ value, ZZ secret)
    : Value(Value::Type::Memory), value(value), secret(secret) {}

/* Take two inputs and produce another input whose message is the sum
 * of the messages from the two operands.
 *
 * Parameters
 * ==========
 * a, b, Input : Inputs. They should be encrypted using the same
 *      generator in order to return a meaningful result.
 * Returns
 * =======
 * result, Input : An input such that result.value is an encryption of
 *      the sum of the messages from a and b, and result.bits are
 *      result.value raised to the value of a single bit from the
 *      encryption key.
 */
Input operator+(Input a, Input b) {
    ZZ value = a.value * b.value;
    vector<ZZ> bits (a.bits.size());
    for (long i = 0; i < a.bits.size(); i++) {
        bits.push_back(a.bits[i] * b.bits[i]);
    }
    return Input(value, bits, a.modulus);
}

Memory operator+(Memory a, Memory b) {
    return Memory(
            a.value + b.value, a.secret + b.secret);
}

/* Multiplication of a single memory location with a single encrypted
 * value. Used for performing multiplication of a memory location and
 * an input, since inputs consist of several encrypted values.
 *
 * Parameters
 * ==========
 * encryption, ZZ : An encrypted value using the Paillier
 *      cryptosystem.
 * mem, Memory : The contents of a memory location of an RMS program.
 * modulus, ZZ : The modulus of the paillier cryptosystem with which
 *      `encryption` was encrypted.
 * Returns
 * =======
 * share, ZZ : Returns a single share of a value.
 */
ZZ multMemoryEncryption(ZZ encryption, Memory mem, ZZ modulus) {
    // First, go for the multiplicative share.
    ZZ multShare = NTL::PowerMod(encryption, mem.secret, modulus);
    // Then use DDLOG to transform to the additive share.

    throw exception();
    // TODO: Finish this
    return ZZ(0);
}

/* Multiplication of a memory location and an input. Consists of
 * multiplying the memory location with the input's value, and also
 * with all of the encrypted bits of the input. The encrypted bits are
 * used to reconstruct a sharing invovling lambda, the secret key of
 * the paillier cryptosystem.
 *
 * Parameters
 * ==========
 * i, Input : The input the multiply.
 * mem, Memory : The memory location to multiply.
 *
 * Returns
 * =======
 * result, Memory : A memory location which describes one additive
 *      share of the multiplication of the encrypted value of the
 *      input and the full number that the shares in `mem` correspond
 *      to.
 */
Memory multMemoryInput(Input i, Memory mem) {
    ZZ value = multMemoryEncryption(i.value, mem, i.modulus);
    ZZ secret;
    long power2 = 0;
    for (ZZ bit : i.bits) {
        ZZ share = multMemoryEncryption(bit, mem, i.modulus);
        secret += share << power2;
        ++power2;
    }
    return Memory(value, secret);
}

/* The following functions are frontends to the operations of
 * multiplication and addition on inputs and memory locations. The
 * actual type of the arguments is determined at runtime (the type of
 * the calling object is determined at runtime by a virtual function,
 * and the type of the other operand is determined at runtime via a
 * dynamic cast.
 */

/* A frontend to all addition operations that are defined on an input.
 *
 * Parameters
 * ==========
 * operand, Value& : Must be an input at the moment.
 * destination, Value& : The place to put the result of the
 *      calculation.
 */
void Input::add(Value& destination, const Value& operand) {
    if (operand.type == Value::Type::Input) {
        //const Input& op = dynamic_cast<const Input&>(operand);
        Input intermediate = dynamic_cast<const Input&>(operand);
        intermediate = *this + intermediate;
        destination = intermediate;
    } else throw exception();

}

/* A frontend to all addition operations that are define on an
 * input. 
 *
 * Parameters
 * ==========
 * operand, Value & : An operand for multiplication with an input.
 */
void Memory::add(Value& destination, const Value& operand) {
    if (operand.type == Value::Type::Memory) {
        Memory intermediate = dynamic_cast<const Memory&>(operand);
        intermediate = *this + intermediate;
        destination = intermediate;
    } else throw exception();
}


/* A frontend to all multiplication operations that are define on an
 * input. 
 *
 * Parameters
 * ==========
 * operand, Value & : An operand for multiplication with an input.
 */
void Input::multiply(Value& destination, const Value& operand) {
    if (operand.type == Value::Type::Memory) {
        Memory intermediate = dynamic_cast<const Memory&>(operand);
        intermediate = multMemoryInput(*this, intermediate);
        destination = intermediate;
    } else throw exception();
}

/* A frontend to all multiplication operations that are define on an
 * input. 
 *
 * Parameters
 * ==========
 * operand, Value & : An operand for multiplication with an input.
 */
void Memory::multiply(Value& destination, const Value& operand) {
    if (operand.type == Value::Type::Input) {
        Input intermediate = dynamic_cast<const Input&>(operand);
        destination = multMemoryInput(intermediate, *this);
    } else throw exception();
}
