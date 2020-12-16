//
// Created by 王梓沐 on 2019/10/3.
//

#ifndef COMPILER_2_EXPRESSION_ANALYSIS_H
#define COMPILER_2_EXPRESSION_ANALYSIS_H


#include <sstream>
#include "words_manager.h"
#include "symbol_table.h"
#include "mid_code.h"

using namespace std;

typedef pair<word_type, string> op_var;

class expression_analysis
{
private:
    symbol_table & table;
    ostringstream & output_buffer;
    ostringstream & error_buffer;
    vector<mid_code> & mid_code_buffer;
    words_manager & manager;
    string scope;

    // 注意使用顺序!!!!!!!!
    vector<word_type> var_factor_head = {
            IDENFR
    };
    vector<word_type> arr_factor_head = {
            IDENFR, LBRACK
    };
    vector<word_type> exp_factor_head = {
            LPARENT
    };
    vector<word_type> unsigned_int_factor_head = {
            INTCON
    };
    vector<word_type> positive_int_factor_head = {
            PLUS, INTCON
    };
    vector<word_type> negative_int_factor_head = {
            MINU, INTCON
    };
    vector<word_type> char_factor_head = {
            CHARCON
    };
    vector<word_type> func_call_head = {
            IDENFR, LPARENT
    };

    bool _item(value_type &vue_type, bool &certain, int &value, string &assign_to);
    bool _factor(value_type &vue_type, bool &certain, int &value, string &assign_to);
    bool _var_factor(value_type &vue_type, bool &certain, int &value, string &assign_to);
    bool _arr_factor(value_type &vue_type, string &assign_to);
    bool _exp_factor(bool &certain, int &value, string &assign_to);
    bool _int_factor(int &value);
    bool _char_factor(int &value);

public:
    expression_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                        vector<mid_code> &midCodeBuffer, words_manager &manager, const string &scope);
    bool expression(value_type &vue_type, bool &certain, int &value, string &assign_to);
};


#endif //COMPILER_2_EXPRESSION_ANALYSIS_H
