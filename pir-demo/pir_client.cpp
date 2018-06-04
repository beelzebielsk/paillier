#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <NTL/ZZ.h>
#include <paillier/paillier.h>
#include <paillier/data.h>
#include <paillier/utility.h>

using namespace std;
using namespace NTL;

//check to see if a certain file exists
bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

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

    cout << "Client starts waiting on 1st server's file." << endl;
    while(!is_file_exist("server1output.txt"));
    cout << "Client finished waiting on 1st server's file." << endl;
    ifstream inFile{"server1output.txt"};
    cout << "Client starts waiting on 1st server's output." << endl;
    inFile.peek();
    while (inFile.eof()) {
        inFile.clear();
        inFile.peek();
    }
    cout << "Client finished waiting on 1st server's output." << endl;
    inFile >> answer1;

    cout << "Client starts waiting on 2st server's file." << endl;
    while(!is_file_exist("server2output.txt"));
    cout << "Client finished waiting on 2st server's file." << endl;
    inFile = ifstream{"server2output.txt"};
    cout << "Client starts waiting on 2st server's output." << endl;
    inFile.peek();
    while (inFile.eof()) {
        inFile.clear();
        inFile.peek();
    }
    cout << "Client finished waiting on 2st server's output." << endl;
    inFile >> answer2;

    cout << "Fin: answer is " << answer1 + answer2 << endl;
    
    return 0;
}
