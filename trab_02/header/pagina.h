#ifndef PAGINA_H
#define PAGINA_H
#include "tupla.h" 
#include <array>

using namespace std;

class Pagina {
    public:
        array <Tupla, 10> tuplas;
        int qnt_tuplas_ocupadas;

        Pagina();
        
        Tupla getTupla(int posicao);
        void setTupla(int posicao, Tupla tupla);
        bool isFull();
        void addTupla(Tupla tupla);
};
#endif