//
// Created by 王梓沐 on 2019/10/1.
//

#ifndef COMPILER_2_VARIABLE_ANALYSIS_H
#define COMPILER_2_VARIABLE_ANALYSIS_H


#include <sstream>
#include "words_manager.h"
#include "symbol_table.h"
#include "mid_code.h"

class variable_analysis {
private:
    enum definition_type {INTDEFINE, CHARDEFINE};
    symbol_table & table;
    ostringstream & output_buffer;
    ostringstream & error_buffer;
    vector<mid_code> & mid_code_buffer;
    words_manager & manager;
    string scope;
    const vector<word_type> CONST_INT_DEFINE_HEAD_SEQUENCE = {
            CONSTTK, INTTK, IDENFR, ASSIGN
    };
    const vector<word_type> CONST_INTTK = {
            CONSTTK, INTTK
    };
    const vector<word_type> CONST_INT_DEFINE_UNIT_SEQUENCE = {
            IDENFR, ASSIGN
    };
    const vector<word_type> CONST_CHAR_DEFINE_HEAD_SEQUENCE = {
            CONSTTK, CHARTK, IDENFR, ASSIGN
    };
    const vector<word_type> CONST_CHARTK = {
            CONSTTK, CHARTK
    };
    const vector<word_type> CONST_CHAR_DEFINE_UNIT_SEQUENCE = {
            IDENFR, ASSIGN
    };
    const vector<word_type> VAR_INT_DEFINE = {
            INTTK, IDENFR
    };
    const vector<word_type> VAR_INT_ARR_DEFINE = {
            INTTK, IDENFR, LBRACK, INTCON, RBRACK
    };
    const vector<word_type> VAR_CHAR_DEFINE = {
            CHARTK, IDENFR
    };
    const vector<word_type> VAR_CHAR_ARR_DEFINE = {
            CHARTK, IDENFR, LBRACK, INTCON, RBRACK
    };
    const vector<word_type> VAR_DEFINE = {
            IDENFR
    };
    const vector<word_type> ARR_DEFINE = {
            IDENFR, LBRACK, INTCON, RBRACK
    };
    const vector<word_type> ARR_DEFINE_HEAD = {
            IDENFR, LBRACK
    };
    const vector<word_type> CHAR_FUNC_REC = {
            CHARTK, IDENFR, LPARENT
    };
    const vector<word_type> INT_FUNC_REC = {
            INTTK, IDENFR, LPARENT
    };

    const set<word_type> CONST_VAR_DEFINE_SPLIT = {
            COMMA, SEMICN
    };

    bool _const_description_line();
    bool _const_definition(definition_type);
    bool _variable_definition_line();
    bool _variable_definition();
    bool _variable_definition_unit(definition_type var_def_type);
    bool _integer(int & value_store);
    void define_unit_skip();
public:
    variable_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                      vector<mid_code> &midCodeBuffer, words_manager &manager, const string &scope);
    bool const_description();
    bool variable_description();
};


#endif //COMPILER_2_VARIABLE_ANALYSIS_H
