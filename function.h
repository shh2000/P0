//
// Created by AERO on 2019/11/17.
//

#ifndef SILLY_GENERATE_FUNCTION_H
#define SILLY_GENERATE_FUNCTION_H

#include <vector>
#include <iostream>
#include "mid_var.h"
#include "utils.h"
#include "mid_code.h"

using namespace std;

extern map<string, mid_var *> global_var_map;

class function
{
public:
    string func_name;
    int var_count;
    int stack_frame_size;
    vector<mid_code> mid_code_list;
    ostringstream target_codes;
    map<string, mid_var*> var_map;
    int push_depth;
    function(string func_name, vector<string> src_mid);
    mid_var * get_var(string & name);
    void ins(string s0 = "", string s1 = "", string s2 = "", string s3 = "");
    void generate_target_codes();
    void load_var_to_reg(string var_name, string reg_name);
    void store_reg_to_var(string var_name, string reg_name);
    void solve_push(mid_code code);
    void solve_call(mid_code code);
    void solve_get(mid_code code);
    void solve_ret(mid_code code);
    void solve_add(mid_code code);
    void solve_sub(mid_code code);
    void solve_mul(mid_code code);
    void solve_div(mid_code code);
    void solve_bge(mid_code code);
    void solve_bgt(mid_code code);
    void solve_ble(mid_code code);
    void solve_blt(mid_code code);
    void solve_beq(mid_code code);
    void solve_bne(mid_code code);
    void solve_goto(mid_code code);
    void solve_arr_set(mid_code code);
    void solve_arr_read(mid_code code);
    void solve_printf(mid_code code);
    void solve_scanf(mid_code code);
    void solve_label(mid_code code);
    void solve_assign_to(mid_code code);
    void solve_next_line(mid_code code);

};


#endif //SILLY_GENERATE_FUNCTION_H
