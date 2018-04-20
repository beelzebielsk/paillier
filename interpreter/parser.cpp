#include "parser.h"
#include <sstream>
    using std::stringstream;
#include <iostream>

void tolower(string& str) {
    for (int i = 0; i < str.length(); i++) {
        str[i] = tolower(str[i]);
    }
}

bool inString(string str, char c) {
    return str.find_first_of(c) != string::npos;
}

bool tokenContains(Token token, string str) {
    return token == str;
}

Instruction::Type Instruction::which(string str) {
    tolower(str);
    if (str == "add") {
        return Type::add;
    } else if (str == "mult") {
        return Type::mult;
    } else if (str == "load") {
        return Type::load;
    } else if (str == "output") {
        return Type::output;
    } else {
        return Type::invalid;
    }
}

Declaration::Type Declaration::which(string str) {
    tolower(str);
    if (str == "input") {
        return Type::input;
    } else if (str == "share") {
        return Type::share;
    } else {
        return Type::invalid;
    }
}

vector<Token> tokenizeProgram(istream& file) {
    vector<Token> tokens;
    char currentCharacter = '\0';
    string currentWord = "";
    string whitespace = " \t\n\r";
    while (file.get(currentCharacter)) {
        if (inString(whitespace, currentCharacter)) {
            if (currentWord == "") continue;
            if (Instruction::isInstructionName(currentWord)) {
                tolower(currentWord);
            }
            tokens.push_back(Token(currentWord));
            currentWord = "";
        } else {
            currentWord += currentCharacter;
        }
    }
    // File may not end in whitespace.
    if (currentWord != "") tokens.push_back(Token(currentWord));
    return tokens;
}

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
        } else if (current == "load" || current == "output") {
            // TODO: If there is not one more token throw an error.
            Token destination = tokens[i+1];
            Token op = tokens[i+2];
            instructions.push_back(
                    Instruction(current, destination, op));
            i += 3;
        } else {
            stringstream s;
            s << "Token problem, position " << i 
                << ", word " << current;
            throw InterpreterError(s.str());
        }
    }
    return instructions;
}

ProgramState execute(vector<Instruction> instructions,
                     ProgramState initialState) {
    /* - The initial state comes from somewhere else. There is no way
     *   to tell the values attached to the names from just the
     *   program (else we'd be breaking security).
     * - The program will contain declarations of names as being
     *   either an input or a share, but that will not make it to
     *   here. Those declarations will just modify the program's
     *   initial state.
     * - For each instruction, get the values bound to identifiers
     *   from the curent program state, then set the value of the
     *   destination identifier with the result of the operation.
     * - You start off from the initial state of the program, modify
     *   it with the instructions, and then return the final state to
     *   the caller.
     */
    for (int i = 0; i < instructions.size(); i++) {
        Instruction current = instructions[i];
        std::cout << "Instruction " << i << ": " 
            << current << std::endl;
        if (current.type == Instruction::add) {
            initialState[current.operand1] = 
                initialState[current.operand2] + 
                initialState[current.operand3];
        } else if (current.type == Instruction::mult) { 
            initialState[current.operand1] = 
                initialState[current.operand2] * 
                initialState[current.operand3];
        } else if (current.type == Instruction::load) { 
            initialState[current.operand1] = 
                initialState[current.operand2];
        } else if (current.type == Instruction::output) { 
            stringstream s;
            s << "Instruction # " << i 
                << ": output not implemented yet!";
            throw InterpreterError(s.str());
            // TODO: Figure out how output works.
        }
    }
    return initialState;
}
