#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

const int B = 4; // número de buffers disponíveis
const int TUPLAS_PAG = 10;

using Tupla = string;
using Pagina = vector<Tupla>;

// Ordena e salva uma run no disco
void sort_run(vector<Tupla>& run, int run_id) {
    vector<Tupla> sorted = run;

    // tratando como se as chaves tivessem separadas por vírgulas, isso é temporário. 
    sort(sorted.begin(), sorted.end(), [](const Tupla& a, const Tupla& b) {
        return a.substr(0, a.find(',')) < b.substr(0, b.find(','));
    });

    ofstream out("run_" + to_string(run_id) + ".txt");
    for (const auto& r : sorted)
        out << r << "\n";
    out.close();
}

// Lê tuplas do arquivo até o limite de memória. Cada página só tem 10 tuplas e o javam não impôs limite
// Nisso, logo, podemos ler todas as tuplas sem problema. O max_count é desnecessário
vector<Tupla> read_Tuplas(ifstream& in, int max_count) {
    vector<Tupla> Tuplas;
    string linha; 
    while (Tuplas.size() < max_count && getline(in, linha)) {
        Tuplas.push_back(linha);
    }
    return Tuplas;
}

// Merge de múltiplos arquivos ordenados
void merge_runs(const vector<string>& run_files, const string& output_file) {
    vector<ifstream> inputs(run_files.size()); //stream dos inpupts 
    vector<string> heads(run_files.size()); //os números quue estamos analisando agora de cada input
    vector<bool> active(run_files.size(), true); //quais inputs ainda tem que ser lidos

    // Abrir cada arquivo e ler a primeira linha
    for (size_t i = 0; i < run_files.size(); ++i) {
        inputs[i].open(run_files[i]);
        if (!getline(inputs[i], heads[i])) {
            active[i] = false;
        }
    }

    ofstream out(output_file);
    // enquanto eu encontrar uma stream ativa, eu repito!
    // o any_of é basicamente uma scan do vetor que retorna true se pelo menos um bool for verdadeiro
    while (any_of(active.begin(), active.end(), [](bool a) { return a; })) {
        string min_val = "";
        size_t min_idx = -1;
        // acha o menor valor das streams ativa e salva-o para gravar
        for (size_t i = 0; i < heads.size(); ++i) {
            if (active[i]) {
                string chave = heads[i].substr(0, heads[i].find(','));
                if (min_idx == -1 || chave < min_val) {
                    min_val = chave;
                    min_idx = i;
                }
            }
        }

        if (min_idx == -1) break;

        out << heads[min_idx] << "\n";

        if (!getline(inputs[min_idx], heads[min_idx])) {
            active[min_idx] = false;
        }
    }

    for (auto& in : inputs) in.close();
    out.close();
}

// Algoritmo completo de ordenação externa
void external_merge_sort(const string& input_file, const string& output_file) {
    ifstream in(input_file);
    vector<string> run_files;
    int run_id = 0;
    // ordena as runs!
    while (!in.eof()) {
        vector<Tupla> buffer = read_Tuplas(in, B * TUPLAS_PAG); // le um bloco do tamanho da pag*tuplas por pag
        if (buffer.empty()) break;
        sort_run(buffer, run_id);
        run_files.push_back("run_" + to_string(run_id) + ".txt");
        ++run_id;
    }
    in.close();
    // ordena conjunto de runs!
    while (run_files.size() > 1) {
        vector<string> next_run_files;
        for (size_t i = 0; i < run_files.size(); i += (B - 1)) {
            //fazemos um grupo de tam B-1 ou, caso não tenha tudo isso de runs, fazemos um grupo com o que tem
            vector<string> group(run_files.begin() + i,
                                 run_files.begin() + min(i + B - 1, run_files.size()));
            string out_run = "run_tmp_" + to_string(i / (B - 1)) + ".txt";
            merge_runs(group, out_run);
            next_run_files.push_back(out_run);
        }

        for (const string& f : run_files)
            fs::remove(f);

        run_files = next_run_files;
    }

    fs::rename(run_files[0], output_file);
}
