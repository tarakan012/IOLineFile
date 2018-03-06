#include <iostream>
#include <algorithm>
#include "FunctorDataClass.h"
#include "Task.h"

static int gNumberLines = 50;
static std::string gstrNameFile = "text.txt";

void PrintUsage()
{
    std::cout << "Usage: app - num number line -name name file\n";
}

bool ParseCommandLine(int argc, char * argv[])
{
    if(1 == argc)
    {
        std::cout << "Default: app - num 50 -name text.txt\n";
        return true;
    }
    char * s = argv[2];
    if(std::string("-num") == argv[1])
    {
        int n32NumberLines = 0;
        if(!(n32NumberLines =std::strtol(s,0,0))) return false;
        gNumberLines = n32NumberLines;
        if(std::string("-name") == argv[3])
        {
            gstrNameFile = argv[4];
            return true;
        }
    }
    PrintUsage();
    return false;
}

 void ReverseString(std::string & str)
 {
     std::reverse(str.begin(), str.end());
 }

int main(int argc, char * argv[])
{
    try {
        if(!ParseCommandLine(argc, argv))
            return 1;

        CFunctorReadLineFromFile functor_read;
        CFunctorWriteLineFromFile functor_write;
        functor_read.Open(gstrNameFile);
        functor_write.Open(gstrNameFile);

        auto par_write = std::make_shared<CParallel>();
        auto par_read = std::make_shared<CParallel>();
        auto task_read = MakeTask(std::move(functor_read));
        CSequential seq_alg;
        int n32Processed{};
        for(int i = 0; i < gNumberLines; ++i) {

            task_read->Schedule(*par_read);
            std:: string str = task_read->GetFuture().get();
            auto task_alg = MakeTask([&str]{ReverseString(str);
                return str;});
            task_alg->Schedule(seq_alg);
            task_alg->Wait();
            functor_write.SetStrLine(str);
            std::cout << "reverse string : " << str << std::endl;
        auto task_write = MakeTask([&functor_write]{return functor_write();});
            task_write->Schedule(*par_write);
            task_write->Wait();
            n32Processed++;
        }
        std::cout << n32Processed << " lines processed successfully\n";
    }
    catch(std::runtime_error & e)
    {

        std::cout << e.what();
    }
    return 0;
}