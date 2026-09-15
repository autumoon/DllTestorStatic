# DllTestorStatic

一个基于 **Visual Studio 2015 + MFC** 的对话框程序框架，用于处理多个文件与目录（如配置文件读写、目录/文件遍历等场景）。

## 结构

```
DllTestorStatic/
├── DllTestorStatic.sln       # VS2015 解决方案
└── DllTestor/
    ├── DllTestor.vcxproj     # 工程文件（v140 工具集）
    ├── DllTestorDlg.cpp/.h   # 主对话框
    ├── MyEdit.cpp/.h         # 自定义编辑框控件
    ├── src/                  # 通用工具（配置、定义等）
    └── res/                  # 图标/位图资源
```

## 编译

1. 用 VS2015 打开 `DllTestorStatic.sln`
2. 选择目标平台与配置（x86/x64，Debug/Release）
3. 生成解决方案