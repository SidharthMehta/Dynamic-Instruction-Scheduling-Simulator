# Dynamic-Instruction-Scheduling-Simulator

Constructed a simulator for an out-of-order superscalar processor based on Tomasulo’s algorithm that fetches, dispatches, and issues N instructions per cycle.
## Inputs to Simulator

The simulator reads a trace file in the following format:  
\<PC\> \<operation type\> \<dest reg #\> \<src1 reg #\> \<src2 reg #\> \<mem address\>  
\<PC\> \<operation type\> \<dest reg #\> \<src1 reg #\> <src2 reg #\> \<mem address\>  
...  
Where:
* \<PC\> is the program counter of the instruction (in hex).
* \<operation type\> is either “0”, “1”, or “2”.
* \<dest reg #\> is the destination register of the instruction. If it is -1, then the instruction does not have a destination register (for example, a conditional branch instruction). Otherwise, it is between 0 and 127.
* \<src1 reg #\> is the first source register of the instruction. If it is -1, then the instruction does not have a first source register. Otherwise, it is between 0 and 127.
* \<src2 reg #\> is the second source register of the instruction. If it is -1, then the instruction does not have a second source register. Otherwise, it is between 0 and 127.
* \<mem address\> is the memory address for memory access instructions. If it is 0, then it’s not a memory access instruction. Otherwise, it is a hex address.  
For example:  
ab120024 0 1 2 3 0  
ab120028 1 -1 1 3 0  
ab12002c 2 1 4 -1 ffe04540  
Means:  
“operation type 0” R1, R2, R3, 0  
“operation type 1” -, R1, R3, 0 // no destination register!  
“operation type 2” R1, R4, - , ffe04540 // memory access instruction  
  
## Outputs from Simulator
The simulator outputs the following measurements after completion of the run:  
1. Total number of instructions in the trace.
2. Total number of cycles to finish the program.
3. Average number of instructions completed per cycle (IPC).  

The simulator also outputs:  
1. Number of L1 accesses
2. Number of L1 misses
3. Contents of L1 cache
4. Number of L2 accesses (if L2 cache size is not 0)
5. Number of L2 misses
6. Contents of L2 cache

## Running the simulator

Simulator accepts command-line arguments as follows:  
**$./sim \<S> \<N> \<BLOCKSIZE> <L1_size> <L1_ASSOC> <L2_SIZE> <L2_ASSOC> \<tracefile>**
* S: Scheduling Queue size.
* N: Peak fetch, dispatch, and issue rate.
* BLOCKSIZE: Positive integer. Block size in bytes. (Same block size for all caches in the memory hierarchy.)
* L1_SIZE: Positive integer. L1 cache size in bytes.
* L1_ASSOC: Positive integer. L1 set-associativity (1 is direct-mapped).
* L2_SIZE: Positive integer. L2 cache size in bytes. L2_SIZE = 0 signifies that there is no L2 cache.
* L2_ASSOC: Positive integer. L2 set-associativity (1 is direct-mapped).
* tracefile: is the filename of the input trace.
Note: BLOCKSIZE, L1_size, L1_ASSOC, L2_SIZE, and L2_ASSOC are all 0 if no data caches are modeled.
