#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>

using namespace std;

class BufferManager {
private:
    struct BufferEntry {
        bool isData;     // True para dados, False para índice
        int id;          // ID do registro (para índice) ou linha (para dados)
        string content; 
        bool dirty;   

        BufferEntry(bool data, int i, string c, bool d) 
            : isData(data), id(i), content(c), dirty(d) {}
    };

    vector<BufferEntry> buffer;
    fstream& indexFile;
    fstream& dataFile;

public:
    BufferManager(fstream& idx, fstream& dat) 
        : indexFile(idx), dataFile(dat) {
        buffer.reserve(2); 
    }

    // Carrega um nó do índice ou dados para o buffer
    string load(bool isData, int id) {
        // Verifica se já está no buffer
        for (auto& entry : buffer) {
            if (entry.isData == isData && entry.id == id) {
                return entry.content;
            }
        }

        // Se buffer cheio, remove um entry
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
       
        buffer.emplace_back(isData, id, newContent, true);
    }
};