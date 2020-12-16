//
// Created by AERO on 2019/11/16.
//

#ifndef COMPILER_CODE_GENERATE_UTILS_H
#define COMPILER_CODE_GENERATE_UTILS_H

#include "mid_code.h"

using namespace std;


extern map<word_type, op_type> relation_op_reverse_map;

extern map<word_type, op_type> relation_op_to_branch_map;

extern map<word_type, op_type> cal_op_to_mid_op;

extern map<string, string> const_string_record;

extern int const_string_count;

string insert_const_str(string & src);


template <typename T0>
bool set_contains(set<T0>& set_, T0& key)
{
    return set_.find(key) != set_.end();
}


template<typename T0>
bool set_contains(set<T0> &set_, T0 key)
{
    return set_.find(key) != set_.end();
}


template <typename T0, typename T1>
bool map_contains(map<T0, T1>& map_, T0& key)
{
    return map_.find(key) != map_.end();
}

template <typename T0, typename T1>
bool map_contains(map<T0, T1>& map_, T0 key)
{
    return map_.find(key) != map_.end();
}

bool is_num(string &str);

bool is_temp_var(string &str);

string temp_var_to_local_name(string &str);



#endif //COMPILER_CODE_GENERATE_UTILS_H
