//
// Created by AERO on 2019/11/30.
//

#ifndef SILLY_GENERATE_INLINE_FUNCTION_H
#define SILLY_GENERATE_INLINE_FUNCTION_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include "global_type.h"
#include "mid_code.h"

using namespace std;
class inline_function
{
private:
    string func_name;
    int count_func_call = 0;
    map<string, int> func_inline_count;
    set<string> called_func;
    bool re_void = false;
    vector<mid_code> mid_code_list;
    vector<string> para_list;
    set<string> local_name_set;
    int count_ret = 0;
    set<string> var_rename;
    vector<mid_code> origin_mid_code;

    static string process_target_var_name(const string & target_name);
    void rename(const string &origin_name, const string &new_name);

public:
    explicit inline_function(vector<string> & src_mid);
    bool has_re_value();
    string get_func_name();
    int get_func_call_num();
    bool has_call_func();
    int para_num();
    vector<string> get_para_list();
    vector<mid_code> get_embed_code(
            string &target_func_name,
            int in_target_time,
            vector<string> &arg_list,
            bool has_target_var,
            string target_var_name
            );
    void embed_inline_code(inline_function & to_embed);
    ostringstream get_code_string_stream();
};


#endif //SILLY_GENERATE_INLINE_FUNCTION_H
