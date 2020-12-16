//
// Created by 王梓沐 on 2019/10/3.
//

#include <cassert>
#include "symbol_table_item.h"
#include "global_type.h"
#include "compile_error.h"

int symbol_table_item::item_count = 0;

symbol_table_item::symbol_table_item(
        string &scope, string &identifier, symbol_item_type item, value_type value, func_type func,
        int int_value, char char_value, int length)
{
    identifier_ = identifier;
    scope_ = scope;
    id_num = item_count++;
    item_type_ = item;
    value_type_ = value;
    func_type_ = func;
    int_value_ = int_value;
    char_value_ = char_value;
    length_ = length;
    formal_parameter_list = vector<value_type>();
}

symbol_table_item::symbol_table_item(string &scope, string &identifier, symbol_item_type item, value_type value, func_type func,
                                     int int_value, char char_value, int length, value_type only_one_para)
{
    identifier_ = identifier;
    scope_ = scope;
    id_num = item_count++;
    item_type_ = item;
    value_type_ = value;
    func_type_ = func;
    int_value_ = int_value;
    char_value_ = char_value;
    length_ = length;
    formal_parameter_list = vector<value_type>({only_one_para});
}

symbol_table_item::symbol_table_item(string &scope, string &identifier, symbol_item_type item, value_type value, func_type func,
                                     int int_value, char char_value, int length, vector<value_type> & para_list)
{
    identifier_ = identifier;
    scope_ = scope;
    id_num = item_count++;
    item_type_ = item;
    value_type_ = value;
    func_type_ = func;
    int_value_ = int_value;
    char_value_ = char_value;
    length_ = length;
    formal_parameter_list = vector<value_type>(para_list);
}

symbol_table_item * symbol_table_item::get_int_var(string &scope, string &identifier)
{
    return new symbol_table_item(
            scope,
            identifier,
            VARIABLE,
            INT_TYPE_VALUE,
            NOT_FUNC,
            0,
            0,
            0);
}

symbol_table_item * symbol_table_item::get_char_var(string &scope, string &identifier)
{
    return new symbol_table_item(
            scope,
            identifier,
            VARIABLE,
            CHAR_TYPE_VALUE,
            NOT_FUNC,
            0,
            0,
            0
            );
}

symbol_table_item * symbol_table_item::get_int_arr_var(string &scope, string &identifier, int length)
{
    return new symbol_table_item(
            scope,
            identifier,
            VARIABLE,
            INT_TYPE_VALUE,
            NOT_FUNC,
            0,
            0,
            length);
}

symbol_table_item * symbol_table_item::get_char_arr_var(string &scope, string &identifier, int length)
{
    return new symbol_table_item(
            scope,
            identifier,
            VARIABLE,
            CHAR_TYPE_VALUE,
            NOT_FUNC,
            0,
            0,
            length);
}

symbol_table_item* symbol_table_item::get_int_constant(
        string &scope, string &identifier, int value)
{
    return new symbol_table_item(
            scope,
            identifier,
            CONSTANT,
            INT_TYPE_VALUE,
            NOT_FUNC,
            value,
            0,
            0);
}

symbol_table_item* symbol_table_item::get_char_constant(
        string &scope, string &identifier, char value)
{
    return new symbol_table_item(
            scope,
            identifier,
            CONSTANT,
            CHAR_TYPE_VALUE,
            NOT_FUNC,
            0,
            value,
            0);
}

symbol_table_item * symbol_table_item::get_int_parameter(string &scope, string &identifier)
{
    return new symbol_table_item(
            scope,
            identifier,
            PARAMETER,
            INT_TYPE_VALUE,
            NOT_FUNC,
            0,
            0,
            0);
}

symbol_table_item * symbol_table_item::get_char_parameter(string &scope, string &identifier)
{
    return new symbol_table_item(
            scope,
            identifier,
            PARAMETER,
            CHAR_TYPE_VALUE,
            NOT_FUNC,
            0,
            0,
            0);
}

symbol_table_item * symbol_table_item::get_re_int_func(string &scope, string &identifier, vector<value_type> para_list)
{
    return new symbol_table_item(
            scope,
            identifier,
            FUNCTION,
            NOT_VALUE,
            RE_INT_TYPE_FUNC,
            0,
            0,
            0,
            para_list);
}

symbol_table_item * symbol_table_item::get_re_char_func(string &scope, string &identifier, vector<value_type> para_list)
{
    return new symbol_table_item(
            scope,
            identifier,
            FUNCTION,
            NOT_VALUE,
            RE_CHAR_TYPE_FUNC,
            0,
            0,
            0,
            para_list);
}

symbol_table_item * symbol_table_item::get_re_void_func(string &scope, string &identifier, vector<value_type> para_list)
{
    return new symbol_table_item(
            scope,
            identifier,
            FUNCTION,
            NOT_VALUE,
            VOID_TYPE_FUNC,
            0,
            0,
            0,
            para_list);
}

symbol_table_item* symbol_table_item::get_temp_store()
{
    return new symbol_table_item(
            (string &) "",
            (string &) "",
            VARIABLE,
            NOT_VALUE,
            NOT_FUNC,
            0,
            0,
            0);
}

int symbol_table_item::get_item_count()
{
    return item_count;
}

string symbol_table_item::get_identifier()
{
    return identifier_;
}

string symbol_table_item::get_scope()
{
    return scope_;
}

int symbol_table_item::get_id_num()
{
    return id_num;
}

symbol_item_type symbol_table_item::get_item_type()
{
    return item_type_;
}

value_type symbol_table_item::get_value_type()
{
    return value_type_;
}

func_type symbol_table_item::get_func_type()
{
    return func_type_;
}

int symbol_table_item::get_int_value()
{
    return int_value_;
}

char symbol_table_item::get_char_value()
{
    return char_value_;
}

int symbol_table_item::get_arr_length()
{
    return length_;
}

symbol_table_item::symbol_table_item(const symbol_table_item &item)
{
    identifier_ = item.identifier_;
    scope_ = item.scope_;
    id_num = item.id_num;
    item_type_ = item.item_type_;
    value_type_  = item.value_type_;
    func_type_  = item.func_type_;
    int_value_ = item.int_value_;
    char_value_ = item.char_value_;
    length_ = item.length_;
}

symbol_table_item::symbol_table_item()
{
    identifier_ = "";
    scope_ = "";
    id_num = -1;
    item_type_ = VARIABLE;
    value_type_  = NOT_VALUE;
    func_type_  = NOT_FUNC;
    int_value_ = 0;
    char_value_ = 0;
    length_ = 0;
}

bool symbol_table_item::valid_param_type(vector<value_type> &args)
{
    assert(func_type_ != NOT_FUNC);
    if (args.size() != formal_parameter_list.size())
    {
        throw para_num_miss_match();
    }
    if (args != formal_parameter_list)
    {
        throw para_type_miss_match();
    }
    return true;
}

bool symbol_table_item::is_array()
{
    return length_ != 0;
}
