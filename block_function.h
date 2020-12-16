//
// Created by AERO on 2019/12/3.
//

#ifndef DAG_BLOCK_FUNCTION_H
#define DAG_BLOCK_FUNCTION_H

#include <sstream>
#include "basic_block.h"

using namespace std;

class block_function
{
private:
    string func_name;
    vector<basic_block*> block_list;
    vector<mid_code> code_list;
    set<int> split_point;
    set<op_type> branch_op_set{
        BGE, BGT, BEQ,
        BLE, BLT, BNE
    };
    map<string, int> label_to_index;
    set<string> local_var;
    set<string> un_temp_var;
    set<string> useless_assign_target;
    set<string> used_local_temp_var;
    set<string> assigned_local_temp_var;
    vector<mid_code> optimized_code;
    vector<mid_code> before_delete_useless;
    map<string, basic_block*> label_to_block;
    string debug_msg;
    int get_label_split_point(const string & label_name);
    void split_basic_block();
    void get_un_temp_var();
    void rename_un_temp_var();
    void get_useless_assign_target();
    void generate_code();
    bool optimize();
    void internal_dag_optimize();
    void link_block();
    void delete_useless_blocks();
    void merge_blocks();
public:
    explicit block_function(vector<mid_code> src_code);
    void debug_output(ofstream &output_file);
    void dag_optimize();
    void output(ofstream &output_file);
};


#endif //DAG_BLOCK_FUNCTION_H
