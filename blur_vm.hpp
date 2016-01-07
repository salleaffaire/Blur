#ifndef BLURRED_VM_HPP___
#define BLURRED_VM_HPP___

#include <cstdint> 

#include "blur_insts.hpp"

class blr_vm {
public:
   blr_vm(unsigned int stack_size, unsigned char *gs) :
      mStackSize(stack_size),
      mStackPointer(0),
      mGlobalSpace(gs) {
      mStack = new unsigned char [mStackSize];

      // Push yeild
      push8(BLR_INST_YIELD);

      // Push call main
      push32(0);            // Main is function ID 0
      push8(BLR_INST_CALL); // Call
   }
   ~blr_vm() {
      if (mStack) {
	 delete [] mStack;
      }
   }
   
   bool run() {
      while (0 != mStackPointer) {
	 
      }
   }


private:
   
   // Stack Access Function
   void push8(uint8_t x) {
      mStack[mStackPointer] = x; ++mStackPointer;
   }
   void push16(uint16_t x) {
      mStack[mStackPointer] = x; ++mStackPointer; // 0
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 1
   }
   void push32(uint32_t x) {
      mStack[mStackPointer] = x; ++mStackPointer; // 0
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 1
      x >>= 8;  
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 2
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 3
   }
   void push64(uint64_t x) {
      mStack[mStackPointer] = x; ++mStackPointer; // 0
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 1
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 2
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 3
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 4
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 5
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 6
      x >>= 8; 
      mStack[mStackPointer] = x & 0xFF; ++mStackPointer; // 7
   }

   uint8_t pop8() {
      uint8_t rval = mStack[mStackPointer]; --mStackPointer; //0
      return rval;
   }
   uint16_t pop16() {
      uint16_t rval = mStack[mStackPointer]; --mStackPointer; // 1
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; //0
      return rval;
   }
   uint32_t pop32() {
      uint32_t rval = mStack[mStackPointer]; --mStackPointer; // 3
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 2
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 1
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 0
      return rval;
   }
   uint64_t pop64() {
      uint64_t rval = mStack[mStackPointer]; --mStackPointer; // 7
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 6
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 5
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 4
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 3
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 2
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 1
      rval <<= 8; rval |= mStack[mStackPointer]; --mStackPointer; // 0
      return rval;
   }   

   // Instruction Stack 
   uint8_t       *mStack;
   unsigned int   mStackSize;
   unsigned int   mStackPointer;
   
   // Global Memory
   uint8_t        *mGlobalSpace;

   // Yeild Memory
   uint8_t        *mYeildSpace; 
};

#endif
