/*
 * SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

extern "C" {
extern int erofs_fuse_main(int argc, char *argv[]);
}

namespace linglong::erofs {

int MountFile(const std::string &path, unsigned long offset, const std::string &mount_point)
{
    int fusefs_pid = vfork();

    if (0 == fusefs_pid) {
        // child
        char *dir = realpath(path.c_str(), nullptr);

        char options[128];
        sprintf(options, "--offset=%lu", offset);

        const char *fuse_argv[] = {dir, options, dir, mount_point.c_str(), nullptr};

        if (0 != erofs_fuse_main(4, const_cast<char **>(fuse_argv))) {
            std::cerr << "erofs_fuse_main" << std::endl;
            return -1;
        }
    }

    waitpid(fusefs_pid, nullptr, 0);

    return 0;
}

} // namespace linglong::erofs
