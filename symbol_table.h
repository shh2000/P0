//
// Created by 王梓沐 on 2019/10/3.
//


#ifndef COMPILER_2_SYMBOL_TABLE_H
#define COMPILER_2_SYMBOL_TABLE_H
#include "symbol_table_item.h"
#include <map>

using namespace std;

typedef symbol_table_item  * sym_tab_ptr;

typedef map<string, sym_tab_ptr> local_scope_map;

class symbol_table
{
private:
    map<string, local_scope_map > scope_name_to_symbol_vector = {
            pair<string, local_scope_map >(GLOBAL_SCOPE, local_scope_map({}))
    };
    map<string, symbol_table_item * > * _get_map_of_this_scope(const string& scope);
    map<string, symbol_table_item * > * _get_or_create_scope(const string& scope);
public:
    const static string GLOBAL_SCOPE;
    static int invalid_func_cnt;
    bool has_this_scope(const string& scope);
    symbol_table_item * get_item_ptr_in_this_scope(const string &scope, const string &idenfr);
    bool get_item_type_in_this_scope(const string& scope, const string& idenfr, symbol_item_type & value_store);
    bool get_item_value_type_in_this_scope(const string& scope, const string& idenfr, value_type & value_store);
    bool get_item_func_type_in_this_scope(const string& scope, const string& idenfr, func_type & value_store);
    bool push_item(const string& scope, symbol_table_item *item, bool create_scope);
    bool has_item_in_this_scope(const string& scope, const string& idenfr);

    symbol_table_item * get_item_ptr(const string &scope, const string &idenfr);
    bool get_item_type(const string& scope, const string& idenfr, symbol_item_type & value_store);
    bool get_item_value_type(const string& scope, const string& idenfr, value_type & value_store);
    bool get_item_func_type(const string& scope, const string& idenfr, func_type & value_store);
    bool has_item(const string& scope, const string& idenfr);
    symbol_table_item * get_item_ptr_expc(const string &scope, const string &idenfr);
};


#endif //COMPILER_2_SYMBOL_TABLE_H
