//
// Created by AERO on 2019/11/17.
//

#include <sstream>
#include "function.h"

using namespace std;

map<string, mid_var*> global_var_map;

function::function(string func_name_, vector<string> src_mid)
{
    /*
    if (func_name_ == "main")
    {
        for (string mid_code: src_mid)
        {
            cout << mid_code << endl;
        }
        // cout << "wdnmd 23333333333333" << endl;
    }
     */

    func_name = func_name_;
    stack_frame_size = 0;
    push_depth = 0;
    vector<string> temp_store;
    for (const auto& src: src_mid)
    {
        istringstream is(src);
        temp_store.clear();
        string temp, head;
        is >> head;
        while (is >> temp)
        {
            temp_store.emplace_back(temp);
        }
        op_type type = op_string_transfer(head);
        mid_code mid(type, temp_store);
        int len = mid_code_list.size();
        mid_code_list.emplace_back(mid);
        assert(len != mid_code_list.size());
        if (type == PARA || type == VAR)
        {
            string var_name = temp_store[1];
            value_type vue_type = value_type_string_transfer(temp_store[0]);
            var_count++;
            var_map[var_name] = new mid_var(vue_type, 4, 1, stack_frame_size);
            stack_frame_size += 4;
        }
        else if (type == ARR)
        {
            string var_name = temp_store[2];
            value_type vue_type = value_type_string_transfer(temp_store[0]);
            int length = atoi(temp_store[1].c_str());
            var_count++;
            var_map[var_name] = new mid_var(vue_type, (4 * length), length, stack_frame_size);
            stack_frame_size += (4 * length);
        }
        for (auto factor: temp_store)
        {
            if (factor[0] == '#')
            {
                var_count++;
                var_map[factor] = new mid_var(INT_TYPE_VALUE, 4, 1, stack_frame_size);
                stack_frame_size += 4;
            }
        }
    }
    stack_frame_size += 4; // ra
}

mid_var * function::get_var(string &name)
{
    if (var_map.find(name) != var_map.end())
    {
        return var_map[name];
    }
    else
    {
        return global_var_map[name];
    }
}

void function::ins(string s0, string s1, string s2, string s3)
{
    target_codes << s0 << " " << s1 << " " << s2 << " " << s3 << endl;
}

void function::load_var_to_reg(string var_name, string reg_name)
{
    mid_var * to_load = get_var(var_name);
    int offset = to_load->offset;
    if (!to_load->is_global)
    {
        offset = -offset;
        ins("lw", reg_name, to_string(offset) + "($fp)");
    }
    else
    {
        ins("lw", reg_name, to_string(offset) + "($gp)");
    }
}


void function::generate_target_codes()
{
    ins(func_name + ":");
    ins("move", "$fp", "$sp");
    ins("addiu", "$sp", "$sp", to_string(-stack_frame_size));
    ins("sw", "$ra", to_string(-(stack_frame_size - 4)) + "($fp)");
    for (auto code: mid_code_list)
    {
        if (func_name == "main")
        {
            // cout << code;
        }
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
                solve_bge(code);
                break;
            case BGT:
                solve_bgt(code);
                break;
            case BLE:
                solve_ble(code);
                break;
            case BLT:
                solve_blt(code);
                break;
            case BNE:
                solve_bne(code);
                break;
            case BEQ:
                solve_beq(code);
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

void function::store_reg_to_var(string var_name, string reg_name)
{
    mid_var* to_load = get_var(var_name);
    if (to_load == nullptr)
    {
        cout << var_name << endl;
    }
    int offset = to_load->offset;
    if (!to_load->is_global)
    {
        offset = -offset;
        ins("sw", reg_name, to_string(offset) + "($fp)");
    }
    else
    {
        ins("sw", reg_name, to_string(offset) + "($gp)");
    }
}

void function::solve_push(mid_code code)
{
    string var_name = code[0];
    if (!is_num(var_name))
    {
        load_var_to_reg(var_name, "$t0");
        ins("sw", "$t0", to_string(push_depth) + "($sp)");
    }
    else
    {
        ins("li", "$at", var_name);
        ins("sw", "$at", to_string(push_depth) + "($sp)");
    }
    push_depth -= 4;
}

void function::solve_call(mid_code code)
{
    push_depth = 0;
    ins("move", "$fp", "$sp");
    ins("jal", code[0]);
    ins("addiu", "$fp", "$fp", to_string(stack_frame_size));
}

void function::solve_get(mid_code code)
{
    string var_name = code[0];
    store_reg_to_var(var_name, "$v0");
}

void function::solve_ret(mid_code code)
{
    if (code.factors_num() > 0)
    {
        string var_name = code[0];
        if (is_num(var_name))
        {
            ins("li", "$v0", var_name);
        }
        else
        {
            load_var_to_reg(var_name, "$v0");
        }
    }
    ins("lw", "$ra", to_string(-(stack_frame_size - 4)) + "($fp)");
    ins("move", "$sp", "$fp");
    ins("jr", "$ra");
}

void function::solve_add(mid_code code)
{
    string dst = code[0];
    string src0 = code[1];
    string src1 = code[2];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$at", to_string(atoi(src0.c_str()) + atoi(src1.c_str())));
        store_reg_to_var(dst, "$at");
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("addiu", "$t0", "$t0", src1);
        store_reg_to_var(dst, "$t0");
    }
    else if (is_num(src0) && !is_num(src1))
    {
        load_var_to_reg(src1, "$t0");
        ins("addiu", "$t0", "$t0", src0);
        store_reg_to_var(dst, "$t0");
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("addu", "$t0", "$t0", "$t1");
        store_reg_to_var(dst, "$t0");
    }
}

void function::solve_sub(mid_code code)
{
    string dst = code[0];
    string src0 = code[1];
    string src1 = code[2];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$at", to_string(atoi(src0.c_str()) - atoi(src1.c_str())));
        store_reg_to_var(dst, "$at");
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("subiu", "$t0", "$t0", src1);
        store_reg_to_var(dst, "$t0");
    }
    else if (is_num(src0) && !is_num(src1))
    {
        load_var_to_reg(src1, "$t0");
        ins("subiu", "$t0", "$t0", src0);
        ins("subu", "$t0", "$0", "$t0");
        store_reg_to_var(dst, "$t0");
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("subu", "$t0", "$t0", "$t1");
        store_reg_to_var(dst, "$t0");
    }
}

void function::solve_mul(mid_code code)
{
    string dst = code[0];
    string src0 = code[1];
    string src1 = code[2];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$at", to_string(atoi(src0.c_str()) * atoi(src1.c_str())));
        store_reg_to_var(dst, "$at");
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("mul", "$t0", "$t0", src1);
        store_reg_to_var(dst, "$t0");
    }
    else if (is_num(src0) && !is_num(src1))
    {
        load_var_to_reg(src1, "$t0");
        ins("mul", "$t0", "$t0", src0);
        store_reg_to_var(dst, "$t0");
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("mul", "$t0", "$t0", "$t1");
        store_reg_to_var(dst, "$t0");
    }
}

void function::solve_div(mid_code code)
{
    string dst = code[0];
    string src0 = code[1];
    string src1 = code[2];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$at", to_string(atoi(src0.c_str()) / atoi(src1.c_str())));
        store_reg_to_var(dst, "$at");
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("div", "$t0", "$t0", src1);
        store_reg_to_var(dst, "$t0");
    }
    else if (is_num(src0) && !is_num(src1))
    {
        load_var_to_reg(src1, "$t0");
        ins("li", "$t1", src0);
        ins("div", "$t1", "$t1", "$t0");
        store_reg_to_var(dst, "$t1");
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("div", "$t0", "$t0", "$t1");
        store_reg_to_var(dst, "$t0");
    }
}

void function::solve_bge(mid_code code)
{
    string dst = code[2];
    string src0 = code[0];
    string src1 = code[1];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$t0", src0);
        ins("li", "$t1", src1);
        ins("bge", "$t0", "$t1", dst);
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("li", "$t1", src1);
        ins("bge", "$t0", "$t1", dst);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        ins("li", "$t0", src0);
        load_var_to_reg(src1, "$t1");
        ins("bge", "$t0", "$t1", dst);
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("bge", "$t0", "$t1", dst);
    }
}

void function::solve_bgt(mid_code code)
{
    string dst = code[2];
    string src0 = code[0];
    string src1 = code[1];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$t0", src0);
        ins("li", "$t1", src1);
        ins("bgt", "$t0", "$t1", dst);
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("li", "$t1", src1);
        ins("bgt", "$t0", "$t1", dst);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        ins("li", "$t0", src0);
        load_var_to_reg(src1, "$t1");
        ins("bgt", "$t0", "$t1", dst);
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("bgt", "$t0", "$t1", dst);
    }
}

void function::solve_ble(mid_code code)
{
    string dst = code[2];
    string src0 = code[0];
    string src1 = code[1];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$t0", src0);
        ins("li", "$t1", src1);
        ins("ble", "$t0", "$t1", dst);
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("li", "$t1", src1);
        ins("ble", "$t0", "$t1", dst);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        ins("li", "$t0", src0);
        load_var_to_reg(src1, "$t1");
        ins("ble", "$t0", "$t1", dst);
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("ble", "$t0", "$t1", dst);
    }
}

void function::solve_blt(mid_code code)
{
    string dst = code[2];
    string src0 = code[0];
    string src1 = code[1];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$t0", src0);
        ins("li", "$t1", src1);
        ins("blt", "$t0", "$t1", dst);
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("li", "$t1", src1);
        ins("blt", "$t0", "$t1", dst);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        ins("li", "$t0", src0);
        load_var_to_reg(src1, "$t1");
        ins("blt", "$t0", "$t1", dst);
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("blt", "$t0", "$t1", dst);
    }
}

void function::solve_beq(mid_code code)
{
    string dst = code[2];
    string src0 = code[0];
    string src1 = code[1];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$t0", src0);
        ins("li", "$t1", src1);
        ins("beq", "$t0", "$t1", dst);
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("li", "$t1", src1);
        ins("beq", "$t0", "$t1", dst);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        ins("li", "$t0", src0);
        load_var_to_reg(src1, "$t1");
        ins("beq", "$t0", "$t1", dst);
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("beq", "$t0", "$t1", dst);
    }
}

void function::solve_bne(mid_code code)
{
    string dst = code[2];
    string src0 = code[0];
    string src1 = code[1];
    if (is_num(src0) && is_num(src1))
    {
        ins("li", "$t0", src0);
        ins("li", "$t1", src1);
        ins("bne", "$t0", "$t1", dst);
    }
    else if (!is_num(src0) && is_num(src1))
    {
        load_var_to_reg(src0, "$t0");
        ins("li", "$t1", src1);
        ins("bne", "$t0", "$t1", dst);
    }
    else if (is_num(src0) && !is_num(src1))
    {
        ins("li", "$t0", src0);
        load_var_to_reg(src1, "$t1");
        ins("bne", "$t0", "$t1", dst);
    }
    else
    {
        load_var_to_reg(src0, "$t0");
        load_var_to_reg(src1, "$t1");
        ins("bne", "$t0", "$t1", dst);
    }
}

void function::solve_goto(mid_code code)
{
    string dst = code[0];
    ins("j", dst);
}

void function::solve_arr_set(mid_code code)
{
    string  arr_name = code[0],
            index = code[1],
            value = code[2];
    mid_var* arr = get_var(arr_name);

    if (is_num(index) && is_num(value))
    {
        int index_offset = atoi(index.c_str()) * 4;
        int total_offset = index_offset + arr->offset;
        ins("li", "$t0", value);
        if (arr->is_global)
        {
            ins("sw", "$t0", to_string(total_offset) + "($gp)");
        }
        else
        {
            ins("sw", "$t0", to_string(-total_offset) + "($fp)");
        }
    }
    else if (is_num(index) && !is_num(value))
    {
        int index_offset = atoi(index.c_str()) * 4;
        int total_offset = index_offset + arr->offset;
        load_var_to_reg(value, "$t0");
        if (arr->is_global)
        {
            ins("sw", "$t0", to_string(total_offset) + "($gp)");
        }
        else
        {
            ins("sw", "$t0", to_string(-total_offset) + "($fp)");
        }
    }
    else if (!is_num(index) && is_num(value))
    {
        load_var_to_reg(index, "$t0");
        ins("sll", "$t0", "$t0", "2");
        ins("addiu", "$t0", "$t0", to_string(arr->offset));
        ins("li", "$t1", value);
        if (arr->is_global)
        {
            ins("addu", "$t0", "$gp", "$t0");
        }
        else
        {
            ins("subu", "$t0", "$fp", "$t0");
        }
        ins("sw", "$t1",  "0($t0)");
    }
    else if (!is_num(index) && !is_num(value))
    {
        load_var_to_reg(index, "$t0");
        ins("sll", "$t0", "$t0", "2");
        ins("addiu", "$t0", "$t0", to_string(arr->offset));
        load_var_to_reg(value, "$t1");
        if (arr->is_global)
        {
            ins("addu", "$t0", "$gp", "$t0");
        }
        else
        {
            ins("subu", "$t0", "$fp", "$t0");
        }
        ins("sw", "$t1",  "0($t0)");
    }
}

void function::solve_arr_read(mid_code code)
{
    string  arr_name = code[0],
            index = code[1],
            target = code[2];
    mid_var* arr = get_var(arr_name);

    if (is_num(index))
    {
        int index_offset = atoi(index.c_str()) * 4;
        int total_offset = index_offset + arr->offset;
        if (arr->is_global)
        {
            ins("lw", "$t0", to_string(total_offset) + "($gp)");
        }
        else
        {
            ins("lw", "$t0", to_string(-total_offset) + "($fp)");
        }
        store_reg_to_var(target, "$t0");
    }
    else
    {
        load_var_to_reg(index, "$t0");
        ins("sll", "$t0", "$t0", "2");
        ins("addiu", "$t0", "$t0", to_string(arr->offset));
        if (arr->is_global)
        {
            ins("addu", "$t0", "$gp", "$t0");
        }
        else
        {
            ins("subu", "$t0", "$fp", "$t0");
        }
        ins("lw", "$t1",  "0($t0)");
        store_reg_to_var(target, "$t1");
    }
}

void function::solve_printf(mid_code code)
{
    value_type type = value_type_string_transfer(code[0]);
    string content = code[1];
    if (type == INT_TYPE_VALUE)
    {
        ins("li", "$v0", "1");
        if (is_num(content))
        {
            ins("li", "$a0", content);
        }
        else
        {
            load_var_to_reg(content, "$a0");
        }
    }
    else if (type == CHAR_TYPE_VALUE)
    {
        ins("li", "$v0", "11");
        if (is_num(content))
        {
            ins("li", "$a0", content);
        }
        else
        {
            load_var_to_reg(content, "$a0");
        }
    }
    else
    {
        ins("li", "$v0", "4");
        ins("la", "$a0", content);
    }
    ins("syscall");
}

void function::solve_scanf(mid_code code)
{
    value_type type = value_type_string_transfer(code[0]);
    string target = code[1];
    if (type == INT_TYPE_VALUE)
    {
        ins("li", "$v0", "5");
    }
    else
    {
        ins("li", "$v0", "12");
    }
    ins("syscall");
    store_reg_to_var(target, "$v0");
}

void function::solve_label(mid_code code)
{
    ins(code[0] + ":");
}

void function::solve_assign_to(mid_code code)
{
    string target = code[0], value = code[1];
    if (is_num(value))
    {
        ins("li", "$t0", value);
        store_reg_to_var(target, "$t0");
    }
    else
    {
        load_var_to_reg(value, "$t0");
        store_reg_to_var(target, "$t0");
    }
}

void function::solve_next_line(mid_code code)
{
    ins("li", "$v0", "11");
    ins("li", "$a0", R"('\n')");
    ins("syscall");
}




