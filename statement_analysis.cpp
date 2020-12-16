//
// Created by 王梓沐 on 2019/10/2.
//
#include <iostream>
#include <cassert>
#include <stack>
#include "wzm_settings.h"
#include "statement_analysis.h"
#include "compile_error.h"
#include "utils.h"

using namespace std;

static void _update_max_length(int & now_max, int new_max)
{
    if (new_max > now_max)
    {
        now_max = new_max;
    }
}

bool statement_analysis::branch_condition_formula(mid_code &branch_mid_code, label_pair &else_label_pair)
{
    expression_analysis exp_analysis(table, output_buffer, error_buffer,
            mid_code_buffer, manager, scope);
    value_type temp = NOT_VALUE;
    bool certain;
    int value;
    string assign_to;
    exp_analysis.expression(temp, certain, value, assign_to);
    string value_str = to_string(value);
    label_pair temp_pair = get_new_if_label_pair(scope);
    string else_begin_label = temp_pair.first;
    else_label_pair = temp_pair;
    if (temp != INT_TYPE_VALUE)
    {
        throw condition_type_error();
    }
    word_type type = manager.get_now_word_type();
    if (relation_op.count(type) == 1)
    {
        manager.print_move_front(output_buffer, 1);
        bool certain2;
        int value2;
        string assign_to2;
        exp_analysis.expression(temp, certain2, value2, assign_to2);
        string value_str2 = to_string(value2);
        if (temp != INT_TYPE_VALUE)
        {
            throw condition_type_error();
        }
        branch_mid_code = mid_code::condition_mid(
                type,
                certain? value_str: assign_to,
                certain2? value_str2: assign_to2,
                else_begin_label, true);

        output_buffer << "<条件>" << endl;
        #ifdef DEBUG
            cout << "<条件>" << endl;
        #endif
        return true; // not must true
    }
    else
    {
        if (certain)
        {
            branch_mid_code = mid_code::condition_mid(
                    NEQ, value, 0, else_begin_label, true);
        }
        else
        {
            branch_mid_code = mid_code::condition_mid(
                    NEQ, assign_to, 0, else_begin_label, true);
        }
        output_buffer << "<条件>" << endl;
        #ifdef DEBUG
            cout << "<条件>" << endl;
        #endif
        return true;
    }
}

bool statement_analysis::_assign_statement()
{
    expression_analysis exp_analysis(table, output_buffer, error_buffer,
            mid_code_buffer, manager, scope);
    try
    {
        bool certain;
        int value;
        string assign_to;
        if (manager.has_sequence_then_print_move(output_buffer, var_assign_start))
        {
            string idenfr = manager.get_recent_word(2).get_str_value();
            symbol_table_item * item = table.get_item_ptr_expc(scope, idenfr);
            if (item->get_item_type() == CONSTANT)
            {
                throw assign_to_const();
            }
            value_type temp = NOT_VALUE;
            exp_analysis.expression(temp, certain, value, assign_to);
            if (certain)
            {
                mid_code_buffer.emplace_back(mid_code::assign_mid(idenfr, value));
            }
            else
            {
                mid_code_buffer.emplace_back(mid_code::assign_mid(idenfr, assign_to));
            }
        }
        else if (manager.has_sequence_then_print_move(output_buffer, arr_assign_start))
        {
            string idenfr = manager.get_recent_word(2).get_str_value();
            table.get_item_ptr_expc(scope, idenfr);
            value_type temp = NOT_VALUE;

            exp_analysis.expression(temp, certain, value, assign_to);
            if (temp != INT_TYPE_VALUE)
            {
                throw index_not_int();
            }

            string index = to_string(value);
            bool index_certain = certain;
            string index_store = assign_to;

            if (!manager.has_sequence_then_print_move(output_buffer, RBRACK))
            {
                throw miss_rbrack();
            }
            assert(manager.has_sequence_then_print_move(output_buffer, ASSIGN));
            exp_analysis.expression(temp, certain, value, assign_to);
            string value_str = to_string(value);
            mid_code_buffer.emplace_back(mid_code::arr_set_mid(
                    idenfr,
                    index_certain? index: index_store,
                    certain? value_str: assign_to));
        }
        else
        {
            assert(false);
        }
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        manager.skip(SEMICN);
    }
    output_buffer << "<赋值语句>" << endl;
    #ifdef DEBUG
        cout << "<赋值语句>" << endl;
    #endif
    return true;
}

bool statement_analysis::_condition_statement()
{
    if (!manager.has_expect_sequence(if_sequence_head, false))
    {
        assert(false);
    }
    manager.print_move_front(output_buffer, if_sequence_head.size());
    mid_code branch_mid_code = mid_code::get_temp_mid();
    bool valid_jump;
    label_pair else_label_pair;
    try
    {
        branch_condition_formula(branch_mid_code, else_label_pair);
        valid_jump = branch_mid_code.is_valid();
        if (!manager.has_sequence_then_print_move(output_buffer, RPARENT))
        {
            throw miss_rparent();
        }
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        condition_block_skip();
    }
    if (valid_jump)
    {
        mid_code_buffer.emplace_back(branch_mid_code);
    }
    // statement
    statement();
    // 思考基本块划分的问题
    if (manager.has_sequence_then_print_move(output_buffer, ELSETK))
    {
        mid_code_buffer.emplace_back(mid_code::goto_mid(else_label_pair.second));
        mid_code_buffer.emplace_back(mid_code::label_mid(else_label_pair.first));
        // statement
        statement();
        mid_code_buffer.emplace_back(mid_code::label_mid(else_label_pair.second));
    }
    else
    {
        // cout << else_label_pair.first << endl;
        mid_code_buffer.emplace_back(mid_code::label_mid(else_label_pair.first));
    }

    output_buffer << "<条件语句>" << endl;
    #ifdef DEBUG
        cout << "<条件语句>" << endl;
    #endif
    return true;
}

bool statement_analysis::_loop_statement()
{
    if (manager.has_expect_sequence(while_sequence_head, false))
    {
        _while_statement();
    }
    else if (manager.has_expect_sequence(do_while_sequence_head, false))
    {
        _do_while_statement();
    }
    else if (manager.has_expect_sequence(for_sequence_head, false))
    {
        _for_statement();
    }
    else
    {
        assert(false);
    }

    output_buffer << "<循环语句>" << endl;
    #ifdef DEBUG
        cout << "<循环语句>" << endl;
    #endif
    return true;
}

bool statement_analysis::_while_statement()
{
    assert(manager.has_expect_sequence(while_sequence_head, false));
    manager.print_move_front(output_buffer, while_sequence_head.size());
    mid_code loop_mid_code = mid_code::get_temp_mid();
    label_pair while_pair;
    int now_len = mid_code_buffer.size();
    try
    {
        loop_condition_formula(loop_mid_code, while_pair);
        if (
                !manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RPARENT}))
                )
        {
            throw miss_rparent();
        }
        stack<mid_code> condition_code;
        int extend_size = mid_code_buffer.size() - now_len;
        int i = 0;
        while (!mid_code_buffer.empty() && i < extend_size)
        {
            condition_code.push(mid_code_buffer.back());
            mid_code_buffer.pop_back();
            i++;
        }
        mid_code_buffer.emplace_back(mid_code::goto_mid(while_pair.second));
        mid_code_buffer.emplace_back(mid_code::label_mid(while_pair.first));
        statement();
        mid_code_buffer.emplace_back(mid_code::label_mid(while_pair.second));
        while (!condition_code.empty())
        {
            mid_code_buffer.emplace_back(condition_code.top());
            condition_code.pop();
        }
        mid_code_buffer.emplace_back(loop_mid_code);
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        condition_block_skip();
    }


    return true;
}

bool statement_analysis::_do_while_statement()
{
    assert(manager.has_sequence_then_print_move(output_buffer, do_while_sequence_head));
    label_pair loop_pair = get_new_loop_label_pair(scope);
    mid_code_buffer.emplace_back(mid_code::label_mid(loop_pair.first));
    // statement
    statement();
    if (!manager.has_sequence_then_print_move(output_buffer, while_sequence_head))
    {
        if (manager.has_invalid_word_in_range(while_sequence_head.size()))
        {
            lexer_error expc;
            expc.record_msg(error_buffer, manager.get_now_word_value().get_line_num());
        }
        else
        {
            do_miss_while expc;
            expc.record_msg(error_buffer, manager.get_now_word_value().get_line_num());
        }
        condition_block_skip();
        return true;
    }
    mid_code do_while_mid_code = mid_code::get_temp_mid();
    try
    {
        do_while_condition_formula(do_while_mid_code, loop_pair.first);
        if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RPARENT})))
        {
            throw miss_rparent();
        }
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        condition_block_skip();
    }
    if (do_while_mid_code.is_valid())
    {
        mid_code_buffer.emplace_back(do_while_mid_code);
    }
    mid_code_buffer.emplace_back(mid_code::label_mid(loop_pair.second));
    return true;
}

bool statement_analysis::_for_statement()
{
    expression_analysis exp_analysis(table, output_buffer, error_buffer,
            mid_code_buffer, manager, scope);
    assert(manager.has_sequence_then_print_move(output_buffer, for_sequence_head));
    // expression

    try
    {
        string idenfr = manager.get_recent_word(2).get_str_value();
        table.get_item_ptr_expc(scope, idenfr);
        value_type temp = NOT_VALUE;
        bool certain;
        int value;
        string assign_to;
        exp_analysis.expression(temp, certain, value, assign_to);
        string value_str = to_string(value);
        mid_code_buffer.emplace_back(mid_code::assign_mid(idenfr, certain? value_str: assign_to));
        if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({SEMICN})))
        {
            throw miss_semicn();
        }
        mid_code loop_mid_code = mid_code::get_temp_mid();
        label_pair loop_pair;
        int now_len = mid_code_buffer.size();
        loop_condition_formula(loop_mid_code, loop_pair);
        if (
                !manager.has_sequence_then_print_move(output_buffer, for_sequence_tail_add)
                && !manager.has_sequence_then_print_move(output_buffer, for_sequence_tail_sub))
        {
            // TODO: ERROR
            return false;
        }
        stack<mid_code> condition_code;
        int extend_len = mid_code_buffer.size() - now_len;
        for (int i = 0; i < extend_len && !mid_code_buffer.empty(); ++i)
        {
            condition_code.push(mid_code_buffer.back());
            mid_code_buffer.pop_back();
        }
        mid_code_buffer.emplace_back(mid_code::goto_mid(loop_pair.second));
        mid_code_buffer.emplace_back(mid_code::label_mid(loop_pair.first));
        word_type type = manager.get_recent_word().get_type();
        string idenfr1 = manager.get_recent_word(2).get_str_value();
        symbol_table_item* item = table.get_item_ptr_expc(scope, idenfr1);
        string idenfr2 = manager.get_recent_word(4).get_str_value();
        table.get_item_ptr_expc(scope, idenfr2);
        _step_size();
        int step = atoi(manager.get_recent_word().get_str_value().c_str());
        if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RPARENT})))
        {
            throw miss_rparent();
        }
        statement();
        if (item->get_item_type() == CONSTANT)
        {
            if (item->get_value_type() == CHAR_TYPE_VALUE)
            {
                mid_code_buffer.emplace_back(
                        mid_code::calculate_mid(type, idenfr2, (int)item->get_char_value(), step));
            }
            else
            {
                mid_code_buffer.emplace_back(
                        mid_code::calculate_mid(type, idenfr2, (int)item->get_int_value(), step));
            }
        }
        else
        {
            mid_code_buffer.emplace_back(mid_code::calculate_mid(type, idenfr2, idenfr1, step));
        }

        mid_code_buffer.emplace_back(mid_code::label_mid(loop_pair.second));
        while (!condition_code.empty())
        {
            mid_code_buffer.emplace_back(condition_code.top());
            condition_code.pop();
        }
        mid_code_buffer.emplace_back(loop_mid_code);
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        condition_block_skip();
    }
    // statement

    return true;
}

bool statement_analysis::_read_statement()
{
    const vector<word_type> scanf_idenfr_unit = {
            COMMA, IDENFR
    };
    assert(manager.has_sequence_then_print_move(output_buffer, scanf_sequence_head));
    try
    {
        symbol_table_item * item;
        string idenfr = manager.get_recent_word().get_str_value();
        item = table.get_item_ptr_expc(scope, idenfr);
        mid_code_buffer.emplace_back(mid_code::scanf_mid(item->get_value_type(), idenfr));
        while (manager.has_expect_sequence(scanf_idenfr_unit, false))
        {
            manager.print_move_front(output_buffer, scanf_idenfr_unit.size());
            idenfr = manager.get_recent_word().get_str_value();
            item = table.get_item_ptr_expc(scope, idenfr);
            mid_code_buffer.emplace_back(mid_code::scanf_mid(item->get_value_type(), idenfr));
        }
        if (manager.has_invalid_word_in_range(scanf_idenfr_unit.size()))
        {
            throw lexer_error();
        }
        if (!manager.has_sequence_then_print_move(output_buffer, RPARENT))
        {
            throw miss_rparent();
        }
        output_buffer << "<读语句>" << endl;
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        manager.skip(SEMICN);
    }
    #ifdef DEBUG
        cout << "<读语句>" << endl;
    #endif
    return true;
}

bool statement_analysis::_write_statement()
{
    expression_analysis exp_analysis(table, output_buffer, error_buffer,
            mid_code_buffer, manager, scope);
    vector<word_type> str_comma_exp = {
            STRCON, COMMA
    };
    vector<word_type> str = {
            STRCON
    };
    assert(manager.has_sequence_then_print_move(output_buffer, printf_sequence_head));
    try
    {
        if (manager.has_expect_sequence(str_comma_exp, false))
        {
            // 因为 "字符串" 需要被输出，所以这里分步输出
            manager.print_move_front(output_buffer, 1);
            string output_str = manager.get_recent_word().get_str_value();
            string label = insert_const_str(output_str);
            mid_code_buffer.emplace_back(mid_code::printf_mid(NOT_VALUE, label));
            output_buffer << "<字符串>" << endl;
            #ifdef DEBUG
                cout << "<字符串>" << endl;
            #endif
            manager.print_move_front(output_buffer, 1);
            // expression
            value_type temp = NOT_VALUE;
            bool certain;
            int value;
            string assign_to;
            exp_analysis.expression(temp, certain, value, assign_to);
            if (certain)
            {
                mid_code_buffer.emplace_back(mid_code::printf_mid(temp, value));
            }
            else
            {
                mid_code_buffer.emplace_back(mid_code::printf_mid(temp, assign_to));
            }
            mid_code_buffer.emplace_back(mid_code::print_next_line_mid());
        }
        else if (manager.has_expect_sequence(str, false))
        {
            // TODO
            manager.print_move_front(output_buffer, 1);
            string output_str = manager.get_recent_word().get_str_value();
            string label = insert_const_str(output_str);
            mid_code_buffer.emplace_back(mid_code::printf_mid(NOT_VALUE, label));
            mid_code_buffer.emplace_back(mid_code::print_next_line_mid());

            output_buffer << "<字符串>" << endl;
            #ifdef DEBUG
                cout << "<字符串>" << endl;
            #endif
        }
        else
        {
            // expression
            value_type temp = NOT_VALUE;
            bool certain;
            int value;
            string assign_to;
            exp_analysis.expression(temp, certain, value, assign_to);
            if (certain)
            {
                mid_code_buffer.emplace_back(mid_code::printf_mid(temp, value));
            }
            else
            {
                mid_code_buffer.emplace_back(mid_code::printf_mid(temp, assign_to));
            }
            mid_code_buffer.emplace_back(mid_code::print_next_line_mid());
        }
        if (!manager.has_sequence_then_print_move(output_buffer, RPARENT))
        {
            throw miss_rparent();
        }
        output_buffer << "<写语句>" << endl;
        #ifdef DEBUG
            cout << "<写语句>" << endl;
        #endif
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        manager.skip(SEMICN);
    }
    return true;
}

bool statement_analysis::_return_statement()
{
    expression_analysis exp_analysis(table, output_buffer, error_buffer,
            mid_code_buffer, manager, scope);
    if (!manager.has_sequence_then_print_move(output_buffer, return_sequence_head))
    {
        assert(false);
    }

    try
    {
        value_type re_type = NOT_VALUE;
        bool certain;
        int value;
        string assign_to;
        if (manager.has_sequence_then_print_move(output_buffer, vector<word_type>({LPARENT})))
        {
            // expression

            exp_analysis.expression(re_type, certain, value, assign_to);
            if (certain)
            {
                mid_code_buffer.emplace_back(mid_code::ret_mid(value));
            }
            else
            {
                mid_code_buffer.emplace_back(mid_code::ret_mid(assign_to));
            }
            if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RPARENT})))
            {
                throw miss_rparent();
            }
        }
        else
        {
            string temp;
            mid_code_buffer.emplace_back(mid_code::ret_mid(temp));
        }
        if (re_type != NOT_VALUE && re_value_type == NOT_VALUE)
        {
            throw re_void_re_mismatch();
        }
        if (re_type != re_value_type)
        {
            throw re_value_type_mismatch();
        }
        cnt_re++;
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        manager.skip(SEMICN);
    }
    output_buffer << "<返回语句>" << endl;
    #ifdef DEBUG
        cout << "<返回语句>" << endl;
    #endif
    return true;
}

bool statement_analysis::_step_size()
{
    if (!manager.has_sequence_then_print_move(output_buffer, INTCON))
    {
        throw lexer_error();
    }

    output_buffer << "<无符号整数>" << endl;
    output_buffer << "<步长>" << endl;
    #ifdef DEBUG
        cout << "<无符号整数>" << endl;
        cout << "<步长>" << endl;
    #endif
    return true;
}

bool statement_analysis::parameter_table_value(vector<value_type> &type_list)
{
    type_list.clear();
    expression_analysis exp_analysis(table, output_buffer, error_buffer,
            mid_code_buffer, manager, scope);
    value_type temp = NOT_VALUE;
    bool certain;
    int value;
    string assign_to;
    vector<string> to_push;
    if (!exp_analysis.expression(temp, certain, value, assign_to))
    {
        output_buffer << "<值参数表>" << endl;
        #ifdef DEBUG
            cout << "<值参数表>" << endl;
        #endif
        return true;
    }
    string value_str = to_string(value);
    // mid_code_buffer << mid_code::push_mid(certain? value_str: assign_to);
    to_push.emplace_back(certain? value_str: assign_to);
    type_list.emplace_back(temp);
    while (true)
    {
        if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({COMMA})))
        {
            break;
        }
        exp_analysis.expression(temp, certain, value, assign_to);
        value_str = to_string(value);
        // mid_code_buffer << mid_code::push_mid(certain? value_str: assign_to);
        to_push.emplace_back(certain? value_str: assign_to);
        type_list.emplace_back(temp);
    }
    for (string para: to_push)
    {
        mid_code_buffer.emplace_back(mid_code::push_mid(para));
    }
    output_buffer << "<值参数表>" << endl;
    #ifdef DEBUG
        cout << "<值参数表>" << endl;
    #endif
    return true;
}

bool statement_analysis::return_value_func_call_statement(value_type &func_re_type)
{
    // 内部再次进行检查
    assert(manager.has_expect_sequence(func_call_head, false));

    string idenfr = manager.get_now_word_value().get_str_value();
    func_type type;
    table.get_item_func_type(scope, idenfr, type);
    if (!(type == RE_INT_TYPE_FUNC || type == RE_CHAR_TYPE_FUNC))
    {
        throw not_define_error();
    }
    if (type == RE_CHAR_TYPE_FUNC)
    {
        func_re_type = CHAR_TYPE_VALUE;
    }
    else if (type == RE_INT_TYPE_FUNC)
    {
        func_re_type = INT_TYPE_VALUE;
    }
    manager.print_move_front(output_buffer, func_call_head.size());
    vector<value_type> arg_list;
    parameter_table_value(arg_list);
    symbol_table_item * item = table.get_item_ptr_expc(scope, idenfr);
    item->valid_param_type(arg_list);
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RPARENT})))
    {
        throw miss_rparent();
    }
    mid_code_buffer.emplace_back(mid_code::call_mid(idenfr));
    output_buffer << "<有返回值函数调用语句>" << endl;
    #ifdef DEBUG
        cout << "<有返回值函数调用语句>" << endl;
    #endif
    return true;
}

bool statement_analysis::return_void_func_call_statement()
{
    assert(manager.has_expect_sequence(func_call_head, false));
    string idenfr = manager.get_now_word_value().get_str_value();
    func_type type;
    table.get_item_func_type(scope, idenfr, type);
    assert(type == VOID_TYPE_FUNC);

    manager.print_move_front(output_buffer, func_call_head.size());
    vector<value_type> arg_list;
    parameter_table_value(arg_list);
    symbol_table_item * item = table.get_item_ptr_expc(scope, idenfr);
    item->valid_param_type(arg_list);
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RPARENT})))
    {
        throw miss_rparent();
    }
    mid_code_buffer.emplace_back(mid_code::call_mid(idenfr));
    output_buffer << "<无返回值函数调用语句>" << endl;
    #ifdef DEBUG
        cout << "<无返回值函数调用语句>" << endl;
    #endif
    return true;
}

bool statement_analysis::func_call_statement()
{
    assert(manager.has_expect_sequence(func_call_head, false));
    string idenfr = manager.get_now_word_value().get_str_value();

    try
    {
        func_type type = table.get_item_ptr_expc(scope, idenfr)->get_func_type();
        if (type == RE_CHAR_TYPE_FUNC || type == RE_INT_TYPE_FUNC)
        {
            value_type temp = NOT_VALUE;
            return return_value_func_call_statement(temp);
        }
        else if (type == VOID_TYPE_FUNC)
        {
            return return_void_func_call_statement();
        }
        else
        {
            // TODO: ERROR
            return false;
        }
    }
    catch (syntax_error& expc)
    {
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        manager.skip(SEMICN);
        return true;
    }
}

statement_analysis::statement_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                                       vector<mid_code> &midCodeBuffer, words_manager &manager, const string &scope,
                                       value_type re_value_type)
        : table(table), output_buffer(outputBuffer),
        error_buffer(errorBuffer), mid_code_buffer(midCodeBuffer),
        manager(manager), scope(scope), re_value_type(re_value_type)
{
    cnt_re = 0;
}

bool statement_analysis::statement()
{
    // 外层试探，内层移动加输出
    int max_length = 0;
    if (
            manager.has_expect_sequence(var_assign_start, false)
            || manager.has_expect_sequence(arr_assign_start, false)
            )
    {
        _update_max_length(max_length, var_assign_start.size());
        _update_max_length(max_length, arr_assign_start.size());
        _assign_statement();
        if (!manager.has_sequence_then_print_move(output_buffer, semicn))
        {
            miss_semicn expc;
            expc.record_msg(error_buffer, manager.get_recent_line_num());
            manager.semicn_end_skip();
        }
    }
    else if (manager.has_expect_sequence(func_call_head, false))
    {
        _update_max_length(max_length, func_call_head.size());
        func_call_statement();
        if (!manager.has_sequence_then_print_move(output_buffer, semicn))
        {
            miss_semicn expc;
            expc.record_msg(error_buffer, manager.get_recent_line_num());
            manager.semicn_end_skip();
        }
    }
    else if (manager.has_expect_sequence(if_sequence_head, false))
    {
        _update_max_length(max_length, if_sequence_head.size());
        _condition_statement();
    }
    else if (
            manager.has_expect_sequence(while_sequence_head, false)
            || manager.has_expect_sequence(do_while_sequence_head, false)
            || manager.has_expect_sequence(for_sequence_head, false)
            )
    {
        _update_max_length(max_length, while_sequence_head.size());
        _update_max_length(max_length, do_while_sequence_head.size());
        _update_max_length(max_length, for_sequence_head.size());
        _loop_statement();
    }
    else if (manager.has_expect_sequence(scanf_sequence_head, false))
    {
        _update_max_length(max_length, scanf_sequence_head.size());
        _read_statement();
        if (!manager.has_sequence_then_print_move(output_buffer, semicn))
        {
            miss_semicn expc;
            expc.record_msg(error_buffer, manager.get_recent_line_num());
            manager.semicn_end_skip();
        }
    }
    else if (manager.has_expect_sequence(printf_sequence_head, false))
    {
        _update_max_length(max_length, printf_sequence_head.size());
        _write_statement();
        if (!manager.has_sequence_then_print_move(output_buffer, semicn))
        {
            miss_semicn expc;
            expc.record_msg(error_buffer, manager.get_recent_line_num());
            manager.semicn_end_skip();
        }
    }
    else if (manager.has_expect_sequence(return_sequence_head, false))
    {
        _update_max_length(max_length, return_sequence_head.size());
        _return_statement();
        if (!manager.has_sequence_then_print_move(output_buffer, semicn))
        {
            miss_semicn expc;
            expc.record_msg(error_buffer, manager.get_recent_line_num());
            manager.semicn_end_skip();
        }
    }
    else if (manager.has_expect_sequence(statement_list_sequence_head, false))
    {
        _update_max_length(max_length, statement_list_sequence_head.size());
        _statement_list();
    }
    else if (manager.has_sequence_then_print_move(output_buffer, semicn))
    {
        // 空语句
    }
    else if (manager.has_invalid_word_in_range(max_length))
    {
        lexer_error expc;
        expc.record_msg(error_buffer, manager.get_now_word_value().get_line_num());
        manager.block_skip();
    }
    else
    {
        return false;
    }

    output_buffer << "<语句>" << endl;
    #ifdef DEBUG
        cout << "<语句>" << endl;
    #endif
    return true;
}

bool statement_analysis::statement_list()
{
    while (statement());

    output_buffer << "<语句列>" << endl;
    #ifdef DEBUG
        cout << "<语句列>" << endl;
    #endif
    return true;
}

bool statement_analysis::_statement_list()
{
    if (!manager.has_sequence_then_print_move(output_buffer, statement_list_sequence_head))
    {
        // TODO: ERROR
        return false;
    }
    statement_list();
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({RBRACE})))
    {
        // TODO: ERROR
        return false;
    }
    return true;
}

void statement_analysis::condition_block_skip()
{
    set<word_type> end_set{LBRACE, RBRACE, SEMICN};
    manager.skip(end_set);
}

int statement_analysis::get_re_cnt()
{
    return cnt_re;
}

bool statement_analysis::loop_condition_formula(mid_code &loop_mid_code, label_pair &loop_label_pair)
{
    expression_analysis exp_analysis(table, output_buffer, error_buffer,
                                     mid_code_buffer, manager, scope);
    value_type temp = NOT_VALUE;
    bool certain;
    int value;
    string assign_to;
    label_pair temp_pair = get_new_loop_label_pair(scope);
    string loop_over_label = temp_pair.first;

    // mid_code_buffer.emplace_back(mid_code::label_mid(temp_pair.first));

    exp_analysis.expression(temp, certain, value, assign_to);
    string value_str = to_string(value);
    loop_label_pair = temp_pair;
    if (temp != INT_TYPE_VALUE)
    {
        throw condition_type_error();
    }
    word_type type = manager.get_now_word_type();
    if (relation_op.count(type) == 1)
    {
        manager.print_move_front(output_buffer, 1);
        bool certain2;
        int value2;
        string assign_to2;
        exp_analysis.expression(temp, certain2, value2, assign_to2);
        string value_str2 = to_string(value2);
        if (temp != INT_TYPE_VALUE)
        {
            throw condition_type_error();
        }

        loop_mid_code = mid_code::condition_mid(
                type,
                certain? value_str: assign_to,
                certain2? value_str2: assign_to2,
                loop_over_label, false);

        output_buffer << "<条件>" << endl;
        #ifdef DEBUG
            cout << "<条件>" << endl;
        #endif
        return true; // not must true
    }
    else
    {
        if (certain)
        {
            loop_mid_code = mid_code::condition_mid(
                    NEQ, value, 0, loop_over_label, false);
        }
        else
        {
            loop_mid_code = mid_code::condition_mid(
                    NEQ, assign_to, 0, loop_over_label, false);
        }
        output_buffer << "<条件>" << endl;
        #ifdef DEBUG
            cout << "<条件>" << endl;
        #endif
        return true;
    }

}

bool statement_analysis::do_while_condition_formula(mid_code &loop_mid_code, string &loop_begin)
{
    expression_analysis exp_analysis(table, output_buffer, error_buffer,
                                     mid_code_buffer, manager, scope);
    value_type temp = NOT_VALUE;
    bool certain;
    int value;
    string assign_to;
    exp_analysis.expression(temp, certain, value, assign_to);
    string value_str = to_string(value);
    if (temp != INT_TYPE_VALUE)
    {
        throw condition_type_error();
    }
    word_type type = manager.get_now_word_type();
    if (relation_op.count(type) == 1)
    {
        manager.print_move_front(output_buffer, 1);
        bool certain2;
        int value2;
        string assign_to2;
        exp_analysis.expression(temp, certain2, value2, assign_to2);
        string value_str2 = to_string(value2);
        if (temp != INT_TYPE_VALUE)
        {
            throw condition_type_error();
        }
        loop_mid_code = mid_code::condition_mid(
                type,
                certain? value_str: assign_to,
                certain2? value_str2: assign_to2,
                loop_begin, false);

        output_buffer << "<条件>" << endl;
        #ifdef DEBUG
            cout << "<条件>" << endl;
        #endif
        return true; // not must true
    }
    else
    {
        if (certain)
        {
            loop_mid_code = mid_code::condition_mid(
                    NEQ, value, 0, loop_begin, false);
        }
        else
        {
            loop_mid_code = mid_code::condition_mid(
                    NEQ, assign_to, 0, loop_begin, false);
        }
        output_buffer << "<条件>" << endl;
        #ifdef DEBUG
            cout << "<条件>" << endl;
        #endif
        return true;
    }
}
