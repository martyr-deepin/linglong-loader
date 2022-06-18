/*
 * Copyright (c) 2022. Uniontech Software Ltd. All rights reserved.
 *
 * Author:     liujianqiang <liujianqiang@uniontech.com>
 *
 * Maintainer: liujianqiang <liujianqiang@uniontech.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <iostream>

#include <gtest/gtest.h>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QProcess>

namespace linglong::squashfs {
int MountFile(const std::string &path, unsigned long offset, const std::string &mount_point);
}

namespace linglong::elf {
long GetELFSize(const std::string &path);
}

TEST(DemoTest, demo01)
{
    QFileInfo fs("../../tests/data/org.deepin.demo_1.2.1_x86_64.uab");
    EXPECT_EQ(fs.exists(), true);
    auto size = linglong::elf::GetELFSize("../../tests/data/org.deepin.demo_1.2.1_x86_64.uab");
    std::cout << size << std::endl;
    EXPECT_EQ(size, 2326904);
    std::string mountPoint("/tmp/uab_test");
    QDir("/tmp/uab_test").mkpath(".");
    //fuse mount uab
    auto result = linglong::squashfs::MountFile("../../tests/data/org.deepin.demo_1.2.1_x86_64.uab", size, mountPoint);
    //fuse umount uab
    QProcess process;
    process.setProgram("fusermount3");
    process.setStandardOutputFile("/dev/null");
    process.setStandardErrorFile("/dev/null");
    process.setArguments({"-u",QString("/tmp/uab_test")});
    process.startDetached();
    EXPECT_EQ(result,0);
}
