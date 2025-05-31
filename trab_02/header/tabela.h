#ifndef TABELA_H
#define TABELA_H

#include <pagina.h>
#include <tupla.h>
#include <vector>

using namespace std;

class Tabela {
    public:
        string nome_tabela;
        array <Pagina, 4> paginas;
        int qnt_pags;
        int qnt_cols;

        Tabela(string nome_tabela, int qnt_cols);
        void adicionarPagina( Pagina& pagina);
        Pagina getPagina(int indice);
        int getQuantidadePaginas();
};

#endif