#include "structures.h"

int registers[32] = {0};
int memory[1024];

int PC = 0;

int cycles = 0;
int stalls = 0;
int instructions_executed = 0;

PipelineReg IF_ID;
PipelineReg ID_EX;
PipelineReg EX_MEM;
PipelineReg MEM_WB;

void IF_stage(vector<Instruction>& program){

    if(PC < program.size()){

        IF_ID.instr = program[PC];
        IF_ID.empty = false;

        PC++;
    }
}

void ID_stage(){

    if(IF_ID.empty) return;

    ID_EX = IF_ID;

    IF_ID.empty = true;
}

void EX_stage(){

    if(ID_EX.empty) return;

    Instruction ins = ID_EX.instr;

    if(ins.opcode=="add"){

        registers[ins.rd] =
        registers[ins.rs1] + registers[ins.rs2];
    }

    if(ins.opcode=="sub"){

        registers[ins.rd] =
        registers[ins.rs1] - registers[ins.rs2];
    }

    EX_MEM = ID_EX;

    ID_EX.empty = true;
}

void MEM_stage(){

    if(EX_MEM.empty) return;

    Instruction ins = EX_MEM.instr;

    if(ins.opcode=="lw"){

        registers[ins.rd] =
        memory[registers[ins.rs1] + ins.imm];
    }

    if(ins.opcode=="sw"){

        memory[registers[ins.rs1] + ins.imm] =
        registers[ins.rd];
    }

    MEM_WB = EX_MEM;

    EX_MEM.empty = true;
}

void WB_stage(){

    if(MEM_WB.empty) return;

    instructions_executed++;

    MEM_WB.empty = true;
}

void run(vector<Instruction>& program){

    while(PC < program.size() ||
          !IF_ID.empty ||
          !ID_EX.empty ||
          !EX_MEM.empty ||
          !MEM_WB.empty){

        cycles++;

        WB_stage();
        MEM_stage();
        EX_stage();
        ID_stage();
        IF_stage(program);
    }
}
