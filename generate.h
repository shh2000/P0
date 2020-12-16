//
// Created by AERO on 2019/11/17.
//

#ifndef SILLY_GENERATE_GENERATE_H
#define SILLY_GENERATE_GENERATE_H
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include "const_define.h"
#include "silly_cnt.h"
#include "function.h"
using namespace std;

class generate
{
public:
    ostringstream & target_code_buffer;
    generate(const string &filename, ostringstream &targetCodeBuffer);

    void ref_gen(const string &file_name);
    void mem_gen(const string &filename);
};


#endif //SILLY_GENERATE_GENERATE_H
