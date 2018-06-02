#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include <NTL/ZZ.h>
#include <assert.h>
#include <paillier/paillier.h>
#include <paillier/data.h>
#include <paillier/utility.h>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

using namespace std;
using namespace NTL;

//check to see if a certain file exists
bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

// share1, share lambda, input1, inputs
// creating share1 and share lambda into input types with value 0s. bits = vector of 512 0s and modulus = 0
void sendToServer(Memory mem, vector<Input> inputs, Paillier p,
                  const char *fileName)
{
    ofstream myfile(fileName);
    Input one{p.encrypt((ZZ)1), p.encryptBits((ZZ)1), p.getModulus()};

    myfile << p.getModulus() << " "
        << one << " "
        << mem << " ";
    for (Input input : inputs) {
        myfile << input << " ";
    }
    
    myfile.close();
}

// creates a vector of inputs from client index binary
vector<Input> createInputs(vector<bool> index_binary, Paillier p)
{
    vector<bool> b = index_binary;
    vector<Input> inputs;

    Paillier paillier = p;

    for(int i = 0; i < b.size(); i++)
    {
        vector<ZZ> z = paillier.encryptBits(ZZ(b[i]));
        ZZ encryption = p.encrypt(ZZ(b[i]));
        inputs.push_back(Input(encryption, z, paillier.getModulus()));
    }
    return inputs;
}

int main (int argc, char** argv) {
    
    cout << "Hey." << endl;
    Paillier p = Paillier(512);
    ZZ index = (ZZ)4;
    vector<Input> inputs = createInputs(ZZToBits(index), p);
    
    auto oneShare = share(ZZ(1), p.getModulus());
    auto lambdaShare = share(p.getLambda(), p.getModulus());
    Memory mem1{oneShare.first, lambdaShare.first};
    Memory mem2{oneShare.second, lambdaShare.second};

    sendToServer(mem1, inputs, p, "server1.txt");
    sendToServer(mem2, inputs, p, "server2.txt");
    ZZ answer1, answer2;

    /*
    ifstream inFile("serveroutput.txt", 
            std::ios_base::in | std::ios_base::app);
    */
    cin >> answer1 >> answer2;
    /*
    inFile.peek();
    while (inFile.eof()) {
        inFile.clear();
        inFile.peek();
    }
    // The actual order of reading doesn't matter, because the client
     * need only add the two shares together.
     ///
    inFile >> answer1;
    inFile.peek();
    while (inFile.eof()) {
        inFile.clear();
        inFile.peek();
    }
    inFile >> answer2;
    */

    cout << "Fin: answer is " << answer1 + answer2 << endl;
    
    return 0;
}
