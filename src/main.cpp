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

using namespace std;
using namespace linglong;

std::string realpath(const std::string &path)
{
    char *p = new char[PATH_MAX];
    auto ret = ::realpath(path.c_str(), p);
    if (ret != p) {
        cerr << "realpath failed:" << errno;
    }
    std::string res = ret;
    delete[] p;
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
        cerr << "load failed! " << exec_path << endl;
        return EXIT_FAILURE;
    }

    // get elf size
    auto bundle_offset = elf::get_bundle_offset(exec_path);

    if (bundle_offset == -1) {
        cerr << "failed to find bundle" << endl;
        return EXIT_FAILURE;
    }

    // FIXME: user runtime dir
    char temp_prefix[1024] = "/tmp/uab-XXXXXX";
    char *mount_point = mkdtemp(temp_prefix);

    erofs::MountFile(exec_path, bundle_offset, mount_point);

    std::string loader_path = mount_point;
    loader_path += "/loader";
    if (!exists(loader_path)) {
        cerr << "can not found" << loader_path << endl;
        return EXIT_FAILURE;
    }

    auto pid = fork();
    if (pid < 0) {
        cerr << "fork exec failed:" << errno << endl;
        return EXIT_FAILURE;
    }

    // run loader for child
    if (pid == 0) {
        if (chdir(mount_point) != 0) {
            cerr << "change to work failed !" << errno << endl;
            return EXIT_FAILURE;
        }

        int c = 1;
        // FIXME, user ARG_MAX
        char *load_argv[16] = {
            (char *)loader_path.data(),
            mount_point,
        };
        if (argc > 1 && argc < 16) {
            for (auto idx = 1; idx < argc; idx++) {
                c++;
                load_argv[c] = argv[idx];
            }
        }
        c++;
        load_argv[c] = nullptr;
        extern char **environ;
        // run loader shell
        auto ret = execve(loader_path.data(), load_argv, environ);
        if (ret != 0) {
            cerr << "run failed!" << endl;
        }
        exit(0);
    }

    while (true) {
        int childPid = waitpid(-1, nullptr, 0);
        if (childPid > 0) {
            // std::cout << "wait " << childPid << " exit" << endl;
        }
        if (childPid < 0) {
            std::cout << "all child exit" << endl;
            return EXIT_SUCCESS;
        }
    }
}
