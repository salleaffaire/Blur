stream a("filename.dat");
 
struct TEST_STRUCTURE_0 {
   bit[16] var0;
   bit[32] var1;
}

proc TEST_PROCEDURE_0 {
   var bit[32] counter = 10;
   var TEST_STRUCTURE_0 ts;
   
   while (n <= 0) (
      a.pop -> ts;      // Pop 48-bits in store in ts
      a.pop -> bit[16]; // Skip 16-bits 

      if (ts.var0 == 0) {
         ts.var1 = ts.var1 / 2;
         break;
      }
   }  
}