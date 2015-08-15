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
   blr_parser() : mVerbose(true), mReported(false), mVerboseDebug(false), mState(true),
                  mTypeSpecifierLevel(0),
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

   // Compiler entry point 
   // <program> is a list of declararions and definitions
   void program() {
      mState = true;

      if (mVerbose) {
         std::cout << "<program>" << std::endl;
      }

      decdef_list();

      if (mVerboseDebug) {
         std::cout << "EXIT : <program>" << std::endl;
      }      
   }

   void decdef_list() {
      if (mVerbose) {
         std::cout << "<decdef_list>" << std::endl;
      }      
      
      do {
         if (mState) { decdef(); }
      } while (mState && !IsEnd() && !isnext(blr_token_rightbrace));

      if (mVerboseDebug) {
         std::cout << "EXIT : <decdef_list>" << std::endl;
      } 
   }

   blr_ast_node *decdef() {
      blr_ast_node *rval = 0;
      if (mVerbose) {
         std::cout << "<decdef>" << std::endl;
      }

      if (isnext(blr_token_struct)) {
         struct_declaration();
      }
      else if (isnext(blr_token_if)) {
         data_definition();
      }
      else if (isnext(blr_token_var)) {
         data_definition();
      }
      else if (isnext(blr_token_func)) {
         fun_declaration();
      }
      else if (isnext(blr_token_using)) {
         using_clause();
      }
      else {
         mState = false;
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <decdef>" << std::endl;
      }
      return rval;
   }

   void using_clause() {
      if (mVerbose) {
         std::cout << "<using_clause>" << std::endl;
      }

      if (mState) { mState = match(blr_token_using); }
      if (mState) { mState = match(blr_token_literal); }

      if (mVerboseDebug) {
         std::cout << "EXIT : <using_clause>" << std::endl;
      }     
   }
   
   blr_ast_node *struct_declaration() {
      std::vector<blr_ast_node *> decdefs;
      std::string name;

      if (mVerbose) {
         std::cout << "<struct_declaration>" << std::endl;
      }
      
      if (mState) { mState = match(blr_token_struct); }
      if (mState) { mState = match(blr_token_name); }
      if (mState) { name = mLastMatchedValue; }
      if (mState) { mState = match(blr_token_leftbrace); }
      if (mState) { 
         do {
            if (mState) { 
               blr_ast_node *rval = decdef(); 
               if (mState) { decdefs.push_back(rval); }
            }
            
         } while (mState && !IsEnd() && !isnext(blr_token_rightbrace));
      }
      if (mState) { mState = match(blr_token_rightbrace); }

      if (!mState) {
         error();
      }
      else {
         blr_ast_node *rval = new blr_ast_node_struct(name, std::move(decdefs));
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <struct_declaration>" << std::endl;
      }
   }

   void data_definition() {
      if (mVerbose) {
         std::cout << "<data_definition>" << std::endl;
      }

      if (match(blr_token_var)) {
         if (mState) { type_specifier(); }
         if (mState) { mState = match(blr_token_name); }

         // Conditional Variable <data-definition-tail>
         if (match(blr_token_if)) {
            if (mState) { mState = match(blr_token_leftpar); }
            if (mState) { complex_expression(); }
            if (mState) { mState = match(blr_token_rightpar); }
         }

         if (mState) { mState = match(blr_token_semicolon); }
      }
      else {
         mState = false;
         error();
      }

      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <data_definition>" << std::endl;
      }
   }
  
   void fun_declaration() {
      if (mVerbose) {
         std::cout << "<fun_declaration>" << std::endl;
      }
      
      if (mState) { mState = match(blr_token_func); }
      if (mState) { type_specifier(); }
      if (mState) { mState = match(blr_token_name); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { params(); }
      if (mState) { mState = match(blr_token_rightpar); }
      //if (mState) { mState = match(blr_token_leftbrace); }
      if (mState) { statement(); }
      //if (mState) { mState = match(blr_token_rightbrace); }
      
      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT: <fun_declaration>" << std::endl;
      }
   }
   
   void params() { 
      if (mVerbose) {
         std::cout << "<params>" << std::endl;
      }
      
      if (mState) { 
         if (isnext(blr_token_rightpar)) {} 
         else { param_list(); }
      }
      
      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <params>" << std::endl;
      }
   }
   
   void param_list() {      
      if (mVerbose) {
         std::cout << "<param-list>" << std::endl;
      }
      
      do {
         if (mState) { type_specifier(); }
         if (mState) { mState = match(blr_token_name); }
      } while (mState && match(blr_token_comma));
               
      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <param-list>" << std::endl;
      }
   }

   blr_ast_node *type_specifier() {
      blr_ast_node * rval;
      ++mTypeSpecifierLevel;
      if (mVerbose) {
         std::cout << "<type_specifier>" << std::endl;
      }
      
      if (mState) { rval = base_type(); }
      if (mState) { 
         while (mState && isnext(blr_token_leftbracket)) { 
            blr_ast_node_array *array_ast = array_definition();
            array_ast->mOf = rval;
            rval = array_ast;
         }
      }
      
      if (!mState) {
         error();
      }     
 
      if (mVerboseDebug) {
         std::cout << "<EXIT : type_specifier>" << std::endl;
      }
      --mTypeSpecifierLevel;
      if (0 == mTypeSpecifierLevel) {
         mASTTypes.push_back(rval);
      }
      return rval;
   }
   
   // This function works but could easily be optimized
   // A better implementation would be to cerate a specialized match_numeral
   // and capture the translated string (to unsigned int) and have
   // size as an uint in the AST node. 
   blr_ast_node_array *array_definition() {
      blr_ast_node_array *rval = 0;
      std::string array_size;
      if (mVerbose) {
         std::cout << "<array_definition>" << std::endl;
      }
      
      if (mState) { mState = match(blr_token_leftbracket); }
      if (mState) {
            if (match(blr_token_numeral)) {
               array_size = mLastMatchedValue;
            }
            else if (match(blr_token_dotdot)) {}
            else { mState = false; }
      }
      if (mState) { mState = match(blr_token_rightbracket); }
      
      if (!mState) {
         error();
      }
      else {
         rval = new blr_ast_node_array(array_size);
      }
      if (mVerboseDebug) {
         std::cout << "EXIT : <array_definition>" << std::endl;
      }
      return rval;
   }
   
   blr_ast_node *base_type() {
      blr_ast_node *rval;
      if (mVerbose) {
         std::cout << "<base_type>" << std::endl;
      } 
      if (mState) {
         if (mState = match(blr_token_bit)) {
            rval = new blr_ast_node_base_type("bit", blr_type_bit);
         }
         else if (mState = match(blr_token_byte)) {
            rval = new blr_ast_node_base_type("byte", blr_type_byte);
         }
         else if (mState = match(blr_token_list)) {
            blr_ast_node *list_type;
            if (mState) { mState = match(blr_token_leftbrace); }
            if (mState) { list_type = type_specifier(); }
            if (mState) { mState = match(blr_token_rightbrace); }
            rval = new blr_ast_node_list("list", list_type);
         }
         else if (mState = match(blr_token_void)) {
            rval = new blr_ast_node_base_type("void", blr_type_void);
         }
         else if (mState = match(blr_token_name)) {
            rval = new blr_ast_node_base_type("struct", blr_type_struct);
         }
      }
      
      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <base_type>" << std::endl;
      }
      return rval;
   }

   void expression() {
      if (mVerbose) {
         std::cout << "<expression>" << std::endl;
      }
      
      if (isnext(blr_token_name) && issecondnext(blr_token_assignment)) {
         if (mState) { mState = match(blr_token_name); }
         if (mState) { mState = match(blr_token_assignment); }
         if (mState) { expression(); }
      }
      else {
         // Here we take a guess but it really can't be anything else
         if (mState) { complex_expression(); }
      }
   
      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <expression>" << std::endl;
      }
   }

   void complex_expression() {

      if (mVerbose) {
         std::cout << "<complex_expression>" << std::endl;
      } 

      if (mState) { simple_expression(); }
      
      while (mState && match(blr_token_dot)) {
         if (match(blr_token_rightarrow)) { 
            defered_call();
         }
         else {
            call();
         }
      }

      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <complex_expression>" << std::endl;
      }     
   }

   void simple_expression() {
      if (mVerbose) {
         std::cout << "<simple_expression>" << std::endl;
      } 
      
      if (mState) { simple_term(); }
      
      while (mState && matchbin1()) {
         if (mState) { simple_term(); } 
      }

      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <simple_expression>" << std::endl;
      } 
   }

   void simple_term() {
      if (mVerbose) {
         std::cout << "<simple_term>" << std::endl;
      } 
      
      if (mState) { simple_factor(); }

      while (mState && matchbin2()) {
         if (mState) { simple_factor(); } 
      }

      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <simple_term>" << std::endl;
      }
   }
   
   void simple_factor() {
      if (mVerbose) {
         std::cout << "<simple_factor>" << std::endl;
      } 

      if (mState) {
         if (matchun()) {
            if (mState) { simple_term(); }
         }
         else if (isnext(blr_token_name) && issecondnext(blr_token_leftpar)) {
            if (mState) { call(); }
         }
         else if (match(blr_token_name))    {}
         else if (match(blr_token_true))    {}
         else if (match(blr_token_false))   {}
         else if (match(blr_token_numeral)) {}
         else if (match(blr_token_literal)) {}
         else if (match(blr_token_leftpar)) {
            if (mState) { complex_expression(); }
            if (mState) { match(blr_token_rightpar); }
         }
         else if (match(blr_token_rightarrow)) {
            if (mState) { defered_call(); }
         }
         else {
            // Empty expression
            mState = false;
         }
      }

      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <simple_factor>" << std::endl;
      } 
   }

   void defered_call() {
      if (mVerbose) {
         std::cout << "<defered-call>" << std::endl;
      }

      if (mState) { call(); }
      
      if (!mState) {
         error();
      }
   }
  
   void call() {
      if (mVerbose) {
         std::cout << "<call>" << std::endl;
      }

      if (mState) { mState = match(blr_token_name); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { args(); }
      if (mState) { mState = match(blr_token_rightpar); }      
      
      if (!mState) {
         error();
      }

   }

   void args() {
      if (mVerbose) {
         std::cout << "<args>" << std::endl;
      }
      
      if (mState && !isnext(blr_token_rightpar)) {
         if (mState) { args_list(); }
      }
      
      if (!mState) {
         error();
      }
   }
   
   bool args_list() {
      if (mVerbose) {
         std::cout << "<args-list>" << std::endl;
      }
      
      do {
         if (mState) { complex_expression(); }
      } while (mState && match(blr_token_comma));
      
      if (!mState) {
         error();
      }      
   }


   void statement() {
      if (mVerbose) {
         std::cout << "<statement>" << std::endl;
      }

      if (mState) {
         if (isnext(blr_token_var)) {
            if (mState) { data_definition(); }
         }
         else if (isnext(blr_token_if)) {
            if (mState) { if_statement(); }
         }
         else if (isnext(blr_token_break)) {
            if (mState) { break_statement(); }
         }
         else if (isnext(blr_token_return)) {
            if (mState) { return_statement(); }
         }
         else if (isnext(blr_token_leftbrace)) {
            if (mState) { compound_statement(); }
         }
         else if (isnext(blr_token_while)) {
            if (mState) { while_statement(); }
         }
         else if (isnext(blr_token_foreach)) {
            if (mState) { foreach_statement(); }
         }
         else if (isnext(blr_token_for)) {
            if (mState) { for_statement(); }
         }
         else {
            // Try an expression statement
            if (mState) { expression_statement(); }
         }
      }

      if (!mState) {
         error();
      }
   }

   void expression_statement() {
      if (mVerbose) {
         std::cout << "<expression_statement>" << std::endl;
      }

      while (mState && !match(blr_token_semicolon)) {
         if (mState) { expression(); }
      }

      if (!mState) {
         error();
      }
   }

   void if_statement() {
      if (mVerbose) {
         std::cout << "<if_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_if); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { expression(); }
      if (mState) { mState = match(blr_token_rightpar); }
      if (mState) { statement(); }
      if (match(blr_token_else)) {
         if (mState) { statement(); }
      }

      if (!mState) {
         error();
      }
   }

   void for_statement() {
      if (mVerbose) {
         std::cout << "<if_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_for); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { expression(); }
      if (mState) { mState = match(blr_token_semicolon); }
      if (mState) { expression(); }
      if (mState) { mState = match(blr_token_semicolon); }
      if (mState) { expression(); }
      if (mState) { mState = match(blr_token_rightpar); }
      if (mState) { statement(); }

      if (!mState) {
         error();
      }
   }

   void while_statement() {
      if (mVerbose) {
         std::cout << "<while_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_while); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { expression(); }
      if (mState) { mState = match(blr_token_rightpar); }
      if (mState) { statement(); }
      if (!mState) {
         error();
      }
   }

   void foreach_statement() {
      if (mVerbose) {
         std::cout << "<foreach_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_foreach); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { mState = match(blr_token_name); }
      if (mState) { mState = match(blr_token_in); }
      if (mState) { mState = match(blr_token_name); }
      if (mState) { statement(); }
      if (!mState) {
         error();
      }
   }

   void break_statement() {
      if (mVerbose) {
         std::cout << "<break_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_break); }
      if (mState) { mState = match(blr_token_semicolon); }

      if (!mState) {
         error();
      }
   }

   void return_statement() {

      if (mVerbose) {
         std::cout << "<return_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_return); }
      if (!isnext(blr_token_semicolon)) {
         expression();
      }
      if (mState) { mState = match(blr_token_semicolon); }
      
      if (!mState) {
         error();
      }
   }

   void compound_statement() {
      if (mVerbose) {
         std::cout << "<compound_statement>" << std::endl;
      }

      // Open Brace {
      if (mState) { mState = match(blr_token_leftbrace); } 
      
      while (mState && !isnext(blr_token_rightbrace)) {
         statement();
      }
      
      // Close Brace }
      if (mState) { mState = match(blr_token_rightbrace); } 

      if (!mState) {
         error();
      }
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
         mLastMatchedValue = (**mLookAheadSymbol).mValue;
         ++mLookAheadSymbol;
         rval = true;
      }
      return rval;
   }

   // Match any weak operators (low order)
   // 
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

   // Match any strong operators (high order)
   // 
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
   
   void output_type_name(blr_ast_node *x) {
      // We have a struct
      if (blr_ast_node_struct *p = dynamic_cast<blr_ast_node_struct *>(x)) {
         std::cout << "Struct { " << std::endl;
         for (auto &x: p->mDecDefs) {
            output_type_name(x);
         }
         std::cout << "}" << std::cout;
      }
      // We have a base type
      else if (blr_ast_node_base_type *p = dynamic_cast<blr_ast_node_base_type *>(x)) {
         if (p->mType == blr_type_bool) {
            std::cout << "bool" << std::endl;
         }
         else if (p->mType == blr_type_string) {
            std::cout << "string" << std::endl;
         }
         else if (p->mType == blr_type_bit) {
            std::cout << "bit" << std::endl;
         }         
         else if (p->mType == blr_type_byte) {
            std::cout << "byte" << std::endl;
         }
         else if (p->mType == blr_type_void) {
            std::cout << "void" << std::endl;
         }
      }
      // We have a struct
      else if (blr_ast_node_array *p = dynamic_cast<blr_ast_node_array *>(x)) {
         std::cout << "Array of ";
         output_type_name(p->mOf);
      }
      // We have a struct
      else if (blr_ast_node_list *p = dynamic_cast<blr_ast_node_list *>(x)) {
         std::cout << "List of ";
         output_type_name(p->mOf);
      }
      // We have a struct
      else  {
         std::cout << "Func" << std::endl;
      }
   }

   void output_all_types() {
      for (auto &x: mASTTypes) {
         output_type_name(x);
      }
   }

private:

   bool mVerbose;
   bool mReported;
   bool mVerboseDebug;

   bool mState;

   bool IsEnd() {
      return (mLookAheadSymbol == mpTokenList->end());
   }

   // Pointer to the Lexer's token list
   std::list<std::shared_ptr<blr_token>>           *mpTokenList;
   std::map<BLR_TOKEN_TYPE, std::string>           *mpTokenTypeNames;

   std::string mLastMatchedValue;

   // Current look-ahead symbol
   std::list<std::shared_ptr<blr_token>>::iterator mLookAheadSymbol;

   // FIRST
   std::map<BLR_RULE, std::list<blr_token>> mFirst;

   // FOLLOW
   std::map<BLR_RULE, std::list<blr_token>> mFollow;

   // AST TREE
   blr_ast_node *mASTRoot;

   // Types (AST Trees)
   unsigned int mTypeSpecifierLevel;
   std::list<blr_ast_node *> mASTTypes;
  

};

#endif
