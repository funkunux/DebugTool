#include <iostream>
#include <fstream>
#include <elf.h>
#include <iomanip>

using namespace std;

#define SHOWKEY(key, value) cout << setw(20) << setiosflags(ios::left) << key << value << endl

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
    ifstream inFile(elf_Path);
    if(!inFile.is_open())
    {
        cout << "Can't open " << elf_Path << endl;
        perror("Open Err");
        return 1;
    }
    
    inFile.read((char *)&elfHeader, sizeof(elfHeader));
    if(0x7f == elfHeader.e_ident[0] && 'E' == elfHeader.e_ident[1] && 'L' == elfHeader.e_ident[2] && 'F' == elfHeader.e_ident[3])
    {
        SHOWKEY("Path:", elf_Path);
        switch(elfHeader.e_ident[EI_CLASS])
        {
            case ELFCLASS32:
                SHOWKEY("Elf Class:", "32bits");
                break;
            case ELFCLASS64:
                SHOWKEY("Elf Class:", "64bits");
                cout << "Unsupport 64bits for now..." << endl; /* TODO:增加64位支持 */
                return 1;
            default:
                cout << "Bad Elf File!" << endl;
                return 1;
        }
        switch(elfHeader.e_ident[EI_DATA])
        {
            case ELFDATA2LSB:
                SHOWKEY("Elf Endian:", "LSB");
                break;
            case ELFDATA2MSB:
                SHOWKEY("Elf Endian:", "MSB");
                break;
            default:
                cout << "Bad Elf File!" << endl;
                return 1;
        }
        switch(elfHeader.e_ident[EI_VERSION])
        {
            case EV_NONE:
                cout << "Bad Elf File!" << endl;
                return 1;
            case EV_CURRENT:
                SHOWKEY("ELF Version:", "Current");
                break;
            default:
                SHOWKEY("ELF Version:", elfHeader.e_ident[EI_VERSION]);
                break;
        }
        switch(elfHeader.e_ident[EI_OSABI])
        {
            case ELFOSABI_SYSV:
                SHOWKEY("ELF OS ABI:", "UNIX System V ABI");
                break;
            case ELFOSABI_LINUX:
                SHOWKEY("ELF OS ABI:", "Linux ABI");
                break;
            default:
                SHOWKEY("ELF OS ABI:", elfHeader.e_ident[EI_OSABI]);
                break;
        }
        switch(elfHeader.e_machine)
        {
            case EM_386:
                SHOWKEY("Machine Type:", "Intel 80386");
                break;
            default:
                SHOWKEY("Machine Type:", elfHeader.e_machine);
                break;
        }
        switch(elfHeader.e_version)
        {
            case EV_NONE:
                cout << "Bad Elf File!" << endl;
                return 1;
            case EV_CURRENT:
                SHOWKEY("File Version:", "Current");
                break;
            default:
                SHOWKEY("File Version:", elfHeader.e_ident[EI_VERSION]);
                break;
        }

        SHOWKEY("Elf Header Size:", elfHeader.e_ehsize);
        SHOWKEY("Entry:", "0x" << hex << elfHeader.e_entry << dec);
        cout << endl << "******** Program Header Table ********" << endl;
        SHOWKEY("Offset:", elfHeader.e_phoff);
        SHOWKEY("Each Entry Size:", elfHeader.e_phentsize);
        SHOWKEY("Entry Num:", elfHeader.e_phnum);
        cout << endl;

        cout << endl << "******** Section Header Table ********" << endl;
        SHOWKEY("Offset:", elfHeader.e_shoff);
        SHOWKEY("Each Entry Size:", elfHeader.e_shentsize);
        SHOWKEY("Entry Num:", elfHeader.e_shnum);
        SHOWKEY("Index:", elfHeader.e_shstrndx);
        cout << endl;
    }
    else
    {
        cout << "Not an elf file!" << endl;
    }

    inFile.close();
}