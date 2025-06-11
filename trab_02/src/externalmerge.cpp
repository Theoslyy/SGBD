#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../header/tupla.h"
#include "../header/pagina.h"
#include "../header/tabela.h"
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

const int B = 4; // número de buffers disponíveis
const int TUPLAS_PAG = 10;


// Ordena e salva uma run no disco
void sort_run(vector<string>& run, int run_id) {
    //aqui, o vetor de strings run é o vetor com as 40 tuplas que queremos ordenar no momento
    vector<string> sorted = run;
    //explicando esse sort embaixo:
    /*
    O sort é uma função do C++ do formato:
    sort(inicio, fim, função_de_comparação)
    nesse caso, nossa função de comparação é: 
    [](const string& a, const string& b) {
    return a < b;
    }
    Isso está errado!
    O que precisamos fazer é:
    Temos cada tupla como uma string no vetor de run, 
    queremos acessar, nessa string, somente o valor correspondente à coluna de ordenação
    ou seja, se a nossa tupla estiver assim:
    0, sauvignon blanc, branco, 1974, 0 
    e queremos ordenar pelo id da uva, temos que pegar o primeiro valor antes da primeira vírgula
    pois é o que corresponde à primeira coluna que é a coluna de uva_id
    Ou seja, realmente a e b serão strings, (a e b são valores consecutivos na entrada)
    O Que precisamos é basicamente fazer um:
    int colA = stoi(a.substr(0, a.find(',')));
    int colB = stoi(b.substr(0, b.find(',')));
    return colA < colB;
    ou seja, pegamos o primeiro valor antes da vírgula (que corresponde à primeira coluna) e o convertemos em inteiro,
    pois estamos comparando um inteiro!
    */


    sort(sorted.begin(), sorted.end(), [](const string& a, const string& b) {
        return a < b; // pega a posição da tupla que corresponde ao índice primário e compara. 
    });
    //gravamos o output num .txt
    ofstream out("run_" + to_string(run_id) + ".txt");
    for (const auto& r : sorted)
        out << r << "\n";
    out.close();
}

// Lê tuplas do arquivo até o limite de memória. Cada página só tem 10 tuplas e o javam não impôs limite
// Logo, podemos ler todas as tuplas sem problema. O max_count é desnecessário
vector<string> read_Tuplas(ifstream& in, int max_count) {
    vector<string> tpls;
    string linha; 
    while (tpls.size() < max_count && getline(in, linha)) {
        tpls.push_back(linha);
    }
    return tpls;
}

// Merge de múltiplos arquivos ordenados
void merge_runs(const vector<string>& run_files, const string& output_file) {
    // to - do:
    // precisamos carregar somente parcialmente essas runs na memória
    // isso pode ser feito com uns ifs ( duh ) 
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
    while(any_of(active.begin(), active.end(), [](bool a) { return a; })) {
        int min_val = INT64_MAX; //isso aqui era pra por um int max qualquer. É só a variavel temp para comparação
        size_t min_idx = -1;
    
        // Acha o menor valor numérico da primeira coluna nas streams ativas
        for (size_t i = 0; i < heads.size(); ++i) {
            if (active[i]) {
                int chave = stoi(heads[i].substr(0, heads[i].find(','))); //achar o primeiro valor
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

// Algoritmo de ordenação externa
void external_merge_sort(string& input_file, string& output_file) {
    ifstream in(input_file);
    vector<string> run_files;
    int run_id = 0;
    // ordena as runs!
    // Isso ainda não está funcionando,
    // precisamos ir lendo a entrada de 2 páginas em duas páginas e ordenando 2 a 2
    // o que essa parte faz é só ler as primeiras 20 tuplas de um arquivo de entrada 
    // No caso, temos que passar a tabela e ir percorrendo de duas em duas páginas
    while (!in.eof()) {
        vector<string> buffer = read_Tuplas(in, B/2 * TUPLAS_PAG); // le um bloco do tamanho da pag*tuplas por pag.
        // Podemos manter quatro páginas em memória, ou seja, podemos ler 4*10 tuplas.
        // No entanto, quando formos juntar blocos de páginas, temos que juntar dois a dois! 
        // Logo, temos que usar b/2 nessa pate.
        if (buffer.empty()) break;
        sort_run(buffer, run_id);
        //depois de darmos sort na run, tacamos no txt e botamos esse txt no vetor de run_files. 
        run_files.push_back("run_" + to_string(run_id) + ".txt");
        ++run_id;
    }
    in.close();
    // ordena conjunto de runs!
    while (run_files.size() > 1) {
        vector<string> next_run_files;
        for (size_t i = 0; i < run_files.size(); i += (B/2)) {
            //fazemos um grupo de tam B/2, ou seja, merge de 2 páginas com 2 páginas, totalizando quatro,
            // ou, caso não tenha tudo isso de runs, fazemos um grupo com o que tem
            vector<string> group(run_files.begin() + i,
                                 run_files.begin() + min(i + B/2 -1, run_files.size()));
            string out_run = "run_tmp_" + to_string(i / (B - 1)) + ".txt";
            merge_runs(group, out_run); ///ordenamos as páginas e as escrevemos no out run.
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

    fs::rename(run_files[0], output_file);
}
