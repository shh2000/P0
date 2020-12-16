//
// Created by AERO on 2019/12/2.
//

#include <iostream>
#include "basic_block.h"
#include "dag_node_manager.h"

basic_block::basic_block(vector<mid_code> &src_code_list, set<string> &local_var_)
{
    local_var = local_var_;
    code_list = src_code_list;

    for (auto& item: code_list)
    {
        if (item.get_op_type() == LABEL)
        {
            begin_label.insert(item[0]);
        }
        else
        {
            break;
        }
    }

    for (auto& item: code_list)
    {
        for (int i = 0; i < item.get_attach_copy().size(); ++i)
        {
            if (is_temp_var(item[i]))
            {
                temp_var.insert(item[i]);
            }
        }
    }
}

bool basic_block::dag_optimize()
{
    const int origin_len = code_list.size();
    dag_node_manager dag(code_list, local_var);
    code_list = dag.get_optimize_result();
    statistic_use_assign_msg_of_local_temp_var();
    return code_list.size() < origin_len;
}

vector<mid_code> & basic_block::get_code_list_ref()
{
    return code_list;
}

set<string> & basic_block::get_temp_var()
{
    return temp_var;
}

void basic_block::rename(set<string> & to_rename)
{
    for (int index = 0; index < code_list.size(); index++)
    {

        for (int i = 0; i < code_list[index].get_attach_copy().size(); ++i)
        {
            const string temp = code_list[index][i];
            if (set_contains(to_rename, temp))
            {
                temp_var.erase(code_list[index][i]);
                code_list[index][i] = temp_var_to_local_name(code_list[index][i]);
                local_var.insert(code_list[index][i]);
                code_list.emplace_back(mid_code::var_mid(code_list[index][i], INT_TYPE_VALUE));
                code_list[index].reset_string();
                // cout << "DEBUG rename un temp var :" << code_list[index];
            }
        }
    }
}

void basic_block::statistic_use_assign_msg_of_local_temp_var()
{
    for (auto & code: code_list)
    {
        switch (code.get_op_type())
        {
            case ASSIGN_TO:
            {
                // cout << "DEBUG BB ASSIGN 0" << endl;
                if (!is_global_var(code[0]))
                {
                    assigned_local_temp_var.insert(code[0]);
                }
                // cout << "DEBUG BB ASSIGN 1" << endl;
                if (
                        !is_global_var(code[1])
                        && !is_num(code[1])
                        )
                {
                    used_local_temp_var.insert(code[1]);
                }
                break;
            }
            case ADD:
            case SUB:
            case MUL:
            case DIVE:
            {
                // cout << "DEBUG BB CALCU 0" << endl;
                if (!is_global_var(code[0]))
                {
                    assigned_local_temp_var.insert(code[0]);
                }
                // cout << "DEBUG BB CALCU 1" << endl;
                if (
                        !is_global_var(code[1])
                        && !is_num(code[1])
                        )
                {
                    used_local_temp_var.insert(code[1]);
                }
                // cout << "DEBUG BB CALCU 2" << endl;
                if (
                        !is_global_var(code[2])
                        && !is_num(code[2])
                        )
                {
                    used_local_temp_var.insert(code[2]);
                }
                break;
            }
            case PUSH:
            {
                // cout << "DEBUG BB PR 0" << endl;
                if (
                        !is_global_var(code[0])
                        && !is_num(code[0])
                        )
                {
                    used_local_temp_var.insert(code[0]);
                }
                break;
            }
            case RET:
            {
                if (code.get_attach_copy().empty())
                {
                    break;
                }
                else
                {
                    if (
                            !is_global_var(code[0])
                            && !is_num(code[0])
                            )
                    {
                        used_local_temp_var.insert(code[0]);
                    }
                }
                break;
            }
            case GET:
            {
                // cout << "DEBUG BB GET 0" << endl;
                if (!is_global_var(code[0]))
                {
                    assigned_local_temp_var.insert(code[0]);
                }
                break;
            }
            case BGE:
            case BGT:
            case BLE:
            case BLT:
            case BNE:
            case BEQ:
            {
                // cout << "DEBUG BB BRANCH 0" << endl;
                if (
                        !is_global_var(code[0])
                        && !is_num(code[0])
                        )
                {
                    used_local_temp_var.insert(code[0]);
                }
                // cout << "DEBUG BB BRANCH 1" << endl;
                if (
                        !is_global_var(code[1])
                        && !is_num(code[1])
                        )
                {
                    used_local_temp_var.insert(code[1]);
                }
                break;
            }
            case ARR_READ:
            {
                // cout << "DEBUG BB ARR_READ 0" << endl;
                if (
                        !is_global_var(code[1])
                        && !is_num(code[1])
                        )
                {
                    used_local_temp_var.insert(code[1]);
                }
                // cout << "DEBUG BB ARR_READ 1" << endl;
                if (
                        !is_global_var(code[2])
                        )
                {
                    assigned_local_temp_var.insert(code[2]);
                }
                break;
            }
            case ARR_SET:
            {
                // cout << "DEBUG BB ARR_SET 0" << endl;
                if (
                        !is_global_var(code[1])
                        && !is_num(code[1])
                        )
                {
                    used_local_temp_var.insert(code[1]);
                }
                // cout << "DEBUG BB ARR_SET 1" << endl;
                if (
                        !is_global_var(code[2])
                        && !is_num(code[2])
                        )
                {
                    used_local_temp_var.insert(code[2]);
                }
                break;
            }
            case PRINTF:
            {
                // cout << "DEBUG BB PRINTF 0" << endl;
                if (
                        !is_global_var(code[1])
                        && !is_num(code[1])
                        )
                {
                    used_local_temp_var.insert(code[1]);
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

bool basic_block::is_global_var(const string &var_name)
{
    // cout << "??? " << var_name << " ???\n";
    string temp = var_name;
    bool is_const = is_num(temp),
            is_local_var = set_contains(local_var, var_name),
            is_temp = is_temp_var(temp);
    return !is_const && !is_local_var && !is_temp;
}

const set<string> &basic_block::get_assigned_local_temp_var()
{
    return assigned_local_temp_var;
}

const set<string> &basic_block::get_used_local_temp_var()
{
    return used_local_temp_var;
}

const set<string> &basic_block::get_begin_label()
{
    return begin_label;
}

void basic_block::add_next_block(basic_block *next_block)
{
    can_reach_blocks.insert(next_block);
}

void basic_block::add_pre_block(basic_block *pre_block)
{
    pre_blocks.insert(pre_block);
}

void basic_block::link_blocks(basic_block *pre, basic_block *next)
{
    pre->add_next_block(next);
    next->add_pre_block(pre);
}

const set<basic_block *> &basic_block::get_pre_blocks()
{
    return pre_blocks;
}

const set<basic_block *> &basic_block::get_next_blocks()
{
    return can_reach_blocks;
}

mid_code basic_block::get_last_code_copy()
{
    return code_list.back();
}

void basic_block::delete_last_code()
{
    if (!code_list.empty())
    {
        code_list.pop_back();
    }
}

