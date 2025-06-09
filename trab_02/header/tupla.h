#ifndef TUPLAS_H
#define TUPLAS_H

#include <vector>
#include <string>
using namespace std;

class Tupla{
    public:
        vector<string> linha;
        int qnt_cols;
        Tupla();
        Tupla(int qnt_cols);
        Tupla( vector<string> linha, int qnt_cols);

        void setLinha(vector<string> linhas);
        void setColuna(int posicao, string valor);
        vector<string> getLinha();
        string getColuna(int posicao);
    };
#endif