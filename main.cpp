#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
using namespace std;
#include "arvoreBPlus.cpp"

int main() {
    std::ifstream input("in.txt");
    std::ofstream output("out.txt");
    std::fstream indexFile("index.txt", std::ios::in | std::ios::out | std::ios::trunc); // Use trunc to clear file
    
    std::string line;
    getline(input, line);
    output << line << endl;

    int grau = stoi(line.substr(4)); 
    BTree arvore(grau, indexFile, "dados.txt");

    while (getline(input, line)) {
        if (line.rfind("INC:", 0) == 0) {
            int valor = stoi(line.substr(4));
            arvore.insert(&arvore, valor);
            output << "INC:" << valor << "/1\n"; 
        } else if (line.rfind("BUS=:", 0) == 0) {
            int valor = stoi(line.substr(5));
            int id = arvore.buscar(&arvore, valor);
            output << "BUS=:" << valor << "/" << (id != -1 ? 1 : 0) << "\n";
        }
    }
    output << "H/" << arvore.getAltura() << "\n";

    return 0;
}