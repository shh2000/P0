#include "generate.h"
#include "Lexer.h"
#include "program_analysis.h"
#include "wzm_settings.h"
#include "utils.h"
#include "inline_optimization.h"
#include "block_function_manager.h"

void output_silly_mid_code(const string &input_filename, const string &output_filename);

int main() {
    Lexer lexer("testfile.txt");
    vector<word> word_list = lexer.get_word_list();
    symbol_table table;
    ostringstream output_buffer, error_buffer, target_code_buffer;
    vector<mid_code> mid_code_buffer;
    words_manager manager(word_list);
    program_analysis program(table, output_buffer, error_buffer, mid_code_buffer, manager);
    program.program();
    string temp;
    if (error_buffer.str().empty())
    {
        ofstream output_file("mid.txt");
        for (auto item: const_string_record)
        {
            output_file << mid_code::declare_string_mid(item.first, item.second);
        }

        //output_file << mid_code_buffer.str();

        for (auto& item: mid_code_buffer)
        {
            output_file << item;
        }

        output_file.close();
        output_silly_mid_code("mid.txt", "origin_mid_code.txt");
        inline_optimization::optimize_code("mid.txt", "inline.txt");
        // return 0;
        block_function_manager temp("inline.txt", "internal_dag_optimize.txt");
        // temp.debug_output("before_dag.txt");
        temp.dag_optimize();
        // temp.debug_output("after_dag.txt");
        temp.output();
        // return 0;
        output_silly_mid_code("internal_dag_optimize.txt", "optimized_mid_code.txt");
        ostringstream wdnmd;
        ofstream out("mips.txt");
        generate gen_mips("internal_dag_optimize.txt", wdnmd);
        out << wdnmd.str();
        out.close();

        // TODO: delete mid files

    }
    else
    {
        ofstream error_file("error.txt");
        error_file << error_buffer.str();
        error_file.close();
    }
    return 0;
}

void output_silly_mid_code(const string &input_filename, const string &output_filename)
{
    ifstream input(input_filename);
    ofstream output(output_filename);
    string temp;
    while (getline(input, temp))
    {
        mid_code temp_code = line_to_mid_code(temp);
        output << temp_code.AS_check_string();
    }
    input.close();
    output.close();
}
