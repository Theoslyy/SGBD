//#include "arvorebplus.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;
class BufferManager {
private:
    struct BufferEntry {
        bool isData;     // True para dados, False para índice
        int id;          // ID do registro (para índice) ou linha (para dados)
        string content;  // Conteúdo serializado
        bool dirty;      // Precisa ser escrito de volta

        BufferEntry(bool data, int i, string c, bool d) 
            : isData(data), id(i), content(c), dirty(d) {}
    };

    vector<BufferEntry> buffer;
    fstream& indexFile;
    fstream& dataFile;

public:
    BufferManager(fstream& idx, fstream& dat) 
        : indexFile(idx), dataFile(dat) {
        buffer.reserve(2);  // Buffer máximo de 2 frames
    }

    // Carrega um nó do índice ou dados para o buffer
    string load(bool isData, int id) {
        // Verifica se já está no buffer
        for (auto& entry : buffer) {
            if (entry.isData == isData && entry.id == id) {
                return entry.content;
            }
        }

        // Se buffer cheio, remove um entry (política LRU simples)
        if (buffer.size() >= 2) {
            flush(buffer[0].isData, buffer[0].id);
            buffer.erase(buffer.begin());
        }

        // Lê do disco
        string line;
        if (isData) {
            dataFile.seekg(id * sizeof(string));
            getline(dataFile, line);
        } else {
            indexFile.seekg(id * sizeof(string));
            getline(indexFile, line);
        }

        buffer.emplace_back(isData, id, line, false);
        return line;
    }

    // Escreve alterações de volta para o disco se necessário
    void flush(bool isData, int id) {
        for (auto it = buffer.begin(); it != buffer.end(); ++it) {
            if (it->isData == isData && it->id == id && it->dirty) {
                if (isData) {
                    dataFile.seekp(it->id * sizeof(string));
                    dataFile << it->content << endl;
                } else {
                    indexFile.seekp(it->id * sizeof(string));
                    indexFile << it->content << endl;
                }
                it->dirty = false;
            }
        }
    }

    // Atualiza conteúdo no buffer
    void update(bool isData, int id, const string& newContent) {
        for (auto& entry : buffer) {
            if (entry.isData == isData && entry.id == id) {
                entry.content = newContent;
                entry.dirty = true;
                return;
            }
        }
        // Se não encontrado, adiciona novo entry
        buffer.emplace_back(isData, id, newContent, true);
    }
};

struct Node {
    vector<int> chaves; // valor no nó 
    vector<int> filhos; // IDs dos filhos 
    bool isLeaf;
    int id; 
    int n; // para sabermos quantas chaves há num nó no momento
    int proxFolha; 
    int pai; 
    vector<int> dadosFolhaPTR; // no caso de um no folha, o ponteiro para os dados
    string serialize() {
        stringstream ss;
        ss << id << ";";
        ss << (isLeaf ? "1" : "0") << ";";
        ss << n << ";";
        for (size_t i = 0; i < chaves.size(); ++i) {
            ss << chaves[i];
            if (i < chaves.size() - 1) ss << ",";
        }
        ss << ";";
        if (isLeaf) {
            for (size_t i = 0; i < dadosFolhaPTR.size(); ++i) {
                ss << dadosFolhaPTR[i];
                if (i < dadosFolhaPTR.size() - 1) ss << ",";
            }
        } else {
            for (size_t i = 0; i < filhos.size(); ++i) {
                ss << filhos[i];
                if (i < filhos.size() - 1) ss << ",";
            }
        }
        ss << ";";
        ss << proxFolha << ";";
        ss << pai;
        return ss.str();
    }

    void deserialize(const string& line) {
        vector<string> parts;
        string part;
        istringstream iss(line);
        while (getline(iss, part, ';')) {
            parts.push_back(part);
        }

        id = stoi(parts[0]);
        isLeaf = parts[1] == "1";
        n = stoi(parts[2]);

        chaves.clear();
        istringstream keys(parts[3]);
        string key;
        while (getline(keys, key, ',')) {
            if (!key.empty()) chaves.push_back(stoi(key));
        }

        istringstream childrenOrData(parts[4]);
        string val;
        if (isLeaf) {
            dadosFolhaPTR.clear();
            while (getline(childrenOrData, val, ',')) {
                if (!val.empty()) dadosFolhaPTR.push_back(stoi(val));
            }
        } else {
            filhos.clear();
            while (getline(childrenOrData, val, ',')) {
                if (!val.empty()) filhos.push_back(stoi(val));
            }
        }

        proxFolha = stoi(parts[5]);
        pai = stoi(parts[6]);
    }
        void serializeAndUpdate(BufferManager& buffer) {
        string serialized = serialize();
        buffer.update(false, id, serialized); // false = índice
    }

    static Node deserializeFromBuffer(BufferManager& buffer, int id) {
        string data = buffer.load(false, id); // false = índice
        Node n;
        n.deserialize(data);
        return n;
    }
    Node(bool leaf = false) : isLeaf(leaf), id(-1), n(0), proxFolha(-1), pai(-1) {}
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
    BufferManager& buffer;
    int raiz;
    int grau; // a arvore tem um certo grau.. 
    int nextId; // contador de ids
    //BTree(int g) : raiz(NULL), grau(g), nextId(0){}
    // teste
    fstream& indexFile;
    string dataFile;
    Node* indexBuffer;
    bool indexDirty;


    //Node* criarNo(bool leaf) {
    //    Node* no = new Node(leaf);
    //    no->id = nextId++;
    //    return no;
    //}
    int getAltura(){
        int height = 0;
        int currentId = raiz;
        while (currentId != -1) {
            height++;
            loadIndexNode(currentId);
            if (indexBuffer->isLeaf) break;
            currentId = indexBuffer->filhos[0];
        }
        return height;

    }
    void loadIndexNode(int id) {
        if (indexBuffer && indexDirty) {
            saveIndexNode();
        }
        indexFile.seekg(id * sizeof(string));
        string line;
        getline(indexFile, line);
        indexBuffer = new Node();
        indexBuffer->deserialize(line);
        indexDirty = false;
    }
        void saveIndexNode() {
        if (!indexBuffer) return;
        indexFile.seekp(indexBuffer->id * sizeof(string));
        indexFile << indexBuffer->serialize() << endl;
        indexDirty = false;
    }

    int createNode(bool isLeaf) {
        Node* node = new Node(isLeaf);
        node->id = nextId++;
        saveIndexNode();
        return node->id;
    }
    Node loadNode(int id) {
        return Node::deserializeFromBuffer(buffer, id);
    }
    void saveNode(Node& node) {
        node.serializeAndUpdate(buffer);
    }
    void insert(BTree* arvore, int k) {
        int indice = buscar(arvore, k);
        if (indice == -1){
            int root_id = createNode(true);
            arvore->raiz = root_id;  
            Node root = loadNode(root_id);
            root.chaves.push_back(k);
            root.n = 1; 
        }
        Node noInserir = loadNode(indice);
        if (noInserir.n == 2*arvore->grau - 1){
            splitNode(arvore, noInserir);
            insert(arvore, k);
        } 
        else{
            auto it = std::upper_bound(noInserir.chaves.begin(), noInserir.chaves.end(), k);
            noInserir.chaves.insert(it, k);
            noInserir.n++;
        }
    } 

     int buscar(BTree* arvore, int k){
        if (arvore->raiz == -1){
            return -1; //arvore vazia
        }
        Node raiz = loadNode(arvore->raiz);
        int resultado = buscarNo(raiz, k);
        // folha for achado pela função BuscarNo, o nó terá sido posto no buffer, então, teremos ele na memória.
        return resultado;  
    }

    int buscarNo(Node x, int k){ // a busca é por ano da uva
            if (x.isLeaf == true){
                int i = 0;
                while(i < x.n){
                    if (k == x.chaves[i]) return x.id;
                } 
            }
            int i = 0; 
            while (i < x.n && k > x.chaves[i])
                i++; 
            if (k <= x.chaves[i]){
                Node proximo = loadNode(x.filhos[i]); 
                return buscarNo(proximo, k);
                // desce para o filho na posição (i) do vetor de filhos (é o filho esquerdo da chave na pos i
                // e o filho direito da chave na pos i - 1)
                // ou seja, estamos usando a convençao de que tudo à esquerda de uma chave é menor ou igual que a chave
                // e à direita é maior que a chave. 
                // Temos que ir descendo nos filhos até chegar na folha. 
                // Depois de descermos uma vez, ou seja, usando o i, 
                // chamamos a função recursivamente até batermos na folha
                }
            Node proximo = loadNode(x.filhos[i+1]); 
            return buscarNo(proximo, k); //caso que a chave é maior que todo mundo!
            }
    void splitLeaf(BTree* arvore, Node folha) { //indice é a posição da qual acabamos de descer para chegarmos na folha,
        // ou seja, é a chave cujo filho esquerdo é essa folha. 
        int oldnodeId = folha.id; int newnodeId = createNode(true);
        int temp = folha.proxFolha;
        vector<int> temp_vec;
        folha.proxFolha = newnodeId;
        int total = folha.n + 1; 
        int meio = (total + 1)/2; // esse cálculo garante um meio melhor
        int temp_key = folha.chaves[meio]; 
        int temp_pai = folha.pai; 

        temp_vec.assign(folha.chaves.begin() + meio, folha.chaves.end());
        folha.chaves.resize(meio);  // eu não queria usar isso... 
        folha.n = folha.chaves.size();
        Node newnode = loadNode(newnodeId);
        newnode.chaves.assign(temp_vec.begin() + meio, temp_vec.end());
        newnode.n = newnode.chaves.size(); 
        newnode.proxFolha = temp;  // A -> C; B foi criado. B -> C, ou seja, aponta para quem A apontava
        // E, agora, A -> B, ou seja, A -> B -> C. 
        newnode.pai = temp_pai; 
        Node pai = loadNode(newnode.pai); 
        if (pai.n == 2*arvore->grau - 1){
            splitNode(arvore, pai);
            insert(arvore, temp_key); 
        }
        else{
            auto it = std::upper_bound(pai.chaves.begin(), pai.chaves.end(), temp_key);
            pai.chaves.insert(it, temp_key);
            int i = 0; 
            for(int filho : pai.filhos){
                if (filho == oldnodeId){
                    break;
                }
                i++; 
            }
            pai.filhos.insert(pai.filhos.begin() + i + 1, newnodeId); //adiciona o novo filho, deslocando
            //todo mundo ao invés de substituir.   
            pai.n++;
        }
    } 
    void splitRoot(BTree* arvore, Node no){
        int oldnodeId = no.id; int newnodeId; int temp;
        if(no.isLeaf)  newnodeId = createNode(true); 
        else newnodeId = createNode(false); 
        if(no.isLeaf) int temp = no.proxFolha;
        vector<int> temp_vec;
        if(no.isLeaf) no.proxFolha = newnodeId;
        int total = no.n + 1; 
        int meio = (total + 1)/2; // esse cálculo garante um meio melhor
        int temp_key = no.chaves[meio]; 

        temp_vec.assign(no.chaves.begin() + meio, no.chaves.end());
        no.chaves.resize(meio);  // eu não queria usar isso... 
        no.n = no.chaves.size();
        Node newnode = loadNode(newnodeId);
        newnode.chaves.assign(temp_vec.begin() + meio, temp_vec.end());
        newnode.n = newnode.chaves.size(); 
        if(no.isLeaf) newnode.proxFolha = temp;  // A -> C; B foi criado. B -> C, ou seja, aponta para quem A apontava
        // E, agora, A -> B, ou seja, A -> B -> C. 
        int newpaiId = createNode(false); 
        Node pai = loadNode(newpaiId); 
        pai.chaves[0] = temp_key; 
        pai.filhos[0] = oldnodeId; 
        pai.filhos[1] = newnodeId; 
        pai.n++;
    }
    void splitNode(BTree* arvore, Node no){
        int oldnodeId = no.id; int newnodeId = createNode(true);
        vector<int> temp_vec;
        int total = no.n + 1; 
        int meio = (total + 1)/2; // esse cálculo garante um meio melhor
        int temp_key = no.chaves[meio]; 
        int temp_pai = no.pai; 

        temp_vec.assign(no.chaves.begin() + meio, no.chaves.end());
        no.chaves.resize(meio);  // eu não queria usar isso... 
        no.n = no.chaves.size();
        Node newnode = loadNode(newnodeId);
        newnode.chaves.assign(temp_vec.begin() + meio, temp_vec.end());
        newnode.n = newnode.chaves.size(); 
        newnode.pai = temp_pai; 
        Node pai = loadNode(newnode.pai); 
        if (pai.n == 2*arvore->grau - 1){
            splitNode(arvore, pai);
            insert(arvore, temp_key); 
        }
        else{
            auto it = std::upper_bound(pai.chaves.begin(), pai.chaves.end(), temp_key);
            pai.chaves.insert(it, temp_key);
            int i = 0; 
            for(int filho : pai.filhos){
                if (filho == oldnodeId){
                    break;
                }
                i++; 
            }
            pai.filhos.insert(pai.filhos.begin() + i + 1, newnodeId); //adiciona o novo filho, deslocando
            //todo mundo ao invés de substituir.   
            pai.n++;
        }
    }
    void splitNode(BTree* arvore, Node no) {
        if (no.id == arvore->raiz) return (splitRoot(arvore, no)); 
        else if (no.isLeaf) return splitLeaf(arvore, no); 
        else return splitNode(arvore, no);
    }
};