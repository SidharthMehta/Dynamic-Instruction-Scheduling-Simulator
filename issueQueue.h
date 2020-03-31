#ifndef ISSUE_QUEUE
#define ISSUE_QUEUE

#include <list>
#include "customDataTypes.h"

using namespace std;

class IssueQueue
{
    private:
        unsigned S;
        Issue *queue;
    public:
        IssueQueue();
        IssueQueue(unsigned);
        unsigned size();
        bool isFull();
        void push(Issue);
        void rename(unsigned);
        void pop(unsigned);
        void update(unsigned);
        vector<Issue> issueInstruction();
};

#endif
