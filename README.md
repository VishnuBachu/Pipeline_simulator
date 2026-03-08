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
* Cycle, stall, and IPC calculation

---

# How to Run

## Compile

```bash
g++ main.cpp pipeline.cpp parser.cpp config.cpp -o simulator
```

## Run

```bash
./simulator
```

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

