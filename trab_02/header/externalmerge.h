#ifndef EXTERNALMERGE_H
#define EXTERNALMERGE_H

#include <string>
#include <tuple>

pair<int,int> external_merge_sort(Tabela* tabela, int coluna, string& output_file);
tuple<int, int, int> sort_merge_join (Tabela* tabela1, Tabela* tabela2, int coluna_join_t1, int coluna_join_t2, const string& output_file);

#endif // EXTERNALMERGE_H