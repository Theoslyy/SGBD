#include "../../header/operador.h"
#include "externalmerge.h"
#include <iostream>
#include <utility>

Operador::Operador(Tabela& tabela1, Tabela& tabela2, string coluna1, string coluna2): t1(&tabela1), t2(&tabela2), col_t1(move(coluna1)), col_t2(move(coluna2)),paginas_lidas(0), paginas_gravadas(0), tuplas_geradas(0) {}

void Operador::executar() {
    int indice_col_t1 = t1->getColunaIndice(col_t1);
    int indice_col_t2 = t2->getColunaIndice(col_t2);

    if (indice_col_t1 == -1 || indice_col_t2 == -1) {
        cerr << "Erro: Coluna de junção não encontrada." << endl;
        return;
    }
    
    // Necessário para nomear o arquivo temporário
    this->arquivo_saida = "temporario.csv";

    // Chamando o método principal e atribuindo os resultados aos atributos da classe
    tuple<int, int, int> resultados = sort_merge_join(t1, t2, indice_col_t1, indice_col_t2, this->arquivo_saida);

    this->paginas_lidas = get<0>(resultados);
    this->paginas_gravadas = get<1>(resultados);
    this->tuplas_geradas = get<2>(resultados);
}

int Operador::numPagsGeradas() const {
    return this->paginas_gravadas;
}

int Operador::numIOExecutados() const {
    return this->paginas_lidas;
}

int Operador::numTuplasGeradas() const {
    return this->tuplas_geradas;
}

// Necessário para esquecificar o nome do arquivo na chamada, seguindo o modelo da main
void Operador::salvarTuplasGeradas(const string& nome_arquivo) {
    if (rename(this->arquivo_saida.c_str(), nome_arquivo.c_str()) != 0) {
        cerr << "\nErro ao renomear o arquivo." << endl;
    } else {
        cout << "\nSalvando as tuplas em: '" << nome_arquivo << "'" << endl;
    }
}