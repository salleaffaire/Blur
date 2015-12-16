#include <iostream>
#include "blur_token.hpp"

void output_token(BLR_TOKEN_TYPE token) {
   if (blr_token_slash == token) {
      std::cout << "/";
   }
   else if (blr_token_plus == token) {
      std::cout << "+";
   }
   else if (blr_token_minus == token) {
      std::cout << "+";
   }
   else if (blr_token_star == token) {
      std::cout << "*";
   }
   else if (blr_token_modulo == token) {
      std::cout << "%";
   }
   else if (blr_token_equal == token) {
      std::cout << "==";
   }
   else if (blr_token_notequal == token) {
      std::cout << "!=";
   }
   else if (blr_token_greater == token) {
      std::cout << ">";
   }
   else if (blr_token_lesser == token) {
      std::cout << ">";
   }
   else if (blr_token_greaterequal == token) {
      std::cout << "<=";
   }
   else if (blr_token_lesserequal == token) {
      std::cout << ">=";
   }
   else if (blr_token_or == token) {
      std::cout << "||";
   }
   else if (blr_token_and == token) {
      std::cout << "&&";
   }
   else if (blr_token_xor == token) {
      std::cout << "^^";
   }
   else if (blr_token_bwor == token) {
      std::cout << "|";
   }
   else if (blr_token_bwand == token) {
      std::cout << "&";
   }
   else if (blr_token_bwxor == token) {
      std::cout << "^";
   }
   else if (blr_token_leftshift == token) {
      std::cout << "<<";
   }
   else if (blr_token_rightshift == token) {
      std::cout << ">>";
   }
   else {
      std::cout << "!NOT A BINARY OPERATOR!" << std::endl;
   }
}
