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

// Whitespace is now significant: it comes through verbatim.
static void test_whitespace_is_preserved() {
  CHECK(drain("  a \t b\r c  ") == "  a \t b\r c  ");
  CHECK(drain("   \t\t  ") == "   \t\t  ");
}

static void test_newline_is_not_ignored() {
  CHECK(drain("a\nb") == "a\nb");
  CHECK(drain("\n\n\n") == "\n\n\n");
}

// Comments are stripped; the terminating '\n' is preserved.
static void test_skips_comments() {
  CHECK(drain("; only a comment") == "");
  CHECK(drain("a ; comment\nb") == "a \nb");   // пробел перед ';' сохранён
  CHECK(drain("; first\n; second\nx") == "\nx"); // '\n' после каждого комментария
}

static void test_comment_at_eof_without_newline() {
  CHECK(drain("a ;tail") == "a ");
}

static void test_leading_comment_before_peek() {
  std::istringstream in("; c\n  z");
  plpp::Plpp p(in);
  // После комментария первый значимый символ — '\n' (не 'z'!).
  CHECK(p.peek() == '\n');
  CHECK(p.get() == '\n');
  // Теперь пробелы — значимые, peek вернёт первый пробел, а не 'z'.
  CHECK(p.peek() == ' ');
  CHECK(p.get() == ' ');
  CHECK(p.get() == ' ');
  CHECK(p.get() == 'z');
  CHECK(p.eof());
}

// Пробелы внутри комментария не важны, но пробел ПЕРЕД ';' значим.
static void test_space_before_comment_is_kept() {
  CHECK(drain("a ; c\nb") == "a \nb");
  CHECK(drain("a; c\nb") == "a\nb");
}

// ';' в начале строки: комментарий съедает строку, '\n' остаётся.
static void test_comment_only_line() {
  CHECK(drain(";\n") == "\n");
  CHECK(drain("a\n;\nb") == "a\n\nb");
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

// offset двигается за каждый прочитанный байт, включая пробелы и комментарии.
static void test_offset_with_whitespace_and_comments() {
  std::istringstream in(" a;c\nb");
  plpp::Plpp p(in);

  CHECK(p.current_offset() == 0);

  CHECK(p.get() == ' ');              // 1 байт
  CHECK(p.current_offset() == 1);

  CHECK(p.get() == 'a');              // ещё 1
  CHECK(p.current_offset() == 2);

  // дальше ';' и 'c' скипнуты как комментарий, '\n' — значимый
  CHECK(p.get() == '\n');
  CHECK(p.current_offset() == 5);     // ' ', 'a', ';', 'c', '\n'

  CHECK(p.get() == 'b');
  CHECK(p.current_offset() == 6);

  CHECK(p.get() == EOF);
  CHECK(p.current_offset() == 6);
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

// Позиция первого значимого символа после комментария.
static void test_position_after_comment() {
  std::istringstream in("ab;xx\ncd");
  plpp::Plpp p(in);

  CHECK(p.get() == 'a');   // 1:1
  CHECK(p.get() == 'b');   // 1:2

  // ';xx' скипнуто, следующий значимый — '\n' на 1:6
  auto pos = p.current_position();
  CHECK(pos.line == 1 && pos.column == 6);
  CHECK(p.get() == '\n');

  pos = p.current_position();
  CHECK(pos.line == 2 && pos.column == 1);
  CHECK(p.get() == 'c');
}

int main() {
  test_empty_input();
  test_peek_does_not_consume();
  test_get_sequence();
  test_whitespace_is_preserved();
  test_newline_is_not_ignored();
  test_skips_comments();
  test_comment_at_eof_without_newline();
  test_leading_comment_before_peek();
  test_space_before_comment_is_kept();
  test_comment_only_line();
  test_offset();
  test_offset_with_whitespace_and_comments();
  test_position_tracking();
  test_position_after_comment();

  if (failures) {
    std::cerr << failures << " check(s) failed\n";
    return 1;
  }
  std::cout << "all plpp tests passed\n";
  return 0;
}
