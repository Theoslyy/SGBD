#include "arvorebplus.h"
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
    vector<int> filhos; // IDs dos filhos 
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


void splitChild(int i, Node *y) {
   // hmm split..
   // tem que criar nós novos...
   // será que é melhor ter os nós como classe ou como struct?
}
}