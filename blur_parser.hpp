#ifndef BLURRED_PARSER_TD_PRED_HPP___
#define BLURRED_PARSER_TD_PRED_HPP___

#include <list>
#include <stdlib.h>
#include "blur_lexer.hpp"
#include "blur_ast.hpp"

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

   // ------------------------------------------------------------------------------------
   // AST tree parsing and building
   // ------------------------------------------------------------------------------------

   // Compiler entry point 
   // <program> is a list of declararions and definitions
   // ------------------------------------------------------------------------------------
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
         rval = struct_declaration();
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

   blr_ast_node *decdef_struct() {
      blr_ast_node *rval = 0;
      if (mVerbose) {
         std::cout << "<decdef_struct>" << std::endl;
      }

      if (isnext(blr_token_struct)) {
         rval = struct_declaration();
      }
      else if (isnext(blr_token_var)) {
         rval = data_definition();
      }
      else if (isnext(blr_token_func)) {
         fun_declaration();
      }
      else {
         mState = false;
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <decdef_struct>" << std::endl;
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
      blr_ast_node *rval;

      if (mVerbose) {
         std::cout << "<struct_declaration>" << std::endl;
      }
      
      if (mState) { mState = match(blr_token_struct); }
      if (mState) { mState = match(blr_token_name); }
      if (mState) { 
	 name = mLastMatchedValue;
	 // Set the structure depth - keeping track of scope 
	 mStructureDepth.push_back(name);
      }
      if (mState) { mState = match(blr_token_leftbrace); }
      if (mState) { 
         do {
            if (mState) { 
               rval = decdef_struct();
               if (mState) { decdefs.push_back(rval); }
            }
            
         } while (mState && !IsEnd() && !isnext(blr_token_rightbrace));
      }
      if (mState) { mState = match(blr_token_rightbrace); }

      if (!mState) {
         error();
      }
      else {
         rval = new blr_ast_node_struct(name, std::move(decdefs));
         mASTStructs.push_back(rval);
	 mStructureDepth.pop_back();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <struct_declaration>" << std::endl;
      }
      return rval;
   }

   blr_ast_node *data_declaration() {
      blr_ast_node *rval = 0;
      blr_ast_node *type = 0;
      std::string name;

      if (mVerbose) {
         std::cout << "<data_declaration>" << std::endl;
      }

      if (match(blr_token_var)) {
         if (mState) { type = type_specifier(); }
         if (mState) { 
            mState = match(blr_token_name);
            if (mState) { name = mLastMatchedValue; }
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
      else {
         rval = new blr_ast_node_variable_declaration(name, type);
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <data_declaration>" << std::endl;
      }
      return rval;
   }

   blr_ast_node *data_definition() {
      blr_ast_node *rval = 0;
      blr_ast_node *cond = 0;
      blr_ast_node *type = 0;
      std::string name;

      if (mVerbose) {
         std::cout << "<data_definition>" << std::endl;
      }

      if (match(blr_token_var)) {
         if (mState) { type = type_specifier(); }
         if (mState) { 
            mState = match(blr_token_name);
            if (mState) { name = mLastMatchedValue; }
         }

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
      else {
         rval = new blr_ast_node_variable_definition(name, type, cond);
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <data_definition>" << std::endl;
      }
      return rval;
   }
  
   // The grammar is written so that the function declaration takes only one statement following the argument list
   // The use of {} to group statement is made possible because { statement list } is defined as a compound
   // statement.
   blr_ast_node *fun_declaration() {
      blr_ast_node *rval = 0;
      
      std::string name;
      std::vector<blr_ast_node *> paramtypes;
      blr_ast_node *returntype;
      blr_ast_node *body;
      
      if (mVerbose) {
         std::cout << "<fun_declaration>" << std::endl;
      }
      
      if (mState) { mState = match(blr_token_func); }
      if (mState) { returntype = type_specifier(); }
      if (mState) { mState = match(blr_token_name); }
      name = mLastMatchedValue;
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { paramtypes = params(); }
      if (mState) { mState = match(blr_token_rightpar); }
      //if (mState) { mState = match(blr_token_leftbrace); }
      if (mState) { body = statement(); }
      //if (mState) { mState = match(blr_token_rightbrace); }
      
      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_function_prototype(name, mStructureDepth, paramtypes, returntype, body);
	 mASTFunctions.push_back(rval);
      }

      if (mVerboseDebug) {
         std::cout << "EXIT: <fun_declaration>" << std::endl;
      }
      return rval;
   }
   
   std::vector<blr_ast_node *> params() {
      std::vector<blr_ast_node *> rval;
      if (mVerbose) {
         std::cout << "<params>" << std::endl;
      }
      
      // We used this seperation between <params> and <param_list> to test for an empty list 
      // Like this: () 
      if (mState) { 
         if (isnext(blr_token_rightpar)) {} 
         else { rval = param_list(); }
      }
      
      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <params>" << std::endl;
      }
      return rval;
   }
   
   std::vector<blr_ast_node *> param_list() {
      std::vector<blr_ast_node *> rval;
      if (mVerbose) {
         std::cout << "<param-list>" << std::endl;
      }
      
      do {
         if (mState) { rval.push_back(type_specifier()); }
         if (mState) { mState = match(blr_token_name); }
      } while (mState && match(blr_token_comma));
               
      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <param-list>" << std::endl;
      }
      return rval;
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
            rval = new blr_ast_node_base_type(mLastMatchedValue, blr_type_dectype);
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

   blr_ast_node *expression() {
      blr_ast_node *rval = 0;
      if (mVerbose) {
         std::cout << "<expression>" << std::endl;
      }
      
      // Is it am assignement ?
      if (isnext(blr_token_name) && issecondnext(blr_token_assignment)) {
         if (mState) { mState = match(blr_token_name); }
	 blr_ast_node *left = new blr_ast_node_expression_variable(mLastMatchedValue);
         if (mState) { mState = match(blr_token_assignment); }
         if (mState) { rval = expression(); }
	 rval = new blr_ast_node_assignment(left, rval);
      }
      else {
         // Here we take a guess but it really can't be anything else
         if (mState) { rval = complex_expression(); }
      }
   
      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <expression>" << std::endl;
      }
      return rval;
   }

   blr_ast_node *complex_expression() {
      blr_ast_node *rval;
      std::string callee;

      if (mVerbose) {
         std::cout << "<complex_expression>" << std::endl;
      } 

      if (mState) { rval = simple_expression(); }
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
      return rval; 
   }

   blr_ast_node *simple_expression() {
      blr_ast_node *rval;

      if (mVerbose) {
         std::cout << "<simple_expression>" << std::endl;
      } 
      
      if (mState) { rval = simple_term(); }
      
      while (mState && matchbin1()) {
	 if (mVerbose) {
	    std::cout << "Binary Operator" << std::endl;
	 } 
         if (mState) { 
	    rval = new blr_ast_node_expression_binary_op(mLastMatchedTokenType, rval, simple_term());
	 } 
      }

      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <simple_expression>" << std::endl;
      } 
      return rval;
   }

   blr_ast_node *simple_term() {
      blr_ast_node *rval = 0;
      if (mVerbose) {
         std::cout << "<simple_term>" << std::endl;
      } 
      
      if (mState) { rval = simple_factor(); }

      while (mState && matchbin2()) {
	 if (mVerbose) {
	    std::cout << "Binary Operator" << std::endl;
	 } 	 
         if (mState) { 
	    rval = new blr_ast_node_expression_binary_op(mLastMatchedTokenType, rval, simple_factor());
	 } 
      }

      if (!mState) {
         error();
      }

      if (mVerboseDebug) {
         std::cout << "EXIT : <simple_term>" << std::endl;
      }
      return rval;
   }
   
   blr_ast_node *simple_factor() {
      blr_ast_node *rval;
      if (mVerbose) {
         std::cout << "<simple_factor>" << std::endl;
      } 

      if (mState) {
	 // Unary Operators
         if (matchun()) {
	    if (mVerbose) {
	       std::cout << "Unary Operator" << std::endl;
	    } 
	    if (mState) { 
	       rval = new blr_ast_node_expression_unary_op(mLastMatchedTokenType, simple_term());
	    }
         }
	 // Function Call
         else if (isnext(blr_token_name) && issecondnext(blr_token_leftpar)) {
            if (mState) { rval = call(); }
         }
         else if (match(blr_token_name))    {
	    rval = new blr_ast_node_expression_variable(mLastMatchedValue);
	 }
         else if (match(blr_token_true))    {
	    rval = new blr_ast_node_expression_boolean(true);
	 }
         else if (match(blr_token_false))   {
	    rval = new blr_ast_node_expression_boolean(false);
	 }
         else if (match(blr_token_numeral)) {
	    rval = new blr_ast_node_expression_number(atoi(mLastMatchedValue.c_str()));
	 }
         else if (match(blr_token_literal)) {
	    rval = new blr_ast_node_expression_literal(mLastMatchedValue);
	 }
         else if (match(blr_token_leftpar)) {
            if (mState) { rval = complex_expression(); }
            if (mState) { match(blr_token_rightpar); }
         }
         else if (match(blr_token_rightarrow)) {
            if (mState) { rval = defered_call(); }
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

      return rval;
   }

   blr_ast_node *defered_call() {
      blr_ast_node *rval = 0;
      if (mVerbose) {
         std::cout << "<defered-call>" << std::endl;
      }

      if (mState) { rval = call(); }
      
      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_expression_defered_call(rval);
      }
      return rval;
   }
  
   blr_ast_node *call() {
      blr_ast_node *rval = 0;
      std::string name;
      std::vector<blr_ast_node *> a;
      if (mVerbose) {
         std::cout << "<call>" << std::endl;
      }

      if (mState) { mState = match(blr_token_name); }
      name = mLastMatchedValue;
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { a = args(); }
      if (mState) { mState = match(blr_token_rightpar); }      
      
      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_expression_call(name, a);
      }
      return rval;
   }

   std::vector<blr_ast_node *> args() {
      std::vector<blr_ast_node *> rval;

      if (mVerbose) {
         std::cout << "<args>" << std::endl;
      }
      
      // We used this seperation between <args> and <args_list> to test for an empty list 
      // Like this: ()
      if (mState && !isnext(blr_token_rightpar)) {
         if (mState) { rval = args_list(); }
      }
      
      if (!mState) {
         error();
      }
      return rval;
   }
   
   std::vector<blr_ast_node *> args_list() {
      // Returning the vector may be a bit slow, it'll need to be optimized later
      // For now I just want to have it working
      std::vector<blr_ast_node *> rval;
      blr_ast_node *curr_arg;
      if (mVerbose) {
         std::cout << "<args-list>" << std::endl;
      }
      
      do {
         if (mState) { curr_arg = complex_expression(); }
	 if (mState) { rval.push_back(curr_arg); }
      } while (mState && match(blr_token_comma));
      
      if (!mState) {
         error();
      }
      return rval;
   }


   blr_ast_node *statement() {
      blr_ast_node *rval = 0;
      if (mVerbose) {
         std::cout << "<statement>" << std::endl;
      }

      if (mState) {
         if (isnext(blr_token_var)) {
            if (mState) { rval = data_definition(); }
         }
         else if (isnext(blr_token_if)) {
            if (mState) { rval = if_statement(); }
         }
         else if (isnext(blr_token_break)) {
            if (mState) { rval = break_statement(); }
         }
         else if (isnext(blr_token_return)) {
            if (mState) { rval = return_statement(); }
         }
         else if (isnext(blr_token_leftbrace)) {
            if (mState) { rval = compound_statement(); }
         }
         else if (isnext(blr_token_while)) {
            if (mState) { rval = while_statement(); }
         }
         else if (isnext(blr_token_foreach)) {
            if (mState) { rval = foreach_statement(); }
         }
         else if (isnext(blr_token_for)) {
            if (mState) { rval = for_statement(); }
         }
         else {
            // Try an expression statement
            if (mState) { rval = expression_statement(); }
         }
      }

      if (!mState) {
         error();
      }
      return rval;
   }

   blr_ast_node *expression_statement() {
      blr_ast_node *rval = 0;
      if (mVerbose) {
         std::cout << "<expression_statement>" << std::endl;
      }

      while (mState && !match(blr_token_semicolon)) {
         if (mState) { rval = expression(); }
      }

      if (!mState) {
         error();
      }
      return rval;
   }

   // if
   blr_ast_node *if_statement() {
      blr_ast_node *rval = 0;
      blr_ast_node *condition = 0;
      blr_ast_node *ifstatement = 0;
      blr_ast_node *elsestatement = 0;
      
      if (mVerbose) {
         std::cout << "<if_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_if); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { condition = expression(); }
      if (mState) { mState = match(blr_token_rightpar); }
      if (mState) { ifstatement = statement(); }
      if (match(blr_token_else)) {
         if (mState) { elsestatement = statement(); }
      }

      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_statement_if(condition, ifstatement, elsestatement);
      }
      return rval;
   }
   
   // for
   blr_ast_node *for_statement() {
      blr_ast_node *rval = 0;

      blr_ast_node *init = 0;
      blr_ast_node *condition = 0;
      blr_ast_node *update = 0;
      blr_ast_node *loopstatement = 0;

      if (mVerbose) {
         std::cout << "<for_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_for); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { init = expression(); }
      if (mState) { mState = match(blr_token_semicolon); }
      if (mState) { condition = expression(); }
      if (mState) { mState = match(blr_token_semicolon); }
      if (mState) { update = statement(); }
      if (mState) { mState = match(blr_token_rightpar); }
      if (mState) { loopstatement = statement(); }

      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_statement_for(init, condition, update, loopstatement);
      }

      return rval;
   }

   // while
   blr_ast_node *while_statement() {
      blr_ast_node *rval = 0;
      blr_ast_node *cond;
      blr_ast_node *loopstatement;
      
      if (mVerbose) {
         std::cout << "<while_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_while); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { cond = expression(); }
      if (mState) { mState = match(blr_token_rightpar); }
      if (mState) { loopstatement = statement(); }
      
      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_statement_while(cond, loopstatement);
      }
      return rval;
   }

   // foreach 
   blr_ast_node *foreach_statement() {
      blr_ast_node *rval = 0;

      std::string element = 0;
      std::string container = 0;
      blr_ast_node *loopstatement = 0;

      if (mVerbose) {
         std::cout << "<foreach_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_foreach); }
      if (mState) { mState = match(blr_token_leftpar); }
      if (mState) { mState = match(blr_token_name); }
      element = mLastMatchedValue;
      if (mState) { mState = match(blr_token_in); }
      if (mState) { mState = match(blr_token_name); }
      container = mLastMatchedValue;
      if (mState) { loopstatement = statement(); }

      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_statement_foreach(element, container, loopstatement);
      }
      return rval;
   }

   // break
   blr_ast_node *break_statement() {
      blr_ast_node *rval;
      if (mVerbose) {
         std::cout << "<break_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_break); }
      if (mState) { mState = match(blr_token_semicolon); }

      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_statement_break(); 
      }
      
      return rval;
   }

   // return
   blr_ast_node *return_statement() {
      blr_ast_node *rval;

      if (mVerbose) {
         std::cout << "<return_statement>" << std::endl;
      }

      if (mState) { mState = match(blr_token_return); }
      if (!isnext(blr_token_semicolon)) {
         rval = expression();
      }
      if (mState) { mState = match(blr_token_semicolon); }
      
      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_statement_return(rval);
      }
      
      return rval;
   }

   // List of statement in between 2 braces 
   // like 
   // {
   //   statement 1
   //   statement 2
   //   ...
   //   statement n
   // }

   blr_ast_node *compound_statement() {
      blr_ast_node *rval = 0;
      std::vector<blr_ast_node *> statementlist;
      if (mVerbose) {
         std::cout << "<compound_statement>" << std::endl;
      }

      // Open Brace {
      if (mState) { mState = match(blr_token_leftbrace); } 
      
      while (mState && !isnext(blr_token_rightbrace)) {
         statementlist.push_back(statement());
      }
      
      // Close Brace }
      if (mState) { mState = match(blr_token_rightbrace); } 

      if (!mState) {
         error();
      }
      else {
	 rval = new blr_ast_node_statement_compound(statementlist);
      }
      
      return rval;
   }

   // ------------------------------------------------------------------------------------
   // Below functions are parser helper functions
   // ------------------------------------------------------------------------------------

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
	 mLastMatchedTokenType = (**mLookAheadSymbol).mType;
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
	 mLastMatchedTokenType = (**mLookAheadSymbol).mType;
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
	 mLastMatchedTokenType = (**mLookAheadSymbol).mType;
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
         else if (p->mType == blr_type_dectype) {
            std::cout << p->mName << std::endl;
         }
      }
      // We have an array
      else if (blr_ast_node_array *p = dynamic_cast<blr_ast_node_array *>(x)) {
         std::cout << "Array of ";
         output_type_name(p->mOf);
      }
      // We have a list
      else if (blr_ast_node_list *p = dynamic_cast<blr_ast_node_list *>(x)) {
         std::cout << "List of ";
         output_type_name(p->mOf);
      }
      // We have a member function
      else  {
         std::cout << "Func" << std::endl;
      }
   }

   void output_struct(blr_ast_node *x) {
      std::cout << "struct {" << std::endl;
      for (auto &y: dynamic_cast<blr_ast_node_struct *>(x)->mDecDefs) {
         //std::cout << x << std::endl;
         // We have a struct
         if (blr_ast_node_struct *p = dynamic_cast<blr_ast_node_struct *>(y)) {
            output_struct(p);
         }
         // We have a var
         else if (blr_ast_node_variable_definition *p = 
                  dynamic_cast<blr_ast_node_variable_definition *>(y)) {
            std::cout << "var " << p->mName << " -> ";
            output_type_name(p->mType);
            if (p->mCond) {
               // Output condition
            }
         }
         // We have a struct
         else  {
            std::cout << "func" << std::endl;
         }
      }
      std::cout << "}" << std::endl;
   }

   void output_function(blr_ast_node *x) {
      if (blr_ast_node_function_prototype *p = dynamic_cast<blr_ast_node_function_prototype *>(x)) {
	 std::cout << "func ";
	 for (auto &s: p->mMemberOf) {
	    std::cout << s << "::";
	 }
	 std::cout << p->mName;
	 std::cout << std::endl;
      }
   }

   void output_all_types() {
      std::cout << "----------------------------------------------------------------" << std::endl;
      std::cout << "Output " << mASTTypes.size() << " type(s)" << std::endl;
      for (auto &x: mASTTypes) {
         output_type_name(x);
      }
      std::cout << std::endl;
   } 

   void output_all_structs() {
      std::cout << "----------------------------------------------------------------" << std::endl;
      std::cout << "Output " << mASTStructs.size() << " struct(s)" << std::endl;
      for (auto &x: mASTStructs) {
         output_struct(x);
      }
      std::cout << std::endl;
   }

   void output_all_functions() {
      std::cout << "----------------------------------------------------------------" << std::endl;
      std::cout << "Output " << mASTFunctions.size() << " function(s)" << std::endl;
      for (auto &x: mASTFunctions) {
         output_function(x);
      }
      std::cout << std::endl;
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

   // Keep the last matched Value
   std::string mLastMatchedValue;

   // Keep the last matched Type when the token is a type 
   BLR_TOKEN_TYPE mLastMatchedTokenType;

   // Keep track of the nested structures  
   std::vector<std::string>  mStructureDepth;

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

   // Structs (AST Trees)
   std::list<blr_ast_node *> mASTStructs;

   // Functions (AST Trees)
   std::list<blr_ast_node *> mASTFunctions;

};

#endif
