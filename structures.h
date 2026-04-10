#ifndef STRUCTURES_H
#define STRUCTURES_H

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
};

struct PipelineReg 
{
	Instruction instr;
	int alu_result;
	bool empty = true;
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

#endif
