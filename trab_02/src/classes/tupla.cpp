#include "tupla.h"
#include <stdexcept>
using namespace std;

Tupla::Tupla(): qnt_cols(0){}
Tupla::Tupla(int qnt_cols) : qnt_cols(qnt_cols) {linha.resize(qnt_cols);}
Tupla::Tupla(vector<string> linha, int qnt_cols) : linha(linha), qnt_cols(qnt_cols) {}

void Tupla::setLinha(vector<string> linha){
    this->linha = linha;
}

vector<string> Tupla::getLinha(){
    return linha;
}

string Tupla::getColuna(int posicao){
    if (posicao >= 0 && posicao < linha.size()) {
        return linha[posicao];
    }
    throw out_of_range("Posição inválida para getColuna");
}

void Tupla::setColuna(int posicao, string valor){
    if (posicao >= 0 && posicao < linha.size()) {
        linha[posicao] = valor;
    } else {
        throw out_of_range("Posição inválida para setColuna");
    }
}