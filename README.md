# COA Pipeline Simulator

A C++ simulator for a **5-stage RISC-V–style pipeline** (COA project).

Pipeline stages:

* Instruction Fetch (IF)
* Instruction Decode (ID)
* Execute (EX)
* Memory Access (MEM)
* Write Back (WB)

## Features

* Assembly parser and labeled RISC-V-style programs (`program.asm` or a path you pass)
* Data hazards, optional **forwarding**, load-use stalls
* **Two-level cache** (configurable): L1I, L1D, unified L2; **LRU** and **FIFO** replacement
* Variable latency for instruction fetch and data accesses (L1 → L2 → main memory)
* **Phase 3 — trace replay** with **virtual memory**:
  * DTLB, flat page table, page walk on TLB miss, page fault on first touch
  * Finite physical memory, frame allocation, page eviction (FIFO or LRU)
  * Dirty page handling and writeback accounting
  * Loads/stores use **PIPT** data cache (translate to physical address, then L1D on PA)
  * Trace mode: instruction fetch is **not** modeled through the instruction cache (per Phase 3 spec)

## Project files

| File | Role |
|------|------|
| `main.cpp` | Entry point, mode selection, metrics output |
| `parser.cpp` | Assembly parsing (`program.asm`) |
| `pipeline.cpp` | 5-stage pipeline simulation |
| `config.cpp` | Reads `config.txt` |
| `cache.cpp`, `cache.h` | L1I, L1D, L2 cache model |
| `vm.cpp`, `vm.h` | Virtual memory (trace mode) |
| `trace.cpp` | Trace file parsing and replay |
| `structures.h` | Shared types and globals |
| `config.txt` | Pipeline, cache, and assembly-mode memory init |
| `vm_config.ini` | Virtual memory settings (trace mode) |
| `program.asm` | Sample assembly program (bubble sort) |
| `trace01.trace` … `trace10.trace` | Phase 3 workload traces |

Requires a **C++17** compiler.

## Build

From the `COA_PROJ` directory:

```bash
g++ -std=c++17 -O2 -o simulator main.cpp parser.cpp pipeline.cpp config.cpp cache.cpp vm.cpp trace.cpp
```

## Run

### Assembly mode (default)

Uses `config.txt` in the current working directory. Loads `program.asm` if you do not pass an argument.

```bash
./simulator
```

Load a specific assembly file:

```bash
./simulator program.asm
```

### Phase 3 trace replay

First argument must be a file whose name ends with **`.trace`**. The simulator reads **`vm_config.ini`** from the current directory unless you pass a second path.

```bash
./simulator trace01.trace
```

Custom VM config path:

```bash
./simulator trace01.trace vm_config.ini
```

### Trace line format

Only these opcodes are supported in traces:

| Opcode | Meaning | Example |
|--------|---------|---------|
| `L` | 32-bit load | `L 0x10000000 x5` |
| `S` | 32-bit store | `S 0x10001000 x6` |
| `ADD` | Add (latency from `config.txt`) | `ADD x7 x5 x6` |
| `MUL` | Multiply (latency from `config.txt`) | `MUL x8 x7 x9` |

Addresses are **virtual** (byte). Registers use the `xN` form.

## Configuration

### `config.txt` (pipeline + caches)

Typical keys:

* `forwarding` (`true` / `false`)
* `ADD_latency`, `MUL_latency`, `main_memory_latency`
* `L1I_size`, `L1I_block_size`, `L1I_associativity`, `L1I_latency`
* `L1D_size`, `L1D_block_size`, `L1D_associativity`, `L1D_latency`
* `L2_size`, `L2_block_size`, `L2_associativity`, `L2_latency` — set **`L2_size 0`** to disable L2 (L1 miss goes straight to main memory)
* `replacement_policy_L1`, `replacement_policy_L2` (`LRU` or `FIFO`)
* `array` — rest of line: initial words in the small **simulated `memory[]` array** used in assembly mode (not the large physical RAM used in trace mode)

See `config.txt` in this repo for an example.

### `vm_config.ini` (Phase 3 virtual memory)

Used only in **trace** mode. Lines are `key = value` (comments with `#` or `;`). Example:

```ini
virtual_size_bytes = 536870912
physical_size_bytes = 262144
page_size_bytes = 4096
dtlb_entries = 16
tlb_hit_latency = 1
page_walk_latency = 10
page_fault_latency = 50
dirty_writeback_cycles = 0
replacement_policy = lru
```

* `replacement_policy` may be `lru` or `fifo`.
* `virtual_size_bytes` must cover the **highest virtual address** used in your traces (raise it if you fault or mis-translate on large VAs).

## Output

### Assembly mode

* `Cycles`, `Instructions`
* `Stalls` — count of cycles where **any** stage stalled (at most **one** stall counted per cycle, so `stalls` ≤ `cycles`)
* `Cache miss rate` (combined L1I + L1D access miss rate)
* `IPC`

### Trace mode (adds VM statistics)

* Same as above, plus:
  * `TLB hits` / `TLB misses`
  * `Page walks`, `Page faults`
  * `Page evictions`, `Dirty writebacks`
  * `Translation penalty cycles`

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
