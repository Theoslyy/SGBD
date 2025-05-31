#include "pagina.h"
#include <stdexcept>

using namespace std;

Pagina::Pagina(){
    this->qnt_tuplas_ocupadas = 0;
}

Tupla Pagina::getTupla(int posicao) {
    if (posicao >= 0 && posicao < qnt_tuplas_ocupadas) {
        return tuplas[posicao];
    } else {
        throw out_of_range("Não há tuplas nessa posição");
    }
}

void Pagina::setTupla(int posicao, Tupla tupla) {
    if (posicao >= 0 && posicao < qnt_tuplas_ocupadas) {
        tuplas[posicao] = tupla;
    } else {
        throw out_of_range("Não há tuplas nessa posição");
    }
}

bool Pagina::isFull() {
    return qnt_tuplas_ocupadas >= tuplas.size();
}