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
   blr_parser() : mVerbose(true), mReported(false), mVerboseDebug(false),
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

      if (mVerboseDebug) {
         std::cout << "EXIT : <program>" << std::endl;
      }      

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

      if (mVerboseDebug) {
         std::cout << "EXIT : <decdef_list>" << std::endl;
      } 

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

      if (mVerboseDebug) {
         std::cout << "EXIT : <decdef>" << std::endl;
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

      if (mVerboseDebug) {
         std::cout << "EXIT : <struct_declaration>" << std::endl;
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

         // Conditional Variable <data-definition-tail>
         if (match(blr_token_if)) {
            if (rval) { rval = match(blr_token_leftpar); }
            if (rval) { rval = simple_expression(); }
            if (rval) { rval = match(blr_token_rightpar); }
         }

         if (rval) { rval = match(blr_token_semicolon); }
      }
      else {
         rval = false;
         error();
      }

      if (!rval) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <data_definition>" << std::endl;
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
 
      if (mVerboseDebug) {
         std::cout << "<EXIT : type_specifier>" << std::endl;
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
      //if (rval) { rval = match(blr_token_leftbrace); }
      if (rval) { rval = statement(); }
      //if (rval) { rval = match(blr_token_rightbrace); }
      
      if (!rval) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT: <fun_declaration>" << std::endl;
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

      if (mVerboseDebug) {
         std::cout << "EXIT : <params>" << std::endl;
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

      if (mVerboseDebug) {
         std::cout << "EXIT : <param-list>" << std::endl;
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
      if (mVerboseDebug) {
         std::cout << "EXIT : <array_definition>" << std::endl;
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

      if (mVerboseDebug) {
         std::cout << "EXIT : <base_type>" << std::endl;
      } 

      return rval;
   }

  bool expression() {
      bool rval = true;
      if (mVerbose) {
         std::cout << "<expression>" << std::endl;
      }
      
      if (isnext(blr_token_name) && issecondnext(blr_token_assignment)) {
         if (rval) { rval = match(blr_token_name); }
         if (rval) { rval = match(blr_token_assignment); }
         if (rval) { rval = expression(); }
      }
      else {
         // Here we take a guess but it really can't be anything else
         if (rval) { rval = simple_expression(); }
      }
   
      if (!rval) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <expression>" << std::endl;
      }

      return rval;
   }

   bool simple_expression() {
      bool rval = true;
      if (mVerbose) {
         std::cout << "<simple_expression>" << std::endl;
      } 
      
      if (rval) { rval = simple_term(); }
      
      while (rval && matchbin1()) {
         if (rval) { rval = simple_term(); } 
      }

      if (!rval) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <simple_expression>" << std::endl;
      } 

      return rval;
   }

   bool simple_term() {
      bool rval = true;
      if (mVerbose) {
         std::cout << "<simple_term>" << std::endl;
      } 
      
      if (rval) { rval = simple_factor(); }

      while (rval && matchbin2()) {
         if (rval) { rval = simple_factor(); } 
      }

      if (!rval) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <simple_term>" << std::endl;
      } 

      return rval;
   }
   
   bool simple_factor() {
      bool rval = true;
      if (mVerbose) {
         std::cout << "<simple_factor>" << std::endl;
      } 

      if (rval) {
         if (matchun()) {
            if (rval) { rval = simple_term(); }
         }
         else if (isnext(blr_token_name) && issecondnext(blr_token_leftpar)) {
         
         }
         else if (match(blr_token_name))    {}
         else if (match(blr_token_true))    {}
         else if (match(blr_token_false))   {}
         else if (match(blr_token_numeral)) {}
         else if (match(blr_token_literal)) {}
         else if (match(blr_token_leftpar)) {
            if (rval) { rval = simple_expression(); }
            if (rval) { match(blr_token_rightpar); }
         }
         else {
            // Empty expression
            rval = false;
         }
      }

      if (!rval) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <simple_factor>" << std::endl;
      } 

      return rval;
   }
  

   bool statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<statement>" << std::endl;
      }

      if (rval) {
         if (isnext(blr_token_var)) {
            if (rval) { rval = data_definition(); }
         }
         else if (isnext(blr_token_if)) {
            if (rval) { rval = if_statement(); }
         }
         else if (isnext(blr_token_break)) {
            if (rval) { rval = break_statement(); }
         }
         else if (isnext(blr_token_return)) {
            if (rval) { rval = return_statement(); }
         }
         else if (isnext(blr_token_leftbrace)) {
            if (rval) { rval = compound_statement(); }
         }
         else if (isnext(blr_token_while)) {
            if (rval) { rval = while_statement(); }
         }
         else if (isnext(blr_token_foreach)) {
            if (rval) { rval = foreach_statement(); }
         }
         else if (isnext(blr_token_for)) {
            if (rval) { rval = for_statement(); }
         }
         else {
            // Try an expression statement
            if (rval) { rval = expression_statement(); }
         }
      }

      if (!rval) {
         error();
      }

      return rval;
   }

   bool expression_statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<expression_statement>" << std::endl;
      }

      while (rval && !match(blr_token_semicolon)) {
         if (rval) { rval = expression(); }
      }

      if (!rval) {
         error();
      }

      return rval;
   }

   bool if_statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<if_statement>" << std::endl;
      }

      if (rval) { rval = match(blr_token_if); }
      if (rval) { rval = match(blr_token_leftpar); }
      if (rval) { rval = expression(); }
      if (rval) { rval = match(blr_token_rightpar); }
      if (rval) { rval = statement(); }
      if (match(blr_token_else)) {
         if (rval) { rval = statement(); }
      }

      if (!rval) {
         error();
      }

      return rval;
   }

   bool for_statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<if_statement>" << std::endl;
      }

      if (rval) { rval = match(blr_token_for); }
      if (rval) { rval = match(blr_token_leftpar); }
      if (rval) { rval = expression(); }
      if (rval) { rval = match(blr_token_semicolon); }
      if (rval) { rval = expression(); }
      if (rval) { rval = match(blr_token_semicolon); }
      if (rval) { rval = expression(); }
      if (rval) { rval = match(blr_token_rightpar); }
      if (rval) { rval = statement(); }

      if (!rval) {
         error();
      }

      return rval;
   }

   bool while_statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<while_statement>" << std::endl;
      }

      if (rval) { rval = match(blr_token_while); }
      if (rval) { rval = match(blr_token_leftpar); }
      if (rval) { rval = expression(); }
      if (rval) { rval = match(blr_token_rightpar); }
      if (rval) { rval = statement(); }
      if (!rval) {
         error();
      }

      return rval;
   }

   bool foreach_statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<foreach_statement>" << std::endl;
      }

      if (rval) { rval = match(blr_token_foreach); }
      if (rval) { rval = match(blr_token_leftpar); }
      if (rval) { rval = match(blr_token_name); }
      if (rval) { rval = match(blr_token_in); }
      if (rval) { rval = match(blr_token_name); }
      if (rval) { rval = statement(); }
      if (!rval) {
         error();
      }

      return rval;
   }

   bool break_statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<break_statement>" << std::endl;
      }

      if (rval) { rval = match(blr_token_break); }
      if (rval) { rval = match(blr_token_semicolon); }

      if (!rval) {
         error();
      }

      return rval;
   }

   bool return_statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<return_statement>" << std::endl;
      }

      if (rval) { rval = match(blr_token_return); }
      if (!isnext(blr_token_semicolon)) {
         rval = expression();
      }
      if (rval) { rval = match(blr_token_semicolon); }
      
      if (!rval) {
         error();
      }

      return rval;
   }

   bool compound_statement() {
      bool rval = true;

      if (mVerbose) {
         std::cout << "<compound_statement>" << std::endl;
      }

      // Open Brace {
      if (rval) { rval = match(blr_token_leftbrace); } 
      
      while (rval && !isnext(blr_token_rightbrace)) {
         rval = statement();
      }
      
      // Close Brace }
      if (rval) { rval = match(blr_token_rightbrace); } 

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

   bool issecondnext(BLR_TOKEN_TYPE tt) {
      //std::cout << "IS In   : " << (*mpTokenTypeNames)[tt] << std::endl;
      //std::cout << "IS Next : " << (*mpTokenTypeNames)[(*mLookAheadSymbol)->mType] 
      //          << std::endl;
      return ((*(std::next(mLookAheadSymbol)))->mType == tt);
   }

   bool match(BLR_TOKEN_TYPE tt) {
      bool rval = false;
      if ((**mLookAheadSymbol).mType == tt) {
         if (mVerbose) {
            std::cout << "   Match In   : " << (*mpTokenTypeNames)[tt] 
                   << " -> " << (**mLookAheadSymbol).mValue << std::endl;
         }
         ++mLookAheadSymbol;
         rval = true;
      }
      return rval;
   }

   bool matchbin1() {
      bool rval = false;
      BLR_TOKEN_TYPE tt = (**mLookAheadSymbol).mType;
      if ((tt == blr_token_plus)         ||
          (tt == blr_token_minus)        ||
          (tt == blr_token_lesser)       ||
          (tt == blr_token_greater)      ||
          (tt == blr_token_lesserequal)  ||
          (tt == blr_token_greaterequal) ||
          (tt == blr_token_equal)        ||
          (tt == blr_token_notequal)     ||
          (tt == blr_token_or)           ||
          (tt == blr_token_and)          ||
          (tt == blr_token_xor)          ||
          (tt == blr_token_bwor)         ||
          (tt == blr_token_bwand)        ||
          (tt == blr_token_leftshift)    ||
          (tt == blr_token_rightshift)) {

         std::cout << "   Match In   : " << (*mpTokenTypeNames)[tt] 
                   << " -> " << (**mLookAheadSymbol).mValue << std::endl;         
         ++mLookAheadSymbol;
         rval = true;         
      }
      return rval;
   }

   bool matchbin2() {
      bool rval = false;
      BLR_TOKEN_TYPE tt = (**mLookAheadSymbol).mType;
      if ((tt == blr_token_slash) ||
          (tt == blr_token_star)  ||
          (tt == blr_token_modulo)) {
         std::cout << "   Match In   : " << (*mpTokenTypeNames)[tt] 
                   << " -> " << (**mLookAheadSymbol).mValue << std::endl;
         ++mLookAheadSymbol;
         rval = true;         
      }
      return rval;
   }

   bool matchun() {
      bool rval = false;
      BLR_TOKEN_TYPE tt = (**mLookAheadSymbol).mType;
      if ((tt == blr_token_plus)  ||
          (tt == blr_token_minus) ||
          (tt == blr_token_not)   ||
          (tt == blr_token_bwnot)) {
         std::cout << "   Match In   : " << (*mpTokenTypeNames)[tt] 
                   << " -> " << (**mLookAheadSymbol).mValue << std::endl;
         ++mLookAheadSymbol;
         rval = true;         
      }
      return rval;
   }

   bool error() {
      if (!mReported) {
         std::cout << "Error at line - " << (**mLookAheadSymbol).mLineNumber << std::endl;
         mReported = true;
      } 
   }

private:

   bool mVerbose;
   bool mReported;
   bool mVerboseDebug;

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
