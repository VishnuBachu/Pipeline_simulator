#include <sstream>
#include "structures.h"
#include <fstream>
Config load_config(string filename)
{
	Config cfg;
	ifstream file(filename);
	string key;
	while(file >> key)
	{
        	if(key=="forwarding")
		{
        		string value;
            		file >> value;
           		cfg.forwarding = (value=="true");
        	}

        	else if(key=="ADD_latency")
		{
        		file >> cfg.add_latency;
        	}
        	else if(key=="MUL_latency")
		{
        		file >> cfg.mul_latency;
        	}
        	else if(key=="array")
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
