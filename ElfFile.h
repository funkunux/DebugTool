#ifndef __ELF_DUMP__
#define __ELF_DUMP__

#include "FileReader.h"
#include "SimpleExcept.h"
#include <elf.h>
#include <vector>
#include <map>
#include "config.h"

class NameTable
{
public:
    char *buff;
    int size;
    NameTable(unsigned int size)
    {
        this->size = size;
        buff = new char[size];
    }
    ~NameTable()
    {
        delete [] buff;
    }

    char* getName(unsigned int index)
    {
        return buff + index;
    }
};

class ElfFile
{
private:
    FileReader *_elfFile;
    Elf32_Ehdr _elfHeader;
    NameTable *secNameTbl;
    NameTable *symNameTbl;
    std::vector<Elf32_Shdr*> vpSectionHdr;
    std::vector<Elf32_Phdr*> vpPrgmmHdr;
    std::vector<Elf32_Sym *> vpFuncSym;

public:
    ElfFile(const char* filePath)
    {
        _elfHeader = {0};
        _elfFile = new FileReader(filePath);
        secNameTbl = NULL;
        symNameTbl = NULL;

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
            Elf32_Shdr *sectionHeader = new Elf32_Shdr();
            if(!getStruct<Elf32_Shdr>(*sectionHeader, _elfHeader.e_shoff + _elfHeader.e_shentsize * index))
            {
                debug("Bad section header by index:%d\n", index);
                throw SimpleExcept("Bad section header!");
            }
            vpSectionHdr.push_back(sectionHeader);
        }

        secNameTbl = new NameTable(vpSectionHdr[_elfHeader.e_shstrndx]->sh_size);
        int sectionNameBuffOff = vpSectionHdr[_elfHeader.e_shstrndx]->sh_offset;
        
        if(!getBuff(secNameTbl->buff, secNameTbl->size, sectionNameBuffOff))
        {
            debug("Get sectionNameBuff failed!\n");
            throw SimpleExcept("Bad sectionNameBuff!");
        }
    }

    bool getBuff(char* buff, int size, int offset)
    {
        int readSize = _elfFile->read(buff, size, offset);
        if(size != readSize)
        {
            debug("Can't read %d bytes but %d\n", size, readSize);
            return false;
        } 
        return true;
    }


    ~ElfFile()
    {
        debug("~ElfFile\n");
        delete _elfFile;
        for(auto sectionHdr : vpSectionHdr)
        {
            delete sectionHdr;
        }
        vpSectionHdr.clear();

        for(auto prgmmHdr : vpPrgmmHdr)
        {
            delete prgmmHdr;
        }
        vpPrgmmHdr.clear();
        if(NULL != secNameTbl) delete secNameTbl;
        if(NULL != symNameTbl) delete symNameTbl;
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

    void showSectionInfo(Elf32_Shdr *section)
    {
        int sizeOfSection = 0;
        int sizeOfEntry = 0;
        
        sizeOfSection = section->sh_size;
        sizeOfEntry = section->sh_entsize;
        if(0 == sizeOfSection)
        {
            return;
        }
        printf("\n******** %s ********\n", secNameTbl->getName(section->sh_name));
        printf("FLAGS:    ");
        if(SHF_WRITE & section->sh_flags)
            printf(" WRITE");
        if(SHF_ALLOC & section->sh_flags)
            printf(" ALLOC");
        if(SHF_EXECINSTR & section->sh_flags)
            printf(" EXECINSTR");
        if(SHF_MASKPROC & section->sh_flags)
            printf(" MASKPROC");
        printf("\n");
        SHOWKEY("Addr:", section->sh_addr);
        SHOWKEY("Size:", sizeOfSection);
        SHOWKEY("EntrySize:", sizeOfEntry);
        if(0 != sizeOfEntry) SHOWKEY("SymCount:", sizeOfSection / sizeOfEntry);
        SHOWKEY("Offset:", section->sh_offset);
        SHOWKEY("Link:", section->sh_link);
        SHOWKEY("Info:", section->sh_info);
        SHOWKEY("Addralign:", section->sh_addralign);
    }

    void getFuncSymTbl()
    {
        for(auto section:vpSectionHdr)
        {
            if(SHT_SYMTAB == section->sh_type)
            {
                //TODO
            }
        }
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

        printf("\n******** Section Info ********\n");
        int index = 0;
        for(auto section : vpSectionHdr)
        {
            showSectionInfo(section);
            index++;
        }

        getFuncSymTbl();
    }
};

#endif