#ifndef DATA_H
#define DATA_H

#include <NTL/ZZ.h>
#include <vector>
#include <istream>
#include <ostream>

class Value {
    public:
    /* Which server is running the computations. */
    static bool serverIdentity;
    /* The modulus with which the ciphertexts have been encrypted. 
     * For the paillier cryptosystem, that's n, not n ** 2. */
    static NTL::ZZ modulus;
    /* Initialize the information for the current server. Call this
     * before performing any computations with Memory or Input, to be
     * safe.
     * Any operator function between Memory and Input may use the
     * information that this is supposed to initialize.
     * Functions outside of this header file should not directly use
     * this information, though it is not promised.
     *
     * Parameters
     * ==========
     * serverIdentity, bool : The identity of the server. False for
     *      server 1, true for server 2. 
     * modulus NTL::ZZ : The modulus of the paillier cryptosystem with
     *      which all of the encryptions were performed. This modulus
     *      should be the paillier n, not n ** 2.
     */
    static void init(bool serverIdentity, NTL::ZZ modulus) {
        Value::serverIdentity = serverIdentity;
        Value::modulus = modulus;
    }
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
    Input operator+(Input& op);
    Input operator-();
    Input operator-(Input& op);
    friend std::ostream& operator<<(std::ostream&, Input);
    friend std::istream& operator>>(std::istream&, Input&);
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

    Memory operator+(Memory& op);
    Memory(NTL::ZZ value, NTL::ZZ secret);
    friend std::ostream& operator<<(std::ostream&, Memory);
    friend std::istream& operator>>(std::istream&, Memory&);
    virtual void add(Value& destination, const Value& operand);
    virtual void multiply(Value& destination, const Value& operand); 
};

/* Multiplies a memory location and an input together. The result is a
 * the value that should go in m.value for a memory location.
 * Specifically, if this number is output from and RMS program run on
 * two different servers, (one for whom serverIdentity = 0, the other
 * for whom serverIdentity = 1), if the two outputs are summed from
 * the results run on two different computers, the result should be:
 *      paillier.decrypt(encryption) * (mem1.value + mem2.value) 
 * Where mem1 is the argument to mem on this server, and mem2 is the
 * argument to mem on the other server.
 *
 * Parameters
 * ==========
 * encryption, ZZ : An encrypted value. Result will be related to the
 *      product of the encrypted message and the contents of the
 *      memory location.
 * mem, Memory : Memory location to perform product with.
 * modulus, ZZ : The modulus of the paillier object that the
 *      encryption was encrypted with.
 * server, bool : Indicates which server the current server is.
 *      Multiplication works a little differently for the two
 *      different servers on which an RMS program may be run.
 * Returns
 * =======
 * share, ZZ : Returns a share of the product 
 *      paillier.decrypt(encryption) * mem.value
 */
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
Memory multMemoryInput(Input i, Memory mem, NTL::ZZ modulus,
                       bool serverIdentity);

/* Succinct way to perform input/memory multiplications directly.
 * Requires the Value::init() function to be called, so that the
 * correct modulus and server identity can be used. Just calls
 * multMemoryInput with the correct server identity.
 *
 * Parameters
 * ==========
 * m, Memory : Memory location to multiply the input by.
 * i, Input : Input to multiply the memory location by.
 *
 * Returns
 * =======
 * result, Memory : The new memory location returned by the product of
 *      the input and memory.
 */
Memory operator*(Memory& m, Input& i);
Memory operator*(Input& i, Memory& m);
#endif
