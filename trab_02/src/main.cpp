#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include "../header/tabela.h"
#include "../header/externalmerge.h"
#include "../header/operador.h"

using namespace std;

int main() {
    Tabela vinho("vinho.csv",0);
    Tabela uva("uva.csv",0);
    Tabela pais("pais.csv",0);

    vinho.carregarDados();
    uva.carregarDados();
    pais.carregarDados();

    Operador op {vinho, uva, "uva_id", "uva_id"};
    //Operador op {vinho, uva, "uva_id", "uva_id"};
    //Operador op {uva, pais, "pais_origem_id", "pais_id"};

    op.executar();

    cout << "#Pags: " << op.numPagsGeradas(); 
    cout << "\n#IOss: " << op.numIOExecutados();
    cout << "\n#Tups: " << op.numTuplasGeradas();

    op.salvarTuplasGeradas("resultado.csv");

    return 0;
}