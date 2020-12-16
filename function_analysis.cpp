//
// Created by 王梓沐 on 2019/10/2.
//
#include "wzm_settings.h"
#include <iostream>
#include <cassert>
#include "function_analysis.h"
#include "variable_analysis.h"
#include "statement_analysis.h"
#include "global_type.h"
#include "compile_error.h"

bool function_analysis::_function_declare_head_return()
{
    if (manager.has_expect_sequence(INT_IDENFR, false))
    {
        manager.print_move_front(output_buffer, INT_IDENFR.size());
        output_buffer << "<声明头部>" << endl;
        #ifdef DEBUG
            cout << "<声明头部>" << endl;
        #endif
        return true;
    }
    else if (manager.has_expect_sequence(CHAR_IDENFR, false))
    {
        manager.print_move_front(output_buffer, CHAR_IDENFR.size());
        output_buffer << "<声明头部>" << endl;
        #ifdef DEBUG
            cout << "<声明头部>" << endl;
        #endif
        return true;
    }
    else
    {
        // TODO: ERROR
        return false;
    }
}

bool function_analysis::_function_declare_head_void()
{
    if (!manager.has_expect_sequence(DECLARE_HEAD_VOID, false))
    {
        // TODO: ERROR
        return false;
    }
    manager.print_move_front(output_buffer, DECLARE_HEAD_VOID.size());
    return true;
}

bool function_analysis::_parameter_table(vector<value_type> &para_list, string &func_idenfr)
{
    value_type para_type = NOT_VALUE;
    if (_parameter_table_unit(para_type, func_idenfr))
    {
        para_list.emplace_back(para_type);
        while (true)
        {
            if (!manager.has_sequence_then_print_move(output_buffer, COMMA))
            {
                break;
            }
            _parameter_table_unit(para_type, func_idenfr);
            para_list.emplace_back(para_type);
        }
    }
    output_buffer << "<参数表>" << endl;
    #ifdef DEBUG
        cout << "<参数表>" << endl;
    #endif
    return true;
}

bool function_analysis::_parameter_table_unit(value_type &para_type, string &func_idenfr)
{
    if (manager.has_expect_sequence(INT_IDENFR, false))
    {
        para_type = INT_TYPE_VALUE;
        manager.print_move_front(output_buffer, INT_IDENFR.size());
        string idenfr = manager.get_recent_word().get_str_value();
        mid_code_buffer.emplace_back(mid_code::para_mid(idenfr, INT_TYPE_VALUE));
        table.push_item(
                func_idenfr,
                symbol_table_item::get_int_parameter(func_idenfr, idenfr),
                true);
        return true;
    }
    else if (manager.has_expect_sequence(CHAR_IDENFR, false))
    {
        para_type = CHAR_TYPE_VALUE;
        manager.print_move_front(output_buffer, CHAR_IDENFR.size());
        string idenfr = manager.get_recent_word().get_str_value();
        mid_code_buffer.emplace_back(mid_code::para_mid(idenfr, CHAR_TYPE_VALUE));
        table.push_item(
                func_idenfr,
                symbol_table_item::get_char_parameter(func_idenfr, idenfr),
                true);
        return true;
    }
    else if (manager.has_invalid_word_in_range(max(INT_IDENFR.size(), CHAR_IDENFR.size())))
    {
        throw lexer_error();
    }
    else
    {
        return false;
    }
}

bool function_analysis::_parameter_table_brackets(vector<value_type> &para_list, string &func_idenfr)
{
    if (!manager.has_sequence_then_print_move(output_buffer, LPARENT))
    {
        if (manager.has_invalid_word_in_range(1))
        {
            throw lexer_error();
        }
        else
        {
            return false;
        }
    }
    _parameter_table(para_list, func_idenfr);
    if (!manager.has_sequence_then_print_move(output_buffer, RPARENT))
    {
        throw miss_rparent();
    }
    return true;
}

bool function_analysis::function_definition_unit()
{
    // 外层试探，内部处理
    if (manager.has_expect_sequence(CHAR_FUNC_REC, false))
    {
        function_have_return_definition();
        return true;
    }
    else if (manager.has_expect_sequence(INT_FUNC_REC, false))
    {
        function_have_return_definition();
        return true;
    }
    else if (manager.has_expect_sequence(VOID_FUNC_REC, false))
    {
        function_no_return_definition();
        return true;
    }
    else if (manager.has_invalid_word_in_range(3))
    {
        lexer_error expc;
        expc.record_msg(error_buffer, manager.get_now_word_value().get_line_num());
        manager.block_skip();
        ostringstream temp;
        temp << symbol_table::invalid_func_cnt++;
        manager.has_sequence_then_print_move(output_buffer, LBRACE);
        _compound_statement(temp.str(), NOT_VALUE);
        manager.block_skip();
        manager.has_sequence_then_print_move(output_buffer, RBRACE);
        return true;
    }
    else
    {
        // TODO: ERROR
        return false;
    }
}

bool function_analysis::function_have_return_definition()
{
    word_type re_val_type = manager.get_now_word_type();
    string idenfr = manager[manager.get_index() + 1].get_str_value();
    assert(_function_declare_head_return());
    mid_code_buffer.emplace_back(mid_code::func_def_mid(idenfr));
    vector<value_type> para_list;

    try
    {
        if (!_parameter_table_brackets(para_list, idenfr))
        {
            // TODO: ERROR
            return false;
        }
        if (re_val_type == INTTK)
        {
            symbol_table_item * item_ptr = symbol_table_item::get_re_int_func(scope, idenfr, para_list);
            table.push_item(scope, item_ptr, true);
        }
        else
        {
            symbol_table_item * item_ptr = symbol_table_item::get_re_char_func(scope, idenfr, para_list);
            table.push_item(scope, item_ptr, true);
        }
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        manager.block_skip();
    }

    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({LBRACE})))
    {
        manager.lbrace_skip();
        manager.has_sequence_then_print_move(output_buffer, LBRACE);
    }
    // compound_statement
    try
    {
        if (re_val_type == INTTK)
        {
            _compound_statement(idenfr, INT_TYPE_VALUE);
        }
        else
        {
            _compound_statement(idenfr, CHAR_TYPE_VALUE);
        }
    }
    catch (syntax_error& expc)
    {
        manager.block_skip();
        expc.record_msg(error_buffer, manager.get_now_word_value().get_line_num());
    }
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RBRACE})))
    {
        manager.rbrace_skip();
        manager.has_sequence_then_print_move(output_buffer, RBRACE);
    }

    output_buffer << "<有返回值函数定义>" << endl;
    #ifdef DEBUG
        cout << "<有返回值函数定义>" << endl;
    #endif
    return true;
}

bool function_analysis::function_no_return_definition()
{
    // 值得注意的是，先在global创了这个函数的声明，再在函数的作用域内定义其它的。
    string idenfr = manager[manager.get_index() + 1].get_str_value();
    assert(_function_declare_head_void());
    mid_code_buffer.emplace_back(mid_code::func_def_mid(idenfr));
    vector<value_type> para_list;
    try
    {
        if (!_parameter_table_brackets(para_list, idenfr))
        {
            // TODO: ERROR
            return false;
        }
        symbol_table_item * item_ptr = symbol_table_item::get_re_void_func(scope, idenfr, para_list);
        table.push_item(
                scope,
                item_ptr,
                true);

    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        manager.block_skip();
    }
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({LBRACE})))
    {
        manager.lbrace_skip();
        manager.has_sequence_then_print_move(output_buffer, LBRACE);
    }
    // compound_statement
    _compound_statement(idenfr, NOT_VALUE);
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RBRACE})))
    {
        manager.rbrace_skip();
        manager.has_sequence_then_print_move(output_buffer, RBRACE);
    }
    output_buffer << "<无返回值函数定义>" << endl;
    #ifdef DEBUG
        cout << "<无返回值函数定义>" << endl;
    #endif
    return true;
}

bool function_analysis::main_function()
{
    string temp = string("main");
    mid_code_buffer.emplace_back(mid_code::func_def_mid(temp));
    manager.print_move_front(output_buffer, MAIN_FUNC_REC.size());
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({LBRACE})))
    {
        // TODO: ERROR
        return false;
    }
    _compound_statement(string("main"), NOT_VALUE);
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RBRACE})))
    {
        // TODO: ERROR
        return false;
    }

    output_buffer << "<主函数>" << endl;
    #ifdef DEBUG
        cout << "<主函数>" << endl;
    #endif
    return true;
}

bool function_analysis::function_definition()
{
    bool has_function = false;
    while (function_definition_unit()) has_function = true;
    return has_function;
}

function_analysis::function_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                                     vector<mid_code> &midCodeBuffer, words_manager &manager, const string &scope)
        : table(table), output_buffer(outputBuffer), error_buffer(errorBuffer), mid_code_buffer(midCodeBuffer),
        manager(manager), scope(scope)
{

}

bool function_analysis::_compound_statement(const string &func_scope, value_type re_value_type)
{
    variable_analysis var_analysis(table, output_buffer, error_buffer, mid_code_buffer, manager, func_scope);
    statement_analysis state_analysis(table, output_buffer,
                                      error_buffer, mid_code_buffer, manager, func_scope, re_value_type);
    var_analysis.const_description();
    var_analysis.variable_description();
    state_analysis.statement_list();
    if (re_value_type != NOT_VALUE && state_analysis.get_re_cnt() == 0)
    {
        throw re_value_type_mismatch();
    }
    if (re_value_type == NOT_VALUE)
    {
        string temp;
        if (mid_code_buffer.back().get_op_type() != RET)
        {
            mid_code_buffer.emplace_back(mid_code::ret_mid(temp));
        }
    }
    output_buffer << "<复合语句>" << endl;
    #ifdef DEBUG
        cout << "<复合语句>" << endl;
    #endif
    return true;
}
