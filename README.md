# Linglong Loader

Linglong Loader

## Build

```bash
sudo apt install -y libsquashfuse-dev  liblz4-dev liblzma-dev liblzo2-dev
mkdir build && cd build
cmake .. && make -j
```

## Test

```bash
## cd ${project_build_root}
strip bin/linglong-loader

rm demo.uab
rm data.sqsfs
rm data

mkdir data
echo '#!/bin/bash' > data/loader
echo 'echo This Default Loader' >> data/loader
echo "echo $@" >> data/loader
chmod +x data/loader

mksquashfs data/* data.sqsfs -comp xz

cat bin/linglong-loader data.sqsfs > demo.uab

chmod +x demo.uab

./demo.uab
```

# 代码覆盖率测试
add --coverage in CMakeLists.txt
```bash
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
```

use gcov、lcov to generate convert html report, make shure at the top level of the project
```bash
./code_coverage.sh
```

## Getting help

Any usage issues can ask for help via

- [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
- [IRC channel](https://webchat.freenode.net/?channels=deepin)
- [Forum](https://bbs.deepin.org)
- [WiKi](https://wiki.deepin.org/)

## Getting involved

We encourage you to report issues and contribute changes

- [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en)
  . (English)
- [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

## License

This project is licensed under [GPLv3]().

```

```
