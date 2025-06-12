#include "../../header/tabela.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

using namespace std;

Tabela::Tabela(string nome_tabela, int qnt_cols) : nome_tabela(nome_tabela), qnt_cols(qnt_cols), qnt_pags(0) {}

void Tabela::adicionarPagina(Pagina& pagina) {
    paginas[qnt_pags] = pagina;
    qnt_pags++;
}
Pagina& Tabela::getPagina(int indice) {
    if (indice >= 0 && indice < qnt_pags) {
        return paginas[indice];
    }
    throw out_of_range("Página não encontrada");
}
int Tabela::getQuantidadePaginas() {
    return qnt_pags;
}

void Tabela::carregarDados(){
    ifstream file(nome_tabela);
    if (!file.is_open()) {
        throw runtime_error("Nao foi possivel abrir o arquivo: " + nome_tabela);
    }

    string linha;
    // Ler o cabeçalho
    if (getline(file, linha)) {
        stringstream ss(linha);
        string item;
        while (getline(ss, item, ',')) {
            header.push_back(item);
        }
        qnt_cols = header.size();
    } else {
        return; // Arquivo vazio
    }

    Pagina pagina_atual;
    while (getline(file, linha)) {
        stringstream ss(linha);
        string item;
        vector<string> valores_linha;
        while (getline(ss, item, ',')) {
            valores_linha.push_back(item);
        }

        if (valores_linha.size() == qnt_cols) {
            Tupla nova_tupla(valores_linha);
            if (pagina_atual.isFull()) {
                paginas.push_back(pagina_atual);
                num_operacoes_IO++; 
                pagina_atual.clear();
            }
            pagina_atual.addTupla(nova_tupla);
        }
    }

    if (pagina_atual.qnt_tuplas_ocupadas > 0) {
        paginas.push_back(pagina_atual);
        num_operacoes_IO++;
    }
    
    file.close();
    cout << "Tabela " << nome_tabela << " carregada. " << paginas.size() << " paginas." << endl;
}

