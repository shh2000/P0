//
// Created by AERO on 2019/10/30.
//

#ifndef COMPILER_2_COMPILE_ERROR_H
#define COMPILER_2_COMPILE_ERROR_H

#include <exception>
#include <sstream>

using namespace std;

class syntax_error: public exception
{
public:
    syntax_error() : exception(){}

    virtual void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " WTF" << endl;
    }
};

class lexer_error: public syntax_error
{
public:
    lexer_error(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " a" << endl;
    }
};

class redefine_error: public syntax_error
{
public:
    redefine_error(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " b" << endl;
    }
};

class not_define_error: public syntax_error
{
public:
    not_define_error(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " c" << endl;
    }
};

class para_num_miss_match: public syntax_error
{
public:
    para_num_miss_match(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " d" << endl;
    }
};

class para_type_miss_match: public syntax_error
{
public:
    para_type_miss_match(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " e" << endl;
    }
};

class condition_type_error: public syntax_error
{
public:
    condition_type_error(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " f" << endl;
    }
};

class re_void_re_mismatch: public syntax_error
{
public:
    re_void_re_mismatch(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " g" << endl;
    }
};

class re_value_type_mismatch: public syntax_error
{
public:
    re_value_type_mismatch(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " h" << endl;
    }
};

class index_not_int: public syntax_error
{
public:
    index_not_int(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " i" << endl;
    }
};

class assign_to_const: public syntax_error
{
public:
    assign_to_const(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " j" << endl;
    }
};

class miss_semicn: public syntax_error
{
public:
    miss_semicn(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " k" << endl;
    }
};

class miss_rparent: public syntax_error
{
public:
    miss_rparent(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " l" << endl;
    }
};

class miss_rbrack: public syntax_error
{
public:
    miss_rbrack(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " m" << endl;
    }
};

class do_miss_while: public syntax_error
{
public:
    do_miss_while(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " n" << endl;
    }
};

class const_init_error: public syntax_error
{
public:
    const_init_error(): syntax_error(){}
    void record_msg(ostringstream & error_buffer, int line_num)
    {
        error_buffer << line_num << " o" << endl;
    }
};

#endif //COMPILER_2_COMPILE_ERROR_H
