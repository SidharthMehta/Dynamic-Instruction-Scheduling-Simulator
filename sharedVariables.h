#ifndef SHARED_VARIABLES
#define SHARED_VARIABLES

#include <iostream>
#include <string.h>
#include <fstream>
#include <list>
#include <vector>
#include "customDataTypes.h"
#include "registerEntity.h"

using namespace std;

extern RegisterEntity *registerFile;

// Arguments
extern unsigned S;         // Scheduling Queue size.
extern unsigned N;         // N: Peak fetch, dispatch, and issue rate.
extern unsigned BLOCKSIZE; // Positive integer. Block size in bytes. (Same block size for all caches in the memory hierarchy.)
extern unsigned L1_SIZE;   // Positive integer. L1 cache size in bytes.
extern unsigned L1_ASSOC;  // Positive integer. L1 set-associativity (1 is direct-mapped).
extern unsigned L2_SIZE;   // Positive integer. L2 cache size in bytes. L2_SIZE = 0 signifies that there is no L2 cache.
extern unsigned L2_ASSOC;  // Positive integer. L2 set-associativity (1 is direct-mapped).
extern string tracefile;   // Filename of the input trace.

extern ifstream trace;
extern list<Instruction> fakeROB;
extern list<Dispatch> dispatchQueue;
extern list<Dispatch> tempDispatchQueue;
extern IssueQueue issueQueue;
extern vector<Issue> tempIssueQueue;
extern list<Execute> executeQueue;

extern unsigned instructionCount;
extern unsigned numberOfCycles;
extern bool endOfFile;
#endif