//
// Created by AERO on 2019/11/16.
//

#include <iostream>
#include "mid_code.h"
#include "utils.h"


map<string, string> const_string_record;

 map<word_type, op_type> relation_op_reverse_map{
        pair<word_type, op_type> (LSS, BGE),
        pair<word_type, op_type> (LEQ, BGT),
        pair<word_type, op_type> (GRE, BLE),
        pair<word_type, op_type> (GEQ, BLT),
        pair<word_type, op_type> (EQL, BNE),
        pair<word_type, op_type> (NEQ, BEQ),
};

map<word_type, op_type> relation_op_to_branch_map{
        pair<word_type, op_type> (LSS, BLT),
        pair<word_type, op_type> (LEQ, BLE),
        pair<word_type, op_type> (GRE, BGT),
        pair<word_type, op_type> (GEQ, BGE),
        pair<word_type, op_type> (EQL, BEQ),
        pair<word_type, op_type> (NEQ, BNE),
};

map<word_type, op_type> cal_op_to_mid_op{
        pair<word_type, op_type> (PLUS, ADD),
        pair<word_type, op_type> (MINU, SUB),
        pair<word_type, op_type> (MULT, MUL),
        pair<word_type, op_type> (DIV, DIVE),
};

int const_string_count = 0;

bool is_num(string & num_str)
{
    int i = 0;
    if (num_str[0] == '-')
    {
        i++;
    }
    while (i < num_str.size())
    {
        if (!isdigit(num_str[i]))
        {
            return false;
        }
        i++;
    }
    return true;
}

string insert_const_str(string & src)
{
    const string& temp = src;
    if (map_contains(const_string_record, temp))
    {
        return const_string_record.find(src)->second;
    }
    string label = "fry_const_string_" + to_string(const_string_count++);
    const_string_record[src] = label;
    return label;
}

bool is_temp_var(string &str)
{
    return str.find_first_of('#') != str.npos;
}


string temp_var_to_local_name(string &str)
{
    if (str[0] != '#')
    {
        return str;
    }
    string temp = str;
    temp[0] = '$';
    return temp;
}

