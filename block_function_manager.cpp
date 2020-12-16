//
// Created by AERO on 2019/12/3.
//

#include <fstream>
#include <iostream>
#include "block_function_manager.h"

block_function_manager::block_function_manager(const string &input_file_name, const string &output_file_name)
{
    in_fn = input_file_name;
    ou_fn = output_file_name;
    vector<mid_code> src_mid_code;
    ifstream input_file(in_fn);
    string temp;
    while (getline(input_file, temp))
    {
        src_mid_code.emplace_back(line_to_mid_code(temp));
    }
    input_file.close();

    int index;
    for (
            index = 0;
            index < src_mid_code.size() && src_mid_code[index].get_op_type() != FUNC;
            ++index
            )
    {
        not_in_func_code.emplace_back(src_mid_code[index]);
    }
    vector<mid_code> func_src_code;
    func_src_code.emplace_back(src_mid_code[index++]);
    while (index < src_mid_code.size())
    {
        mid_code cur = src_mid_code[index];
        if (cur.get_op_type() == FUNC)
        {
            block_function temp_func(func_src_code);
            func_list.emplace_back(temp_func);
            func_src_code.clear();
            func_src_code.emplace_back(cur);
        }
        else
        {
            func_src_code.emplace_back(cur);
        }
        index++;
    }
    /*
    for (auto & item: func_src_code)
    {
        cout << item;
    }
    */
    block_function temp_func(func_src_code);
    func_list.emplace_back(temp_func);
}

void block_function_manager::debug_output(const string & debug_file_name)
{
    ofstream output_file(debug_file_name);
    for (auto& item: func_list)
    {
        item.debug_output(output_file);
    }
    output_file.close();
}

void block_function_manager::dag_optimize()
{
    for (auto & item: func_list)
    {
        item.dag_optimize();
    }
}

void block_function_manager::output()
{
    ofstream output_file(ou_fn);
    for (auto& item: not_in_func_code)
    {
        output_file << item;
    }
    for (auto& item: func_list)
    {
        item.output(output_file);
    }
    output_file.close();
}
