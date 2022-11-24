/*
 * SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <elf.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>


namespace linglong::elf {

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ELFDATANATIVE ELFDATA2LSB
#elif __BYTE_ORDER == __BIG_ENDIAN
#define ELFDATANATIVE ELFDATA2MSB
#else
#error "Unknown machine endian"
#endif

#define bswap_16(value) ((((value)&0xff) << 8) | ((value) >> 8))

#define bswap_32(value) \
    (((uint32_t)bswap_16((uint16_t)((value)&0xffff)) << 16) | (uint32_t)bswap_16((uint16_t)((value) >> 16)))

#define bswap_64(value) \
    (((uint64_t)bswap_32((uint32_t)((value)&0xffffffff)) << 32) | (uint64_t)bswap_32((uint32_t)((value) >> 32)))

template<typename P>
inline uint16_t file16_to_cpu(uint16_t val, const P &ehdr)
{
    if (ehdr.e_ident[EI_DATA] != ELFDATANATIVE)
        val = bswap_16(val);
    return val;
}

template<typename P>
uint32_t file32_to_cpu(uint32_t val, const P &ehdr)
{
    if (ehdr.e_ident[EI_DATA] != ELFDATANATIVE)
        val = bswap_32(val);
    return val;
}

template<typename P>
uint64_t file64_to_cpu(uint64_t val, const P &ehdr)
{
    if (ehdr.e_ident[EI_DATA] != ELFDATANATIVE)
        val = bswap_64(val);
    return val;
}

auto read_elf64(FILE *fd, Elf64_Ehdr &ehdr) -> decltype(ehdr.e_shoff + (ehdr.e_shentsize * ehdr.e_shnum))
{
    off_t ret = -1;

    fseeko(fd, 0, SEEK_SET);
    // read ELF header
    if (fread(&ehdr, 1, sizeof ehdr, fd) != sizeof(ehdr)) {
        return -1;
    }

    ehdr.e_shoff = file64_to_cpu(ehdr.e_shoff, ehdr);
    ehdr.e_shstrndx = file16_to_cpu(ehdr.e_shstrndx, ehdr);
    ehdr.e_shnum = file16_to_cpu(ehdr.e_shnum, ehdr);

    Elf64_Shdr shdr;

    // read section name string table
    // first, read its header
    fseek(fd, ehdr.e_shoff + ehdr.e_shstrndx * sizeof shdr, SEEK_SET);
    if (fread(&shdr, 1, sizeof shdr, fd) != sizeof(shdr)) {
        return -1;
    }

    // next, read the section, string data
    char *sh = (char *)malloc(shdr.sh_size);

    do {
        fseek(fd, shdr.sh_offset, SEEK_SET);
        if (fread(sh, 1, shdr.sh_size, fd) != shdr.sh_size) {
            break;
        }

        // read all section headers
        for (int idx = 0; idx < ehdr.e_shnum; idx++) {
            fseek(fd, ehdr.e_shoff + idx * sizeof(shdr), SEEK_SET);
            if (fread(&shdr, 1, sizeof(shdr), fd) != sizeof(shdr)) {
                break;
            }

            shdr.sh_name = file32_to_cpu(shdr.sh_name, ehdr);
            shdr.sh_offset = file32_to_cpu(shdr.sh_offset, ehdr);

            if (strcmp(sh + shdr.sh_name, ".bundle") == 0) {
                ret = shdr.sh_offset;
                break;
            }
        }
    } while (false);
    free(sh);
    return ret;
}

off_t get_bundle_offset(const std::string &path)
{
    FILE *fd;
    off_t size = -1;
    Elf64_Ehdr ehdr;

    fd = fopen(path.c_str(), "rb");
    if (fd == nullptr) {
        return -1;
    }

    do {
        auto ret = fread(ehdr.e_ident, 1, EI_NIDENT, fd);
        if (ret != EI_NIDENT) {
            break;
        }
        if ((ehdr.e_ident[EI_DATA] != ELFDATA2LSB) && (ehdr.e_ident[EI_DATA] != ELFDATA2MSB)) {
            break;
        }
        if (ehdr.e_ident[EI_CLASS] == ELFCLASS32) {
            // size = read_elf32(fd);
        } else if (ehdr.e_ident[EI_CLASS] == ELFCLASS64) {
            size = read_elf64(fd, ehdr);
        } else {
            break;
        }
    } while (false);

    fclose(fd);
    return size;
}

} // namespace linglong::elf
