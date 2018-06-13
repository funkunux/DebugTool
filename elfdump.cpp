#include <iostream>
#include <elf.h>
#include "SimpleExcept.h"
#include "FileReader.h"
#include "ElfFile.h"
#include "config.h"

using namespace std;

void showUsage(const string& name)
{
    cout << "Usage: " << name << " <elf_Path>" << endl;
}

int main(int argc, char** argv)
{
    char buff[256] = {0};
    Elf32_Ehdr elfHeader = {0};
    if(2 != argc)
    {
        showUsage(argv[0]);
    }

    const char* elf_Path = argv[1];
    
    ElfFile(elf_Path).info();
}