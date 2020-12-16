//
// Created by 王梓沐 on 2019/10/3.
//

#ifndef COMPILER_2_SYMBOL_TABLE_ITEM_H
#define COMPILER_2_SYMBOL_TABLE_ITEM_H

#include <string>
#include <vector>
#include "global_type.h"

using namespace std;

class symbol_table_item
{
private:
    static int item_count;

    string identifier_;
    string scope_;
    int id_num;
    symbol_item_type item_type_;
    value_type value_type_;
    func_type func_type_;
    int int_value_;
    char char_value_;
    int length_; // 如果为0, 则说明不是数组.
    vector <value_type> formal_parameter_list; // 仅对函数有效果

public:
    static symbol_table_item *get_int_var(string &scope, string &identifier);
    static symbol_table_item *get_char_var(string &scope, string &identifier);
    static symbol_table_item *get_int_arr_var(string &scope, string &identifier, int length);
    static symbol_table_item *get_char_arr_var(string &scope, string &identifier, int length);
    static symbol_table_item* get_int_constant(string &scope, string &identifier, int value);
    static symbol_table_item* get_char_constant(string &scope, string &identifier, char value);
    static symbol_table_item *get_int_parameter(string &scope, string &identifier);
    static symbol_table_item *get_char_parameter(string &scope, string &identifier);
    static symbol_table_item *get_re_int_func(string &scope, string &identifier, vector<value_type> para_list);
    static symbol_table_item *get_re_char_func(string &scope, string &identifier, vector<value_type> para_list);
    static symbol_table_item *get_re_void_func(string &scope, string &identifier, vector<value_type> para_list);
    static symbol_table_item* get_temp_store();
    static int get_item_count();
    symbol_table_item(
            string &scope,
            string &identifier,
            symbol_item_type item,
            value_type value,
            func_type func,
            int int_value,
            char char_value,
            int length
    );
    symbol_table_item(
            string &scope,
            string &identifier,
            symbol_item_type item,
            value_type value,
            func_type func,
            int int_value,
            char char_value,
            int length,
            value_type only_one_para
    );
    symbol_table_item(
            string &scope,
            string &identifier,
            symbol_item_type item,
            value_type value,
            func_type func,
            int int_value,
            char char_value,
            int length,
            vector<value_type> & para_list
    );
    symbol_table_item();
    symbol_table_item(
            const symbol_table_item & item
            );
    string get_identifier();
    string get_scope();
    int get_id_num();
    symbol_item_type get_item_type();
    value_type get_value_type();
    func_type get_func_type();
    int get_int_value();
    char get_char_value();
    int get_arr_length();
    bool valid_param_type(vector<value_type> &args);
    bool is_array();
};


#endif //COMPILER_2_SYMBOL_TABLE_ITEM_H
