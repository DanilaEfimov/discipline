#include "../include/pl/compiler/preprocessor/plpp.hpp"

#include <cctype>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

static int failures = 0;
static const char *current_test = "";

// ---------------------------------------------------------------- repr

// Readable form of one char: 'a', '\n', ' ', '\x07'.
static std::string char_repr(int ch) {
  if (ch == EOF)
    return "EOF";
  std::string s = "'";
  switch (ch) {
  case '\n':
    s += "\\n";
    break;
  case '\t':
    s += "\\t";
    break;
  case '\r':
    s += "\\r";
    break;
  case '\b':
    s += "\\b";
    break;
  case '\0':
    s += "\\0";
    break;
  case '\'':
    s += "\\'";
    break;
  case '\\':
    s += "\\\\";
    break;
  default:
    if (std::isprint(ch)) {
      s += static_cast<char>(ch);
    } else {
      char buf[8];
      std::snprintf(buf, sizeof buf, "\\x%02X", ch & 0xFF);
      s += buf;
    }
  }
  s += "'";
  return s;
}

// Readable form of a string with escapes; spaces are shown as '·' so that
// trailing/leading spaces are visible.
static std::string str_repr(const std::string &text) {
  std::string s = "\"";
  for (char c : text) {
    switch (c) {
    case '\n':
      s += "\\n";
      break;
    case '\t':
      s += "\\t";
      break;
    case '\r':
      s += "\\r";
      break;
    case '\b':
      s += "\\b";
      break;
    case '\0':
      s += "\\0";
      break;
    case '"':
      s += "\\\"";
      break;
    case '\\':
      s += "\\\\";
      break;
    case ' ':
      s += "\xC2\xB7";
      break;
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
  s += "\"";
  return s;
}

template <class T> static std::string repr(const T &v) {
  if constexpr (std::is_same_v<T, std::string>) {
    return str_repr(v);
  } else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, int>) {
    return char_repr(v) +
           (v == EOF ? "" : " (" + std::to_string(static_cast<int>(v)) + ")");
  } else {
    std::ostringstream os;
    os << std::boolalpha << v;
    return os.str();
  }
}

// ---------------------------------------------------------------- macros

static void report(const char *file, int line, const std::string &what) {
  ++failures;
  std::cerr << "FAIL [" << current_test << "] " << file << ":" << line << "\n"
            << what << "\n";
}

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond))                                                               \
      report(__FILE__, __LINE__, std::string("  check failed: ") + #cond);     \
  } while (0)

// Compares two values and prints both on failure.
#define CHECK_EQ(actual, expected)                                             \
  do {                                                                         \
    auto a_ = (actual);                                                        \
    auto b_ = (expected);                                                      \
    if (!(a_ == b_)) {                                                         \
      report(__FILE__, __LINE__,                                               \
             std::string("  expression: ") + #actual + "\n  expected:   " +    \
                 repr(b_) + "\n  actual:     " + repr(a_));                    \
    }                                                                          \
  } while (0)

// Runs the input through Plpp::get() and compares the result string.
#define CHECK_DRAIN(input, expected)                                           \
  do {                                                                         \
    std::string in_ = (input);                                                 \
    std::string want_ = (expected);                                            \
    std::string got_ = drain(in_);                                             \
    if (got_ != want_) {                                                       \
      report(__FILE__, __LINE__,                                               \
             "  input:      " + str_repr(in_) + "\n  expected:   " +           \
                 str_repr(want_) + "\n  actual:     " + str_repr(got_));       \
    }                                                                          \
  } while (0)

// Checks current_position() as line:column.
#define CHECK_POS(p, exp_line, exp_col)                                        \
  do {                                                                         \
    auto pos_ = (p).current_position();                                        \
    if (pos_.line != (exp_line) || pos_.column != (exp_col)) {                 \
      report(__FILE__, __LINE__,                                               \
             "  expected position: " + std::to_string(exp_line) + ":" +        \
                 std::to_string(exp_col) +                                     \
                 "\n  actual position:   " + std::to_string(pos_.line) + ":" + \
                 std::to_string(pos_.column));                                 \
    }                                                                          \
  } while (0)

#define RUN(t)                                                                 \
  do {                                                                         \
    current_test = #t;                                                         \
    t();                                                                       \
  } while (0)

// ---------------------------------------------------------------- helpers

// Reads everything through Plpp::get() and returns it as a string.
// Capped so that a broken get() (that does not advance) cannot loop forever.
static std::string drain(const std::string &text) {
  std::istringstream in(text);
  plpp::Plpp p(in);
  std::string out;
  const std::size_t limit = text.size() + 16;
  while (!p.eof()) {
    out.push_back(static_cast<char>(p.get()));
    if (out.size() > limit) {
      out += "<...runaway: get() does not advance...>";
      break;
    }
  }
  return out;
}

// ---------------------------------------------------------------- tests

static void test_empty_input() {
  std::istringstream in("");
  plpp::Plpp p(in);
  CHECK(p.eof());
  CHECK_EQ(p.peek(), EOF);
  CHECK_EQ(p.get(), EOF);
}

static void test_peek_does_not_consume() {
  std::istringstream in("ab");
  plpp::Plpp p(in);
  CHECK_EQ(p.peek(), 'a');
  CHECK_EQ(p.peek(), 'a');
  CHECK_EQ(p.get(), 'a');
  CHECK_EQ(p.peek(), 'b');
}

static void test_get_sequence() {
  std::istringstream in("abc");
  plpp::Plpp p(in);
  CHECK_EQ(p.get(), 'a');
  CHECK_EQ(p.get(), 'b');
  CHECK_EQ(p.get(), 'c');
  CHECK_EQ(p.get(), EOF);
  CHECK(p.eof());
}

// ' ' is an ordinary character: it is never skipped.
static void test_space_is_a_regular_char() {
  CHECK_DRAIN(" ", " ");
  CHECK_DRAIN("   ", "   ");
  CHECK_DRAIN("a b", "a b");
  CHECK_DRAIN("  a  ", "  a  ");
}

// '\t', '\r' and '\b' are skipped.
static void test_tab_cr_backspace_are_skipped() {
  CHECK_DRAIN("\t", "");
  CHECK_DRAIN("\r", "");
  CHECK_DRAIN("\b", "");
  CHECK_DRAIN("\t\r\b", "");
  CHECK_DRAIN("a\tb", "ab");
  CHECK_DRAIN("a\rb", "ab");
  CHECK_DRAIN("a\bb", "ab");
  CHECK_DRAIN("a\t\r\bb", "ab");
  CHECK_DRAIN("\t\r\ba", "a");
  CHECK_DRAIN("a\t\r\b", "a");
}

// Mixed: spaces stay, skipped chars vanish.
static void test_mixed_whitespace() {
  CHECK_DRAIN("  a \t b\r c  ", "  a  b c  ");
  CHECK_DRAIN("   \t\t  ", "     ");
  CHECK_DRAIN(" \t ", "  ");
}

// "\r\n" line endings: '\r' disappears, '\n' remains.
static void test_crlf() { CHECK_DRAIN("a\r\nb", "a\nb"); }

static void test_newline_is_not_ignored() {
  CHECK_DRAIN("a\nb", "a\nb");
  CHECK_DRAIN("\n\n\n", "\n\n\n");
}

static void test_eof_only_when_nothing_significant_remains() {
  {
    std::istringstream in("\t\r\b");
    plpp::Plpp p(in);
    CHECK(p.eof());
  }
  {
    std::istringstream in("\t ");
    plpp::Plpp p(in);
    CHECK(!p.eof());
    CHECK_EQ(p.get(), ' ');
    CHECK(p.eof());
  }
}

static void test_peek_skips_only_ignored_chars() {
  std::istringstream in("\t\r\b a");
  plpp::Plpp p(in);
  CHECK_EQ(p.peek(), ' ');
  CHECK_EQ(p.get(), ' ');
  CHECK_EQ(p.get(), 'a');
}

// Comments are stripped; the terminating '\n' is preserved.
static void test_skips_comments() {
  CHECK_DRAIN("; only a comment", "");
  CHECK_DRAIN("a ; comment\nb", "a \nb"); // space before ';' is kept
  CHECK_DRAIN("; first\n; second\nx", "\n\nx");
}

static void test_comment_at_eof_without_newline() {
  CHECK_DRAIN("a ;tail", "a ");
}

static void test_leading_comment_before_peek() {
  std::istringstream in("; c\n  z");
  plpp::Plpp p(in);
  CHECK_EQ(p.peek(), '\n');
  CHECK_EQ(p.get(), '\n');
  CHECK_EQ(p.peek(), ' ');
  CHECK_EQ(p.get(), ' ');
  CHECK_EQ(p.get(), ' ');
  CHECK_EQ(p.get(), 'z');
  CHECK(p.eof());
}

static void test_space_before_comment_is_kept() {
  CHECK_DRAIN("a ; c\nb", "a \nb");
  CHECK_DRAIN("a; c\nb", "a\nb");
}

// Tabs/CRs/backspaces inside a comment don't matter.
static void test_skipped_chars_inside_comment() {
  CHECK_DRAIN("a;\t\r\b x\nb", "a\nb");
}

static void test_comment_only_line() {
  CHECK_DRAIN(";\n", "\n");
  CHECK_DRAIN("a\n;\nb", "a\n\nb");
}

static void test_offset() {
  std::istringstream in("ab");
  plpp::Plpp p(in);
  CHECK_EQ(p.current_offset(), std::size_t{0});
  p.get();
  CHECK_EQ(p.current_offset(), std::size_t{1});
  p.get();
  CHECK_EQ(p.current_offset(), std::size_t{2});
  p.get(); // EOF must not advance
  CHECK_EQ(p.current_offset(), std::size_t{2});
}

// offset counts every consumed byte: spaces, skipped chars and comments.
static void test_offset_with_whitespace_and_comments() {
  std::istringstream in(" a;c\nb");
  plpp::Plpp p(in);

  CHECK_EQ(p.current_offset(), std::size_t{0});

  CHECK_EQ(p.get(), ' ');
  CHECK_EQ(p.current_offset(), std::size_t{1});

  CHECK_EQ(p.get(), 'a');
  CHECK_EQ(p.current_offset(), std::size_t{2});

  // ';' and 'c' skipped as a comment, '\n' is significant
  CHECK_EQ(p.get(), '\n');
  CHECK_EQ(p.current_offset(), std::size_t{5});

  CHECK_EQ(p.get(), 'b');
  CHECK_EQ(p.current_offset(), std::size_t{6});

  CHECK_EQ(p.get(), EOF);
  CHECK_EQ(p.current_offset(), std::size_t{6});
}

static void test_offset_counts_skipped_chars() {
  std::istringstream in("\ta\t\rb");
  plpp::Plpp p(in);

  CHECK_EQ(p.get(), 'a');
  CHECK_EQ(p.current_offset(), std::size_t{2}); // '\t', 'a'

  CHECK_EQ(p.get(), 'b');
  CHECK_EQ(p.current_offset(), std::size_t{5}); // + '\t', '\r', 'b'
}

static void test_position_tracking() {
  std::istringstream in("ab\nc");
  plpp::Plpp p(in);

  CHECK_POS(p, 1u, 1u);

  p.get(); // a
  CHECK_POS(p, 1u, 2u);

  p.get(); // b
  p.get(); // \n
  CHECK_POS(p, 2u, 1u);

  p.get(); // c
  CHECK_POS(p, 2u, 2u);
}

// A space advances the column like any other char; a tab does too (it is
// consumed, just not returned).
static void test_position_with_space_and_skipped_chars() {
  std::istringstream in(" a\tb");
  plpp::Plpp p(in);

  CHECK_EQ(p.get(), ' ');
  CHECK_POS(p, 1u, 2u);

  CHECK_EQ(p.get(), 'a');
  CHECK_POS(p, 1u, 3u);

  CHECK_EQ(p.get(), 'b'); // '\t' skipped, then 'b'
  CHECK_POS(p, 1u, 5u);
}

// peek() consumes ignored chars, so the position moves to the first
// significant char.
static void test_position_after_peek_over_ignored() {
  std::istringstream in("\t\tx");
  plpp::Plpp p(in);

  CHECK_EQ(p.peek(), 'x');
  CHECK_POS(p, 1u, 3u);
}

static void test_position_after_comment() {
  std::istringstream in("ab;xx\ncd");
  plpp::Plpp p(in);

  CHECK_EQ(p.get(), 'a'); // 1:1
  CHECK_EQ(p.get(), 'b'); // 1:2

  // peek() skips ';xx'; the next significant char is '\n' at 1:6
  CHECK_EQ(p.peek(), '\n');
  CHECK_POS(p, 1u, 6u);

  CHECK_EQ(p.get(), '\n');
  CHECK_POS(p, 2u, 1u);

  CHECK_EQ(p.get(), 'c');
}

int main() {
  RUN(test_empty_input);
  RUN(test_peek_does_not_consume);
  RUN(test_get_sequence);
  RUN(test_space_is_a_regular_char);
  RUN(test_tab_cr_backspace_are_skipped);
  RUN(test_mixed_whitespace);
  RUN(test_crlf);
  RUN(test_newline_is_not_ignored);
  RUN(test_eof_only_when_nothing_significant_remains);
  RUN(test_peek_skips_only_ignored_chars);
  RUN(test_skips_comments);
  RUN(test_comment_at_eof_without_newline);
  RUN(test_leading_comment_before_peek);
  RUN(test_space_before_comment_is_kept);
  RUN(test_skipped_chars_inside_comment);
  RUN(test_comment_only_line);
  RUN(test_offset);
  RUN(test_offset_with_whitespace_and_comments);
  RUN(test_offset_counts_skipped_chars);
  RUN(test_position_tracking);
  RUN(test_position_with_space_and_skipped_chars);
  RUN(test_position_after_peek_over_ignored);
  RUN(test_position_after_comment);

  if (failures) {
    std::cerr << "\n" << failures << " check(s) failed\n";
    return 1;
  }
  std::cout << "all plpp tests passed\n";
  return 0;
}
