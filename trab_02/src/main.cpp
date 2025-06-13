#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include "../header/tabela.h"
#include "../header/externalmerge.h"

using namespace std;

int main() {

    Tabela tabela_uva("uva_teste.csv", 0);
    Tabela tabela_vinho("vinho_teste.csv", 0);

    try {
        cout << "\nCarregando dados da tabela Uva..." << endl;
        tabela_uva.carregarDados();
        
        cout << "Carregando dados da tabela Vinho..." << endl;
        tabela_vinho.carregarDados();
    } catch (const runtime_error& e) {
        cerr << "Erro ao carregar arquivos: " << e.what() << endl;
        return 1;
    }

    // Exemplo: Uva ▷◁(uva_id = uva_id) Vinho
    string coluna_join_uva = "uva_id";
    string coluna_join_vinho = "uva_id";
    string arquivo_saida = "resultado_join_uva_vinho.csv";

    int indice_uva = tabela_uva.getColunaIndice(coluna_join_uva);
    int indice_vinho = tabela_vinho.getColunaIndice(coluna_join_vinho);

    if (indice_uva == -1 || indice_vinho == -1) {
        cerr << "Erro: Uma das colunas de junção não foi encontrada no cabeçalho dos arquivos." << endl;
        return 1;
    }

    tuple<int, int, int> resultados = sort_merge_join(&tabela_uva, &tabela_vinho, indice_uva, indice_vinho, arquivo_saida);

    cout << "\n==============================================" << endl;
    cout << "      RESULTADOS FINAIS DA OPERAÇÃO" << endl;
    cout << "==============================================" << endl;
    cout << "(1) Quantidade de IO's (páginas lidas): " << get<0>(resultados) << endl;
    cout << "(2) Quantidade de páginas gravadas em disco: " << get<1>(resultados) << endl;
    cout << "(3) Quantidade de tuplas geradas na junção: " << get<2>(resultados) << endl;
    cout << "As tuplas resultantes foram salvas em: '" << arquivo_saida << "'" << endl;
    cout << "==============================================" << endl;

    return 0;
}