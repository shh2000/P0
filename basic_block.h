//
// Created by AERO on 2019/12/2.
//

#ifndef DAG_BASIC_BLOCK_H
#define DAG_BASIC_BLOCK_H

#include <vector>
#include "mid_code.h"

using namespace std;


class basic_block
{
private:
    vector<mid_code> code_list;
    set<string>  local_var;
    set<string> temp_var;
    set<string> used_local_temp_var;
    set<string> assigned_local_temp_var;
    set<string> begin_label;
    set<basic_block*> pre_blocks;
    set<basic_block*> can_reach_blocks;
    void add_next_block(basic_block* next_block);
    void add_pre_block(basic_block* pre_block);
    // TODO: pre and next stream analysis
public:
    explicit basic_block(vector<mid_code> &src_code_list, set<string> &local_var_);
    vector<mid_code> & get_code_list_ref();
    bool dag_optimize();
    set<string> & get_temp_var();
    void rename(set<string> & to_rename);
    void statistic_use_assign_msg_of_local_temp_var();
    bool is_global_var(const string & var_name);
    const set<string> & get_assigned_local_temp_var();
    const set<string> & get_used_local_temp_var();
    const set<string> & get_begin_label();
    static void link_blocks(basic_block* pre, basic_block* next);
    const set<basic_block*> & get_pre_blocks();
    const set<basic_block*> & get_next_blocks();
    mid_code get_last_code_copy();
    void delete_last_code();
};


#endif //DAG_BASIC_BLOCK_H
