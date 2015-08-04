
#include "bnf_parser.hpp"

#include "blurred_lexer.hpp"
#include "blurred_parser.hpp"

int main(int argc, char *argv[]) {

   #if 0

   bnf_parser bnfp("grammar/blurred_rr.bnf");
   bnfp.lex();
   //std::cout << "Token List :" << std::endl;
   //bnfp.output_token_list();

   std::cout << "BNF Grammar : " << bnfp.get_file_name() << std::endl;
   bnfp.output_root();

   std::cout << " " << std::endl;
   bnfp.output_null();

   std::cout << " " << std::endl;
   bnfp.parse();

   std::cout << "Production Rules List : " << std::endl;
   bnfp.output_productions();
   
   #endif

   #if 1
   // Test Blurred Lexer
   blr_lexer lexer("tests/test1.blr");

   lexer.run();
   lexer.output_token_list();

   // Setup the parser
   blr_parser parser;
   parser.init(lexer.get_token_list_pointer(),
               lexer.get_token_type_names());

   // Call the top level production
   parser.program();
   #endif


   return 0;
}
