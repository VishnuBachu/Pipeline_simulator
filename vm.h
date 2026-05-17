#ifndef VM_H
#define VM_H

#include <cstdint>
#include <string>

struct VmConfig
{
	uint32_t virtual_size_bytes = 65536;
	uint32_t physical_size_bytes = 16384;
	uint32_t page_size_bytes = 4096;
	int dtlb_entries = 4;
	int tlb_hit_latency = 1;
	int page_walk_latency = 10;
	int page_fault_latency = 50;
	int dirty_writeback_cycles = 0;
	bool replacement_fifo = true;
};

bool load_vm_config(const std::string& filename, VmConfig& cfg);
void vm_init(const VmConfig& cfg);
void vm_reset_stats();

// One load/store: resolve VA to PA, update TLB/page table, update stats.
// Returns cycles to charge for the translation path (TLB walk, fault, writebacks).
int vm_translate_access(uint32_t va, bool is_write, uint32_t* physical_byte_addr);

bool phys_read_word(uint32_t pa, int* out);
bool phys_write_word(uint32_t pa, int value);

extern bool trace_replay_mode;

extern long long stat_tlb_hits;
extern long long stat_tlb_misses;
extern long long stat_page_walks;
extern long long stat_page_faults;
extern long long stat_page_evictions;
extern long long stat_dirty_writebacks;
extern long long stat_translation_penalty_cycles;

#endif
