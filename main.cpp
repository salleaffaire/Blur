#include <iostream>
#include <algorithm>

#include "bnf_parser.hpp"

#include "blur_lexer.hpp"
#include "blur_parser.hpp"

// Simple command line parser
// -------------------------------------------------------------------------------
char* getCmdOption(char ** begin, char ** end, const std::string & option) {
   char ** itr = std::find(begin, end, option);
   if (itr != end && ++itr != end) {
      return *itr;
   }
   return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
   return std::find(begin, end, option) != end;
}
// -------------------------------------------------------------------------------

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
   char * filename = getCmdOption(argv, argv + argc, "-f");
   
   if (filename) {
      // Test Blurred Lexer
      std::cout << "Compiling : " << filename << std::endl;
      blr_lexer lexer(filename);
      
      lexer.run();
      lexer.output_token_list();
      
      // Setup the parser
      blr_parser parser;
      parser.init(lexer.get_token_list_pointer(),
                  lexer.get_token_type_names());  


      // Call the top level production
      parser.program();

      // Output all types
      parser.output_all_types();

      // Output all structs
      parser.output_all_structs();

      // Output all Functions
      parser.output_all_functions();

      if (true == parser.get_state()) {
	 std::cout << "Compilation completed without errors." << std::endl << std::endl;
      }
      else {
	 std::cout << "PARSING ERRORS." << std::endl << std::endl;
      }
   }
#endif
     
   return 0;
}
