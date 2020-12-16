//
// Created by AERO on 2019/11/30.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include "inline_optimization.h"
#include "utils.h"

inline_optimization::inline_optimization(const string &input_file_name, const string &output_file_name)
{
    this->input_file_name = input_file_name;
    this->output_file_name = output_file_name;
    vector<string> src_mid_code;
    ifstream input_file(input_file_name);
    string temp;
    while (getline(input_file, temp))
    {
        src_mid_code.emplace_back(temp);
    }
    input_file.close();

    int index;
    for (
            index = 0;
            index < src_mid_code.size() && src_mid_code[index].find(FUNC_HEAD) == src_mid_code[index].npos;
            ++index
            )
    {
        not_in_func_code.emplace_back(src_mid_code[index]);
    }
    vector<string> func_src_code;
    func_src_code.emplace_back(src_mid_code[index++]);
    // cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;
    // cout << src_mid_code[index - 1] << endl;
    while (index < src_mid_code.size())
    {
        string cur = src_mid_code[index];
        // cout << "cur:" << cur << endl;
        if (cur.find(FUNC_HEAD) != cur.npos)
        {
            inline_function temp_func(func_src_code);
            function_list.emplace_back(func_src_code);
            func_src_code.clear();
            func_src_code.emplace_back(cur);
        }
        else
        {
            func_src_code.emplace_back(cur);
        }
        index++;
    }
    inline_function temp_func(func_src_code);
    function_list.emplace_back(temp_func);
    for (auto & item: function_list)
    {
        // cout << item.get_func_name() << endl;
        name_to_func[item.get_func_name()] = &item;
    }
}

bool inline_optimization::optimize()
{
    ofstream optimized_code(output_file_name);
    func_to_embed.clear();
    for (auto & item: function_list)
    {
        if (item.get_func_call_num() == 0)
        {
            func_to_embed.insert(item.get_func_name());
        }
    }
    /*
    cout << "~~~~~~~~~~~~~~~~~" << endl;
    for (auto& item: func_to_embed)
    {
        cout << item << endl;
    }
    cout << "@@@@@@@@@@@@@@@@@" << endl;
    for (auto& item: function_list)
    {
        cout << item.get_func_name() << endl;
    }
    cout << "~~~~~~~~~~~~~~~~~" << endl;
     */
    if (func_to_embed.empty() || func_to_embed.size() == function_list.size())
    {
        // cout << " no need to inline" << endl;
        output_mid_code(optimized_code);
        return false;
    }
    for (auto & item: func_to_embed)
    {
        for (auto & func: function_list)
        {
            // cout << item << endl;
            inline_function & temp = *name_to_func[item];
            func.embed_inline_code(temp);
        }
    }
    output_mid_code(optimized_code);

    return true;
}

void inline_optimization::output_mid_code(ofstream &optimized_code)
{
    // cout << endl << "output mid code" << endl;
    for (auto & item: not_in_func_code)
    {
        // cout << item << endl;
        optimized_code << item << endl;
    }
    for (auto & item: function_list)
    {
        if (set_contains(func_to_embed, item.get_func_name()) && item.get_func_name() != "main")
        {
            continue;
        }
        // cout << item.get_func_name() << endl;
        // cout << item.get_code_string_stream().str();
        optimized_code << item.get_code_string_stream().str();
    }
    // cout << endl << "output mid code finish" << endl;
    optimized_code.close();
}

void inline_optimization::optimize_code(const string &input_file_name, const string &output_file_name)
{
    ifstream input_file(input_file_name);
    ofstream output_file(output_file_name);
    string temp;

    while (getline(input_file, temp))
    {
        output_file << temp << endl;
    }
    input_file.close();
    output_file.close();
    int i = 0;
    while (i < 20)
    {
        inline_optimization optimization(output_file_name, output_file_name);
        if (!optimization.optimize())
        {
            break;
        }
        i++;
    }
    if (i >= 20)
    {
        // cout << "INLINE OPTIMIZE RUN AT LIST 20 ROUNDS" << endl;
    }
}
