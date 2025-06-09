#include <iostream>
#include <fstream>  
#include <sstream> 
#include <vector>
#include <string>

#include "../header/tupla.h"
#include "../header/pagina.h"
#include "../header/tabela.h"

using namespace std;

// Main gerada por IA para testar as classes, Revisar/ Alterar

void exibirTupla(const Tupla& t) {
    vector<string> linha = t.linha; 
    for (size_t i = 0; i < linha.size(); ++i) {
        cout << linha[i] << (i == linha.size() - 1 ? "" : " | ");
    }
    cout << endl;
}

void exibirPagina(const Pagina& p) {
    cout << "  --- Pagina (Tuplas Ocupadas: " << p.qnt_tuplas_ocupadas << "/10) ---" << endl;
    for (int i = 0; i < p.qnt_tuplas_ocupadas; ++i) {
        cout << "    Tupla " << i << ": ";
        Tupla tempTupla = p.tuplas[i]; 
        exibirTupla(tempTupla);
    }
    cout << "  ---------------------------------------" << endl;
}

void exibirTabela(const Tabela& tbl) {
    cout << "=== Tabela: " << tbl.nome_tabela << " ===" << endl;
    cout << "Colunas: " << tbl.qnt_cols << endl;
    cout << "Páginas no Buffer: " << tbl.qnt_pags << "/4" << endl;
    for (int i = 0; i < tbl.qnt_pags; ++i) {
        cout << " Buffer Pagina " << i << ":" << endl;
        Pagina tempPagina = tbl.paginas[i]; 
        exibirPagina(tempPagina);
    }
    cout << "==========================" << endl;
}

// --- Função para Parsear Linha CSV ---
vector<string> parseCSVLine(const string& linha_csv) {
    vector<string> atributos;
    stringstream ss(linha_csv);
    string atributo;
    while (getline(ss, atributo, ',')) {
        atributos.push_back(atributo);
    }
    return atributos;
}

int main() {
    const int VINHO_QNT_COLS = 5; 
    Tabela tabela_vinhos("vinhos", VINHO_QNT_COLS);

    ifstream arquivo_csv("vinhos.csv");
    if (!arquivo_csv.is_open()) {
        cerr << "Erro ao abrir o arquivo vinhos.csv!" << endl;
        return 1;
    }

    string linha_csv;
    Pagina pagina_atual; 

    cout << "Carregando dados de vinhos.csv..." << endl;

    while (getline(arquivo_csv, linha_csv)) {
        if (linha_csv.empty()) continue; 

        vector<string> atributos = parseCSVLine(linha_csv);

        if (static_cast<int>(atributos.size()) == VINHO_QNT_COLS) {
            Tupla nova_tupla(atributos, VINHO_QNT_COLS);
            
            if (pagina_atual.isFull()) {
                if (tabela_vinhos.getQuantidadePaginas() < 4) {
                    tabela_vinhos.adicionarPagina(pagina_atual);
                } else {
                    cout << "Buffer da tabela 'vinhos' está cheio (4 páginas). Mais dados do CSV não serão carregados neste buffer." << endl;
                    break;
                }
                pagina_atual = Pagina(); 
            }
            pagina_atual.addTupla(nova_tupla);
        } else {
            cerr << "Aviso: Linha com quantidade de colunas inesperada ignorada: " << linha_csv << endl;
        }
    }

    if (pagina_atual.qnt_tuplas_ocupadas > 0 && tabela_vinhos.getQuantidadePaginas() < 4) {
        tabela_vinhos.adicionarPagina(pagina_atual);
    }

    arquivo_csv.close();
    cout << "Dados carregados." << endl << endl;

    exibirTabela(tabela_vinhos);

    return 0;
}