#ifndef __ELF_DUMP__
#define __ELF_DUMP__

#include "FileReader.h"
#include "SimpleExcept.h"
#include <elf.h>
#include <vector>
#include "config.h"

class ElfFile
{
private:
    FileReader *_elfFile;
    Elf32_Ehdr _elfHeader;
    std::vector<Elf32_Shdr*> vpSessionHdr;
    std::vector<Elf32_Phdr*> vpPrgmmHdr;

public:
    ElfFile(const char* filePath)
    {
        _elfHeader = {0};
        _elfFile = new FileReader(filePath);

        if(!getStruct<Elf32_Ehdr>(_elfHeader))
        {
            throw SimpleExcept("Bad elf header!");
        }
        if(isElfFile())
        {
            debug("%s is not an elf file!\n", filePath);
            throw SimpleExcept("Bad elf file!");
        }
        for(int index = 0; index < _elfHeader.e_phnum; index++)
        {
            Elf32_Phdr *prgmmHeader = new Elf32_Phdr();
            if(!getStruct<Elf32_Phdr>(*prgmmHeader, _elfHeader.e_phoff + _elfHeader.e_phentsize * index))
            {
                debug("Bad programm header by index:%d\n", index);
                throw SimpleExcept("Bad programm header!");
            }
            vpPrgmmHdr.push_back(prgmmHeader);
        }

        for(int index = 0; index < _elfHeader.e_shnum; index++)
        {
            Elf32_Shdr *sessionHeader = new Elf32_Shdr();
            if(!getStruct<Elf32_Shdr>(*sessionHeader, _elfHeader.e_shoff + _elfHeader.e_shentsize * index))
            {
                debug("Bad session header by index:%d\n", index);
                throw SimpleExcept("Bad session header!");
            }
            vpSessionHdr.push_back(sessionHeader);
        }
    }

    ~ElfFile()
    {
        debug("~ElfFile\n");
        delete _elfFile;
        for(auto sessionHdr : vpSessionHdr)
        {
            delete sessionHdr;
        }
        vpSessionHdr.clear();

        for(auto prgmmHdr : vpPrgmmHdr)
        {
            delete prgmmHdr;
        }
        vpPrgmmHdr.clear();
    }

    template<typename T>
    bool getStruct(T &header, unsigned int offset = 0)
    {
        int readSize = 0;
        readSize = _elfFile->read((char *)(&header), sizeof(T), offset);
        if(sizeof(T) != readSize)
        {
            debug("Read size error: %d, should be %d\n", readSize, sizeof(T));
            return false;
        }
        return true;
    }

    bool isElfFile()
    {
        return (0x7f != _elfHeader.e_ident[0] || 'E' != _elfHeader.e_ident[1] || 'L' != _elfHeader.e_ident[2] || 'F' != _elfHeader.e_ident[3]);
    }

    char* getSessionName(int index)
    {
        int offset = _elfHeader.e_shoff + _elfHeader.e_shstrndx + _elfHeader.e_shentsize;
        return "";
    }

    void info()
    {
        switch(_elfHeader.e_ident[EI_CLASS])
        {
            case ELFCLASS32:
                SHOWKEY("Elf Class:", "32bits");
                break;
            case ELFCLASS64:
                SHOWKEY("Elf Class:", "64bits");
                throw SimpleExcept("Unsupport 64bits for now...\n"); /* TODO:增加64位支持 */
            default:
                throw SimpleExcept("Bad Elf File!\n");
        }
        switch(_elfHeader.e_ident[EI_DATA])
        {
            case ELFDATA2LSB:
                SHOWKEY("Elf Endian:", "LSB");
                break;
            case ELFDATA2MSB:
                SHOWKEY("Elf Endian:", "MSB");
                break;
            default:
                throw SimpleExcept("Bad Elf File!\n");
        }
        switch(_elfHeader.e_ident[EI_VERSION])
        {
            case EV_NONE:
                throw SimpleExcept("Bad Elf File!\n");
            case EV_CURRENT:
                SHOWKEY("ELF Version:", "Current");
                break;
            default:
                SHOWKEY("ELF Version:", _elfHeader.e_ident[EI_VERSION]);
                break;
        }
        switch(_elfHeader.e_ident[EI_OSABI])
        {
            case ELFOSABI_SYSV:
                SHOWKEY("ELF OS ABI:", "UNIX System V ABI");
                break;
            case ELFOSABI_LINUX:
                SHOWKEY("ELF OS ABI:", "Linux ABI");
                break;
            default:
                SHOWKEY("ELF OS ABI:", _elfHeader.e_ident[EI_OSABI]);
                break;
        }
        switch(_elfHeader.e_machine)
        {
            case EM_386:
                SHOWKEY("Machine Type:", "Intel 80386");
                break;
            default:
                SHOWKEY("Machine Type:", _elfHeader.e_machine);
                break;
        }
        switch(_elfHeader.e_version)
        {
            case EV_NONE:
                throw SimpleExcept("Bad Elf File!\n");
            case EV_CURRENT:
                SHOWKEY("File Version:", "Current");
                break;
            default:
                SHOWKEY("File Version:", _elfHeader.e_ident[EI_VERSION]);
                break;
        }

        SHOWKEY("Elf Header Size:", _elfHeader.e_ehsize);
        SHOWKEY("Entry:", "0x" << std::hex << _elfHeader.e_entry << std::dec);
        printf("\n******** Program Header Table ********\n");
        SHOWKEY("Offset:", _elfHeader.e_phoff);
        SHOWKEY("Each Entry Size:", _elfHeader.e_phentsize);
        SHOWKEY("Entry Num:", _elfHeader.e_phnum);   /* TODO：最大值为PN_XNUM */

        printf("\n******** Section Header Table ********\n");
        SHOWKEY("Offset:", "0x" << std::hex << _elfHeader.e_shoff << std::dec);
        SHOWKEY("Each Entry Size:", _elfHeader.e_shentsize);
        SHOWKEY("Entry Num:", _elfHeader.e_shnum);   /* TODO：最大值为SHN_LORESERVE */
        SHOWKEY("Index of StrName:", _elfHeader.e_shstrndx);
        SHOWKEY("StrName Table Offset:", _elfHeader.e_shoff + _elfHeader.e_shstrndx * _elfHeader.e_shentsize);
    }
};

#endif