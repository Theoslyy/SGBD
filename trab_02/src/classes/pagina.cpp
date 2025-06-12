#include "../../header/pagina.h"
#include <stdexcept>

using namespace std;

Pagina::Pagina(){
    this->qnt_tuplas_ocupadas = 0;
}

Tupla& Pagina::getTupla(int posicao) {
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

void Pagina::addTupla(Tupla tupla) {
    if (!isFull()) {
        tuplas[qnt_tuplas_ocupadas] = tupla;
        qnt_tuplas_ocupadas++;
    } else {
        throw out_of_range("Página cheia");
    }
}
bool Pagina::isFull() {
    return qnt_tuplas_ocupadas >= tuplas.size();
}

void Pagina::clear(){
    qnt_tuplas_ocupadas = 0;
}