//
// Created by 王梓沐 on 2019/9/30.
//
#include "wzm_settings.h"
#include <sstream>
#include "words_manager.h"

using namespace std;

words_manager::words_manager(const vector<word> & word_sequence) {
    word_sequence_ = vector<word>(word_sequence);
    sequence_length = word_sequence_.size();
    index = 0;
    finish_index = sequence_length;
}

bool words_manager::has_expect_sequence(
        const vector<word_type> & expect_sequence, bool move_index = false
                ) {
    int temp_index = index;
    int count = 0;
    while (
            count < expect_sequence.size()
            && temp_index < sequence_length
            )
    {
        if (word_sequence_[temp_index].get_type() != expect_sequence[count])
        {
            return false;
        }
        count++;
        temp_index++;
    }
    if (count != expect_sequence.size())
    {
        return false;
    }
    if (move_index)
    {
        index += count;
    }
    return true;
}

bool words_manager::is_finish() {
    return index >= sequence_length;
}

bool words_manager::set_index(int new_index) {
    if (new_index > sequence_length || new_index < 0)
    {
        return false;
    }
    index = new_index;
    return true;
}

bool words_manager::retract(int move = 1) {
    if (move > index || (index - move) > sequence_length)
    {
        return false;
    }
    index -= move;
    return true;
}

bool words_manager::read_next_word() {
    if (index < sequence_length)
    {
        index++;
        return true;
    }
    else
    {
        return false;
    }
    /*
     * TODO: 如果是非法字符，将根据之后错误检测的规则进行skip.
     */
}

int words_manager::get_line_count() {
    if (index >= sequence_length)
    {
        return word_sequence_[sequence_length].get_line_num();
    }
    return word_sequence_[index].get_line_num();
}

word words_manager::get_now_word_value() {
    return (index >= 0 && index < sequence_length)? word_sequence_[index]:
           (sequence_length > 0)? word_sequence_[sequence_length - 1]:
           word(1, INVALID, "", false);
}

word words_manager::get_word_by_index(int query_index) {
    return word_sequence_[query_index];
}

int words_manager::get_start_index() {
    return start_index;
}

int words_manager::get_finish_index() {
    return finish_index;
}

word &words_manager::operator[](int query_index) {
    return word_sequence_[query_index];
}

int words_manager::get_index() {
    return index;
}

word_type words_manager::get_now_word_type() {
    return (index >= 0 && index < sequence_length)? get_now_word_value().get_type(): INVALID;
}

bool words_manager::move_by_int(int length) {
    int new_index = length + index;
    return set_index(new_index);
}

void words_manager::print_move_front(ostringstream & output_buffer, int length)
{
    if (length < 0)
    {
        return;
    }
    for (int i = 0; i < length && index < finish_index; ++i)
    {
        output_buffer << word_sequence_[index].to_string() << endl;
        #ifdef DEBUG
            cout << word_sequence_[index].to_string() << endl;
        #endif
        move_by_int(1);
    }
}

bool words_manager::has_sequence_then_print_move(
        ostringstream & output_buffer,
        const vector<word_type> & sequence)
{
    if (has_expect_sequence(sequence, false))
    {
        print_move_front(output_buffer, sequence.size());
        return true;
    }
    return false;
}

bool words_manager::has_expect_sequence(const word_type & type, bool if_move)
{
    return has_expect_sequence(vector<word_type>({type}), if_move);
}

bool words_manager::has_sequence_then_print_move(ostringstream & output_buffer, const word_type & type)
{
    return has_sequence_then_print_move(output_buffer, vector<word_type>({type}));
}

void words_manager::skip(const set<word_type> &end_mark_set)
{
    int now_line = get_now_word_value().get_line_num();
    while (index < sequence_length)
    {
        if (get_now_word_value().get_line_num() != now_line)
        {
            return;
        }
        if (end_mark_set.count(get_now_word_type()) != 0)
        {
            return;
        }
        index++;
    }
}

void words_manager::skip()
{
    const set<word_type> end_set{};
    skip(end_set);
}

void words_manager::skip(word_type end_mark)
{
    const set<word_type> end_set{end_mark};
    skip(end_set);
}

bool words_manager::has_types_in_range(const set<word_type>& types, int front_range)
{
    int now_line = get_now_word_value().get_line_num();
    for (
            int temp_index = index, i = 0;
            i < front_range && temp_index < sequence_length;
            i++, temp_index++)
    {
        int word_line = get_word_by_index(temp_index).get_line_num();
        if (word_line != now_line)
        {
            return false;
        }
        if (types.count(get_word_by_index(temp_index).get_type()) != 0)
        {
            return true;
        }
    }
    return false;
}

bool words_manager::has_invalid_word_in_range(int front_range)
{
    set<word_type> temp_set{INVALID};
    return has_types_in_range(temp_set, front_range);
}

int words_manager::get_recent_line_num(int back_length)
{
    int query_index = index - back_length;
    return query_index < 0 ? 1: word_sequence_[query_index].get_line_num();
}

word words_manager::get_recent_word(int back_length)
{
    int query_index = index - back_length;
    return word_sequence_[query_index];
}

void words_manager::miss_semicn_skip_line()
{
    int now_line = get_now_word_value().get_line_num();
    if (now_line != get_recent_line_num())
    {
        return;
    }
    skip();
}

void words_manager::semicn_end_skip()
{
    skip(SEMICN);
}

void words_manager::lbrace_skip()
{
    skip(LBRACE);
}

void words_manager::rbrace_skip()
{
    skip(RBRACE);
}

void words_manager::block_skip()
{
    set<word_type> end_set{LBRACE, RBRACE};
    skip(end_set);
}
