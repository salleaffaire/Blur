#include "blur_common.hpp"

void blr_store_little_endian(uint8_t *d, uint32_t s) {
   *(d+0) = (s      ) & 0xFF;
   *(d+1) = (s >> 8 ) & 0xFF;
   *(d+2) = (s >> 16) & 0xFF;
   *(d+3) = (s >> 24) & 0xFF;
}

void blr_store_little_endian(uint8_t *d, uint16_t s) {
   *(d+0) = (s     )  & 0xFF;
   *(d+1) = (s >> 8)  & 0xFF;
}

void blr_store_big_endian(uint8_t *d, uint32_t s) {
   *(d+0) = (s >> 24) & 0xFF;
   *(d+1) = (s >> 16) & 0xFF;
   *(d+2) = (s >> 8 ) & 0xFF;
   *(d+3) = (s      ) & 0xFF;
}

void blr_store_big_endian(uint8_t *d, uint16_t s) {
   *(d+0) = (s >> 8) & 0xFF;
   *(d+1) = (s     ) & 0xFF;
}



