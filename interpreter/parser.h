#include <fstream>
#include <ctype.h>
// tolower function.
#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using std::ifstream;

typedef string Token;
typedef string Value;
typedef std::unordered_map<string, Value> ProgramState;

void tolower(string& str) {
    for (int i = 0; i < str.length(); i++) {
        str[i] = tolower(str[i]);
    }
}

class Instruction {
    public:
        Instruction(string instruction, string op1, string op2, string op3 = "")
            : operand1(op1), operand2(op2), operand3(op3) {
                InstructionType type = whichInstruction(instruction);
                if (type == InstructionType::invalid) {
                    throw 0;
                }
                this->type = type;
        }
        enum class InstructionType {
            // The invalid instruction type is for reporting a string
            // that is not an instruction.
            add, multiply, load, output, invalid
        };
        static bool isInstructionName(string str) {
            tolower(str);
            return str == "add" || 
                str == "mult" ||
                str == "load" ||
                str == "output";
        }
        static InstructionType whichInstruction(string str) {
            tolower(str);
            if (str == "add") {
                return InstructionType::add;
            } else if (str == "mult") {
                return InstructionType::multiply;
            } else if (str == "load") {
                return InstructionType::load;
            } else if (str == "output") {
                return InstructionType::output;
            } else {
                return InstructionType::invalid;
            }
        }
        InstructionType type;
        string operand1;
        // Not all instructions use the 2nd operand.
        string operand2;
        string operand3;
};

class InterpreterError {};

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
bool tokenContains(Token token, string str) {
    return token == str;
}

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
vector<Token> tokenizeProgram(string filename) {
    ifstream file(filename);
    vector<Token> tokens;
    char currentCharacter = '\0';
    string currentWord = "";
    string whitespace = " \t\n\r";
    while (file.get(currentCharacter)) {
        if (whitespace.find_first_of(currentCharacter)) {
            if (currentWord == "") continue;
            if (isInstructionName(currentWord)) {
                tolower(currentWord);
            }
            tokens.push_back(Token(currentWord));
            currentWord = "";
        } else {
            currentWord += currentCharacter;
        }
    }
    return tokens;
}

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
vector<Instruction> createInstructionList(vector<Token> tokens) {
    vector<Instruction> instructions;
    int i = 0;
    while(i < tokens.size()) {
        Token current = tokens[i];
        if (current == "add" || current == "mult") {
            // TODO: If there are not two more tokens, throw an error.
            Token destination = tokens[i+1];
            Token op1 = tokens[i+2];
            Token op2 = tokens[i+3];
            instructions.push_back(
                    Instruction(current, destination, op1, op2));
            i += 4;
        } else if (current == "load" || current == "store") {
            // TODO: If there is not one more token throw an error.
            Token destination = tokens[i+1];
            Token op = tokens[i+2];
            instructions.push_back(
                    Instruction(current, destination, op));
            i += 3;
        } else {
            throw InterpreterError();
        }
    }
}

/* Takes a list of instructions, executes those instructions on some
 * initial program state, and then returns the final program state.
 *
 * Parameters
 * ==========
 * Returns
 * =======
 */
ProgramState execute(vector<Instruction> instructions,
                     ProgramState initialState) {
    /* - Create initial program state. How do you place the inputs in
     *   the initial program state? Should we create and pass in an
     *   initial state from elsewhere? Should the language contain
     *   facilities for specifying the initial state, attaching names
     *   to inputs? Might that reveal something? How would the
     *   language provide that?
     *   - The initial program state should consist of the inputs that
     *     all parties receive, and the shares where each party
     *     receives only one.
     *   - Already, we can see that we have to handle some stuff
     *     outside of the program text. We can't deliver different
     *     copies of the program to each party (that would be a shitty
     *     concern for a person to pay attention to, and I think it
     *     would ruin the point. The language and library should keep
     *     track of that concern entirely).
     *   - The easiest solution that would definitely work no matter
     *     what we do is to just transmit the names and values of the
     *     inputs and shares separately. The client part of the
     *     software could just build the two different payloads for
     *     the two different parties (the inputs and the shares).
     *   - We can also treat names as a sort of syntactic sugar. All
     *     the identifiers are numbers, the names get turned into
     *     numbers, just in case somehow using human-readable names
     *     leaks something, somehow.
     *   - We need a hybrid of the two. The values themselves should
     *     not be part of the program, especially because the program
     *     and the program's inputs are not the same thing. However,
     *     the user needs a way of attaching names to inputs and
     *     shares. So I think that we need to add declarations of
     *     shares and inputs to the start of the program, as a header.
     * - The program state pairs names (locations) to values.
     *   Operating on the program state means trnsforming names from
     *   the program to the values the correspond to the names from
     *   the program state.
     * - When all the instructions are finished, return the program
     *   state. Then, we can do whatever transmission logic is needed
     *   with the program outputs to the correct parties.
     * - This is sort of like the scheme interpreter you built. The
     *   nice thing about the scheme interpreter is that the input to
     *   the interpreter was already machine-readable. This is the
     *   function that does the machine readable stuff. So all you
     *   really have to make is instruction-to-action, since the
     *   entire language is 4 primitives (maybe more for starting
     *   definitions).
     */
    return ProgramState();
}
