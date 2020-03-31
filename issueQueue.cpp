#include <iostream>
#include <list>
#include <vector>
#include "issueQueue.h"
#include "sharedVariables.h"

using namespace std;

IssueQueue::IssueQueue()
{
}

IssueQueue::IssueQueue(unsigned size)
{
    S = size;
    queue = new Issue[S];
}

bool IssueQueue::isFull()
{
    bool full = true;
    for (unsigned iter = 0; iter < S; iter++)
    {
        if (queue[iter].isEmpty)
        {
            full = false;
            break;
        }
    }
    return full;
}

unsigned IssueQueue::size()
{
    unsigned size = 0;
    for (unsigned iter = 0; iter < S; iter++)
    {
        if (!queue[iter].isEmpty)
        {
            size++;
        }
    }
    return size;
}

void IssueQueue::pop(unsigned tag)
{
    for (unsigned iter = 0; iter < S; iter++)
    {
        if (queue[iter].tag == tag)
        {
            queue[iter].isEmpty = true;
            queue[iter].src1Ready = false;
            queue[iter].src2Ready = false;
            break;
        }
    }
}

void IssueQueue::push(Issue instruction)
{
    unsigned iter;
    for (iter = 0; iter < S; iter++)
    {
        if (queue[iter].isEmpty)
        {
            queue[iter] = instruction;
            rename(iter);
            break;
        }
    }
}

void IssueQueue::rename(unsigned current)
{
    if (queue[current].src1Name != -1)
    {
        queue[current].src1Ready = registerFile[queue[current].src1Name].isReady();
        queue[current].src1Name = registerFile[queue[current].src1Name].getName();
    }
    else
    {
        queue[current].src1Ready = true;
    }

    if (queue[current].src2Name != -1)
    {
        queue[current].src2Ready = registerFile[queue[current].src2Name].isReady();
        queue[current].src2Name = registerFile[queue[current].src2Name].getName();
    }
    else
    {
        queue[current].src2Ready = true;
    }

    if (queue[current].destRegister != -1)
    {
        queue[current].destName = queue[current].tag;
        registerFile[queue[current].destRegister].setName(queue[current].destName);
        registerFile[queue[current].destRegister].setReady(false);
    }
}

vector<Issue> IssueQueue::issueInstruction()
{
    vector<Issue> tempQueue;

    for (unsigned iter = 0; iter < S; iter++)
    {
        if (queue[iter].src1Ready && queue[iter].src2Ready && !queue[iter].isEmpty)
        {
            tempQueue.push_back(queue[iter]);
        }
    }
    return tempQueue;
}

void IssueQueue::update(unsigned reg)
{
    for (unsigned iter = 0; iter < S; iter++)
    {
        if (queue[iter].src1Name == reg)
        {
            queue[iter].src1Ready = true;
        }
        if (queue[iter].src2Name == reg)
        {
            queue[iter].src2Ready = true;
        }
    }
}