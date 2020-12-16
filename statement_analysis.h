//
// Created by 王梓沐 on 2019/10/2.
//

#ifndef COMPILER_2_STATEMENT_ANALYSIS_H
#define COMPILER_2_STATEMENT_ANALYSIS_H

#include <sstream>
#include "words_manager.h"
#include "expression_analysis.h"

using namespace std;

class statement_analysis
{
private:
    set<word_type> relation_op = {
            LSS, LEQ, GRE, GEQ, EQL, NEQ
    };
    vector<word_type> var_assign_start = {
            IDENFR, ASSIGN
    };
    vector<word_type> arr_assign_start = {
            IDENFR, LBRACK
    };
    vector<word_type> arr_assign_mid = {
            RBRACK, ASSIGN
    };
    vector<word_type> if_sequence_head = {
            IFTK, LPARENT
    };
    vector<word_type> else_sequence_head = {
            ELSETK
    };
    vector<word_type> while_sequence_head = {
            WHILETK, LPARENT
    };
    vector<word_type> do_while_sequence_head = {
            DOTK,
    };
    vector<word_type> for_sequence_head = {
            FORTK, LPARENT, IDENFR, ASSIGN
    };
    vector<word_type> for_sequence_tail_add = {
            SEMICN, IDENFR, ASSIGN, IDENFR, PLUS
    };
    vector<word_type> for_sequence_tail_sub = {
            SEMICN, IDENFR, ASSIGN, IDENFR, MINU
    };
    vector<word_type> scanf_sequence_head = {
            SCANFTK, LPARENT, IDENFR
    };
    vector<word_type> printf_sequence_head = {
            PRINTFTK, LPARENT
    };
    vector<word_type> return_sequence_head = {
            RETURNTK
    };
    vector<word_type> func_call_head = {
            IDENFR, LPARENT
    };
    vector<word_type> semicn = {
            SEMICN
    };
    vector<word_type> statement_list_sequence_head = {
            LBRACE
    };

    symbol_table & table;
    ostringstream & output_buffer;
    ostringstream & error_buffer;
    vector<mid_code> & mid_code_buffer;
    words_manager & manager;
    string scope;
    value_type re_value_type;
    int cnt_re;

    bool _condition_statement();
    bool _assign_statement();
    bool _loop_statement();
    bool _while_statement();
    bool _do_while_statement();
    bool _for_statement();
    bool _read_statement();
    bool _write_statement();
    bool _return_statement();
    bool branch_condition_formula(mid_code &branch_mid_code, label_pair &else_label_pair);
    bool loop_condition_formula(mid_code &loop_mid_code, label_pair &loop_label_pair);
    bool do_while_condition_formula(mid_code &loop_mid_code, string &loop_begin);
    bool _step_size(); // 看起来好蠢
    bool _statement_list();
    void condition_block_skip();

public:
    statement_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                       vector<mid_code> &midCodeBuffer, words_manager &manager, const string &scope,
                       value_type re_value_type);
    bool parameter_table_value(vector<value_type> &type_list);
    bool func_call_statement();
    bool return_value_func_call_statement(value_type &func_re_type);
    bool return_void_func_call_statement();
    bool statement();
    bool statement_list();
    int get_re_cnt();
};


#endif //COMPILER_2_STATEMENT_ANALYSIS_H
