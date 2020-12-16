//
// Created by 王梓沐 on 2019/10/2.
//

#ifndef COMPILER_2_FUNCTION_ANALYSIS_H
#define COMPILER_2_FUNCTION_ANALYSIS_H


#include <sstream>
#include "Lexer.h"
#include "words_manager.h"
#include "symbol_table.h"
#include "mid_code.h"

using namespace std;

class function_analysis
{
private:
    symbol_table & table;
    ostringstream & output_buffer;
    ostringstream & error_buffer;
    vector<mid_code> & mid_code_buffer;
    words_manager & manager;
    string scope;

    const vector<word_type> INT_IDENFR = {
            INTTK, IDENFR
    };
    const vector<word_type> CHAR_IDENFR = {
            CHARTK, IDENFR
    };
    const vector<word_type> DECLARE_HEAD_VOID = {
            VOIDTK, IDENFR
    };
    const vector<word_type> MAIN_FUNC_REC = {
            VOIDTK, MAINTK, LPARENT, RPARENT
    };
    const vector<word_type> CHAR_FUNC_REC = {
            CHARTK, IDENFR, LPARENT
    };
    const vector<word_type> INT_FUNC_REC = {
            INTTK, IDENFR, LPARENT
    };
    const vector<word_type> VOID_FUNC_REC = {
            VOIDTK, IDENFR, LPARENT
    };
    bool _function_declare_head_return();
    bool _function_declare_head_void();
    bool _parameter_table(vector<value_type> &para_list, string &func_idenfr);
    bool _parameter_table_unit(value_type &para_type, string &func_idenfr);
    bool _parameter_table_brackets(vector<value_type> &para_list, string &func_idenfr);
    bool _compound_statement(const string &func_scope, value_type re_value_type);
public:
    bool function_definition_unit();
    bool function_have_return_definition();
    bool function_no_return_definition();
    bool main_function();
    bool function_definition();
    function_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                      vector<mid_code> &midCodeBuffer, words_manager &manager, const string &scope);
};


#endif //COMPILER_2_FUNCTION_ANALYSIS_H
