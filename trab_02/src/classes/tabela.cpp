#include <tabela.h>
#include <stdexcept>

using namespace std;

Tabela::Tabela(string nome_tabela, int qnt_cols) : nome_tabela(nome_tabela), qnt_cols(qnt_cols), qnt_pags(0) {}

void Tabela::adicionarPagina(Pagina& pagina) {
    paginas[qnt_pags] = pagina;
    qnt_pags++;
}
Pagina Tabela::getPagina(int indice) {
    if (indice >= 0 && indice < qnt_pags) {
        return paginas[indice];
    }
    throw out_of_range("Página não encontrada");
}
int Tabela::getQuantidadePaginas() {
    return qnt_pags;
}
