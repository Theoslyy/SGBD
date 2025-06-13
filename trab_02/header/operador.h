#pragma once
#include <string>
#include <tuple>
#include "tabela.h"

using namespace std;

class Operador {
private:
    Tabela* t1;
    Tabela* t2;
    string col_t1;
    string col_t2;
    string arquivo_saida;

    int paginas_lidas;
    int paginas_gravadas;
    int tuplas_geradas;

public:
    Operador(Tabela& tabela1, Tabela& tabela2, string coluna1, string coluna2);

    void executar();
    int numPagsGeradas() const;
    int numIOExecutados() const;
    int numTuplasGeradas() const;
    void salvarTuplasGeradas(const string& nome_arquivo);
};