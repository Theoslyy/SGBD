//#include "arvorebplus.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

struct Node {
    bool isLeaf;
    int id;
    vector<int> chaves; // valor no nó 
    vector<Node*> filhos; // IDs dos filhos 
    int n; // para sabermos quantas chaves há num nó
    string serialize() {
       //converter nó em um string para ser armazenado
    }

    Node deserialize() {
        //converter string de volta em um nó
    }

};
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
    BTree(int g) {
        raiz = NULL;
        grau = g;
    }
    void BTree::insert(int k) {
        if (raiz == NULL) {
            raiz->chaves.push_back(k); 
            raiz->n = 1;
        } 
        else {
            // busca onde inserir o registro
            // vê se dá para inserir no nó específico
            // se não, split e reajuste. 
  }
}
bool BuscarFolha(Node *x, int k){
    Node* folha = BuscarNo(x, k);  
    int i = 0; 
    while(i < folha->n){
        if (k == folha->chaves[i]) return true; 
        i++;
    }
    return false; 
}

Node* BuscarNo(Node *x, int k){ // a busca é por ano da uva
        if (x->isLeaf == true){
            return x; 
        }
        int i = 0; 
        while (i < x->n && k > x->chaves[i])
            i++; 
        if (k <= x->chaves[i]){
            return BuscarNo(x->filhos[i], k);
            // desce para o filho na posição (i) do vetor de filhos (é o filho esquerdo da chave na pos i
            // e o filho direito da chave na pos i - 1)
            // ou seja, estamos usando a convençao de que tudo à esquerda de uma chave é menor ou igual que a chave
            // e à direita é maior que a chave. 
            // Temos que ir descendo nos filhos até chegar na folha. 
            // Depois de descermos uma vez, ou seja, usando o i, 
            // chamamos a função recursivamente até batermos na folha
            }
        return BuscarNo(x->filhos[x->n-1], k); //caso que a chave é maior que todo mundo!
        }
void splitChild(int i, Node *y) {
   // hmm split..
   // tem que criar nós novos...
   // será que é melhor ter os nós como classe ou como struct?
}
};