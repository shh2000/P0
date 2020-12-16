//
// Created by 王梓沐 on 2019/10/3.
//

#include <iostream>
#include "symbol_table.h"
#include "compile_error.h"


const string symbol_table::GLOBAL_SCOPE = "20000412";
int symbol_table::invalid_func_cnt = 0;

bool symbol_table::has_this_scope(const string& scope)
{
    return scope_name_to_symbol_vector.count(scope) != 0;
}

map<string, symbol_table_item*> * symbol_table::_get_map_of_this_scope(const string& scope)
{
    map<string, map<string, symbol_table_item*> >::iterator it_find;
    it_find = scope_name_to_symbol_vector.find(scope);
    if (it_find != scope_name_to_symbol_vector.end())
    {
        return &(it_find->second);
    }
    else
    {
        return nullptr;
    }
}

symbol_table_item * symbol_table::get_item_ptr_in_this_scope(const string &scope, const string &idenfr)
{
    map<string, symbol_table_item*> * scope_local_map = _get_map_of_this_scope(scope);
    if (scope_local_map == nullptr)
    {
        return nullptr;
    }
    else
    {
        if((*scope_local_map).find(idenfr) == (*scope_local_map).end())
        {
            // cout << idenfr << endl;
            return nullptr;
        }
        else
        {
            return (*scope_local_map).find(idenfr)->second;
        }
    }
}

bool symbol_table::get_item_type_in_this_scope(const string& scope, const string& idenfr, symbol_item_type &value_store)
{
    symbol_table_item * item_ptr;
    if ((item_ptr = get_item_ptr_in_this_scope(scope, idenfr)) != nullptr)
    {
        value_store = item_ptr->get_item_type();
        return true;
    }
    return false;
}

bool symbol_table::get_item_func_type_in_this_scope(const string& scope, const string& idenfr, func_type &value_store)
{
    symbol_table_item * item_ptr;
    if ((item_ptr = get_item_ptr_in_this_scope(scope, idenfr)) != nullptr)
    {
        value_store = item_ptr->get_func_type();
        return true;
    }
    return false;
}

bool symbol_table::get_item_value_type_in_this_scope(const string& scope, const string& idenfr, value_type &value_store)
{
    symbol_table_item * item_ptr;
    if ((item_ptr = get_item_ptr_in_this_scope(scope, idenfr)) != nullptr)
    {
        value_store = item_ptr->get_value_type();
        return true;
    }
    return false;
}

bool symbol_table::push_item(const string& scope, symbol_table_item *item, bool create_scope)
{
    // cout << "push " << item->get_identifier() << " to " << scope << endl;
    if (get_item_ptr_in_this_scope(scope, item->get_identifier()) != nullptr)
    {
        throw redefine_error();
    }
    if (create_scope)
    {
        map<string, symbol_table_item*> & local_symbol_table = *(_get_or_create_scope(scope));
        local_symbol_table[item->get_identifier()] = item;
        return true;
    }
    else
    {
        map<string, symbol_table_item*> * local_symbol_table_ptr = _get_map_of_this_scope(scope);
        if (local_symbol_table_ptr == nullptr)
        {
            return false;
        }
        else
        {
            (*local_symbol_table_ptr)[item->get_identifier()] = item;
            return true;
        }
    }
}

map<string, symbol_table_item*> *symbol_table::_get_or_create_scope(const string& scope)
{
    map<string, symbol_table_item*> * scope_local_map = _get_map_of_this_scope(scope);
    if (scope_local_map == nullptr)
    {
        scope_name_to_symbol_vector[scope] = map<string, symbol_table_item*>();
        return &scope_name_to_symbol_vector[scope];
    }
    else
    {
        return scope_local_map;
    }
}

bool symbol_table::has_item_in_this_scope(const string &scope, const string &idenfr)
{
    map<string, symbol_table_item*> * scope_local_map = _get_map_of_this_scope(scope);
    if (scope_local_map == nullptr)
    {
        return false;
    }
    else
    {
        return (*scope_local_map).find(idenfr) != (*scope_local_map).end();
    }
}

symbol_table_item * symbol_table::get_item_ptr(const string &scope, const string &idenfr)
{
    symbol_table_item * item_ptr = nullptr;
    if ((item_ptr = get_item_ptr_in_this_scope(scope, idenfr)) != nullptr)
    {
        return item_ptr;
    }
    else return get_item_ptr_in_this_scope(GLOBAL_SCOPE, idenfr);
}

bool symbol_table::get_item_type(const string &scope, const string &idenfr, symbol_item_type &value_store)
{
    if (get_item_type_in_this_scope(scope, idenfr, value_store))
    {
        return true;
    }
    else return get_item_type_in_this_scope(GLOBAL_SCOPE, idenfr, value_store);
}

bool symbol_table::get_item_value_type(const string &scope, const string &idenfr, value_type &value_store)
{
    if (get_item_value_type_in_this_scope(scope, idenfr, value_store))
    {
        return true;
    }
    else return get_item_value_type_in_this_scope(GLOBAL_SCOPE, idenfr, value_store);
}

bool symbol_table::get_item_func_type(const string &scope, const string &idenfr, func_type &value_store)
{
    if (get_item_func_type_in_this_scope(scope, idenfr, value_store))
    {
        return true;
    }
    else return get_item_func_type_in_this_scope(GLOBAL_SCOPE, idenfr, value_store);
}

bool symbol_table::has_item(const string &scope, const string &idenfr)
{
    if (has_item_in_this_scope(scope, idenfr))
    {
        return true;
    }
    else return has_item_in_this_scope(GLOBAL_SCOPE, idenfr);
}

symbol_table_item *symbol_table::get_item_ptr_expc(const string &scope, const string &idenfr)
{
    symbol_table_item * item = get_item_ptr(scope, idenfr);
    if (item == nullptr)
    {
        throw not_define_error();
    }
    return item;
}
