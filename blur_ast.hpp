#ifndef BLURRED_AST_HPP___
#define BLURRED_AST_HPP___

#include <memory>
#include <list>

#include <cstdint>
#include <string>

#include "blur_token.hpp"


enum BLR_TYPE {
   blr_type_bool,
   blr_type_string,
   blr_type_bit,
   blr_type_byte,
   blr_type_int16,
   blr_type_int32,
   blr_type_int64,
   blr_type_void,
   blr_type_dectype
};

class blr_ast_node {
public:
   //blr_ast_node() {}
   virtual ~blr_ast_node() {}
};

// Expression AST Nodes
// -------------------------------------------------------------------------------

class blr_ast_node_expression : public blr_ast_node {
public:
   //blr_ast_node_expression() {}
   virtual ~blr_ast_node_expression() {}
};

class blr_ast_node_expression_integer_number_literal : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_integer_number_literal() {}
   int64_t mValue;
   blr_ast_node_expression_integer_number_literal(int64_t value) : mValue(value) {}
};

class blr_ast_node_expression_float_number_literal : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_float_number_literal() {}
   double mValue;
   blr_ast_node_expression_float_number_literal(int64_t value) : mValue(value) {}
};

class blr_ast_node_expression_string_literal : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_string_literal() {}
   std::string mValue;
   blr_ast_node_expression_string_literal(std::string value) : mValue(value) {}
};

class blr_ast_node_expression_boolean_literal : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_boolean_literal() {}
   bool mValue;
   blr_ast_node_expression_boolean_literal(bool value) : mValue(value) {}
};

class blr_ast_node_expression_variable : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_variable() {}
   std::string mValue;
   blr_ast_node_expression_variable(std::string value) : mValue(value) {}
};

class blr_ast_node_expression_call : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_call() {
      for (auto &x: mArgs) {
         delete x;
      }
   }
   std::string mCallee;
   std::vector<blr_ast_node *> mArgs;
   blr_ast_node_expression_call(std::string callee, std::vector<blr_ast_node *> a) : 
      mCallee(callee), mArgs(a) {}
};

class blr_ast_node_expression_call_member : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_call_member() {
      for (auto &x: mArgs) {
         delete x;
      }
      delete mCalledOn;
   }
   std::string mCallee;
   blr_ast_node *mCalledOn;
   std::vector<blr_ast_node *> mArgs;
   blr_ast_node_expression_call_member(std::string callee, blr_ast_node *calledon, std::vector<blr_ast_node *> a) : 
      mCallee(callee), mCalledOn(calledon), mArgs(a) {}
};


class blr_ast_node_expression_defered_call : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_defered_call() {
      delete mCall;
   }
   blr_ast_node *mCall;
   blr_ast_node_expression_defered_call(blr_ast_node * call) : 
      mCall(call) {}  
};

class blr_ast_node_expression_binary_op : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_binary_op() {
      delete mLeftExp;
      delete mRightExp;
   }
   BLR_TOKEN_TYPE mOp;
   blr_ast_node  *mLeftExp;
   blr_ast_node  *mRightExp;
   blr_ast_node_expression_binary_op(BLR_TOKEN_TYPE op,
                                     blr_ast_node *l, blr_ast_node *r) : 
      mOp(op), mLeftExp(l), mRightExp(r) {}
};

class blr_ast_node_expression_unary_op : public blr_ast_node_expression {
public:
   virtual ~blr_ast_node_expression_unary_op() {
      delete mExp;
   }
   BLR_TOKEN_TYPE mOp;
   blr_ast_node  *mExp;
   blr_ast_node_expression_unary_op(BLR_TOKEN_TYPE op,
                                    blr_ast_node *e) : 
      mOp(op), mExp(e) {}
};

class blr_ast_node_expression_complex : public blr_ast_node {
public:
   virtual ~blr_ast_node_expression_complex() {
      for (auto &x: mExpressionList) {
	 delete x;
      }
   } 
   std::vector<blr_ast_node *> mExpressionList;
   
   blr_ast_node_expression_complex(std::vector<blr_ast_node *> explist) : 
      mExpressionList(explist) {}
};

// Type declaration
// ---------------------------------------------------------------------------------
class blr_ast_node_struct: public blr_ast_node {
public:
   virtual ~blr_ast_node_struct() {
      for (auto &x: mDecDefs) {
         delete x;
      }   
   }
   std::string mName;
   std::vector<blr_ast_node *> mDecDefs;
   blr_ast_node_struct(std::string name, std::vector<blr_ast_node *> decdefs) :
      mName(name), mDecDefs(decdefs) {}
};

class blr_ast_node_base_type: public blr_ast_node {
public:
   virtual ~blr_ast_node_base_type() {   
   }   
   std::string mName;
   BLR_TYPE    mType;
   blr_ast_node_base_type(std::string name, BLR_TYPE type) :
      mName(name), mType(type) {}
};

class blr_ast_node_array: public blr_ast_node {
public:
   virtual ~blr_ast_node_array() {
      delete mOf;
   }
   std::string mSize;
   blr_ast_node *mOf;
   blr_ast_node_array(std::string size) :
      mSize(size) {}
};

class blr_ast_node_list: public blr_ast_node {
public:
   virtual ~blr_ast_node_list() {
      delete mOf;
   }
   std::string mName;
   blr_ast_node *mOf;
   blr_ast_node_list(std::string name, blr_ast_node *of) :
      mName(name), mOf(of) {}
};

// Variable declaration
// ---------------------------------------------------------------------------------
class blr_ast_node_variable_declaration: public blr_ast_node {
public:
   virtual ~blr_ast_node_variable_declaration() {
      delete mType;
   }
   std::string   mName;
   blr_ast_node *mType;

   blr_ast_node_variable_declaration(std::string name, blr_ast_node *type) :
      mName(name), mType(type) {}
};

// Variable definition
// ---------------------------------------------------------------------------------
class blr_ast_node_variable_definition: public blr_ast_node {
public:
   virtual ~blr_ast_node_variable_definition() {
      delete mCond;
      delete mType;
   }
   std::string   mName;
   blr_ast_node *mType;
   blr_ast_node *mCond;

   blr_ast_node_variable_definition(std::string name, blr_ast_node *type, blr_ast_node *cond) :
      mName(name), mType(type), mCond(cond) {}
};



// Function Declaration 
// ---------------------------------------------------------------------------------
class blr_ast_node_function_prototype : public blr_ast_node {
public:
   virtual ~blr_ast_node_function_prototype() {
      delete mStatement;
      delete mReturnType;
      for (auto &x: mParamTypes) {
	 delete x;
      }
   }
   std::string                  mName;
   std::vector<std::string>     mMemberOf;
   std::vector<blr_ast_node *>  mParamTypes;
   blr_ast_node                *mReturnType;
   blr_ast_node                *mStatement;

   blr_ast_node_function_prototype(std::string name, std::vector<std::string> memberof, 
				   std::vector<blr_ast_node *> paramtypes, blr_ast_node *rt, blr_ast_node *st) :
      mName(name), mMemberOf(memberof), mParamTypes(paramtypes), mReturnType(rt), mStatement(st) {}
   
};

// Statements 
// --------------------------------------------------------------------------------- 
class blr_ast_node_statement : public blr_ast_node {
public:
   virtual ~blr_ast_node_statement() {}
};

class blr_ast_node_statement_if : public blr_ast_node_statement {
public:
   virtual ~blr_ast_node_statement_if() {
      delete mCondition;
      delete mIfStatement;
      delete mElseStatement;
   }
   
   blr_ast_node *mCondition;
   blr_ast_node *mIfStatement;
   blr_ast_node *mElseStatement;

   blr_ast_node_statement_if(blr_ast_node *cond, blr_ast_node *ifs, blr_ast_node *elses) :
      mCondition(cond), mIfStatement(ifs), mElseStatement(elses) {}
};

class blr_ast_node_statement_while : public blr_ast_node_statement {
public:
   virtual ~blr_ast_node_statement_while() {
      delete mCondition;
      delete mLoopStatement;
   }
   
   blr_ast_node *mCondition;
   blr_ast_node *mLoopStatement;


   blr_ast_node_statement_while(blr_ast_node *cond, blr_ast_node *loopstatement) :
      mCondition(cond), mLoopStatement(loopstatement) {}
};

class blr_ast_node_statement_for : public blr_ast_node_statement {
public:
   virtual ~blr_ast_node_statement_for() {
      delete mInit;
      delete mCondition;
      delete mUpdate;
      delete mLoopStatement;
   }
   
   blr_ast_node *mInit;
   blr_ast_node *mCondition;
   blr_ast_node *mUpdate;
   blr_ast_node *mLoopStatement;


   blr_ast_node_statement_for(blr_ast_node *init, blr_ast_node *cond, blr_ast_node *update, blr_ast_node *loopstatement) :
      mInit(init), mCondition(cond), mUpdate(update), mLoopStatement(loopstatement) {}
};

class blr_ast_node_statement_foreach : public blr_ast_node_statement {
public:
   virtual ~blr_ast_node_statement_foreach() {;
      delete mLoopStatement;
   }
   
   std::string   mElement;
   std::string   mContainer;
   blr_ast_node *mLoopStatement;


   blr_ast_node_statement_foreach(std::string elem, std::string container,  blr_ast_node *loopstatement) :
      mElement(elem), mContainer(container), mLoopStatement(loopstatement) {}
};

class blr_ast_node_statement_break : public blr_ast_node_statement {
public:
   virtual ~blr_ast_node_statement_break() {
   }

   blr_ast_node_statement_break() {}
};

class blr_ast_node_statement_return : public blr_ast_node_statement {
public:
   virtual ~blr_ast_node_statement_return() {
      delete mRval;
   }
   blr_ast_node *mRval;
   
   blr_ast_node_statement_return(blr_ast_node *rval) : mRval(rval) {}
};

class blr_ast_node_statement_compound : public blr_ast_node_statement {
public:
   virtual ~blr_ast_node_statement_compound() {
      for (auto &x: mStatementList) {
         delete x;
      }        
   }
   std::vector<blr_ast_node *> mStatementList;
   
   blr_ast_node_statement_compound(std::vector<blr_ast_node *> statementlist) :
      mStatementList(statementlist) {}
};

#endif
