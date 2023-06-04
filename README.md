# rxposed

#### 介绍
rxposed,是一个Android平台全局注入框架，通过ptrace zygote进程的方式，可以为我们提供ndk接口和java接口，并且可以通过管理程序，进行行为管控。
进行了更新，可以目前可以兼容到android 13。从原理上来讲，他是一个注入版的lsposed。


[核心技术点具体实现说明文档](/document/android10.md)

| 项目         | 描述                                                                 | 链接      |
|------------|--------------------------------------------------------------------|---------|
| `Manager`  | rxposed 行为管控app                                                    | [Manager/README.md](/Manager/README.md) |
| `Tool`     | 进行ptrace的注入工具                                                      |  |
| `nativeloader` | rxposed 核心代码，在应用启动的时候进行hook，加载模块并执行                                | [nativeloader/README.md](/nativeloader/README.md) |
| `loadxposed` | 一个rxposed的模块，用于注入加载xposed以及管理xposed模块（目前有些问题，正在处理hook框架的问题，高版本不可用） | [loadxposed/README.md](/loadxposed/README.md) |


## 致谢
https://github.com/sanfengAndroid/fake-linker

https://github.com/LSPosed/LSPosed