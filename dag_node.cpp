//
// Created by AERO on 2019/12/2.
//

#include "dag_node.h"
#include <utility>
#include <iostream>

using namespace std;

dag_node::dag_node(mid_code srcCode, int id) : src_code(std::move(srcCode)), id(id)
{

}

bool dag_node::is_valid()
{
    return valid;
}

mid_code dag_node::get_code_copy()
{
    return src_code;
}

op_type dag_node::get_code_type()
{
    return src_code.get_op_type();
}

set<dag_node *> dag_node::get_parents_copy()
{
    return parents;
}

bool dag_node::disconnect_parent(dag_node *parent)
{
    if (!set_contains(parents, (dag_node*) parent))
    {
        return false;
    }
    parents.erase(parent);
    return true;
}

bool dag_node::disconnect_children()
{
    for (auto & item: children)
    {
        item->disconnect_parent(this);
    }
    return true;
}

bool dag_node::connect_to_parent(dag_node *parent)
{
    assert(is_valid());
    parents.insert(parent);
    return true;
}

void dag_node::set_invalid(dag_node *invalid_root)
{
    assert(invalid_root != nullptr);
    invalid_root->valid = false;
    /*
    set<dag_node*> visited;
    queue<dag_node*> que;
    que.push(invalid_root);
    dag_node * front = nullptr;
    while (!que.empty())
    {
        front  = que.front();
        que.pop();
        if (
                !set_contains(visited, (dag_node *)front)
                || !front->is_valid()
                )
        {
            continue;
        }
        visited.insert(front);
        front->valid = false;
        for (auto & item: front->parents)
        {
            if (
                    !set_contains(visited, item)
                    || !item->is_valid()
                    )
            {
                continue;
            }
            que.push(item);
        }
    }
     */
}

set<dag_node *> &dag_node::get_parents_reference()
{
    return parents;
}

bool dag_node::add_var(const string& var_name)
{
    var.insert(var_name);
    return true;
}

bool dag_node::del_var(const string &var_name)
{
    var.erase(var_name);
    return true;
}

void dag_node::set_children(vector<dag_node *> children_set)
{
    children = std::move(children_set);
    for (auto & child: children)
    {
        child->connect_to_parent(this);
    }
}

void dag_node::add_child(dag_node *new_child)
{
    children.emplace_back(new_child);
    new_child->connect_to_parent(this);
}

vector<dag_node *> dag_node::get_children_copy()
{
    return children;
}

vector<dag_node *> dag_node::get_children_reference()
{
    return children;
}

int dag_node::get_child_location(dag_node *child)
{
    for (int i = 0; i < children.size(); ++i)
    {
        if (children[i] == child)
        {
            return i;
        }
    }
    return -1;
}

string dag_node::get_represent()
{
    return represent;
}

void dag_node::set_represent(string & var_name)
{
    represent = var_name;
}

const set<string> &dag_node::get_var_set()
{
    return var;
}

void dag_node::add_pass_global_var(const string &var_name)
{
    // cout << "add global pass " << var_name << endl;
    global_var_pass.insert(var_name);
}

void dag_node::del_pass_global_var(const string &var_name)
{
    global_var_pass.erase(var_name);
}

void dag_node::clear_pass_global_var()
{
    global_var_pass.clear();
}

const set<string> &dag_node::get_pass_global_var_name()
{
    return global_var_pass;
}
