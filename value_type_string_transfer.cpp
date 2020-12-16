//
// Created by AERO on 2019/11/17.
//

#include "global_type.h"
#include "value_type_string_transfer.h"

map<string, value_type > string_value_type_map{
        pair<string, value_type>("void", NOT_VALUE),
        pair<string, value_type>("int", INT_TYPE_VALUE),
        pair<string, value_type>("char", CHAR_TYPE_VALUE)
};

map<value_type, string> value_type_string_map {
        pair<value_type, string>(NOT_VALUE, "void"),
        pair<value_type, string>(INT_TYPE_VALUE, "int"),
        pair<value_type, string>(CHAR_TYPE_VALUE, "char")
};