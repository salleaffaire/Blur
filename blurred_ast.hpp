#ifndef BLURRED_AST_HPP___
#define BLURRED_AST_HPP___

#include <memory>
#include <list>

#include "blurred_token.hpp"

class blr_ast_node {
public:
   std::list<std::unique_ptr<blr_ast_node *>> mChidren;
   blr_token                                 *mpToken;
};


#endif
