#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "arvorebplus.cpp"
#include "buffer.cpp"
int main() {
    fstream indexFile("index.txt", ios::in | ios::out);
    fstream dataFile("vinhos.csv", ios::in | ios::out);
    
    BufferManager buffer(indexFile, dataFile);
    //BTree arvore(buffer, 2); 

    // Processamento de operações
    ifstream input("in.txt");
    ofstream output("out.txt");
    
    string line;

}