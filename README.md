# rxposed

#### 介绍
rxposed,是一个Android平台全局注入框架，通过ptrace zygote进程的方式，可以为我们提供ndk接口和java接口，并且可以通过管理程序，进行行为管控。

[Manager](./Manager/README.md)  rxposed 行为管控app

RxposedTool   ptrace  注入工具

nativeloader  rxposed 注入到zygote中进行hook的代码，以及在hook的app启动的时候提供ndk接口和java接口

loadxposed    这个属于一个rxposed的模块，主要用于加载和管理xposed模块


