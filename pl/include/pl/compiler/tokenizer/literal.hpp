#pragma once

namespace plc {

inline constexpr char dec_prefix = 'd';
inline constexpr char hex_prefix = 'x';
inline constexpr char bin_prefix = 'b';

enum class LiteralPrefix {
    NotAprefix,
    Dec,
    Hex,
    Bin,
};

inline bool is_literal_prefix(char ch) {
    return ch == dec_prefix || ch == hex_prefix || ch == bin_prefix;
}

inline LiteralPrefix resolve_prefix(char ch) {
    if(is_literal_prefix(ch)) {
        if(ch == dec_prefix) return LiteralPrefix::Dec;
        if(ch == hex_prefix) return LiteralPrefix::Hex;
        if(ch == bin_prefix) return LiteralPrefix::Bin;
    }
    return LiteralPrefix::NotAprefix;
}

inline constexpr char prefix_to_char(LiteralPrefix prefix) {
    switch(prefix) {
        case LiteralPrefix::Bin: return bin_prefix;
        case LiteralPrefix::Hex: return hex_prefix;
        case LiteralPrefix::Dec: return dec_prefix;
        default: return '?';
    }
}

} // namespace plc
