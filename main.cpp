
#include "blurred_lexer.hpp"
#include "blurred_parser.hpp"

int main(int argc, char *argv[]) {

   #if 1
   // Test Blurred Lexer
   blr_lexer lexer("tests/test2.blr");
   lexer.run();

   blr_parser parser(lexer.get_token_list_pointer());

   lexer.output_token_list();
   #endif


   return 0;
}
