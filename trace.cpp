#include "structures.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using std::string;

static string trim(const string& s)
{
	const auto a = s.find_first_not_of(" \t\r\n");
	if (a == string::npos) return {};
	const auto b = s.find_last_not_of(" \t\r\n");
	return s.substr(a, b - a + 1);
}

static int parse_reg_token(string tok)
{
	if (tok.empty()) return 0;
	if (tok.back() == ',') tok.pop_back();
	if (!tok.empty() && tok[0] == 'x') tok = tok.substr(1);
	return stoi(tok);
}

vector<Instruction> load_trace(const string& filename)
{
	vector<Instruction> program;
	std::ifstream file(filename);
	if (!file) return program;

	string line;
	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#') continue;

		std::istringstream ss(line);
		string op;
		ss >> op;
		if (op.empty()) continue;

		std::transform(op.begin(), op.end(), op.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		if (op == "l")
		{
			string addr_tok, reg_tok;
			ss >> addr_tok >> reg_tok;
			Instruction ins{};
			ins.opcode = "lw";
			ins.trace_mem = true;
			ins.trace_va = static_cast<uint32_t>(std::stoul(addr_tok, nullptr, 0));
			ins.rd = parse_reg_token(reg_tok);
			ins.rs1 = -1;
			ins.rs2 = -1;
			program.push_back(ins);
		}
		else if (op == "s")
		{
			string addr_tok, reg_tok;
			ss >> addr_tok >> reg_tok;
			Instruction ins{};
			ins.opcode = "sw";
			ins.trace_mem = true;
			ins.trace_va = static_cast<uint32_t>(std::stoul(addr_tok, nullptr, 0));
			ins.rd = parse_reg_token(reg_tok);
			ins.rs1 = -1;
			ins.rs2 = -1;
			program.push_back(ins);
		}
		else if (op == "add")
		{
			string a, b, c;
			ss >> a >> b >> c;
			Instruction ins{};
			ins.opcode = "add";
			ins.rd = parse_reg_token(a);
			ins.rs1 = parse_reg_token(b);
			ins.rs2 = parse_reg_token(c);
			program.push_back(ins);
		}
		else if (op == "mul")
		{
			string a, b, c;
			ss >> a >> b >> c;
			Instruction ins{};
			ins.opcode = "mul";
			ins.rd = parse_reg_token(a);
			ins.rs1 = parse_reg_token(b);
			ins.rs2 = parse_reg_token(c);
			program.push_back(ins);
		}
	}
	return program;
}
