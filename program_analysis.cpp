//
// Created by 王梓沐 on 2019/10/4.
//
#include <iostream>
#include "wzm_settings.h"
#include "program_analysis.h"
#include "variable_analysis.h"
#include "function_analysis.h"

using namespace std;

bool program_analysis::program()
{
    variable_analysis var_analysis(table, output_buffer, error_buffer,
            mid_code_buffer, manager, scope);
    function_analysis func_analysis(table, output_buffer, error_buffer,
            mid_code_buffer, manager, scope);
    var_analysis.const_description();
    var_analysis.variable_description();
    func_analysis.function_definition();
    func_analysis.main_function();
    output_buffer << "<程序>" << endl;
    #ifdef DEBUG
        cout << "<程序>" << endl;
    #endif
    return true;
}

program_analysis::program_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                                   vector<mid_code> &midCodeBuffer, words_manager &manager)
        : table(table), output_buffer(outputBuffer), error_buffer(errorBuffer), mid_code_buffer(midCodeBuffer),
        manager(manager)
{

}
