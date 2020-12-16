//
// Created by AERO on 2019/11/16.
//

#include <sstream>
#include "mid_code.h"
#include "utils.h"
#include "value_type_string_transfer.h"

map<string, int> scope_temp_var_count;
map<string, int> scope_if_count;
map<string, int> scope_loop_count;

map<op_type, word_type> smart_transfer_map{
        pair<op_type, word_type>(DIVE, DIV),
        pair<op_type, word_type>(ADD, PLUS),
        pair<op_type, word_type>(SUB, MINU),
        pair<op_type, word_type>(MUL, MULT),
};


op_type op_string_transfer(string & op_str)
{
    for (int i = 0; i < op_string.size(); ++i)
    {
        if (op_str == op_string[i])
        {
            return static_cast<op_type>(i);
        }
    }
    return static_cast<op_type>(-1);
}

string op_string_transfer(op_type type)
{
    return op_string[(int)type];
}


string value_type_string_transfer(value_type type)
{
    return value_type_string_map.find(type)->second;
}

value_type value_type_string_transfer(string & str)
{
    return string_value_type_map.find(str)->second;
}


int get_count_table_new_count(map<string, int>& count_table, string& scope)
{
    if (count_table.find(scope) == count_table.end())
    {
        count_table[scope] = 1;
        return 0;
    }
    else
    {
        return count_table[scope]++;
    }
}

string get_new_var_name(string & scope)
{
    return "#" + scope + "_" + to_string(get_count_table_new_count(scope_temp_var_count, scope));
}

label_pair get_new_if_label_pair(string & scope)
{
    int cnt = get_count_table_new_count(scope_if_count, scope);
    string else_label = "else_begin_" + to_string(cnt) + "_" + scope,
            over_label = "else_over_" + to_string(cnt) + "_" + scope;
    return label_pair(else_label, over_label);
}

label_pair get_new_loop_label_pair(string & scope)
{
    int cnt = get_count_table_new_count(scope_loop_count, scope);
    string loop_label = "loop_begin_" + to_string(cnt) + "_" + scope,
            over_label = "loop_judge_" + to_string(cnt) + "_" + scope;
    return label_pair(loop_label, over_label);
}

bool mode_cmp(op_type mod, int src0, int src1)
{
    switch (mod)
    {
        case BGE:
            return src0 >= src1;
        case BGT:
            return src0 > src1;
        case BLE:
            return src0 <= src1;
        case BLT:
            return src0 < src1;
        case BEQ:
            return src0 == src1;
        case BNE:
            return src0 != src1;
        default:
            return false;
    }
}

mid_code::mid_code(op_type type_, vector<string> &attach_msg, bool valid_)
{
    type = type_;
    factor_strings = attach_msg;
    string temp = op_string[type_];
    valid = valid_;
    for (auto& item: factor_strings)
    {
        temp += (" " + item);
    }
    temp += "\n";
    use_str_print = temp;
}

ostream &operator<<(ostream &os, const mid_code &mid)
{
    os << mid.use_str_print;
    return os;
}

mid_code line_to_mid_code(string &src_str)
{
    istringstream is(src_str);
    vector<string> temp_store;
    string temp, head;
    is >> head;
    while (is >> temp)
    {
        temp_store.emplace_back(temp);
    }
    op_type type = op_string_transfer(head);
    return mid_code(type, temp_store);
}

mid_code mid_code::func_def_mid(string &func_name)
{
    vector<string>temp{
            func_name
    };
    return mid_code(FUNC, temp);
}

mid_code mid_code::para_mid(string &para_name, value_type type_)
{
    vector<string>temp{
            value_type_string_map[type_],
            para_name,
    };
    return mid_code(PARA, temp);
}

mid_code mid_code::push_mid(string &value)
{
    vector<string>temp{
            value
    };
    return mid_code(PUSH, temp);
}

mid_code mid_code::push_mid(int value)
{
    string temp = to_string(value);
    return push_mid(temp);
}

mid_code mid_code::call_mid(string &func_name)
{
    vector<string>temp{
            func_name
    };
    return mid_code(CALL, temp);
}

mid_code mid_code::get_mid(string &var_name)
{
    vector<string>temp{
            var_name
    };
    return mid_code(GET, temp);
}

mid_code mid_code::ret_mid(string &value)
{
    vector<string>temp{
            value
    };
    return mid_code(RET, temp);
}

mid_code mid_code::ret_mid(int value)
{
    string temp = to_string(value);
    return ret_mid(temp);
}

mid_code mid_code::var_mid(string &var_name, value_type type_)
{
    vector<string>temp{
            value_type_string_map[type_],
            var_name
    };
    return mid_code(VAR, temp);
}

mid_code mid_code::arr_mid(string &arr_name, value_type type_, int length)
{
    vector<string>temp{
            value_type_string_map[type_],
            to_string(length),
            arr_name
    };
    return mid_code(ARR, temp);
}

mid_code mid_code::calculate_mid(word_type type, string &dst, string &src0, string &src1)
{
    if (is_num(src0) && is_num(src1))
    {

        return calculate_mid(type, dst, atoi(src0.c_str()), atoi(src1.c_str()));
    }
    else if (is_num(src0) && !is_num(src1))
    {
        return calculate_mid(type, dst, atoi(src0.c_str()), src1);
    }
    else if (!is_num(src0) && is_num(src1))
    {
        return calculate_mid(type, dst, src0, atoi(src1.c_str()));
    }
    // 特判简化
    op_type cal_type = cal_op_to_mid_op.find(type)->second;
    if (cal_type == SUB && src0 == src1)
    {
        return assign_mid(dst, 0);
    }
    if (cal_type == DIVE && src0 == src1)
    {
        return assign_mid(dst, 1);
    }
    vector<string> temp = {
            dst, src0, src1
    };
    return mid_code(cal_type, temp);
}

mid_code mid_code::calculate_mid(word_type type, string &dst, int src0, string &src1)
{
    // 特判简化
    op_type cal_type = cal_op_to_mid_op.find(type)->second;
    if (src0 == 0)
    {
        if (cal_type == ADD)
        {
            return assign_mid(dst, src1);
        }
        else if (cal_type == SUB)
        {
            vector<string> temp = {
                    dst, to_string(src0), src1
            };
            return mid_code(cal_type, temp);
        }
        else
        {
            return assign_mid(dst, 0);
        }
    }
    if (src0 == 1 && cal_type == MUL)
    {
        return assign_mid(dst, src1);
    }
    vector<string> temp = {
            dst, to_string(src0), src1
    };
    return mid_code(cal_type, temp);
}

mid_code mid_code::calculate_mid(word_type type, string &dst, string & src0, int src1)
{
    // 特判简化
    op_type cal_type = cal_op_to_mid_op.find(type)->second;
    if (src1 == 0)
    {
        if (cal_type == ADD || cal_type == SUB)
        {
            return assign_mid(dst, src0);
        }
        else
        {
            return assign_mid(dst, 0);
        }
    }
    if (src1 == 1)
    {
        if (cal_type == MUL || cal_type == DIVE)
        {
            return assign_mid(dst, src0);
        }
    }
    vector<string> temp = {
            dst, src0, to_string(src1)
    };
    return mid_code(cal_type, temp);
}

mid_code mid_code::calculate_mid(word_type type, string &dst, int src0, int src1)
{
    // 特判简化
    op_type cal_type = cal_op_to_mid_op.find(type)->second;
    switch (cal_type)
    {
        case ADD:
            return assign_mid(dst, src0 + src1);
        case SUB:
            return assign_mid(dst, src0 - src1);
        case MUL:
            return assign_mid(dst, src0 * src1);
        case DIVE:
            return assign_mid(dst, src1 == 0? 0: (src0 / src1));
        default:
            return assign_mid(dst, -2333333);
    }
}

mid_code mid_code::condition_mid(word_type relation_op, string &src0, string &src1, string &target_label, bool reverse)
{
    if (is_num(src0) && is_num(src1))
    {
        return condition_mid(relation_op, atoi(src0.c_str()), atoi(src1.c_str()), target_label, reverse);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        return condition_mid(relation_op, atoi(src0.c_str()), src1, target_label, reverse);
    }
    else if (!is_num(src0) && is_num(src1))
    {
        return condition_mid(relation_op, src0, atoi(src1.c_str()), target_label, reverse);
    }
    op_type branch_op = reverse? relation_op_reverse_map.find(relation_op)->second
            : relation_op_to_branch_map.find(relation_op)->second;
    vector<string> temp = {
            src0, src1, target_label
    };
    return mid_code(branch_op, temp);
}

mid_code mid_code::condition_mid(word_type relation_op, int src0, string &src1, string &target_label, bool reverse)
{
    op_type branch_op = reverse? relation_op_reverse_map.find(relation_op)->second:
            relation_op_to_branch_map.find(relation_op)->second;
    vector<string> temp = {
            to_string(src0), src1, target_label
    };
    return mid_code(branch_op, temp);
}

mid_code mid_code::condition_mid(word_type relation_op, string &src0, int src1, string &target_label, bool reverse)
{
    op_type branch_op = reverse? relation_op_reverse_map.find(relation_op)->second:
            relation_op_to_branch_map.find(relation_op)->second;
    vector<string> temp = {
            src0, to_string(src1), target_label
    };
    return mid_code(branch_op, temp);
}

mid_code mid_code::condition_mid(word_type relation_op, int src0, int src1, string &target_label, bool reverse)
{
    op_type branch_op = reverse? relation_op_reverse_map.find(relation_op)->second:
            relation_op_to_branch_map.find(relation_op)->second;
    bool result = mode_cmp(branch_op, src0, src1);
    vector<string> temp = {
            target_label
    };
    return mid_code(GOTO, temp, result);
}

mid_code mid_code::goto_mid(string &target_label)
{
    vector<string> temp = {
            target_label
    };
    return mid_code(GOTO, temp);
}

mid_code mid_code::arr_set_mid(string &arr_name, string &index, string &value)
{
    vector<string> temp = {
            arr_name,
            index,
            value
    };
    return mid_code(ARR_SET, temp);
}

mid_code mid_code::arr_read_mid(string &arr_name, string &index, string &target)
{
    vector<string> temp = {
            arr_name,
            index,
            target
    };
    return mid_code(ARR_READ, temp);
}

mid_code mid_code::printf_mid(value_type print_type, string &content)
{
    vector<string> temp = {
            value_type_string_map[print_type],
            content
    };
    return mid_code(PRINTF, temp);
}

mid_code mid_code::printf_mid(value_type print_type, int content)
{
    vector<string> temp = {
            value_type_string_map[print_type],
            to_string(content)
    };
    return mid_code(PRINTF, temp);
}

mid_code mid_code::scanf_mid(value_type input_type, string &target)
{
    vector<string>temp{
            value_type_string_transfer(input_type),
            target
    };
    return mid_code(SCANF, temp);
}

mid_code mid_code::exit_mid()
{
    vector<string>temp;
    return mid_code(EXIT, temp);
}

mid_code mid_code::label_mid(string &label_name)
{
    vector<string>temp{
            label_name
    };
    return mid_code(LABEL, temp);
}

mid_code mid_code::assign_mid(string &dst, string &src)
{
    vector<string>temp{
            dst, src
    };
    return mid_code(ASSIGN_TO, temp);
}

mid_code mid_code::assign_mid(string &dst, int value)
{
    vector<string>temp{
            dst, to_string(value)
    };
    return mid_code(ASSIGN_TO, temp);
}

mid_code mid_code::get_temp_mid()
{
    vector<string>temp;
    return mid_code(BGT, temp);
}

op_type mid_code::get_op_type()
{
    return type;
}

bool mid_code::is_valid()
{
    return valid;
}

mid_code mid_code::print_next_line_mid()
{
    vector<string> temp;
    return mid_code(NEXT_LINE, temp);
}

mid_code mid_code::declare_string_mid(string src_str, string label_name)
{
    vector<string> temp{
        label_name,
        "\"" + src_str + "\""
    };
    return mid_code(STR, temp);
}

string &mid_code::operator[](int index)
{
    return factor_strings[index];
}

int mid_code::factors_num()
{
    return factor_strings.size();
}

vector<string> mid_code::get_attach_copy()
{
    return factor_strings;
}

mid_code mid_code::smart_gen_mid_code(op_type type, vector<string> attachment, bool valid)
{
    switch (type)
    {
        case ADD:
        case SUB:
        case MUL:
        case DIVE:
        {
            return calculate_mid(smart_transfer_map[type], attachment[0], attachment[1], attachment[2]);
        }
        case BGE:
        case BGT:
        case BLE:
        case BLT:
        case BNE:
        case BEQ:
        {
            if (is_num(attachment[0]) && is_num(attachment[1]))
            {
                bool result = mode_cmp(
                        type,
                        atoi(attachment[0].c_str()),
                        atoi(attachment[1].c_str()));
                if (result)
                {
                    return goto_mid(attachment[2]);
                }
                else
                {
                    return mid_code(type, attachment);
                }
            }
        }
        default:
        {
            return mid_code(type, attachment);
        }
    }
}

void mid_code::reset_string()
{
    string temp = op_string[type];
    for (auto& item: factor_strings)
    {
        temp += (" " + item);
    }
    temp += "\n";
    use_str_print = temp;
}

int mid_code::get_attach_size()
{
    return factor_strings.size();
}

string mid_code::toString()
{
    return use_str_print;
}

string mid_code::AS_check_string()
{
    switch (type)
    {
        case GET:
        {
            return factor_strings[0] + " = " + "(RET)\n";
        }

        case ADD:
        {
            return factor_strings[0] + " = " + factor_strings[1] + " + " + factor_strings[2] + "\n";
        }
        case SUB:
        {
            return factor_strings[0] + " = " + factor_strings[1] + " - " + factor_strings[2] + "\n";
        }
        case MUL:
        {
            return factor_strings[0] + " = " + factor_strings[1] + " * " + factor_strings[2] + "\n";
        }
        case DIVE:
        {
            return factor_strings[0] + " = " + factor_strings[1] + " / " + factor_strings[2] + "\n";
        }

        case ASSIGN_TO:
        {
            return factor_strings[0] + " = " + factor_strings[1] + "\n";
        }

        case ARR_SET:
        {
            return factor_strings[0] + "[" + factor_strings[1] + "] = " + factor_strings[2] + "\n";
        }

        case ARR_READ:
        {
            return factor_strings[2] + " = " + factor_strings[0] + "[" + factor_strings[1] + "]\n";
        }

        default:
        {
            return use_str_print;
        }
    }
}

