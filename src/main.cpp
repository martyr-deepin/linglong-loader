/*
 * SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <pthread.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

namespace linglong {

namespace elf {

off_t get_bundle_offset(const std::string &path);

}

namespace erofs {

int MountFile(const std::string &path, size_t offset, const std::string &mount_point);

} // namespace erofs

} // namespace linglong

std::string realpath(const std::string &path)
{
    char *buffer = new char[PATH_MAX];
    auto ret = ::realpath(path.c_str(), buffer);
    if (ret != buffer) {
        std::cerr << "realpath failed:" << errno;
    }
    std::string res = ret;
    delete[] buffer;
    return res;
}

bool exists(const std::string &path)
{
    return access(path.c_str(), F_OK) == 0;
}

int main(int argc, char **argv)
{
    prctl(PR_SET_PDEATHSIG, SIGKILL);

    auto exec_path = realpath("/proc/self/exe");

    if (!exists(exec_path)) {
        std::cerr << "load failed! " << exec_path << std::endl;
        return EXIT_FAILURE;
    }

    // get elf size
    auto bundle_offset = linglong::elf::get_bundle_offset(exec_path);

    if (bundle_offset == -1) {
        std::cerr << "failed to find bundle" << std::endl;
        return EXIT_FAILURE;
    }

    // FIXME: user runtime dir
    char temp_prefix[1024] = "/tmp/uab-XXXXXX";
    char *mount_point = mkdtemp(temp_prefix);

    linglong::erofs::MountFile(exec_path, bundle_offset, mount_point);

    std::string loader_path = mount_point;
    loader_path += "/loader";
    if (!exists(loader_path)) {
        std::cerr << "can not found" << loader_path << std::endl;
        return EXIT_FAILURE;
    }

    auto pid = fork();
    if (pid < 0) {
        std::cerr << "fork exec failed:" << errno << std::endl;
        return EXIT_FAILURE;
    }

    // run loader for child
    if (pid == 0) {
        if (chdir(mount_point) != 0) {
            std::cerr << "change to work failed !" << errno << std::endl;
            return EXIT_FAILURE;
        }

        int cnt = 1;
        // FIXME, user ARG_MAX
        char *load_argv[16] = {
            (char *)loader_path.data(),
            mount_point,
        };
        if (argc > 1 && argc < 16) {
            for (auto idx = 1; idx < argc; idx++) {
                cnt++;
                load_argv[cnt] = argv[idx];
            }
        }
        cnt++;
        load_argv[cnt] = nullptr;
        extern char **environ;
        // run loader shell
        auto ret = execve(loader_path.data(), load_argv, environ);
        if (ret != 0) {
            std::cerr << "run failed!" << std::endl;
        }
        exit(0);
    }

    while (true) {
        int childPid = waitpid(-1, nullptr, 0);
        if (childPid > 0) {
            // std::cout << "wait " << childPid << " exit" << endl;
        }
        if (childPid < 0) {
            std::cout << "all child exit" << std::endl;
            return EXIT_SUCCESS;
        }
    }
}
