#include "structures.h"
#include "cache.h"
#include "vm.h"
#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

Config load_config(string filename);
vector<Instruction> load_program(string filename);
void run(vector<Instruction>& program);

Config config;

static bool ends_with_trace(const string& s)
{
	const string suf = ".trace";
	return s.size() >= suf.size() && s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

int main(int argc, char* argv[])
{
	trace_replay_mode = false;
	config = load_config("config.txt");
	init_caches(config);

	vector<Instruction> program;
	string vm_ini = "vm_config.ini";
	if (argc >= 2)
	{
		const string arg1 = argv[1];
		if (ends_with_trace(arg1))
		{
			if (argc >= 3) vm_ini = argv[2];
			VmConfig vm{};
			if (!load_vm_config(vm_ini, vm))
			{
				cerr << "Failed to read VM config: " << vm_ini << endl;
				return 1;
			}
			vm_init(vm);
			trace_replay_mode = true;
			program = load_trace(arg1);
			if (program.empty())
			{
				cerr << "Empty or missing trace: " << arg1 << endl;
				return 1;
			}
		}
		else
		{
			program = load_program(arg1);
		}
	}
	else
	{
		program = load_program("program.asm");
	}

	run(program);

	cout << "Execution finished\n";
	cout << "Cycles: " << cycles << endl;
	cout << "Instructions: " << instructions_executed << endl;
	cout << "Stalls: " << stalls << endl;
	cout << "Cache miss rate: " << overall_l1_miss_rate() << endl;

	if (cycles > 0)
		cout << "IPC: " << (static_cast<double>(instructions_executed) / cycles) << endl;
	else
		cout << "IPC: n/a\n";

	if (trace_replay_mode)
	{
		cout << "TLB hits: " << stat_tlb_hits << endl;
		cout << "TLB misses: " << stat_tlb_misses << endl;
		cout << "Page walks: " << stat_page_walks << endl;
		cout << "Page faults: " << stat_page_faults << endl;
		cout << "Page evictions: " << stat_page_evictions << endl;
		cout << "Dirty writebacks: " << stat_dirty_writebacks << endl;
		cout << "Translation penalty cycles: " << stat_translation_penalty_cycles << endl;
	}
	return 0;
}
