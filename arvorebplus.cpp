//#include "arvorebplus.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;


struct Node {
    vector<int> chaves; // valor no nó 
    vector<Node*> filhos; // IDs dos filhos 
    bool isLeaf;
    int id; 
    int n; // para sabermos quantas chaves há num nó no momento
    Node* proxFolha; 
    Node* pai; 
    string serialize() {
       //converter nó em um string para ser armazenado
    }

    Node deserialize() {
        //converter string de volta em um nó
    }
    Node(bool leaf = false) : isLeaf(leaf), n(0), proxFolha(nullptr), pai(nullptr), id(-1) {}
};

// O protótipo do uso de buffer está implementado
// Tem o vetor que recebe o nó
// A implementação real tem que ser um disk-read ao invés de só atribuir! 
// Bota o nó no buffer, lendo do disco, e chama a função.
/*
TO-DOS:
1. Inicialização da Árvore B+
2. Carregamento das coisas em disco
3. Busca e inserção/remoção
4. Construção -> (é só fazer inserção e pra construir a árvore vai inserindo todos os registros yippee)
*/
class BTree {
    Node *raiz;
    int grau; // a arvore tem um certo grau.. 
    int nextId; // contador de ids
    BTree(int g) : raiz(nullptr), grau(g), nextId(0){}

    Node* criarNo(bool leaf) {
        Node* no = new Node(leaf);
        no->id = nextId++;
        return no;
    }

    void BTree::insert(int k) {
        if (raiz->n == 0) { // se não há nenhuma chave na raiz, o n = 0
            raiz->chaves.push_back(k); 
            raiz->n = 1;
        } 
        else {
            // busca onde inserir o registro
            // vê se dá para inserir no nó específico
            // se não, split e reajuste. 
        }
    }

    bool BuscarFolha(BTree* arvore, int k, Node* buffer[2]){
        buffer[1] = arvore->raiz; //pega a raiz e bota no buffer
        Node* folha = BuscarNo(arvore->raiz, k, buffer);  //busca na raiz. Essa atribuição é permita porque, quando o nó
        // folha for achado pela função BuscarNo, o nó terá sido posto no buffer, então, teremos ele na memória.
        int i = 0; 
        while(i < folha->n){
            if (k == folha->chaves[i]) return true; 
            i++;
        }
        return false; 
    }

    Node* BuscarNo(Node *x, int k, Node* buffer[2]){ // a busca é por ano da uva
            if (x->isLeaf == true){
                return x; 
            }
            int i = 0; 
            while (i < x->n && k > x->chaves[i])
                i++; 
            if (k <= x->chaves[i]){
                buffer[1] = x->filhos[i]; 
                return BuscarNo(x->filhos[i], k, buffer);
                // desce para o filho na posição (i) do vetor de filhos (é o filho esquerdo da chave na pos i
                // e o filho direito da chave na pos i - 1)
                // ou seja, estamos usando a convençao de que tudo à esquerda de uma chave é menor ou igual que a chave
                // e à direita é maior que a chave. 
                // Temos que ir descendo nos filhos até chegar na folha. 
                // Depois de descermos uma vez, ou seja, usando o i, 
                // chamamos a função recursivamente até batermos na folha
                }
            buffer[1] = x->filhos[(x->n)-1];
            return BuscarNo(x->filhos[(x->n)-1], k, buffer); //caso que a chave é maior que todo mundo!
            }
    void splitLeaf(Node* folha, int indice) { //indice é a posição da qual acabamos de descer para chegarmos na folha,
        // ou seja, é a chave cujo filho esquerdo é essa folha. 
        Node* novaFolha = new Node(true);
        int total = folha->n + 1; 
        int meio = (total + 1)/2; // esse cálculo garante um meio melhor
        novaFolha->chaves.assign(folha->chaves.begin() + meio, folha->chaves.end());
        folha->chaves.resize(meio); // eu não queria usar isso... 
        folha->n = folha->chaves.size(); // a quantidade de chaves é a quantidade de chaves que tem no vetor! wow!
        novaFolha->n = novaFolha->chaves.size(); 

        novaFolha->proxFolha = folha->proxFolha; // A -> C; B foi criado. B -> C, ou seja, aponta para quem A apontava
        folha->proxFolha = novaFolha; // E, agora, A -> B, ou seja, A -> B -> C. 
        if (folha->pai->n == 2*this->grau + 1){
            splitNode(folha->pai);
        }
        folha->pai->chaves.insert(folha->pai->chaves.begin() + indice, novaFolha->chaves[0]); // isso daqui eh só uma parada com iterator. 
        folha->pai->filhos.insert(folha->pai->filhos.begin() + indice + 1, novaFolha); // fazer isso insere o valor sem substituir ninguém, dando um push no vetor
        folha->pai->n++;

    }
    void splitNode(Node* no) {
    // hmm split..
    // tem que criar nós novos...
    // será que é melhor ter os nós como classe ou como struct?
    }
};