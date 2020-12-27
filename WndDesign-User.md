# WndDesign使用说明

WndDesign是一款

纯c++编写，面向对象设计，丰富的样式，十分灵活

关于WndDesign的历史背景，见WndDesign-Old代码库。



vs创建项目，项目配置（c++17，窗口程序，Unicode字符集，），

下载WndDesignCore.dll，WndDesignCore.lib，WndDesign.lib，WndDesignHelper.lib（注意debug和release版本）。

将头文件另存添加到依赖项。

编写Init及Final函数。





样例程序（简单）







代码接口：

窗口样式

窗口操作接口





样例程序（简单）

样例程序（复杂）





建议的设计模式：

窗口类的单独管理

头文件引用关系

动画在窗口类内单独建立一个列表，在cpp文件中实现。动画类单独建立一个文件夹



先做设计，比如需要哪些窗口，有什么动效，再尝试使用，