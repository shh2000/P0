//
// Created by AERO on 2019/12/2.
//

#ifndef DAG_DAG_NODE_MANAGER_H
#define DAG_DAG_NODE_MANAGER_H

#include <set>
#include <map>
#include "utils.h"
#include "dag_node.h"

using namespace std;


class dag_node_manager
{
private:
    static int dag_var_count;
    vector<mid_code> code_list;
    set<string> & local_var;
    vector<dag_node*> dag_node_list;
    map<string, dag_node*> var_node_map;
    vector<mid_code> label_code;
    vector<mid_code> var_define_code;
    vector<mid_code> para_define_code;
    vector<mid_code> array_define_code;
    vector<mid_code> func_define_code;
    vector<mid_code> new_code;
    set<string> var_be_assigned;
    void build_dag_graph();
    void get_var_be_assigned();

    dag_node * get_dag_node_by_name(const string &value_name);

    void relate_node_var(dag_node* var_node,const string & var_name);
    void irrelate_node_var(dag_node* var_node, const string & var_name);
    void erase_var(const string & var_name);
    dag_node *get_or_create_var_node(const string &var_name, value_type type);
    void make_var_invalid_and_erase(const string & var_name);
    static dag_node *get_common_ancester(const vector<dag_node*> & children_to_find, op_type type);
    bool is_global_var(const string & var_name);

    void ins_push(mid_code & code);
    void ins_call(mid_code & code);
    void ins_get(mid_code & code);
    void ins_ret(mid_code & code);
    void ins_calcu(mid_code & code);
    void ins_branch(mid_code & code);
    void ins_goto(mid_code & code);
    void ins_arr_set(mid_code & code);
    void ins_arr_read(mid_code & code);
    void ins_printf(mid_code & code);
    void ins_scanf(mid_code & code);
    void ins_assign(mid_code & code);
    void ins_next_line(mid_code & code);
    static string gen_dag_temp_var();
    void generate_new_code();
    string select_represent(set<string> var_set);
    void set_node_represent(dag_node* node_ptr);
    void update_var(dag_node* node, string & var_name);
    void assign_remained_var();
    void assign_global_var(int end_point);

    void gen_push(dag_node * node);
    void gen_call(dag_node * node);
    void gen_get(dag_node * node);
    void gen_ret(dag_node * node);
    void gen_calcu(dag_node * node);
    void gen_branch(dag_node * node);
    void gen_goto(dag_node * node);
    void gen_arr_set(dag_node * node);
    void gen_arr_read(dag_node * node);
    void gen_printf(dag_node * node);
    void gen_scanf(dag_node * node);
    // void gen_assign(dag_node * node);
    void gen_next_line(dag_node * node);
    void gen_var(dag_node * node);
public:
    dag_node_manager(vector<mid_code> src_code_list, set<string> &src_local_var);
    vector<mid_code> get_optimize_result();
};


#endif //DAG_DAG_NODE_MANAGER_H
