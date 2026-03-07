#include "structures.h"
#include <fstream>

Config load_config(string filename){

    Config cfg;

    ifstream file(filename);
    string key,value;

    while(file >> key >> value){

        if(key=="forwarding")
            cfg.forwarding = (value=="true");

        if(key=="ADD_latency")
            cfg.add_latency = stoi(value);

        if(key=="MUL_latency")
            cfg.mul_latency = stoi(value);
    }

    return cfg;
}