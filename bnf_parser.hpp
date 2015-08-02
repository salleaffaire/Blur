#ifndef BNF_PARSER_HPP___
#define BNF_PARSER_HPP___

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
   }
   
   auto find_terminal(std::string name, bool &found) -> 
      std::map<std::string, bnf_terminal>::iterator {
      found = true;
      auto it = mTerminals.find(name);
      if (it == mTerminals.end()) {
         found = false;
      }
      return it;
   }

   auto find_production(std::string name, bool &found) -> 
      std::map<std::string, bnf_production>::iterator {
      found = true;
      auto it = mProductions.find(name);
      if (it == mProductions.end()) {
         found = false;
      }
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
            rval = lex_root(mPayloadSize-mLexerPointer);
         }
         else if (mPayload[mLexerPointer] == '/') {
            rval = lex_null(mPayloadSize-mLexerPointer);
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
      return rval;
   }

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

   std::map<std::string, bnf_terminal>    mTerminals;
   std::map<std::string, bnf_production>  mProductions;

   std::string mRoot;
   std::string mNull;

   std::string mFileName;
   bool        mState;

   unsigned int mPayloadSize;
   char *       mPayload;

   unsigned int mLexerPointer;
   
   std::map<bnf_token_type, std::string> mTokenNames;
   std::list<bnf_token *> mTokenList;

};

#endif


