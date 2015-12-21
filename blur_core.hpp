#ifndef BLURRED_CORE_HPP___
#define BLURRED_CORE_HPP___

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "blur_ast.hpp"


class blr_type {
public:
   blr_type(unsigned int size) : mSize(size) {}
   unsigned int mSize;
};

class blr_core_structure_variable {
public:
   blr_core_structure_variable(std::string type, unsigned int offset, unsigned int size) :
      mType(type), mOffset(offset), mSize(size) {}
   std::string  mType;
   unsigned int mOffset;
   unsigned int mSize;
};

class blr_core_structure_function {
public:
};

class blr_core_structure : public blr_type {
public:
   blr_core_structure() : blr_type(0) {

   }
   ~blr_core_structure() {
      for (auto &x: mVariableMap) {
	 delete x.second;
      }
      for (auto &x: mFunctionMap) {
	 delete x.second;
      }   
   }
   
   // Add a variable to the structure - this function is called from add_structure
   // in blr_core
   void add_variable(std::string name, blr_core_structure_variable *var) {
      mVariableMap[name] = var;
   }
   void add_function(std::string name, blr_core_structure_function *fun) {
      mFunctionMap[name] = fun;
   }

   // Variable Map <Variable Name -> Structure Variable>
   std::map<std::string, blr_core_structure_variable *> mVariableMap;

   // Function Map <Variable Name -> Structure Variable>
   std::map<std::string, blr_core_structure_function *> mFunctionMap;
   
};

class blr_core {
public:
   // Size of 0xFFFFFFFF is undefined
   // 
   blr_core() {
      mTypeMap["void"]   = new blr_type(0);           // 0 byte
      mTypeMap["bit"]    = new blr_type(1);           // 1 byte  
      mTypeMap["int16"]  = new blr_type(2);           // 2 bytes
      mTypeMap["int32"]  = new blr_type(4);           // 4 bytes
      mTypeMap["int64"]  = new blr_type(8);           // 8 bytes
      mTypeMap["string"] = new blr_type(0xFFFFFFFF);  // undef 
      mTypeMap["bool"]   = new blr_type(1);           // 1 byte
   }

   ~blr_core() {
      for (auto &x: mTypeMap) {
	 delete x.second;
      }
      for (auto &x: mStructureMap) {
	 delete x.second;
      }
   }

   bool add_structure(blr_ast_node *s) {
      bool rval = true; 
      blr_core_structure *bcs = new blr_core_structure;
      std::string name;
      unsigned int current_offset = 0;
      
      if (blr_ast_node_struct *p = dynamic_cast<blr_ast_node_struct *>(s)) {
	 name = p->mName;
	 for (auto &x: p->mDecDefs) {
	    // We have a var
	    if (blr_ast_node_variable_definition *y = dynamic_cast<blr_ast_node_variable_definition *>(x)) {
	       std::string type = "";
	       type_specifier_name(y->mType, type);
	       blr_core_structure_variable *bcsv = new blr_core_structure_variable(type, 0, 0);
	       bcs->add_variable(y->mName, bcsv);
	    }
	    // We have a nested structure
	    else if (blr_ast_node_struct *y = dynamic_cast<blr_ast_node_struct *>(x)) {
	       
	    }
	    // We have a member function
	    else if (blr_ast_node_function_prototype *y = dynamic_cast<blr_ast_node_function_prototype *>(x)) {
	       std::cout << "FOUND FUNCTION!!!! " << std::endl;
	       bcs->add_function(y->mName, new blr_core_structure_function);
	    }
	 }
      }
      else {
	 std::cout << "Not a Structure" << std::endl;
	 rval = false;
      }

      mStructureMap[name] = bcs;

      return rval;
   } 

   std::string type_specifier_name(blr_ast_node *x, std::string &tsname) {
      if (blr_ast_node_base_type *p = dynamic_cast<blr_ast_node_base_type *>(x)) {
	 tsname += p->mName;
      }
      // We have an array
      else if (blr_ast_node_array *p = dynamic_cast<blr_ast_node_array *>(x)) {
         type_specifier_name(p->mOf, tsname);
	 tsname += "[";
	 tsname += p->mSize;
	 tsname += "]";
      }
      // We have a list
      else if (blr_ast_node_list *p = dynamic_cast<blr_ast_node_list *>(x)) {
	 std::string nullstr = "";
	 tsname = "list{" + type_specifier_name(p->mOf, nullstr) + "}";
      }
      return tsname;
   }

   void output_structures() {
      for (auto &x: mStructureMap) {
	 std::cout << "Structure : " << x.first;
	 std::cout << std::endl;
	 for (auto &y: (x.second)->mVariableMap) {
	    std::cout << " Variable : " << y.first << " of type " << (y.second)->mType << std::endl; 	    
	 }
	 for (auto &y: (x.second)->mFunctionMap) {
	    std::cout << " Function : " << y.first; 	    
	 }
	 std::cout << std::endl;
      }
   }

   // Base Types
   std::map<std::string, blr_type *> mTypeMap;


   // Declared Types
   std::map<std::string, blr_core_structure *> mStructureMap;
   
};

#endif
