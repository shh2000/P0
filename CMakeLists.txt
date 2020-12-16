cmake_minimum_required(VERSION 3.15)
project(FORMAL)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(mid_code)
include_directories(symbol_table)
include_directories(syntax_analysis)

add_executable(FORMAL
        mid_code.cpp
        mid_code.h
        symbol_table.cpp
        symbol_table.h
        symbol_table_item.cpp
        symbol_table_item.h
        expression_analysis.cpp
        expression_analysis.h
        function_analysis.cpp
        function_analysis.h
        program_analysis.cpp
        program_analysis.h
        statement_analysis.cpp
        statement_analysis.h
        variable_analysis.cpp
        variable_analysis.h
        compile_error.h
        global_type.h
        Lexer.cpp
        Lexer.h
        main.cpp
        utils.cpp
        utils.h
        words_manager.cpp
        words_manager.h
        wzm_settings.h
        const_define.h
        mid_var.cpp
        mid_var.h
        generate.cpp
        generate.h
        value_type_string_transfer.h
        value_type_string_transfer.cpp
        function.cpp
        function.h
        inline_function.cpp
        inline_function.h
        inline_optimization.cpp
        inline_optimization.h basic_block.cpp basic_block.h dag_node.cpp dag_node.h dag_node_manager.cpp dag_node_manager.h
        block_function.cpp block_function.h block_function_manager.cpp block_function_manager.h
        silly_cnt.cpp
        silly_cnt.h
        )
