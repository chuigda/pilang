#include "yystype.h"
#include "mstring.h"
#include "y.tab.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static bool my_strcmpi(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s2 != '\0') {
    if (tolower(*s1) != tolower(*s2)) {
      return false;
    }
  }
  if (*s1 != '\0' || *s2 != '\0') {
    return false;
  }
  return true;
}

FILE *fp_lex_in;

static int peeked_char;
static uint16_t peeked_row;
static uint16_t peeked_col;
static int peeked_char_count = 0;

static char curchar_ = 31;
static uint16_t currow_ = 1;
static uint16_t curcol_ = 0;

static void get_next_char(void) {
  int value = fgetc(fp_lex_in);
  curchar_ = value != EOF ? (char)value : '\0';
  if (curchar_ == '\n') {
    currow_++;
    curcol_ = 0;
  }
  else {
    curcol_++;
  }
}

static char curchar(void) {
  return curchar_;
}

static uint16_t currow(void) {
  return currow_;
}

static uint16_t curcol(void) {
  return curcol_;
}

static void lex_warn(const char *warn_text, uint16_t line,
                     uint16_t col) {
  fprintf(stderr, "at (%d, %d): warning: %s\n", line, col, warn_text);
}

static int maybe_id_to_kwd(const char *str) {
  #define STRING_CASE(EXPECT, TOKEN_KIND) \
    if (!my_strcmpi(str, EXPECT)) { \
      yylval.token.token_kind = TOKEN_KIND; \
      return TOKEN_KIND; \
    }
  
  STRING_CASE("function", TK_FUNCTION)
  STRING_CASE("takes", TK_TAKES)
  STRING_CASE("returns", TK_RETURNS)
  STRING_CASE("begin", TK_BEGIN)
  STRING_CASE("end", TK_END)

  #undef STRING_CASE
}

static int lex_id_or_kwd(void) {
  char buffer[32];
  uint16_t row = currow();
  uint16_t col = curcol();
  int idx = 0;
  while (idx < 31 && isalnum(curchar())) {
    buffer[idx] = curchar();
    ++idx;
    get_next_char();
  }
  buffer[idx] = '\0';
  if (idx == 31) {
    lex_warn("Identifier length exceeds 31 characters", row, col);
    while (isalnum(curchar())) {
      get_next_char();
    }
  }
  yylval.token.row = row;
  yylval.token.col = row;
  yylval.token.replaced = 0;

  int convert_result = maybe_id_to_kwd(buffer);
  if (convert_result != TK_ID) {
    return convert_result;
  }
  yylval.token.token_kind = TK_ID;
  yylval.token.val.svalue = create_string(buffer);
  return TK_ID;
}

int yylex(void) {
  get_next_char();
  switch (curchar()) {
  case '\0':
    return -1;
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
  case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
  case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
  case 'v': case 'w': case 'x': case 'y': case 'z':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
  case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
  case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
  case 'V': case 'W': case 'X': case 'Y': case 'Z':
    return lex_id_or_kwd();
  }
  return -1;
}
