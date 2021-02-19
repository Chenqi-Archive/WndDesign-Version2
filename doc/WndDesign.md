# WndDesign设计文档



[TOC]



## c++代码说明

#### 编译环境

使用Visual Studio 2019编译，项目配置如下：

* Output Directory: `$(SolutionDir)build\$(platform)\$(Configuration)\`
* Intermediate Directory: `$(SolutionDir)temp\$(ProjectName)\$(Platform)\$(Configuration)\`

* C++ Language Standard: `C++17`

* Working Directory: `$(OutputPath)`

#### 代码风格

* 整个项目使用WndDesign命名空间。
* 命名空间使用`BEGIN_NAMESPACE(name)`和`END_NAMESPACE(name)`包围。
* 指针：
  * 不出现任何裸指针形式`int* p`。（部分系统接口除外）
  * 作为单一引用的指针使用引用类型`int& p`。
  * 需要变更引用，且不能为null，使用`std::reference_wrapper`。
  * 可以null初始化，使用`ref_ptr`标记的裸指针（`template<class T> using ref_ptr = T*`）。
  * 维护分配对象的指针使用`std::unique_ptr`，`std::shared_ptr`等智能指针。
* 命名习惯：
  * 内部使用的名称（使用者不必关心的名称）前加下划线。
  * 类成员使用小写，前加下划线，便于在类内部的函数中区分，单词间加下划线（_parent_size）（m_parent显得有些麻烦），结构体成员可以不加下划线前缀。
  * 类成员函数的单词首字母大写。layer.SetSize()
  * STL的一些包装函数使用小写加下划线命名，与stl命名保持一致。如`swap_list_iter()`。





## 项目代码结构

### 架构示意图

【图（架构示意图）】





### 各部分的简要说明

------

以下由WndDesignCore实现（动态库）

#### 基础工具（common）

##### 基础宏定义

##### 常用类型定义

##### 常用的c++帮助组件

* Uncopyable类：禁止对象的复制，通常用于某些携带独一无二资源的类。

* reversion_wrapper配接器：用于range-based-for中的反向遍历。



#### 系统（system）

##### wic

用于图片文件

##### dwrite

文本样式

##### d2d

基本图案

图层

##### d3d

d2d底层接口，3d绘制尚未实现

##### win32

为d3d绘图提供窗口

接受消息

其它Win32API



#### 几何（geometry）

大小，位置，向量，矩形区域的基本定义



#### 图形对象（figure）

每个图形对象包装了D2DDeviceContext的接口函数，提供了统一的虚函数接口，可以在一个RenderTarget上绘图。

包括：直线、矩形、圆角矩形、椭圆

图片

文本框

几何图案

layer



#### 图层（layer）

【附图，架构以及绘图对象的传递】



Layer对象及合成样式



##### FigureQueue

一次绘制操作所绘制的图形对象的组合，

并附有这个组合图形对象的位置和变换属性。

由Target进行解析。

##### Target Tile

每个Tile维护了一个D2D位图，作为图形对象的最终绘图目标（RenderTarget）。

##### Layer

绘图对象的直接绘图目标。

layer内部维护了一些Target作为绘图的最终目标。

layer也可以将自己的某个区域作为绘图对象。

layer的属性：整个区域、当前缓存区域



#### 消息（message）

鼠标消息（手写笔）

键盘消息（输入法）



#### 窗口基础（wnd）

##### WndBase

子窗口、绘图、消息处理



##### WndObject



##### DesktopBase



##### DesktopWndFrame



##### DesktopObjectImpl





------

以下由WndDesign实现（静态库）

#### 窗口样式（style）





#### 窗口对象（wnd）

WndObject

Wnd



OverlapLayout

ListLayout

FlowLayout

SplitLayout

TableLayout



FinalWnd

TextBox

ImageBox

Button

EditBox





## 设计要点

### 窗口层级关系

#### 作为父窗口

##### 子窗口数据结构

子窗口使用链表维护，链表元素构造析构时自动调用添加或删除函数。



##### 子窗口操作

###### 添加子窗口



###### 移除子窗口



###### 子窗口区域变化



###### 子窗口合成样式变化



##### 消息分发



#### layer中间层

每个窗口维护一个可以容纳多个窗口的底层layer，以及多个上层layer。



##### layer基类





##### 多窗口layer（作为底层layer）





##### 单窗口layer（作为上层layer）





#### 作为子窗口

##### 父窗口数据结构

每个窗口只能有一个父窗口，如果重置父窗口时，父窗口已经存在，需要先解除当前的父窗口。当窗口析构时，自动解除当前父窗口。



##### 样式结构

##### 合成



#### 桌面窗口

##### 桌面layer

##### 桌面消息分发



### 窗口样式

- 位置和大小：用于计算layer窗口大小。与父窗口相关。

- 合成属性：用于确定layer是否创建，以及合成的样式。

- 绘制属性：用于确定绘制的图案，与自己有关。



#### 几何区域样式

##### 大小

##### 位置

##### 旋转角度



#### 合成样式

##### 透明度

##### 合成效果



#### 绘图样式

##### 边框

##### 背景



### 图案



