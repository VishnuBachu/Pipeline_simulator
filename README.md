# COA Pipeline Simulator

A C++ based simulator for a **5-stage RISC-V pipeline** developed as part of the **COA project**.

The simulator models the following pipeline stages:

* Instruction Fetch (IF)
* Instruction Decode (ID)
* Execute (EX)
* Memory Access (MEM)
* Write Back (WB)

Features implemented:

* Assembly instruction parser
* Pipeline stage simulation
* Data hazard detection
* Data forwarding
* Stall handling for load-use hazards
* Two-level cache model:
  * L1I (instruction cache)
  * L1D (data cache)
  * Unified L2 cache
* Replacement policies: **LRU** and **FIFO**
* Variable memory latency for:
  * Instruction fetches
  * Loads and stores
* Cycle, stall, cache miss rate, and IPC calculation

---

# How to Run

## Compile

```bash
g++ main.cpp pipeline.cpp parser.cpp config.cpp cache.cpp -o simulator
```

## Run

```bash
./simulator
```

For Windows PowerShell:

```powershell
g++ main.cpp pipeline.cpp parser.cpp config.cpp cache.cpp -o simulator.exe
.\simulator.exe
```

---

# Configuration

The simulator reads parameters from `config.txt`.

Supported keys:

* `forwarding` (`true`/`false`)
* `ADD_latency`
* `MUL_latency`
* `main_memory_latency`
* `L1I_size`, `L1I_block_size`, `L1I_associativity`, `L1I_latency`
* `L1D_size`, `L1D_block_size`, `L1D_associativity`, `L1D_latency`
* `L2_size`, `L2_block_size`, `L2_associativity`, `L2_latency`
* `replacement_policy_L1` (`LRU` or `FIFO`)
* `replacement_policy_L2` (`LRU` or `FIFO`)
* `array` (initial memory values)

Example:

```txt
forwarding true
ADD_latency 1
MUL_latency 3
main_memory_latency 50

L1I_size 1024
L1I_block_size 64
L1I_associativity 1
L1I_latency 1

L1D_size 1024
L1D_block_size 64
L1D_associativity 1
L1D_latency 1

L2_size 4096
L2_block_size 64
L2_associativity 4
L2_latency 8

replacement_policy_L1 LRU
replacement_policy_L2 FIFO

array 9 4 3 1 7
```

---

# Output Metrics

At the end of execution, the simulator prints:

* `Cycles`
* `Instructions`
* `Stalls`
* `Cache miss rate`
* `IPC` (Instructions Per Cycle)
 
---

# Meeting Minutes

## Meeting 6

**Date:** 8 March 2026
**Members:** Rudresh Prasad, Bachu Vishnu

### Decisions

* Final testing of the simulator using bubble sort.
* Added array initialization through the config file.
* Verified stalls, cycles, and IPC calculations.

### Tasks

* **Rudresh:** Debug branch behavior and stall counting.
* **Vishnu:** Run test cases and verify outputs.

---

## Meeting 5

**Date:** 7 March 2026
**Members:** Rudresh Prasad, Bachu Vishnu

### Decisions

* Implemented hazard detection logic.
* Added forwarding logic from **EX/MEM** and **MEM/WB** stages.
* Implemented stall mechanism for load-use hazards.

### Tasks

* **Rudresh:** Test forwarding enabled/disabled cases.
* **Vishnu:** Verify pipeline execution with sample programs.

---

## Meeting 4

**Date:** 5 March 2026
**Members:** Rudresh Prasad, Bachu Vishnu

### Decisions

* Completed implementation of pipeline stages:

  * IF
  * ID
  * EX
  * MEM
  * WB
* Implemented pipeline registers between stages.

### Tasks

* **Rudresh:** Implement execution logic for instructions.
* **Vishnu:** Implement memory stage.

---

## Meeting 3

**Date:** 3 March 2026
**Members:** Rudresh Prasad, Bachu Vishnu

### Decisions

* Implemented assembly parser.
* Added support for labels and branching instructions.
* Verified instruction parsing for `add`, `sub`, `lw`, `sw`, `bne`.

### Tasks

* **Rudresh:** Extend parser for remaining instructions.
* **Vishnu:** Begin implementing pipeline execution.

---

## Meeting 2

**Date:** 28 February 2026
**Members:** Rudresh Prasad, Bachu Vishnu

### Decisions

* Designed data structures for instructions and pipeline registers.
* Decided to use a **vector** to store program instructions.
* Planned the structure of the simulator modules.

### Tasks

* **Rudresh:** Implement instruction structure and parsing.
* **Vishnu:** Design pipeline registers.

---

## Meeting 1

**Date:** 25 February 2026
**Members:** Rudresh Prasad, Bachu Vishnu

### Decisions

* Chose **C++** as the programming language.
* Decided to implement a **5-stage pipeline simulator**.
* Reviewed the **RISC-V ISA** and selected instructions required for the simulator.

### Tasks

* **Rudresh:** Begin instruction parsing implementation.
* **Vishnu:** Research pipeline implementation strategies.

---

