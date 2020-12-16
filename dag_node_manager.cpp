//
// Created by AERO on 2019/12/2.
//

#include <iostream>
#include <utility>
#include "dag_node_manager.h"

int dag_node_manager::dag_var_count = 0;

dag_node_manager::dag_node_manager(vector<mid_code> src_code_list, set<string> &src_local_var) :local_var{src_local_var}
{
    code_list = std::move(src_code_list);
    for (auto & item: code_list)
    {
        switch (item.get_op_type())
        {
            case VAR:
            {
                var_define_code.emplace_back(item);
                break;
            }
            case PARA:
            {
                para_define_code.emplace_back(item);
                break;
            }
            case LABEL:
            {
                label_code.emplace_back(item);
                break;
            }
            case FUNC:
            {
                func_define_code.emplace_back(item);
                break;
            }
            case ARR:
            {
                array_define_code.emplace_back(item);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    // cout << 1111111 << endl;
    get_var_be_assigned();

    // cout << 2222222 << endl;
    /*

    cout << "~~~~~~~~~~~~~~~~~~~~\n";

    for (auto & item: code_list)
    {
        cout << item;
    }
    cout << "@@@@@@@@@@@@@@@@@@@@\n";
    */
    build_dag_graph();
    // cout << 3333333 << endl;

    generate_new_code();
    /*
    for (auto & item: dag_node_list)
    {
        cout << item->get_code_copy();
        cout << "represent: " << item->get_represent() << endl;
        cout << "var: ";
        for (auto& fck: item->get_var_set())
        {
            cout << fck << " ";
        }
        cout << endl;
    }
    cout << "new code:!!!!" << endl;
    for (auto& item: new_code)
    {
        cout << item;
    }
    cout << "~~~~~~~~~~~~~~~~~~~~\n";
    */
    // cout << 4444444 << endl;

    for (auto item: dag_node_list)
    {
        delete item;
    }
}

void dag_node_manager::build_dag_graph()
{
    for (auto& item: code_list)
    {
        // cout << item;
        switch (item.get_op_type())
        {
            case PUSH:
            {
                ins_push(item);
                break;
            }
            case CALL:
            {
                ins_call(item);
                break;
            }
            case GET:
            {
                ins_get(item);
                break;
            }
            case RET:
            {
                ins_ret(item);
                break;
            }
            case ADD:
            case SUB:
            case MUL:
            case DIVE:
            {
                ins_calcu(item);
                break;
            }
            case BGE:
            case BGT:
            case BLE:
            case BLT:
            case BEQ:
            case BNE:
            {
                ins_branch(item);
                break;
            }
            case GOTO:
            {
                ins_goto(item);
                break;
            }
            case ARR_SET:
            {
                ins_arr_set(item);
                break;
            }
            case ARR_READ:
            {
                ins_arr_read(item);
                break;
            }
            case PRINTF:
            {
                ins_printf(item);
                break;
            }
            case SCANF:
            {
                ins_scanf(item);
                break;
            }
            case ASSIGN_TO:
            {
                ins_assign(item);
                break;
            }
            case NEXT_LINE:
            {
                ins_next_line(item);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}



dag_node *dag_node_manager::get_dag_node_by_name(const string &value_name)
{
    if (!map_contains(var_node_map, value_name))
    {
        return nullptr;
    }
    else
    {
        return var_node_map[value_name];
    }
}

void dag_node_manager::relate_node_var(dag_node *var_node, const string& var_name)
{
    var_node_map[var_name] = var_node;
    var_node->add_var(var_name);
}

void dag_node_manager::irrelate_node_var(dag_node *var_node, const string &var_name)
{
    var_node_map.erase(var_name);
    var_node->del_var(var_name);
}


void dag_node_manager::erase_var(const string &var_name)
{
    string temp = var_name;
    if (var_name == "c1")
    {
        cout << "c1 !!!!!!!!!!!!\n";
    }
    if (!map_contains(var_node_map, var_name))
    {
        return;
    }
    else
    {
        irrelate_node_var(var_node_map[var_name], var_name);
    }
}


dag_node *dag_node_manager::get_or_create_var_node(const string &var_name, value_type type)
{
    dag_node* ver_node = get_dag_node_by_name(var_name);
    // cout << "get or create " << var_name << endl;
    if (ver_node != nullptr)
    {
        /*
        cout << "find same as:\n";
        for (auto& item: ver_node->get_var_set())
        {
            cout << item << " ";
        }
        cout << endl;
        cout << "mid code is " << ver_node->get_code_copy();
         */
        return ver_node;
    }
    else
    {
        string temp = var_name;
        dag_node_list.emplace_back(
                new dag_node(mid_code::var_mid(temp, type),
                dag_node_list.size())
                );
        ver_node = dag_node_list.back();
        relate_node_var(ver_node, var_name);
        // cout << "create new mid code: " << ver_node->get_code_copy();
        return ver_node;
    }
}

void dag_node_manager::get_var_be_assigned()
{
    for (auto & item: code_list)
    {
        switch (item.get_op_type())
        {
            case ASSIGN_TO:
            case GET:
            case ADD:
            case SUB:
            case MUL:
            case DIVE:
            {
                if (!is_temp_var(item[0]))
                {
                    var_be_assigned.insert(item[0]);
                }
                break;
            }
            case ARR_READ:
            {
                if (!is_temp_var(item[2]))
                {
                    var_be_assigned.insert(item[2]);
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}


dag_node *dag_node_manager::get_common_ancester(const vector<dag_node *>& children_to_find, op_type type)
{
    if (children_to_find.empty())
    {
        return nullptr;
    }
    for (auto& item: children_to_find[0]->get_parents_reference())
    {
        bool judge = true;
        if (
                item->get_code_type() != type
                || !item->is_valid()
                )
        {
            continue;
        }
        for (int j = 0; j < children_to_find.size(); ++j)
        {
            judge &= (item->get_child_location(children_to_find[j]) == j);
        }
        if (judge)
        {
            return item;
        }
    }
    return nullptr;
}

string dag_node_manager::select_represent(set<string> var_set)
{
    if (var_set.empty())
    {
        return gen_dag_temp_var();
    }
    for (string item: var_set)
    {
        if (!is_temp_var(item) && !is_num(item) && !is_global_var(item))
        {
            return item;
        }
    }
    string temp = *var_set.begin();
    return is_global_var(temp)? gen_dag_temp_var(): temp;
}



void dag_node_manager::make_var_invalid_and_erase(const string & var_name)
{
    erase_var(var_name);
    for (auto& item: dag_node_list)
    {
        if (
                item->get_code_type() == VAR
                && item->get_code_copy()[1] == var_name
                )
        {
            dag_node::set_invalid(item);
        }
    }
}

void dag_node_manager::update_var(dag_node *node, string &var_name)
{
    erase_var(var_name);
    relate_node_var(node, var_name);
}


bool dag_node_manager::is_global_var(const string &var_name)
{
    // cout << var_name << endl;
    string temp = var_name;
    bool is_const = is_num(temp),
            is_local_var = set_contains(local_var, var_name),
            is_temp = is_temp_var(temp);
    return !is_const && !is_local_var && !is_temp;
}

void dag_node_manager::set_node_represent(dag_node *node_ptr)
{
    if (!node_ptr->get_represent().empty())
    {
        return;
    }

    if (node_ptr->get_code_type() == VAR)
    {
        node_ptr->set_represent(node_ptr->get_code_copy()[1]);
        return;
    }

    string tag = select_represent(node_ptr->get_var_set());
    /*
    cout << "DEBUG: SELECT in {" ;
    for (auto & item: node_ptr->get_var_set())
    {
        cout << item << " ";
    }
    cout << "} select " << tag << endl;
     */
    var_be_assigned.erase(tag);
    node_ptr->set_represent(tag);
    // cout << "DEBUG set represent: " << tag << endl;
}


void dag_node_manager::ins_push(mid_code &code)
{
    dag_node * parent;
    dag_node* ver_node = get_or_create_var_node(code[0], INT_TYPE_VALUE);
    dag_node_list.emplace_back(new dag_node(mid_code::push_mid(code[0]), dag_node_list.size()));
    parent = dag_node_list.back();
    parent->add_child(ver_node);
}

void dag_node_manager::ins_call(mid_code &code)
{
    dag_node_list.emplace_back(new dag_node(code, dag_node_list.size()));
    for (auto& item: dag_node_list)
    {
        if (item->get_code_type() == ARR_READ && is_global_var(item->get_code_copy()[0]))
        {
            dag_node::set_invalid(item);
        }
    }
    auto iter = var_node_map.begin();
    while(iter != var_node_map.end()) {
        if (is_global_var(iter->first))
        {
            // cout << "erase " << iter->first << endl;
            erase_var(iter->first);
            // cout << "erase_success";
        }
        iter++;
    }
}

void dag_node_manager::ins_get(mid_code &code)
{
    dag_node * var_node = dag_node_list.back();
    auto * parent = new dag_node(code, dag_node_list.size());
    parent->add_child(var_node);
    dag_node_list.emplace_back(parent);
    update_var(dag_node_list.back(), code[0]);
    if (is_global_var(code[0]))
    {
        dag_node_list.back()->add_pass_global_var(code[0]);
    }
}

void dag_node_manager::ins_ret(mid_code &code)
{
    dag_node * parent;
    if (code.get_attach_copy().size() <= 0)
    {
        dag_node_list.emplace_back(new dag_node(code, dag_node_list.size()));
        return;
    }
    dag_node* ver_node = get_or_create_var_node(code[0], INT_TYPE_VALUE);
    dag_node_list.emplace_back(new dag_node(mid_code::ret_mid(code[0]), dag_node_list.size()));
    parent = dag_node_list.back();
    parent->add_child(ver_node);
}

void dag_node_manager::ins_calcu(mid_code &code)
{
    dag_node *lchild = get_or_create_var_node(code[1], INT_TYPE_VALUE),
             *rchild = get_or_create_var_node(code[2], INT_TYPE_VALUE);
    vector<dag_node *> children_to_find{lchild, rchild};
    dag_node * cal_node = get_common_ancester(children_to_find, code.get_op_type());
    if (cal_node == nullptr)
    {
        auto *temp = new dag_node(code, dag_node_list.size());
        dag_node_list.emplace_back(temp);
        cal_node = dag_node_list.back();
        cal_node->set_children(children_to_find);
    }
    update_var(cal_node, code[0]);
    if (is_global_var(code[0]))
    {
        cal_node->add_pass_global_var(code[0]);
    }
}

void dag_node_manager::ins_branch(mid_code &code)
{
    dag_node *lchild = get_or_create_var_node(code[0], INT_TYPE_VALUE),
            *rchild = get_or_create_var_node(code[1], INT_TYPE_VALUE);
    // cout << "woc " << code[0] << " " << code[1] << endl;
    auto *branch_node = new dag_node(code, dag_node_list.size());
    dag_node_list.emplace_back(branch_node);
    dag_node * node_ptr = dag_node_list.back();
    // cout << lchild->get_code_copy() << rchild->get_code_copy();
    node_ptr->add_child(lchild);
    node_ptr->add_child(rchild);
    // cout << node_ptr->get_children_reference().size() << endl;
    // cout << node_ptr->get_children_reference()[0]->get_code_copy() << node_ptr->get_children_reference()[1]->get_code_copy();
    /*
    for (auto& item: node_ptr->get_children_reference())
    {
        cout << item->get_code_copy();
    }
    cout << "!!!!!!!!!!!!!!!!!!!\n";
     */
}

void dag_node_manager::ins_goto(mid_code &code)
{
    dag_node_list.emplace_back(new dag_node(code, dag_node_list.size()));
}

void dag_node_manager::ins_arr_set(mid_code &code)
{
    dag_node *child0 = get_or_create_var_node(code[0], INT_TYPE_VALUE), // arr_name
            *child1 = get_or_create_var_node(code[1], INT_TYPE_VALUE),  // index
            *child2 = get_or_create_var_node(code[2], INT_TYPE_VALUE);  // value

    auto * set_arr_node = new dag_node(code, dag_node_list.size());

    set_arr_node->add_child(child0);
    set_arr_node->add_child(child1);
    set_arr_node->add_child(child2);
    dag_node_list.emplace_back(set_arr_node);

    for (auto& item: dag_node_list)
    {
        if (
                item->get_code_type() == ARR_READ
                && item->get_code_copy()[0] == code[0]
                )
        {
            dag_node::set_invalid(item);
        }
    }
}

void dag_node_manager::ins_arr_read(mid_code &code)
{
    dag_node *child0 = get_or_create_var_node(code[0], INT_TYPE_VALUE), // arr_name
            *child1 = get_or_create_var_node(code[1], INT_TYPE_VALUE);  // index

    vector<dag_node*> children_to_find{child0, child1};
    dag_node * arr_read_node = get_common_ancester(children_to_find, ARR_READ);
    if (arr_read_node == nullptr)
    {
        auto *temp = new dag_node(code, dag_node_list.size());
        temp->set_children(children_to_find);
        dag_node_list.emplace_back(temp);
        arr_read_node = dag_node_list.back();
    }

    update_var(arr_read_node, code[2]);
    if (is_global_var(code[2]))
    {
        arr_read_node->add_pass_global_var(code[2]);
    }
}

void dag_node_manager::ins_printf(mid_code &code)
{
    dag_node* var_node = get_or_create_var_node(code[1], value_type_string_transfer(code[0]));
    auto *temp = new dag_node(code, dag_node_list.size());
    dag_node_list.emplace_back(temp);
    dag_node* printf_node = dag_node_list.back();
    printf_node->add_child(var_node);
}

void dag_node_manager::ins_scanf(mid_code &code)
{
    dag_node_list.emplace_back(
            new dag_node(code, dag_node_list.size())
            );

    erase_var(code[1]);
}

void dag_node_manager::ins_assign(mid_code &code)
{
    dag_node* var_node = get_or_create_var_node(code[1], INT_TYPE_VALUE);
    update_var(var_node, code[0]);
    if (is_global_var(code[0]))
    {
        var_node->add_pass_global_var(code[0]);
    }
}

void dag_node_manager::ins_next_line(mid_code &code)
{
    dag_node_list.emplace_back(new dag_node(code, dag_node_list.size()));
}

string dag_node_manager::gen_dag_temp_var()
{
    return "#dag_temp_var_" + to_string(dag_var_count++);
}

void dag_node_manager::assign_remained_var()
{
    for (string item: var_be_assigned)
    {
        if (is_global_var(item))
        {
            continue;
        }
        dag_node *node = get_dag_node_by_name(item);
        // DEBUG
        if (node == nullptr)
        {
            cout << " name is " << item << endl;
        }
        assert(node != nullptr);
        set_node_represent(node);
        string src = node->get_represent();
        new_code.emplace_back(mid_code::assign_mid(item, src));
    }
    var_be_assigned.clear();
    assign_global_var(dag_node_list.size());
}

void dag_node_manager::generate_new_code()
{
    /*
    if (!dag_node_list.empty())
    {
        cout << dag_node_list[0].get_code_copy();
    }
     */
    new_code.clear();
    for (int i = 0; i < dag_node_list.size(); i++)
    {
        dag_node * item = dag_node_list[i];
        // cout << item->get_code_copy();
        switch (item->get_code_type())
        {
            case PUSH:
            {
                gen_push(item);
                break;
            }
            case CALL:
            {
                // cout << "wcnmd" << endl;
                assign_global_var(i);
                // cout << "wdnmd" << endl;
                gen_call(item);
                break;
            }
            case GET:
            {
                gen_get(item);
                break;
            }
            case RET:
            {
                //  can optimize
                assign_remained_var();
                gen_ret(item);
                break;
            }
            case ADD:
            case SUB:
            case MUL:
            case DIVE:
            {
                gen_calcu(item);
                break;
            }
            case BGE:
            case BGT:
            case BLE:
            case BLT:
            case BEQ:
            case BNE:
            {
                // nice
                assign_remained_var();
                gen_branch(item);
                break;
            }
            case GOTO:
            {
                // nice
                assign_remained_var();
                gen_goto(item);
                break;
            }
            case ARR_SET:
            {
                gen_arr_set(item);
                break;
            }
            case ARR_READ:
            {
                gen_arr_read(item);
                break;
            }
            case PRINTF:
            {
                gen_printf(item);
                break;
            }
            case SCANF:
            {
                gen_scanf(item);
                break;
            }
            case NEXT_LINE:
            {
                gen_next_line(item);
                break;
            }
            case VAR:
            {
                gen_var(item);
            }
            default:
            {
                break;
            }
        }
    }

    assign_remained_var();
}

void dag_node_manager::gen_push(dag_node *node)
{
    string temp = node->get_children_reference()[0]->get_represent();
    new_code.emplace_back(mid_code::push_mid(temp));
}

void dag_node_manager::gen_call(dag_node *node)
{
    new_code.emplace_back(node->get_code_copy());
}

void dag_node_manager::gen_get(dag_node *node)
{
    set_node_represent(node);
    string temp = node->get_represent();
    new_code.emplace_back(mid_code::get_mid(temp));
}

void dag_node_manager::gen_ret(dag_node *node)
{
    if (node->get_children_reference().empty())
    {
        new_code.emplace_back(node->get_code_copy());
        return;
    }
    string temp = node->get_children_reference()[0]->get_represent();
    new_code.emplace_back(mid_code::ret_mid(temp));
}

void dag_node_manager::gen_calcu(dag_node *node)
{
    set_node_represent(node);
    string dst = node->get_represent(),
            src0 = node->get_children_reference()[0]->get_represent(),
            src1 = node->get_children_reference()[1]->get_represent();
    vector<string> attach{dst, src0, src1};
    new_code.emplace_back(mid_code::smart_gen_mid_code(node->get_code_type(), attach));
}

void dag_node_manager::gen_branch(dag_node *node)
{
    string src0 = node->get_children_reference()[0]->get_represent(),
            src1 = node->get_children_reference()[1]->get_represent(),
            target = node->get_code_copy()[2];
    /*
    cout << "debug BRANCH: " << "1\n" << node->get_children_reference()[0]->get_code_copy() << "2\n"
    << node->get_children_reference()[1]->get_code_copy() << "3\n" << node->get_code_copy() << "??????????????\n";\
    */
    // cout << node->get_children_reference()[0]->get_code_type() << "  " << node->get_children_reference()[1]->get_code_type() << endl;
    vector<string> attach{src0, src1, target};
    new_code.emplace_back(mid_code::smart_gen_mid_code(node->get_code_type(), attach));
}

void dag_node_manager::gen_goto(dag_node *node)
{
    new_code.emplace_back(node->get_code_copy());
}

void dag_node_manager::gen_arr_set(dag_node *node)
{
    string arr_name = node->get_children_reference()[0]->get_represent(),
            index = node->get_children_reference()[1]->get_represent(),
            var_src = node->get_children_reference()[2]->get_represent();
    vector<string> attach{arr_name, index, var_src};
    new_code.emplace_back(mid_code(ARR_SET, attach));
}

void dag_node_manager::gen_arr_read(dag_node *node)
{
    set_node_represent(node);
    string arr_name = node->get_children_reference()[0]->get_represent(),
            index = node->get_children_reference()[1]->get_represent(),
            target = node->get_represent();
    vector<string> attach{arr_name, index, target};
    new_code.emplace_back(mid_code(ARR_READ, attach));
}

void dag_node_manager::gen_printf(dag_node *node)
{
    string type = node->get_code_copy()[0],
            var = node->get_children_reference()[0]->get_represent();
    vector<string> attach{type, var};
    new_code.emplace_back(mid_code(PRINTF, attach));
}

void dag_node_manager::gen_scanf(dag_node *node)
{
    new_code.emplace_back(node->get_code_copy());
}

void dag_node_manager::gen_next_line(dag_node *node)
{
    new_code.emplace_back(node->get_code_copy());
}

void dag_node_manager::gen_var(dag_node *node)
{
    string value = node->get_code_copy()[1];
    if (is_num(value))
    {
        // cout <<"GEN: is num: " << value << endl;
        node->set_represent(value);
    }
    else
    {
        set_node_represent(node);
    }
}

vector<mid_code> dag_node_manager::get_optimize_result()
{
    vector<mid_code> result;
    for (auto& item: func_define_code)
    {
        result.emplace_back(item);
    }
    for (auto& item: para_define_code)
    {
        result.emplace_back(item);
    }
    for (auto& item: label_code)
    {
        result.emplace_back(item);
    }
    for (auto& item: var_define_code)
    {
        result.emplace_back(item);
    }
    for (auto& item: array_define_code)
    {
        result.emplace_back(item);
    }
    for (auto& item: new_code)
    {
        result.emplace_back(item);
    }
    return result;
}

void dag_node_manager::assign_global_var(int end_point)
{
    // cout << "call assign global " << endl;
    map<string, dag_node*> global_var_last_value;
    for (int i = 0; i < end_point && i < dag_node_list.size(); i++)
    {
        dag_node* temp = dag_node_list[i];
        for (const auto& var_pass: temp->get_pass_global_var_name())
        {
            global_var_last_value[var_pass] = temp;
        }
        temp->clear_pass_global_var();
    }
    auto iter = global_var_last_value.begin();
    while (iter != global_var_last_value.end())
    {
        string dst = iter->first, src = iter->second->get_represent();
        // cout << "assign global " << dst << endl;
        new_code.emplace_back(
                mid_code::assign_mid(dst, src)
                );
        iter++;
    }
}




/*
void dag_node_manager::gen_assign(dag_node *node)
{

}
 */








