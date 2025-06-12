#ifndef OPERACAO_H
#define OPERACAO_H

#include "tabela.h" 
#include <string>
#include <vector>

class Operacao {
public:
    Operacao(Tabela& t1, Tabela& t2, const std::string& c1, const std::string& c2);

    void executarExtenalMerge();
    void salvarResultado(const std::string& nome_arquivo);
    long long numPagsGeradas() const;
    long long numIOsExecutados() const;
    long long numTuplasGeradas() const;

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

    std::vector<Tupla> lerBlocoParaMemoria(Tabela& tabela, int& indicePaginaAtual);
    void ordenarEGravarRun(std::vector<Tupla>& buffer, int idRun, int join_col_idx, const std::string& prefixo);
    std::vector<std::string> criarRunsOrdenadas(Tabela& tabela, int join_col_idx);
};

#endif // OPERACAO_H