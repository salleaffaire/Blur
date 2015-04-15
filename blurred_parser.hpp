#ifndef BLURRED_PARSER_TD_PRED_HPP___
#define BLURRED_PARSER_TD_PRED_HPP___

#include <list>
#include "blurred_lexer.hpp"

enum BLR_RULE {
   blr_rule_program,
   blr_rule_declaration_list,
   blr_rule_declaration_rest,
   blr_rule_declaration,
   blr_rule_data_declaration,
   blr_rule_fun_declaration
};

class blr_parser {
public:
   blr_parser() : mpTokenList((std::list<std::shared_ptr<blr_token>> *)0) {}
   blr_parser(std::list<std::shared_ptr<blr_token>> *tl) : mpTokenList(tl) {}

   ~blr_parser() {}

   void init(std::list<std::shared_ptr<blr_token>> *tl) {
      mpTokenList = tl;
      mLookAheadSymbol = mpTokenList->begin();
                                                
      // Calculate FIRST and FOLLOW
      // ------------------------------------------------------
      // 
      
   }
   
   // Run parser (Equivalent to the <program> rule)
   bool run() {
      bool rval = true;

      rval = declaration_list();

      return rval;
   }

   int choose_production(BLR_RULE rule) {
      switch (rule) {
      case blr_rule_declaration_rest:
         break;
      }
   }

   bool declaration_list() {
      bool rval = true;
      
      rval = declaration();
      rval = declaration_rest();

      return rval;
   }

   bool declaration_rest() {
      bool rval = true;

      int pr = choose_production(blr_rule_declaration_rest);
      
      if (0 == pr) {
         rval = declaration();
         rval = declaration_rest();
      }
      else {
         // Do nothing
      }
      
      return rval;      
   }

   bool declaration() {
      bool rval = true;
      
      return rval;
   }
   
   bool match(BLR_TOKEN_TYPE tt) {
      if ((**mLookAheadSymbol).mType == tt) {
         ++mLookAheadSymbol;
      }
   }

private:
   // Pointer to the Lexer's token list
   std::list<std::shared_ptr<blr_token>>           *mpTokenList;

   // Current look-ahead symbol
   std::list<std::shared_ptr<blr_token>>::iterator mLookAheadSymbol;

   // FIRST

   // FOLLOW

};

#endif
