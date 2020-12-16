//
// Created by AERO on 2019/12/8.
//

#include "silly_cnt.h"

#include <utility>
#include <algorithm>
#include <iostream>


void silly_cnt::statistic_func_var()
{
    int weight = 1;
    for (auto & code: code_list)
    {
        switch (code.get_op_type())
        {
            case PUSH:
            case GET:
            {
                if (is_func_var(code[0]))
                {
                    func_var_statistic[code[0]] += weight;
                }
                break;
            }
            case RET:
            {
                if (code.get_attach_size() != 0)
                {
                    if (is_func_var(code[0]))
                    {
                        func_var_statistic[code[0]] += weight;
                    }
                }
                break;
            }
            case ADD:
            case SUB:
            case DIVE:
            case MUL:
            {
                for (auto & var: code.get_attach_copy())
                {
                    if (is_func_var(var))
                    {
                        func_var_statistic[var] += weight;
                    }
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
                if (is_func_var(code[0]))
                {
                    func_var_statistic[code[0]] += weight;
                }
                if (is_func_var(code[1]))
                {
                    func_var_statistic[code[1]] += weight;
                }
                if (code[2].find("loop_begin_") != code[2].npos)
                {
                    weight /= 10;
                }
                break;
            }
            case GOTO:
            {
                if (code[0].find("loop_judge_") != code[0].npos)
                {
                    weight *= 10;
                }
                break;
            }
            case ARR_SET:
            case ARR_READ:
            {
                if (is_func_var(code[1]))
                {
                    func_var_statistic[code[1]] += weight;
                }
                if (is_func_var(code[2]))
                {
                    func_var_statistic[code[2]] += weight;
                }
                break;
            }
            case SCANF:
            case PRINTF:
            {
                if (is_func_var(code[1]))
                {
                    func_var_statistic[code[1]] += weight;
                }
                break;
            }
            case ASSIGN_TO:
            {
                if (is_func_var(code[0]))
                {
                    func_var_statistic[code[0]] += weight;
                }
                if (is_func_var(code[1]))
                {
                    func_var_statistic[code[1]] += weight;
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

silly_cnt::silly_cnt(vector<mid_code> src_code_list, map<string, int> global_map, string func_name)
{
    this->func_name = func_name;
    code_list = std::move(src_code_list);
    global_var_offset = std::move(global_map);
    init_stack();
    for (auto &item: func_var)
    {
        func_var_statistic[item] = 0;
    }
    statistic_func_var();

    pre_alloc_reg();
}

void silly_cnt::init_stack()
{
    stack_frame_size = 0;
    vector<string> temp_store;
    for (auto & code: code_list)
    {
        if (code.get_op_type() == PARA)
        {
            string name = code[1];
            func_var.insert(code[1]);
            func_var_offset[code[1]] = stack_frame_size;
            stack_frame_size += 4;
            para_list.emplace_back(code[1]);
            para_set.insert(code[1]);
        }
    }

    for (auto & code: code_list)
    {
        if (code.get_op_type() == VAR)
        {
            string name = code[1];
            func_var.insert(code[1]);
            func_var_offset[code[1]] = stack_frame_size;
            stack_frame_size += 4;
        }
        else if (code.get_op_type() == ARR)
        {
            string arr_name = code[2];
            int length = atoi(code[1].c_str());

            // not sure
            func_var.insert(arr_name);

            func_var_offset[arr_name] = stack_frame_size;
            stack_frame_size += (4 * length);
        }
    }

    for (auto & code: code_list)
    {
        for (auto factor: code.get_attach_copy())
        {
            // cout << factor <<  "  " << is_temp_var(factor) << endl;
            if (is_temp_var(factor))
            {
                func_var.insert(factor);
                const string& temp = factor;
                if (map_contains(func_var_offset, temp))
                {
                    continue;
                }
                func_var_offset[temp] = stack_frame_size;
                stack_frame_size += 4;
            }
        }
    }
    stack_frame_size += 4; // ra
}

bool silly_cnt::is_func_var(string &var_name)
{
    const string & temp = var_name;
    return  set_contains(func_var, temp) && !is_num(var_name);
}

bool silly_cnt::is_global_var(string &var_name)
{
    const string & temp = var_name;
    return !is_func_var(var_name) && map_contains(global_var_offset, temp) && !is_num(var_name);
}

static int cmp(pair<string, int> a, pair<string, int> b)
{
    return a.second > b.second;
}

void silly_cnt::pre_alloc_reg()
{
    vector<pair<string, int>> statistic_record;
    auto iter = func_var_statistic.begin();
    while (iter != func_var_statistic.end())
    {
        statistic_record.emplace_back(pair<string, int>(iter->first, iter->second));
        iter++;
    }
    sort(statistic_record.begin(), statistic_record.end(), cmp);
    vector<Reg> reg_list;
    for (auto & item: static_allocate_set)
    {
        reg_list.emplace_back(item);
    }
    cout << func_name << endl;
    for (int i = 0; i < reg_list.size() && i < statistic_record.size(); ++i)
    {
        if (!is_global_var(statistic_record[i].first))
        {
            // 分配寄存器

            cout << "DEBUG allocate " <<
            reg_string_transfer(reg_list[i]) << " to " << statistic_record[i].first
            << ". the weight is " << statistic_record[i].second << endl;

            allocate_reg(statistic_record[i].first, reg_list[i]);
        }
    }
}

void silly_cnt::allocate_reg(string &name, Reg alloc_reg)
{
    var_to_reg[name] = alloc_reg;
    reg_to_var[alloc_reg] = name;
}

bool silly_cnt::has_reg(string &var_name)
{
    const string & temp = var_name;
    return map_contains(var_to_reg, temp);
}

string silly_cnt::get_reg(string &var_name)
{
    assert(has_reg(var_name));
    return reg_string_transfer(var_to_reg[var_name]);
}

void silly_cnt::save_all_reg()
{
    for (auto& item: var_to_reg)
    {
        string temp = item.first;
        int offset = -get_offset(temp);
        string reg_name = reg_string_transfer(item.second);
        output_ins("sw", reg_name, to_string(offset) + "($fp)" );
    }
}

int silly_cnt::get_offset(string &var_name)
{
    if (is_func_var(var_name))
    {
        return func_var_offset[var_name];
    }
    else
    {
        return global_var_offset[var_name];
    }
}

void silly_cnt::output_ins(const string & a, const string & b, const string & c, const string & d)
{
    output_buffer << a << " " << b << " " << c << " " << d << endl;
}

void silly_cnt::output_debug(const string & msg)
{
    output_buffer << "# " << msg << endl;
}

void silly_cnt::load_var_to_reg(string &var_name, Reg reg)
{
    int offset = get_offset(var_name);
    if (is_func_var(var_name))
    {
        offset = -offset;
        output_ins("lw", reg_string_transfer(reg), to_string(offset) + "($fp)");
    }
    else
    {
        output_ins("lw", reg_string_transfer(reg), to_string(offset) + "($gp)");
    }
}

void silly_cnt::recover_all_reg()
{
    for (auto& item: var_to_reg)
    {
        string temp = item.first;
        int offset = -get_offset(temp);
        string reg_name = reg_string_transfer(item.second);
        output_ins("lw", reg_name, to_string(offset) + "($fp)" );
    }
}

void silly_cnt::store_var_to_reg(string &var_name, Reg reg)
{
    int offset = get_offset(var_name);
    if (is_func_var(var_name))
    {
        offset = -offset;
        output_ins("sw", reg_string_transfer(reg), to_string(offset) + "($fp)");
    }
    else
    {
        output_ins("sw", reg_string_transfer(reg), to_string(offset) + "($gp)");
    }
}



void silly_cnt::generate_code()
{
    output_ins(func_name + ":");
    output_ins("move", "$fp", "$sp");
    output_ins("addiu", "$sp", "$sp", to_string(-stack_frame_size));
    output_ins("sw", "$ra", to_string(-(stack_frame_size - 4)) + "($fp)");

    for (auto & item: para_list)
    {
        if (has_reg(item))
        {
            load_var_to_reg(item, reg_string_transfer(get_reg(item)));
        }
    }

    for (auto code: code_list)
    {
        if (func_name == "main")
        {
            // cout << code;
        }
        output_debug(code.toString());
        switch (code.get_op_type())
        {
            case PUSH:
                solve_push(code);
                break;
            case CALL:
                solve_call(code);
                break;
            case GET:
                solve_get(code);
                break;
            case RET:
                solve_ret(code);
                break;
            case ADD:
                solve_add(code);
                break;
            case SUB:
                solve_sub(code);
                break;
            case MUL:
                solve_mul(code);
                break;
            case DIVE:
                solve_div(code);
                break;
            case BGE:
            case BGT:
            case BLE:
            case BLT:
            case BNE:
            case BEQ:
                solve_branch(code);
                break;
            case GOTO:
                solve_goto(code);
                break;
            case ARR_SET:
                solve_arr_set(code);
                break;
            case ARR_READ:
                solve_arr_read(code);
                break;
            case PRINTF:
                solve_printf(code);
                break;
            case SCANF:
                solve_scanf(code);
                break;
            case LABEL:
                solve_label(code);
                break;
            case ASSIGN_TO:
                solve_assign_to(code);
                break;
            case NEXT_LINE:
                solve_next_line(code);
                break;
            default:
                break;
        }
    }
}

void silly_cnt::solve_push(mid_code code)
{
    string var_name = code[0];
    string reg_name;
    if (!is_num(var_name))
    {
        if (has_reg(var_name))
        {
            reg_name = get_reg(var_name);
        }
        else
        {
            load_var_to_reg(var_name, A0);
            reg_name = reg_string_transfer(A0);
        }
        output_ins("sw", reg_name, to_string(push_depth) + "($sp)");
    }
    else
    {
        output_ins("li", "$a0", var_name);
        output_ins("sw", "$a0", to_string(push_depth) + "($sp)");
    }
    push_depth -= 4;
}

void silly_cnt::solve_call(mid_code code)
{
    push_depth = 0;
    save_all_reg();
    output_ins("move", "$fp", "$sp");
    output_ins("jal", code[0]);
    output_ins("addiu", "$fp", "$fp", to_string(stack_frame_size));
    recover_all_reg();
}

void silly_cnt::solve_get(mid_code code)
{
    string var_name = code[0];
    if (has_reg(var_name))
    {
        output_ins("move", get_reg(var_name), "$v0");
    }
    else
    {
        store_var_to_reg(var_name, V0);
    }
}

void silly_cnt::solve_ret(mid_code code)
{
    if (code.factors_num() > 0)
    {
        string var_name = code[0];
        if (is_num(var_name))
        {
            output_ins("li", "$v0", var_name);
        }
        else
        {
            if (has_reg(var_name))
            {
                output_ins("move", "$v0", get_reg(var_name));
            }
            else
            {
                load_var_to_reg(var_name, V0);
            }
        }
    }
    output_ins("lw", "$ra", to_string(-(stack_frame_size - 4)) + "($fp)");
    output_ins("move", "$sp", "$fp");
    output_ins("jr", "$ra");
}

void silly_cnt::solve_add(mid_code code)
{
    set<Reg> to_allocate = temp_reg_set;
    string src0 = code[1];
    string src1 = code[2];
    string dst = code[0];
    Reg reg_dst = get_bundle_or_temp_reg(dst, to_allocate);
    string reg_dst_str = reg_string_transfer(reg_dst);
    if (is_num(src0) && is_num(src1))
    {
        output_ins("li", reg_dst_str, to_string(atoi(src0.c_str()) + atoi(src1.c_str())));
    }
    else if (!is_num(src0) && is_num(src1))
    {
        string reg_src0 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src0, to_allocate));
        output_ins("addiu", reg_dst_str, reg_src0, src1);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        string reg_src1 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src1, to_allocate));
        output_ins("addiu", reg_dst_str, reg_src1, src0);
    }
    else
    {
        string reg_src0 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src0, to_allocate));
        string reg_src1 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src1, to_allocate));
        output_ins("addu", reg_dst_str, reg_src0, reg_src1);
    }
    if (is_temp_reg(reg_dst))
    {
        store_var_to_reg(dst, reg_dst);
    }
}

Reg silly_cnt::get_bundle_or_temp_reg(string &var_name, set<Reg> &to_allocate)
{
    if (has_reg(var_name))
    {
        return reg_string_transfer(get_reg(var_name));
    }
    else
    {
        string temp  = reg_string_transfer(*(to_allocate.begin()));
        to_allocate.erase(reg_string_transfer(temp));
        return reg_string_transfer(temp);
    }
}

Reg silly_cnt::get_bundle_or_temp_reg_for_read(string &var_name, set<Reg> &to_allocate)
{
    if (has_reg(var_name))
    {
        return reg_string_transfer(get_reg(var_name));
    }
    else
    {
        string temp  = reg_string_transfer(*(to_allocate.begin()));
        to_allocate.erase(reg_string_transfer(temp));
        load_var_to_reg(var_name, reg_string_transfer(temp));
        return reg_string_transfer(temp);
    }
}

Reg silly_cnt::get_temp_reg(set<Reg> &to_allocate)
{
    string temp  = reg_string_transfer(*(to_allocate.begin()));
    to_allocate.erase(reg_string_transfer(temp));
    return reg_string_transfer(temp);
}

bool silly_cnt::is_temp_reg(Reg reg)
{
    const Reg & temp = reg;
    return set_contains(temp_reg_set, temp);
}

void silly_cnt::solve_sub(mid_code code)
{
    set<Reg> to_allocate = temp_reg_set;
    string src0 = code[1];
    string src1 = code[2];
    string dst = code[0];
    Reg reg_dst = get_bundle_or_temp_reg(dst, to_allocate);
    string reg_dst_str = reg_string_transfer(reg_dst);
    if (is_num(src0) && is_num(src1))
    {
        output_ins("li", reg_dst_str, to_string(atoi(src0.c_str()) - atoi(src1.c_str())));
    }
    else if (!is_num(src0) && is_num(src1))
    {
        string reg_src0 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src0, to_allocate));
        output_ins("subiu", reg_dst_str, reg_src0, src1);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        string reg_src1 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src1, to_allocate));
        output_ins("subiu", reg_dst_str, reg_src1, src0);
        output_ins("subu", reg_dst_str, "$0", reg_dst_str);
    }
    else
    {
        string reg_src0 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src0, to_allocate));
        string reg_src1 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src1, to_allocate));
        output_ins("subu", reg_dst_str, reg_src0, reg_src1);
    }
    if (is_temp_reg(reg_dst))
    {
        store_var_to_reg(dst, reg_dst);
    }
}

void silly_cnt::solve_mul(mid_code code)
{
    set<Reg> to_allocate = temp_reg_set;
    string src0 = code[1];
    string src1 = code[2];
    string dst = code[0];
    Reg reg_dst = get_bundle_or_temp_reg(dst, to_allocate);
    string reg_dst_str = reg_string_transfer(reg_dst);
    if (is_num(src0) && is_num(src1))
    {
        output_ins("li", reg_dst_str, to_string(atoi(src0.c_str()) * atoi(src1.c_str())));
    }
    else if (!is_num(src0) && is_num(src1))
    {
        string reg_src0 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src0, to_allocate));
        output_ins("mul", reg_dst_str, reg_src0, src1);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        string reg_src1 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src1, to_allocate));
        output_ins("mul", reg_dst_str, reg_src1, src0);
    }
    else
    {
        string reg_src0 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src0, to_allocate));
        string reg_src1 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src1, to_allocate));
        output_ins("mul", reg_dst_str, reg_src0, reg_src1);
    }
    if (is_temp_reg(reg_dst))
    {
        store_var_to_reg(dst, reg_dst);
    }
}

void silly_cnt::solve_div(mid_code code)
{
    set<Reg> to_allocate = temp_reg_set;
    string src0 = code[1];
    string src1 = code[2];
    string dst = code[0];
    Reg reg_dst = get_bundle_or_temp_reg(dst, to_allocate);
    string reg_dst_str = reg_string_transfer(reg_dst);
    if (is_num(src0) && is_num(src1))
    {
        output_ins("li", reg_dst_str, to_string(atoi(src0.c_str()) / atoi(src1.c_str())));
    }
    else if (!is_num(src0) && is_num(src1))
    {
        string reg_src0 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src0, to_allocate));
        string temp_reg = reg_string_transfer(get_temp_reg(to_allocate));
        output_ins("li", temp_reg, src1);
        output_ins("div", reg_src0, temp_reg);
        output_ins("mflo", reg_dst_str);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        string reg_src1 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src1, to_allocate));
        string temp_reg = reg_string_transfer(get_temp_reg(to_allocate));
        output_ins("li", temp_reg, src0);
        output_ins("div", temp_reg, src1);
        output_ins("mflo", reg_dst_str);
    }
    else
    {
        string reg_src0 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src0, to_allocate));
        string reg_src1 = reg_string_transfer(get_bundle_or_temp_reg_for_read(src1, to_allocate));
        output_ins("div", reg_src0, reg_src1);
        output_ins("mflo", reg_dst_str);
    }
    if (is_temp_reg(reg_dst))
    {
        store_var_to_reg(dst, reg_dst);
    }
}

void silly_cnt::solve_branch(mid_code code)
{
    // TODO 结合mars做一点优化
    map<op_type, string> op_map{
            pair<op_type, string>(BGE, "bge"),
            pair<op_type, string>(BGT, "bgt"),
            pair<op_type, string>(BLE, "ble"),
            pair<op_type, string>(BLT, "blt"),
            pair<op_type, string>(BNE, "bne"),
            pair<op_type, string>(BEQ, "beq")
    };
    set<Reg> to_allocate = temp_reg_set;
    op_type type = code.get_op_type();
    string dst = code[2];
    string src0 = code[0];
    string src1 = code[1];
    string branch_str = op_map[type];
    if (is_num(src0) && is_num(src1))
    {
        if (mode_cmp(type, atoi(src0.c_str()), atoi(src1.c_str())))
        {
            output_ins("j", dst);
        }
    }
    else if (!is_num(src0) && is_num(src1))
    {
        Reg reg0 = get_bundle_or_temp_reg_for_read(src0, to_allocate);
        string reg_name0 = reg_string_transfer(reg0);
        output_ins(branch_str, reg_name0, src1, dst);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        // 试着反写？
        string temp_reg = reg_string_transfer(get_temp_reg(to_allocate));
        output_ins("li", temp_reg, src0);
        Reg reg1 = get_bundle_or_temp_reg_for_read(src1, to_allocate);
        string reg_name1 = reg_string_transfer(reg1);
        output_ins(branch_str, temp_reg, reg_name1, dst);
    }
    else
    {
        Reg reg0 = get_bundle_or_temp_reg_for_read(src0, to_allocate);
        string reg_name0 = reg_string_transfer(reg0);
        Reg reg1 = get_bundle_or_temp_reg_for_read(src1, to_allocate);
        string reg_name1 = reg_string_transfer(reg1);
        output_ins(branch_str, reg_name0, reg_name1, dst);
    }
}

void silly_cnt::solve_goto(mid_code code)
{
    output_ins("j", code[0]);
}

void silly_cnt::solve_arr_set(mid_code code)
{
    set<Reg> to_allocate = temp_reg_set;
    string  arr_name = code[0],
            index = code[1],
            value = code[2];
    int offset = get_offset(arr_name);

    if (is_num(index) && is_num(value))
    {
        int index_offset = atoi(index.c_str()) * 4;
        int total_offset = index_offset + offset;
        string temp_reg = reg_string_transfer(get_temp_reg(to_allocate));
        output_ins("li", temp_reg, value);
        if (is_func_var(arr_name))
        {
            output_ins("sw", temp_reg, to_string(-total_offset) + "($fp)");
        }
        else
        {
            output_ins("sw", temp_reg, to_string(total_offset) + "($gp)");
        }
    }
    else if (is_num(index) && !is_num(value))
    {
        int index_offset = atoi(index.c_str()) * 4;
        int total_offset = index_offset + offset;
        string value_reg = reg_string_transfer(get_bundle_or_temp_reg_for_read(value, to_allocate));
        if (is_func_var(arr_name))
        {
            output_ins("sw", value_reg, to_string(-total_offset) + "($fp)");
        }
        else
        {
            output_ins("sw", value_reg, to_string(total_offset) + "($gp)");
        }
    }
    else if (!is_num(index) && is_num(value))
    {
        string index_reg = reg_string_transfer(get_bundle_or_temp_reg_for_read(index, to_allocate));
        string value_reg = reg_string_transfer(get_temp_reg(to_allocate));
        string temp_reg = reg_string_transfer(get_temp_reg(to_allocate));
        output_ins("sll", temp_reg, index_reg, "2");
        // output_ins("addiu", temp_reg, temp_reg, to_string(offset));
        output_ins("li", value_reg, value);
        if (is_func_var(arr_name))
        {
            output_ins("subu", temp_reg, "$fp", temp_reg);
            output_ins("sw", value_reg,  to_string(-offset) + "(" + temp_reg + ")");
        }
        else
        {
            output_ins("addu", temp_reg, "$gp", temp_reg);
            output_ins("sw", value_reg,  to_string(offset) + "(" + temp_reg + ")");
        }
    }
    else if (!is_num(index) && !is_num(value))
    {
        string index_reg = reg_string_transfer(get_bundle_or_temp_reg_for_read(index, to_allocate));
        string value_reg = reg_string_transfer(get_bundle_or_temp_reg_for_read(value, to_allocate));
        string temp_reg = reg_string_transfer(get_temp_reg(to_allocate));
        output_ins("sll", temp_reg, index_reg, "2");
        // output_ins("addiu", temp_reg, temp_reg, to_string(offset));
        if (is_func_var(arr_name))
        {
            output_ins("subu", temp_reg, "$fp", temp_reg);
            output_ins("sw", value_reg,  to_string(-offset) + "(" + temp_reg + ")");
        }
        else
        {
            output_ins("addu", temp_reg, "$gp", temp_reg);
            output_ins("sw", value_reg,   to_string(offset) + "(" + temp_reg + ")");
        }
    }
}

void silly_cnt::solve_arr_read(mid_code code)
{
    set<Reg> to_allocate = temp_reg_set;
    string  arr_name = code[0],
            index = code[1],
            target = code[2];
    int offset = get_offset(arr_name);
    Reg reg_dst = get_bundle_or_temp_reg(target, to_allocate);
    string reg_dst_str = reg_string_transfer(reg_dst);
    if (is_num(index))
    {
        int index_offset = atoi(index.c_str()) * 4;
        int total_offset = index_offset + offset;
        if (is_func_var(arr_name))
        {
            output_ins("lw", reg_dst_str, to_string(-total_offset) + "($fp)");
        }
        else
        {
            output_ins("lw", reg_dst_str, to_string(total_offset) + "($gp)");
        }
    }
    else
    {
        string index_reg = reg_string_transfer(get_bundle_or_temp_reg_for_read(index, to_allocate));
        string temp_reg = reg_string_transfer(get_temp_reg(to_allocate));
        output_ins("sll", temp_reg, index_reg, "2");
        // output_ins("addiu", temp_reg, temp_reg, to_string(offset));
        if (is_func_var(arr_name))
        {
            output_ins("subu", temp_reg, "$fp", temp_reg);
            output_ins("lw", reg_dst_str,  to_string(-offset) + "(" + temp_reg + ")");
        }
        else
        {
            output_ins("addu", temp_reg, "$gp", temp_reg);
            output_ins("lw", reg_dst_str,  to_string(offset) + "(" + temp_reg + ")");
        }
    }
    if (is_temp_reg(reg_dst))
    {
        store_var_to_reg(target, reg_dst);
    }
}

void silly_cnt::solve_printf(mid_code code)
{
    value_type type = value_type_string_transfer(code[0]);
    string content = code[1];
    if (type == INT_TYPE_VALUE)
    {
        output_ins("li", "$v0", "1");
        if (is_num(content))
        {
            output_ins("li", "$a0", content);
        }
        else
        {
            if (has_reg(content))
            {
                output_ins("move", reg_string_transfer(A0), get_reg(content));
            }
            else
            {
                load_var_to_reg(content, A0);
            }
        }
    }
    else if (type == CHAR_TYPE_VALUE)
    {
        output_ins("li", "$v0", "11");
        if (is_num(content))
        {
            output_ins("li", "$a0", content);
        }
        else
        {
            if (has_reg(content))
            {
                output_ins("move", reg_string_transfer(A0), get_reg(content));
            }
            else
            {
                load_var_to_reg(content, A0);
            }
        }
    }
    else
    {
        output_ins("li", "$v0", "4");
        output_ins("la", "$a0", content);
    }
    output_ins("syscall");
}

void silly_cnt::solve_scanf(mid_code code)
{
    value_type type = value_type_string_transfer(code[0]);
    string target = code[1];
    if (type == INT_TYPE_VALUE)
    {
        output_ins("li", "$v0", "5");
    }
    else
    {
        output_ins("li", "$v0", "12");
    }
    output_ins("syscall");
    if (has_reg(target))
    {
        output_ins("move", get_reg(target), "$v0");
    }
    else
    {
        store_var_to_reg(target, V0);
    }
}

void silly_cnt::solve_label(mid_code code)
{
    output_ins(code[0] + ":");
}

void silly_cnt::solve_assign_to(mid_code code)
{
    if (!is_num(code[1]))
    {
        set<Reg> to_allocate = temp_reg_set;
        Reg dst = get_bundle_or_temp_reg(code[0], to_allocate);
        Reg src = get_bundle_or_temp_reg_for_read(code[1], to_allocate);
        output_ins("move", reg_string_transfer(dst), reg_string_transfer(src));
        if (is_temp_reg(dst))
        {
            store_var_to_reg(code[0], dst);
        }
    }
    else
    {
        set<Reg> to_allocate = temp_reg_set;
        Reg dst = get_bundle_or_temp_reg(code[0], to_allocate);
        output_ins("li", reg_string_transfer(dst), code[1]);
        if (is_temp_reg(dst))
        {
            store_var_to_reg(code[0], dst);
        }
    }
}

void silly_cnt::solve_next_line(mid_code code)
{
    output_ins("li", "$v0", "11");
    output_ins("li", "$a0", R"('\n')");
    output_ins("syscall");
}

string silly_cnt::get_output_result()
{
    // return "li li $v0 10\nsyscall";
    return output_buffer.str();
}

string silly_cnt::reg_string_transfer(Reg reg_num)
{
    return reg_string[(int)reg_num];
}

Reg silly_cnt::reg_string_transfer(const string &reg_name)
{
    for (int i = 0; i < reg_string.size(); i++)
    {
        if (reg_string[i] == reg_name)
        {
            return static_cast<Reg>(i);
        }
    }
    cout << "Unknow reg_name " << reg_name << endl;
    return V0;
}
