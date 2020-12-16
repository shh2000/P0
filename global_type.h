//
// Created by AERO on 2019/10/30.
//

#ifndef COMPILER_2_GLOBAL_TYPE_H
#define COMPILER_2_GLOBAL_TYPE_H


enum symbol_item_type {
    CONSTANT,
    VARIABLE,
    FUNCTION,
    PARAMETER,
};

enum value_type {
    NOT_VALUE,
    INT_TYPE_VALUE,
    CHAR_TYPE_VALUE,
};

enum func_type {
    NOT_FUNC,
    VOID_TYPE_FUNC,
    RE_INT_TYPE_FUNC,
    RE_CHAR_TYPE_FUNC,
};


#endif //COMPILER_2_GLOBAL_TYPE_H
