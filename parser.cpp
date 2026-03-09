#include "structures.h"
#include <fstream>
#include <sstream>
#include <map>

map<string,int> labels;
Instruction parse_line(string line)
{
	Instruction ins{}; //init all fields to zero

	stringstream ss(line);

	ss >> ins.opcode;

	if(ins.opcode=="add" || ins.opcode=="sub")
	{
        	char c;
        	ss >> c >> ins.rd >> c >> c >> ins.rs1 >> c >> c >> ins.rs2;
        }

        if(ins.opcode=="addi")
	{
        	char c;
        	ss >> c >> ins.rd >> c >> c >> ins.rs1 >> c >> ins.imm;
        }
        if(ins.opcode=="mul")
	{
        	char c;
        	ss >> c >> ins.rd >> c >> c >> ins.rs1 >> c >> c >> ins.rs2;
	}  
	if(ins.opcode=="lw" || ins.opcode=="sw")
	{
        	char c;
        	ss >> c >> ins.rd >> c >> ins.imm >> c >> c >> ins.rs1 >> c;
	}
	if(ins.opcode=="li"){
        	char c;
		ss >> c >> ins.rd >> c >> ins.imm;
	}
	if(ins.opcode=="blt")
	{
        	string r1, r2, label;

        	ss >> r1 >> r2 >> label;

        	if(r1.back()==',') r1.pop_back();
        	if(r2.back()==',') r2.pop_back();

        	r1 = r1.substr(1);
        	r2 = r2.substr(1);

        	ins.rs1 = stoi(r1);
        	ins.rs2 = stoi(r2);

        	ins.imm = labels[label];
	}
	if(ins.opcode=="bne")
	{
		string r1, r2, label;

		ss >> r1 >> r2 >> label;

		if(r1.back()==',') r1.pop_back();
		if(r2.back()==',') r2.pop_back();

		r1 = r1.substr(1);   // remove x
		r2 = r2.substr(1);

		ins.rs1 = stoi(r1);
		ins.rs2 = stoi(r2);

        	ins.imm = labels[label];
	}

	if(ins.opcode=="jal")
	{
        	char c;
        	string label;
        	ss >> c >> ins.rd >> c >> label;
        	ins.imm = labels[label];
	}

	return ins;
}
vector<Instruction> load_program(string filename)
{
	vector<Instruction> program;
	ifstream file(filename);
	string line;
	
	int line_number = 0;

	// first pass is for collect labels
	while(getline(file,line))
	{
        	if(line.empty()) continue;
        	if(line[0]=='#') continue;
		if(line[0]=='.') continue;   // skips .text .data .globl
        	size_t pos = line.find(':');

        	if(pos != string::npos)
		{
        		string label = line.substr(0,pos);
        		labels[label] = line_number;
        	}

        	else
		{
            		line_number++;
        	}
	}

	file.clear();
	file.seekg(0);
	// second pass: parse instructions
	while(getline(file,line))
	{
		if(line.empty()) continue;
	        if(line[0]=='#') continue;
		if(line[0]=='.') continue;   // again skips .text .data .globl

	        size_t pos = line.find(':');

	        if(pos != string::npos)
		{
	        	line = line.substr(pos+1);
			line.erase(0, line.find_first_not_of(" \t"));
	        }
	
	        if(line.empty()) continue;
	
	        program.push_back(parse_line(line));
	}

	return program;
}
