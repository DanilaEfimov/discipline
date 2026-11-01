#pragma once

#include "../assembler/abi.hpp"
#include "../tokenizer/token.hpp"
#include "../helper.hpp"
#include "../diagnostics/diagnostic.hpp"
#include "../diagnostics/warnings.hpp"
#include <cstdint>
#include <limits>
#include <format>
#include <string_view>

namespace plp {

inline constexpr int char_to_digit_dec(char c) noexcept {
    return c - '0';
}

inline constexpr int char_to_digit_hex(char c) noexcept {
    if (plc::TokenRecognizer::is_digit_dec(c))
        return char_to_digit_dec(c);

    if (std::isupper(static_cast<unsigned char>(c)))
        return c - 'A' + 10;

    return c - 'a' + 10;
}

inline constexpr int char_to_digit_bin(char c) noexcept {
    return c == '0' ? 0 : 1;
}

inline plabi::value_t parse_literal(const plc::Token &token) {
    if (token.lexeme.size() < 2 || token.lexeme[0] != '0')
        throw plc::Diagnostic{
            plc::Severity::Warning, plc::literal_out_of_range, token.position,
            std::format("malformed literal: '{}'", token.lexeme), {}, {}};

    const char prefix = token.lexeme[1];
    const std::string_view body = std::string_view(token.lexeme).substr(2);

    if (body.empty())
        throw plc::Diagnostic{
            plc::Severity::Warning, plc::literal_out_of_range, token.position,
            std::format("empty literal body after prefix '{}'", token.lexeme),
            {}, {}};

    std::uint64_t val = 0;

    if (prefix == 'd') {
        for (char c : body) {
            if (!plc::TokenRecognizer::is_digit_dec(c))
                throw plc::Diagnostic{
                    plc::Severity::Warning, plc::literal_out_of_range,
                    token.position,
                    std::format("bad decimal digit '{}' in literal '{}'", c,
                                token.lexeme),
                    {}, {}};
            val = val * 10 + char_to_digit_dec(c);
        }
    } else if (prefix == 'x') {
        for (char c : body) {
            if (!plc::TokenRecognizer::is_digit_hex(c))
                throw plc::Diagnostic{
                    plc::Severity::Warning, plc::literal_out_of_range,
                    token.position,
                    std::format("bad hex digit '{}' in literal '{}'", c,
                                token.lexeme),
                    {}, {}};
            val = val * 16 + char_to_digit_hex(c);
        }
    } else if (prefix == 'b') {
        for (char c : body) {
            if (c != '0' && c != '1')
                throw plc::Diagnostic{
                    plc::Severity::Warning, plc::literal_out_of_range,
                    token.position,
                    std::format("bad binary digit '{}' in literal '{}'", c,
                                token.lexeme),
                    {}, {}};
            val = val * 2 + char_to_digit_bin(c);
        }
    } else {
        throw plc::Diagnostic{
            plc::Severity::Warning, plc::literal_out_of_range, token.position,
            std::format("unknown literal prefix '{}' in '{}'", prefix,
                        token.lexeme),
            {}, {}};
    }

    if (val > std::numeric_limits<plabi::value_t>::max())
    throw plc::Diagnostic{
        plc::Severity::Warning, plc::literal_out_of_range,
        token.position,
        std::format("literal '{}' does not fit in 32 bits",
                    token.lexeme),
        {}, {}};

    return static_cast<plabi::value_t>(val);
}

} // namespace plp
