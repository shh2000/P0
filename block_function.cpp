//
// Created by AERO on 2019/12/3.
//

#include "block_function.h"
#include "utils.h"

#include <utility>
#include <algorithm>
#include <queue>
#include <fstream>
#include <iostream>


block_function::block_function(vector<mid_code> src_code)
{
    code_list = std::move(src_code);
}

int block_function::get_label_split_point(const string &label_name)
{
    if (!map_contains(label_to_index, label_name))
    {
        return -1;
    }
    int label_index = label_to_index[label_name];
    while (label_index >= 0)
    {
        if (code_list[label_index].get_op_type() != LABEL)
        {
            break;
        }
        label_index--;
    }
    return label_index + 1;
}

void block_function::split_basic_block()
{
    for (int i = 0; i < code_list.size(); i++)
    {
        if (code_list[i].get_op_type() == LABEL)
        {
            label_to_index[code_list[i][0]] = i;
        }
    }
    // cout << 111111111111 << endl;
    for (int i = 0; i < code_list.size(); i++)
    {
        // cout << code_list[i] << endl;
        if (set_contains(branch_op_set, code_list[i].get_op_type()))
        {
            split_point.insert(i + 1);
            int label_index = get_label_split_point(code_list[i][2]);
            split_point.insert(label_index);
        }
        else if (code_list[i].get_op_type() == GOTO)
        {
            split_point.insert(i + 1);
            int label_index = get_label_split_point(code_list[i][0]);
            split_point.insert(label_index);
        }
        else if (code_list[i].get_op_type() == RET)
        {
            split_point.insert(i + 1);
        }
        else if (code_list[i].get_op_type() == FUNC)
        {
            func_name = code_list[i][0];
        }
        else if (code_list[i].get_op_type() == VAR || code_list[i].get_op_type() == PARA)
        {
            local_var.insert(code_list[i][1]);
        }
        else if (code_list[i].get_op_type() == ARR)
        {
            local_var.insert(code_list[i][2]);
        }
    }
    vector<int> point;
    for (auto& item: split_point)
    {
        point.emplace_back(item);
    }
    sort(point.begin(), point.end());
    queue<int> point_queue;
    for (auto& item: point)
    {
        point_queue.push(item);
    }
    int index = 0;
    while (!point_queue.empty())
    {
        vector<mid_code> temp;
        int end_index = point_queue.front();
        point_queue.pop();
        if (index < end_index)
        {
            while (index < end_index)
            {
                temp.emplace_back(code_list[index]);
                index++;
            }
            block_list.emplace_back(new basic_block(temp, local_var));
        }
    }
    if (index < code_list.size())
    {
        vector<mid_code> temp;
        while (index < code_list.size())
        {
            temp.emplace_back(code_list[index]);
            index++;
        }
        block_list.emplace_back(new basic_block(temp, local_var));
    }
}

void block_function::debug_output(ofstream &output_file)
{
    output_file << debug_msg;
}

void block_function::internal_dag_optimize()
{
    cout << "func " << func_name << endl;
    string wdnmd = "call_fibo";
    int block_cnt = 0;
    for (auto& item: block_list)
    {
        for (int i = 0; i < 10; ++i)
        {
            // cout << "~~~~~~~~~~~~~~~~~~~~~block " << block_cnt << " optimize " << i << "~~~~~~~~~~~~~~~~~" << endl;
            if (!item->dag_optimize())
            {
                break;
            }
        }
        block_cnt++;
    }
    get_useless_assign_target();
    generate_code();
}

void block_function::output(ofstream &output_file)
{
    // cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    for (auto & code: code_list)
    {
        output_file << code;
        // cout << code;
    }
    // cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}

void block_function::get_un_temp_var()
{
    for (int i = 0; i < block_list.size(); ++i)
    {
        for (int j = i + 1; j < block_list.size(); ++j)
        {
            set<string> & set0 = block_list[i]->get_temp_var();
            set<string> & set1 = block_list[j]->get_temp_var();
            for (auto& var: set0)
            {
                if (set_contains(set1, var))
                {
                    un_temp_var.insert(var);
                }
            }
        }
    }
}

void block_function::rename_un_temp_var()
{
    for (auto& block: block_list)
    {
        block->rename(un_temp_var);
    }
}

void block_function::get_useless_assign_target()
{
    assigned_local_temp_var.clear();
    used_local_temp_var.clear();
    useless_assign_target.clear();
    for (auto & block: block_list)
    {
        for (auto& item: block->get_assigned_local_temp_var())
        {
            assigned_local_temp_var.insert(item);
        }
        for (auto& item: block->get_used_local_temp_var())
        {
            used_local_temp_var.insert(item);
        }
    }
    for (auto& item: assigned_local_temp_var)
    {
        if (!set_contains(used_local_temp_var, item))
        {
            // cout << "DEBUG " << item << " is useless" << endl;
            useless_assign_target.insert(item);
        }
    }
}

void block_function::generate_code()
{
    optimized_code.clear();
    set<string> & set_ref = useless_assign_target;
    ostringstream debug_msg_buffer;
    int block_count = 0;
    debug_msg_buffer << "@@@@@@@@@ not delete use less @@@@@@@@@@@" << endl;
    for (auto & block: block_list)
    {
        debug_msg_buffer << "~~~~~~~ block " << block_count << " begin ~~~~~~~" << endl;
        for (auto & code: block->get_code_list_ref())
        {
            before_delete_useless.emplace_back(code);
            debug_msg_buffer << code;
        }
        debug_msg_buffer << "~~~~~~~ block " << block_count++ << " end ~~~~~~~" << endl;
    }
    debug_msg_buffer << "@@@@@@@@@    delete use less    @@@@@@@@@@@" << endl;

    for (auto & block: block_list)
    {
        debug_msg_buffer << "~~~~~~~ block " << block_count << " begin ~~~~~~~" << endl;
        for (auto & code: block->get_code_list_ref())
        {
            switch (code.get_op_type())
            {
                case ASSIGN_TO:
                {
                    const string temp = code[0];
                    if (!set_contains(set_ref, temp))
                    {
                        debug_msg_buffer << code;
                        optimized_code.emplace_back(code);
                    }
                    break;
                }
                case ADD:
                case SUB:
                case MUL:
                case DIVE:
                {
                    const string temp = code[0];
                    if (!set_contains(set_ref, temp))
                    {
                        debug_msg_buffer << code;
                        optimized_code.emplace_back(code);
                    }
                    break;
                }
                case GET:
                {
                    const string temp = code[0];
                    if (!set_contains(set_ref, temp))
                    {
                        debug_msg_buffer << code;
                        optimized_code.emplace_back(code);
                    }
                    break;
                }
                case ARR_READ:
                {
                    const string temp = code[2];
                    if (!set_contains(set_ref, temp))
                    {
                        debug_msg_buffer << code;
                        optimized_code.emplace_back(code);
                    }
                    break;
                }
                default:
                {
                    debug_msg_buffer << code;
                    optimized_code.emplace_back(code);
                    break;
                }
            }
        }
        debug_msg_buffer << "~~~~~~~ block " << block_count++ << " end ~~~~~~~" << endl;
    }
    debug_msg_buffer << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    debug_msg_buffer << "origin size: " << code_list.size() << endl;
    debug_msg_buffer << "before delete useless size: " << before_delete_useless.size() << endl;
    debug_msg_buffer << "after delete useless size: " << optimized_code.size() << endl;

    if (code_list.size() > before_delete_useless.size())
    {
        code_list = (before_delete_useless.size() < optimized_code.size())? before_delete_useless: optimized_code;
    }
    else if (code_list.size() > optimized_code.size())
    {
        code_list = (before_delete_useless.size() < optimized_code.size())? before_delete_useless: optimized_code;
    }
    debug_msg_buffer << "finally code list size is " << code_list.size() << endl;
    debug_msg = debug_msg_buffer.str();
}

bool block_function::optimize()
{
    for (auto & block_ptr: block_list)
    {
        delete block_ptr;
    }
    block_list.clear();
    split_point.clear();
    label_to_index.clear();
    un_temp_var.clear();
    useless_assign_target.clear();
    used_local_temp_var.clear();
    assigned_local_temp_var.clear();
    optimized_code.clear();
    before_delete_useless.clear();
    debug_msg = "";
    int len = code_list.size();
    split_basic_block();

    link_block();
    delete_useless_blocks();
    merge_blocks();

    get_un_temp_var();
    rename_un_temp_var();
    internal_dag_optimize();
    return code_list.size() < len;
}

void block_function::dag_optimize()
{
    int count = 0;
    bool judge;
    do {
        // cout << "func: " << func_name << " " << count++ << " times optimize" << endl;
        judge = optimize();
        // cout << debug_msg;
    } while (judge);
}

void block_function::link_block()
{
    // label relate block
    for (auto block_ptr: block_list)
    {
        for (auto & label: block_ptr->get_begin_label())
        {
            label_to_block[label] = block_ptr;
        }
    }

    for (int i = 0; i < block_list.size(); i++)
    {
        basic_block* block_ptr = block_list[i];
        mid_code last_code = block_ptr->get_last_code_copy();
        switch (last_code.get_op_type())
        {
            case BGE:
            case BGT:
            case BLE:
            case BLT:
            case BEQ:
            case BNE:
            {
                basic_block::link_blocks(block_ptr, label_to_block[last_code[2]]);
                if (i < (block_list.size() - 1))
                {
                    basic_block::link_blocks(block_ptr, block_list[i + 1]);
                }
                break;
            }
            case GOTO:
            {
                basic_block::link_blocks(block_ptr, label_to_block[last_code[0]]);
                break;
            }
            case RET:
            {
                break;
            }
            default:
            {
                if (i < (block_list.size() - 1))
                {
                    basic_block::link_blocks(block_ptr, block_list[i + 1]);
                }
                break;
            }
        }
    }
}

void block_function::delete_useless_blocks()
{
    vector<basic_block*> new_block_list;
    if (!block_list.empty())
    {
        new_block_list.emplace_back(block_list[0]);
    }
    for (int i = 1; i < block_list.size(); ++i)
    {
        if (!block_list[i]->get_pre_blocks().empty())
        {
            new_block_list.emplace_back(block_list[i]);
        }
        else
        {
            delete block_list[i];
        }
    }
    block_list = new_block_list;
}

void block_function::merge_blocks()
{
    vector<mid_code> new_code_list;
    for (int i = 0; i < (block_list.size() - 1); ++i)
    {
        mid_code last_code = block_list[i]->get_last_code_copy();

        switch (last_code.get_op_type())
        {
            case BGE:
            case BGT:
            case BLE:
            case BLT:
            case BNE:
            case BEQ:
            {
                if (label_to_block[last_code[2]] == block_list[i + 1])
                {
                    block_list[i]->delete_last_code();
                }
                break;
            }
            case GOTO:
            {
                if (label_to_block[last_code[0]] == block_list[i + 1])
                {
                    block_list[i]->delete_last_code();
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
    code_list.clear();
    for (auto block_ptr: block_list)
    {
        for (auto & code: block_ptr->get_code_list_ref())
        {
            code_list.emplace_back(code);
        }
    }
    local_var.clear();
    label_to_index.clear();
    split_point.clear();
    block_list.clear();
    split_basic_block();
}

