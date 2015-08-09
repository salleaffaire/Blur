#ifndef BLURRED_AST_HPP___
#define BLURRED_AST_HPP___

#include <memory>
#include <list>

#include <cstdint>
#include <string>

#include "blurred_token.hpp"


enum BLR_TYPE {
   blr_type_bool,
   blr_type_string,
   blr_type_bit,
   blr_type_void
};

class blr_ast_node {
public:
   virtual ~blr_ast_node() {}
};

// Expression AST Nodes
// -------------------------------------------------------------------------------

class blr_ast_node_expression : public blr_ast_node {
public:
};

class blr_ast_node_expression_number : public blr_ast_node_expression {
public:
   int64_t mValue;
   blr_ast_node_expression_number(int64_t value) : mValue(value) {}
};

class blr_ast_node_expression_literal : public blr_ast_node_expression {
public:
   std::string mValue;
   blr_ast_node_expression_literal(std::string value) : mValue(value) {}
};

class blr_ast_node_expression_boolean : public blr_ast_node_expression {
public:
   bool mValue;
   blr_ast_node_expression_boolean(bool value) : mValue(value) {}
};

class blr_ast_node_expression_variable : public blr_ast_node_expression {
public:
   std::string mValue;
   blr_ast_node_expression_variable(std::string value) : mValue(value) {}
};

class blr_ast_node_expression_call : public blr_ast_node_expression {
public:
   std::string mCallee;
   std::vector<blr_ast_node *> mArgs;
   blr_ast_node_expression_call(std::string callee, std::vector<blr_ast_node *> a) : 
      mCallee(callee), mArgs(a) {}   
};

class blr_ast_node_expression_defered_call : public blr_ast_node_expression {
public:
};

class blr_ast_node_expression_binary_op : public blr_ast_node_expression {
public:
   BLR_TOKEN_TYPE mOp;
   blr_ast_node  *mLeftExp;
   blr_ast_node  *mRightExp;;
   blr_ast_node_expression_binary_op(BLR_TOKEN_TYPE op,
                                     blr_ast_node *l, blr_ast_node *r) : 
      mOp(op), mLeftExp(l), mRightExp(r) {}
};

class blr_ast_node_expression_unary_op : public blr_ast_node_expression {
public:
   BLR_TOKEN_TYPE mOp;
   blr_ast_node  *mExp;
   blr_ast_node_expression_unary_op(BLR_TOKEN_TYPE op,
                                    blr_ast_node *e) : 
      mOp(op), mExp(e) {}
};

// Type declaration
// ---------------------------------------------------------------------------------
class blr_ast_node_struct: public blr_ast_node {
public:
   std::string mName;
   std::vector<blr_ast_node *> mDecDefs;
   blr_ast_node_struct(std::string name, std::vector<blr_ast_node *> decdefs) :
      mName(name), mDecDefs(decdefs) {}
};

class blr_ast_node_base_type: public blr_ast_node {
   std::string mName;
   BLR_TYPE    mType;
   blr_ast_node_base_type(std::string name, BLR_TYPE type) :
      mName(name), mType(type) {}
};

class blr_ast_node_array: public blr_ast_node {
   std::string mName;
   blr_ast_node *mOf;
   blr_ast_node_array(std::string name, blr_ast_node *of) :
      mName(name), mOf(of) {}
};

class blr_ast_node_list: public blr_ast_node {
   std::string mName;
   blr_ast_node *mOf;
   blr_ast_node_list(std::string name, blr_ast_node *of) :
      mName(name), mOf(of) {}
};

// Variable declaration
// ---------------------------------------------------------------------------------
class blr_ast_node_variable_declaration: public blr_ast_node {
public:
   std::string mName;
   BLR_TYPE    mType;

   blr_ast_node_variable_declaration(std::string name, BLR_TYPE type) :
      mName(name), mType(type) {}
};


// Function Declaration and Definition AST nodes
// ---------------------------------------------------------------------------------
class blr_ast_node_function_prototype : public blr_ast_node {
public:
   std::string           mName;
   std::vector<BLR_TYPE> mArgType;
};


#endif
