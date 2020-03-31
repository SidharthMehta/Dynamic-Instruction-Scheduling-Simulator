#include <iostream>
#include <fstream>
#include <string.h>
#include <list>
#include <vector>
#include <iomanip>
#include <algorithm>
#include "customDataTypes.h"
#include "registerEntity.h"
#include "issueQueue.h"
#include "sharedVariables.h"
#include "CACHE.h"

using namespace std;

// Arguments
unsigned S;         // Scheduling Queue size.
unsigned N;         // N: Peak fetch, dispatch, and issue rate.
unsigned BLOCKSIZE; // Positive integer. Block size in bytes. (Same block size for all caches in the memory hierarchy.)
unsigned L1_SIZE;   // Positive integer. L1 cache size in bytes.
unsigned L1_ASSOC;  // Positive integer. L1 set-associativity (1 is direct-mapped).
unsigned L2_SIZE;   // Positive integer. L2 cache size in bytes. L2_SIZE = 0 signifies that there is no L2 cache.
unsigned L2_ASSOC;  // Positive integer. L2 set-associativity (1 is direct-mapped).
string tracefile;   // Filename of the input trace.
ifstream trace;
RegisterEntity *registerFile;

list<Instruction> fakeROB;
list<Dispatch> dispatchQueue;
IssueQueue issueQueue;
list<Execute> executeQueue;

CACHE L2;
CACHE L1;

unsigned instructionCount;
unsigned numberOfCycles;
bool endOfFile;

bool compare(Issue, Issue);

bool advanceCycle();
void fetch();
void dispatch();
void issue();
void execute();
void fakeRetire();
void getArguments(char **);

int main(int argc, char *argv[])
{
    getArguments(argv);

    registerFile = new RegisterEntity[128];
    issueQueue = IssueQueue(S);
    instructionCount = 0;
    numberOfCycles = 0;
    endOfFile = false;

    if (L2_SIZE != 0 && L1_SIZE != 0)
    {
        //Initialize L2 CACHE
        L2 = CACHE(BLOCKSIZE, L2_SIZE, L2_ASSOC, 1, 1, NULL, 10, 20);

        //Initialize L1 CACHE
        L1 = CACHE(BLOCKSIZE, L1_SIZE, L1_ASSOC, 1, 1, &L2, 5, 20);
    }
    else if (L1_SIZE != 0)
    {
        //Initialize L1 CACHE
        L1 = CACHE(BLOCKSIZE, L1_SIZE, L1_ASSOC, 1, 1, NULL, 5, 20);
    }

    //Read trace file
    trace.open(tracefile);
    if (!trace.is_open())
    {
        //If file cannot be read exit
        exit(0);
    }

    do
    {
        fakeRetire();
        execute();
        issue();
        dispatch();
        fetch();
    } while (advanceCycle());

    if (L1_SIZE != 0)
    {
        cout << "L1 CACHE CONTENTS";
        cout << "\na. number of accesses :" << dec << L1.READ;
        cout << "\nb. number of misses :" << dec << L1.READ_MISS;
        L1.CacheStatus();
        cout << "\n\n";
        if (L2_SIZE != 0)
        {
            cout << "L2 CACHE CONTENTS";
            cout << "\na. number of accesses :" << dec << L2.READ;
            cout << "\nb. number of misses :" << dec << L2.READ_MISS;
            L2.CacheStatus();
            cout << "\n\n";
        }
    }
    cout << "CONFIGURATION\n";
    cout << "\tsuperscalar bandwidth (N) = " << dec << N << "\n";
    cout << "\tdispatch queue size (2*N) = " << dec << 2 * N << "\n";
    cout << "\tschedule queue size (S)   = " << dec << S << "\n";
    cout << "RESULTS\n";
    cout << "\tnumber of instructions = " << dec << instructionCount << "\n";
    cout << "\tnumber of cycles = \t" << dec << numberOfCycles << "\n";
    cout << "\tIPC = \t\t\t" << fixed << setprecision(2) << (float)instructionCount / numberOfCycles;

    return 0;
}

void fakeRetire()
{
    while (fakeROB.size() > 0)
    {
        list<Instruction>::iterator iterROB = fakeROB.begin();
        if (iterROB->instructionState == WB)
        {
            cout << dec << iterROB->tag << " fu{" << iterROB->operation << "}"
                 << " src{" << iterROB->src1Register << "," << iterROB->src2Register << "}"
                 << " dst{" << iterROB->destRegister << "}"
                 << " IF{" << iterROB->IF.beginCycle << "," << iterROB->IF.endCycle - iterROB->IF.beginCycle << "}"
                 << " ID{" << iterROB->ID.beginCycle << "," << iterROB->ID.endCycle - iterROB->ID.beginCycle << "}"
                 << " IS{" << iterROB->IS.beginCycle << "," << iterROB->IS.endCycle - iterROB->IS.beginCycle << "}"
                 << " EX{" << iterROB->EX.beginCycle << "," << iterROB->EX.endCycle - iterROB->EX.beginCycle << "}"
                 << " WB{" << iterROB->WB.beginCycle << "," << iterROB->WB.endCycle - iterROB->WB.beginCycle << "}"
                 << "\n";
            fakeROB.erase(iterROB);
        }
        else
        {
            break;
        }
    }
}

void execute()
{
    if (executeQueue.size() > 0)
    {
        list<Instruction>::iterator iterROB;
        list<Execute>::iterator iter;
        for (iterROB = fakeROB.begin(); iterROB != fakeROB.end(); iterROB++)
        {
            Instruction *instructionEntity = &*iterROB;
            if (instructionEntity->isExecutionComplete(numberOfCycles))
            {
                instructionEntity->WB.beginCycle = numberOfCycles;
                instructionEntity->WB.endCycle = numberOfCycles + 1;
                instructionEntity->EX.endCycle = numberOfCycles;
                instructionEntity->instructionState = WB;
            }
        }

        for (iterROB = fakeROB.begin(); iterROB != fakeROB.end(); iterROB++)
        {
            Instruction *instructionEntity = &*iterROB;
            if (instructionEntity->instructionState == WB)
            {
                for (iter = executeQueue.begin(); iter != executeQueue.end(); iter++)
                {
                    Execute *executeEntity = &*iter;
                    if (executeEntity->tag == instructionEntity->tag)
                    {
                        if (executeEntity->destName != -1 && executeEntity->destName == registerFile[executeEntity->destRegister].getName())
                        {
                            registerFile[executeEntity->destRegister].setReady(true);
                        }
                        issueQueue.update(executeEntity->destName);
                        executeQueue.erase(iter);
                        break;
                    }
                }
            }
        }
    }
}

bool compare(Issue instruction1, Issue instruction2)
{
    return instruction1.tag < instruction2.tag;
}

void issue()
{
    if (issueQueue.size() > 0)
    {
        // From the issue_list, construct a temp list of instructions whose
        // operands are ready – these are the READY instructions. Scan the READY
        // instructions in ascending order of tags and issue up to N of them.
        vector<Issue> tempIssueQueue = issueQueue.issueInstruction();

        // Sort instructions
        sort(tempIssueQueue.begin(), tempIssueQueue.end(), compare);

        list<Instruction>::iterator iterROB;
        vector<Issue>::iterator iter;
        unsigned instructionIssued = 0;
        // PUSH data and change state from IS to EX
        for (iter = tempIssueQueue.begin(); iter != tempIssueQueue.end() && instructionIssued++ < N; iter++)
        {
            Issue *issueEntity = &*iter;
            executeQueue.push_back((Execute)*issueEntity);
            for (iterROB = fakeROB.begin(); iterROB != fakeROB.end(); iterROB++)
            {
                Instruction *instructionEntity = &*iterROB;
                if (instructionEntity->tag == issueEntity->tag)
                {
                    unsigned latency = 0;
                    instructionEntity->instructionState = EX;
                    if (L1_SIZE > 0 && instructionEntity->operation == 2)
                    {
                        latency = L1.readFromAddress(instructionEntity->memAddress);
                    }
                    else if (instructionEntity->operation == 2)
                    {
                        latency = 5;
                    }
                    instructionEntity->updateLatency(latency);
                    instructionEntity->EX.beginCycle = numberOfCycles;
                    instructionEntity->IS.endCycle = numberOfCycles;
                    break;
                }
            }
        }

        // POP data
        for (iterROB = fakeROB.begin(); iterROB != fakeROB.end(); iterROB++)
        {
            Instruction *instructionEntity = &*iterROB;
            if (instructionEntity->instructionState == EX)
            {
                for (iter = tempIssueQueue.begin(); iter != tempIssueQueue.end(); iter++)
                {
                    Issue *issueEntity = &*iter;
                    if (issueEntity->tag == instructionEntity->tag)
                    {
                        issueQueue.pop(issueEntity->tag);
                        break;
                    }
                }
            }
        }
    }
}

void dispatch()
{
    if (dispatchQueue.size() > 0)
    {
        list<Dispatch> tempDispatchQueue;
        list<Dispatch>::iterator iter;
        list<Instruction>::iterator iterROB;
        /*
        while(!issueQueue.isFull())
        {
            Dispatch *dispatchEntity = &*dispatchQueue.front();
            if(dispatchEntity.instructionState == ID)
            {
                dispatchQueue.pop_front();
                issueQueue.push((Issue)*dispatchEntity);
            }
        }
        */
        // Put instrctions into temp queue for which state is ID
        for (iterROB = fakeROB.begin(); iterROB != fakeROB.end(); iterROB++)
        {
            Instruction *instructionEntity = &*iterROB;
            if (instructionEntity->instructionState == ID)
            {
                for (iter = dispatchQueue.begin(); iter != dispatchQueue.end(); iter++)
                {
                    Dispatch *dispatchEntity = &*iter;
                    if (dispatchEntity->tag == instructionEntity->tag)
                    {
                        tempDispatchQueue.push_back(*dispatchEntity);
                        break;
                    }
                }
            }
        }

        // PUSH data into issueQueue
        for (iter = tempDispatchQueue.begin(); iter != tempDispatchQueue.end() && !issueQueue.isFull(); iter++)
        {
            Dispatch *dispatchEntity = &*iter;
            issueQueue.push((Issue)*dispatchEntity);
            for (iterROB = fakeROB.begin(); iterROB != fakeROB.end(); iterROB++)
            {
                Instruction *instructionEntity = &*iterROB;
                if (instructionEntity->tag == dispatchEntity->tag)
                {
                    instructionEntity->instructionState = IS;
                    instructionEntity->ID.endCycle = numberOfCycles;
                    instructionEntity->IS.beginCycle = numberOfCycles;
                    break;
                }
            }
        }

        // POP data from dispatchQueue
        for (iterROB = fakeROB.begin(); iterROB != fakeROB.end(); iterROB++)
        {
            Instruction *instructionEntity = &*iterROB;
            if (instructionEntity->instructionState == IS)
            {
                for (iter = dispatchQueue.begin(); iter != dispatchQueue.end(); iter++)
                {
                    Dispatch *dispatchEntity = &*iter;
                    if (dispatchEntity->tag == instructionEntity->tag)
                    {
                        dispatchQueue.pop_front();
                        break;
                    }
                }
            }
        }

        // Put instructions in IF stage to ID stage
        for (iterROB = fakeROB.begin(); iterROB != fakeROB.end(); iterROB++)
        {
            Instruction *instructionEntity = &*iterROB;
            if (instructionEntity->instructionState == IF)
            {
                for (iter = dispatchQueue.begin(); iter != dispatchQueue.end(); iter++)
                {
                    Dispatch *dispatchEntity = &*iter;
                    if (dispatchEntity->tag == instructionEntity->tag)
                    {
                        dispatchEntity->instructionState = ID;
                        instructionEntity->instructionState = ID;
                        instructionEntity->IF.endCycle = numberOfCycles;
                        instructionEntity->ID.beginCycle = numberOfCycles;
                        break;
                    }
                }
            }
        }
    }
}

void fetch()
{
    unsigned PC;

    string line;
    unsigned parse = 0;
    unsigned element = 0;

    //Dispatch fetchedInstruction;
    Instruction currentInstruction;
    unsigned fetchCount = 0;

    // Read new instructions from the trace as long as 1) you have not
    // reached the end-of-file, 2) the fetch bandwidth is not exceeded,
    // and 3) the dispatch queue is not full.
    while (fetchCount < N && dispatchQueue.size() < 2 * N)
    {
        if (getline(trace, line))
        {
            string lineBuffer[6] = {"", "", "", "", "", ""};
            fetchCount++;

            while (parse != line.length())
            {
                if (line[parse] != ' ')
                {
                    lineBuffer[element] += line[parse];
                }
                else
                {
                    lineBuffer[element] += '\0';
                    element++;
                }
                parse++;
            }
            element = 0;
            parse = 0;

            // Offload this to a method

            PC = strtoul(lineBuffer[0].c_str(), 0, 16);
            currentInstruction.operation = strtoul(lineBuffer[1].c_str(), 0, 10);
            currentInstruction.destRegister = strtoul(lineBuffer[2].c_str(), 0, 10);
            currentInstruction.src1Register = strtoul(lineBuffer[3].c_str(), 0, 10);
            currentInstruction.src2Register = strtoul(lineBuffer[4].c_str(), 0, 10);
            currentInstruction.memAddress = strtoul(lineBuffer[5].c_str(), 0, 16);
            currentInstruction.tag = instructionCount++;
            currentInstruction.instructionState = IF;
            currentInstruction.IF.beginCycle = numberOfCycles;
            // 1) Push the new instruction onto the fake-ROB. Initialize the
            // instruction’s data structure, including setting its state to IF.
            fakeROB.push_back(currentInstruction);

            // 2) Add the instruction to the dispatch_list and reserve a
            // dispatch queue entry (e.g., increment a count of the number
            // of instructions in the dispatch queue).
            // dispatchQueue.push_back(fetchedInstruction);
            dispatchQueue.push_back((Dispatch)currentInstruction);
        }
        else
        {
            endOfFile = true;
            trace.close();
            break;
        }
    }
}

bool advanceCycle()
{
    if (fakeROB.empty() && endOfFile)
    {
        return false;
    }
    else
    {
        numberOfCycles++;
        return true;
    }
}

void getArguments(char *argv[])
{
    S = strtoul(argv[1], 0, 10);
    N = strtoul(argv[2], 0, 10);
    BLOCKSIZE = strtoul(argv[3], 0, 10);
    L1_SIZE = strtoul(argv[4], 0, 10);
    L1_ASSOC = strtoul(argv[5], 0, 10);
    L2_SIZE = strtoul(argv[6], 0, 10);
    L2_ASSOC = strtoul(argv[7], 0, 10);
    tracefile = argv[8];
}