#ifndef BLURRED_LEXER_HPP___
#define BLURRED_LEXER_HPP___

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <memory>

#define MAX_TOKEN_SIZE 256

#define MIN(X,Y) ((X < Y) ? (X) : (Y))

enum BLR_LEXER_STATE {
   blr_l_ok     = 0x0000,
   blr_l_uninit = 0x0001,
   blr_l_error  = 0xF000
};

enum BLR_LEXER_ERROR {
   blr_l_error_unknown = 0x0000,
   blr_l_error_file    = 0x0001, // Input file can't be opened
   blr_l_error_syntax  = 0x0002
};

enum BLR_TOKEN_TYPE {
   // General names
   blr_token_name, 

   // Reserved words
   blr_token_while,
   blr_token_if,
   blr_token_break,

   blr_token_var,
   blr_token_struct,
   blr_token_proc,

   blr_token_bit,
   blr_token_list,
   blr_token_sizeof,
   blr_token_in,

   // Operators
   blr_token_slash,
   blr_token_plus,
   blr_token_minus,
   blr_token_star,
   blr_token_leftpar,
   blr_token_rightpar,
   blr_token_leftbrace,
   blr_token_rightbrace,
   blr_token_leftbracket,
   blr_token_rightbracket,
   blr_token_semicolon,
   blr_token_dot,
   blr_token_dotdot,
   blr_token_equal,
   blr_token_notequal,
   blr_token_greater,
   blr_token_lesser,
   blr_token_greaterequal,
   blr_token_lesserequal,
   blr_token_rightarrow,
   blr_token_not,
   blr_token_or,
   blr_token_and,
   blr_token_xor,
   blr_token_bwnot,
   blr_token_bwor,
   blr_token_bwand,
   blr_token_bwxor,
   blr_token_leftshift,
   blr_token_rightshift,
   blr_token_assignment,
   
   // Numerals
   blr_token_numeral,

   // String Literals
   blr_token_literal
};

struct blr_token {
   BLR_TOKEN_TYPE   mType;
   char            mValue[MAX_TOKEN_SIZE];
};


class blr_lexer {
public:
   explicit blr_lexer() :
      mPayload((char *)0), mPayloadSize(0),
      mState(blr_l_uninit), mError(blr_l_error_unknown) {
      init();
   }

   explicit blr_lexer(std::string filename) :
      mPayload((char *)0), mPayloadSize(0),
      mState(blr_l_uninit), mError(blr_l_error_unknown) { 

      // Read the file into memory
      // ------------------------------------------
      std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
      if (in.fail()) {
         mState = blr_l_error;
         mError = blr_l_error_file;
      }
      else {
         mState = blr_l_ok;
         
         mPayloadSize = in.tellg();

         // std::cout << "File size of : " << filesize << " bytes." << std::endl;

         // return at the begining
         in.seekg(0, in.beg);

         // Allocate mem ory for file
         mPayload = new char [mPayloadSize];

         // Read 
         in.read((char *)mPayload, mPayloadSize);
         in.close();
      }

      init();
   }

   ~blr_lexer() {
      if (mPayload) delete []mPayload;
   }

   bool init() {
      // Token names map
      mTokenTypeNames[blr_token_name]          = "Name";

      // Reserved words
      mTokenTypeNames[blr_token_while]         = "While";  
      mTokenTypeNames[blr_token_break]         = "Break";  
      mTokenTypeNames[blr_token_if]            = "If";  
      mTokenTypeNames[blr_token_var]           = "Var";  
      mTokenTypeNames[blr_token_proc]          = "Proc"; 
      mTokenTypeNames[blr_token_struct]        = "Struct";       
      mTokenTypeNames[blr_token_in]            = "In"; 
      mTokenTypeNames[blr_token_sizeof]        = "Sizeof"; 

      mTokenTypeNames[blr_token_list]          = "List";  
      mTokenTypeNames[blr_token_bit]           = "Bit"; 

      // Operators
      mTokenTypeNames[blr_token_slash]         = "Slash";  
      mTokenTypeNames[blr_token_plus]          = "Plus";  
      mTokenTypeNames[blr_token_minus]         = "Minus"; 
      mTokenTypeNames[blr_token_star]          = "Star";  
      mTokenTypeNames[blr_token_leftpar]       = "Left Parenthese";  
      mTokenTypeNames[blr_token_rightpar]      = "Right Parenthese";  
      mTokenTypeNames[blr_token_leftbrace]     = "Left Brace";  
      mTokenTypeNames[blr_token_rightbrace]    = "Right Brace";  
      mTokenTypeNames[blr_token_leftbracket]   = "Left Bracket";  
      mTokenTypeNames[blr_token_rightbracket]  = "Right Bracket";  
      mTokenTypeNames[blr_token_semicolon]     = "Semicolon";  
      mTokenTypeNames[blr_token_dot]           = "Dot";  
      mTokenTypeNames[blr_token_dotdot]        = "Dot Dot";  
      mTokenTypeNames[blr_token_equal]         = "Equal";  
      mTokenTypeNames[blr_token_notequal]      = "Not Equal";  
      mTokenTypeNames[blr_token_greater]       = "Greater";  
      mTokenTypeNames[blr_token_lesser]        = "Lesser";  
      mTokenTypeNames[blr_token_greaterequal]  = "Greater or Equal";  
      mTokenTypeNames[blr_token_lesserequal]   = "Lesser or Equal";
      mTokenTypeNames[blr_token_rightarrow]    = "Right Arrow";  
      mTokenTypeNames[blr_token_not]           = "Not";  
      mTokenTypeNames[blr_token_or]            = "Or";  
      mTokenTypeNames[blr_token_and]           = "And";  
      mTokenTypeNames[blr_token_xor]           = "Xor";  
      mTokenTypeNames[blr_token_bwnot]         = "Bitwise Not";  
      mTokenTypeNames[blr_token_bwor]          = "Bitwise Or";  
      mTokenTypeNames[blr_token_bwand]         = "Bitwise And";  
      mTokenTypeNames[blr_token_bwxor]         = "Bitwise Xor";  
      mTokenTypeNames[blr_token_leftshift]     = "Left Shift";  
      mTokenTypeNames[blr_token_rightshift]    = "Right Shift";  
      mTokenTypeNames[blr_token_assignment]    = "Assignment";

      // Constant - Numerals
      mTokenTypeNames[blr_token_numeral]       = "Numeral";  

      // String literals
      mTokenTypeNames[blr_token_literal]       = "String Literal";  

      // Reserved words map
      mReservedWords["while"]  = blr_token_while;
      mReservedWords["break"]  = blr_token_break;
      mReservedWords["if"]     = blr_token_if;     
      mReservedWords["var"]    = blr_token_var;
      mReservedWords["proc"]   = blr_token_proc;
      mReservedWords["struct"] = blr_token_struct;
      mReservedWords["in"]     = blr_token_in;
      mReservedWords["sizeof"] = blr_token_sizeof;
      mReservedWords["bit"]    = blr_token_bit;
      mReservedWords["list"]   = blr_token_list;

      // Error message map
      mErrorMessages[blr_l_error_syntax] = "Syntax";
   }

   bool blr_isalpha(const char c) {
      return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c == '_');
   }

   bool blr_isdecdigit(const char c) {
      return (c >= '0' && c <= '9');
   }

   bool blr_ishexdigit(const char c) {
      return blr_isdecdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
   }

   bool blr_readdecnumber(char **pc, blr_token *token) {
      bool rval = true;

      // Token name is set to name - will be changed to reserved if part of the 
      // reserved words
      token->mType = blr_token_numeral;

      unsigned int current_char = 0;

      while ((blr_isdecdigit(**pc) || ('.' == **pc)) && (current_char < MAX_TOKEN_SIZE)) {
         token->mValue[current_char] = **pc;
         ++current_char;
         ++(*pc);
      }

      return rval;
   }

   bool blr_readalpha(char **pc, blr_token *token) {
      bool rval = true;

      // Token name is set to name - will be changed to reserved if part of the 
      // reserved words
      token->mType = blr_token_name;
      
      // Store first caracter
      unsigned int current_char = 1;
      token->mValue[0] = **pc;
      ++(*pc);

      while ((blr_isalpha(**pc) || blr_isdecdigit(**pc)) && (current_char < MAX_TOKEN_SIZE)) {
         token->mValue[current_char] = **pc;
         ++current_char;
         ++(*pc);
      }
      token->mValue[current_char] = 0x00;

      // Try to find a reserved word
      auto it = mReservedWords.find(token->mValue);
      if (it != mReservedWords.end()) {
         token->mType = it->second;
      }
      
      return rval;
   }

   bool blr_readcomment_line(char **pc) {
      bool rval = true;
      
      // Skip the double slash '//'
      ++(*pc);
      ++(*pc);

      while ('\n' != **pc) {
         ++(*pc);
      }
      
      return rval;
   }

   bool blr_readoperator(char **pc, blr_token *token) {
      bool rval = true;      
      
      switch (**pc) {
      case '/':
         token->mType = blr_token_slash;
         ++(*pc);
         break;
 
      case '-':
         token->mType = blr_token_minus;
         ++(*pc);         
         if ('>' == **pc) {
            token->mType = blr_token_rightarrow;
            ++(*pc);
         }
         break;
      
      case '+':
         token->mType = blr_token_plus;
         ++(*pc);
         break;

      case '*':
         token->mType = blr_token_star;
         ++(*pc);         
         break;

      case '(':
         token->mType = blr_token_leftpar;
         ++(*pc);
         break;

      case ')':
         token->mType = blr_token_rightpar;
         ++(*pc);
         break;

      case '{':
         token->mType = blr_token_leftbrace;
         ++(*pc);
         break;

      case '}':
         token->mType = blr_token_rightbrace;
         ++(*pc);
         break;

      case '[':
         token->mType = blr_token_leftbracket;
         ++(*pc);
         break;

      case ']':
         token->mType = blr_token_rightbracket;
         ++(*pc);
         break;

      case ';':
         token->mType = blr_token_semicolon;
         ++(*pc);
         break;

      case '.':
         token->mType = blr_token_dot;
         ++(*pc);
         if ('.' == **pc) {
            token->mType = blr_token_dotdot;
            ++(*pc);
         } 
         break;

      case '~':
         token->mType = blr_token_bwnot;
         ++(*pc);
         break;

      case '=':
         token->mType = blr_token_assignment;
         ++(*pc);
         if ('=' == **pc) {
            token->mType = blr_token_equal;
            ++(*pc);
         }
         break;

      case '!':
         token->mType = blr_token_not;
         ++(*pc);
         if ('=' == **pc) {
            token->mType = blr_token_notequal;
            ++(*pc);
         }
         break;

      case '^':
         token->mType = blr_token_bwxor;
         ++(*pc);
         if ('^' == **pc) {
            token->mType = blr_token_xor;
            ++(*pc);
         }
         break;

      case '&':
         token->mType = blr_token_bwand;
         ++(*pc);
         if ('&' == **pc) {
            token->mType = blr_token_and;
            ++(*pc);
         }
         break;

      case '|':
         token->mType = blr_token_bwor;
         ++(*pc);
         if ('|' == **pc) {
            token->mType = blr_token_or;
            ++(*pc);
         }
         break;

      case '>':
         token->mType = blr_token_greater;
         ++(*pc);
         if ('=' == **pc) {
            token->mType = blr_token_greaterequal;
            ++(*pc);
         }
         break;

      case '<':
         token->mType = blr_token_lesser;
         ++(*pc);
         if ('=' == **pc) {
            token->mType = blr_token_lesserequal;
            ++(*pc);
         }
         break;
      }

      return rval; 
   }

   bool blr_readliteral(char **pc, blr_token *token, unsigned int charsleft) {
      bool rval = true;
      
      // Token name is set to name - will be changed to reserved if part of the 
      // reserved words
      token->mType = blr_token_literal;
      
      // Skip first caracter " 
      unsigned int current_char = 0;
      ++(*pc);

      const unsigned int limit = MIN(MAX_TOKEN_SIZE, charsleft);
      //std::cout << "Chars Left = " << charsleft << std::endl;
      //std::cout << "Chars Limit = " << limit << std::endl;

      while (('"' != **pc) && (current_char < limit)) {
         // If escaped 
         if ('\\' == **pc) {
            ++(*pc);
            if ('t' == **pc) {
               token->mValue[current_char] = '\t';   // Tab            
            }
            else if ('n' == **pc) {
               token->mValue[current_char] = '\n';   // New line
            }
            else if ('r' == **pc) {
               token->mValue[current_char] = '\r';   // Carriage Return
            }
            else if ('"' == **pc) {
               token->mValue[current_char] = '"';    // Double quote
            }
            else if ('\\' == **pc) {
               token->mValue[current_char] = '\\';   // Backslash
            }
         }
         else {
            token->mValue[current_char] = **pc;
         }
         
         ++current_char;
         ++(*pc);
      }

      if ('"' != **pc) {
         mState = blr_l_error;
         mError = blr_l_error_syntax;
         rval = false;
      }
      else {
         // Skip last caracter "
         ++(*pc);
      }


      return rval;       
   }

   bool run() {
      if (blr_l_ok == mState) {

         char *pc = mPayload;
         const char *pe = mPayload + mPayloadSize;

         bool need_new_token = true;
         blr_token *current_token;

         while (pc < pe) {
            std::cout << *pc;
            if (need_new_token) {
               current_token = new blr_token;
               current_token->mValue[0] = 0x00;
               need_new_token = false;
            }

            // If a name
            if (blr_isalpha(*pc)) {
               if (blr_readalpha(&pc, current_token)) {
                  mTokenList.push_back(std::shared_ptr<blr_token>(current_token));
                  need_new_token = true;
               }
            }
            // If a comment
            else if (*pc == '/') {
               // Peek ahead -  
               if (*(pc+1) == '/') {
                  blr_readcomment_line(&pc);
               }
               else {
                  if (blr_readoperator(&pc, current_token)) {
                     mTokenList.push_back(std::shared_ptr<blr_token>(current_token));
                     need_new_token = true;
                  }       
               }
            }
            // If an operator
            else if ((*pc == '*') || (*pc == '+') || (*pc == '-') || 
                     (*pc == '(') || (*pc == ')') || (*pc == '{') || 
                     (*pc == '}') || (*pc == '[') || (*pc == ']') ||
                     (*pc == ';') || (*pc == '.') || (*pc == '<') ||
                     (*pc == '>') || (*pc == '=') || (*pc == '!') ||
                     (*pc == '^') || (*pc == '&') || (*pc == '|')) {
               if (blr_readoperator(&pc, current_token)) {
                  mTokenList.push_back(std::shared_ptr<blr_token>(current_token));
                  need_new_token = true;
               }
            }
            // If a string literal
            else if (*pc == '"') {
               if (blr_readliteral(&pc, current_token, pe-pc)) {
                  mTokenList.push_back(std::shared_ptr<blr_token>(current_token));
                  need_new_token = true;
               }
               else {
                  err();
               }
            }
            // If a number
            else if (blr_isdecdigit(*pc)) {
               if (('0' == *pc) && (('x' == *(pc+1)) || ('X' == *(pc+1)))) {
                  // TODO HEX
               } 
               else {
                  if (blr_readdecnumber(&pc, current_token)) {
                     mTokenList.push_back(std::shared_ptr<blr_token>(current_token));
                     need_new_token = true;
                  }
                  else {
                     err();
                  }
               }
            }
            // It's something else
            else {
               // Advance the pointer  
               ++pc;
            }
         }
      }
      else {
         err();
      }
   }
   
   void output_token_list() {
      std::cout << std::endl;
      std::cout << "TOKEN LIST" << std::endl;
      std::cout << "-------------------------------------------" << std::endl;
      for (auto &a : mTokenList) {
         std::cout << mTokenTypeNames[a->mType] << " : " << a->mValue << std::endl; 
      }
   }

   std::list<std::shared_ptr<blr_token>> *get_token_list_pointer() {
      return &mTokenList;
   }
   
private:
   char        *mPayload;
   unsigned int mPayloadSize;

   BLR_LEXER_STATE mState;
   BLR_LEXER_ERROR mError;

   std::list<std::shared_ptr<blr_token>>  mTokenList;
   std::map<std::string, BLR_TOKEN_TYPE>  mReservedWords;
   std::map<BLR_LEXER_ERROR, std::string> mErrorMessages;

   // This is mostly used for debugging
   std::map<BLR_TOKEN_TYPE, std::string> mTokenTypeNames;

   bool err() {
      std::cout << std::endl;
      std::cout << "Error " << mError << std::endl;
   }
};

#endif
