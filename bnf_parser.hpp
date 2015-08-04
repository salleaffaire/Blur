#ifndef BNF_PARSER_HPP___
#define BNF_PARSER_HPP___

#include <memory>
#include <string>
#include <list>
#include <map>
#include <fstream>
#include <iostream>

enum bnf_token_type {
   bnft_term,
   bnft_nterm, 
   bnft_yeild,
   bnft_or
};

class bnf_token {
public:
   bnf_token_type mType;
   std::string    mValue;
};

class bnf_symbol {
public:
   bnf_symbol(std::string name) : mName(name) {}
   std::string mName;

   virtual bool IsTerminal() = 0;
};

class bnf_terminal : public bnf_symbol {
public:
   bnf_terminal(std::string name) : bnf_symbol(name) {}

   bool IsTerminal() { return true; }
};

class bnf_production : public bnf_symbol {
public:
   bnf_production(std::string name) :  bnf_symbol(name) {}

   bool IsTerminal() { return false; }

   std::list<std::list<bnf_symbol *>> mExpensionList; 
};


class bnf_parser {
public:
   bnf_parser(std::string filename) : mRoot(""), mNull(""), mFileName(filename), mState(true),
                                      mLexerPointer(0) {

      mIsLexed = false;

      mTokenNames[bnft_term]  = "Terminal";
      mTokenNames[bnft_nterm] = "Non Terminal";
      mTokenNames[bnft_yeild] = "Yeild";
      mTokenNames[bnft_or]    = "Or";

      // Read the file into memory
      // ------------------------------------------
      std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
      if (in.fail()) {
         mState = false;
      }
      else {
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
   }
   ~bnf_parser() {
      if (mPayload) delete [] mPayload;
      for (auto t: mTokenList) {
         delete t;
      } 
      for (auto &t: mTerminals) {
         delete t.second;
      }
      for (auto &t: mProductions) {
         delete t.second;
      }
   }
   
   auto find_terminal(std::string name) -> 
      std::map<std::string, bnf_symbol *>::iterator {
      auto it = mTerminals.find(name);
      return it;
   }

   auto find_production(std::string name) -> 
      std::map<std::string, bnf_symbol *>::iterator {
      auto it = mProductions.find(name);
      return it;
   }

   bool isalpha(const char c) {
      return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c == '_');
   }

   bool lex_comment(int char_left) {
      // Skip the '#' caracter
      ++mLexerPointer;
      
      // While not end of line
      while ((mPayload[mLexerPointer] != '\n') && (mPayload[mLexerPointer] != '\f') && 
             (char_left > 0)) {
         ++mLexerPointer;--char_left;
      }
      // Skip additional CR or LF
      while ((mPayload[mLexerPointer] == '\n') || (mPayload[mLexerPointer] == '\f') && 
              (char_left > 0)) {
         ++mLexerPointer;--char_left;
      }
   }

   bool lex_root(int char_left) {
      bool rval = true;

      // Skip >
      ++mLexerPointer; --char_left;
      
      while (mPayload[mLexerPointer] != '<' &&
             (char_left > 0) ) {
         ++mLexerPointer; --char_left;
      }
      // Skip <
      ++mLexerPointer; --char_left;

      if (char_left) {
         while (mPayload[mLexerPointer] != '>' &&
                (char_left > 0)) {
            mRoot += mPayload[mLexerPointer];
            ++mLexerPointer; --char_left;
         }

         if (char_left) {
            // Skip >
            ++mLexerPointer; --char_left;
         }
         else {
            rval = false;
         }
         
      }
      else {
         rval = false;
      }
      
      return rval;
   }

   bool lex_null(int char_left) {
      bool rval = true;

      // Skip /
      ++mLexerPointer; --char_left;
      
      // Skip white spaces
      while (((mPayload[mLexerPointer] == ' ')  ||
              (mPayload[mLexerPointer] == '\t')) &&
             (char_left > 0)) { 
         ++mLexerPointer; --char_left;
      }
      
      while ((mPayload[mLexerPointer] != ' ')  &&
             (mPayload[mLexerPointer] != '\t') &&
             (mPayload[mLexerPointer] != '\n') &&
             (mPayload[mLexerPointer] != '\f') && 
             char_left > 0) {
         mNull += mPayload[mLexerPointer];
         ++mLexerPointer; --char_left;
      }

      return rval;
   }

   bool lex_nterm(int char_left) {
      bool rval = true;

      bnf_token *tok = new bnf_token{bnft_nterm, std::string("")};

      // Skip <
      ++mLexerPointer; --char_left;
      
      while (mPayload[mLexerPointer] != '>' &&
             (char_left > 0) ) {
         tok->mValue += mPayload[mLexerPointer];
         ++mLexerPointer; --char_left;
      }

      // Skip >
      ++mLexerPointer; --char_left;
      
      if (rval == true) {
         mTokenList.push_back(tok);
      }

      return rval;
   }

   bool lex_term(int char_left) {
      bool rval = true;

      bnf_token *tok = new bnf_token{bnft_term, std::string("")};
      
      //std::cout << "char_left = " << char_left << std::endl;
      while ((mPayload[mLexerPointer] != ' ')  &&
             (mPayload[mLexerPointer] != '\t') &&
             (mPayload[mLexerPointer] != '\n') &&
             (mPayload[mLexerPointer] != '\f') && 
             char_left > 0) {
         tok->mValue += mPayload[mLexerPointer];
         ++mLexerPointer; --char_left;
      }
      
      if (rval == true) {
         mTokenList.push_back(tok);
      }

      return rval;
   }

   bool lex() {
      bool rval = true;
      while (mLexerPointer < mPayloadSize) {
         if (mPayload[mLexerPointer] == '#') {
            rval = lex_comment(mPayloadSize-mLexerPointer);
         }
         else if (mPayload[mLexerPointer] == ':') {
            ++mLexerPointer;
            if (mPayload[mLexerPointer] == ':') {
                ++mLexerPointer;
                if (mPayload[mLexerPointer] == '=') {
                   ++mLexerPointer;
                }
                else {
                   rval = false;
                   break;
                }
            }
            else {
               rval = false;
               break;
            }
            mTokenList.push_back(new bnf_token {bnft_yeild, " "});
         }
         else if (mPayload[mLexerPointer] == '|') {
             ++mLexerPointer;
             mTokenList.push_back(new bnf_token {bnft_or, " "});
         }
         else if (mPayload[mLexerPointer] == '>') {
            if ((mPayload[mLexerPointer-1] == '\n') || (mPayload[mLexerPointer-1] == '\f') || 
                (mLexerPointer == 0)) {
               rval = lex_root(mPayloadSize-mLexerPointer);
            }
            else {
                rval = lex_term(mPayloadSize-mLexerPointer);
            }
         }
         else if (mPayload[mLexerPointer] == '/') {
            // If first caracter on a line 
            if ((mPayload[mLexerPointer-1] == '\n') || (mPayload[mLexerPointer-1] == '\f') || 
                (mLexerPointer == 0)) {
               rval = lex_null(mPayloadSize-mLexerPointer);
            } 
            else {
               rval = lex_term(mPayloadSize-mLexerPointer);
            }
         }
         else if (mPayload[mLexerPointer] == '<') {
            rval = lex_nterm(mPayloadSize-mLexerPointer);
         }
         else if ((mPayload[mLexerPointer] == '\t')  ||
                  (mPayload[mLexerPointer] == '\n')  ||
                  (mPayload[mLexerPointer] == '\f')  ||
                  (mPayload[mLexerPointer] == ' ')) {
            ++mLexerPointer;
         }
         else {
            rval = lex_term(mPayloadSize-mLexerPointer);
         }
      }
      if (rval) mIsLexed = true;
      return rval;
   }

   bool parse_production() {
      bool rval = true;

      // Is this a new non-terminal?
      if (find_production((*mTokenCurrent)->mValue) == mProductions.end()) {
         //std::cout << "New production symbol : " << (*mTokenCurrent)->mValue << std::endl;
         mProductions[(*mTokenCurrent)->mValue] = 
            new bnf_production((*mTokenCurrent)->mValue);
      }
      else {
         //std::cout << "Adding to production symbol : " << (*mTokenCurrent)->mValue << std::endl;
      }
      
      // Current production pointer
      bnf_production *prod_pointer = 
         reinterpret_cast<bnf_production *>(mProductions[(*mTokenCurrent)->mValue]);

      // Advace to next token
      ++mTokenCurrent;++mTokenNext;

      // This should be a yeild 
      if ((*mTokenCurrent)->mType != bnft_yeild ) {
         rval = false;
      }
      else {
         // Advace to next token
         ++mTokenCurrent;++mTokenNext;
         
         std::list<bnf_symbol *> temp;
         // While not at the end and next is not yeild
         while (mTokenCurrent != mTokenList.end()) {
            if ((mTokenNext != mTokenList.end()) && 
                ((*mTokenNext)->mType == bnft_yeild)) {
               break;
            }
            // Found "OR"
            if ((*mTokenCurrent)->mType == bnft_or) {
               prod_pointer->mExpensionList.push_back(temp);
               temp.clear();
            }
            // Found "Terminal"
            else if ((*mTokenCurrent)->mType == bnft_term) {
               // Is this a new termial symbol
               if (find_terminal((*mTokenCurrent)->mValue) == mTerminals.end()) {
                  //std::cout << "New terminal symbol : " << (*mTokenCurrent)->mValue << std::endl;
                  mTerminals[(*mTokenCurrent)->mValue] = 
                     new bnf_terminal((*mTokenCurrent)->mValue);
               }
               temp.push_back(mTerminals[(*mTokenCurrent)->mValue]);
            }
            // Found "Non Terminal"
            else if ((*mTokenCurrent)->mType == bnft_nterm) {
               // Is this a new termial symbol
               if (find_production((*mTokenCurrent)->mValue) == mProductions.end()) {
                  //std::cout << "New production symbol : " << (*mTokenCurrent)->mValue << std::endl;
                  mProductions[(*mTokenCurrent)->mValue] = 
                     new bnf_production((*mTokenCurrent)->mValue);
               }
               temp.push_back(mProductions[(*mTokenCurrent)->mValue]);
            }
            ++mTokenCurrent;++mTokenNext;
         }
         
         // Push the last production 
         prod_pointer->mExpensionList.push_back(temp);
      }
      return rval;
   }
   
   bool parse() {
      bool rval = true;

      std::cout << "Parsing ..." << std::endl;

      // All good ?
      if (mIsLexed && mState) {
         mTokenCurrent = mTokenList.begin();
         mTokenNext    = std::next(mTokenCurrent);
         const auto end = mTokenList.end();

         while (mTokenCurrent != end) {
            if ((*mTokenCurrent)->mType == bnft_nterm) {
               parse_production();
            }
            else {
               //
               std::cout << "Can't find non-terminal to initiate a production" << std::endl;
               std::cout << "Token type = " << mTokenNames[(*mTokenCurrent)->mType] << std::endl;
               // Something went wrong
               rval = false;
               break;
            }
         } 
      }
      else {
         std::cout << "Grammar not lexed or invalid state" << std::endl;
         std::cout << "State : " << mState << std::endl;
         std::cout << "Lexed : " << mIsLexed << std::endl;
         rval = false;
      }

      return rval;
   }

   // Calculate LR Parsing Tables
   bool calculate_firsts_table() {
      bool rval = true;

      

      return rval;
   }

   // Debug output functions
   void output_token_list() {
      for (auto t: mTokenList) {
         std::cout << mTokenNames[t->mType] << " : " << t->mValue << std::endl;
      }
   }
   void output_root() {
      std::cout << "Root : " << mRoot << std::endl;
   }
   void output_null() {
      std::cout << "Null : " << mNull << std::endl;
   }
   void output_productions() {
      for (auto &p: mProductions) {
         std::cout << "Production : <" << p.second->mName << ">" << std::endl;
         //std::cout << p.second->IsTerminal() << std::endl;

         bnf_production *prod = dynamic_cast<bnf_production *>(p.second);
         //std::cout << prod->mExpensionList.size() << std::endl;
         // For all expension lists
         int prod_number = 0;
         for (auto &el: prod->mExpensionList) {
            // For all item in list
            std::cout << prod_number << " ::= ";
            for (auto &li: el) {
               if (li->IsTerminal()) {
                  std::cout << li->mName << " ";
               }
               else {
                  std::cout << "<" << li->mName << "> ";
               }
            } 
            std::cout << std::endl;
            ++prod_number;
         }
      }
   }
   
   std::string get_file_name() {
      return mFileName;
   }

private:   
   // Parser variables
   std::map<std::string, bnf_symbol *>  mTerminals;
   std::map<std::string, bnf_symbol *>  mProductions;

   std::string mRoot;
   std::string mNull;

   // LR Parsing Tables
   std::map<std::string, std::list<bnf_terminal *>> mFirsts; 

   std::string mFileName;
   bool        mState;
   bool        mIsLexed;

   unsigned int mPayloadSize;
   char *       mPayload;

   // Lexer variables
   unsigned int mLexerPointer;
   
   std::map<bnf_token_type, std::string> mTokenNames;
   std::list<bnf_token *> mTokenList;
   std::list<bnf_token *>::iterator mTokenCurrent;
   std::list<bnf_token *>::iterator mTokenNext;
};

#endif


