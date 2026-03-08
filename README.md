# COA_PROJ
Meeting Minutes
Meeting 6

Date: 8 March 2026
Members: Rudresh Prasad, Bachu Vishnu

Decisions:

Final testing of the simulator using bubble sort.
Added array initialization through the config file.
Verified stalls, cycles, and IPC calculations.

Tasks:

Rudresh: debug branch behavior and stall counting.
Vishnu : run test cases and verify outputs.

Meeting 5

Date: 7 March 2026
Members: Rudresh Prasad, Bachu Vishnu

Decisions:

Implemented hazard detection logic.
Added forwarding logic from EX/MEM and MEM/WB stages.
Implemented stall mechanism for load-use hazards.

Tasks:

Rudresh: test forwarding enabled/disabled cases.
Vishnu: verify pipeline execution with sample programs.

Meeting 4

Date: 5 March 2026
Members: Rudresh Prasad, Bachu Vishnu

Decisions:

Completed implementation of pipeline stages:
IF
ID
EX
MEM
WB
Implemented pipeline registers between stages.

Tasks:

Rudresh: implement execution logic for instructions.
Vishnu: implement memory stage.

Meeting 3

Date: 3 March 2026
Members: Rudresh Prasad, Bachu vishnu

Decisions:

Implemented assembly parser.
Added support for labels and branching instructions.
Verified instruction parsing for add, sub, lw, sw, bne.

Tasks:

Rudresh: extend parser for remaining instructions.
Vishnu: begin implementing pipeline execution.

Meeting 2

Date: 28 February 2026
Members: Rudresh Prasad, Bachu Vishnu

Decisions:

Designed data structures for instructions and pipeline registers.
Decided to use a vector to store program instructions.
Planned the structure of the simulator modules.

Tasks:

Rudresh: implement instruction structure and parsing.
Vishnu: design pipeline registers.

Meeting 1

Date: 25 February 2026
Members: Rudresh Prasad, Bachu Vishnu

Decisions:

Chose C++ as the programming language.
Decided to implement a 5-stage pipeline.
Reviewed RISC-V ISA and selected instructions required for the simulator.

Tasks:

Rudresh: begin instruction parsing implementation.
Vishnu: research pipeline implementation strategies.
