#ifndef CUSTOM_DATA_TYPES
#define CUSTOM_DATA_TYPES

typedef enum
{
    IF,
    ID,
    IS,
    EX,
    WB
} state;

struct Execute
{
    unsigned tag;
    unsigned operation;
    int destRegister;
    int destName; 
    unsigned memAddress;
};

struct Issue
{
    bool isEmpty = true;
    unsigned tag;
    unsigned operation;
    int destRegister;
    int destName;
    int src1Name;
    bool src1Ready;
    int src2Name;
    bool src2Ready;
    unsigned memAddress;

    operator Execute();
};

struct Dispatch
{
    state instructionState;
    unsigned tag;
    unsigned operation;
    int destRegister;
    int src1Register;
    int src2Register;
    unsigned memAddress;

    operator Issue();
};


struct TimingInfo
{
    unsigned beginCycle;
    unsigned endCycle;
};

struct Instruction
{
    public:
        TimingInfo IF;
        TimingInfo ID;
        TimingInfo IS;
        TimingInfo EX;
        TimingInfo WB;

        unsigned tag;
        state instructionState;
        unsigned operation;
        int destRegister;
        int src1Register;
        int src2Register;
        unsigned memAddress;
        int latency = 0;

        void updateLatency(unsigned);
        bool isExecutionComplete(unsigned);
        operator Dispatch();
};

#endif