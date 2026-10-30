#pragma once

#include "tokenizer/token.hpp"
#include <string>
#include <array>

#define KEYWORD_TYPE const char*
#define KEYWORD inline constexpr KEYWORD_TYPE

namespace plc {

// arithmetic vvv   
KEYWORD kw_add = "add";
KEYWORD kw_mul = "mul";
KEYWORD kw_div = "div";
// comparing vvv
KEYWORD kw_eq = "eq";
KEYWORD kw_lt = "lt";
// memory vvv
KEYWORD kw_write = "write";
// input / output vvv
KEYWORD kw_read = "read";
KEYWORD kw_print = "print";
// control vvv
KEYWORD kw_jmp = "jmp";
KEYWORD kw_jmz = "jmz";
KEYWORD kw_switch = "switch";
KEYWORD kw_skip = "skip";
KEYWORD kw_back = "back";
KEYWORD kw_end = "end";

inline const std::array<KEYWORD_TYPE, 15> keywords = {
    kw_add, kw_mul, kw_div, kw_eq, kw_lt, kw_write,
    kw_read, kw_print, kw_jmp, kw_jmz, kw_switch, kw_skip,
    kw_back, kw_end,
};  // keywords

inline constexpr int not_a_keyword = -1;

inline int is_keyword(const std::string& word) {
    for(std::size_t i = 0; i < keywords.size(); ++i)
        if(word == keywords[i]) return i;
    return not_a_keyword;
}

} // namespace plc
