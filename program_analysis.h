//
// Created by 王梓沐 on 2019/10/4.
//

#ifndef COMPILER_2_PROGRAM_ANALYSIS_H
#define COMPILER_2_PROGRAM_ANALYSIS_H

#include <sstream>
#include "words_manager.h"
#include "symbol_table.h"
#include "mid_code.h"

using namespace std;

class program_analysis
{
private:
    symbol_table & table;
    ostringstream & output_buffer;
    ostringstream & error_buffer;
    vector<mid_code> & mid_code_buffer;
    words_manager & manager;
    string scope = symbol_table::GLOBAL_SCOPE;

public:
    bool program();
    program_analysis(symbol_table &table, ostringstream &outputBuffer, ostringstream &errorBuffer,
                     vector<mid_code> &midCodeBuffer, words_manager &manager);
};


#endif //COMPILER_2_PROGRAM_ANALYSIS_H
