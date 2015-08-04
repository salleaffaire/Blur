#ifndef BLURRED_PARSER_TD_PRED_HPP___
#define BLURRED_PARSER_TD_PRED_HPP___

#include <list>
#include "blurred_lexer.hpp"
#include "blurred_ast.hpp"

// Production Rules
// -----------------------------------------------
enum BLR_RULE {
   blr_rule_program,
   blr_rule_decdef_list,
   blr_rule_decdef_rest,
   blr_rule_decdef,
   blr_rule_struct_declaration,
   blr_rule_data_definition,
   blr_rule_data_definition_cond,
   blr_rule_type_specifier,
   blr_rule_array_definition,
   blr_rule_base_type,
   blr_rule_fun_declaration,
   blr_rule_params,
   blr_rule_param_list,
   blr_rule_statement,
   blr_rule_expression_statement,
   blr_rule_compound_statement,
   blr_rule_statement_list,
   blr_rule_if_statement,
   blr_rule_return_statement,
   blr_rule_break_statement,
   blr_rule_while_statement,
   blr_rule_for_statement,
   blr_rule_expression,
   blr_rule_simple_expression,
   blr_rule_simple_term,
   blr_rule_factor,
   blr_rule_bin_operator_1,
   blr_rule_bin_operator_2,
   blr_rule_un_operator,
   blr_rule_call,
   blr_rule_args,
   blr_rule_arg_list,
   blr_rule_arg,
   blr_rule_constant,
   blr_rule_mutable
};

class blr_parser {
public:
   blr_parser() : mVerbose(true),
                  mpTokenList((std::list<std::shared_ptr<blr_token>> *)0) {}
   blr_parser(std::list<std::shared_ptr<blr_token>> *tl) : mVerbose(true),
                                                           mpTokenList(tl) {}

   ~blr_parser() {}

   void init(std::list<std::shared_ptr<blr_token>> *tl) {
      mpTokenList = tl;
      mLookAheadSymbol = mpTokenList->begin();
                                                
      // Calculate FIRST and FOLLOW
      // ------------------------------------------------------
      // 
      
   }

   bool program() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "program" << std::endl;
      }

      rval = decdef_list();

      return rval;
   }

   bool decdef_list() {
      bool rval = true;
      
      if (mVerbose) {
         std::cout << "decdef_list" << std::endl;
      }      
      
      rval = decdef();
      rval = decdef_rest();
      return rval;
   }

   bool decdef_rest() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "decdef_rest" << std::endl;
      }

      if (!IsEnd()) {
         if (!match(blr_token_rightbrace)) {
            rval = decdef();
            rval = decdef_rest();
         }
      }
      
      return rval;      
   }

   bool decdef() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "decdef" << std::endl;
      }

      // Found struct declaration
      if (match(blr_token_struct)) {
         struct_declaration();
      }
      else if (match(blr_token_struct)) {
         
      }
      
      return rval;
   }
   
   bool struct_declaration() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "struct_declaration" << std::endl;
      }
      

      return rval;
   }

   bool match(BLR_TOKEN_TYPE tt) {
      bool rval = false;
      if ((**mLookAheadSymbol).mType == tt) {
         ++mLookAheadSymbol;
         rval = true;
      }
      return rval;
   }

private:

   bool mVerbose; 

   bool IsEnd() {
      return (mLookAheadSymbol == mpTokenList->end());
   }

   // Pointer to the Lexer's token list
   std::list<std::shared_ptr<blr_token>>           *mpTokenList;

   // Current look-ahead symbol
   std::list<std::shared_ptr<blr_token>>::iterator mLookAheadSymbol;

   // FIRST
   std::map<BLR_RULE, std::list<blr_token>> mFirst;

   // FOLLOW
   std::map<BLR_RULE, std::list<blr_token>> mFollow;

   // AST TREE
   blr_ast_node mASTRoot;

};

#endif
