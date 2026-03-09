#include "structures.h"

Config load_config(string filename);
vector<Instruction> load_program(string filename);
void run(vector<Instruction>& program);

Config config;

int main()
{
	config = load_config("config.txt");   

	vector<Instruction> program = load_program("program.asm");
	run(program);

	cout << "Execution finished\n";
	cout << "Cycles: " << cycles << endl;
	cout << "Instructions: " << instructions_executed << endl;
	cout << "Stalls: " << stalls << endl;

	double IPC = (double)instructions_executed / cycles;

	cout << "IPC: " << IPC << endl;
}
