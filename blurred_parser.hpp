#ifndef BLURRED_PARSER_TD_PRED_HPP___
#define BLURRED_PARSER_TD_PRED_HPP___

#include <list>
#include "blurred_lexer.hpp"
#include "blurred_ast.hpp"

// First version
// LL(1) parser - very simple and perhaps not very efficient
// had to introduce 'var' and 'func' keyword to simplify the grammar
// and make it more LL(1) friendly
// ------------------------------------------------------------------ 

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

   ~blr_parser() {}

   void init(std::list<std::shared_ptr<blr_token>> *tl,
             std::map<BLR_TOKEN_TYPE, std::string> *ttn) {
      mpTokenList = tl;
      mLookAheadSymbol = mpTokenList->begin();

      mpTokenTypeNames = ttn;
                                                
      // Calculate FIRST and FOLLOW
      // ------------------------------------------------------
      // 
      
   }

   bool program() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<program>" << std::endl;
      }

      rval = decdef_list();

      return rval;
   }

   bool decdef_list() {
      bool rval = true;
      
      if (mVerbose) {
         std::cout << "<decdef_list>" << std::endl;
      }      
      
      do {
         if (rval) { rval = decdef(); }
      } while (rval && !IsEnd() && !isnext(blr_token_rightbrace));

      return rval;
   }

   bool decdef() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<decdef>" << std::endl;
      }

      if (isnext(blr_token_struct)) {
         rval = struct_declaration();
      }
      else if (isnext(blr_token_if)) {
         rval = data_definition();
      }
      else if (isnext(blr_token_var)) {
         rval = data_definition();
      }
      else if (isnext(blr_token_func)) {
         rval = fun_declaration();
      } 
      else {
         rval = false;
         error();
      }
      
      return rval;
   }
   
   bool struct_declaration() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<struct_declaration>" << std::endl;
      }
      
      if (rval) { rval = match(blr_token_struct); }
      if (rval) { rval = match(blr_token_name); }
      if (rval) { rval = match(blr_token_leftbrace); }
      if (rval) { rval = decdef_list(); }
      if (rval) { rval = match(blr_token_rightbrace); }

      if (!rval) {
         error();
      }

      return rval;
   }

   bool data_definition() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<data_definition>" << std::endl;
      }

      if (match(blr_token_var)) {
         if (rval) { rval = type_specifier(); }
         if (rval) { rval = match(blr_token_name); }
         if (rval) { rval = match(blr_token_semicolon); }
      }
      else if (match(blr_token_if)) {
         if (rval) { rval = match(blr_token_leftpar); }
         if (rval) { rval = simple_expression(); }
         if (rval) { rval = match(blr_token_rightpar); }
      }
      else {
         rval = false;
         error();
      }
      
      if (!rval) {
         error();
      }
     
      return rval;
   }
   
   bool type_specifier() {
      bool rval = true;
      
      if (mVerbose) {
         std::cout << "<type_specifier>" << std::endl;
      }
      
      if (rval) { rval = base_type(); }
      if (rval) { 
         if (isnext(blr_token_leftbracket)) { rval = array_definition(); }
      }
      
      if (!rval) {
         error();
      }      
      
      return rval;
   }
   
   bool fun_declaration() {
      bool rval = true;
      
      if (mVerbose) {
         std::cout << "<fun_declaration>" << std::endl;
      }
      
      if (rval) { rval = match(blr_token_func); }
      if (rval) { rval = type_specifier(); }
      if (rval) { rval = match(blr_token_name); }
      if (rval) { rval = match(blr_token_leftpar); }
      if (rval) { rval = params(); }
      if (rval) { rval = match(blr_token_rightpar); }
      if (rval) { rval = match(blr_token_leftbrace); }
      if (rval) { rval = statement(); }
      if (rval) { rval = match(blr_token_rightbrace); }
      
      if (!rval) {
         error();
      }
      
      return rval;
   }
   
   bool params() {
      bool rval = true;
      
      if (mVerbose) {
         std::cout << "<params>" << std::endl;
      }
      
      if (rval) { 
         if (isnext(blr_token_rightpar)) {} 
         else { rval = param_list(); }
      }
      
      if (!rval) {
         error();
      }
      
      return rval;
   }
   
   bool param_list() {
      bool rval = true;
      
      if (mVerbose) {
         std::cout << "<param-list>" << std::endl;
      }
      
      do {
         if (rval) { rval = type_specifier(); }
         if (rval) { rval = match(blr_token_name); }
      } while (rval && match(blr_token_comma));
               
      if (!rval) {
         error();
      }
      
      return rval;
   }

   bool array_definition() {
      bool rval = true;
      if (mVerbose) {
         std::cout << "<array_definition>" << std::endl;
      }      
      while (rval && isnext(blr_token_leftbracket)) {
         if (rval) { rval = match(blr_token_leftbracket); }
         if (rval) { 
            if (match(blr_token_numeral)) {}
            else if (match(blr_token_dotdot)) {}
            else { rval = false; }
         }
         if (rval) { rval = match(blr_token_rightbracket); }
      }
      
      if (!rval) {
         error();
      }    
      
      return rval;
   }   
   
   bool base_type() {
      bool rval = true;
      if (mVerbose) {
         std::cout << "<base_type>" << std::endl;
      } 
      if (rval) {
         if (rval = match(blr_token_bit)) {}
         else if (rval = match(blr_token_list)) {
            if (rval) { rval = match(blr_token_leftbrace); }
            if (rval) { rval = type_specifier(); }
            if (rval) { rval = match(blr_token_rightbrace); }
         }
         else if (rval = match(blr_token_void)) {}
         else if (rval = match(blr_token_name)) {}
      }
      
      if (!rval) {
         error();
      }         

      return rval;
   }

   bool simple_expression() {
      bool rval = true;
      if (mVerbose) {
         std::cout << "<simple_expression>" << std::endl;
      } 

      return rval;
   }

   bool statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<statement>" << std::endl;
      }

      if (!rval) {
         error();
      }

      return rval;
   }

   bool isnext(BLR_TOKEN_TYPE tt) {
      //std::cout << "IS In   : " << (*mpTokenTypeNames)[tt] << std::endl;
      //std::cout << "IS Next : " << (*mpTokenTypeNames)[(*mLookAheadSymbol)->mType] 
      //          << std::endl;
      return ((*mLookAheadSymbol)->mType == tt);
   }

   bool match(BLR_TOKEN_TYPE tt) {
      bool rval = false;
      std::cout << "   Match In   : " << (*mpTokenTypeNames)[tt] << std::endl;
      std::cout << "   Match Next : " << (*mpTokenTypeNames)[(*mLookAheadSymbol)->mType] 
                << std::endl;
      if ((**mLookAheadSymbol).mType == tt) {
         ++mLookAheadSymbol;
         rval = true;
      }
      return rval;
   }

   bool error() {
      std::cout << "Error - " << std::endl; 
   }

private:

   bool mVerbose; 

   bool IsEnd() {
      return (mLookAheadSymbol == mpTokenList->end());
   }

   // Pointer to the Lexer's token list
   std::list<std::shared_ptr<blr_token>>           *mpTokenList;
   std::map<BLR_TOKEN_TYPE, std::string>           *mpTokenTypeNames;

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
