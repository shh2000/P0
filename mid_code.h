//
// Created by AERO on 2019/11/16.
//

#ifndef COMPILER_CODE_GENERATE_MID_CODE_H
#define COMPILER_CODE_GENERATE_MID_CODE_H

#include <string>
#include <vector>
#include <cassert>
#include "global_type.h"
#include "Lexer.h"

using namespace std;

enum op_type {
    FUNC, PARA, PUSH,
    CALL, GET,RET,
    VAR, ARR, ADD,
    SUB, MUL, DIVE,
    BGE, BGT, BLE,
    BLT, BNE, BEQ,
    GOTO, ARR_SET, ARR_READ,
    PRINTF, SCANF, EXIT,
    LABEL, ASSIGN_TO, NEXT_LINE,
    STR
};

const vector<string> op_string = {
        "@func", "@para", "@push",
        "@call", "@get", "@ret",
        "@mid_var", "@arr", "@add",
        "@sub", "@mul", "@div",
        "@bge", "@bgt", "@ble",
        "@blt", "@bne", "@beq",
        "@goto", "@arr_set", "@arr_read",
        "@printf", "@scanf", "@exit",
        "@label", "@assign", "@next_line",
        "@string"
};


typedef pair<string, string> label_pair;

extern map<string, int> scope_temp_var_count;
extern map<string, int> scope_if_count;
extern map<string, int> scope_loop_count;

op_type op_string_transfer(string & op_str);
string op_string_transfer(op_type type);

value_type value_type_string_transfer(string & str);
string value_type_string_transfer(value_type type);

string get_new_var_name(string & scope);
label_pair get_new_if_label_pair(string & scope);
label_pair get_new_loop_label_pair(string & scope);
bool mode_cmp(op_type mod, int src0, int src1);

class mid_code
{
private:
    op_type type;
    string use_str;
    string use_str_print;
    vector<string> factor_strings;
    bool valid = true;
public:
    mid_code(op_type, vector<string> &attach_msg, bool valid = true);
    friend ostream &operator<<(ostream &os, const mid_code &mid);
    static mid_code func_def_mid(string & func_name);
    static mid_code para_mid(string &para_name, value_type type_);
    static mid_code push_mid(string &value);
    static mid_code push_mid(int value);
    static mid_code call_mid(string & func_name);
    static mid_code get_mid(string & var_name);
    static mid_code ret_mid(string & value);
    static mid_code ret_mid(int value);
    static mid_code var_mid(string & var_name, value_type type_);
    static mid_code arr_mid(string & arr_name, value_type type_, int length);
    static mid_code calculate_mid(word_type type, string & dst, string & src0, string & src1);
    static mid_code calculate_mid(word_type type, string & dst, int src0, string & src1);
    static mid_code calculate_mid(word_type type, string & dst, string & src0, int src1);
    static mid_code calculate_mid(word_type type, string & dst, int src0, int src1);
    static mid_code condition_mid(word_type relation_op, string & src0,
            string & src1, string & target_label, bool reverse);
    static mid_code condition_mid(word_type relation_op, int src0,
            string & src1, string & target_label, bool reverse);
    static mid_code condition_mid(word_type relation_op, string & src0,
            int src1, string & target_label, bool reverse);
    static mid_code condition_mid(word_type relation_op, int src0,
            int src1, string & target_label, bool reverse);
    static mid_code goto_mid(string & target_label);
    static mid_code arr_set_mid(string & arr_name, string & index, string & value);
    static mid_code arr_read_mid(string & arr_name, string & index, string & target);
    static mid_code printf_mid(value_type print_type, string & content);
    static mid_code printf_mid(value_type print_type, int content);
    static mid_code scanf_mid(value_type input_type, string & target);
    static mid_code exit_mid();
    static mid_code label_mid(string & label_name);
    static mid_code assign_mid(string & dst, string & src);
    static mid_code assign_mid(string & dst, int value);
    static mid_code get_temp_mid();
    static mid_code print_next_line_mid();
    static mid_code declare_string_mid(string src_str, string label_name);
    op_type get_op_type();
    bool is_valid();
    string &operator[](int);
    int factors_num();
    vector<string> get_attach_copy();
    static mid_code smart_gen_mid_code(op_type type, vector<string> attachment, bool valid = true);
    void reset_string();
    int get_attach_size();
    string toString();
    string AS_check_string();
};


mid_code line_to_mid_code(string & src_str);

#endif //COMPILER_CODE_GENERATE_MID_CODE_H
