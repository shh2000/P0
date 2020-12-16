//
// Created by AERO on 2019/12/2.
//

#ifndef DAG_DAG_NODE_H
#define DAG_DAG_NODE_H


#include "mid_code.h"
#include "utils.h"
#include <queue>

using namespace std;

class dag_node
{
private:
    mid_code src_code;
    set<dag_node*> parents;
    set<string> var;
    vector<dag_node*> children;
    bool valid = true;
    string represent;
    set<string> global_var_pass;
public:
    explicit dag_node(mid_code srcCode, int id);
    const int id;
    bool is_valid();
    mid_code get_code_copy();
    op_type get_code_type();
    set<dag_node*> get_parents_copy();
    set<dag_node*> & get_parents_reference();
    bool add_var(const string &var_name);
    bool del_var(const string &var_name);
    bool disconnect_parent(dag_node* parent);
    bool disconnect_children();
    bool connect_to_parent(dag_node * parent);
    void set_children(vector<dag_node*> children_set);
    void add_child(dag_node* new_child);
    vector<dag_node*> get_children_copy();
    vector<dag_node*> get_children_reference();
    static void set_invalid(dag_node *invalid_root);
    int get_child_location(dag_node* child);
    string get_represent();
    void set_represent(string & var_name);
    const set<string> & get_var_set();
    void add_pass_global_var(const string & var_name);
    void del_pass_global_var(const string & var_name);
    void clear_pass_global_var();
    const set<string> & get_pass_global_var_name();
};


#endif //DAG_DAG_NODE_H
