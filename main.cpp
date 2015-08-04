
#include "bnf_parser.hpp"

#include "blurred_lexer.hpp"
#include "blurred_parser.hpp"

int main(int argc, char *argv[]) {

   #if 1

   bnf_parser bnfp("grammar/blurred_rr.bnf");
   bnfp.lex();
   bnfp.output_token_list();
   bnfp.output_root();
   bnfp.output_null();

   bnfp.parse();
   bnfp.output_productions();
   
   #endif

   #if 0
   // Test Blurred Lexer
   blr_lexer lexer("tests/test3.blr");

   lexer.run();
   lexer.output_token_list();

   // Setup the parser
   blr_parser parser(lexer.get_token_list_pointer());
   #endif


   return 0;
}
