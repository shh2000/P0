//
// Created by AERO on 2019/11/17.
//


#include "generate.h"

generate::generate(const string &filename, ostringstream &targetCodeBuffer)
        : target_code_buffer(targetCodeBuffer)
{
    ref_gen(filename);
}

void generate::ref_gen(const string &file_name)
{
    map<string, int> global_var_offset;
    ifstream input_file(file_name);
    string src;
    vector<string> temp_store;
    string temp, head;
    target_code_buffer << ".data" << endl;
    int string_length = 0;
    mid_code code = mid_code::get_temp_mid();

    while (getline(input_file, src))
    {
        istringstream is(src);
        temp_store.clear();
        is >> head;
        op_type type = op_string_transfer(head);
        string ori_string;
        if (type == STR)
        {
            is >> temp;
            temp_store.emplace_back(temp);
            getline(is, ori_string);
            ori_string = ori_string.substr(
                    ori_string.find_first_of('\"') + 1,
                    ori_string.find_last_of('\"') - ori_string.find_first_of('\"') - 1);

            temp_store.emplace_back(ori_string);
        }
        else
        {
            while (is >> temp)
            {
                temp_store.emplace_back(temp);
            }
        }
        code = mid_code(type, temp_store);
        if (type != STR)
        {
            break;
        }
        // cout << code[1] << endl;
        target_code_buffer << code[0] << ":.asciiz " << "\"" << code[1] << "\"" << endl;
        string_length += code[1].size();
    }
    int global_offset = 0;
    string_length = string_length - (string_length % 4) + 4;
    target_code_buffer << "li $gp " << string_length + data_base_address << endl;
    if (code.get_op_type() == VAR)
    {
        global_var_offset[code[1]] = global_offset;
        global_offset += 4;
    }
    else if (code.get_op_type() == ARR)
    {
        int length = atoi(code[1].c_str());
        global_var_offset[code[2]] = global_offset;
        global_offset += (4 * length);
    }
    else if (code.get_op_type() == FUNC)
    {
        goto text;
    }

    while (getline(input_file, src))
    {
        istringstream is(src);
        temp_store.clear();
        is >> head;
        while (is >> temp)
        {
            temp_store.emplace_back(temp);
        }
        op_type type = op_string_transfer(head);
        code = mid_code(type, temp_store);
        if (type == VAR)
        {
            global_var_offset[code[1]] = global_offset;
            global_offset += 4;
        }
        else if (type == ARR)
        {
            int length = atoi(code[1].c_str());
            global_var_offset[code[2]] = global_offset;
            global_offset += (4 * length);
        }
        else
        {
            break;
        }
    }
    text:
    target_code_buffer << ".text" << endl;
    target_code_buffer << "jal main" << endl;
    target_code_buffer << "li $v0 10" << endl;
    target_code_buffer << "syscall" << endl;
    string cur_func_name = code[0];
    vector<string> func_mid_codes;
    cout << "global var offset\n";
    for (auto & item: global_var_offset)
    {
        cout << item.first << ": " << item.second << endl;
    }
    while (getline(input_file, src))
    {
        if (src.find("@func") != src.npos)
        {
            istringstream is(src);
            temp_store.clear();
            is >> head;
            while (is >> temp)
            {
                temp_store.emplace_back(temp);
            }
            op_type type = op_string_transfer(head);
            code = mid_code(type, temp_store);
            vector<mid_code> temp_code_list;
            for (auto & str: func_mid_codes)
            {
                temp_code_list.emplace_back(line_to_mid_code(str));
            }
            silly_cnt silly(temp_code_list, global_var_offset, cur_func_name);
            silly.generate_code();
            target_code_buffer << silly.get_output_result();
            func_mid_codes.clear();
            cur_func_name = code[0];
        }
        else
        {
            func_mid_codes.emplace_back(src);
        }
    }
    vector<mid_code> temp_code_list;
    for (auto & str: func_mid_codes)
    {
        temp_code_list.emplace_back(line_to_mid_code(str));
    }
    silly_cnt silly(temp_code_list, global_var_offset, cur_func_name);
    silly.generate_code();
    target_code_buffer << silly.get_output_result();
    input_file.close();
}

void generate::mem_gen(const string &filename)
{
    ifstream input_file(filename);
    string src;
    vector<string> temp_store;
    string temp, head;
    target_code_buffer << ".data" << endl;
    int string_length = 0;
    mid_code code = mid_code::get_temp_mid();

    while (getline(input_file, src))
    {
        istringstream is(src);
        temp_store.clear();
        is >> head;
        op_type type = op_string_transfer(head);
        string ori_string;
        if (type == STR)
        {
            is >> temp;
            temp_store.emplace_back(temp);
            getline(is, ori_string);
            ori_string = ori_string.substr(
                    ori_string.find_first_of('\"') + 1,
                    ori_string.find_last_of('\"') - ori_string.find_first_of('\"') - 1);

            temp_store.emplace_back(ori_string);
        }
        else
        {
            while (is >> temp)
            {
                temp_store.emplace_back(temp);
            }
        }
        code = mid_code(type, temp_store);
        if (type != STR)
        {
            break;
        }
        // cout << code[1] << endl;
        target_code_buffer << code[0] << ":.asciiz " << "\"" << code[1] << "\"" << endl;
        string_length += code[1].size();
    }
    int global_offset = 0;
    string_length = string_length - (string_length % 4) + 4;
    target_code_buffer << "li $gp " << string_length + data_base_address << endl;
    if (code.get_op_type() == VAR)
    {
        global_var_map[code[1]] = new mid_var(value_type_string_transfer(
                code[0]), 4, 1, global_offset, true);
        global_offset += 4;
    }
    else if (code.get_op_type() == ARR)
    {
        int length = atoi(code[1].c_str());
        global_var_map[code[2]] = new mid_var(value_type_string_transfer(
                code[0]), 4 * length, length, global_offset, true);
        global_offset += (4 * length);
    }
    else if (code.get_op_type() == FUNC)
    {
        goto text;
    }

    while (getline(input_file, src))
    {
        istringstream is(src);
        temp_store.clear();
        is >> head;
        while (is >> temp)
        {
            temp_store.emplace_back(temp);
        }
        op_type type = op_string_transfer(head);
        code = mid_code(type, temp_store);
        if (type == VAR)
        {
            global_var_map[code[1]] = new mid_var(value_type_string_transfer(
                    code[0]), 4, 1, global_offset, true);
            global_offset += 4;
        }
        else if (type == ARR)
        {
            int length = atoi(code[1].c_str());
            global_var_map[code[2]] = new mid_var(value_type_string_transfer(
                    code[0]), 4 * length, length, global_offset, true);
            global_offset += (4 * length);
        }
        else
        {
            break;
        }
    }
    text:
    target_code_buffer << ".text" << endl;
    target_code_buffer << "jal main" << endl;
    target_code_buffer << "li $v0 10" << endl;
    target_code_buffer << "syscall" << endl;
    string cur_func_name = code[0];
    vector<string> func_mid_codes;
    while (getline(input_file, src))
    {
        if (src.find("@func") != src.npos)
        {
            istringstream is(src);
            temp_store.clear();
            is >> head;
            while (is >> temp)
            {
                temp_store.emplace_back(temp);
            }
            op_type type = op_string_transfer(head);
            code = mid_code(type, temp_store);

            function func(cur_func_name, func_mid_codes);
            func.generate_target_codes();
            target_code_buffer << func.target_codes.str();
            func_mid_codes.clear();
            cur_func_name = code[0];
        }
        else
        {
            func_mid_codes.emplace_back(src);
        }
    }
    function func(cur_func_name, func_mid_codes);
    func.generate_target_codes();
    // cout << target_code_buffer.str();
    target_code_buffer << func.target_codes.str();
    // cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    // cout << target_code_buffer.str();
    input_file.close();
}
