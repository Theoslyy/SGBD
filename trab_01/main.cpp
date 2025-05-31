#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
using namespace std;
#include "arvoreBPlus.cpp"

int main() {
    ifstream input("in.txt");
    ofstream output("out.txt");
    fstream indexFile("index.txt", std::ios::in | std::ios::out | std::ios::trunc);
    string line;
    getline(input, line);
    output << line << endl;

    int grau = stoi(line.substr(4)); 
    BTree arvore(grau, indexFile, "dados.txt");


    while (getline(input, line)) {
        std::cout << "chegou aqui\n" << line << std::endl;
        if (line.rfind("INC:", 0) == 0) {
            int valor = stoi(line.substr(4));
            printf("chegou aqui1\n");
            arvore.insert(&arvore, valor);
            printf("chegou aqui\n");
            output << "INC:" << valor << "/1\n"; 
            printf("chegou aqui\n");
        } else if (line.rfind("BUS=:", 0) == 0) {
            int valor = stoi(line.substr(5));
            int id = arvore.buscarIgualdade(&arvore, valor);
            if (id == -1){
                cout << "Não há esta chave";
                output << "BUS=:" << valor << "/" << (id != -1 ? 1 : 0) << "\n";
            }
            else output << "BUS=:" << valor << "/" << (id != -1 ? 1 : 0) << "\n";
        }
        printf("chegou aqui fora do if\n");
    }
    cout << arvore.getAltura() << ":a altura é"; 
    output << "H/" << arvore.getAltura() << "\n";

    return 0;
}