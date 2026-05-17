#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
using namespace std;
extern int memory[1024];
struct Instruction
{
	string opcode = "";
	int rd = -1;
	int rs1 = -1;
	int rs2 = -1;		//registers start with safe val's	
	int imm = 0;
	string label = "";
	// Phase 3 trace replay: L/S use virtual address here (byte).
	uint32_t trace_va = 0;
	bool trace_mem = false;
};

struct PipelineReg 
{
	Instruction instr;
	int alu_result;
	bool empty = true;
	uint32_t mem_phys_addr = 0;
};

struct Config
{
	bool forwarding;
	int add_latency;
	int mul_latency;
	int main_memory_latency;

	int l1i_size;
	int l1i_block_size;
	int l1i_associativity;
	int l1i_latency;

	int l1d_size;
	int l1d_block_size;
	int l1d_associativity;
	int l1d_latency;

	int l2_size;
	int l2_block_size;
	int l2_associativity;
	int l2_latency;

	string replacement_policy_l1;
	string replacement_policy_l2;
};
extern Config config;
extern int cycles;
extern int stalls;
extern int instructions_executed;

vector<Instruction> load_trace(const string& filename);

#endif
