#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

//structs gerais para operacao, transacao e timestamp. 
// op -> nao guarda efetivamente como as transacoes estao, so qual e a transacao
// transcao -> nao guarda efetivamente o timestamp de uma transacao, so seu estado de ativacao
// ts -> guarda o timestamp de read e write (r, w) de uma transacao
struct Op { string tipo, trans; char dado = 0; };

struct Transacao { int ts; bool ativa = true; };

struct TS { int r = -1, w = -1; };

//parseamento da entrada
string trim(string s) {
    auto not_ws = [](int ch){ return !isspace(ch) && ch!=';'; };
    s.erase(s.begin(),  find_if(s.begin(),  s.end(),  not_ws));
    s.erase(find_if(s.rbegin(), s.rend(), not_ws).base(), s.end());
    return s;
}

vector<string> split(const string& s, char delim) {
    vector<string> v; string tok; istringstream ss(s);
    while (getline(ss, tok, delim)) {
        tok = trim(tok);
        if (!tok.empty()) v.push_back(tok);
    }
    return v;
}


//parseamento de uma operacao string para transforma-la no struct Op
Op parse_op(const string& op) {
    Op o; size_t i = 1;
    if (op[0]=='r' || op[0]=='w') {
        o.tipo = op.substr(0,1);
        string num;
        while (i<op.size() && isdigit(op[i])) num += op[i++];
        o.trans = "t"+num;
        auto p = op.find('(');
        o.dado = (p!=string::npos && p+1<op.size()) ? op[p+1] : 0;
    } else if (op[0]=='c') {                      
        string num;
        while (i<op.size() && isdigit(op[i])) num += op[i++];
        o.tipo = "c";  o.trans = "t"+num;
    }
    return o;
}

//processamento dos escalonamentos, func principal do programa
void executa(const string& id, const vector<string>& ops,
             const map<string,int>& ts_init, ofstream& out)
{
    map<string,Transacao> T;
    for (map<string, int>::const_iterator it = ts_init.begin(); it != ts_init.end(); ++it) 
    T[it->first] = { it->second }; 
    //preenche o map T com o nome e o status de cada transacao
    //dava para usar um auto, aqui, mas preferimos deixar explicito 
    //a transacao de nome it->first recebe o timestamp it->second

    map<char,TS> obj; //cada objeto a ser tocado pelas transacoes
    int momento = 0;
    //percorremos todas as operacoes 
    for (const string& s : ops) {
        Op op = parse_op(s);
        Transacao& tr = T[op.trans]; 
        //se a transacao esta inativa (rollback) ou e de commit, passamos  
        if (!tr.ativa) { ++momento; continue; }
        if (op.tipo == "c") { 
            for (auto& [ch, ts] : obj) {
                ts.r = -1;
                ts.w = -1;
            }
            ++momento; 
            continue; }
        //se nao, pegamos o estado do objeto 
        TS& ts = obj[op.dado];

        //e aplicamos as regras para verificar serializacao 
        if (op.tipo=="r") {
            if (tr.ts < ts.w) { out<<id<<"-ROLLBACK-"<< momento <<"\n"; return; }
            ts.r = max(ts.r, tr.ts); 
            //como a operacao so chega aqui se for valida, poderiamos ignorar esse max e so atribuir o timestamp como o mais novo time stamp
            //pois ele sempre sera maior que o timestamp anterior ( caso contrario, estariamos tentando aplicar uma transacao
            //mais antiga sendo que uma mais nova ja foi aplicada, ou seja, teriamos um conflito ). O max, no entanto, 
            //garante um pouco mais de segurança) 
            ofstream log_op(string(1, op.dado) + ".txt", ios::app);
            log_op << id << ", Read, " << momento << "\n";
            log_op.close();
        } 
        if (op.tipo == "w") {
            if (tr.ts < ts.r || tr.ts < ts.w) {
                cout << id << " " << op.dado; 
                out << id << "-ROLLBACK-" << momento << "\n";
                return;
            }
            //regra de thomas nao se aplica pelo enunciado
            //if (tr.ts < ts.w) {
            //escrita obsoleta pela Regra de Thomas
            //    ++momento;
            //    continue;
            //}
            else{
            ts.w = tr.ts;

            ofstream log_op(string(1, op.dado) + ".txt", ios::app);
            log_op << id << ", Write, " << momento << "\n";
            log_op.close();
            }
        }
        ++momento;
    }
    out<<id<<"-OK\n";
    for (const auto& [objeto, ts] : obj) {
        ofstream file(string(1, objeto) + ".txt", ios::app);
        file << objeto << ", " << ts.r << ", " << ts.w << "\n";
        file.close();
    }
}

int main() {
    ifstream in("in2.txt"); ofstream out("out.txt");
    string linha;

    //pega os objetos (comeco do arquivo)
    getline(in, linha);
    vector<char> objetos;
    for (auto& s : split(linha, ',')) objetos.push_back(s[0]);
    for (char obj : objetos) {
        ofstream clear(string(1, obj) + ".txt", ios::trunc);
        clear.close();
    }

    //pega as transacoes (2 linha)
    getline(in, linha);
    vector<string> trans = split(linha, ',');

    //pega os timestamps (3 linha)
    getline(in, linha);
    vector<string> ts_str = split(linha, ',');
    map<string,int> ts_init;
    for (size_t i=0;i<trans.size();++i) ts_init[trans[i]] = stoi(ts_str[i]);

    //por fim, as linhas restantes sao os escalonamentos (schedules no livro)
    while (getline(in, linha)) {
        linha = trim(linha);
        if (linha.empty()) continue;
        auto parte = split(linha, '-');
        string id = parte[0];
        vector<string> ops = split(parte[1], ' ');
        executa(id, ops, ts_init, out);
    }
}
