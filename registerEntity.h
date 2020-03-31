#ifndef REGISTER
#define REGISTER

struct RegisterEntity
{
    private:
        int name;
        bool ready;
    public:
        RegisterEntity();
        int getName();
        void setName(unsigned);
        bool isReady();
        void setReady(bool);        
};

#endif