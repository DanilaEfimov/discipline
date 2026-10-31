#pragma once

namespace plpp {

inline constexpr char start_of_comment = '#';
inline constexpr bool is_space(char ch) {
    return ch == ' ' || ch == '\t' 
        || ch == '\r' || ch == '\b';
};

} // namespace plpp
