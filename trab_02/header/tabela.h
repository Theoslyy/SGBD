#ifndef TABELA_H
#define TABELA_H

#include "pagina.h"
#include "tupla.h"
#include <vector>

using namespace std;

class Tabela {
    public:
        string nome_tabela;
        vector <Pagina> paginas;
        vector <string> header;
        int qnt_pags;
        int qnt_cols;
        int num_operacoes_IO;

        Tabela(string nome_tabela, int qnt_cols);
        void adicionarPagina( Pagina& pagina);
        Pagina& getPagina(int indice);
        int getQuantidadePaginas();
        void carregarDados();
        int getColunaIndice(string nome_coluna);
};

#endif