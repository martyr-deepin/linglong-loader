/*
 * Copyright (c) 2021. Uniontech Software Ltd. All rights reserved.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <unistd.h>
#include <sys/sem.h>

#include <string>
#include <array>
#include <memory>
#include <iostream>

extern "C" {
extern int fusefs_main(int argc, char *argv[], void (*mounted)());
}

namespace linglong::squashfs {

key_t sem_key;

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

struct sembuf sem_lock = {0, -1, SEM_UNDO};

struct sembuf sem_unlock = {0, 1, SEM_UNDO};

int GetSemID(key_t key)
{
    int sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
    union semun sem_union = {0};
    sem_union.val = 0;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) {
        std::cerr << "set sem failed" << std::endl;
        return -1;
    }
    return sem_id;
}

void FuseMounted()
{
    int sem_id = GetSemID(sem_key);
    semop(sem_id, &sem_unlock, 1);

    std::cout << "fuse_mounted" << std::endl;
}

int MountFile(const std::string &path, unsigned long offset, const std::string &mount_point)
{
    sem_key = getpid();
    int sem_id = GetSemID(sem_key);

    int fusefs_pid = fork();

    if (0 == fusefs_pid) {
        // child
        char *dir = realpath(path.c_str(), nullptr);

        char options[128];
        sprintf(options, "ro,offset=%lu", offset);

        const char *fuse_argv[] = {
            dir, "-o", options, dir, const_cast<char *>(mount_point.data()),
        };

        if (0 != fusefs_main(5, const_cast<char **>(fuse_argv), FuseMounted)) {
            std::cerr << "fusefs_main" << std::endl;
            return -1;
        }
    }

    // parent
    // wait mounted
    semop(sem_id, &sem_lock, 1);
    return 0;
}

} // namespace linglong::squashfs