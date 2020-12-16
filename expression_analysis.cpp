//
// Created by 王梓沐 on 2019/10/3.
//
#include "wzm_settings.h"
#include <iostream>
#include <cassert>
#include <queue>
#include "expression_analysis.h"
#include "statement_analysis.h"
#include "compile_error.h"

static void _update_max_length(int & now_max, int new_max)
{
    if (new_max > now_max)
    {
        now_max = new_max;
    }
}

bool expression_analysis::_item(value_type &vue_type, bool &certain, int &value, string &assign_to)
{
    bool temp_certain = false, has_uncertain = false;
    int temp_value = 1;
    string temp_var_name;
    int local_value = 1;

    if (!_factor(vue_type, temp_certain, temp_value, temp_var_name))
    {
        return false;
    }
    certain = temp_certain;
    if (!certain)
    {
        has_uncertain = true;
        assign_to = get_new_var_name(scope);
        mid_code_buffer.emplace_back(mid_code::assign_mid(assign_to, temp_var_name));
    }
    else
    {
        local_value = temp_value;
    }
    while (true)
    {
        word_type type = manager.get_now_word_type();
        if (type == MULT)
        {
            manager.print_move_front(output_buffer, 1);
        }
        else if (type == DIV)
        {
            manager.print_move_front(output_buffer, 1);
        }
        else
        {
            break;
        }
        vue_type = INT_TYPE_VALUE;
        value_type temp = NOT_VALUE;
        _factor(temp, temp_certain, temp_value, temp_var_name);
        certain &= temp_certain;
        if (certain)
        {
            if (type == MULT)
            {
                local_value *= temp_value;
            }
            else
            {
                local_value /= temp_value;
            }
        }
        else if (!has_uncertain)
        {
            has_uncertain = true;
            assign_to = get_new_var_name(scope);
            if (local_value == 1 && type == MULT)
            {

                mid_code_buffer.emplace_back(mid_code::assign_mid(assign_to, temp_var_name));
            }
            else if (local_value == 0)
            {
                mid_code_buffer.emplace_back(mid_code::assign_mid(assign_to, 0));
            }
            else
            {
                mid_code_buffer.emplace_back(
                        mid_code::calculate_mid(type, assign_to, local_value, temp_var_name));
            }
        }
        else if (temp_certain)
        {
            mid_code_buffer.emplace_back(mid_code::calculate_mid(type, assign_to, assign_to, temp_value));
        }
        else
        {
            mid_code_buffer.emplace_back(
                    mid_code::calculate_mid(type, assign_to, assign_to, temp_var_name));
        }
    }
    if (certain)
    {
        value = local_value;
    }
    output_buffer << "<项>" << endl;
    #ifdef DEBUG
        cout << "<项>" << endl;
    #endif
    return true;
}

bool expression_analysis::expression(value_type &vue_type, bool &certain, int &value, string &assign_to)
{
    bool temp_certain = false, has_uncertain = false;
    int temp_value = 0;
    string temp_var_name;
    int local_value = 0;
    word_type temp_op = PLUS;
    word_type op = manager.get_now_word_type();
    bool has_start_op = false;
    if (op == PLUS)
    {
        manager.print_move_front(output_buffer, 1);
        has_start_op = true;
        temp_op = PLUS;
    }
    else if (op == MINU)
    {
        manager.print_move_front(output_buffer, 1);
        has_start_op = true;
        temp_op = MINU;
    }
    if (!_item(vue_type, temp_certain, temp_value, temp_var_name))
    {
        return false;
    }
    certain = temp_certain;
    if (!certain)
    {
        has_uncertain = true;
        assign_to = get_new_var_name(scope);
        mid_code_buffer.emplace_back(mid_code::assign_mid(assign_to, temp_var_name));
        if (temp_op == MINU)
        {
            mid_code_buffer.emplace_back(mid_code::calculate_mid(temp_op, assign_to, 0, assign_to));
        }
    }
    else
    {
        local_value = temp_value;
        if (temp_op == MINU)
        {
            local_value = -local_value;
        }
    }
    while (true)
    {
        word_type type = manager.get_now_word_type();
        if (type == PLUS)
        {
            manager.print_move_front(output_buffer, 1);
        }
        else if (type == MINU)
        {
            manager.print_move_front(output_buffer, 1);
        }
        else
        {
            break;
        }
        vue_type = INT_TYPE_VALUE;
        value_type temp = NOT_VALUE;
        _item(temp, temp_certain, temp_value, temp_var_name);
        certain &= temp_certain;
        if (certain)
        {
            if (type == PLUS)
            {
                local_value += temp_value;
            }
            else
            {
                local_value -= temp_value;
            }
        }
        else if (!has_uncertain)
        {
            has_uncertain = true;
            assign_to = get_new_var_name(scope);
            if (local_value == 0 && type == PLUS)
            {
                mid_code_buffer.emplace_back(mid_code::assign_mid(assign_to, temp_var_name));
            }
            else
            {
                mid_code_buffer.emplace_back(
                        mid_code::calculate_mid(type, assign_to, local_value, temp_var_name));
            }
        }
        else if (temp_certain)
        {
            mid_code_buffer.emplace_back(mid_code::calculate_mid(type, assign_to, assign_to, temp_value));
        }
        else
        {
            mid_code_buffer.emplace_back(
                    mid_code::calculate_mid(type, assign_to, assign_to, temp_var_name));
        }
    }
    if (certain)
    {
        value = local_value;
    }
    vue_type = (vue_type == CHAR_TYPE_VALUE && !has_start_op)? CHAR_TYPE_VALUE: INT_TYPE_VALUE;
    output_buffer << "<表达式>" <<endl;
    #ifdef DEBUG
        cout << "<表达式>" << endl;
    #endif
    return true;
}

bool expression_analysis::_var_factor(value_type &vue_type, bool &certain, int &value, string &assign_to)
{
    if (!manager.has_sequence_then_print_move(output_buffer, var_factor_head))
    {
        assert(false);
    }
    string idenfr = manager.get_recent_word().get_str_value();
    symbol_table_item * item = table.get_item_ptr_expc(scope, idenfr);
    vue_type = (!item->is_array() && item->get_value_type() == CHAR_TYPE_VALUE)? CHAR_TYPE_VALUE: INT_TYPE_VALUE;
    certain = item->get_item_type() == CONSTANT;
    if (certain)
    {
        if (vue_type == CHAR_TYPE_VALUE)
        {
            value = (int)item->get_char_value();
        }
        else
        {
            value = item->get_int_value();
        }
    }
    else
    {
        assign_to = idenfr;
    }
    return true;
}

bool expression_analysis::_arr_factor(value_type &vue_type, string &assign_to)
{
    if (!manager.has_sequence_then_print_move(output_buffer, arr_factor_head))
    {
        assert(false);
    }
    string idenfr = manager.get_recent_word(2).get_str_value();
    symbol_table_item * item = table.get_item_ptr_expc(scope, idenfr);
    if (item->get_value_type() == CHAR_TYPE_VALUE && item->is_array())
    {
        vue_type = CHAR_TYPE_VALUE;
    }
    else
    {
        vue_type = INT_TYPE_VALUE;
    }
    value_type temp = NOT_VALUE;
    bool certain;
    int value;
    string index_name;
    expression(temp, certain, value, index_name);
    assign_to = get_new_var_name(scope);
    if (certain)
    {
        string temp_index = to_string(value);
        mid_code_buffer.emplace_back(mid_code::arr_read_mid(idenfr, temp_index, assign_to));
    }
    else
    {
        mid_code_buffer.emplace_back(mid_code::arr_read_mid(idenfr, index_name, assign_to));
    }
    if (temp != INT_TYPE_VALUE)
    {
        throw index_not_int();
    }
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RBRACK})))
    {
        throw miss_rbrack();
    }
    return true;
}

bool expression_analysis::_exp_factor(bool &certain, int &value, string &assign_to)
{
    if (!manager.has_sequence_then_print_move(output_buffer, exp_factor_head))
    {
        assert(false);
    }
    value_type temp = NOT_VALUE;
    expression(temp, certain, value, assign_to);
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RPARENT})))
    {
        throw miss_rparent();
    }
    return true;
}

bool expression_analysis::_int_factor(int &value)
{
    if (manager.has_sequence_then_print_move(output_buffer, unsigned_int_factor_head))
    {
        value = atoi(manager.get_recent_word().get_str_value().c_str());
    }
    else if (manager.has_sequence_then_print_move(output_buffer, positive_int_factor_head))
    {
        value = atoi(manager.get_recent_word().get_str_value().c_str());
    }
    else if (manager.has_sequence_then_print_move(output_buffer, negative_int_factor_head))
    {
        value = -atoi(manager.get_recent_word().get_str_value().c_str());
    }
    else
    {
        assert(false);
    }
    output_buffer << "<无符号整数>" << endl;
    output_buffer << "<整数>" << endl;
    #ifdef DEBUG
        cout << "<无符号整数>" << endl;
        cout << "<整数>" << endl;
    #endif
    return true;
}

bool expression_analysis::_char_factor(int &value)
{
    if (!manager.has_sequence_then_print_move(output_buffer, char_factor_head))
    {
        // TODO: ERROR
        return false;
    }
    value = (int)manager.get_recent_word().get_str_value()[0];
    return true;
}

expression_analysis::expression_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                                         vector<mid_code> &midCodeBuffer, words_manager &manager, const string &scope)
        : table(table), output_buffer(outputBuffer), error_buffer(errorBuffer), mid_code_buffer(midCodeBuffer),
        manager(manager), scope(scope)
{

}

bool expression_analysis::_factor(value_type &vue_type, bool &certain, int &value, string &assign_to)
{
    // 外层预读与识别，内层处理与推进。
    // 由于数组、函数、变量皆是标识符开头，所以务必注意比较顺序。
    statement_analysis state_analysis(table, output_buffer, error_buffer, mid_code_buffer,
                                      manager, scope, NOT_VALUE);
    int max_length = 0;
    if (manager.has_expect_sequence(arr_factor_head, false))
    {
        _update_max_length(max_length, arr_factor_head.size());
        _arr_factor(vue_type, assign_to);
        certain = false;
    }
    else if (manager.has_expect_sequence(func_call_head, false))
    {
        _update_max_length(max_length, func_call_head.size());
        state_analysis.return_value_func_call_statement(vue_type);
        certain = false;
        assign_to = get_new_var_name(scope);
        mid_code_buffer.emplace_back(mid_code::get_mid(assign_to));
    }
    else if (manager.has_expect_sequence(var_factor_head, false))
    {
        _update_max_length(max_length, var_factor_head.size());
        _var_factor(vue_type, certain, value, assign_to);
    }
    else if (manager.has_expect_sequence(exp_factor_head, false))
    {
        _update_max_length(max_length, exp_factor_head.size());
        _exp_factor(certain, value, assign_to);
        vue_type = INT_TYPE_VALUE;
    }
    else if (
            manager.has_expect_sequence(unsigned_int_factor_head, false)
            || manager.has_expect_sequence(negative_int_factor_head, false)
            || manager.has_expect_sequence(positive_int_factor_head, false)
            )
    {
        _update_max_length(max_length, unsigned_int_factor_head.size());
        _update_max_length(max_length, negative_int_factor_head.size());
        _update_max_length(max_length, positive_int_factor_head.size());
        _int_factor(value);
        certain = true;
        vue_type = INT_TYPE_VALUE;
    }
    else if (manager.has_expect_sequence(char_factor_head, false))
    {
        _update_max_length(max_length, char_factor_head.size());
        _char_factor(value);
        certain = true;
        vue_type = CHAR_TYPE_VALUE;
    }
    else if (manager.has_invalid_word_in_range(max_length))
    {
        throw lexer_error();
    }
    else
    {
        return false;
    }
    output_buffer << "<因子>" << endl;
    #ifdef DEBUG
        cout << "<因子>" << endl;
    #endif
    return true;
}
