#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Instruction {
    string opcode;
    int rd;
    int rs1;
    int rs2;
    int imm;
    string label;
};

struct PipelineReg {
    Instruction instr;
    bool empty = true;
};

struct Config {
    bool forwarding;
    int add_latency;
    int mul_latency;
};
extern int cycles;
extern int stalls;
extern int instructions_executed;

#endif
