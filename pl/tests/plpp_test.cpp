#include "../include/pl/compiler/preprocessor/plpp.hpp"

#include <iostream>
#include <sstream>
#include <string>

static int failures = 0;

#define CHECK(cond)                                                          \
  do {                                                                       \
    if (!(cond)) {                                                           \
      ++failures;                                                            \
      std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond   \
                << "\n";                                                     \
    }                                                                        \
  } while (0)

// Reads everything through Plpp::get() and returns it as a string.
static std::string drain(const std::string& text) {
  std::istringstream in(text);
  plpp::Plpp p(in);
  std::string out;
  while (!p.eof()) out.push_back(static_cast<char>(p.get()));
  return out;
}

static void test_empty_input() {
  std::istringstream in("");
  plpp::Plpp p(in);
  CHECK(p.eof());
  CHECK(p.peek() == EOF);
  CHECK(p.get() == EOF);
}

static void test_peek_does_not_consume() {
  std::istringstream in("ab");
  plpp::Plpp p(in);
  CHECK(p.peek() == 'a');
  CHECK(p.peek() == 'a');
  CHECK(p.get() == 'a');
  CHECK(p.peek() == 'b');
}

static void test_get_sequence() {
  std::istringstream in("abc");
  plpp::Plpp p(in);
  CHECK(p.get() == 'a');
  CHECK(p.get() == 'b');
  CHECK(p.get() == 'c');
  CHECK(p.get() == EOF);
  CHECK(p.eof());
}

static void test_skips_whitespace() {
  CHECK(drain("  a \t b\r c  ") == "abc");
  CHECK(drain("   \t\t  ") == "");
}

static void test_newline_is_not_ignored() {
  CHECK(drain("a\nb") == "a\nb");
}

static void test_skips_comments() {
  CHECK(drain("; only a comment") == "");
  CHECK(drain("a ; comment\nb") == "ab");
  CHECK(drain("; first\n; second\nx") == "x");
}

static void test_comment_at_eof_without_newline() {
  CHECK(drain("a ;tail") == "a");
}

static void test_leading_ignored_before_peek() {
  std::istringstream in("  ; c\n  z");
  plpp::Plpp p(in);
  CHECK(p.peek() == 'z');
  CHECK(p.get() == 'z');
  CHECK(p.eof());
}

static void test_offset() {
  std::istringstream in("ab");
  plpp::Plpp p(in);
  CHECK(p.current_offset() == 0);
  p.get();
  CHECK(p.current_offset() == 1);
  p.get();
  CHECK(p.current_offset() == 2);
  p.get();  // EOF must not advance
  CHECK(p.current_offset() == 2);
}

static void test_position_tracking() {
  std::istringstream in("ab\nc");
  plpp::Plpp p(in);

  auto pos = p.current_position();
  CHECK(pos.line == 1 && pos.column == 1);

  p.get();  // a
  pos = p.current_position();
  CHECK(pos.line == 1 && pos.column == 2);

  p.get();  // b
  p.get();  // \n
  pos = p.current_position();
  CHECK(pos.line == 2 && pos.column == 1);

  p.get();  // c
  pos = p.current_position();
  CHECK(pos.line == 2 && pos.column == 2);
}

int main() {
  test_empty_input();
  test_peek_does_not_consume();
  test_get_sequence();
  test_skips_whitespace();
  test_newline_is_not_ignored();
  test_skips_comments();
  test_comment_at_eof_without_newline();
  test_leading_ignored_before_peek();
  test_offset();
  test_position_tracking();

  if (failures) {
    std::cerr << failures << " check(s) failed\n";
    return 1;
  }
  std::cout << "all plpp tests passed\n";
  return 0;
}
