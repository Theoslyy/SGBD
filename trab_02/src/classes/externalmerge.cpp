#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../header/tupla.h"
#include "../header/pagina.h"
#include "../header/tabela.h"
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

//variaveis auxiliares
const int TUPLAS_PAG = 10;
const int MAX_PAG = 4; //é o B
const int MAX_TUPLA = 10;
const int MAX_BUFFER = MAX_PAG * MAX_TUPLA; // 40 tuplas
int page_reads; int page_writes; 

// Ordena e salva uma run no disco
void sort_run(vector<Tupla>& run, int run_id, int coluna) {
    //aqui, o vetor de strings run é o vetor com as tuplas que queremos ordenar no momento
    vector<Tupla> sorted = run;
    //explicando esse sort embaixo:
    /*
    O sort é uma função do C++ do formato:
    sort(inicio, fim, função_de_comparação)
    função de comparação genérica: 
    [](const string& a, const string& b) {
    return a < b;
    }
    */
    sort(sorted.begin(), sorted.end(), [coluna](Tupla& a, Tupla& b) {
        int colA = stoi(a.getColuna(coluna));
        int colB = stoi(b.getColuna(coluna));
        return colA < colB; // pega a posição da tupla que corresponde ao índice primário e compara. 
    });
    int tuplas_total = 0;
    //gravamos o output num .txt
    ofstream out("run_" + to_string(run_id) + ".txt");
    for (auto& r : sorted){
        const vector<string>& linha = r.getLinha();
        for (size_t i = 0; i < linha.size(); ++i) {
            out << linha[i];
            if (i < linha.size() - 1) out << ","; // comma between values
        }
        out << "\n";
        tuplas_total++; 
    }
    out.close();
    int paginas_escritas = (tuplas_total + TUPLAS_PAG - 1) / TUPLAS_PAG;
    page_writes += paginas_escritas;
}

// Lê tuplas do arquivo até o limite de memória. Cada página só tem 10 tuplas e o javam não impôs limite
// Logo, podemos ler todas as tuplas sem problema. O max_count é desnecessário
vector<Tupla> read_buffer(Tabela* tabela, int start_page, int& next_page) {
    vector<Tupla> buffer;
    int paginas_lidas = 0;
    int i = start_page;

    while (i < tabela->qnt_pags && paginas_lidas < MAX_PAG) {
        Pagina& p = tabela->getPagina(i);
        for (int j = 0; j < p.qnt_tuplas_ocupadas; ++j) {
            buffer.push_back(p.getTupla(j));
        }
        paginas_lidas++;
        page_reads++;
        i++;
    }

    next_page = (i < tabela->qnt_pags) ? i : -1; //operador ternario... <3
    return buffer;
}
//poderia nao ser uma funcao, mas ia deixar o codigo do merge_runs feio :(
vector<string> split_csv(const string& line) {
    vector<string> cols;
    stringstream ss(line);
    string item;
    while (getline(ss, item, ',')) {
        cols.push_back(item);
    }
    return cols;
}

// Merge de múltiplos arquivos ordenados
void merge_runs(const vector<string>& run_files, int coluna, const string& output_file) {
    //a quantidade de páginas lidas nessa etapa será a quantidade de páginas escritas na etapa anterior
    vector<ifstream> inputs(run_files.size());
    vector<string> heads(run_files.size());
    vector<bool> active(run_files.size(), true);

    for (size_t i = 0; i < run_files.size(); ++i) {
        inputs[i].open(run_files[i]);
        if (!getline(inputs[i], heads[i])) {
            active[i] = false;
        }
    }

    ofstream out(output_file);
    vector<string> output_buffer;
    int total_pags_escritas = 0;

    while(any_of(active.begin(), active.end(), [](bool a) { return a; })) {
        int min_val = INT64_MAX;
        size_t min_idx = -1;

        for (size_t i = 0; i < heads.size(); ++i) {
            if (active[i]) {
                vector<string> cols = split_csv(heads[i]); //dividir a linha em colunas 
                int chave = stoi(cols[coluna]); //acessar o valor da coluna que queremos para ordenar baseando-se nessa coluna
                if (min_idx == -1 || chave < min_val) {
                    min_val = chave;
                    min_idx = i;
                }
            }
        }

        if (min_idx == -1) break;

        output_buffer.push_back(heads[min_idx]);

        if (output_buffer.size() == MAX_TUPLA) {
            for (const auto& linha : output_buffer) {
                out << linha << "\n";
            }
            total_pags_escritas++;
            output_buffer.clear();
        }
        //check se a próxima linha é não nula
        if (!getline(inputs[min_idx], heads[min_idx])) {
            active[min_idx] = false;
        }
    }

    // Escreve o restante
    if(!output_buffer.empty()){
        total_pags_escritas++;
        for (const auto& linha : output_buffer) {
        out << linha << "\n";
    }
    }
    for (auto& in : inputs) in.close();
    out.close();
    page_writes += total_pags_escritas;
}

//funcao auxiliar para contar quantas tuplas tem numa run e descobrir com quantas paginas estamos lidando
//na prática, isso era para contar, tambem, como I/Os
//e, daí, estariamos lendo, na verdade, o dobro de páginas
int contar_tuplas(const string& filename) {
    ifstream in(filename);
    int count = 0;
    string linha;
    while (getline(in, linha)) count++;
    return count;
}

// Algoritmo de ordenação externa
pair<int,int> external_merge_sort(Tabela* tabela, int coluna, string& output_file) { 
    int next_page = 0;
    page_reads = 0;
    page_writes = 0;
    vector<string> run_files;
    int run_id = 0;
    while (next_page != -1) {
        vector<Tupla> buffer = read_buffer(tabela, next_page, next_page);

        if (buffer.empty()) break;

        sort_run(buffer, run_id, coluna);
        run_files.push_back("run_" + to_string(run_id) + ".txt");
        run_id++;
    }
    // ordena conjunto de runs!
    int lidas;

    int contador_rodadas = 0;
    while (run_files.size() > 1) {
        contador_rodadas++;
        vector<string> next_run_files;
        for (size_t i = 0; i < run_files.size(); i += 3) {
            //ordenamos de 3 em 3 e mantemos uma página de OUT
            vector<string> group;
            lidas = 0;
            for (size_t j = i; j < i + 3 && j < run_files.size(); j++) {
                lidas += (contar_tuplas(run_files[j]) + 9)/10; //qtd de paginas nessa run
                group.push_back(run_files[j]);
            }
            string out_run = "run_tmp_" + to_string(contador_rodadas) + "_" + to_string(i / 3) + ".txt";
            merge_runs(group, coluna, out_run); ///ordenamos as páginas e as escrevemos no out run.
            page_reads += lidas; 
            next_run_files.push_back(out_run); 
        }
        //limpo o vetor de run_files
        for (const string& f : run_files)
            fs::remove(f);
        //as runs a serem ordenadas agora são as que estavam no vetor next_run_files
        run_files = next_run_files;
        //assim, essa parte da função funcionará da seguinte forma?
        /*
        todas as runs terão, inicialmente, tam de duas páginas
        então, iremos ordenar essas runs 2 a 2, criando runs de 4 páginas
        Daí, iremos ordenar essas runs 2 a 2 novamente, só que, agora, cada run terá tamanho 4
        Logo, precisamos carregar somente parcialmente essas runs na memória, isso será feito na func merge_runs
        Faremos isso até a run ter tamanho do numero de elementos. 
        */
    }
    if (!run_files.empty()) fs::rename(run_files[0], output_file); //Restará só uma run no vetor de runs.. que é o vetor todo! 
    return make_pair(page_reads, page_writes);

}
int comparar_criar_tuplas_ordenadas(const string& arquivo1, const string& arquivo2, int coluna1, int coluna2, const string& output_file, int& join_reads, int& join_writes) {
    ifstream in1(arquivo1);
    ifstream in2(arquivo2);
    ofstream out(output_file);

    // Linha de leitura em cada arq
    string linha1, linha2;
    int tuplas_geradas = 0;
    int lines_read1 = 0, lines_read2 = 0;
    vector<string> output_buffer;

    // static_cast<bool>() porque tava dando erro de conversão implícita
    bool has_line1 = static_cast<bool>(getline(in1, linha1));
    if (has_line1) lines_read1++;
    bool has_line2 = static_cast<bool>(getline(in2, linha2));
    if (has_line2) lines_read2++;

    while (has_line1 && has_line2) {
        vector<string> cols1 = split_csv(linha1);
        vector<string> cols2 = split_csv(linha2);
        // conversão para comparar as chaves
        int key1 = stoi(cols1[coluna1]);
        int key2 = stoi(cols2[coluna2]);

        if (key1 < key2) {
            //linha 1 é descartada e lida a próxima linha do arquivo 1
            has_line1 = static_cast<bool>(getline(in1, linha1));
            if (has_line1) lines_read1++;
        } else if (key2 < key1) {
            //linha 2 é descartada e lida a próxima linha do arquivo 2
            has_line2 = static_cast<bool>(getline(in2, linha2));
            if (has_line2) lines_read2++;
        } else {

            int current_key = key1;
            vector<string> block1, block2;

            // Lê blocos de linhas com a mesma chave
            while (has_line1 && stoi(split_csv(linha1)[coluna1]) == current_key) {
                block1.push_back(linha1);
                has_line1 = static_cast<bool>(getline(in1, linha1));
                if (has_line1) lines_read1++;
            }
            // Lê blocos de linhas com a mesma chave do segundo arq
            while (has_line2 && stoi(split_csv(linha2)[coluna2]) == current_key) {
                block2.push_back(linha2);
                has_line2 = static_cast<bool>(getline(in2, linha2));
                if (has_line2) lines_read2++;
            }

            // Cruzamento dos blocos, vai iterar sobre linha do bloco 1 combinando com linha do bloco 2
            for (const auto& r1 : block1) {
                for (const auto& r2 : block2) {
                    output_buffer.push_back(r1 + "," + r2);
                    tuplas_geradas++;
                    if (output_buffer.size() == TUPLAS_PAG) {
                        for (const auto& line : output_buffer) out << line << "\n";
                        output_buffer.clear();
                        join_writes++;
                    }
                }
            }
        }
    }
    // Escreve o restante do buffer
    if (!output_buffer.empty()) {
        for (const auto& line : output_buffer) out << line << "\n";
        join_writes++;
    }

    in1.close();
    in2.close();
    out.close();

    join_reads = ((lines_read1 + TUPLAS_PAG - 1) / TUPLAS_PAG) + ((lines_read2 + TUPLAS_PAG - 1) / TUPLAS_PAG);

    return tuplas_geradas;
}

tuple<int, int, int> sort_merge_join(Tabela* tabela1, Tabela* tabela2, int coluna_join_t1, int coluna_join_t2, const string& output_file) {
    string sorted_file1 = "sorted_temp_t1.txt";
    string sorted_file2 = "sorted_temp_t2.txt";

    pair<int, int> output_primeiro_ems = external_merge_sort(tabela1, coluna_join_t1, sorted_file1);

    pair<int, int> output_segundo_ems = external_merge_sort(tabela2, coluna_join_t2, sorted_file2);

    // contadores para I/O da junção
    int join_reads = 0, join_writes = 0;
    int tuplas_geradas = comparar_criar_tuplas_ordenadas(sorted_file1, sorted_file2, coluna_join_t1, coluna_join_t2, output_file, join_reads, join_writes);

    fs::remove(sorted_file1);
    fs::remove(sorted_file2);

    int total_reads = output_primeiro_ems.first + output_segundo_ems.first + join_reads;
    int total_writes = output_primeiro_ems.second + output_segundo_ems.second + join_writes;

    cout << "\nOperação Sort-Merge Join finalizada." << endl;
    cout << "CUSTO TOTAL I/O: " << total_reads << " leituras, " << total_writes << " escritas." << endl;

    return make_tuple(total_reads, total_writes, tuplas_geradas);
}