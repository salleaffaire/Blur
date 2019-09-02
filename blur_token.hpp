#ifndef BLURRED_TOKEN_HPP___
#define BLURRED_TOKEN_HPP___

#define MAX_TOKEN_SIZE 256

enum BLR_TOKEN_TYPE {
  // General names
  blr_token_name,

  // Reserved words
  blr_token_func,
  blr_token_var,

  blr_token_while,
  blr_token_if,
  blr_token_else,
  blr_token_for,
  blr_token_foreach,
  blr_token_break,

  // Class
  blr_token_class,

  blr_token_int8,
  blr_token_int16,
  blr_token_int32,
  blr_token_int64,
  blr_token_list,
  blr_token_string,
  blr_token_void,
  blr_token_sizeof,
  blr_token_in,
  blr_token_using,
  blr_token_return,

  blr_token_true,
  blr_token_false,

  // Operators
  blr_token_slash,   // Binary
  blr_token_plus,    // Binary
  blr_token_minus,   // Binary
  blr_token_star,    // Binary
  blr_token_modulo,  // Binary
  blr_token_leftpar,
  blr_token_rightpar,
  blr_token_leftbrace,
  blr_token_rightbrace,
  blr_token_leftbracket,
  blr_token_rightbracket,
  blr_token_semicolon,
  blr_token_comma,
  blr_token_dot,
  blr_token_dotdot,
  blr_token_equal,         // Binary
  blr_token_notequal,      // Binary
  blr_token_greater,       // Binary
  blr_token_lesser,        // Binary
  blr_token_greaterequal,  // Binary
  blr_token_lesserequal,   // Binary
  blr_token_rightarrow,
  blr_token_not,
  blr_token_or,   // Binary
  blr_token_and,  // Binary
  blr_token_xor,  // Binary
  blr_token_bwnot,
  blr_token_bwor,        // Binary
  blr_token_bwand,       // Binary
  blr_token_bwxor,       // Binary
  blr_token_leftshift,   // Binary
  blr_token_rightshift,  // Binary
  blr_token_assignment,
  blr_token_hash,

  // Numerals
  blr_token_numeral,

  // String Literals
  blr_token_literal
};

struct blr_token {
  BLR_TOKEN_TYPE mType;
  char mValue[MAX_TOKEN_SIZE];
  unsigned int mLineNumber;
  unsigned int mCharacterOnLine;
};

void output_token(BLR_TOKEN_TYPE token);

#endif
