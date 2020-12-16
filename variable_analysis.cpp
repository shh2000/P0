//
// Created by 王梓沐 on 2019/10/1.
//
#include "wzm_settings.h"
#include <iostream>
#include <cassert>
#include "variable_analysis.h"
#include "compile_error.h"

using namespace std;

bool variable_analysis::_const_description_line() {
    try
    {
        if (manager.has_expect_sequence(CONST_INT_DEFINE_HEAD_SEQUENCE, false))
        {
            _const_definition(INTDEFINE);
        }
        else if (manager.has_expect_sequence(CONST_CHAR_DEFINE_HEAD_SEQUENCE, false))
        {
            _const_definition(CHARDEFINE);
        }
        else if (manager.has_invalid_word_in_range(
                max(CONST_INT_DEFINE_HEAD_SEQUENCE.size(), CONST_CHAR_DEFINE_HEAD_SEQUENCE.size()))
                )
        {
            throw lexer_error();
        }
        else
        {
            // 常量定义结束
            return false;
        }
    }
    catch (lexer_error& expc)
    {
        // 如果是因为非法字符，则跳过这行接着搞一手。
        expc.record_msg(error_buffer, manager.get_now_word_value().get_line_num());
        manager.skip();
        return true;
    }
    if (!manager.has_sequence_then_print_move(output_buffer, vector<word_type>({SEMICN})))
    {
        // 缺少分号，则这行崩掉但是，下一行还得接着搞
        miss_semicn excp;
        excp.record_msg(error_buffer, manager.get_recent_line_num());
        manager.miss_semicn_skip_line();
    }
    return true;
}

bool variable_analysis::_const_definition(definition_type type) {
    // 处理开头的那个定义
    if (type == INTDEFINE)
    {
        assert(manager.has_sequence_then_print_move(output_buffer, CONST_INTTK));
        while (true)
        {
            if (!manager.has_sequence_then_print_move(output_buffer, CONST_INT_DEFINE_UNIT_SEQUENCE))
            {
                if (manager.has_invalid_word_in_range(CONST_INT_DEFINE_UNIT_SEQUENCE.size()))
                {
                    throw lexer_error();
                }
                else
                {
                    break;
                }
            }
            word temp = manager.get_recent_word(2);
            assert(temp.get_type() == IDENFR);
            string idenfr = temp.get_str_value();
            int value;
            try
            {
                _integer(value);
            }
            catch (const_init_error& excp)
            {
                excp.record_msg(error_buffer, manager.get_recent_line_num());
                define_unit_skip();
                continue;
            }
            try
            {
                table.push_item(
                        scope,
                        symbol_table_item::get_int_constant(scope, idenfr, value),
                        true);
            }
            catch (redefine_error& excp)
            {
                excp.record_msg(error_buffer, manager.get_recent_line_num());
                define_unit_skip();
                continue;
            }
            if (!manager.has_sequence_then_print_move(output_buffer, COMMA))
            {
                break;
            }
        }
    }
    else
    {
        // TODO: 读取第一个char值
        assert(manager.has_sequence_then_print_move(output_buffer, CONST_CHARTK));
        while (manager.has_sequence_then_print_move(output_buffer, CONST_CHAR_DEFINE_UNIT_SEQUENCE))
        {
            word temp = manager.get_recent_word(2);
            assert(temp.get_type() == IDENFR);
            string idenfr = temp.get_str_value();
            if (!manager.has_sequence_then_print_move(output_buffer, CHARCON))
            {
                word_type temp_type = manager.get_now_word_type();
                if (temp_type == INVALID)
                {
                    throw lexer_error();
                }
                else
                {
                    const_init_error excep;
                    excep.record_msg(error_buffer, manager.get_recent_line_num());
                }
                define_unit_skip();
                continue;
            }
            char value = manager.get_recent_word().get_str_value()[0];
            try
            {
                table.push_item(
                        scope,
                        symbol_table_item::get_char_constant(scope, idenfr, value),
                        true);
            }
            catch (redefine_error& excp)
            {
                excp.record_msg(error_buffer, manager.get_recent_line_num());
                define_unit_skip();
                continue;
            }
            if (!manager.has_sequence_then_print_move(output_buffer, COMMA))
            {
                break;
            }
        }
    }
    output_buffer << "<常量定义>" << endl;
    #ifdef DEBUG
        cout << "<常量定义>" << endl;
    #endif
    return true;
}

bool variable_analysis::const_description()
{
    bool has_description = false;
    while (_const_description_line()) has_description = true;
    if (has_description)
    {
        output_buffer << "<常量说明>" << endl;
        #ifdef DEBUG
            cout << "<常量说明>" << endl;
        #endif
    }
    return has_description;
}

bool variable_analysis::_variable_definition_line()
{
    try
    {
        if (
                (
                        manager.has_expect_sequence(VAR_INT_DEFINE, false)
                        || manager.has_expect_sequence(VAR_CHAR_DEFINE, false)
                )
                && !(
                        manager.has_expect_sequence(CHAR_FUNC_REC, false)
                        || manager.has_expect_sequence(INT_FUNC_REC, false)
                )
                )
        {
            _variable_definition();
        }
        else if (manager.has_invalid_word_in_range(1))
        {
            throw lexer_error();
        }
        else
        {
            //TODO: ERROR
            return false;
        }
    }
    catch (lexer_error& expc)
    {
        // 如果是因为非法字符，则跳过这行接着搞一手。
        expc.record_msg(error_buffer, manager.get_now_word_value().get_line_num());
        manager.skip();
        return true;
    }
    if (!manager.has_sequence_then_print_move(output_buffer, SEMICN))
    {
        miss_semicn expc;
        expc.record_msg(error_buffer, manager.get_recent_line_num());
        manager.miss_semicn_skip_line();
    }
    return true;
}

bool variable_analysis::_variable_definition()
{
    // 注意比对顺序
    vector<word_type> sequence;
    bool has_define_unit = false;
    if (manager.get_now_word_type() == INTTK)
    {
        manager.print_move_front(output_buffer, 1);
        while (_variable_definition_unit(INTDEFINE))
        {
            has_define_unit = true;
            if (!manager.has_sequence_then_print_move(output_buffer, COMMA))
            {
                break;
            }
        }
    }
    else if (manager.get_now_word_type() == CHARTK)
    {
        manager.print_move_front(output_buffer, 1);
        while (_variable_definition_unit(CHARDEFINE))
        {
            has_define_unit = true;
            if (!manager.has_sequence_then_print_move(output_buffer, COMMA))
            {
                break;
            }
        }
    }
    else
    {
        assert(false);
    }
    if (!has_define_unit)
    {
        // TODO: ERROR
        return false;
    }

    output_buffer << "<变量定义>" << endl;
    #ifdef DEBUG
        cout << "<变量定义>" << endl;
    #endif
    return true;
}

bool variable_analysis::variable_description()
{
    bool has_description = false;
    while (_variable_definition_line()) has_description = true;
    if (has_description)
    {
        output_buffer << "<变量说明>" << endl;
        #ifdef DEBUG
            cout << "<变量说明>" << endl;
        #endif
    }
    return has_description;
}

variable_analysis::variable_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                                     vector<mid_code> &midCodeBuffer, words_manager &manager, const string &scope)
        : table(table), output_buffer(outputBuffer), error_buffer(errorBuffer), mid_code_buffer(midCodeBuffer),
        manager(manager), scope(scope)
{

}

bool variable_analysis::_variable_definition_unit(definition_type var_def_type)
{
    // 注意顺序，先判数组，再判单变量。
    if (manager.has_sequence_then_print_move(output_buffer, ARR_DEFINE_HEAD))
    {
        string idenfr = manager.get_recent_word(2).get_str_value();
        if (!manager.has_sequence_then_print_move(output_buffer, INTCON))
        {
            throw lexer_error();
        }
        output_buffer << "<无符号整数>" << endl;
        #ifdef DEBUG
            cout << "<无符号整数>" << endl;
        #endif
        int var = atoi(
                manager.get_word_by_index(
                        manager.get_index() - 1
                        ).get_str_value().c_str()
        );
        if (var <= 0)
        {
            // TODO: ERROR
            return false;
        }
        if (!manager.has_sequence_then_print_move(output_buffer, RBRACK))
        {
            miss_rbrack expc;
            expc.record_msg(error_buffer, manager.get_recent_line_num());
            define_unit_skip();
        }
        symbol_table_item * item = (var_def_type == INTDEFINE)?
                symbol_table_item::get_int_arr_var(scope, idenfr, var):
                symbol_table_item::get_char_arr_var(scope, idenfr, var);
        mid_code_buffer.emplace_back(mid_code::arr_mid(
                idenfr, var_def_type == INTDEFINE? INT_TYPE_VALUE:CHAR_TYPE_VALUE, var));
        try
        {
            table.push_item(scope, item, true);
        }
        catch (redefine_error& expc)
        {
            expc.record_msg(error_buffer, manager.get_recent_line_num());
            define_unit_skip();
        }
        return true;
    }
    else if (manager.has_sequence_then_print_move(output_buffer, VAR_DEFINE))
    {
        string idenfr = manager.get_recent_word().get_str_value();
        symbol_table_item * item = (var_def_type == INTDEFINE) ?
                                   symbol_table_item::get_int_var(scope, idenfr) :
                                   symbol_table_item::get_char_var(scope, idenfr);
        mid_code_buffer.emplace_back(mid_code::var_mid(
                idenfr, var_def_type == INTDEFINE? INT_TYPE_VALUE:CHAR_TYPE_VALUE));
        try
        {
            table.push_item(scope, item, true);
        }
        catch (redefine_error& expc)
        {
            expc.record_msg(error_buffer, manager.get_recent_line_num());
            define_unit_skip();
        }
        return true;
    }
    else
    {
        throw lexer_error();
    }
}

bool variable_analysis::_integer(int & value_store)
{
    if (manager.has_sequence_then_print_move(output_buffer, vector<word_type>({PLUS, INTCON})))
    {
        string temp = manager[manager.get_index() - 1].get_str_value();
        value_store = atoi(temp.c_str());
    }
    else if (manager.has_sequence_then_print_move(output_buffer, vector<word_type>({MINU, INTCON})))
    {
        string temp = manager[manager.get_index() - 1].get_str_value();
        value_store = -atoi(temp.c_str());
    }
    else if (manager.has_sequence_then_print_move(output_buffer, INTCON))
    {
        string temp = manager[manager.get_index() - 1].get_str_value();
        value_store = atoi(temp.c_str());
    }
    else
    {
        if (manager.has_invalid_word_in_range(2))
        {
            throw lexer_error();
        }
        else
        {
            throw const_init_error();
        }
    }
    output_buffer << "<无符号整数>" << endl;
    output_buffer << "<整数>" << endl;
    #ifdef DEBUG
        cout << "<无符号整数>" << endl;
        cout << "<整数>" << endl;
    #endif
    return true;
}

void variable_analysis::define_unit_skip()
{
    manager.skip(CONST_VAR_DEFINE_SPLIT);
    manager.has_sequence_then_print_move(output_buffer, COMMA);
}
