#include "vm.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::vector;

bool trace_replay_mode = false;

long long stat_tlb_hits = 0;
long long stat_tlb_misses = 0;
long long stat_page_walks = 0;
long long stat_page_faults = 0;
long long stat_page_evictions = 0;
long long stat_dirty_writebacks = 0;
long long stat_translation_penalty_cycles = 0;

namespace
{
VmConfig g_cfg{};
vector<int> g_phys_words;
int g_num_frames = 0;
int g_num_pages = 0;

struct Pte
{
	bool valid = false;
	bool dirty = false;
	int frame = -1;
};

vector<Pte> g_pt;
vector<int> g_frame_to_vpn;
vector<long long> g_frame_last_use;
vector<int> g_frame_fifo_order;
long long g_vm_clock = 0;

struct TlbEntry
{
	bool valid = false;
	uint32_t vpn = 0;
	uint32_t pfn = 0;
	bool dirty = false;
	long long last_use = 0;
};

vector<TlbEntry> g_tlb;

static string trim(string s)
{
	auto a = s.find_first_not_of(" \t\r\n");
	if (a == string::npos) return {};
	auto b = s.find_last_not_of(" \t\r\n");
	return s.substr(a, b - a + 1);
}

static string to_lower(string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
	return s;
}

static int find_tlb_vpn(uint32_t vpn)
{
	for (int i = 0; i < static_cast<int>(g_tlb.size()); ++i)
	{
		if (g_tlb[i].valid && g_tlb[i].vpn == vpn) return i;
	}
	return -1;
}

static void tlb_touch(int idx)
{
	g_vm_clock++;
	g_tlb[idx].last_use = g_vm_clock;
}

static void tlb_invalidate_vpn(uint32_t vpn)
{
	for (auto& e : g_tlb)
	{
		if (e.valid && e.vpn == vpn) e.valid = false;
	}
}

static void tlb_install(uint32_t vpn, uint32_t pfn, bool dirty)
{
	int slot = -1;
	for (int i = 0; i < static_cast<int>(g_tlb.size()); ++i)
	{
		if (!g_tlb[i].valid)
		{
			slot = i;
			break;
		}
	}
	if (slot < 0)
	{
		slot = 0;
		for (int i = 1; i < static_cast<int>(g_tlb.size()); ++i)
		{
			if (g_tlb[i].last_use < g_tlb[slot].last_use) slot = i;
		}
	}
	g_tlb[slot].valid = true;
	g_tlb[slot].vpn = vpn;
	g_tlb[slot].pfn = pfn;
	g_tlb[slot].dirty = dirty;
	tlb_touch(slot);
}

static int pick_evict_frame()
{
	if (g_cfg.replacement_fifo)
	{
		if (g_frame_fifo_order.empty()) return 0;
		int f = g_frame_fifo_order.front();
		g_frame_fifo_order.erase(g_frame_fifo_order.begin());
		return f;
	}
	int victim = 0;
	long long best = g_frame_last_use[0];
	for (int f = 1; f < g_num_frames; ++f)
	{
		if (g_frame_last_use[f] < best)
		{
			best = g_frame_last_use[f];
			victim = f;
		}
	}
	return victim;
}

static int alloc_frame_for_vpn(uint32_t vpn, int& extra_penalty)
{
	extra_penalty = 0;
	for (int f = 0; f < g_num_frames; ++f)
	{
		if (g_frame_to_vpn[f] < 0)
		{
			g_frame_to_vpn[f] = static_cast<int>(vpn);
			g_frame_last_use[f] = ++g_vm_clock;
			if (g_cfg.replacement_fifo) g_frame_fifo_order.push_back(f);
			return f;
		}
	}
	stat_page_evictions++;
	const int f = pick_evict_frame();
	const int old_vpn = g_frame_to_vpn[f];
	if (old_vpn >= 0 && old_vpn < g_num_pages)
	{
		Pte& old_pte = g_pt[static_cast<size_t>(old_vpn)];
		if (old_pte.valid && old_pte.dirty)
		{
			stat_dirty_writebacks++;
			extra_penalty += g_cfg.dirty_writeback_cycles;
		}
		old_pte.valid = false;
		old_pte.dirty = false;
		old_pte.frame = -1;
		tlb_invalidate_vpn(static_cast<uint32_t>(old_vpn));
	}
	g_frame_to_vpn[f] = static_cast<int>(vpn);
	g_frame_last_use[f] = ++g_vm_clock;
	if (g_cfg.replacement_fifo) g_frame_fifo_order.push_back(f);
	return f;
}
} // namespace

void vm_reset_stats()
{
	stat_tlb_hits = stat_tlb_misses = 0;
	stat_page_walks = stat_page_faults = 0;
	stat_page_evictions = stat_dirty_writebacks = 0;
	stat_translation_penalty_cycles = 0;
}

bool load_vm_config(const string& filename, VmConfig& cfg)
{
	std::ifstream file(filename);
	if (!file) return false;
	string line;
	string section;
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#' || line[0] == ';') continue;
		if (line.front() == '[' && line.back() == ']')
		{
			section = to_lower(line.substr(1, line.size() - 2));
			continue;
		}
		const auto eq = line.find('=');
		if (eq == string::npos) continue;
		string key = trim(line.substr(0, eq));
		string val = trim(line.substr(eq + 1));
		key = to_lower(key);
		val = to_lower(val);

		if (key == "virtual_size_bytes") cfg.virtual_size_bytes = static_cast<uint32_t>(stoul(val));
		else if (key == "physical_size_bytes") cfg.physical_size_bytes = static_cast<uint32_t>(stoul(val));
		else if (key == "page_size_bytes") cfg.page_size_bytes = static_cast<uint32_t>(stoul(val));
		else if (key == "dtlb_entries") cfg.dtlb_entries = stoi(val);
		else if (key == "tlb_hit_latency") cfg.tlb_hit_latency = stoi(val);
		else if (key == "page_walk_latency") cfg.page_walk_latency = stoi(val);
		else if (key == "page_fault_latency") cfg.page_fault_latency = stoi(val);
		else if (key == "dirty_writeback_cycles" || key == "dirty_writeback_latency")
			cfg.dirty_writeback_cycles = stoi(val);
		else if (key == "replacement_policy")
		{
			cfg.replacement_fifo = (val == "fifo");
		}
	}
	return true;
}

void vm_init(const VmConfig& cfg)
{
	g_cfg = cfg;
	if (g_cfg.page_size_bytes == 0) g_cfg.page_size_bytes = 4096;
	g_num_pages = static_cast<int>(g_cfg.virtual_size_bytes / g_cfg.page_size_bytes);
	g_num_frames = static_cast<int>(g_cfg.physical_size_bytes / g_cfg.page_size_bytes);
	if (g_num_pages < 1) g_num_pages = 1;
	if (g_num_frames < 1) g_num_frames = 1;

	g_pt.assign(static_cast<size_t>(g_num_pages), Pte{});
	const size_t words = static_cast<size_t>(g_cfg.physical_size_bytes / 4u);
	g_phys_words.assign(words, 0);
	g_frame_to_vpn.assign(static_cast<size_t>(g_num_frames), -1);
	g_frame_last_use.assign(static_cast<size_t>(g_num_frames), 0);
	g_frame_fifo_order.clear();

	g_tlb.assign(static_cast<size_t>(std::max(1, g_cfg.dtlb_entries)), TlbEntry{});
	vm_reset_stats();
	g_vm_clock = 0;
}

bool phys_read_word(uint32_t pa, int* out)
{
	if (pa + 3u < pa) return false;
	const uint32_t idx = pa / 4u;
	if (idx >= g_phys_words.size()) return false;
	*out = g_phys_words[idx];
	return true;
}

bool phys_write_word(uint32_t pa, int value)
{
	if (pa + 3u < pa) return false;
	const uint32_t idx = pa / 4u;
	if (idx >= g_phys_words.size()) return false;
	g_phys_words[idx] = value;
	return true;
}

int vm_translate_access(uint32_t va, bool is_write, uint32_t* physical_byte_addr)
{
	if (va >= g_cfg.virtual_size_bytes)
	{
		*physical_byte_addr = 0;
		return g_cfg.tlb_hit_latency;
	}
	const uint32_t page_size = g_cfg.page_size_bytes;
	const uint32_t vpn = va / page_size;
	const uint32_t offset = va % page_size;

	const int tlb_i = find_tlb_vpn(vpn);
	if (tlb_i >= 0)
	{
		stat_tlb_hits++;
		const int pen = g_cfg.tlb_hit_latency;
		stat_translation_penalty_cycles += pen;
		tlb_touch(tlb_i);
		if (is_write)
		{
			g_tlb[tlb_i].dirty = true;
			if (vpn < g_pt.size()) g_pt[vpn].dirty = true;
		}
		const uint32_t pfn = g_tlb[tlb_i].pfn;
		*physical_byte_addr = pfn * page_size + offset;
		if (static_cast<int>(pfn) >= 0 && static_cast<int>(pfn) < g_num_frames)
			g_frame_last_use[static_cast<size_t>(pfn)] = ++g_vm_clock;
		return pen;
	}

	stat_tlb_misses++;
	stat_page_walks++;
	int penalty = g_cfg.page_walk_latency;

	if (vpn >= g_pt.size() || !g_pt[vpn].valid)
	{
		stat_page_faults++;
		penalty += g_cfg.page_fault_latency;
		int extra = 0;
		const int frame = alloc_frame_for_vpn(vpn, extra);
		penalty += extra;
		g_pt[vpn].valid = true;
		g_pt[vpn].dirty = is_write;
		g_pt[vpn].frame = frame;
	}
	else if (is_write)
	{
		g_pt[vpn].dirty = true;
	}

	const uint32_t pfn = static_cast<uint32_t>(g_pt[vpn].frame);
	tlb_install(vpn, pfn, g_pt[vpn].dirty);
	if (is_write)
	{
		const int ti = find_tlb_vpn(vpn);
		if (ti >= 0) g_tlb[ti].dirty = true;
	}

	stat_translation_penalty_cycles += penalty;
	*physical_byte_addr = pfn * page_size + offset;
	if (static_cast<int>(pfn) >= 0 && static_cast<int>(pfn) < g_num_frames)
		g_frame_last_use[static_cast<size_t>(pfn)] = ++g_vm_clock;
	return penalty;
}
