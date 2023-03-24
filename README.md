# rxposed

#### 介绍
rxposed,是一个Android平台全局注入框架，通过ptrace zygote进程的方式，可以为我们提供ndk接口和java接口，并且可以通过管理程序，进行行为管控。

[Manager](Manager/README.md)  

RxposedTool   ptrace  注入工具

nativeloader  rxposed 注入到zygote中进行hook的代码，以及在hook的app启动的时候提供ndk接口和java接口

loadxposed    这个属于一个rxposed的模块，主要用于加载和管理xposed模块


`[技术文档](/document/DOC.md)`|[技术文档](/document/DOC.md)

|项目|描述|
|----|-----|
|`Manager`|rxposed 行为管控app|
|`_斜体2_`| _斜体2_|
|`**粗体1**`|**粗体1**|
|`__粗体2__`|__粗体2__|
|`这是一个 ~~删除线~~`|这是一个 ~~删除线~~|
|`***斜粗体1***`|***斜粗体1***|
|`___斜粗体2___`|___斜粗体2___|
|`***~~斜粗体删除线1~~***`|***~~斜粗体删除线1~~***|
|`~~***斜粗体删除线2***~~`|~~***斜粗体删除线2***~~|

