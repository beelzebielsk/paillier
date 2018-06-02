#include <iostream>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include <iostream>
#include <fstream>

#include <sstream>
#include <string>
#include <paillier/data.h>

using namespace std;
using namespace NTL;

ZZ execute_server(bool identity);
void readInputFile(string, ZZ&, Input&, Memory&, vector<Input>&);


int main(int argc, char ** args)
{
    int identity = atoi(args[1]);
    ZZ answer = execute_server(identity);
    ofstream o;
    //o.open("serveroutput.txt");
    o.open("serveroutput");
    /*
    if (identity) {
        o.open("server2output.txt");
        o.open("serveroutput.txt");
    } else {
        o.open("server1output.txt");
    }
    */
    o << answer << " ";

    return 0;
}

ZZ execute_server(bool identity) {

    //initalize server databse to this following bit array of length 20
    //vector<bool> server_db {0,1,0,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1};
    vector<bool> server_db {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    ZZ modulus;
    Input one{(ZZ)0, {}, (ZZ)0};
    Memory mem{(ZZ)0, (ZZ)0};
    vector<Input> indexBits;

    cout << "Start reading input file." << endl;

    if (identity) {
        readInputFile("server2.txt", modulus, one, mem, indexBits);
    } else {
        readInputFile("server1.txt", modulus, one, mem, indexBits);
    }

    cout << "Finish reading input file." << endl;

    Value::init(identity, modulus);

    // The arrays will be indexed by the current bit of the current
    // database index. So the first vector will store negations, and
    // the second vector will store the normal bits.
    vector<vector<Input>> allInput{{}, indexBits};
    cout << "Start creating allInput:" << endl;
    for (Input bit : indexBits) {
        //cout << "bit to negate:" << bit << endl;
        allInput[0].push_back(one - bit);
        //cout << "bits in current bit: " << bit.bits.size() << endl;
        //cout << "bits in current negation: " << allInput[0][allInput[0].size() - 1].bits.size() << endl;
        //cout << "bits in negative bit: " << (-bit).bits.size() << endl;
        //cout << "bits in twice bit: " << (bit + bit).bits.size() << endl;
    }
    cout << "Finish creating allInput:" << endl;
    vector<Memory> equalityResults;

    /*
    cout << "Before superfluous ZZ:" << endl;
    long memory = 64;
    while(true) {
        cout << "Allocating " << memory << " bytes of memory:" << endl;
        unsigned char * sponge = new unsigned char[memory];
        delete[] sponge;
        cout << "Successfully allocated." << endl;
        memory *= 2;
    }
    cout << "After superfluous ZZ:" << endl;
    */
    cout << "bits of one:" << one.bits.size() << endl;
    for (unsigned int serverIndex = 0; serverIndex < server_db.size();
         serverIndex++) {
        // These will only contribute 0s contribute to our answer.
        if (server_db[serverIndex] == 0) continue;

        Memory result = mem;
        unsigned int bitsInServerIndex = (sizeof serverIndex) * 8;
        unsigned int numBits = (bitsInServerIndex < indexBits.size() ?  
                bitsInServerIndex : indexBits.size());
        for (unsigned int i = 0; i < numBits; i++) {
            bool bit = (serverIndex >> i) & 1;
            //if (i == 0) {
                //cout << (allInput[bit][i]) << endl;
                cout << (allInput[bit][i].bits.size()) << endl;
            //}
            result = result * allInput[bit][i];
        }
        equalityResults.push_back(result);
    }
    if (equalityResults.empty()) {
        /* If equalityResults is empty, then the db is all 0s, and so
         * the only answer to return is 0, since all answers should
         * have been 0. I'm also assuming that the contents of the
         * database are public, so there's no reason to try and hide
         * the real result of the computation in this case.
         */
        return (ZZ)0;
    }
    Memory finalResult = equalityResults[0];
    for (int i; i < equalityResults.size(); i++) {
        finalResult = finalResult + equalityResults[i];
    }
    return finalResult.value;
}

void readInputFile(
        string filename, ZZ& modulus, Input& one, Memory& mem,
        vector<Input>& indexBits){

    ifstream input_file(filename);
    vector<Input> inputs;

    if(input_file.is_open()) {
        input_file >> modulus
            >> one
            >> mem;

        Input currentInput{(ZZ)0, {}, (ZZ)0};
        while(input_file >> currentInput) 
            inputs.push_back(currentInput);
        indexBits = inputs;
    }
    else cout << "Unable to open " << filename << endl;
}
