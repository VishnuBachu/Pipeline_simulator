#include <sstream>
#include "structures.h"
#include <fstream>
#include <algorithm>
#include <cctype>

static string normalize_key(string key)
{
	std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){ return static_cast<char>(tolower(c)); });
	return key;
}

Config load_config(string filename)
{
	Config cfg{};
	cfg.forwarding = true;
	cfg.add_latency = 1;
	cfg.mul_latency = 3;
	cfg.main_memory_latency = 50;

	cfg.l1i_size = 1024;
	cfg.l1i_block_size = 64;
	cfg.l1i_associativity = 1;
	cfg.l1i_latency = 1;

	cfg.l1d_size = 1024;
	cfg.l1d_block_size = 64;
	cfg.l1d_associativity = 1;
	cfg.l1d_latency = 1;

	cfg.l2_size = 4096;
	cfg.l2_block_size = 64;
	cfg.l2_associativity = 4;
	cfg.l2_latency = 8;

	cfg.replacement_policy_l1 = "LRU";
	cfg.replacement_policy_l2 = "FIFO";

	ifstream file(filename);
	string key;
	while(file >> key)
	{
		string nkey = normalize_key(key);
        	if(nkey=="forwarding")
		{
        		string value;
            		file >> value;
			string nvalue = normalize_key(value);
           		cfg.forwarding = (nvalue=="true");
        	}

        	else if(nkey=="add_latency")
		{
        		file >> cfg.add_latency;
        	}
        	else if(nkey=="mul_latency")
		{
        		file >> cfg.mul_latency;
        	}
        	else if(nkey=="main_memory_latency" || nkey=="memory_latency" || nkey=="main_memory_access_time")
		{
			file >> cfg.main_memory_latency;
		}
		else if(nkey=="l1i_size")
		{
			file >> cfg.l1i_size;
		}
		else if(nkey=="l1i_block_size")
		{
			file >> cfg.l1i_block_size;
		}
		else if(nkey=="l1i_associativity")
		{
			file >> cfg.l1i_associativity;
		}
		else if(nkey=="l1i_latency")
		{
			file >> cfg.l1i_latency;
		}
		else if(nkey=="l1d_size")
		{
			file >> cfg.l1d_size;
		}
		else if(nkey=="l1d_block_size")
		{
			file >> cfg.l1d_block_size;
		}
		else if(nkey=="l1d_associativity")
		{
			file >> cfg.l1d_associativity;
		}
		else if(nkey=="l1d_latency")
		{
			file >> cfg.l1d_latency;
		}
		else if(nkey=="l2_size")
		{
			file >> cfg.l2_size;
		}
		else if(nkey=="l2_block_size")
		{
			file >> cfg.l2_block_size;
		}
		else if(nkey=="l2_associativity")
		{
			file >> cfg.l2_associativity;
		}
		else if(nkey=="l2_latency")
		{
			file >> cfg.l2_latency;
		}
		else if(nkey=="replacement_policy_l1")
		{
			file >> cfg.replacement_policy_l1;
		}
		else if(nkey=="replacement_policy_l2")
		{
			file >> cfg.replacement_policy_l2;
		}
		else if(nkey=="cache_size")
		{
			int v;
			file >> v;
			cfg.l1i_size = v;
			cfg.l1d_size = v;
		}
		else if(nkey=="block_size")
		{
			int v;
			file >> v;
			cfg.l1i_block_size = v;
			cfg.l1d_block_size = v;
			cfg.l2_block_size = v;
		}
		else if(nkey=="associativity")
		{
			int v;
			file >> v;
			cfg.l1i_associativity = v;
			cfg.l1d_associativity = v;
		}
		else if(nkey=="cache_latency" || nkey=="access_latency")
		{
			int v;
			file >> v;
			cfg.l1i_latency = v;
			cfg.l1d_latency = v;
		}
		else if(nkey=="replacement_policy")
		{
			string p;
			file >> p;
			cfg.replacement_policy_l1 = p;
			cfg.replacement_policy_l2 = p;
		}
        	else if(nkey=="array")
		{
                	int val;
                	int idx = 0;

                	string line;
                	getline(file,line);
               		stringstream ss(line);

            		while(ss >> val)
			{
                		memory[idx++] = val;
            		}
        	}
        }
        return cfg;
}
