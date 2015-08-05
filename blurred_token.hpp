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

   blr_token_struct,

   blr_token_bit,
   blr_token_list,
   blr_token_void,
   blr_token_sizeof,
   blr_token_in,
   blr_token_using,

   // Operators
   blr_token_slash,
   blr_token_plus,
   blr_token_minus,
   blr_token_star,
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
   blr_token_equal,
   blr_token_notequal,
   blr_token_greater,
   blr_token_lesser,
   blr_token_greaterequal,
   blr_token_lesserequal,
   blr_token_rightarrow,
   blr_token_not,
   blr_token_or,
   blr_token_and,
   blr_token_xor,
   blr_token_bwnot,
   blr_token_bwor,
   blr_token_bwand,
   blr_token_bwxor,
   blr_token_leftshift,
   blr_token_rightshift,
   blr_token_assignment,
   
   // Numerals
   blr_token_numeral,

   // String Literals
   blr_token_literal
};

struct blr_token {
   BLR_TOKEN_TYPE   mType;
   char             mValue[MAX_TOKEN_SIZE];
};



#endif
