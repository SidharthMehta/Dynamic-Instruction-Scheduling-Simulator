#include "customDataTypes.h"

Issue::operator Execute()
{
    Execute caster;
    caster.tag = tag;
    caster.operation = operation;
    caster.destRegister = destRegister;
    caster.destName = destName;
    caster.memAddress = memAddress;
    return caster;
}

Dispatch::operator Issue()
{
    Issue caster;
    caster.tag = tag;
    caster.operation = operation;
    caster.src1Name = src1Register;
    caster.src2Name = src2Register;
    caster.destRegister = destRegister;
    caster.destName = -1;
    caster.memAddress = memAddress;
    caster.isEmpty = false;
    caster.src1Ready = false;
    caster.src2Ready = false;
    return caster;
}

Instruction::operator Dispatch()
{
    Dispatch caster;
    caster.instructionState = instructionState;
    caster.tag = tag;
    caster.operation = operation;
    caster.src1Register = src1Register;
    caster.src2Register = src2Register;
    caster.destRegister = destRegister;
    caster.memAddress = memAddress;
    return caster;
}

void Instruction::updateLatency(unsigned latency)
{
    this->latency = latency;
}
bool Instruction::isExecutionComplete(unsigned numberOfCycles)
{
    bool complete = false;
    if(this->instructionState == state::EX)
    {
        switch(operation)
        {
            case 0:
                if(numberOfCycles-EX.beginCycle == 1)
                {
                   complete = true; 
                }
                break;
            case 1:
                if(numberOfCycles-EX.beginCycle == 2)
                {
                    complete = true; 
                }
                break;
            case 2:
                if(numberOfCycles-EX.beginCycle == latency)
                {
                    complete = true; 
                }
                break;
        }
    }
    return complete; 
}

