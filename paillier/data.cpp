#include "./data.h"
    using NTL::ZZ;
    using std::vector;
    using NTL::BytesFromZZ;
    using NTL::NumBytes;
    using NTL::InvMod;
#include <exception>
    using std::exception;

#include "utility.h"
#include "ddlog.h"

Input::Input(ZZ value, vector<ZZ> bits, ZZ modulus) 
    : Value(Value::Type::Input), value(value), bits(bits),
    modulus(modulus) {}

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
Input Input::operator+(Input op) {
    ZZ opMod = this->modulus * this->modulus;
    ZZ value = (this->value * op.value) % opMod;
    vector<ZZ> bits;
    for (long i = 0; i < bits.size(); i++) {
        bits.push_back((this->bits[i] * op.bits[i]) % opMod);
    }
    return Input(value, bits, modulus);
}

Input Input::operator-() {
    ZZ newValue = InvMod(value, modulus * modulus);
    vector<ZZ> newBits;
    for (ZZ bit : bits) {
        newBits.push_back(InvMod(bit, modulus * modulus));
    }
    return Input(newValue, newBits, modulus);
}

Memory operator+(Memory a, Memory b) {
    return Memory(
            a.value + b.value, a.secret + b.secret);
}

ZZ multMemoryEncryption(ZZ encryption, Memory mem, ZZ modulus,
                        bool server) {
    // Let server = false be server 1
    // Let server = true be server 2

    ZZ cipherModulus = modulus * modulus;
    ZZ generator = modulus + 1;

    // First, go for the multiplicative share.
    ZZ multShare = NTL::PowerMod(encryption, mem.secret, cipherModulus);
    // Then use DDLOG to transform to the additive share.

    // Check which server we're working wih and apply DDLOG
    if(!server){
        return (ZZ)(-DDLog::getStepsA(multShare, generator,
                    cipherModulus));
    }
    else {
        return (ZZ)(DDLog::getStepsB(multShare, generator,
                    cipherModulus));
    }
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
    /*
    ZZ value = multMemoryEncryption(i.value, mem, i.modulus);
    ZZ secret;
    long power2 = 0;
    for (ZZ bit : i.bits) {
        ZZ share = multMemoryEncryption(bit, mem, i.modulus);
        secret += share << power2;
        ++power2;
    }
    return Memory(value, secret);
    */
    // Not implemented yet.
    throw exception();
    return Memory{(ZZ)0, (ZZ)0};
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
    /*
    if (operand.type == Value::Type::Input) {
        //const Input& op = dynamic_cast<const Input&>(operand);
        Input intermediate = dynamic_cast<const Input&>(operand);
        intermediate = *this + intermediate;
        destination = intermediate;
    } else throw exception();
    */
    // Not implemented yet.
    throw exception();
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
    /*
    if (operand.type == Value::Type::Memory) {
        Memory intermediate = dynamic_cast<const Memory&>(operand);
        intermediate = multMemoryInput(*this, intermediate);
        destination = intermediate;
    } else throw exception();
    */
    // Not implemented yet.
    throw exception();
}

/* A frontend to all multiplication operations that are define on an
 * input. 
 *
 * Parameters
 * ==========
 * operand, Value & : An operand for multiplication with an input.
 */
void Memory::multiply(Value& destination, const Value& operand) {
    /*
    if (operand.type == Value::Type::Input) {
        Input intermediate = dynamic_cast<const Input&>(operand);
        destination = multMemoryInput(intermediate, *this);
    } else throw exception();
    */
    // Not implemented yet.
    throw exception();
}

std::pair<ZZ, ZZ> share(ZZ number, ZZ modulus) {
    // Generate number in [-modulus ** 3, modulus ** 3].
    // - modulus = 2
    // - cube = 8
    // - range is [-8, 8]
    // - number is in [0, 3]
    // - number is 3:
    //     - if number is three, then -8, -7, -6 cannot be shares.
    //       They're too low, no other share can add with them to get
    //       3.
    
    // This is a safe assumption. The actual shared numbers ought to
    // be smaller than M, and M ought to be MUCH smaller than n ** 2,
    // since there should be many special points in Z/Zn**2.
    NTL::ZZ normalizedNum = number % (modulus * modulus);
    NTL::ZZ cube = NTL::power(modulus, 3);
    NTL::ZZ share1 = RandomInRange(-cube + number, cube);
    NTL::ZZ share2 = number - share1;
    return std::pair<ZZ, ZZ>{share1, share2};
}
