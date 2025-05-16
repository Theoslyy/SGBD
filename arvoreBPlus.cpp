//#include "arvorebplus.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

struct Node {
    vector<int> chaves; // valor no nó 
    vector<int> filhos; // IDs dos filhos 
    bool isLeaf;
    int id; 
    int n; // para sabermos quantas chaves há num nó no momento
    int proxFolha; 
    int pai; 
    vector<int> dadosFolhaPTR; // no caso de um no folha, o ponteiro para os dados
    string serialize() const {
        stringstream ss;
        ss << this->id << "|" << this->isLeaf << "|" << this->n << "|" << this->pai << "|" << this->proxFolha << "|";
        for (int chave : this->chaves) ss << chave << ",";
        ss << "|";
        for (int filho : this->filhos) ss << filho << ",";
        return ss.str();
    }
    
    void deserialize(const string& line) {
        printf("Linha recebida para deserializar: '%s'\n", line.c_str());
        
        if (line.empty()) {
            printf("Linha vazia detectada!\n");
            throw invalid_argument("Empty line cannot be deserialized.");
        }
        stringstream ss(line);
        string segment;
        
        getline(ss, segment, '|'); id = stoi(segment);
        getline(ss, segment, '|'); isLeaf = stoi(segment);
        getline(ss, segment, '|'); n = stoi(segment);
        getline(ss, segment, '|'); pai = stoi(segment);
        getline(ss, segment, '|'); proxFolha = stoi(segment);

        getline(ss, segment, '|');
        stringstream ssChaves(segment);
        string chaveStr;
        while (getline(ssChaves, chaveStr, ',')) {
            if (!chaveStr.empty()) chaves.push_back(stoi(chaveStr));
        }

        getline(ss, segment, '|');
        stringstream ssFilhos(segment);
        string filhoStr;
        while (getline(ssFilhos, filhoStr, ',')) {
            if (!filhoStr.empty()) filhos.push_back(stoi(filhoStr));
        }
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
    //int raiz;
    int grau; // a arvore tem um certo grau.. 
    int nextId; // contador de ids
    //BTree(int g) : raiz(NULL), grau(g), nextId(0){}
    // teste
    fstream& indexFile;
    string dataFile;
    Node* indexBuffer;
    bool indexDirty;
    int rootID;

    
    //Node* criarNo(bool leaf) {
    //    Node* no = new Node(leaf);
    //    no->id = nextId++;
    //    return no;
    //}
    public:
    BTree(int grau, std::fstream& indexFile, const std::string& dataFile)
    : grau(grau), indexFile(indexFile), dataFile(dataFile), indexBuffer(nullptr), indexDirty(false), rootID(-1) {
        indexFile.seekg(0, ios::beg);
        int maxId = -1;
        string line;
        while (getline(indexFile, line)) {
            if (!line.empty()) {
                Node node;
                node.deserialize(line);
                if (node.pai == -1) rootID = node.id;
                if (node.id > maxId) maxId = node.id;
            }
        }
        nextId = maxId + 1;
    }
    int getAltura(){
        cout << "aqui";
        int height = 0;
        height = buscarporAltura(this, height);
        return height;

    }
    void loadIndexNode(int nodeId) {
        if (indexBuffer) delete indexBuffer; // liberar buffer anterior
        indexBuffer = new Node();

        indexFile.clear();  // limpa flags do stream
        indexFile.seekg(0, ios::beg);

        string line;
        int currentLine = 0;
        while (getline(indexFile, line)) {
            if (currentLine == nodeId) {
                indexBuffer->deserialize(line);
                return;
            }
            currentLine++;
        }

        // Se não achou o nó
        throw runtime_error("Node ID not found in index file");
    }

    void saveIndexNode() {
        if (!indexBuffer) return;  // nada a salvar

        indexFile.clear(); // limpa flags
        indexFile.seekg(0, ios::beg);

        // Lê todas as linhas para um vetor
        vector<string> lines;
        string line;
        while (getline(indexFile, line)) {
            lines.push_back(line);
        }

        // Atualiza a linha do nó no vetor
        if (indexBuffer->id < 0 || indexBuffer->id >= (int)lines.size()) {
            throw runtime_error("Node ID inválido para salvar");
        }
        lines[indexBuffer->id] = indexBuffer->serialize();

        // Reescreve o arquivo todo (simplificação)
        indexFile.close();
        indexFile.open("index.txt", ios::out | ios::trunc);
        for (auto& l : lines) {
            indexFile << l << endl;
        }
        indexFile.flush();

        // Reabre para leitura/escrita no construtor etc
        indexFile.close();
        indexFile.open("index.txt", ios::in | ios::out);
    }

    int createNode(bool leaf) {
        Node novo(leaf);
        novo.id = nextId++;
        novo.n = 0;
        novo.pai = -1;
        novo.proxFolha = -1;

        // Adiciona no final do arquivo
        indexFile.clear();
        indexFile.seekp(0, ios::end);
        indexFile << novo.serialize() << endl;
        indexFile.flush();

        return novo.id;
    }

    Node loadNode(int nodeId) {
        string line;
        int currentLine = 0;
        cout << "Lendo nó com ID: " << nodeId << endl;
        indexFile.clear(); // Limpa flags de erro
        indexFile.seekg(0, ios::beg); // Volta ao início do arquivo
        bool firstLinePrinted = false;
        while (getline(indexFile, line)) {
            if (!firstLinePrinted) {
                cout << "Primeira linha lida: " << line << endl;
                firstLinePrinted = true;
            }
            if (currentLine == nodeId) {
                Node node;
                node.deserialize(line);
                cout << "hi"; 
                return node;
                cout << "hi 2"; 
            }
            currentLine++;
        }
        throw runtime_error("Node ID not found in loadNode");
    }

    void saveNode(const Node& node) {
        // Lê todas as linhas para um vetor
        indexFile.clear();
        indexFile.seekg(0, ios::beg);

        vector<string> lines;
        string line;
        while (getline(indexFile, line)) {
            lines.push_back(line);
        }

        if (node.id < 0 || node.id >= (int)lines.size()) {
            throw runtime_error("Node ID inválido para saveNode");
        }
        lines[node.id] = node.serialize();

        // Reescreve arquivo inteiro
        indexFile.close();
        indexFile.open("index.txt", ios::out | ios::trunc);
        for (auto& l : lines) {
            indexFile << l << endl;
        }
        indexFile.flush();

        indexFile.close();
        indexFile.open("index.txt", ios::in | ios::out);
    }
    void insert(BTree* arvore, int k) {
        printf("chegou aqui no ini insert\n");
        int indice = buscar(arvore, k);
        printf("chegou depois da busca no insert\n");
        if (indice == -1){
            printf("chegou para inserir na raiz apos criar");
            int root_id = createNode(true);
            cout << "id do no criado:" << root_id; 
            arvore->rootID = root_id;
            cout << "id da raiz:" <<root_id;  
            Node root = loadNode(root_id);
            cout << "hi three";
            root.chaves.push_back(k);
            root.n = 1; 
            arvore->saveNode(root);
            return;
        }
        Node noInserir = loadNode(indice);
        if (noInserir.n == 2*arvore->grau - 1){
            split(arvore, noInserir);
            insert(arvore, k);
            return; 
        } 
        else{
            auto it = std::upper_bound(noInserir.chaves.begin(), noInserir.chaves.end(), k);
            noInserir.chaves.insert(it, k);
            noInserir.n++;
            arvore->saveNode(noInserir);
            return;
        }
    } 


int buscar(BTree* arvore, int k) {
    printf("[1] Starting search for key=%d\n", k);
    
    if (arvore->rootID == -1) {
        printf("[2] Empty tree (rootID=-1)\n");
        return -1;
    }

    printf("[3] Loading root node ID=%d\n", arvore->rootID);
    Node raiz = arvore->loadNode(arvore->rootID); // <-- Likely culprit
    cout << raiz.n;
    printf("[4] Loaded root node: id=%d, isLeaf=%d, n=%d\n", 
           raiz.id, raiz.isLeaf, raiz.n);
    
    printf("[5] Entering buscarNo...\n");
    int resultado = buscarNo(raiz, k);
    
    printf("[6] Search result: %d\n", resultado);
    return resultado;
}

    int buscarNo(Node& x, int k){ // a busca é por ano da uva
            cout << "[BUSCARNO] Node ID=" << x.id << ", isLeaf=" << x.isLeaf << ", n=" << x.n << " " << x.chaves[0] << endl;
            cout << "hi é folha";
            if (x.isLeaf == true){
                cout << "hi é folha";
                int i = 0;
                while(i < x.n){ // x.n
                    if (k == x.chaves[i]) return x.id;
                    i++;
                } 
                return  x.id;
            }
            int i = 0; 
            while (i < 1 && k > x.chaves[i]) // x.n
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
        int buscarIgualdade(BTree* arvore, int k){
            if (arvore->rootID == -1) {
                printf("[2] Empty tree (rootID=-1)\n");
                return -1;
            }

            Node raiz = arvore->loadNode(arvore->rootID); // <-- Likely culprit
            cout << raiz.n;
            int resultado = 0;
            resultado = buscarIgual(raiz, k, resultado);
            
            printf("[6] Search result: %d\n", resultado);
            return resultado;
        }
        int buscarIgual(Node& x, int k, int resultado){ // a busca é por ano da uva
            if (x.isLeaf == true){
                int i = 0;
                while(i < x.n){ // x.n
                    if (k == x.chaves[i]){
                        int j = i;
                        while(k == x.chaves[j]) { 
                            if (j == x.n){
                                resultado++;
                                int id_next = x.proxFolha;
                                Node x = loadNode(id_next);
                                j == 0;
                            }
                            else{
                                resultado++;
                                j++;
                            }
                        }
                        return resultado;
                    }
                    i++;
                } 
            printf("Não achou-se...");
            return  -1;
        }
        int i = 0; 
        while (i < 1 && k > x.chaves[i]) // x.n
            i++; 
        if (k <= x.chaves[i]){
            Node proximo = loadNode(x.filhos[i]); 
            return buscarIgual(proximo, k, resultado);
            // desce para o filho na posição (i) do vetor de filhos (é o filho esquerdo da chave na pos i
            // e o filho direito da chave na pos i - 1)
            // ou seja, estamos usando a convençao de que tudo à esquerda de uma chave é menor ou igual que a chave
            // e à direita é maior que a chave. 
            // Temos que ir descendo nos filhos até chegar na folha. 
            // Depois de descermos uma vez, ou seja, usando o i, 
            // chamamos a função recursivamente até batermos na folha
            }
        Node proximo = loadNode(x.filhos[i+1]);

        return buscarIgual(proximo, k, resultado); //caso que a chave é maior que todo mundo!
        }
        int buscarporAltura(BTree* arvore, int h){
            if (arvore->rootID == -1) {
                return h;
            }
            Node raiz = arvore->loadNode(arvore->rootID); // <-- Likely culprit
            int resultado = buscarAltura(raiz, h);
            return resultado;
        }
        int buscarAltura(Node& x, int h){ // a busca é por ano da uva
        cout << "passei por aqui";
        if (x.isLeaf == true){
            return  h;
        }
        else{
            h++;
            Node proximo = loadNode(x.filhos[0]); 
            return buscarAltura(proximo, h);
        }
        }
    void splitLeaf(BTree* arvore, Node folha) { //indice é a posição da qual acabamos de descer para chegarmos na folha,
        // ou seja, é a chave cujo filho esquerdo é essa folha. 
        cout << " to aqui ";
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
        arvore->saveNode(folha);
        Node newnode = loadNode(newnodeId);
        newnode.chaves.assign(temp_vec.begin() + meio, temp_vec.end());
        newnode.n = newnode.chaves.size(); 
        newnode.proxFolha = temp;  // A -> C; B foi criado. B -> C, ou seja, aponta para quem A apontava
        // E, agora, A -> B, ou seja, A -> B -> C. 
        newnode.pai = temp_pai; 
        arvore->saveNode(newnode);
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
            arvore->saveNode(pai);
        }
    } 
    void splitRoot(BTree* arvore, Node no){
        int oldnodeId = no.id; int newnodeId; int temp;
        if(no.isLeaf)  newnodeId = createNode(true); 
        else newnodeId = createNode(false); 
        if(no.isLeaf) int temp = no.proxFolha;
        // Initialize temp_vec with the same size as no.chaves
        vector<int> temp_vec(no.chaves.size());
        if(no.isLeaf) no.proxFolha = newnodeId;
        int total = no.n; 
        int meio = (total)/2; // esse cálculo garante um meio melhor
        int temp_key = no.chaves[meio]; 

        temp_vec.assign(no.chaves.begin() + meio, no.chaves.end());
        no.chaves.resize(meio);  // eu não queria usar isso... 
        no.n = no.chaves.size();
        arvore->saveNode(no);
        Node newnode = loadNode(newnodeId);
        newnode.chaves.assign(temp_vec.begin() + meio, temp_vec.end());
        newnode.n = newnode.chaves.size(); 
        if(no.isLeaf) newnode.proxFolha = temp;  // A -> C; B foi criado. B -> C, ou seja, aponta para quem A apontava
        // E, agora, A -> B, ou seja, A -> B -> C.
        arvore->saveNode(newnode); 
        int newpaiId = createNode(false); 
        Node pai = loadNode(newpaiId); 
        pai.chaves[0] = temp_key; 
        pai.filhos[0] = oldnodeId; 
        pai.filhos[1] = newnodeId; 
        pai.n++;
        arvore->rootID = newpaiId;
        arvore->saveNode(pai);
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
        arvore->saveNode(no);
        Node newnode = loadNode(newnodeId);
        newnode.chaves.assign(temp_vec.begin() + meio, temp_vec.end());
        newnode.n = newnode.chaves.size(); 
        newnode.pai = temp_pai;
        arvore->saveNode(newnode); 
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
            arvore->saveNode(pai);
        }
    }
    void split(BTree* arvore, Node no) {
        if (no.id == arvore->rootID) {return (splitRoot(arvore, no)); }
        else if (no.isLeaf) {  return splitLeaf(arvore, no); }
        else {return splitNode(arvore, no);}
    }
};
