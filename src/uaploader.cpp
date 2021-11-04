/*
 * Copyright (c) 2019-2021. Uniontech Software Ltd. All rights reserved.
 *
 * Author:     Heysion <heysoin@deepin.com>
 *
 * Maintainer: Heysion <heysion@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <elf.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
//#include <fuse.h>
//#include "squashfuse/ll.h"

using namespace std;

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ELFDATANATIVE ELFDATA2LSB
#elif __BYTE_ORDER == __BIG_ENDIAN
#define ELFDATANATIVE ELFDATA2MSB
#else
#error "Unknown machine endian"
#endif

#define bswap_16(value) \
((((value) & 0xff) << 8) | ((value) >> 8))

#define bswap_32(value) \
(((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
(uint32_t)bswap_16((uint16_t)((value) >> 16)))

#define bswap_64(value) \
(((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) \
<< 32) | \
(uint64_t)bswap_32((uint32_t)((value) >> 32)))

template<typename P>
inline uint16_t file16_to_cpu(uint16_t val, const P &ehdr) {
    if (ehdr.e_ident[EI_DATA] != ELFDATANATIVE)
        val = bswap_16(val);
    return val;
}

template<typename P>
uint32_t file32_to_cpu(uint32_t val, const P &ehdr) {
    if (ehdr.e_ident[EI_DATA] != ELFDATANATIVE)
        val = bswap_32(val);
    return val;
}

template<typename P>
uint64_t file64_to_cpu(uint64_t val, const P &ehdr) {
    if (ehdr.e_ident[EI_DATA] != ELFDATANATIVE)
        val = bswap_64(val);
    return val;
}

auto read_elf64(FILE *fd, Elf64_Ehdr &ehdr) -> decltype(ehdr.e_shoff + (ehdr.e_shentsize * ehdr.e_shnum)) {
    Elf64_Ehdr ehdr64;
    off_t ret = -1;

    fseeko(fd, 0, SEEK_SET);
    ret = fread(&ehdr64, 1, sizeof(ehdr64), fd);
    if (ret < 0 || (size_t) ret != sizeof(ehdr64)) {
        return -1;
    }

    ehdr.e_shoff = file64_to_cpu<Elf64_Ehdr>(ehdr64.e_shoff, ehdr);
    ehdr.e_shentsize = file16_to_cpu<Elf64_Ehdr>(ehdr64.e_shentsize, ehdr);
    ehdr.e_shnum = file16_to_cpu<Elf64_Ehdr>(ehdr64.e_shnum, ehdr);

    return (ehdr.e_shoff + (ehdr.e_shentsize * ehdr.e_shnum));

}

auto get_elf_size(const std::string path) -> decltype(-1) {
    FILE *fd = nullptr;
    off_t size = -1;
    Elf64_Ehdr ehdr;

    fd = fopen(path.c_str(), "rb");
    if (fd == nullptr) {
        return -1;
    }
    auto ret = fread(ehdr.e_ident, 1, EI_NIDENT, fd);
    if (ret != EI_NIDENT) {
        return -1;
    }
    if ((ehdr.e_ident[EI_DATA] != ELFDATA2LSB) &&
        (ehdr.e_ident[EI_DATA] != ELFDATA2MSB)) {
        return -1;
    }
    if (ehdr.e_ident[EI_CLASS] == ELFCLASS32) {
        //size = read_elf32(fd);
    } else if (ehdr.e_ident[EI_CLASS] == ELFCLASS64) {
        size = read_elf64(fd, ehdr);
    } else {
        return -1;
    }
    fclose(fd);
    return size;
}

std::string run_exec(const std::string &cmd) {
    std::array<char, 1024> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        return nullptr;
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
//    auto user_home = std::getenv("HOME");
//    auto user_name = std::getenv("USER");
    auto exec_path = std::filesystem::canonical("/proc/self/exe");

    if (!filesystem::exists(exec_path)) {
        cout << "load failed! " << exec_path << endl;
        exit(-1);
    }
    auto elf_fd = fopen(exec_path.c_str(), "rb");
    if (!elf_fd) {
        cout << "load failed! " << exec_path << endl;
        exit(-1);
    }

    // get elf size
    auto elf_size = get_elf_size(exec_path);
    struct stat elf_stat;
    if (stat(exec_path.c_str(), &elf_stat) != 0) {
        exit(-1);
    }
    if (elf_size >= elf_stat.st_size) {
        cout << "loader can not run for single!" << endl;
        exit(-1);
    }

    char temp_prefix[1024] = "/tmp/uap-XXXXXX";
    char *mount_point = mkdtemp(temp_prefix);

    // mount data.sqsfs
    std::string cmd = "squashfuse -o ro,offset=";
    cmd += std::to_string(elf_size);
    cmd += " ";
    cmd += exec_path;
    cmd += " ";
    cmd += mount_point;

    run_exec(cmd);
    std::string loader_path = mount_point;
    loader_path += "/.loader";
    if (!std::filesystem::exists(loader_path)) {
        cout << "can not run!" << endl;
        exit(-1);
    }

    auto pid = fork();

    // run loader for child
    if (pid == 0) {
        if (chdir(mount_point) != 0) {
            cout << "change to work failed !" << endl;
            exit(-1);
        }

        char *load_argv[] = {
                loader_path.data(),
                mount_point,
                nullptr,
        };
        extern char **environ;
        // run loader shell
        auto ret = execve(loader_path.data(), load_argv, environ);
        if (ret != 0) {
            perror("run");
            cout << "run failed!" << endl;
        }
        exit(0);
    }
    if (pid < 0) {
        cout << "fork exec failed!" << endl;
        exit(-1);
    }
    waitpid(pid, 0, 0);
    // cleanup
    std::string umount_cmd = "fusermount -u ";
    umount_cmd += mount_point;
    run_exec(umount_cmd);
    rmdir(mount_point);
    return 0;
}