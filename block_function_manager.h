//
// Created by AERO on 2019/12/3.
//

#ifndef DAG_BLOCK_FUNCTION_MANAGER_H
#define DAG_BLOCK_FUNCTION_MANAGER_H

#include "block_function.h"

using namespace std;


class block_function_manager
{
private:
    vector<block_function> func_list;
    string in_fn, ou_fn;
    vector<mid_code> not_in_func_code;
public:
    block_function_manager(const string & input_file_name, const string & output_file_name);
    void debug_output(const string & debug_file_name);
    void dag_optimize();
    void output();
};


#endif //DAG_BLOCK_FUNCTION_MANAGER_H
