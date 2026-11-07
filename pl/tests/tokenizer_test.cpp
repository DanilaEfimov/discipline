#include "../include/pl/compiler/tokenizer/tokenizer.hpp"

#include <cctype>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using plc::TokenCategory;

static int failures = 0;
static const char* current_test = "";

// ---------------------------------------------------------------- repr

static std::string str_repr(const std::string& text) {
  std::string s = "\"";
  for (char c : text) {
    switch (c) {
      case '\n': s += "\\n"; break;
      case '\t': s += "\\t"; break;
      case '\r': s += "\\r"; break;
      case '\b': s += "\\b"; break;
      case '\0': s += "\\0"; break;
      case '"':  s += "\\\""; break;
      case '\\': s += "\\\\"; break;
      case ' ':  s += "\xC2\xB7"; break;  // visible space
      default:
        if (std::isprint(static_cast<unsigned char>(c))) {
          s += c;
        } else {
          char buf[8];
          std::snprintf(buf, sizeof buf, "\\x%02X", c & 0xFF);
          s += buf;
        }
    }
  }
  return s + "\"";
}

static const char* cat_name(TokenCategory c) {
  switch (c) {
    case TokenCategory::Eof: return "Eof";
    case TokenCategory::Eol: return "Eol";
    case TokenCategory::Space: return "Space";
    case TokenCategory::Unknown: return "Unknown";
    case TokenCategory::Identifier: return "Identifier";
    case TokenCategory::Keyword: return "Keyword";
    case TokenCategory::Comma: return "Comma";
    case TokenCategory::Column: return "Column";
    case TokenCategory::Semicolumn: return "Semicolumn";
    case TokenCategory::Directive: return "Directive";
    case TokenCategory::lBracket: return "lBracket";
    case TokenCategory::rBracket: return "rBracket";
    case TokenCategory::lfBracket: return "lfBracket";
    case TokenCategory::rfBracket: return "rfBracket";
    case TokenCategory::lsBracket: return "lsBracket";
    case TokenCategory::rsBracket: return "rsBracket";
    case TokenCategory::Period: return "Period";
    case TokenCategory::Literal: return "Literal";
  }
  return "?";
}

// ---------------------------------------------------------------- expectations

// Expected token. If check_lex is false only the category is compared.
struct Exp {
  TokenCategory cat;
  std::string lex;
  bool check_lex;
};

static Exp E(TokenCategory c) { return {c, "", false}; }
static Exp E(TokenCategory c, std::string l) { return {c, std::move(l), true}; }
static const Exp END = E(TokenCategory::Eof);

using Pos = std::pair<std::size_t, std::size_t>;
static Pos P(std::size_t line, std::size_t col) { return {line, col}; }

static std::string describe(const std::vector<plc::Token>& toks) {
  std::string s;
  for (const auto& t : toks) {
    if (!s.empty()) s += "  ";
    s += cat_name(t.category);
    if (t.category != TokenCategory::Eof) s += " " + str_repr(t.lexeme);
  }
  return s;
}

static std::string describe(const std::vector<Exp>& exps) {
  std::string s;
  for (const auto& e : exps) {
    if (!s.empty()) s += "  ";
    s += cat_name(e.cat);
    if (e.check_lex) s += " " + str_repr(e.lex);
  }
  return s;
}

static std::string describe(const std::vector<Pos>& ps) {
  std::string s;
  for (const auto& p : ps) {
    if (!s.empty()) s += "  ";
    s += std::to_string(p.first) + ":" + std::to_string(p.second);
  }
  return s;
}

// ---------------------------------------------------------------- helpers

struct Lexer {
  std::istringstream in;
  plpp::Plpp pp;
  plc::Tokenizer tk;

  explicit Lexer(const std::string& text, plc::fileid_t file = 1)
      : in(text), pp(in), tk(pp, file) {}
};

// Collects tokens up to and including Eof. Capped so that a tokenizer that
// does not advance cannot loop forever.
static std::vector<plc::Token> lex_all(const std::string& text,
                                       plc::fileid_t file = 1) {
  Lexer lx(text, file);
  std::vector<plc::Token> out;
  const std::size_t limit = text.size() + 4;
  for (;;) {
    out.push_back(lx.tk.next());
    if (out.back().category == TokenCategory::Eof) break;
    if (out.size() > limit) break;  // runaway
  }
  return out;
}

static void report(const char* file, int line, const std::string& what) {
  ++failures;
  std::cerr << "FAIL [" << current_test << "] " << file << ":" << line << "\n"
            << what << "\n";
}

static bool matches(const std::vector<plc::Token>& got,
                    const std::vector<Exp>& want) {
  if (got.size() != want.size()) return false;
  for (std::size_t i = 0; i < got.size(); ++i) {
    if (got[i].category != want[i].cat) return false;
    if (want[i].check_lex && got[i].lexeme != want[i].lex) return false;
  }
  return true;
}

static void check_tokens(const char* file, int line, const std::string& input,
                         const std::vector<Exp>& want) {
  auto got = lex_all(input);
  if (!matches(got, want)) {
    report(file, line,
           "  input:      " + str_repr(input) +
           "\n  expected:   " + describe(want) +
           "\n  actual:     " + describe(got));
  }
}

static void check_positions(const char* file, int line,
                            const std::string& input,
                            const std::vector<Pos>& want) {
  auto toks = lex_all(input);
  std::vector<Pos> got;
  for (const auto& t : toks)
    got.push_back({t.position.local.line, t.position.local.column});
  if (got != want) {
    report(file, line,
           "  input:      " + str_repr(input) +
           "\n  tokens:     " + describe(toks) +
           "\n  expected:   " + describe(want) +
           "\n  actual:     " + describe(got));
  }
}

#define CHECK(cond)                                                          \
  do {                                                                       \
    if (!(cond))                                                             \
      report(__FILE__, __LINE__, std::string("  check failed: ") + #cond);   \
  } while (0)

#define CHECK_TOKENS(input, ...) \
  check_tokens(__FILE__, __LINE__, (input), std::vector<Exp>{__VA_ARGS__})

#define CHECK_POSITIONS(input, ...) \
  check_positions(__FILE__, __LINE__, (input), std::vector<Pos>{__VA_ARGS__})

#define RUN(t)                                                               \
  do {                                                                       \
    current_test = #t;                                                       \
    t();                                                                     \
  } while (0)

// ---------------------------------------------------------------- tests

static void test_empty_input() {
  CHECK_TOKENS("", END);
}

static void test_eof_is_sticky() {
  Lexer lx("");
  CHECK(lx.tk.next().category == TokenCategory::Eof);
  CHECK(lx.tk.next().category == TokenCategory::Eof);
  CHECK(lx.tk.next().category == TokenCategory::Eof);
}

static void test_single_char_symbols() {
  CHECK_TOKENS(":,.$(){}[]",
               E(TokenCategory::Column, ":"),
               E(TokenCategory::Comma, ","),
               E(TokenCategory::Period, "."),
               E(TokenCategory::Directive, "$"),
               E(TokenCategory::lBracket, "("),
               E(TokenCategory::rBracket, ")"),
               E(TokenCategory::lfBracket, "{"),
               E(TokenCategory::rfBracket, "}"),
               E(TokenCategory::lsBracket, "["),
               E(TokenCategory::rsBracket, "]"),
               END);
}

static void test_eol() {
  CHECK_TOKENS("\n", E(TokenCategory::Eol, "\n"), END);
  CHECK_TOKENS("\n\n", E(TokenCategory::Eol), E(TokenCategory::Eol), END);
}

static void test_every_keyword() {
  for (const char* kw : plc::keywords)
    CHECK_TOKENS(kw, E(TokenCategory::Keyword, kw), END);
}

static void test_identifiers() {
  CHECK_TOKENS("foo", E(TokenCategory::Identifier, "foo"), END);
  CHECK_TOKENS("_", E(TokenCategory::Identifier, "_"), END);
  CHECK_TOKENS("Foo_Bar", E(TokenCategory::Identifier, "Foo_Bar"), END);
  CHECK_TOKENS("loop", E(TokenCategory::Identifier, "loop"), END);
}

// A keyword is matched as a whole word, case-sensitively.
static void test_keyword_lookalikes_are_identifiers() {
  CHECK_TOKENS("adds", E(TokenCategory::Identifier, "adds"), END);
  CHECK_TOKENS("ad", E(TokenCategory::Identifier, "ad"), END);
  CHECK_TOKENS("ADD", E(TokenCategory::Identifier, "ADD"), END);
  CHECK_TOKENS("end_", E(TokenCategory::Identifier, "end_"), END);
}

static void test_literals() {
  CHECK_TOKENS("0d123", E(TokenCategory::Literal, "0d123"), END);
  CHECK_TOKENS("0d0", E(TokenCategory::Literal, "0d0"), END);
  CHECK_TOKENS("0hFF", E(TokenCategory::Literal, "0hFF"), END);
  CHECK_TOKENS("0hdeadBEEF", E(TokenCategory::Literal, "0hdeadBEEF"), END);
  CHECK_TOKENS("0b101", E(TokenCategory::Literal, "0b101"), END);
  CHECK_TOKENS("0b0", E(TokenCategory::Literal, "0b0"), END);
}

static void test_literal_stops_at_first_foreign_char() {
  CHECK_TOKENS("0hFg",
               E(TokenCategory::Literal, "0hF"),
               E(TokenCategory::Identifier, "g"), END);
  CHECK_TOKENS("0b102",
               E(TokenCategory::Literal, "0b10"),
               E(TokenCategory::Unknown), END);
  CHECK_TOKENS("0d12a",
               E(TokenCategory::Literal, "0d12"),
               E(TokenCategory::Identifier, "a"), END);
}

static void test_literals_around_punctuation() {
  CHECK_TOKENS("0b101,0d7",
               E(TokenCategory::Literal, "0b101"),
               E(TokenCategory::Comma, ","),
               E(TokenCategory::Literal, "0d7"), END);
  CHECK_TOKENS("[0hFF]",
               E(TokenCategory::lsBracket, "["),
               E(TokenCategory::Literal, "0hFF"),
               E(TokenCategory::rsBracket, "]"), END);
}

// A prefix with no digits after it is not a literal.
static void test_prefix_without_digits_is_unknown() {
  CHECK_TOKENS("0h", E(TokenCategory::Unknown), END);
  CHECK_TOKENS("0b", E(TokenCategory::Unknown), END);
  CHECK_TOKENS("0d", E(TokenCategory::Unknown), END);
  CHECK_TOKENS("0hg",
               E(TokenCategory::Unknown),
               E(TokenCategory::Identifier, "g"), END);
}

// A bare '0' is not a literal, and must not swallow the character after it.
static void test_bare_zero_does_not_swallow_next_char() {
  CHECK_TOKENS("0", E(TokenCategory::Unknown), END);
  CHECK_TOKENS("0\nadd",
               E(TokenCategory::Unknown),
               E(TokenCategory::Eol),
               E(TokenCategory::Keyword, "add"), END);
  CHECK_TOKENS("0,",
               E(TokenCategory::Unknown),
               E(TokenCategory::Comma), END);
}

static void test_unknown_chars() {
  for (const char* s : {"@", "#", "\"", "1", "9", "+"})
    CHECK_TOKENS(s, E(TokenCategory::Unknown), END);
}

// A byte >= 0x80 must not be mistaken for EOF (-1).
static void test_high_byte_is_not_eof() {
  CHECK_TOKENS("\xFF", E(TokenCategory::Unknown), END);
}

static void test_unknown_does_not_eat_following_token() {
  CHECK_TOKENS("@add",
               E(TokenCategory::Unknown),
               E(TokenCategory::Keyword, "add"), END);
}

// DECISION NEEDED: ' ' reaches the tokenizer as a regular character, and
// TokenCategory::Space exists, so these expect one Space token per ' '.
// If you'd rather skip spaces in the tokenizer, delete this test and
// remove Space tokens from the expectations below.
static void test_space_token() {
  CHECK_TOKENS(" ", E(TokenCategory::Space, " "), END);
  CHECK_TOKENS("a b",
               E(TokenCategory::Identifier, "a"),
               E(TokenCategory::Space),
               E(TokenCategory::Identifier, "b"), END);
}

static void test_comments() {
  CHECK_TOKENS("; only a comment", END);
  CHECK_TOKENS("add;c\nx",
               E(TokenCategory::Keyword, "add"),
               E(TokenCategory::Eol),
               E(TokenCategory::Identifier, "x"), END);
}

static void test_label_and_instruction_line() {
  CHECK_TOKENS("loop: add 0d1,0d2\njmp loop\n",
               E(TokenCategory::Identifier, "loop"),
               E(TokenCategory::Column),
               E(TokenCategory::Space),
               E(TokenCategory::Keyword, "add"),
               E(TokenCategory::Space),
               E(TokenCategory::Literal, "0d1"),
               E(TokenCategory::Comma),
               E(TokenCategory::Literal, "0d2"),
               E(TokenCategory::Eol),
               E(TokenCategory::Keyword, "jmp"),
               E(TokenCategory::Space),
               E(TokenCategory::Identifier, "loop"),
               E(TokenCategory::Eol),
               END);
}

static void test_directive_and_period() {
  CHECK_TOKENS("$define x",
               E(TokenCategory::Directive, "$"),
               E(TokenCategory::Identifier, "define"),
               E(TokenCategory::Space),
               E(TokenCategory::Identifier, "x"), END);
  CHECK_TOKENS(".data",
               E(TokenCategory::Period, "."),
               E(TokenCategory::Identifier, "data"), END);
}

static void test_comment_after_instruction_keeps_eol() {
  CHECK_TOKENS("print ; hello\nend",
               E(TokenCategory::Keyword, "print"),
               E(TokenCategory::Space),
               E(TokenCategory::Eol),
               E(TokenCategory::Keyword, "end"), END);
}

// Token position = position of its FIRST character (1-based).
static void test_positions() {
  CHECK_POSITIONS("add x\nfoo",
                  P(1, 1),   // add
                  P(1, 4),   // ' '
                  P(1, 5),   // x
                  P(1, 6),   // \n
                  P(2, 1),   // foo
                  P(2, 4));  // EOF
}

static void test_position_of_multichar_tokens() {
  CHECK_POSITIONS("0hFF x",
                  P(1, 1),   // 0hFF
                  P(1, 5),   // ' '
                  P(1, 6),   // x
                  P(1, 7));  // EOF
}

static void test_position_after_skipped_chars() {
  CHECK_POSITIONS("\tadd",
                  P(1, 2),   // add, after the skipped '\t'
                  P(1, 5));  // EOF
}

static void test_position_over_blank_lines() {
  CHECK_POSITIONS("\n\nx",
                  P(1, 1),   // \n
                  P(2, 1),   // \n
                  P(3, 1),   // x
                  P(3, 2));  // EOF
}

static void test_file_id_is_propagated() {
  for (const auto& t : lex_all("add x\n0hF", 7))
    CHECK(t.position.file_id == 7);
}

int main() {
  RUN(test_empty_input);
  RUN(test_eof_is_sticky);
  RUN(test_single_char_symbols);
  RUN(test_eol);
  RUN(test_every_keyword);
  RUN(test_identifiers);
  RUN(test_keyword_lookalikes_are_identifiers);
  RUN(test_literals);
  RUN(test_literal_stops_at_first_foreign_char);
  RUN(test_literals_around_punctuation);
  RUN(test_prefix_without_digits_is_unknown);
  RUN(test_bare_zero_does_not_swallow_next_char);
  RUN(test_unknown_chars);
  RUN(test_high_byte_is_not_eof);
  RUN(test_unknown_does_not_eat_following_token);
  RUN(test_space_token);
  RUN(test_comments);
  RUN(test_label_and_instruction_line);
  RUN(test_directive_and_period);
  RUN(test_comment_after_instruction_keeps_eol);
  RUN(test_positions);
  RUN(test_position_of_multichar_tokens);
  RUN(test_position_after_skipped_chars);
  RUN(test_position_over_blank_lines);
  RUN(test_file_id_is_propagated);

  if (failures) {
    std::cerr << "\n" << failures << " check(s) failed\n";
    return 1;
  }
  std::cout << "all tokenizer tests passed\n";
  return 0;
}
