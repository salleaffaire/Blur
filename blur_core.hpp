#ifndef BLURRED_CORE_HPP___
#define BLURRED_CORE_HPP___

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "blur_ast.hpp"
#include "blur_common.hpp"

// Base type - each type derives from this base
// Built in basic types are derived directly from this and only have a size (in
// bytes)
//
// Each type has a type descriptor which is stored in the type descriptor table
// The format of the type descriptor table is the following
// -----------------------------------------------------------------------------------------------
// Basic Types
//
// 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06
// 05 04 03 02 01 00
// -----------------------------------------------------------------------------------------------
// [ size in bytes ] [ index in class function table (CFT) ] [ index in class
// variable table (CVT) ]

// Each object has the following class in memory
//
// 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06
// 05 04 03 02 01 00
// -----------------------------------------------------------------------------------------------
// [ is virtual (0 not)  ] [ pointer to dynamic type table in type desccritor
// table (TDT)        ] [ object ... ]

// NOTES:::
// -----------------------------------------------------------------------------------------------
// [ array depth ] [ depth 0 size ] [ ... ] [ depth n size ]

class blr_core_function {
 public:
 public:
  blr_core_function() {}
  ~blr_core_function() {}
};

class blr_core_member_variable {
 public:
  blr_core_member_variable(std::string type, unsigned int offset)
      : mTypeName(type), mOffset(offset) {}

  std::string mTypeName;
  unsigned int mOffset;
};

// Type Descriptor Table (TDT)
class blr_tdt {
 public:
  blr_tdt() : mTable((uint8_t *)0), mSize(0) {}
  ~blr_tdt() {
    if (mTable) {
      delete mTable;
    }
  }
  uint8_t *mTable;
  uint32_t mSize;
};

// Class descriptors
class blr_type {
 public:
  blr_type() {}
  blr_type(uint32_t size) : mSize(size) {}
  ~blr_type() {
    for (auto &x : mCFT) {
      delete x.second;
    }
    for (auto &x : mCVT) {
      delete x.second;
    }
  }

  // Total size of the object
  uint32_t mSize;

  // Map of member functions
  std::map<std::string, blr_core_function *> mCFT;
  // Map of member variables
  std::map<std::string, blr_core_member_variable *> mCVT;
};

class blr_core {
 public:
  // TDT
  blr_tdt mTDT;

  // Size of 0xFFFFFFFF is undefined
  //
  blr_core() {
    // Pushing all base type to the TypeMap
    mTypeMap["void"] = new blr_type(0);   // 0 byte
    mTypeMap["int8"] = new blr_type(1);   // 1 byte
    mTypeMap["int16"] = new blr_type(2);  // 2 bytes
    mTypeMap["int32"] = new blr_type(4);  // 4 bytes
    mTypeMap["int64"] = new blr_type(8);  // 8 bytes

    mTypeMap["list"] = new blr_type(0);
    mTypeMap["string"] = new blr_type(0);
  }

  ~blr_core() {
    for (auto &x : mTypeMap) {
      delete x.second;
    }
  }

  bool build_tdt() {
    if (mTDT.mTable != 0) {
      delete[] mTDT.mTable;
    }
    uint32_t size = mTypeMap.size() * 2 * 4;
    mTDT.mTable = new uint8_t[size];
    mTDT.mSize = size;

    uint8_t *p = mTDT.mTable;
    for (auto &x : mTypeMap) {
      blr_store_little_endian(p, x.second->mSize);
      p += 4;
      blr_store_little_endian(p, (uint32_t)0);
      p += 4;
    }
  }

  void adjust_offset(unsigned int &offset, unsigned int size) {
    // If not already aligned
    if ((offset & 0x03) != 0) {
      unsigned int remainder_complement = 0x04 - (offset & 0x03);
      if (size > remainder_complement) {
        offset += remainder_complement;
      }
    }
  }

  unsigned int calculate_size(blr_ast_node *node) {
    unsigned int rval = 0;

    if (blr_ast_node_base_type *p =
            dynamic_cast<blr_ast_node_base_type *>(node)) {
      rval = mTypeMap[p->mName]->mSize;
    } else if (blr_ast_node_array *p =
                   dynamic_cast<blr_ast_node_array *>(node)) {
      rval = p->mSizeNumeral * calculate_size(p->mOf);
    }

    return rval;
  }

  std::string get_base_type(blr_ast_node *node) {
    std::string rval = "";
    if (blr_ast_node_base_type *p =
            dynamic_cast<blr_ast_node_base_type *>(node)) {
      rval = p->mName;
    } else if (blr_ast_node_array *p =
                   dynamic_cast<blr_ast_node_array *>(node)) {
      rval = get_base_type(p->mOf);
    } else if (blr_ast_node_list *p = dynamic_cast<blr_ast_node_list *>(node)) {
      rval = get_base_type(p->mOf);
    }
    return rval;
  }

  bool add_type(blr_ast_node *s) {
    bool rval = true;

    blr_type *type = new blr_type;

    std::string class_name;
    unsigned int current_offset = 0;

    // Test if it's indeed a class, it better be but just in case something went
    // horibly wrong
    if (blr_ast_node_class *p = dynamic_cast<blr_ast_node_class *>(s)) {
      // Catch the class name
      class_name = p->mName;
      // For each element of the class (it could be a var, a func or a class)
      for (auto &x : p->mDecDefs) {
        // We have a variable
        if (blr_ast_node_variable_declaration *y =
                dynamic_cast<blr_ast_node_variable_declaration *>(x)) {
          std::string var_name = y->mName;
          // It is of a base type
          if (blr_ast_node_base_type *z =
                  dynamic_cast<blr_ast_node_base_type *>(y->mType)) {
            // Does the type name exist?
            auto it = mTypeMap.find(z->mName);
            if (it != mTypeMap.end()) {
              // Get its size
              unsigned int size = calculate_size(z);
              // There may be a need to align the data
              adjust_offset(current_offset, size);
              type->mCVT[var_name] =
                  new blr_core_member_variable(z->mName, current_offset);
              current_offset += size;
            } else {
              std::cout << "error : " << z->mName << " used in class "
                        << class_name << " does not exist. " << std::endl;
              rval = false;
            }
          }
          // it is an array
          else if (blr_ast_node_array *z =
                       dynamic_cast<blr_ast_node_array *>(y->mType)) {
            // Does the type name exist?
            std::string base_type = get_base_type(z);
            auto it = mTypeMap.find(base_type);
            if (it != mTypeMap.end()) {
              // Get its size
              unsigned int size = calculate_size(z);
              // There may be a need to align the data
              adjust_offset(current_offset, size);
              type->mCVT[var_name] =
                  new blr_core_member_variable(base_type, current_offset);
              current_offset += size;
            } else {
              std::cout << "error : " << base_type << " used in class "
                        << class_name << " does not exist. " << std::endl;
              rval = false;
            }
          }
          // it is an list
          else if (blr_ast_node_list *z =
                       dynamic_cast<blr_ast_node_list *>(y->mType)) {
          }

        }
        // We have a nested class
        else if (blr_ast_node_class *y =
                     dynamic_cast<blr_ast_node_class *>(x)) {
        }
        // We have a member function
        else if (blr_ast_node_function_prototype *y =
                     dynamic_cast<blr_ast_node_function_prototype *>(x)) {
          std::cout << "FOUND FUNCTION!!!! " << std::endl;
        }
      }
    } else {
      std::cout << "Not a class !!!" << std::endl;
      rval = false;
    }

    if (rval) {
      // Adjust the size
      type->mSize = current_offset;
      // Add it to the map
      mTypeMap[class_name] = type;
    }

    return rval;
  }

  bool add_gv(blr_ast_node *node) {
    if (blr_ast_node_variable_definition *y =
            dynamic_cast<blr_ast_node_variable_definition *>(node)) {
      std::cout << "Adding " << y->mName << " of base type ";
      // Does the type name exist?
      std::string base_type = get_base_type(y->mType);
      std::cout << base_type << std::endl;

      auto it = mTypeMap.find(base_type);
      if (it == mTypeMap.end()) {
        std::cout << "Error: type does not exist" << std::endl;
      }
    }
  }

  void output_types() {
    unsigned int count = 0;
    for (auto &x : mTypeMap) {
      std::cout << "Type " << count << ": " << x.first << " of "
                << x.second->mSize << " bytes." << std::endl;

      // Output members
      std::cout << x.second->mCVT.size() << " variable members." << std::endl;
      for (auto &y : x.second->mCVT) {
        std::cout << " * " << y.first << " at offset " << y.second->mOffset
                  << std::endl;
      }
      std::cout << x.second->mCFT.size() << " function members." << std::endl;
      for (auto &y : x.second->mCFT) {
      }
      ++count;
    }
  }

  // All Types
  std::map<std::string, blr_type *> mTypeMap;
};

#endif
