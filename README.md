# rxposed

#### 介绍
rxposed,是一个Android平台全局注入框架，通过ptrace zygote进程的方式，可以为我们提供ndk接口和java接口，并且可以通过管理程序，进行行为管控。
目前只支持android 13 ,后续可能会对android 11 和12 兼容，但是不会兼容11一下。
他类似一个注入版的lsposed，提供了更为自由的模块功能，以及隐蔽性，和便捷性。


[核心技术点具体实现说明文档](/document/android10.md)

| 项目         | 描述                                                                 | 使用说明                                           |
|------------|--------------------------------------------------------------------|------------------------------------------------|
| `Manager`  | rxposed 行为管控app 以及要注入的so                                  | [Manager/README.md](/document/instructions.md) |
| `Tool`     | 进行ptrace的注入工具，同时添加了将注入的so的内存段进行隐藏的功能       |                                                |
| `loadxposed` | 一个rxposed的模块，用于注入加载xposed以及管理xposed模块（目前有些问题，正在处理hook框架的问题，高版本不可用）（感觉没啥用，后续可能删除了） | [loadxposed/README.md](/loadxposed/README.md)  |



#### 模块
https://github.com/Thehepta/Rxmodule


## 致谢
https://github.com/sanfengAndroid/fake-linker

https://github.com/LSPosed/LSPosed