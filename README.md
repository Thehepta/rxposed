# rxposed

### 介绍
rxposed,是一个Android平台全局注入框架，通过ptrace zygote进程的方式，对应用app进行注入。


### 平台支持
目前只支持android 13 ,后续可能会对android 11 和12 兼容，但是不会兼容11以下，主要是因为andrid11以上开始有包可见性问题，这样就可以隐藏安装在手机上的管理app。

### 工具定位
一个注入版的lsposed，同时又是一个本地版本的frida，相比于lsposed，可能模块较少，生态不够完整，没有线上库。但是会提供一些足够强大的模块源码，来辅助你完成一个强大，可以像frida提供的功能一样的模块。



### 优点

#### 强对抗

+ 内存痕迹对抗：没有任何内存痕迹，不存在hook了某些内存，或者改变了内存布局，数据被检测到的问题
+ 特征检测对抗：在工程构建的时候，就考虑到了以后可能存在的特征对抗问题，比如符号，字符串，so名字，所以，工程小，容易修改，只有一个so文件。
+ 模块隐藏：不仅本身提供了反检测功能，所有的模块也提供了隐藏加载功能（https://github.com/Thehepta/HideApk）
+ 依赖检测：既没有外部so依赖，工具本身也不依赖于magisk或者kernerlsu的模块，只需要root权限即可，不会因为别的工具被检测而无法使用

#### 稳定不黏牙
本身是通过ptrace zygote的方式注入到应用中，如果不使用，可以通过软重启（杀死zygote进程让他自动重启）的方式关闭注入功能。代码量小，内存改动极小，所有的改动都会在app运行时恢复，已经使用了一年多了，测试过各大厂商，均未出现崩溃情况。

### 缺点

+ 模块生态：不管是生态，规模，线上模块库，模块管理都无法与lsposed媲美，如果你想要一个像lsposed那样具有成熟app生态的改机工具，那你要失望了
+ 模块开发：与frida那种脚本式开发，提供了大量的api，开发比较简单的方式不同，rxposed的开发与lsposed类似，你的逆向开发能力决定了模块的上限，当然也需要很多正向开发知识，我会提供很多demo模块，实现各种各样的功能，来供你参考，组装，修改还是要你自己来




### 可能需要者

广大安全开发研究者，它的主要的目是用来无感知注入，有时候这可能需要修改特征，自行编译完成对抗。



[核心技术点具体实现说明文档](/document/android10.md)

| 项目         | 描述                                                                 | 使用说明                                           |
|------------|--------------------------------------------------------------------|------------------------------------------------|
| `Manager`  | rxposed 行为管控app 以及要注入的so                                  | [Manager/README.md](/document/instructions.md) |
| `Tool`     | 进行ptrace的注入工具，同时添加了将注入的so的内存段进行隐藏的功能       |                                                |
| `loadxposed` | 一个rxposed的模块，用于注入加载xposed以及管理xposed模块（目前有些问题，正在处理hook框架的问题，高版本不可用）（感觉没啥用，后续可能删除了） | [loadxposed/README.md](/loadxposed/README.md)  |



### 模块地址
https://github.com/Thehepta/Rxmodule


### 致谢
https://github.com/sanfengAndroid/fake-linker

https://github.com/LSPosed/LSPosed

https://github.com/frida/frida



### License

LSPosed is licensed under the **GNU General Public License v3 (GPL-3)** (http://www.gnu.org/copyleft/gpl.html).