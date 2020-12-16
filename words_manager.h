//
// Created by 王梓沐 on 2019/9/30.
//

#ifndef COMPILER_2_WORDS_MANAGER_H
#define COMPILER_2_WORDS_MANAGER_H

#include <vector>
#include "Lexer.h"

using namespace std;

class words_manager {
private:
    vector<word> word_sequence_;
    int index;
    int sequence_length;
    int start_index = 0;
    int finish_index;

public:
    explicit words_manager(const vector<word> &);
    bool has_expect_sequence(const vector<word_type> &, bool);
    bool is_finish();
    bool set_index(int);
    bool move_by_int(int);
    bool retract(int);
    bool read_next_word();
    int get_line_count();
    word get_now_word_value();
    word get_word_by_index(int);
    word_type get_now_word_type();
    int get_start_index();
    int get_finish_index();
    int get_index();
    void print_move_front(ostringstream &, int);
    bool has_sequence_then_print_move(ostringstream &, const vector<word_type> &);
    bool has_expect_sequence(const word_type &, bool);
    bool has_sequence_then_print_move(ostringstream &, const word_type &);
    word &operator[](int);
    void skip();
    void skip(word_type end_mark);
    void skip(const set<word_type> & end_mark_set);
    bool has_types_in_range(const set<word_type>& types, int front_range);
    bool has_invalid_word_in_range(int front_range);
    int get_recent_line_num(int back_length = 1);
    word get_recent_word(int back_length = 1);
    void miss_semicn_skip_line();
    void semicn_end_skip();
    void lbrace_skip();
    void rbrace_skip();
    void block_skip();
};


#endif //COMPILER_2_WORDS_MANAGER_H
