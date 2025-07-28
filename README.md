# Code Manager

需要C++20

由于使用了额外的库，请在编译时加入一下参数：

```bash
-std=c++20 -lstdc++fs -municode -Wall
```

或者使用已经编译好的文件：CM.exe

请在系统环境变量`path`中添加这个文件所在的文件夹路径，这样就可以正常使用了。
如果你的Windows版本较低，应该在`path`变量的末尾加上`;<这个文件夹的路径>`

现有的命令：

* add -  增加一道题目
* list - 列出题目列表
* del - 删除题目
* search - 搜索题目
* start - 打开题目
* help  - 帮助
* about - 关于

注：这个程序会把文件复制，所以原文件被删除也不会有影响。

关注我 (LEGENDrd)：[Bilibili](https://space.bilibili.com/3546767803026063?spm_id_from=333.1007.0.0)，[CSDN博客](https://blog.csdn.net/R_D_LEGEND?type=blog)

## language_map.vk

用于存储文件拓展名与语言的关系。
格式：
文件拓展名 语言

例：

```vk
.cpp c++
.py python
```

## name_map.vk

用于存储文件名与其拓展名之间的关系
格式：
文件名（无拓展名） 文件拓展名

例：

```vk
hello .cpp
test .py
``
