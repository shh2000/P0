//
// Created by AERO on 2019/11/17.
//

#ifndef SILLY_GENERATE_MID_VAR_H
#define SILLY_GENERATE_MID_VAR_H


#include "global_type.h"

class mid_var
{
public:
    value_type type;
    int size;
    int length;
    int offset;
    bool is_global;
    mid_var(value_type type_, int size_, int length_, int offset_, bool is_global_ = false);
};


#endif //SILLY_GENERATE_MID_VAR_H
