# Linglong Loader

Linglong Loader

## Getting started

```bash
sudo apt install squashfuse fuse -y
mkdir build && cd build 
cmake .. && make -j1

# 建议 
strip bin/uloader

# 生成 loader

# python3 scripts/create_uap.py  #构造 一个启动shell

# 制作 data.sqsfs
mkdir work && cp loader
mksquashfs work/loader work/* data.sqsfs -comp xz
cat bin/uloader data.sqsfs > appid_version_arch.uap
chmod -x appid_version_arch.uap
```
## Default Loader

cat loader
```bash
#!/bin/bash
echo "This Default Loader"
echo $@
./demo-test
```
第一个参数默认是挂载的路径，后续的参数是启动是携带的参数
## License
GPLv3
