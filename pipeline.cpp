#include "structures.h"
#include "cache.h"
#include "vm.h"

int registers[32] = {0};
int memory[1024];
int ex_cycles_remaining = 0;
Instruction current_ex_instr;
int PC = 0;

int cycles = 0;
int stalls = 0;
bool stall_pipeline = false;
int instructions_executed = 0;

namespace
{
bool stall_this_tick = false;
inline void note_stall()
{
	stall_this_tick = true;
}
} // namespace

PipelineReg IF_ID;
PipelineReg ID_EX;
PipelineReg EX_MEM;
PipelineReg MEM_WB;

bool if_pending = false;
int if_pending_pc = -1;
int if_cycles_remaining = 0;

int mem_cycles_remaining = 0;
bool mem_latency_started = false;

bool writes_to_reg(const Instruction& ins)
{
	return (ins.opcode=="add" || ins.opcode=="sub" ||
	ins.opcode=="addi" || ins.opcode=="lw" ||
        ins.opcode=="jal" || ins.opcode=="mul" ||
	ins.opcode=="li");
}

bool uses_rs1(const Instruction& ins)
{
	return (ins.opcode=="add" || ins.opcode=="sub" ||
            ins.opcode=="addi" || ins.opcode=="lw" ||
            ins.opcode=="sw" || ins.opcode=="bne" ||
            ins.opcode=="mul");
}

bool uses_rs2(const Instruction& ins)
{
	return (ins.opcode=="add" || ins.opcode=="sub" ||
            ins.opcode=="sw" || ins.opcode=="bne" ||
            ins.opcode=="mul");
}

int get_reg_value(int reg)
{
	if(!EX_MEM.empty &&
	   EX_MEM.instr.rd == reg &&
           writes_to_reg(EX_MEM.instr))
        	return EX_MEM.alu_result;

	if(!MEM_WB.empty &&
	   MEM_WB.instr.rd == reg &&
           writes_to_reg(MEM_WB.instr))
        	return MEM_WB.alu_result;

	return registers[reg];
}

void flush_pending_if()
{
	if_pending = false;
	if_pending_pc = -1;
	if_cycles_remaining = 0;
}

void IF_stage(vector<Instruction>& program)
{
	if(stall_pipeline)
	{
        	stall_pipeline = false;
        	return;
	}

	if(if_pending)
	{
		if_cycles_remaining--;
		if(if_cycles_remaining > 0)
		{
			note_stall();
			return;
		}

		if(IF_ID.empty && if_pending_pc >= 0 && if_pending_pc < static_cast<int>(program.size()))
		{
			IF_ID.instr = program[if_pending_pc];
			IF_ID.empty = false;
			PC = if_pending_pc + 1;
		}
		flush_pending_if();
		return;
	}

	if(!IF_ID.empty) return;
	if(PC >= static_cast<int>(program.size())) return;

	const int latency = trace_replay_mode
		? 1
		: access_instruction_cache(static_cast<unsigned int>(PC * 4));
	if(latency <= 1)
	{
		IF_ID.instr = program[PC];
		IF_ID.empty = false;
		PC++;
	}
	else
	{
		if_pending = true;
		if_pending_pc = PC;
		if_cycles_remaining = latency - 1;
		note_stall();
	}
}

void ID_stage()
{
	if(IF_ID.empty) return;
	if(!ID_EX.empty)
	{
		note_stall();
		return;
	}

	Instruction ins = IF_ID.instr;
	if(!config.forwarding)
	{
		if(!EX_MEM.empty && writes_to_reg(EX_MEM.instr))
		{
			int rd = EX_MEM.instr.rd;
        	        if((uses_rs1(ins) && ins.rs1 == rd) ||
        	        (uses_rs2(ins) && ins.rs2 == rd))
			{
        	        	stall_pipeline = true;
        	        	note_stall();
        	        	return;
        	    	}
        	}
    	}

	ID_EX = IF_ID;
	IF_ID.empty = true;
}

void EX_stage()
{
	if(ex_cycles_remaining > 0)
	{
        	ex_cycles_remaining--;
        	if(ex_cycles_remaining > 0) return;
	}
	if(ID_EX.empty) return;
	if(!EX_MEM.empty)
	{
		note_stall();
		return;
	}

    	Instruction ins = ID_EX.instr;
	current_ex_instr = ins;

	int v1 = registers[ins.rs1];
	int v2 = registers[ins.rs2];

	if(config.forwarding)
	{
        	v1 = get_reg_value(ins.rs1);
        	v2 = get_reg_value(ins.rs2);
	}

	int result = 0;
	if(ins.opcode=="add")
	{
        	result = v1 + v2;
        	ex_cycles_remaining = config.add_latency;
	}
	else if(ins.opcode=="sub")
	{
        	result = v1 - v2;
        	ex_cycles_remaining = config.add_latency;
	}
	else if(ins.opcode=="mul")
	{
        	result = v1 * v2;
        	ex_cycles_remaining = config.mul_latency;
	}
	else if(ins.opcode=="addi")
	{
        	result = v1 + ins.imm;
        	ex_cycles_remaining = config.add_latency;
	}
	else if(ins.opcode=="li")
	{
		result = ins.imm;
		ex_cycles_remaining = config.add_latency;
	}
	else if(ins.opcode=="bne")
	{
        	if(v1 != v2)
		{
    			PC = ins.imm;
        		IF_ID.empty = true;
			flush_pending_if();
       			note_stall();
		}
	}
	else if(ins.opcode=="jal")
	{
        	registers[ins.rd] = PC;
        	PC = ins.imm;
        	IF_ID.empty = true;
		flush_pending_if();
        	note_stall();
	}

	EX_MEM = ID_EX;
	EX_MEM.alu_result = result;
	ID_EX.empty = true;
}

void MEM_stage()
{
	if(EX_MEM.empty) return;
	if(!MEM_WB.empty)
	{
		note_stall();
		return;
	}

	Instruction ins = EX_MEM.instr;
	if((ins.opcode=="lw" || ins.opcode=="sw") && !mem_latency_started)
	{
		const uint32_t va_byte = ins.trace_mem
			? ins.trace_va
			: static_cast<uint32_t>(registers[ins.rs1] + ins.imm);
		uint32_t pa_byte = va_byte;
		int trans_cycles = 0;
		if (trace_replay_mode)
			trans_cycles = vm_translate_access(va_byte, ins.opcode == "sw", &pa_byte);
		EX_MEM.mem_phys_addr = pa_byte;
		mem_cycles_remaining = trans_cycles + access_data_cache(pa_byte);
		mem_latency_started = true;
	}

	if((ins.opcode=="lw" || ins.opcode=="sw") && mem_cycles_remaining > 1)
	{
		mem_cycles_remaining--;
		note_stall();
		return;
	}

	MEM_WB = EX_MEM;
	if(ins.opcode=="lw")
	{
		const uint32_t pa_byte = EX_MEM.mem_phys_addr;
		int val = 0;
		if (trace_replay_mode)
		{
			if (!phys_read_word(pa_byte, &val)) val = 0;
			MEM_WB.alu_result = val;
		}
		else
		{
			const int addr = static_cast<int>(pa_byte) / 4;
			if (addr >= 0 && addr < 1024)
				MEM_WB.alu_result = memory[addr];
			else
				MEM_WB.alu_result = 0;
		}
	}
	else if(ins.opcode=="sw")
	{
		const uint32_t pa_byte = EX_MEM.mem_phys_addr;
		if (trace_replay_mode)
			phys_write_word(pa_byte, registers[ins.rd]);
		else
		{
			const int addr = static_cast<int>(pa_byte) / 4;
			if (addr >= 0 && addr < 1024)
				memory[addr] = registers[ins.rd];
		}
		MEM_WB.alu_result = 0;
	}
	else
	{
		MEM_WB.alu_result = EX_MEM.alu_result;
	}

	mem_cycles_remaining = 0;
	mem_latency_started = false;
	EX_MEM.empty = true;
}

void WB_stage()
{
	if(MEM_WB.empty) return;
	Instruction ins = MEM_WB.instr;
	if(writes_to_reg(ins) && ins.opcode != "sw")
	{
        	registers[ins.rd] = MEM_WB.alu_result;
	}
	instructions_executed++;
	MEM_WB.empty = true;
}

void run(vector<Instruction>& program)
{
	while(PC < static_cast<int>(program.size()) ||
	      !IF_ID.empty ||
	      !ID_EX.empty ||
	      !EX_MEM.empty ||
	      !MEM_WB.empty ||
	      if_pending)
	{
		stall_this_tick = false;
		cycles++;
		WB_stage();
		MEM_stage();
        	EX_stage();
        	ID_stage();
        	IF_stage(program);
		if (stall_this_tick)
			stalls++;
	}
}
