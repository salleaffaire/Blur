#ifndef BLURRED_COMMON_HPP___
#define BLURRED_COMMON_HPP___

#include <cstdint>

void blr_store_little_endian(uint8_t *d, uint16_t s); 
void blr_store_little_endian(uint8_t *d, uint32_t s); 

void blr_store_big_endian(uint8_t *d, uint16_t s); 
void blr_store_big_endian(uint8_t *d, uint32_t s); 


#endif
