//
// Created by AERO on 2019/11/30.
//

#ifndef SILLY_GENERATE_INLINE_OPTIMIZATION_H
#define SILLY_GENERATE_INLINE_OPTIMIZATION_H

#include <string>
#include <map>
#include "inline_function.h"

using namespace std;

class inline_optimization
{
private:
    const string FUNC_HEAD = "@func";
    map<string, inline_function *> name_to_func;
    vector<inline_function> function_list;
    set<string> func_to_embed;
    vector<string> not_in_func_code;
    string input_file_name, output_file_name;
    void output_mid_code(ofstream &optimized_code);
public:
    inline_optimization(const string &input_file_name, const string &output_file_name);
    bool optimize();
    static void optimize_code(const string &input_file_name, const string &output_file_name);
};


#endif //SILLY_GENERATE_INLINE_OPTIMIZATION_H
