#include "structures.h"
#include <fstream>
#include <sstream>

Instruction parse_line(string line){

    Instruction ins;

    stringstream ss(line);

    ss >> ins.opcode;

    if(ins.opcode=="add" || ins.opcode=="sub"){

        char c;

        ss >> c >> ins.rd >> c >> c >> ins.rs1 >> c >> c >> ins.rs2;
    }

    return ins;
}

vector<Instruction> load_program(string filename){

    vector<Instruction> program;

    ifstream file(filename);

    string line;

    while(getline(file,line)){

        if(line.empty()) continue;

        program.push_back(parse_line(line));
    }

    return program;
}
