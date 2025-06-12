#include "../header/operacao.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

Operacao::Operacao(Tabela& t1, Tabela& t2, const std::string& c1, const std::string& c2)
    : tabela1(t1), tabela2(t2), nome_coluna1(c1), nome_coluna2(c2) {
    this->io_total = 0;
    this->paginas_geradas = 0;
    this->tuplas_geradas = 0;
    this->join_col_idx1 = -1;
    this->join_col_idx2 = -1;
}
vector<Tupla> Operacao::lerBlocoParaMemoria(Tabela& tabela, int& indicePaginaAtual) {
    vector<Tupla> buffer;
    const int MAX_PAGINAS_EM_MEMORIA = 4; 
    int paginasLidas = 0;

    while (indicePaginaAtual < tabela.getQuantidadePaginas() && paginasLidas < MAX_PAGINAS_EM_MEMORIA) {
        Pagina& p = tabela.getPagina(indicePaginaAtual);
        for (int j = 0; j < p.qnt_tuplas_ocupadas; ++j) {
            buffer.push_back(p.getTupla(j));
        }
        paginasLidas++;
        this->io_total++;
        indicePaginaAtual++;
    }
    return buffer;
}


void Operacao::executarExtenalMerge() {
    // rapaz...
}

void Operacao::salvarResultado(const std::string& nome_arquivo) {
    // Precisa da segunda parte
}

long long Operacao::numPagsGeradas() const {
    return this->paginas_geradas;
}

long long Operacao::numIOsExecutados() const {
    return this->io_total;
}

long long Operacao::numTuplasGeradas() const {
    return this->tuplas_geradas;
}