# DllTestorStatic

一个基于 **Visual Studio 2015 + MFC** 的对话框程序框架，用于处理多个文件与目录
（配置文件读写、目录/文件遍历、批量复制等场景）。很多小工具都可以基于它开发。

## 结构

```
DllTestorStatic/
├── DllTestorStatic.sln        # VS2015 解决方案
└── DllTestor/
    ├── DllTestor.vcxproj      # 工程文件（v140 工具集）
    ├── DllTestorDlg.cpp/.h    # 主对话框
    ├── MyEdit.cpp/.h          # 支持拖拽的自定义编辑框
    ├── BuildConfig.h          # 编译期开关（命令行、背景、耗时统计等）
    ├── src/                   # 通用工具（配置、定义、日志、耗时、目录遍历等）
    └── res/                   # 图标/位图资源
```

## 编译

1. 用 VS2015 打开 `DllTestorStatic.sln`
2. 选择目标平台与配置（x86/x64，Debug/Release）
3. 生成解决方案

## 处理模式

处理模式由 `CDllTestorDlg::m_eProcessMode` 控制，取值：

- `ProcessMode::File` 只处理文件（默认）
- `ProcessMode::Dir`  只处理目录
- `ProcessMode::Both` 文件和目录都处理

当前默认值为 `File`，切换方式是在 `CDllTestorDlg` 的构造函数初始化列表中修改：

```cpp
, m_eProcessMode(ProcessMode::File)   // 改成 Dir 或 Both
```

切换后，界面按钮文字、列表标题会自动对应“文件/目录/项目”。

## 拖拽支持

- 主对话框支持将文件或目录拖入添加到列表。
- 目标目录编辑框支持拖拽，且只允许单个目录。
- 拖拽会根据当前处理模式过滤类型。

## 配置文件

配置文件与 exe 同目录、同名，扩展名为 `.ini`。
例如 `DllTestorStatic.exe` 对应 `DllTestorStatic.ini`。

段名固定为 `[INI_PRESUFFIX]`，字段：

| 字段 | 含义 |
|---|---|
| `INI_REMPATH` | 是否在下次启动时恢复上次的待处理列表 |
| `INI_DST_DIRS` | 上次选择的目标目录（多个用 `\|` 分隔） |
| `INI_ALL_ITEMS` | 上次的待处理项目列表（多个用 `\|` 分隔） |
| `INI_FILE_SUFFIXS` | 文件后缀过滤（多个用 `\|` 分隔，`*` 表示全部） |
| `INI_WIN_WIDTH` | 上次窗口宽度 |
| `INI_WIN_HEIGHT` | 上次窗口高度 |

如果 ini 不存在，首次启动会自动写入一份默认配置。
读取时对空值做了过滤，`INI_FILE_SUFFIXS` 为空时会回退为 `*`。

## 背景图

如果 exe 同目录存在同名 `.bmp` 文件（例如 `DllTestorStatic.bmp`），会作为对话框背景。
否则使用资源中的 `IDB_BITMAP1`。
通过 `BuildConfig.h` 中的 `DLG_BACKGROUND` 宏可以整体关闭背景绘制。

## 编译期开关（BuildConfig.h）

- `CMD_INPUT`  支持命令行参数传入待处理项
- `CMD_OUTPUT` 附加控制台输出
- `DLG_BACKGROUND` 是否绘制对话框背景
- `DLG_ELAPSED_TIME` 处理结束时是否显示耗时

## 交互约定

- 主处理入口在 `CDllTestorDlg::OnBnClickedOk`，需要为具体工具替换其中“主程序”段。
- 处理完成后会弹窗提示“成功 x 个，失败 y 个”（开启 `DLG_ELAPSED_TIME` 时附耗时）。
- 帮助按钮（标题栏问号）会把开发者邮箱复制到剪贴板。