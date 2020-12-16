//
// Created by AERO on 2019/12/8.
//

#ifndef DAG_SILLY_CNT_H
#define DAG_SILLY_CNT_H

#include <sstream>
#include "utils.h"

using namespace std;


enum Reg{
    V0, V1, A0, A1, A2, A3,
    T0, T1, T2, T3, T4, T5,
    T6, T7, S0, S1, S2, S3,
    S4, S5, S6, S7,
    T8, T9, K0, K1,
    AT, RA
};

class silly_cnt
{
private:
    vector<string> reg_string{
            "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
            "$t0", "$t1", "$t2", "$t3", "$t4", "$t5",
            "$t6", "$t7", "$s0", "$s1", "$s2", "$s3",
            "$s4", "$s5", "$s6", "$s7",
            "$t8", "$t9", "$k0", "$k1",
            "$at", "$ra"
    };

    set<Reg> static_allocate_set{
            V1, A1, A2, A3,
            T0, T1, T2, T3, T4, T5,
            T6, T7, S0, S1, S2, S3,
            S4, S5, S6, S7, T8, K0, K1, T9, RA
    };
    set<Reg> temp_reg_set{
            AT, A0, V0
    };
    int push_depth = 0;
    int stack_frame_size = 0;
    string func_name;
    vector<string> para_list;
    set<string> para_set;
    vector<mid_code> code_list;

    // 用于记录变量的寄存器分配情况，如果不在map中意味着该变量没有被分配到寄存器
    map<string, Reg> var_to_reg;
    map<Reg, string> reg_to_var;

    map<string, int> func_var_statistic;
    map<string, int> func_var_offset;
    map<string, int> global_var_offset;
    set<string> func_var;
    ostringstream output_buffer;
    void statistic_func_var();
    void pre_alloc_reg();
    void init_stack();
    bool is_func_var(string & var_name);
    bool is_global_var(string & var_name);
    void allocate_reg(string & name, Reg alloc_reg);
    bool has_reg(string &var_name);
    string get_reg(string & var_name);
    void save_all_reg();
    void output_ins(const string & a = "", const string & b = "", const string & c = "", const string & d = "");
    void output_debug(const string & msg);
    int get_offset(string & var_name);
    void load_var_to_reg(string & var_name, Reg reg);
    void recover_all_reg();
    void store_var_to_reg(string & var_name, Reg reg);
    Reg get_bundle_or_temp_reg(string &var_name, set<Reg> &to_allocate);
    Reg get_bundle_or_temp_reg_for_read(string &var_name, set<Reg> &to_allocate);
    Reg get_temp_reg(set<Reg> &to_allocate);
    bool is_temp_reg(Reg reg);
    void solve_push(mid_code code);
    void solve_call(mid_code code);
    void solve_get(mid_code code);
    void solve_ret(mid_code code);
    void solve_add(mid_code code);
    void solve_sub(mid_code code);
    void solve_mul(mid_code code);
    void solve_div(mid_code code);
    void solve_branch(mid_code code);
    void solve_goto(mid_code code);
    void solve_arr_set(mid_code code);
    void solve_arr_read(mid_code code);
    void solve_printf(mid_code code);
    void solve_scanf(mid_code code);
    void solve_label(mid_code code);
    void solve_assign_to(mid_code code);
    void solve_next_line(mid_code code);
public:
    silly_cnt(vector<mid_code> src_code_list, map<string, int> global_map, string func_name);
    string get_output_result();
    string reg_string_transfer(Reg reg_num);
    Reg reg_string_transfer(const string &reg_name);
    void generate_code();
};


#endif //DAG_SILLY_CNT_H
