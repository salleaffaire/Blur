#ifndef BLURRED_AST_HPP___
#define BLURRED_AST_HPP___

#include <memory>
#include <list>

enum blr_ast_type {
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
};

class blr_ast_node_expression_literal : public blr_ast_node_expression {
public:
};

class blr_ast_node_expression_boolean : public blr_ast_node_expression {
public:
};

class blr_ast_node_expression_variable : public blr_ast_node_expression {
public:
};

class blr_ast_node_expression_call : public blr_ast_node_expression {
public:
};

class blr_ast_node_expression_binary_op : public blr_ast_node_expression {
public:
};

class blr_ast_node_expression_unary_op : public blr_ast_node_expression {
public:
};


#endif
