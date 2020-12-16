//
// Created by 王梓沐 on 2019/9/27.
//

#ifndef COMPLIER_1_1_LEXER_H
#define COMPLIER_1_1_LEXER_H

#include <string>
#include <map>
#include <vector>
#include <set>

using namespace std;

enum word_type {
    IDENFR, INTCON, CHARCON,
    STRCON, CONSTTK, INTTK,
    CHARTK, VOIDTK, MAINTK,
    IFTK, ELSETK, DOTK,
    WHILETK, FORTK, SCANFTK,
    PRINTFTK, RETURNTK, PLUS,
    MINU, MULT, DIV, LSS,
    LEQ, GRE, GEQ, EQL, NEQ,
    ASSIGN, SEMICN, COMMA,
    LPARENT, RPARENT, LBRACK,
    RBRACK, LBRACE, RBRACE, INVALID
};

const vector<string> type_string = {
        "IDENFR", "INTCON", "CHARCON",
        "STRCON", "CONSTTK", "INTTK",
        "CHARTK", "VOIDTK", "MAINTK",
        "IFTK", "ELSETK", "DOTK",
        "WHILETK", "FORTK", "SCANFTK",
        "PRINTFTK", "RETURNTK", "PLUS",
        "MINU", "MULT", "DIV", "LSS",
        "LEQ", "GRE", "GEQ", "EQL", "NEQ",
        "ASSIGN", "SEMICN", "COMMA",
        "LPARENT", "RPARENT", "LBRACK",
        "RBRACK", "LBRACE", "RBRACE", "INVALID"
};

class word {
private:
    int line_;
    word_type type_;
    string str_value_;
    bool valid_;
public:
    word(int line, word_type type, const std::string& str_value, bool valid);
    string to_string();
    string get_str_value();
    string debug_str();
    word_type get_type();
    bool is_valid();
    int get_line_num();
    static string type_string_transfer(word_type);
    static word_type type_string_transfer(const string&);
};

class Lexer{
private:
    vector<string> src_code;
    vector<word> word_list;
    string processing_string;
    int line_index = 0;
    const vector<string> reserve_words = {
            "const", "int", "char", "void",
            "main", "if", "else", "do",
            "while", "for", "scanf", "printf", "return",
    };

    // 务必小心这里的vector内部的顺序,由长到短。
    map<char, vector<string> > char_to_op_str = {
            {'<', {string("<="), string("<")}},
            {'>', {string(">="), string(">")}},
            {'=', {string("=="), string("=")}},
            {'!', {string("!=")}},
            {'+', {string("+")}},
            {'-', {string("-")}},
            {'*', {string("*")}},
            {'/', {string("/")}},
            {';', {string(";")}},
            {',', {string(",")}},
            {'(', {string("(")}},
            {')', {string(")")}},
            {'[', {string("[")}},
            {']', {string("]")}},
            {'{', {string("{")}},
            {'}', {string("}")}},
    };

    map<char, vector<word_type> > char_to_op_enum = {
            {'<', {LEQ, LSS}},
            {'>', {GEQ, GRE}},
            {'=', {EQL, ASSIGN}},
            {'!', {NEQ}},
            {'+', {PLUS}},
            {'-', {MINU}},
            {'*', {MULT}},
            {'/', {DIV}},
            {';', {SEMICN}},
            {',', {COMMA}},
            {'(', {LPARENT}},
            {')', {RPARENT}},
            {'[', {LBRACK}},
            {']', {RBRACK}},
            {'{', {LBRACE}},
            {'}', {RBRACE}},
    };

    void generate_word_list();
    void analysis_line();
    void delete_blank();
    bool analysis_reserve();
    bool analysis_identifier();
    bool analysis_int();
    bool analysis_char();
    bool analysis_string();
    bool analysis_op();
    bool analysis_illegal_word();
    int get_line_num();
    static bool is_letter(char);
    static bool is_letter_or_digit(char);
    static bool is_char_con_content(char);
    static bool is_string_content(char);
    static bool is_op_char(char);
    static bool a_start_with_b(const string & a, const string & b);

public:
    explicit Lexer(const std::string& file_path);
    vector<word> get_word_list();
};

#endif //COMPLIER_1_1_LEXER_H
