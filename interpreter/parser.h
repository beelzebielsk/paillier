#ifndef PAILLIER_INTERPRETER_H
#define PAILLIER_INTERPRETER_H

#include <ctype.h>
    // tolower
#include <vector>
    using std::vector;
#include <unordered_map>
#include <istream>
    using std::istream;
#include <ostream>
    using std::ostream;
#include <exception>

using std::string;

typedef string Token;
// TODO: change what value applies to. Value should probably be NTL
// numbers, since that's what we'll be operating on.
typedef long long int Value;
typedef std::unordered_map<string, Value> ProgramState;

/* Mutates the argument string such that all letters are lowercase.
 *
 * Parameters
 * ==========
 * str, string& : String to lowercase.
 */
void tolower(string&);

/* Tests if a character is inside of a string.
 * Parameters
 * ==========
 * str, string : String to test against.
 * c, char : Character to test for.
 *
 * Returns
 * =======
 * inString, bool : True if c is a character in str, and false
 *      otherwise.
 */
bool inString(string, char);

/* Returns true if the string content of a token contains the string
 * "string".
 * This is a wrapper around the semantics of checking a token for a
 * string. It will contain the logic for doing so, even if tokens
 * eventually become more complicated than just strings.
 *
 * Parameters
 * ==========
 * token, Token: The token to test against.
 * str, string: The string to use when checking if a string is in the
 *      value of a token.
 * Returns
 * =======
 * isContained, bool : True if str is contained in the token's value.
 */
bool tokenContains(Token, string);

struct InterpreterError : std::exception {
    string message;
    InterpreterError(string msg) : message(msg) {};
    InterpreterError() : message("Error!") {};
    friend ostream& operator<<(ostream& out, InterpreterError e) {
        out << e.what();
        return out;
    }
    const char * what() const noexcept {
        return message.c_str();
    }
    /*
    ~InterpreterError() {
        delete &message;
    }
    */

};

/* Instructions:
 * The operands to all of the operations are identifiers.
 * - add: destination addend addend. Take the two addends and add them
 *   together, placing the result in the destination. More
 *   technically, the addends are identifiers. We add the values bound
 *   to the identifiers of the addends, add them together, and bind
 *   the result to the destination identifier.
 * - mult: destination factor factor. Take the two factors and
 *   multiply them together, placing the result in the destination.
 * - load: destination value. destination and value are identifiers.
 *   Take the value bound to the `value` identifier, and bind that
 *   value to the `destination` identifier.
 * - output: ?
 */
class Instruction {
    public:
        Instruction(string instruction, string op1, 
                    string op2 = "", string op3 = "")
                    : operand1(op1), operand2(op2), operand3(op3) {
            Type type = which(instruction);
            if (type == Type::invalid) {
                throw InterpreterError("Invalid instruction name.");
            }
            this->type = type;
        }
        enum class Type {
            /* The invalid instruction type is for reporting a string
             * that is not an instruction.
             * It is not meant for use outside of this class. It
             * should be considered private.
             */
            add, mult, load, output, invalid
        };

        static const Type add    = Type::add;
        static const Type mult   = Type::mult;
        static const Type load   = Type::load;
        static const Type output = Type::output;

        static bool isInstructionName(string str) {
            tolower(str);
            return str == "add" || 
                str == "mult" ||
                str == "load" ||
                str == "output";
        }
        static Type which(string);
        Type type;
        string operand1;
        string operand2;
        // Not all instructions use the 3rd operand.
        string operand3;

        friend bool operator==(
                const Instruction i1, const Instruction i2) {
            i1.type == i2.type &&
            i1.operand1 == i2.operand1 &&
            i1.operand2 == i2.operand2 &&
            i1.operand3 == i2.operand3;
        }
    friend std::ostream& operator<<(std::ostream& out, Type type) {
        if (type == Instruction::add) {
            out << "Instruction::add";
        } else if (type == Instruction::mult) {
            out << "Instruction::mult";
        } else if (type == Instruction::load) {
            out << "Instruction::load";
        } else if (type == Instruction::output) {
            out << "Instruction::output";
        }
        return out;
    }

    friend std::ostream& operator<<(std::ostream& out,
            Instruction i) {
        out << "["
            << i.type
            << ", "
            << i.operand1;
        if (i.operand2 != "") out << ", " << i.operand2;
        if (i.operand3 != "") out << ", " << i.operand3;
        out << "]";
        return out;
    }
};

class Declaration {
    public:
        enum class Type {
            input, share, invalid
        };
        static const Type input   = Type::input;
        static const Type share   = Type::share;
        static const Type invalid = Type::invalid;

        string identifierName;
        Type type;

        Declaration(string type, string name) 
                : identifierName(name) {
            Type declarationType = which(type);
            if (declarationType == Type::invalid) {
                throw InterpreterError("Invalid Declaration type.");
            }
            this->type = declarationType;
        }

        static Type which(string);
        static bool isDeclarationType(string type) {
            tolower(type);
            return type == "input" || type == "share"; 
        }
    friend std::ostream& operator<<(std::ostream& out, Type type) {
        if (type == Declaration::input) {
            out << "Declaration::input";
        } else if (type == Declaration::share) {
            out << "Declaration::share";
        }
        return out;
    }

};



/* We're going to need:
 * - Separate the words in a textfile into a list of words, separated
 *   by whitespace.
 * - We're going to need a way to evaluate these programs. The operans
 *   are going to be names. All of them (apparently). If that's true,
 *   then to evaluate a program, we will need a mapping from input
 *   values to names. I assume that we will not need any mappings from
 *   memory locations to names. Those will all be handled during the
 *   program's execution. There won't be any initialization of program
 *   memory.
 * - We're going to need a parser. It will take a list of tokens and a
 *   mapping from names to input values (not a mapping from memory
 *   locations to values, that will be generated during execution),
 *   and it will evaluate the program.
 */

/* Takes a stream, which represents an RMS file, and produces a list
 * of tokens from the file. The list of tokens will be consumed by
 * parsing functions.
 *
 * Parameters
 * ==========
 * file, istream& : The file to tokenize.
 *
 * Returns
 * =======
 * tokens, vector<Token> : The tokens from the file.
 */
vector<Token> tokenizeProgram(istream& file);

/* This takes a list of tokens and produces a list of machine-readable
 * instructions. This is here because I think that the execution of a
 * program is going to get a lot more complicated with time. It's
 * simple for now. So I'm separating the step of creating the
 * instructions to become executed from their execution. We could do
 * some sort of loop-unwrapping or preprocessing here if we end up
 * requiring something like that.
 * 
 * Parameters
 * ==========
 * tokens, vector<Token> : A list of tokens from a source program.
 *      If the tokens create incorrect statements, then this function
 *      will throw an error.
 * Returns
 * =======
 * instructions, vector<Instruction> : A list of machine-readable
 *      instructions.
 */
vector<Instruction> createInstructionList(vector<Token> tokens);

/* Takes a list of instructions, executes those instructions on some
 * initial program state, and then returns the final program state.
 *
 * Parameters
 * ==========
 * instructions, vector<Instruction> : The list of instructions to
 *      execute.
 * initialState, ProgramState : The initial state of the program. It
 *      contains keys for the identifiers that were declared at the
 *      beginning of the program, and values are bound to that
 *      identifier based on the values transmitted from the client to
 *      the server executing this program.
 * Returns
 * =======
 * finalState, ProgramState : The final state of the program.
 */
ProgramState execute(vector<Instruction> instructions,
                     ProgramState initialState);

#endif
