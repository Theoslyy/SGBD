#ifndef OPERACAO_H
#define OPERACAO_H

#include "tabela.h"
#include <string>
#include <vector>

class Operacao {
private:
    Tabela& tabela1;
    Tabela& tabela2;
    std::string nome_coluna1;
    std::string nome_coluna2;
    int join_col_idx1;
    int join_col_idx2;

    long long io_total;
    long long paginas_geradas;
    long long tuplas_geradas;
    
    Tabela tabela_resultado;

    std::string prepararArquivoParaOrdenacao(Tabela& tabela, int join_col_idx, const std::string& sufixo);

public:
    Operacao(Tabela& t1, Tabela& t2, const std::string& c1, const std::string& c2);

    void executar();
    void salvarResultado(const std::string& nome_arquivo);

    long long numPagsGeradas() const;
    long long numIOsExecutados() const;
    long long numTuplasGeradas() const;
};

#endif // OPERACAO_H