#include "cache.h"
#include <algorithm>
#include <cctype>
#include <vector>

using std::max;
using std::string;
using std::vector;

namespace
{
enum class ReplacementPolicy
{
	LRU,
	FIFO
};

struct CacheLine
{
	bool valid = false;
	uint32_t tag = 0;
	long long last_used = 0;
	long long inserted_at = 0;
};

class SetAssociativeCache
{
public:
	SetAssociativeCache() = default;

	void configure(int size_bytes, int block_size, int associativity, int latency, ReplacementPolicy policy)
	{
		block_size_ = max(1, block_size);
		associativity_ = max(1, associativity);
		latency_ = max(1, latency);
		policy_ = policy;

		const int blocks = max(1, size_bytes / block_size_);
		num_sets_ = max(1, blocks / associativity_);
		sets_.assign(num_sets_, vector<CacheLine>(associativity_));
		timer_ = 0;
		accesses_ = 0;
		misses_ = 0;
	}

	bool access(uint32_t byte_address)
	{
		accesses_++;
		timer_++;

		const uint32_t block_number = byte_address / static_cast<uint32_t>(block_size_);
		const int set_idx = static_cast<int>(block_number % static_cast<uint32_t>(num_sets_));
		const uint32_t tag = block_number / static_cast<uint32_t>(num_sets_);

		auto& set = sets_[set_idx];
		for(auto& line : set)
		{
			if(line.valid && line.tag == tag)
			{
				line.last_used = timer_;
				return true;
			}
		}

		misses_++;
		int victim_idx = -1;
		for(int i = 0; i < static_cast<int>(set.size()); i++)
		{
			if(!set[i].valid)
			{
				victim_idx = i;
				break;
			}
		}

		if(victim_idx == -1)
		{
			victim_idx = 0;
			for(int i = 1; i < static_cast<int>(set.size()); i++)
			{
				if(policy_ == ReplacementPolicy::LRU)
				{
					if(set[i].last_used < set[victim_idx].last_used) victim_idx = i;
				}
				else
				{
					if(set[i].inserted_at < set[victim_idx].inserted_at) victim_idx = i;
				}
			}
		}

		set[victim_idx].valid = true;
		set[victim_idx].tag = tag;
		set[victim_idx].last_used = timer_;
		set[victim_idx].inserted_at = timer_;
		return false;
	}

	int latency() const { return latency_; }
	long long accesses() const { return accesses_; }
	long long misses() const { return misses_; }

private:
	int block_size_ = 64;
	int associativity_ = 1;
	int latency_ = 1;
	int num_sets_ = 1;
	ReplacementPolicy policy_ = ReplacementPolicy::LRU;
	vector<vector<CacheLine>> sets_;
	long long timer_ = 0;
	long long accesses_ = 0;
	long long misses_ = 0;
};

SetAssociativeCache l1i_cache;
SetAssociativeCache l1d_cache;
SetAssociativeCache l2_cache;
int main_memory_latency = 50;

ReplacementPolicy parse_policy(const string& policy_name)
{
	string p = policy_name;
	std::transform(p.begin(), p.end(), p.begin(), [](unsigned char c){ return static_cast<char>(tolower(c)); });
	if(p == "fifo") return ReplacementPolicy::FIFO;
	return ReplacementPolicy::LRU;
}
}

void init_caches(const Config& cfg)
{
	l1i_cache.configure(cfg.l1i_size, cfg.l1i_block_size, cfg.l1i_associativity, cfg.l1i_latency, parse_policy(cfg.replacement_policy_l1));
	l1d_cache.configure(cfg.l1d_size, cfg.l1d_block_size, cfg.l1d_associativity, cfg.l1d_latency, parse_policy(cfg.replacement_policy_l1));
	l2_cache.configure(cfg.l2_size, cfg.l2_block_size, cfg.l2_associativity, cfg.l2_latency, parse_policy(cfg.replacement_policy_l2));
	main_memory_latency = max(1, cfg.main_memory_latency);
}

int access_instruction_cache(uint32_t byte_address)
{
    int latency = l1i_cache.latency();

    // L1I hit
    if(l1i_cache.access(byte_address)) 
        return latency;

    // L1I miss → check L2
    latency += l2_cache.latency();

    if(l2_cache.access(byte_address))
    {
        // Bring block into L1I
        l1i_cache.access(byte_address);
        return latency;
    }

    // L2 miss → go to memory
    latency += main_memory_latency;

    //  Fill both L2 and L1I
    l2_cache.access(byte_address);
    l1i_cache.access(byte_address);

    return latency;
}

int access_data_cache(uint32_t byte_address)
{
    int latency = l1d_cache.latency();

    // L1D hit
    if(l1d_cache.access(byte_address)) 
        return latency;

    // L1D miss → check L2
    latency += l2_cache.latency();

    if(l2_cache.access(byte_address))
    {
        // Bring block into L1D
        l1d_cache.access(byte_address);
        return latency;
    }

    // L2 miss → go to memory
    latency += main_memory_latency;

    //Fill both L2 and L1D
    l2_cache.access(byte_address);
    l1d_cache.access(byte_address);

    return latency;
}

double overall_l1_miss_rate()
{
	const long long accesses = l1i_cache.accesses() + l1d_cache.accesses();
	const long long misses = l1i_cache.misses() + l1d_cache.misses();
	if(accesses == 0) return 0.0;
	return static_cast<double>(misses) / static_cast<double>(accesses);
}
