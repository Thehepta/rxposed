## 为什么android 10 和11 不一样

因为Android11 开始有了包可见性的问题，导致的原因就是，假如你要访问一个外部的app应用，需要在配置文件中写明。而rxposed 加载外部模块的时机是在应用启动的时候，通过contentprovider访问rxposed管理应用，获取配置，然后根据配置加载外部应用，但是android 11以后，
是无法做到的，除非你更改包可见性，或者在配置文件中提前写上rxposed管理应用的包名。
对于这种问题的解决，最终参考了lspode的处理方式，修改了发起contentprovider访问rxposed管理应用的时机。

## 绕过包可见性的问题

android 的应用区分，是基于用户的。但是每个应用都是基于zygote，从zygote孵化，所以，应用进程启动的最初时机，是root权限，然后修改为android app应用。所以，我在android 应用设置uid之前，以root的权限发起contentprovider访问rxposed管理应用。

## 识别启动的应用

我们向rxposed管理应用 获取配置，是需要告诉他我们是谁的，那我们如何知道zygote 孵化出来的进程是谁那，所以我们要提前获取这个uid。
hook zygote的nativeSpecializeAppProcess （参考lsposed）这个函数，这个函数会在fork之后，设置uid之前，并且这个函数能获取uid作为参数，我们在这个函数发起contentprovider。

## 没有 application context 发起contentprovider

在这个位置发起contentprovider 由于时机过早，并没有application context，所以我们需要在没有context 的情况下发起请求。
这个地方参考 android 的content 这个命令，是如何在没有 context 的情况下发起请求的

## selinux 策略

zygote 作为root用户发起 contentprovider 可能有限制，所以可能需要修改selinux 策略。


## 查找nativeSpecializeAppProcess 地址

先通过反射获取nativeSpecializeAppProcess 地址，然后通过jni 查找这个函数的native 地址。
