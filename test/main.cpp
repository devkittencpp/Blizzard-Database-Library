
#include<DBCD.h>
#include<iostream>

int main()
{
    auto Cls = LibTestClass();

    auto string = Cls.GetPrintString();

    std::cout << string << std::endl;

    return 0;
}