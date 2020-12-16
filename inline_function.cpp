//
// Created by AERO on 2019/11/30.
//

#include <sstream>
#include <iostream>
#include "inline_function.h"
#include "utils.h"

inline_function::inline_function(vector<string> &src_mid)
{
    assert(!src_mid.empty());
    for (auto & src_str: src_mid)
    {
        mid_code mid = line_to_mid_code(src_str);
        mid_code_list.emplace_back(mid);
        // cout << mid;
    }
    // cout << op_string_transfer(mid_code_list[0].get_op_type()) << endl;
    // cout << mid_code_list[0].get_op_type() << "  " << FUNC << endl;
    assert(mid_code_list[0].get_op_type() == FUNC);
    func_name = mid_code_list[0][0];
    for (auto & mid: mid_code_list)
    {
        // cout << mid;
        op_type type = mid.get_op_type();
        if (type == PARA)
        {
            para_list.emplace_back(mid[1]);
            local_name_set.insert(mid[1]);
        }
        else if (type == VAR)
        {
            local_name_set.insert(mid[1]);
        }
        else if (type == ARR)
        {
            local_name_set.insert(mid[2]);
        }
        else
        {
            for (auto & str: mid.get_attach_copy())
            {
                if (str[0] == '#')
                {
                    local_name_set.insert(str);
                }
            }
        }
        switch (type)
        {
            case RET:
            {
                count_ret++;
                re_void = mid.get_attach_copy().empty();
                break;
            }
            case CALL:
            {
                called_func.insert(mid[0]);
                break;
            }
            case BGE:
            case BGT:
            case BLE:
            case BLT:
            case BNE:
            case BEQ:
            {
                local_name_set.insert(mid[2]);
                break;
            }
            case LABEL:
            case GOTO:
            {
                local_name_set.insert(mid[0]);
                break;
            }
            default:
            {
                // pass
            }
        }

    }
}

bool inline_function::has_re_value()
{
    return !re_void;
}

string inline_function::get_func_name()
{
    return func_name;
}

int inline_function::get_func_call_num()
{
    return called_func.size();
}

bool inline_function::has_call_func()
{
    return !called_func.empty();
}

int inline_function::para_num()
{
    return para_list.size();
}

vector<string> inline_function::get_para_list()
{
    return para_list;
}

vector<mid_code> inline_function::get_embed_code(string &target_func_name, int in_target_time, vector<string> &arg_list,
                                                 bool has_target_var, string target_var_name)
{
    vector<mid_code> inline_code_list;
    string suffix = "_" + target_func_name + "_LK_" + func_name + "_" + to_string(in_target_time);
    vector<string> temp_attach;
    string re_label = "return_label" + suffix;
    int count_arg = 0;
    for (auto & mid: mid_code_list)
    {
        switch (mid.get_op_type())
        {
            case FUNC:
            {
                break;
            }
            case RET:
            {
                if (has_target_var)
                {
                    string src_var = mid[0] + (is_num(mid[0])? "": suffix);
                    inline_code_list.emplace_back(mid_code::assign_mid(target_var_name, src_var));
                }
                inline_code_list.emplace_back(mid_code::goto_mid(re_label));
                break;
            }
            case PARA:
            {
                string new_name = mid[1] + suffix;
                string value_str = arg_list[count_arg++];
                inline_code_list.emplace_back(mid_code::var_mid(
                        new_name, value_type_string_transfer(mid[0])));
                inline_code_list.emplace_back(mid_code::assign_mid(new_name, value_str));
                break;
            }
            default:
            {
                temp_attach.clear();
                for (auto & msg: mid.get_attach_copy())
                {
                    temp_attach.emplace_back(msg + (local_name_set.find(msg) != local_name_set.end()? suffix: ""));
                }
                inline_code_list.emplace_back(mid_code(mid.get_op_type(), temp_attach));
                break;
            }
        }
    }
    if (count_ret != 0)
    {
        inline_code_list.emplace_back(mid_code::label_mid(re_label));
    }
    return inline_code_list;
}

void inline_function::embed_inline_code(inline_function &to_embed)
{
    assert(to_embed.called_func.empty());
    if (called_func.find(to_embed.get_func_name()) == called_func.end())
    {
        return;
    }
    origin_mid_code = mid_code_list;
    mid_code_list.clear();
    for (int i = 0; i < origin_mid_code.size(); ++i)
    {
        // cout << origin_mid_code[i];
        if (
                origin_mid_code[i].get_op_type() == CALL
                && origin_mid_code[i][0] == to_embed.get_func_name()
                )
        {
            vector<string> var_push;
            for (int j = 0; j < to_embed.para_num(); ++j)
            {
                int index = j + i - to_embed.para_num();
                assert(origin_mid_code[index].get_op_type() == PUSH);
                var_push.emplace_back(origin_mid_code[index][0]);
            }
            // 由于我在每个函数末位加了个ret，所以当前必不是最后一位
            string target_var_name, origin_name;
            bool has_target_var = false;
            if (origin_mid_code[i + 1].get_op_type() == GET)
            {
                origin_name = origin_mid_code[i + 1][0];
                target_var_name = process_target_var_name(origin_name);
                has_target_var = true;
                rename(origin_name, target_var_name);
            }
            if (!map_contains(func_inline_count, to_embed.get_func_name()))
            {
                func_inline_count[to_embed.get_func_name()] = 0;
            }
            vector<mid_code> embed_code = to_embed.get_embed_code(
                    func_name,
                    func_inline_count[to_embed.get_func_name()]++,
                    var_push,
                    has_target_var,
                    target_var_name
                    );
            int T = var_push.size();
            while (T--)
            {
                mid_code_list.pop_back();
            }
            if (has_target_var)
            {
                mid_code_list.emplace_back(mid_code::var_mid(target_var_name, INT_TYPE_VALUE));
            }
            for (const auto& item: embed_code)
            {
                mid_code_list.emplace_back(item);
            }
            if (has_target_var)
            {
                i++;
            }
        }
        else
        {
            mid_code_list.emplace_back(origin_mid_code[i]);
        }
    }
}

ostringstream inline_function::get_code_string_stream()
{
    ostringstream buffer;
    for (auto & item: mid_code_list)
    {
        buffer << item;
    }
    return buffer;
}

string inline_function::process_target_var_name(const string &target_name)
{
    if (target_name[0] != '#')
    {
        return target_name;
    }
    string new_name = target_name.substr(1, target_name.size() - 1);
    new_name = "$rename_" + new_name;
    return new_name;
}

void inline_function::rename(const string &origin_name, const string &new_name)
{
    if (origin_name == new_name)
    {
        return;
    }
    for (auto & item : origin_mid_code)
    {
        for (int j = 0; j < item.get_attach_copy().size(); j++)
        {
            if (item[j] == origin_name)
            {
                item[j] = new_name;
                item.reset_string();
            }
        }
    }
    for (auto & item : mid_code_list)
    {
        for (int j = 0; j < item.get_attach_copy().size(); j++)
        {
            if (item[j] == origin_name)
            {
                item[j] = new_name;
                item.reset_string();
            }
        }
    }
}


