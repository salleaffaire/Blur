#ifndef BLURRED_CORE_HPP___
#define BLURRED_CORE_HPP___

#include <string>
#include <map>
#include "blur_ast.hpp"

class blur_structure {
piubic:
   std::string   mName;
   blr_ast_node *mAST;
};

class blur_core {
public:
   blur_core() {}
   ~blur_core() {
      for (auto &x: mStructureMap) {
	 detele x.second;
      }
   }

private:
   std::map<std::string, blur_structure *> mStructureMap;
};

#endif
