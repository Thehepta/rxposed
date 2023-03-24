# rxposed

#### 介绍
rxposed,是一个Android平台全局注入框架，通过ptrace zygote进程的方式，可以为我们提供ndk接口和java接口，并且可以通过管理程序，进行行为管控。


[核心技术点具体实现说明文档](/document/DOC.md)

| 项目         | 描述                                    | 链接      |
|------------|---------------------------------------|---------|
| `Manager`  | rxposed 行为管控app                       | 行为管控app |
| `Tool`     | 进行ptrace的注入工具                         | 行为管控app |
| `nativeloader` | rxposed 核心代码，在应用启动的时候进行hook，加载模块并执行   | 行为管控app |
| `loadxposed` | 一个rxposed的模块，用于注入加载xposed以及管理xposed模块 | 行为管控app |

