# 简介

本项目是[idea4good/GuiLite](https://github.com/idea4good/GuiLite)的C语言实现版本，基于**2024-06-20**节点的版本（提交ID：[e9c4b57](https://github.com/idea4good/GuiLite/commit/e9c4b573ce3325c05d3a9065f16363d9348f6e64)）。



# 需求说明

作为芯片从业人员，国产芯片普遍资源有限（ROM和RAM比较少-都是成本，CPU速度比较高-100MHz），需要在512KB ROM，20KB左右RAM资源上实现手环之类的GUI操作（要有触摸），CPU可以跑96MHz。

第一次搞嵌入式GUI，问了一圈朋友，LVGL直接放弃（太绚丽了，个人觉得也不可能跑得动，而且代码应该也比较复杂，魔改会比较困难），有人建议`手撸`，那要死人了。最终有朋友推荐了[idea4good/GuiLite](https://github.com/idea4good/GuiLite)，看了下介绍，GUI简单直接，所需的ROM和RAM也比较少，效果图里面也有很多所需的场景，持续有更新，[ Apache-2.0 license](https://github.com/idea4good/GuiLite#Apache-2.0-1-ov-file)，比较符合我的需求。

尝试放到芯片上跑，受限于芯片资源和使用场景（基本没Heap，开发环境基本都是C）。由于要支持C++环境，带进来一堆系统库，作为搞嵌入式裸机环境的程序员，完全无法接受各种调用系统库操作，此外加上C++环境后，code size一下子也膨胀了很多。

关键是调试麻烦！！！调试麻烦！！！调试麻烦！！！看反汇编的时候太痛苦了（因为我是C语言小白）。

没什么说的，一共也就几千行代码，手撸成C语言（没现成的，问下来有人干了这个事情，但是没开源）。



# 所需资源分析

资源分GUI代码和控件所需的资源以及Framebuffer。Framebuffer是固定的，各个GUI有专门的优化处理。

## GUI代码和控件所需资源

对于嵌入式环境而言，code size和ram size至关重要。所以以典型的cm0嵌入式开发环境为例，对code size和ram size进行分析。编译出来的大小见下表。

可以看到不同的例程所需资源差异巨大，这个涉及到GUI用到了哪些控件，字库，图片等。

注意：由于不同lib库对于printf、malloc等接口影响较大，库这些接口都不实现。资源紧张的场景可以按需简易实现。

注意：在`porting\cm0\GCC`路径下运行`porting\cm0\GCC\utils_analysis_elf_size.py`脚本，可以打印下面的表格保存在`porting\cm0\GCC\output\README.md`。

| app                | Code(Bytes)         | RAM(Bytes)          |
| ------------------ | ------------------- | ------------------- |
|             Hello3D|                19300|                 1124|
|        Hello3Ddonut|                23096|                 5824|
|         Hello3Dwave|                22848|                 5240|
|      HelloAnimation|              1939652|                  680|
|         HelloCircle|                18860|                29636|
|       HelloExSimple|                16332|                  644|
|           HelloFont|              1646292|                  584|
|         HelloKeypad|                11596|                 1068|
|         HelloLayers|                14440|                  328|
|          HelloMario|               387324|                  392|
|       HelloMolecule|                 9764|                 1736|
|           HelloNets|                16924|                26128|
|       HelloParticle|                11924|                 2304|
|       HelloPendulum|                31144|                  368|
|         HelloScroll|              1210076|                 2160|
|          HelloSlide|              3941864|                 1512|
|           HelloStar|                 7364|                 3848|
|          HelloTimer|                87820|                  712|
|    HelloTransparent|               711596|                 3128|
|           HelloWave|                13924|                 7652|
|        HelloWidgets|                27236|                 6696|
|        HelloWindows|              2461744|                 1084|



以`HelloExSimple`为例，实现1个button+1个label只需要`16332`的code size和`644`的ram size字节，这里面字体占用的code size接近1半。

![image-20240627205351143](https://markdown-1306347444.cos.ap-shanghai.myqcloud.com/img/image-20240627205351143.png)



## Framebuffer

其实GuiLite所需的Code Size和Ram Size都比较小，但是其对Framebuffer需要很大。单个页面，不支持动效之类的场景还好，如果需要支持Scroll、Dialog、Slice等功能，就需要很多Framebuffer了。这块优化并不好，如果没有复杂动效的话，不需要提供Framebuffer，所需的资源就上面列表的值。

不然就是要n个Framebuffer了，一个240*320的RGB565的屏幕，需要`153,600Bytes=150KBytes`，这很可怕了，更别说部分场景还需要乘以n。








# 代码架构

没什么东西，也就是源代码，例程和编译配置。

- **example**：各种GUI例程，基本是照搬GuiLite的来。
- **porting**：程序的主入口，根据平台不同，有一些不同实现。
- **src**：GuiLite代码实现部分，结构参考GuiLite处理。
- **build.mk和Makefile**：Makefile文件。

```
GuiLite_C
 ├── build.mk
 ├── Makefile
 ├── example
 │   ...
 ├── porting
 │   ...
 └── src
     ...
```





# 使用说明

## 环境搭建

目前暂时只支持Windows编译，最终生成exe，可以直接在PC上跑。

目前需要安装如下环境：

- GCC环境，笔者用的msys64+mingw，用于编译生成exe，参考这个文章安装即可。[Win7下msys64安装mingw工具链 - Milton - 博客园 (cnblogs.com)](https://www.cnblogs.com/milton/p/11808091.html)。



## 编译说明

本项目都是由makefile组织编译的，编译整个项目只需要执行`make all`即可，调用`make run`可以运行。

根据具体需要可以调整一些参数，目前Makefile支持如下参数配置。

- **APP**：选择example中的例程，默认选择为`Hello3D`。
- **PORT**：选择porting中的环境，也就是当前平台，默认选择为`windows`，cm0只用于评估code size和ram size需要专门编译。

也就是可以通过如下指令来编译工程：

```shell
make all APP=Hello3D
```

执行`make run`后，在windows环境就会弹出一个窗口，演示GUI效果了。

![image-20240627173328170](https://markdown-1306347444.cos.ap-shanghai.myqcloud.com/img/image-20240627173328170.png)

















# 改动说明



## 编译运行

为了方便维护，将[idea4good/GuiLite](https://github.com/idea4good/GuiLite)和[idea4good/GuiLiteSamples](https://github.com/idea4good/GuiLiteSamples)两个合在一起。

这样修改源码和Sample可以同时进行。

windows环境也不再使用Visual Studio编译，直接调用系统API进行窗口绘制。





## 代码结构

保留原本的代码结构不变，不过将源码和例程合并在一起。

原本只有一个`.h`文件，变成一个`.h`和一个`.c`。

`GuiLite.h`原本是所有代码都放在这里，虽然代码看起来清爽了，但是可维护性和可阅读性比较差。这里只是将所有控件都放进来，应用层只需要引用这个头文件即可。

各个平台实现目前也不做过多考虑了，全部命令行操作。只关心code size和基本使用。目前只实现了windows和cm0的版本。

代码组织方式用`Makefile`，简单直接。





## C++改动说明

还好作者用的C++特性较少，改起来比较轻松，下面对改动进行说明。

### 类

所有类用struct来实现，为方便使用，都用typedef声明下。

```c
// C++ class impl
class AAA
{
}



// C struct impl
typedef struct AAA AAA;
struct AAA
{

}
```



### 类-成员

成员有结构体成员来实现，有一些类里面static处理，通过外部定义变量来处理，写代码的人自己控制操作空间。

```c
// C++ class impl
class AAA
{
    int m_aaa;
    static int m_bbb;
}



// C struct impl
typedef struct AAA AAA;
struct AAA
{
	int m_aaa;
}

static int AAA_m_bbb;
```





### 类-构造函数、方法

public、protect和private就不区分了，软件自己控制操作空间。直接在方法名前面加入`class_`来区分。第一个传参调整为类对象的指针，名称为self。

对于构造函数（析构也一样，不过本项目没有），定义函数`class_init`来实现，**因为编译器不会帮你调用，所以需要自己手动调用**。

```c
// C++ class impl
class AAA
{
    AAA(aaa) : m_aaa(aaa) {}
public:
    void func_1(void)
    {
        
    }
    
protect:
    void func_2(void)
    {
        
    }
    
private:
    void func_3(void)
    {
        
    }
    
    int m_aaa;
}



// C struct impl
typedef struct AAA AAA;
struct AAA
{
}
void AAA_func_1(AAA *self)
{

}

void AAA_func_2(AAA *self)
{

}

void AAA_func_3(AAA *self)
{
    
}
void AAA_init(AAA *self, int aaa)
{
	self->m_aaa = aaa;
}
```





### 类-同名函数(重载)

函数有多个名称一样的函数，传参不同，或者部分参数有默认值。对于这种情况最全的参数列表为默认名称。最简单为`func_simple`，有其他参数，使用`func_with_xxx`。

对于构造函数（析构也一样，不过本项目没有），定义函数`class_init`来实现，**因为编译器不会帮你调用，所以需要自己手动调用**。

```c
// C++ class impl
class AAA
{
    void func_1(int a=10)
    {
        
    }
}



// C struct impl
typedef struct AAA AAA;
struct AAA
{
}
void AAA_func_1(AAA *self, int a)
{

}

void AAA_func_1_simple(AAA *self)
{
	AAA_func_1(self, 10);
}

```



类-默认传参

最全的参数列表为默认名称。

最简单为func_simple

有其他参数，使用func_with_xxx









### 类-虚函数

这里最麻烦的就是**虚函数**了处理了，因为涉及到类继承，函数覆盖等处理。简单的处理就是一个虚函数一个函数指针，但是这样当类里面的虚函数比较多时，所以RAM就很多了。

所以这里用一个麻烦的处理，用函数列表来做，所有集成类的构造函数（也就是`class_init`）需要重新赋值虚函数表。

为区分，虚函数的函数命令需要在函数前面加入`class_vir_`。还要声明一个结构体为`struct class_vir_api`来定义虚函数表，同时类的成员加入`const class_vir_api *m_api`来存储函数列表指针。

```c
// C++ class impl
class AAA
{
    AAA(aaa) : m_aaa(aaa) {}

    virtual void func_virtual_1(void)
    {
        
    }
    
    int m_aaa;
}



// C virtual api impl
typedef struct AAA_vir_api AAA_vir_api;
struct AAA_vir_api
{
    void (*func_virtual_1)(AAA *self);
}

// C struct impl
typedef struct AAA AAA;
struct AAA
{
    const AAA_vir_api* m_api; // virtual api
}
void AAA_vir_func_virtual_1(AAA *self)
{

}

static const AAA_vir_api AAA_vir_api_table = {
        AAA_vir_func_virtual_1,
};

void AAA_init(AAA *self, int aaa)
{
	self->m_aaa = aaa;
    
    self->m_api = &AAA_vir_api_table; // set virtual api.
}
```







### 类-继承

暂时只考虑只继承一个父类，不考虑继承多个父类的处理。

子类需要定义第一个成员为父类`base`，构造函数需要先调用父类的构造函数，有虚函数重写的，需要重新定义虚函数表，并覆盖。

所有涉及虚函数，使用基类作为函数self传参。虚函数实现的api接口，传参为基类的api，需要转一下`BBB *b= (BBB*)self;`。

```c
// C++ class impl
class AAA
{
    virtual void func_virtual_1(void)
    {
        
    }
}

class BBB : public AAA
{
    virtual void func_virtual_1(void)
    {
        
    }
}


// C virtual api impl
typedef struct AAA_vir_api AAA_vir_api;
struct AAA_vir_api
{
    void (*func_virtual_1)(AAA *self);
}

// C struct impl
typedef struct AAA AAA;
struct AAA
{
    const AAA_vir_api* m_api; // virtual api
}
void AAA_vir_func_virtual_1(AAA *self)
{

}

static const AAA_vir_api AAA_vir_api_table = {
        AAA_vir_func_virtual_1,
};

void AAA_init(AAA *self)
{
    self->m_api = &AAA_vir_api_table; // set virtual api.
}




// C struct impl
typedef struct BBB BBB;
struct BBB
{
    AAA base; // base class
}
void BBB_vir_func_virtual_1(AAA *self)
{
	BBB *bbb = (BBB *)self;
}

static const AAA_vir_api BBB_vir_api_table = {
        BBB_vir_func_virtual_1,
};

void BBB_init(BBB *self)
{
	AAA_init(&self->base); // call base init func
    
    self->base.m_api = &BBB_vir_api_table; // set virtual api.
}
```

















# 下一步计划

## Framebuffer全面移除

对于嵌入式GUI而言，GUI自身所需的Code Size其实完全比不上字体、图片等资源所需的空间（虽然这些可以放在外部存储中）。Ram其实是很关键的，而大头更多是Framebuffer。

目前Surface的设计，多个Surface就需要多个Framebuffer的设计，对于嵌入式而言还是太不友好了。

后续要么只保留一个Framebuffer，要么全部移除。



## 资源外部加载接口实现

目前想JPG和MP4这些，作者希望大家自己写控件实现，后续考虑提供专门的接口函数，方便MCU使用。当然现在这样也完全没问题，只是跨平台写代码麻烦点。





## 注释完善

用AI加一些注释，不然对于新人太不友好了。











