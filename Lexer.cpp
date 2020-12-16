//
// Created by 王梓沐 on 2019/9/27.
//

#include <fstream>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "Lexer.h"

using namespace std;

std::string word::to_string() {
    return type_string[type_] + " " + str_value_;
}

word::word(int line, word_type type, const std::string& str_value, bool valid = true) {
    this->line_ = line;
    this->type_ = type;
    this->str_value_ = str_value;
    this->valid_ = valid;
}

word_type word::get_type() {
    return type_;
}

string word::get_str_value() {
    return str_value_;
}

string word::type_string_transfer(word_type type) {
    return type_string[type];
}

static int strcmp_ignore_case(const char *a, const char *b) {
    while (*a && *b)
    {
        char upper_a = toupper(*a), upper_b = toupper(*b);
        if (upper_a != upper_b)
        {
            return upper_a - upper_b;
        }
        a++;
        b++;
    }
    return toupper(*a) - toupper(*b);
}

word_type word::type_string_transfer(const string& str) {
    for (int i = 0; i < type_string.size(); ++i) {
        if (!strcmp_ignore_case(str.c_str(),type_string[i].c_str()))
        {
            return (word_type)i;
        }
    }
    return (word_type)-1;
}

bool word::is_valid() {
    return valid_;
}

string word::debug_str() {
    char buffer[32];
    string temp;
    sprintf(buffer, "%d", line_);
    temp = string(buffer);
    string re_value = "line: " + temp
            + ", type: " + type_string[type_]
            + ", value: " + str_value_ + ", is_valid: "
            + (valid_ ? "true":"false") + "\n";
    return re_value;
}

int word::get_line_num() {
    return line_;
}

vector<word> Lexer::get_word_list() {
    return word_list;
}

Lexer::Lexer(const std::string& file_path) {
    ifstream input_file(file_path);
    string temp;
    while (getline(input_file, temp))
    {
        src_code.emplace_back(temp);
    }
    input_file.close();
    generate_word_list();
}

void Lexer::delete_blank() {
    int i = 0;
    while (isspace(processing_string[i]))i++;
    processing_string = processing_string.substr(i);
}

void Lexer::analysis_line() {
    processing_string = src_code[line_index];
    while (!processing_string.empty())
    {
        delete_blank();
        if (processing_string.empty())
        {
            break;
        }
        if (analysis_reserve())
        {
            goto next_word;
        }
        else if (analysis_identifier())
        {
            goto next_word;
        }
        else if (analysis_int())
        {
            goto next_word;
        }
        else if (analysis_char())
        {
            goto next_word;
        }
        else if (analysis_string())
        {
            goto next_word;
        }
        else if (analysis_op())
        {
            goto next_word;
        }
        else
        {
            analysis_illegal_word();
        }
        next_word:
        delete_blank();
    }
}

bool Lexer::analysis_reserve() {
    string extend_line = processing_string + " ";
    for (const string& reserve:reserve_words)
    {
        if (
                a_start_with_b(extend_line, reserve)
                && !is_letter_or_digit(extend_line[reserve.length()])
                )
        {
            word_list.emplace_back(
                    get_line_num(),
                    word::type_string_transfer(reserve + "TK"), reserve, true);
            processing_string = processing_string.substr(reserve.length());
            return true;
        }
    }
    return false;
}

bool Lexer::analysis_identifier() {
    if (processing_string.length() == 0)
    {
        return false;
    }
    if (!is_letter(processing_string[0]))
    {
        return false;
    }
    int i = 0;
    while (is_letter_or_digit(processing_string[i]))i++;
    word_list.emplace_back(
            get_line_num(), IDENFR, processing_string.substr(0, i), true);
    processing_string = processing_string.substr(i);
    return true;
}

bool Lexer::analysis_int() {
    if (processing_string.length() == 0)
    {
        return false;
    }
    if (
            !isdigit(processing_string[0])
            )
    {
        return false;
    }
    int i = 0;
    while (isdigit(processing_string[i]))i++;

    if (
            processing_string[0] == '0'
            && i > 1
            )
    {
        return false;
    }

    word_list.emplace_back(
            get_line_num(), INTCON, processing_string.substr(0, i), true);
    processing_string = processing_string.substr(i);
    return true;
}

bool Lexer::analysis_op() {
    if (processing_string.length() == 0)
    {
        return false;
    }
    if (!is_op_char(processing_string[0]))
    {
        return false;
    }
    vector<string> possible_string(char_to_op_str[processing_string[0]]);
    vector<word_type> possible_enum(char_to_op_enum[processing_string[0]]);
    for (int i = 0; i < possible_string.size(); ++i) {
        if (a_start_with_b(processing_string, possible_string[i]))
        {
            word_list.emplace_back(
                    get_line_num(), possible_enum[i], possible_string[i], true);
            processing_string = processing_string.substr(possible_string[i].length());
            return true;
        }
    }
    return false;
}

bool Lexer::analysis_char() {
    if (processing_string.length() < 3)
    {
        return false;
    }
    if (
            processing_string[0] != '\''
            || processing_string[2] != '\''
            || !is_char_con_content(processing_string[1])
            )
    {
        return false;
    }
    word_list.emplace_back(
            get_line_num(), CHARCON, processing_string.substr(1,1), true);
    processing_string = processing_string.substr(3);
    return true;
}

bool Lexer::analysis_string() {
    if (processing_string.length() == 0)
    {
        return false;
    }
    if (processing_string[0] != '"')
    {
        return false;
    }
    int i = 1;
    while (processing_string[i] && is_string_content(processing_string[i]))i++;
    if (processing_string[i] != '"')
    {
        return false;
    }
    string str_value = processing_string.substr(1, i - 1);
    processing_string = processing_string.substr(i + 1);
    string final_value;
    for (int i = 0; i < str_value.size(); ++i)
    {

        if (str_value[i] == '\\')
        {
            final_value += "\\\\";
        }
        else
        {
            final_value += str_value.substr(i, 1);
        }
    }
    word new_item(get_line_num(), STRCON, final_value, true);

    word_list.emplace_back(new_item);
    return true;
}

bool Lexer::analysis_illegal_word() {
    int i = 0;
    while (processing_string[i] && !isspace(processing_string[i])) i++;
    string str_value = processing_string.substr(0, i);
    processing_string = processing_string.substr(i);
    word_list.emplace_back(get_line_num(), INVALID, str_value, false);
    return true;
}

bool Lexer::a_start_with_b(const string &a, const string &b) {
    return (a.find(b) == 0) && (a.find(b) != a.npos);
}

bool Lexer::is_letter(char c) {
    return isalpha(c) || c == '_';
}

bool Lexer::is_letter_or_digit(char c) {
    return is_letter(c) || isdigit(c);
}

int Lexer::get_line_num() {
    return line_index + 1;
}

bool Lexer::is_char_con_content(char c) {
    return is_letter(c) || isdigit(c)
    || c == '-' || c == '+'
    || c == '/' || c == '*';
}

bool Lexer::is_string_content(char c) {
    return c == 32
    || c == 33
    || (35 <= c && c <= 126);
}

bool Lexer::is_op_char(char c) {
    return c == '+'
    || c == '-' || c == '*'
    || c == '/' || c == ';'
    || c == ',' || c == '('
    || c == ')' || c == '['
    || c == ']' || c == '{'
    || c == '}' || c == '<'
    || c == '>' || c == '='
                || c == '!';
}

void Lexer::generate_word_list() {
    for (line_index = 0;line_index < src_code.size();line_index++)
    {
        analysis_line();
    }
}
