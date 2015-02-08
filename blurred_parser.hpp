#ifndef BLURRED_PARSER_TD_PRED_HPP___
#define BLURRED_PARSER_TD_PRED_HPP___

#include <list>
#include "blurred_lexer.hpp"

class blr_parser {
public:
   blr_parser() : mpTokenList((std::list<std::shared_ptr<blr_token>> *)0) {}
   blr_parser(std::list<std::shared_ptr<blr_token>> *tl) : mpTokenList(tl) {}

   ~blr_parser() {}

   void init(std::list<std::shared_ptr<blr_token>> *tl) {
      mpTokenList = tl;
   }

private:
   std::list<std::shared_ptr<blr_token>> *mpTokenList;
   std::list<std::shared_ptr<blr_token>>::iterator mLookAheadSymbol;
};

#endif
