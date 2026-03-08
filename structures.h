#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>
#include <vector>
#include <string>
using namespace std;
extern int memory[1024];
struct Instruction {
    string opcode = "";
    int rd = -1;
    int rs1 = -1;
    int rs2 = -1;		//registers start with safe val's	
    int imm = 0;
    string label = "";
};

struct PipelineReg {
    Instruction instr;
    int alu_result;
    bool empty = true;
};

struct Config {
    bool forwarding;
    int add_latency;
    int mul_latency;
};
extern Config config;
extern int cycles;
extern int stalls;
extern int instructions_executed;

#endif
