# Dynamic-Instruction-Scheduling-Simulator

Constructed a simulator for an out-of-order superscalar processor based on Tomasulo’s algorithm that fetches, dispatches, and issues N instructions per cycle.
## Inputs to Simulator

The simulator reads a trace file in the following format:  
\<hex branch PC\> t|n  
\<hex branch PC\> t|n  
...
Where:
* \<hex branch PC\> is the address of the branch instruction in memory. This field is used to index into predictors.
* "t" indicates the branch is actually taken (Note! Not that it is predicted taken!). Similarly, "n" indicates the branch is actually not-taken.  
Example:  
00a3b5fc t  
00a3b604 t  
00a3b60c n  
...
  
## Outputs from Simulator

The simulator outputs the following measurements after completion of the run:  
Without BTB:
1. number of accesses to the predictor (i.e., number of branches)
2. number of branch mispredictions (predicted taken when not-taken, or predicted not-taken when taken)
3. branch misprediction rate (# mispredictions/# branches)  

With BTB:
1. size of BTB
2. number of branches
3. number of predictions from Branch Predictor (number of BTB hits)
4. number of mispredictions from Branch Predictor (predicted taken when not-taken, or predicted not-taken when taken)
5. number of banches miss in BTB and taken (number of mispredictions due to BTB miss)
6. total number of mispredictions (item d + item e)
7. branch misprediction rate (item f / item b)

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
