#ifndef CACHE_H
#define CACHE_H

#include "structures.h"
#include <cstdint>

void init_caches(const Config& cfg);
int access_instruction_cache(uint32_t byte_address);
int access_data_cache(uint32_t byte_address);
double overall_l1_miss_rate();

#endif
