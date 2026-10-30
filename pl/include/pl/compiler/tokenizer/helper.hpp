#pragma once

#include "token.hpp"
#include <istream>
#include <optional>

namespace plc {

struct ReservedSymbols {
  static constexpr char sym_zero = '0';
  static constexpr char sym_directive = '$';
  static constexpr char sym_comma = ',';
  static constexpr char sym_column = ':';
  static constexpr char sym_period = '.';
  static constexpr char sym_semicolumn = ';';
  static constexpr char sym_lbracket = '(';
  static constexpr char sym_rbracket = ')';
  static constexpr char sym_lfbracket = '{';
  static constexpr char sym_rfbracket = '}';
  static constexpr char sym_lsbracket = '[';
  static constexpr char sym_rsbracket = ']';
  static constexpr char sym_oel = '\n';
};

class TokenRecognizer {

private:
  template <typename F>
  static tokensize try_literal_of(std::istream &in, F &&checker) {
    tokensize length = 0;
    while (true) {
      int ch = in.peek();
      if (ch == std::char_traits<char>::eof())
        break;
      if (!checker(static_cast<char>(ch)))
        break;
      in.get();
      length++;
    }
    return length == 0 ? TokenRecognizer::no_match : length;
  }

public:
  static constexpr tokensize no_match = 0;

  static constexpr bool is_digit_dec(char c) { return c >= '0' && c <= '9'; }

  static constexpr bool is_digit_hex(char c) {
    return TokenRecognizer::is_digit_dec(c) || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
  }

  static constexpr bool is_digit_bin(char c) { return c == '0' || c == '1'; }

  static constexpr bool is_ident_char(char c) {
    unsigned char ch = static_cast<unsigned char>(c);
    return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }

  static tokensize try_identifier(std::istream &in) {
    int ch = in.peek();
    if (ch == std::char_traits<char>::eof() ||
        TokenRecognizer::is_digit_dec(ch))
      return TokenRecognizer::no_match;

    tokensize length = 1;
    while (is_ident_char(in.peek()) || is_digit_dec(in.peek())) {
      in.get();
      length++;
    }

    return length;
  }

  static bool try_identifier(std::istream &in, std::string &out) {
    int ch = in.peek();
    out.clear();

    while (true) {
      int c = in.peek();
      if (c == std::char_traits<char>::eof())
        break;
      char cc = static_cast<char>(c);
      if (!TokenRecognizer::is_ident_char(cc) &&
          !TokenRecognizer::is_digit_dec(cc))
        break;
      in.get();
      out.push_back(cc);
    }

    return !out.empty();
  }

  static tokensize try_literal_hex(std::istream &in) {
    return try_literal_of(in, TokenRecognizer::is_digit_hex);
  }

  static tokensize try_literal_bin(std::istream &in) {
    return try_literal_of(in, TokenRecognizer::is_digit_bin);
  }

  static tokensize try_literal_dec(std::istream &in) {
    return try_literal_of(in, TokenRecognizer::is_digit_dec);
  }

  static tokensize try_directive(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) ==
                   ReservedSymbols::sym_directive
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_comma(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) == ReservedSymbols::sym_comma
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_column(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) == ReservedSymbols::sym_column
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_period(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) == ReservedSymbols::sym_period
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_semicolumn(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) ==
                   ReservedSymbols::sym_semicolumn
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_lbracket(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) ==
                   ReservedSymbols::sym_lbracket
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_rbracket(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) ==
                   ReservedSymbols::sym_rbracket
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_lfbracket(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) ==
                   ReservedSymbols::sym_lfbracket
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_rfbracket(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) ==
                   ReservedSymbols::sym_rfbracket
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_lsbracket(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) ==
                   ReservedSymbols::sym_lsbracket
               ? 1
               : TokenRecognizer::no_match;
  }

  static tokensize try_rsbracket(std::istream &in) {
    return static_cast<unsigned char>(in.peek()) ==
                   ReservedSymbols::sym_rsbracket
               ? 1
               : TokenRecognizer::no_match;
  }
};

} // namespace plc
